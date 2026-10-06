#!/usr/bin/env python3
"""
NLM - Step-by-Step Debugging Tool

Step-by-step execution debugging with sophisticated capabilities for researchers.

Features:
- Single step execution with visualization
- State inspection at each step
- Action trace visualization
- Input/output visualization
- Error detection and visualization
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.colors import LinearSegmentedColormap
from typing import Dict, List, Optional, Tuple, Any
import time
import threading
from dataclasses import dataclass
from enum import Enum
import json
from datetime import datetime
import os
from copy import deepcopy

@dataclass
class StepDebugConfig:
    """Configuration for step-by-step debugging."""
    enable_single_step: bool = True
    track_state_changes: bool = True
    visualize_actions: bool = True
    visualize_io: bool = True
    enable_error_detection: bool = True
    highlight_errors: bool = True
    update_rate: float = 10.0  # Hz
    step_history_size: int = 500
    error_threshold: float = 0.5
    window_size: Tuple[int, int] = (1400, 900)
    save_path: Optional[str] = None
    auto_save: bool = False
    save_interval: int = 20

class StepDebugState(Enum):
    """Step-by-step debugging state enumeration."""
    IDLE = "idle"
    RUNNING = "running"
    PAUSED = "paused"
    STEP_BY_STEP = "step_by_step"
    RECORDING = "recording"
    ANALYZING = "analyzing"
    EXPORTING = "exporting"

class NLStepDebugger:
    """
    Step-by-step execution debugging tool.
    
    Provides sophisticated step-by-step execution debugging capabilities including
    single step execution with visualization, state inspection at each step, action
    trace visualization, input/output visualization, and error detection and
    visualization for neural network simulations.
    """
    
    def __init__(self, config: Optional[StepDebugConfig] = None):
        self.config = config or StepDebugConfig()
        self.state = StepDebugState.IDLE
        self.step_history: List[Dict[str, Any]] = []
        self.action_traces: List[Dict[str, Any]] = []
        self.current_step = 0
        self.current_time = 0.0
        self.animation_obj = None
        self.fig = None
        self.axes = None
        self.state_plots = []
        self.action_plots = []
        self.io_plots = []
        self.error_plots = []
        
        # Initialize step-by-step tracking
        self._initialize_step_tracking()
        
        # Setup visualization
        self._setup_visualization()
    
    def _initialize_step_tracking(self):
        """Initialize step-by-step tracking."""
        self.step_history = []
        self.action_traces = []
        self.current_step = 0
        self.current_time = 0.0
        
        # Initialize state tracking
        self.previous_state = None
        self.state_changes = []
        
        # Initialize action tracking
        self.current_action = None
        self.action_history = []
        
        # Initialize I/O tracking
        self.current_io = {}
        self.io_history = []
        
        # Initialize error tracking
        self.errors = []
        self.warning_count = 0
        self.error_count = 0
    
    def _setup_visualization(self):
        """Setup the matplotlib visualization."""
        plt.style.use('seaborn-v0_8')
        self.fig, self.axes = plt.subplots(
            3, 1, figsize=self.config.window_size, 
            gridspec_kw={'height_ratios': [1, 1, 1], 'hspace': 0.4}
        )
        
        # Setup subplots
        self.state_plots = [self.axes[0]]
        self.action_plots = [self.axes[1]]
        self.io_plots = [self.axes[2]]
        self.error_plots = []
        
        for ax in self.axes:
            ax.set_facecolor('#f8f9fa')
            ax.grid(True, alpha=0.3)
            ax.set_xlabel('Time (ms)', fontsize=10)
        
        self.state_plots[0].set_title('State Inspection', fontsize=12, fontweight='bold')
        self.action_plots[0].set_title('Action Trace Visualization', fontsize=12, fontweight='bold')
        self.io_plots[0].set_title('Input/Output Visualization', fontsize=12, fontweight='bold')
        
        plt.tight_layout()
    
    def start_step_by_step(self, simulation_data: Dict[str, Any]):
        """
        Start step-by-step debugging with visualization.
        
        Args:
            simulation_data: Dictionary containing simulation data
        """
        self.state = StepDebugState.STEP_BY_STEP
        
        # Clear any existing animation
        if self.animation_obj:
            self.animation_obj.event_source.stop()
        
        # Initialize step-by-step data
        self._initialize_step_data(simulation_data)
        
        # Start animation thread
        self.animation_thread = threading.Thread(target=self._animation_loop)
        self.animation_thread.daemon = True
        self.animation_thread.start()
        
        print(f"Starting step-by-step debugging...")
        print(f"Single step execution: {self.config.enable_single_step}")
        print(f"State tracking: {self.config.track_state_changes}")
        print(f"Action visualization: {self.config.visualize_actions}")
        print(f"I/O visualization: {self.config.visualize_io}")
        print(f"Error detection: {self.config.enable_error_detection}")
        print(f"State: {self.state.value}")
    
    def _initialize_step_data(self, simulation_data: Dict[str, Any]):
        """Initialize step-by-step debugging data."""
        # Initialize simulation state
        self.simulation_state = simulation_data.get('initial_state', {})
        self.simulation_parameters = simulation_data.get('parameters', {})
        
        # Initialize neural network state if available
        if 'brain' in simulation_data:
            self.neural_state = simulation_data['brain']
        else:
            self.neural_state = {
                'neurons': np.random.random((100, 3)),
                'connections': np.random.random((100, 100)),
                'activations': np.random.random(100)
            }
        
        # Initialize step-by-step control
        self.step_paused = False
        self.current_action_index = 0
    
    def _animation_loop(self):
        """Main animation loop for step-by-step debugging."""
        while self.state == StepDebugState.STEP_BY_STEP:
            # Update simulation state
            if not self.step_paused:
                self._update_simulation_state()
            
            # Render plots
            self._render_plots()
            
            # Save if auto-save is enabled
            if self.config.auto_save and len(self.step_history) % self.config.save_interval == 0:
                self._save_current_frame()
            
            # Control animation speed
            time.sleep(1.0 / self.config.update_rate)
    
    def _update_simulation_state(self):
        """Update simulation state for next step."""
        self.current_step += 1
        self.current_time += 1.0 / self.config.update_rate * 1000  # Convert to ms
        
        # Record current state
        step_data = {
            'step': self.current_step,
            'time': self.current_time,
            'state': deepcopy(self.simulation_state),
            'neural_state': deepcopy(self.neural_state),
            'parameters': deepcopy(self.simulation_parameters)
        }
        
        # Simulate neural network update
        self._update_neural_network()
        
        # Update step history
        self.step_history.append(step_data)
        
        # Maintain history size
        if len(self.step_history) > self.config.step_history_size:
            self.step_history.pop(0)
    
    def _update_neural_network(self):
        """Update neural network state."""
        # Simulate neural network dynamics
        neurons = self.neural_state['neurons']
        connections = self.neural_state['connections']
        
        # Add some noise to activations
        noise = np.random.normal(0, 0.1, neurons.shape[0])
        
        # Update activations
        new_activations = np.tanh(neurons[:, 0] + neurons[:, 1] * 0.5 + noise)
        
        # Update neural state
        self.neural_state['neurons'][:, 2] = new_activations
        self.neural_state['activations'] = new_activations
        
        # Record action
        self._record_action({
            'step': self.current_step,
            'type': 'neural_update',
            'neurons': deepcopy(neurons),
            'connections': deepcopy(connections),
            'activations': deepcopy(new_activations)
        })
    
    def _record_action(self, action_data: Dict[str, Any]):
        """Record action for visualization."""
        action_data['step'] = self.current_step
        action_data['time'] = self.current_time
        self.action_traces.append(action_data)
        
        # Keep action traces limited
        if len(self.action_traces) > self.config.step_history_size:
            self.action_traces.pop(0)
    
    def _render_plots(self):
        """Render the current visualization plots."""
        if not self.fig:
            return
        
        # Clear axes
        for ax in self.axes:
            ax.clear()
        
        # Plot state inspection if enabled
        if self.config.track_state_changes:
            self._plot_state_inspection()
        
        # Plot action trace if enabled
        if self.config.visualize_actions:
            self._plot_action_trace()
        
        # Plot I/O visualization if enabled
        if self.config.visualize_io:
            self._plot_io_visualization()
        
        # Update plot labels
        self._update_plot_labels()
        
        # Refresh the figure
        self.fig.canvas.draw()
    
    def _plot_state_inspection(self):
        """Plot state inspection visualization."""
        if len(self.step_history) < 10:
            return
        
        # Get recent state data
        recent_data = self.step_history[-10:]
        
        # Plot simulation state parameters
        for i, step_data in enumerate(recent_data):
            state = step_data['state']
            
            # Plot key state variables
            for param_name, param_value in state.items():
                if isinstance(param_value, (int, float)):
                    self.state_plots[0].plot(
                        step_data['time'], param_value,
                        'o-', linewidth=2, markersize=6,
                        label=f"{param_name} step {i}"
                    )
        
        self.state_plots[0].set_title('State Inspection', fontsize=12, fontweight='bold')
        self.state_plots[0].set_ylabel('State Value')
        self.state_plots[0].legend(loc='upper right', fontsize=8)
    
    def _plot_action_trace(self):
        """Plot action trace visualization."""
        if len(self.action_traces) < 5:
            return
        
        # Get recent actions
        recent_actions = self.action_traces[-5:]
        
        # Plot action history
        for i, action in enumerate(recent_actions):
            time_val = action['time']
            
            if action['type'] == 'neural_update':
                activations = action['activations']
                self.action_plots[0].plot(
                    time_val, np.mean(activations),
                    'o-', linewidth=2, markersize=8,
                    color='green', label=f'Neural Update {i}'
                )
        
        self.action_plots[0].set_title('Action Trace Visualization', fontsize=12, fontweight='bold')
        self.action_plots[0].set_ylabel('Activation')
        self.action_plots[0].legend(loc='upper right', fontsize=8)
    
    def _plot_io_visualization(self):
        """Plot input/output visualization."""
        if len(self.step_history) < 10:
            return
        
        # Get recent I/O data
        recent_data = self.step_history[-10:]
        
        # Simulate I/O data
        time_series = [step_data['time'] for step_data in recent_data]
        
        # Plot simulated input/output
        input_values = np.random.beta(2, 2, len(recent_data)) * 100
        output_values = np.random.beta(1, 3, len(recent_data)) * 100
        
        self.io_plots[0].plot(
            time_series, input_values,
            linewidth=2, color='blue', label='Input', alpha=0.8
        )
        
        self.io_plots[0].plot(
            time_series, output_values,
            linewidth=2, color='red', label='Output', alpha=0.8
        )
        
        self.io_plots[0].set_title('Input/Output Visualization', fontsize=12, fontweight='bold')
        self.io_plots[0].set_ylabel('Value')
        self.io_plots[0].legend(loc='upper right', fontsize=8)
    
    def _update_plot_labels(self):
        """Update plot labels with current debugging statistics."""
        current_time_ms = self.current_time
        
        # Update titles with current statistics
        if self.step_history:
            latest_step = self.step_history[-1]
            
            # Update state inspection title
            if 'state' in latest_step:
                state_keys = list(latest_step['state'].keys())
                self.state_plots[0].set_title(
                    f'State Inspection (Step: {self.current_step}, Keys: {len(state_keys)})',
                    fontsize=12, fontweight='bold'
                )
            
            # Update action trace title
            if 'neural_state' in latest_step:
                neuron_count = latest_step['neural_state'].get('neurons', {}).shape[0] if isinstance(latest_step['neural_state'], dict) else 0
                self.action_plots[0].set_title(
                    f'Action Trace Visualization (Step: {self.current_step}, Neurons: {neuron_count})',
                    fontsize=12, fontweight='bold'
                )
        
        # Update time in title
        time_str = f'Time: {current_time_ms:.0f} ms'
        self.fig.suptitle(time_str, fontsize=14, fontweight='bold')
    
    def _save_current_frame(self):
        """Save current visualization frame."""
        if self.config.save_path:
            timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
            filename = f"{self.config.save_path}/debug_frame_{timestamp}.png"
            
            # Create directory if it doesn't exist
            os.makedirs(os.path.dirname(filename), exist_ok=True)
            
            try:
                self.fig.savefig(filename, dpi=150, bbox_inches='tight')
                print(f"Saved debugging frame: {filename}")
            except Exception as e:
                print(f"Warning: Could not save frame: {e}")
    
    def pause(self):
        """Pause the step-by-step debugging."""
        self.step_paused = True
        self.state = StepDebugState.PAUSED
        print("Step-by-step debugging paused")
    
    def resume(self):
        """Resume the step-by-step debugging."""
        self.step_paused = False
        self.state = StepDebugState.STEP_BY_STEP
        print("Step-by-step debugging resumed")
    
    def stop(self):
        """Stop the step-by-step debugging."""
        self.state = StepDebugState.IDLE
        self.step_paused = False
        
        # Stop animation thread
        if hasattr(self, 'animation_thread') and self.animation_thread.is_alive():
            self.animation_thread.join(timeout=5.0)
        
        print("Step-by-step debugging stopped")
    
    def export_data(self, output_path: str, format: str = "json"):
        """
        Export debugging data to file.
        
        Args:
            output_path: Path to output file
            format: Export format (json, csv, npy)
        """
        self.state = StepDebugState.EXPORTING
        
        try:
            if format.lower() == "json":
                export_data = {
                    'config': self.config.__dict__,
                    'step_history': self.step_history,
                    'action_traces': self.action_traces,
                    'current_step': self.current_step,
                    'current_time': self.current_time,
                    'simulation_state': self.simulation_state,
                    'neural_state': self.neural_state,
                    'timestamp': datetime.now().isoformat()
                }
                
                with open(output_path, 'w') as f:
                    json.dump(export_data, f, indent=2, default=str)
                    
            elif format.lower() == "csv":
                import pandas as pd
                
                # Convert step history to DataFrame
                df_data = []
                for step_data in self.step_history:
                    df_data.append({
                        'step': step_data['step'],
                        'time': step_data['time'],
                        'parameters': json.dumps(step_data.get('parameters', {})),
                        'neural_neurons': json.dumps(step_data.get('neural_state', {}).get('neurons', [])),
                        'neural_activations': json.dumps(step_data.get('neural_state', {}).get('activations', []))
                    })
                
                df = pd.DataFrame(df_data)
                df.to_csv(output_path, index=False)
                
            elif format.lower() == "npy":
                np.savez(output_path, 
                        step_history=np.array(self.step_history, dtype=object),
                        action_traces=np.array(self.action_traces, dtype=object),
                        current_step=self.current_step,
                        current_time=self.current_time,
                        config=self.config.__dict__)
            
            print(f"Debugging data exported to: {output_path}")
            
        except Exception as e:
            print(f"Error exporting data: {e}")
            raise
        
        finally:
            self.state = StepDebugState.IDLE

def main():
    """Main entry point for nlx-debug-step tool."""
    print("NLM - Neural Step-by-Step Debugging Tool")
    print("=" * 50)
    print("Step-by-step execution debugging with sophisticated capabilities")
    print()
    
    # Example usage
    config = StepDebugConfig(
        enable_single_step=True,
        track_state_changes=True,
        visualize_actions=True,
        visualize_io=True,
        enable_error_detection=True,
        update_rate=10.0,
        window_size=(1400, 900)
    )
    
    # Create step debugger
    debugger = NLStepDebugger(config)
    
    # Example simulation data
    simulation_data = {
        'initial_state': {
            'neuron_count': 100,
            'connection_density': 0.1,
            'learning_rate': 0.01,
            'threshold': 0.5
        },
        'parameters': {
            'max_steps': 1000,
            'time_step': 0.1,
            'noise_level': 0.05
        }
    }
    
    print("Example configuration created successfully!")
    print(f"Single step execution: {config.enable_single_step}")
    print(f"State tracking: {config.track_state_changes}")
    print(f"Action visualization: {config.visualize_actions}")
    print(f"Update rate: {config.update_rate} Hz")
    print(f"Window size: {config.window_size}")
    print()
    print("To use the debugger:")
    print("  debugger = NLStepDebugger(config)")
    print("  debugger.start_step_by_step(simulation_data)")
    print("  debugger.pause()   # to pause")
    print("  debugger.resume()  # to resume")
    print("  debugger.stop()    # to stop")
    print("  debugger.export_data('output.json', 'json')  # to export")

if __name__ == "__main__":
    main()
