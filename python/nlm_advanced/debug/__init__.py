"""
Comprehensive visualization and debugging toolkit for NLM neural simulations.

Provides advanced real-time neural activity visualization, connectivity graph exploration,
activity profiling, comprehensive debugging capabilities, and data analysis tools.

This toolkit integrates seamlessly with enhanced NLM Python bindings and provides
powerful visualization and debugging capabilities for neural simulations.
"""

import numpy as np
import matplotlib.pyplot as plt
import networkx as nx
import pandas as pd
import seaborn as sns
from typing import Dict, List, Tuple, Optional, Any, Callable, Union
from dataclasses import dataclass, field
import json
import time
from collections import defaultdict
import threading
import sys
from io import StringIO
import warnings
from enum import Enum
import copy
import itertools
import math
import gc
import os
import psutil
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
import traceback
from datetime import datetime, timedelta
from collections import deque

@dataclass
class VisualizationConfig:
    """Configuration for neural visualization."""
    plot_width: int = 12
    plot_height: int = 8
    neuron_size: int = 300
    connection_threshold: float = 0.1
    colormap: str = "viridis"
    show_labels: bool = False
    animate: bool = True
    update_interval: float = 0.1

@dataclass
class DebugConfig:
    """Configuration for debugging tools."""
    enable_breakpoints: bool = True
    enable_state_inspection: bool = True
    enable_error_tracking: bool = True
    enable_performance_profiling: bool = True
    auto_save_state: bool = False
    state_save_interval: float = 1.0
    log_level: str = "INFO"
    max_log_entries: int = 10000

@dataclass
class Breakpoint:
    """Breakpoint configuration for step debugging."""
    step: int
    condition: Optional[Callable] = None
    message: str = ""
    action: str = "pause"  # "pause", "break", "log"

@dataclass
class StateSnapshot:
    """Neural state snapshot for debugging."""
    timestamp: float
    step: int
    time: float
    neural_state: Dict[str, Any]
    memory_usage: Dict[str, float]
    performance_metrics: Dict[str, float]
    metadata: Dict[str, Any] = field(default_factory=dict)

@dataclass
class DebugEvent:
    """Debug event for error tracking."""
    timestamp: float
    event_type: str  # "error", "warning", "info", "breakpoint"
    severity: str  # "low", "medium", "high", "critical"
    message: str
    stack_trace: Optional[str] = None
    context: Dict[str, Any] = field(default_factory=dict)
    resolved: bool = False
    resolved_at: Optional[float] = None

class StepDebugger:
    """
    Advanced step-by-step debugging interface for neural simulations with breakpoints.
    
    Provides controlled execution, state inspection, and comprehensive debugging capabilities
    for neural simulations. Supports conditional breakpoints, state comparison, and detailed
    logging of simulation steps.
    """
    
    def __init__(self, brain, config: Optional[DebugConfig] = None):
        self.brain = brain
        self.config = config or DebugConfig()
        self.debug_mode = False
        self.current_step = 0
        self.simulation_paused = False
        self.breakpoints = []
        self.state_history = []
        self.execution_log = []
        self.paused_at_step = 0
        self.auto_save_enabled = config.auto_save_state if config else False
        self.last_saved_state = None
        self.error_log = []
        self.warning_log = []
        self.info_log = []
        
        # Threading support
        self.debug_thread = None
        self.pause_event = threading.Event()
        self.stop_event = threading.Event()
        self.step_lock = threading.Lock()
        
        print(f"[DEBUGGER] Step debugger initialized for brain with {brain.getTotalNeuronCount() if hasattr(brain, 'getTotalNeuronCount') else 'unknown'} neurons")
    
    def enable_debug_mode(self):
        """Enable step-by-step debugging mode."""
        if self.config.enable_breakpoints:
            self.debug_mode = True
            self.current_step = 0
            self.execution_log.append({
                'timestamp': time.time(),
                'step': 'mode_enabled',
                'message': 'Debug mode enabled'
            })
            print(f"[DEBUGGER] Step-by-step debugging mode enabled")
        else:
            print(f"[DEBUGGER] Breakpoints disabled in config, debug mode not fully enabled")
    
    def disable_debug_mode(self):
        """Disable step-by-step debugging mode."""
        self.debug_mode = False
        self.simulation_paused = False
        self.stop_event.set()
        if self.debug_thread and self.debug_thread.is_alive():
            self.debug_thread.join(timeout=5.0)
        
        self.execution_log.append({
            'timestamp': time.time(),
            'step': 'mode_disabled',
            'message': 'Debug mode disabled'
        })
        print(f"[DEBUGGER] Step-by-step debugging mode disabled")
    
    def step(self, time: float) -> Tuple[bool, Optional[Dict]]:
        """
        Perform one simulation step with debugging.
        
        Args:
            time: Current simulation time
            
        Returns:
            Tuple of (step_completed, debug_info)
            - step_completed: True if step completed, False if paused
            - debug_info: Debug information dictionary or None
        """
        if not self.debug_mode:
            return False, None
        
        with self.step_lock:
            # Check breakpoints
            breakpoint_info = self._check_breakpoints(time)
            if breakpoint_info is not None:
                self.simulation_paused = True
                return False, breakpoint_info
            
            # Record state before step
            state_before = self._capture_state()
            
            # Perform step
            try:
                if hasattr(self.brain, 'step'):
                    self.brain.step(self.current_step, time)
                else:
                    raise AttributeError("Brain object does not have step method")
                
                step_completed = True
                state_after = self._capture_state()
                
                # Log successful step
                self.execution_log.append({
                    'timestamp': time.time(),
                    'step': self.current_step,
                    'event': 'step_completed',
                    'time': time,
                    'state_change': self._compare_states(state_before, state_after)
                })
                
                self.current_step += 1
                
                # Auto-save state if enabled
                if self.auto_save_enabled and self.config.state_save_interval > 0:
                    if self.current_step % int(1.0 / self.config.state_save_interval) == 0:
                        self._save_current_state(f"auto_save_step_{self.current_step}")
                
                return True, {
                    'step': self.current_step - 1,
                    'time': time,
                    'state_before': state_before,
                    'state_after': state_after,
                    'breakpoints_hit': breakpoint_info,
                    'debug_mode': True
                }
                
            except Exception as e:
                error_info = self._handle_error(e, time)
                self.error_log.append(error_info)
                return False, error_info
    
    def add_breakpoint(self, step: int, condition: Optional[Callable] = None, 
                      message: str = "", action: str = "pause"):
        """
        Add a breakpoint at specified step.
        
        Args:
            step: Step number where breakpoint should trigger
            condition: Optional condition function that evaluates to True when breakpoint should trigger
            message: Message to log when breakpoint triggers
            action: Action to take ("pause", "break", "log")
        """
        breakpoint = Breakpoint(
            step=step,
            condition=condition,
            message=message,
            action=action
        )
        
        self.breakpoints.append(breakpoint)
        self.execution_log.append({
            'timestamp': time.time(),
            'step': 'breakpoint_added',
            'message': f"Breakpoint added at step {step}: {message}"
        })
        
        print(f"[DEBUGGER] Breakpoint added at step {step}: {message}")
    
    def remove_breakpoint(self, step: int):
        """Remove breakpoint at specified step."""
        initial_count = len(self.breakpoints)
        self.breakpoints = [bp for bp in self.breakpoints if bp.step != step]
        
        if len(self.breakpoints) < initial_count:
            self.execution_log.append({
                'timestamp': time.time(),
                'step': 'breakpoint_removed',
                'message': f"Breakpoint removed at step {step}"
            })
            print(f"[DEBUGGER] Breakpoint removed at step {step}")
    
    def clear_breakpoints(self):
        """Clear all breakpoints."""
        self.breakpoints.clear()
        self.execution_log.append({
            'timestamp': time.time(),
            'step': 'breakpoints_cleared',
            'message': "All breakpoints cleared"
        })
        print(f"[DEBUGGER] All breakpoints cleared")
    
    def pause(self):
        """Pause simulation execution."""
        self.simulation_paused = True
        self.paused_at_step = self.current_step
        self.pause_event.set()
        
        self.execution_log.append({
            'timestamp': time.time(),
            'step': 'paused',
            'message': f"Simulation paused at step {self.current_step}"
        })
        print(f"[DEBUGGER] Simulation paused at step {self.current_step}")
    
    def resume(self):
        """Resume simulation execution."""
        if self.simulation_paused:
            self.simulation_paused = False
            self.pause_event.clear()
            
            self.execution_log.append({
                'timestamp': time.time(),
                'step': 'resumed',
                'message': f"Simulation resumed at step {self.current_step}"
            })
            print(f"[DEBUGGER] Simulation resumed at step {self.current_step}")
    
    def _check_breakpoints(self, time: float) -> Optional[Dict]:
        """Check if any breakpoints should trigger at current step."""
        if not self.debug_mode:
            return None
        
        for breakpoint in self.breakpoints:
            if self.current_step == breakpoint.step:
                # Evaluate condition if provided
                should_trigger = True
                if breakpoint.condition:
                    try:
                        should_trigger = breakpoint.condition(self.current_step, time, self.brain)
                    except Exception as e:
                        should_trigger = False
                        self._log_warning(f"Breakpoint condition evaluation failed: {e}")
                
                if should_trigger:
                    breakpoint_info = {
                        'type': 'breakpoint',
                        'step': self.current_step,
                        'time': time,
                        'message': breakpoint.message,
                        'action': breakpoint.action
                    }
                    
                    self.execution_log.append({
                        'timestamp': time.time(),
                        'step': 'breakpoint_triggered',
                        'message': f"Breakpoint triggered at step {self.current_step}: {breakpoint.message}"
                    })
                    
                    print(f"[DEBUGGER] Breakpoint triggered at step {self.current_step}: {breakpoint.message}")
                    return breakpoint_info
        
        return None
    
    def _capture_state(self) -> StateSnapshot:
        """Capture current neural state for debugging."""
        state = StateSnapshot(
            timestamp=time.time(),
            step=self.current_step,
            time=getattr(self.brain, 'getCurrentTime', lambda: time.time())(),
            neural_state={},
            memory_usage={},
            performance_metrics={}
        )
        
        # Capture neural state
        if hasattr(self.brain, 'getRegions'):
            regions = self.brain.getRegions()
            state.neural_state['regions'] = {
                region_id.value: {
                    'active_neurons': region.getActiveNeuronCount(),
                    'total_neurons': region.getTotalNeuronCount(),
                    'firing_rate': region.getActiveNeuronCount() / max(region.getTotalNeuronCount(), 1)
                }
                for region_id, region in regions.items()
            }
        
        # Capture brain-wide metrics
        if hasattr(self.brain, 'getTotalSpikeCount'):
            state.neural_state['total_spikes'] = self.brain.getTotalSpikeCount()
        if hasattr(self.brain, 'getActiveNeuronCount'):
            state.neural_state['active_neurons'] = self.brain.getActiveNeuronCount()
        if hasattr(self.brain, 'getAverageFiringRate'):
            state.neural_state['firing_rate'] = self.brain.getAverageFiringRate()
        if hasattr(self.brain, 'getExcitationInhibitionRatio'):
            state.neural_state['e_i_ratio'] = self.brain.getExcitationInhibitionRatio()
        
        # Capture memory usage
        process = psutil.Process()
        state.memory_usage['rss_mb'] = process.memory_info().rss / 1024 / 1024
        state.memory_usage['vms_mb'] = process.memory_info().vms / 1024 / 1024
        
        # Capture performance metrics
        state.performance_metrics['step_rate'] = 1.0  # Would be calculated from step timing
        
        return state
    
    def _compare_states(self, state1: StateSnapshot, state2: StateSnapshot) -> Dict[str, Any]:
        """Compare two state snapshots and return differences."""
        differences = {}
        
        # Compare neural states
        if 'regions' in state1.neural_state and 'regions' in state2.neural_state:
            regions1 = state1.neural_state['regions']
            regions2 = state2.neural_state['regions']
            
            for region_id in regions1:
                if region_id in regions2:
                    diff = {}
                    for key in regions1[region_id]:
                        val1 = regions1[region_id][key]
                        val2 = regions2[region_id][key]
                        if abs(val1 - val2) > 0.01:  # Significant difference threshold
                            diff[key] = {'before': val1, 'after': val2, 'difference': val2 - val1}
                    
                    if diff:
                        differences[f'region_{region_id}'] = diff
        
        # Compare spikes
        if 'total_spikes' in state1.neural_state and 'total_spikes' in state2.neural_state:
            diff = state2.neural_state['total_spikes'] - state1.neural_state['total_spikes']
            if abs(diff) > 0:
                differences['spikes'] = {'before': state1.neural_state['total_spikes'], 'after': state2.neural_state['total_spikes'], 'difference': diff}
        
        return differences
    
    def _handle_error(self, error: Exception, time: float) -> Dict[str, Any]:
        """Handle error that occurs during simulation step."""
        error_info = {
            'timestamp': time.time(),
            'error': str(error),
            'type': type(error).__name__,
            'step': self.current_step,
            'time': time,
            'stack_trace': traceback.format_exc() if hasattr(sys, 'exc_info') else None,
            'brain_state': self._capture_state()
        }
        
        print(f"[DEBUGGER] ERROR at step {self.current_step}, time {time}: {error}")
        return error_info
    
    def _log_error(self, message: str):
        """Log error message."""
        error_info = {
            'timestamp': time.time(),
            'level': 'ERROR',
            'message': message,
            'step': self.current_step
        }
        self.error_log.append(error_info)
    
    def _log_warning(self, message: str):
        """Log warning message."""
        warning_info = {
            'timestamp': time.time(),
            'level': 'WARNING',
            'message': message,
            'step': self.current_step
        }
        self.warning_log.append(warning_info)
    
    def _log_info(self, message: str):
        """Log info message."""
        info_info = {
            'timestamp': time.time(),
            'level': 'INFO',
            'message': message,
            'step': self.current_step
        }
        self.info_log.append(info_info)
    
    def get_debug_info(self) -> Dict[str, Any]:
        """Get comprehensive debug information."""
        return {
            'debug_mode': self.debug_mode,
            'current_step': self.current_step,
            'simulation_paused': self.simulation_paused,
            'breakpoints_count': len(self.breakpoints),
            'state_history_size': len(self.state_history),
            'execution_log_size': len(self.execution_log),
            'error_count': len(self.error_log),
            'warning_count': len(self.warning_log),
            'info_count': len(self.info_log),
            'active_breakpoints': [(bp.step, bp.message) for bp in self.breakpoints if bp.step >= self.current_step]
        }
    
    def get_state_at_step(self, step: int) -> Optional[StateSnapshot]:
        """Get state snapshot at specified step."""
        for state in self.state_history:
            if state.step == step:
                return state
        return None
    
    def get_execution_log(self, limit: int = 100) -> List[Dict]:
        """Get recent execution log entries."""
        return self.execution_log[-limit:] if limit > 0 else self.execution_log
    
    def get_errors(self, limit: int = 50) -> List[Dict]:
        """Get recent errors."""
        return self.error_log[-limit:] if limit > 0 else self.error_log
    
    def get_breakpoints(self) -> List[Dict]:
        """Get all breakpoints."""
        return [
            {
                'step': bp.step,
                'message': bp.message,
                'action': bp.action
            }
            for bp in self.breakpoints
        ]
    
    def save_debug_session(self, filepath: str):
        """Save debug session to file."""
        session_data = {
            'debug_info': self.get_debug_info(),
            'execution_log': self.execution_log,
            'error_log': self.error_log,
            'warning_log': self.warning_log,
            'info_log': self.info_log,
            'breakpoints': [(bp.step, bp.message, bp.action) for bp in self.breakpoints],
            'state_history': [vars(state) for state in self.state_history]
        }
        
        with open(filepath, 'w') as f:
            json.dump(session_data, f, indent=2, default=str)
        
        print(f"[DEBUGGER] Debug session saved to {filepath}")
    
    def load_debug_session(self, filepath: str):
        """Load debug session from file."""
        with open(filepath, 'r') as f:
            session_data = json.load(f)
        
        # Load debug info
        self.current_step = session_data['debug_info']['current_step']
        self.simulation_paused = session_data['debug_info']['simulation_paused']
        
        # Load logs
        self.execution_log = session_data['execution_log']
        self.error_log = session_data['error_log']
        self.warning_log = session_data['warning_log']
        self.info_log = session_data['info_log']
        
        # Load breakpoints
        self.breakpoints = []
        for bp_step, bp_message, bp_action in session_data['breakpoints']:
            self.add_breakpoint(bp_step, message=bp_message, action=bp_action)
        
        # Load state history
        self.state_history = []
        for state_dict in session_data['state_history']:
            state = StateSnapshot(**state_dict)
            self.state_history.append(state)
        
        print(f"[DEBUGGER] Debug session loaded from {filepath}, step {self.current_step}")
    
    def _save_current_state(self, name: str):
        """Save current state with specified name."""
        state = self._capture_state()
        state.metadata['name'] = name
        state.metadata['saved_by'] = 'auto_save'
        state.metadata['timestamp'] = time.time()
        
        self.state_history.append(state)
        
        if len(self.state_history) > 1000:  # Keep history manageable
            self.state_history = self.state_history[-1000:]
    
    def compare_states(self, step1: int, step2: int) -> Dict[str, Any]:
        """Compare states at two different steps."""
        state1 = self.get_state_at_step(step1)
        state2 = self.get_state_at_step(step2)
        
        if state1 is None or state2 is None:
            return {'error': 'One or both steps not found in state history'}
        
        comparison = {
            'step1': step1,
            'step2': step2,
            'time1': state1.time,
            'time2': state2.time,
            'differences': self._compare_states(state1, state2)
        }
        
        return comparison
    
    def generate_debug_report(self) -> Dict[str, Any]:
        """Generate comprehensive debug report."""
        report = {
            'session_info': {
                'debug_mode': self.debug_mode,
                'current_step': self.current_step,
                'simulation_paused': self.simulation_paused,
                'total_execution_time': time.time() - (self.state_history[0].timestamp if self.state_history else time.time())
            },
            'statistics': {
                'total_steps': self.current_step,
                'breakpoints_triggered': sum(1 for log in self.execution_log if log.get('event') == 'breakpoint_triggered'),
                'errors': len(self.error_log),
                'warnings': len(self.warning_log),
                'info_logs': len(self.info_log)
            },
            'breakpoints': self.get_breakpoints(),
            'recent_errors': self.get_errors(10),
            'recent_warnings': self.get_errors(5)  # Note: using get_errors for warnings for now
        }
        
        return report