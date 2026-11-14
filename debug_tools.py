#!/usr/bin/env python3
"""
Development tools and debugging utilities for NLM

This module provides comprehensive debugging and development tools
for NLM, including:
- Advanced debugging capabilities
- Visualization and analysis tools
- Development utilities
- Performance debugging tools
- Memory debugging tools
"""

import sys
import os
from typing import Dict, List, Any, Optional, Callable, Union
import time
import json
import threading
from dataclasses import dataclass
from enum import Enum
import traceback
import logging

# Try to import pynlm
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    pynlm = None

class DebugLevel(Enum):
    """Debug levels"""
    NONE = 0
    BASIC = 1
    DETAILED = 2
    VERBOSE = 3
    TRACE = 4

class DebugComponent(Enum):
    """Debug components"""
    NEURON = "neuron"
    SYNAPSE = "synapse"
    BRAIN = "brain"
    AGENT = "agent"
    WORLD = "world"
    MEMORY = "memory"
    PLASTICITY = "plasticity"
    DEVELOPMENT = "development"
    SENSOR = "sensor"
    MOTOR = "motor"
    PERFORMANCE = "performance"
    CONFIG = "config"

class DebugSession:
    """Debug session for NLM debugging and analysis"""
    
    def __init__(self, debug_level: DebugLevel = DebugLevel.DETAILED,
                 enabled_components: List[DebugComponent] = None):
        self.debug_level = debug_level
        self.enabled_components = enabled_components or [
            DebugComponent.NEURON, DebugComponent.SYNAPSE, DebugComponent.BRAIN,
            DebugComponent.AGENT, DebugComponent.WORLD, DebugComponent.MEMORY,
            DebugComponent.PLASTICITY, DebugComponent.DEVELOPMENT, DebugComponent.PERFORMANCE
        ]
        
        # Debug state
        self.session_start = time.time()
        self.step_count = 0
        self.debug_data: Dict[str, Any] = {}
        self.breakpoints: Dict[str, bool] = {}
        self.watch_variables: Dict[str, Any] = {}
        
        # Component-specific debug data
        self.component_debug: Dict[DebugComponent, Dict[str, Any]] = {
            comp: {} for comp in DebugComponent
        }
        
        # Logging setup
        self.logger = self._setup_logging()
        
        # Performance tracking
        self.performance_timers: Dict[str, float] = {}
        self.memory_usage: Dict[str, float] = {}
        
        # Visualization data
        self.viz_data: Dict[str, Any] = {}
    
    def _setup_logging(self) -> logging.Logger:
        """Setup logging for debugging session"""
        logger = logging.getLogger('nlm.debug')
        logger.setLevel(logging.DEBUG)
        
        # Console handler
        console_handler = logging.StreamHandler(sys.stdout)
        console_handler.setLevel(logging.DEBUG)
        
        # Debug formatter with component information
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - [%(component)s] - %(message)s'
        )
        console_handler.setFormatter(formatter)
        logger.addHandler(console_handler)
        
        # File handler for detailed debugging
        file_handler = logging.FileHandler('nlm_debug.log', mode='a')
        file_handler.setLevel(logging.DEBUG)
        file_handler.setFormatter(formatter)
        logger.addHandler(file_handler)
        
        return logger
    
    def enable_component(self, component: DebugComponent):
        """Enable debugging for a component"""
        if component not in self.enabled_components:
            self.enabled_components.append(component)
            self.logger.info(f"Enabled debugging for component: {component.value}")
    
    def disable_component(self, component: DebugComponent):
        """Disable debugging for a component"""
        if component in self.enabled_components:
            self.enabled_components.remove(component)
            self.logger.info(f"Disabled debugging for component: {component.value}")
    
    def set_debug_level(self, level: DebugLevel):
        """Set debug level"""
        self.debug_level = level
        self.logger.info(f"Set debug level to: {level.value}")
    
    def log_debug(self, message: str, component: DebugComponent = None,
                  level: DebugLevel = None):
        """Log debug message"""
        if level is None:
            level = self.debug_level
        
        if level.value < self.debug_level.value:
            return
        
        # Determine component
        if component is None:
            component = DebugComponent.BRAIN
        
        # Log message
        formatted_message = f"[{component.value}] {message}"
        
        if self.debug_level.value >= DebugLevel.TRACE.value:
            # Add stack trace for trace level
            formatted_message += f"\n{self._get_stack_trace()}"
        
        self.logger.debug(formatted_message)
    
    def _get_stack_trace(self) -> str:
        """Get formatted stack trace"""
        stack = traceback.extract_stack()
        return "\n".join([f"  {frame}" for frame in stack[-5:]])
    
    def start_timing(self, operation: str):
        """Start timing an operation"""
        self.performance_timers[operation] = time.time()
    
    def end_timing(self, operation: str) -> float:
        """End timing and return duration"""
        if operation not in self.performance_timers:
            return 0.0
        
        start_time = self.performance_timers[operation]
        duration = time.time() - start_time
        
        del self.performance_timers[operation]
        
        # Log performance data
        self.log_debug(f"Operation '{operation}' took {duration:.4f}s")
        
        return duration
    
    def record_memory_usage(self, component: DebugComponent, memory_mb: float):
        """Record memory usage for a component"""
        self.memory_usage[component.value] = memory_mb
        
        self.log_debug(
            f"Memory usage for {component.value}: {memory_mb:.2f} MB",
            component=component
        )
    
    def set_breakpoint(self, component: DebugComponent, condition: str):
        """Set a breakpoint for a component"""
        self.breakpoints[component.value] = condition
        self.log_debug(
            f"Set breakpoint for {component.value}: {condition}",
            component=component
        )
    
    def check_breakpoints(self, component: DebugComponent) -> bool:
        """Check if breakpoints are triggered"""
        if component.value not in self.breakpoints:
            return False
        
        condition = self.breakpoints[component.value]
        
        # Evaluate condition (simplified)
        try:
            # For now, just check if condition contains key components
            if 'neuron' in condition and component == DebugComponent.NEURON:
                return True
            elif 'synapse' in condition and component == DebugComponent.SYNAPSE:
                return True
            elif 'brain' in condition and component == DebugComponent.BRAIN:
                return True
            elif 'memory' in condition and component == DebugComponent.MEMORY:
                return True
        except:
            pass
        
        return False
    
    def watch_variable(self, name: str, value: Any):
        """Watch a variable"""
        self.watch_variables[name] = value
        self.log_debug(
            f"Watching variable '{name}' = {value}"
        )
    
    def get_debug_data(self, component: DebugComponent = None) -> Dict[str, Any]:
        """Get debug data for component"""
        if component:
            return self.component_debug[component].copy()
        else:
            return self.debug_data.copy()
    
    def record_neuron_debug(self, neuron_data: Dict[str, Any]):
        """Record neuron-specific debug data"""
        if DebugComponent.NEURON in self.enabled_components:
            self.component_debug[DebugComponent.NEURON][neuron_data.get('id', 'unknown')] = neuron_data
            
            self.log_debug(
                f"Recorded neuron debug: {neuron_data.get('id', 'unknown')}",
                component=DebugComponent.NEURON
            )
    
    def record_synapse_debug(self, synapse_data: Dict[str, Any]):
        """Record synapse-specific debug data"""
        if DebugComponent.SYNAPSE in self.enabled_components:
            self.component_debug[DebugComponent.SYNAPSE][synapse_data.get('id', 'unknown')] = synapse_data
            
            self.log_debug(
                f"Recorded synapse debug: {synapse_data.get('id', 'unknown')}",
                component=DebugComponent.SYNAPSE
            )
    
    def record_brain_debug(self, brain_data: Dict[str, Any]):
        """Record brain-specific debug data"""
        if DebugComponent.BRAIN in self.enabled_components:
            self.component_debug[DebugComponent.BRAIN]['brain'] = brain_data
            
            self.log_debug(
                f"Recorded brain debug: {brain_data.get('neuron_count', 0)} neurons",
                component=DebugComponent.BRAIN
            )
    
    def record_agent_debug(self, agent_data: Dict[str, Any]):
        """Record agent-specific debug data"""
        if DebugComponent.AGENT in self.enabled_components:
            self.component_debug[DebugComponent.AGENT]['agent'] = agent_data
            
            self.log_debug(
                f"Recorded agent debug: curiosity={agent_data.get('curiosity', 0):.3f}",
                component=DebugComponent.AGENT
            )
    
    def record_world_debug(self, world_data: Dict[str, Any]):
        """Record world-specific debug data"""
        if DebugComponent.WORLD in self.enabled_components:
            self.component_debug[DebugComponent.WORLD]['world'] = world_data
            
            self.log_debug(
                f"Recorded world debug: agent pos=({world_data.get('agent_x', 0):.2f}, {world_data.get('agent_y', 0):.2f})",
                component=DebugComponent.WORLD
            )
    
    def record_memory_debug(self, memory_data: Dict[str, Any]):
        """Record memory-specific debug data"""
        if DebugComponent.MEMORY in self.enabled_components:
            self.component_debug[DebugComponent.MEMORY]['memory'] = memory_data
            
            self.log_debug(
                f"Recorded memory debug: working_memory={memory_data.get('working_memory_size', 0)} traces",
                component=DebugComponent.MEMORY
            )
    
    def record_plasticity_debug(self, plasticity_data: Dict[str, Any]):
        """Record plasticity-specific debug data"""
        if DebugComponent.PLASTICITY in self.enabled_components:
            self.component_debug[DebugComponent.PLASTICITY]['plasticity'] = plasticity_data
            
            self.log_debug(
                f"Recorded plasticity debug: STDP active={plasticity_data.get('stdp_enabled', False)}",
                component=DebugComponent.PLASTICITY
            )
    
    def record_development_debug(self, development_data: Dict[str, Any]):
        """Record development-specific debug data"""
        if DebugComponent.DEVELOPMENT in self.enabled_components:
            self.component_debug[DebugComponent.DEVELOPMENT]['development'] = development_data
            
            self.log_debug(
                f"Recorded development debug: stage={development_data.get('stage', 'unknown')}",
                component=DebugComponent.DEVELOPMENT
            )
    
    def create_visualization_data(self, data_type: str, data: Dict[str, Any]):
        """Create visualization data"""
        self.viz_data[data_type] = data
        self.log_debug(
            f"Created visualization data: {data_type}",
            component=DebugComponent.PERFORMANCE
        )
    
    def generate_debug_report(self) -> Dict[str, Any]:
        """Generate comprehensive debug report"""
        report = {
            'session_info': {
                'start_time': self.session_start,
                'duration': time.time() - self.session_start,
                'step_count': self.step_count,
                'debug_level': self.debug_level.value,
                'enabled_components': [comp.value for comp in self.enabled_components],
                'breakpoints': self.breakpoints,
                'watch_variables': self.watch_variables,
            },
            'performance_timers': self.performance_timers,
            'memory_usage': self.memory_usage,
            'component_debug': {comp.value: len(self.component_debug[comp]) for comp in DebugComponent},
            'visualization_data': list(self.viz_data.keys()),
        }
        
        return report
    
    def export_debug_data(self, filepath: str):
        """Export debug data to file"""
        debug_data = {
            'session_info': {
                'start_time': self.session_start,
                'duration': time.time() - self.session_start,
                'step_count': self.step_count,
                'debug_level': self.debug_level.value,
                'enabled_components': [comp.value for comp in self.enabled_components],
            },
            'component_debug': self.component_debug,
            'visualization_data': self.viz_data,
            'performance_timers': self.performance_timers,
            'memory_usage': self.memory_usage,
        }
        
        try:
            with open(filepath, 'w') as f:
                json.dump(debug_data, f, indent=2, default=str)
            
            self.logger.info(f"Debug data exported to: {filepath}")
            return True
        except Exception as e:
            self.logger.error(f"Error exporting debug data: {e}")
            return False

class NeuronDebugger:
    """Advanced neuron debugging capabilities"""
    
    def __init__(self, debug_session: DebugSession):
        self.debug_session = debug_session
        self.logger = self.debug_session.logger
    
    def debug_neuron_creation(self, neuron_id: str, neuron_type: str):
        """Debug neuron creation"""
        neuron_data = {
            'id': neuron_id,
            'type': neuron_type,
            'timestamp': time.time(),
            'debug_level': 'creation'
        }
        
        self.debug_session.record_neuron_debug(neuron_data)
        
        self.logger.info(f"Neuron created: id={neuron_id}, type={neuron_type}")
    
    def debug_neuron_step(self, neuron_id: str, membrane_potential: float,
                         firing_state: str, spike_time: Optional[float] = None):
        """Debug neuron step"""
        neuron_data = {
            'id': neuron_id,
            'membrane_potential': membrane_potential,
            'firing_state': firing_state,
            'spike_time': spike_time,
            'timestamp': time.time(),
            'debug_level': 'step'
        }
        
        self.debug_session.record_neuron_debug(neuron_data)
        
        self.logger.debug(f"Neuron step: id={neuron_id}, V={membrane_potential:.3f}, state={firing_state}")
    
    def debug_neuron_connectivity(self, neuron_id: str, synapses: List[Dict[str, Any]]):
        """Debug neuron connectivity"""
        neuron_data = {
            'id': neuron_id,
            'synapses': synapses,
            'total_synapses': len(synapses),
            'timestamp': time.time(),
            'debug_level': 'connectivity'
        }
        
        self.debug_session.record_neuron_debug(neuron_data)
        
        self.logger.info(f"Neuron connectivity: id={neuron_id}, {len(synapses)} synapses")

class SynapseDebugger:
    """Advanced synapse debugging capabilities"""
    
    def __init__(self, debug_session: DebugSession):
        self.debug_session = debug_session
        self.logger = self.debug_session.logger
    
    def debug_synapse_creation(self, synapse_id: str, source_id: str, dest_id: str,
                             weight: float, type: str):
        """Debug synapse creation"""
        synapse_data = {
            'id': synapse_id,
            'source_id': source_id,
            'destination_id': dest_id,
            'weight': weight,
            'type': type,
            'timestamp': time.time(),
            'debug_level': 'creation'
        }
        
        self.debug_session.record_synapse_debug(synapse_data)
        
        self.logger.info(f"Synapse created: id={synapse_id}, {source_id} -> {dest_id}")
    
    def debug_synapse_plasticity(self, synapse_id: str, weight_change: float,
                                 ltp: bool, ltd: bool):
        """Debug synapse plasticity"""
        synapse_data = {
            'id': synapse_id,
            'weight_change': weight_change,
            'ltp': ltp,
            'ltd': ltd,
            'timestamp': time.time(),
            'debug_level': 'plasticity'
        }
        
        self.debug_session.record_synapse_debug(synapse_data)
        
        change_type = 'LTP' if ltp else ('LTD' if ltd else 'None')
        self.logger.debug(f"Synapse plasticity: id={synapse_id}, {change_type}, Δw={weight_change:.4f}")

class BrainDebugger:
    """Advanced brain debugging capabilities"""
    
    def __init__(self, debug_session: DebugSession):
        self.debug_session = debug_session
        self.logger = self.debug_session.logger
    
    def debug_brain_initialization(self, config: Dict[str, Any]):
        """Debug brain initialization"""
        brain_data = {
            'neuron_count': config.get('neuron_count', 0),
            'region_count': config.get('region_count', 0),
            'connection_probability': config.get('connection_probability', 0.0),
            'plasticity_enabled': config.get('plasticity_enabled', False),
            'neuromodulation_enabled': config.get('neuromodulation_enabled', False),
            'development_enabled': config.get('development_enabled', False),
            'timestamp': time.time(),
            'debug_level': 'initialization'
        }
        
        self.debug_session.record_brain_debug(brain_data)
        
        self.logger.info(f"Brain initialized: {brain_data['neuron_count']} neurons, {brain_data['region_count']} regions")
    
    def debug_brain_step(self, step: int, brain_metrics: Dict[str, Any]):
        """Debug brain step"""
        brain_data = {
            'step': step,
            'firing_rate': brain_metrics.get('firing_rate', 0.0),
            'active_neurons': brain_metrics.get('active_neurons', 0),
            'spike_count': brain_metrics.get('spike_count', 0),
            'average_firing_rate': brain_metrics.get('average_firing_rate', 0.0),
            'e_i_ratio': brain_metrics.get('e_i_ratio', 1.0),
            'timestamp': time.time(),
            'debug_level': 'step'
        }
        
        self.debug_session.record_brain_debug(brain_data)
        
        self.logger.info(f"Brain step {step}: {brain_metrics.get('active_neurons', 0)} active neurons, {brain_metrics.get('firing_rate', 0.0):.2f} Hz")

class AgentDebugger:
    """Advanced agent debugging capabilities"""
    
    def __init__(self, debug_session: DebugSession):
        self.debug_session = debug_session
        self.logger = self.debug_session.logger
    
    def debug_agent_initialization(self, world_config: Dict[str, Any]):
        """Debug agent initialization"""
        agent_data = {
            'world_width': world_config.get('world_width', 0.0),
            'world_height': world_config.get('world_height', 0.0),
            'vision_width': world_config.get('vision_width', 0),
            'vision_height': world_config.get('vision_height', 0),
            'reward_modulation': world_config.get('reward_modulation', False),
            'curiosity': world_config.get('curiosity', False),
            'development': world_config.get('development', False),
            'timestamp': time.time(),
            'debug_level': 'initialization'
        }
        
        self.debug_session.record_agent_debug(agent_data)
        
        self.logger.info(f"Agent initialized: world size {world_config.get('world_width', 0.0)}x{world_config.get('world_height', 0.0)}")
    
    def debug_agent_decision(self, step: int, action: Dict[str, Any], reward: float,
                           curiosity: float, novelty: float):
        """Debug agent decision"""
        agent_data = {
            'step': step,
            'action': action,
            'reward': reward,
            'curiosity': curiosity,
            'novelty': novelty,
            'timestamp': time.time(),
            'debug_level': 'decision'
        }
        
        self.debug_session.record_agent_debug(agent_data)
        
        action_str = action.get('type', 'unknown') if isinstance(action, dict) else str(action)
        self.logger.info(f"Agent decision: step {step}, action={action_str}, reward={reward:.2f}, curiosity={curiosity:.3f}")

class WorldDebugger:
    """Advanced world debugging capabilities"""
    
    def __init__(self, debug_session: DebugSession):
        self.debug_session = debug_session
        self.logger = self.debug_session.logger
    
    def debug_world_initialization(self, config: Dict[str, Any]):
        """Debug world initialization"""
        world_data = {
            'world_type': config.get('world_type', 'simple'),
            'width': config.get('width', 0.0),
            'height': config.get('height', 0.0),
            'vision_width': config.get('vision_width', 0),
            'vision_height': config.get('vision_height', 0),
            'agent_start_x': config.get('agent_start_x', 0.0),
            'agent_start_y': config.get('agent_start_y', 0.0),
            'random_seed': config.get('random_seed', 0),
            'timestamp': time.time(),
            'debug_level': 'initialization'
        }
        
        self.debug_session.record_world_debug(world_data)
        
        self.logger.info(f"World initialized: {world_data['width']}x{world_data['height']}, agent at ({world_data['agent_start_x']}, {world_data['agent_start_y']})")
    
    def debug_world_step(self, step: int, world_metrics: Dict[str, Any]):
        """Debug world step"""
        world_data = {
            'step': step,
            'agent_x': world_metrics.get('agent_x', 0.0),
            'agent_y': world_metrics.get('agent_y', 0.0),
            'agent_orientation': world_metrics.get('agent_orientation', 0.0),
            'agent_energy': world_metrics.get('agent_energy', 0.0),
            'agent_health': world_metrics.get('agent_health', 100.0),
            'sim_time': world_metrics.get('sim_time', 0.0),
            'timestamp': time.time(),
            'debug_level': 'step'
        }
        
        self.debug_session.record_world_debug(world_data)
        
        self.logger.info(f"World step {step}: agent at ({world_data['agent_x']:.2f}, {world_data['agent_y']:.2f}), energy={world_data['agent_energy']:.1f}")

class MemoryDebugger:
    """Advanced memory debugging capabilities"""
    
    def __init__(self, debug_session: DebugSession):
        self.debug_session = debug_session
        self.logger = self.debug_session.logger
    
    def debug_memory_state(self, step: int, memory_metrics: Dict[str, Any]):
        """Debug memory state"""
        memory_data = {
            'step': step,
            'working_memory_size': memory_metrics.get('working_memory_size', 0),
            'working_memory_capacity': memory_metrics.get('working_memory_capacity', 0),
            'episodic_memory_count': memory_metrics.get('episodic_memory_count', 0),
            'episodic_memory_capacity': memory_metrics.get('episodic_memory_capacity', 0),
            'associative_strength': memory_metrics.get('associative_strength', 0.0),
            'memory_decay_rate': memory_metrics.get('memory_decay_rate', 0.0),
            'timestamp': time.time(),
            'debug_level': 'state'
        }
        
        self.debug_session.record_memory_debug(memory_data)
        
        self.logger.info(f"Memory state: step {step}, working_memory={memory_data['working_memory_size']}/{memory_data['working_memory_capacity']}, episodic={memory_data['episodic_memory_count']}")

class PerformanceDebugger:
    """Advanced performance debugging capabilities"""
    
    def __init__(self, debug_session: DebugSession):
        self.debug_session = debug_session
        self.logger = self.debug_session.logger
    
    def debug_performance_metrics(self, step: int, performance_metrics: Dict[str, Any]):
        """Debug performance metrics"""
        self.debug_session.start_timing('performance_metrics')
        
        try:
            self.debug_session.log_debug(
                f"Performance metrics: step {step}, "
                f"step_time={performance_metrics.get('step_time', 0.0):.4f}s, "
                f"memory={performance_metrics.get('memory_usage', 0.0):.1f}MB",
                component=DebugComponent.PERFORMANCE
            )
            
            duration = self.debug_session.end_timing('performance_metrics')
            
            if duration > 0.1:  # Log slow performance
                self.logger.warning(f"Performance issue: step {step} took {duration:.3f}s")
            
        except Exception as e:
            self.debug_session.log_debug(f"Error in performance debugging: {e}")
    
    def debug_memory_usage(self, memory_mb: float):
        """Debug memory usage"""
        self.debug_session.record_memory_usage(
            DebugComponent.PERFORMANCE,
            memory_mb
        )
        
        self.logger.debug(f"Memory usage: {memory_mb:.1f} MB")
    
    def debug_timing(self, operation: str, duration: float):
        """Debug timing information"""
        self.debug_session.log_debug(
            f"Timing: {operation} = {duration:.4f}s",
            component=DebugComponent.PERFORMANCE
        )

class DebugVisualization:
    """Visualization tools for debugging"""
    
    def __init__(self, debug_session: DebugSession):
        self.debug_session = debug_session
        self.logger = self.debug_session.logger
    
    def create_firing_rate_plot(self, firing_rates: List[float], step: int):
        """Create firing rate visualization data"""
        viz_data = {
            'type': 'firing_rate_plot',
            'step': step,
            'data': firing_rates,
            'mean': sum(firing_rates) / len(firing_rates) if firing_rates else 0,
            'max': max(firing_rates) if firing_rates else 0,
            'min': min(firing_rates) if firing_rates else 0,
            'timestamp': time.time()
        }
        
        self.debug_session.create_visualization_data('firing_rate', viz_data)
        
        self.logger.info(f"Created firing rate plot: step {step}, mean={viz_data['mean']:.2f}")
    
    def create_neuron_activity_plot(self, neuron_activities: Dict[str, List[float]], step: int):
        """Create neuron activity visualization data"""
        viz_data = {
            'type': 'neuron_activity_plot',
            'step': step,
            'neurons': list(neuron_activities.keys()),
            'data': neuron_activities,
            'timestamp': time.time()
        }
        
        self.debug_session.create_visualization_data('neuron_activity', viz_data)
        
        self.logger.info(f"Created neuron activity plot: step {step}, {len(neuron_activities)} neurons")
    
    def create_performance_timeline(self, performance_data: List[Dict[str, Any]]):
        """Create performance timeline visualization"""
        viz_data = {
            'type': 'performance_timeline',
            'data': performance_data,
            'timestamp': time.time()
        }
        
        self.debug_session.create_visualization_data('performance_timeline', viz_data)
        
        self.logger.info(f"Created performance timeline: {len(performance_data)} data points")
    
    def create_memory_usage_plot(self, memory_data: List[float], step: int):
        """Create memory usage visualization"""
        viz_data = {
            'type': 'memory_usage_plot',
            'step': step,
            'data': memory_data,
            'max_memory': max(memory_data) if memory_data else 0,
            'min_memory': min(memory_data) if memory_data else 0,
            'average_memory': sum(memory_data) / len(memory_data) if memory_data else 0,
            'timestamp': time.time()
        }
        
        self.debug_session.create_visualization_data('memory_usage', viz_data)
        
        self.logger.info(f"Created memory usage plot: step {step}, max={viz_data['max_memory']:.1f} MB")

class DebugReporter:
    """Generate debug reports"""
    
    def __init__(self, debug_session: DebugSession):
        self.debug_session = debug_session
        self.logger = self.debug_session.logger
    
    def generate_summary_report(self) -> Dict[str, Any]:
        """Generate summary report of debugging session"""
        report = {
            'session_duration': time.time() - self.debug_session.session_start,
            'total_steps': self.debug_session.step_count,
            'debug_level': self.debug_session.debug_level.value,
            'enabled_components': [comp.value for comp in self.debug_session.enabled_components],
            'component_debug_counts': {
                comp.value: len(self.debug_session.component_debug[comp])
                for comp in DebugComponent
            },
            'total_breakpoints': len(self.debug_session.breakpoints),
            'total_watch_variables': len(self.debug_session.watch_variables),
            'performance_timers': len(self.debug_session.performance_timers),
            'memory_usage_points': len(self.debug_session.memory_usage),
        }
        
        return report
    
    def generate_performance_report(self) -> Dict[str, Any]:
        """Generate performance report"""
        # Calculate performance statistics
        if not self.debug_session.performance_timers:
            return {}
        
        durations = list(self.debug_session.performance_timers.values())
        
        report = {
            'operation_count': len(self.debug_session.performance_timers),
            'total_duration': sum(durations),
            'average_duration': sum(durations) / len(durations) if durations else 0,
            'min_duration': min(durations) if durations else 0,
            'max_duration': max(durations) if durations else 0,
        }
        
        return report
    
    def generate_component_report(self, component: DebugComponent) -> Dict[str, Any]:
        """Generate report for specific component"""
        if component not in self.debug_session.component_debug:
            return {}
        
        debug_data = self.debug_session.component_debug[component]
        
        report = {
            'component': component.value,
            'data_points': len(debug_data),
            APPENDIX: {
                'last_update': max(
                    [item.get('timestamp', 0) for item in debug_data.values()],
                    default=0
                ),
                'debug_levels': {
                    level: sum(1 for item in debug_data.values() 
                             if item.get('debug_level', '') == level)
                    for level in ['creation', 'step', 'connectivity', 'plasticity', 'initialization', 'decision', 'state']
                }
            }
        }
        
        return report
    
    def export_all_reports(self, output_dir: str):
        """Export all debug reports to files"""
        os.makedirs(output_dir, exist_ok=True)
        
        # Export summary report
        summary_report = self.generate_summary_report()
        summary_file = os.path.join(output_dir, 'debug_summary.json')
        with open(summary_file, 'w') as f:
            json.dump(summary_report, f, indent=2)
        
        # Export component reports
        for component in DebugComponent:
            component_report = self.generate_component_report(component)
            if component_report:
                component_file = os.path.join(output_dir, f'debug_{component.value}.json')
                with open(component_file, 'w') as f:
                    json.dump(component_report, f, indent=2)
        
        # Export debug session data
        debug_file = os.path.join(output_dir, 'debug_session.json')
        self.debug_session.export_debug_data(debug_file)
        
        self.logger.info(f"Debug reports exported to: {output_dir}")
        return True

class NLMDebugger:
    """Main debugger class for NLM"""
    
    def __init__(self, debug_level: DebugLevel = DebugLevel.DETAILED,
                 enabled_components: List[DebugComponent] = None):
        self.debug_session = DebugSession(debug_level, enabled_components)
        
        # Initialize component debuggers
        self.neuron_debugger = NeuronDebugger(self.debug_session)
        self.synapse_debugger = SynapseDebugger(self.debug_session)
        self.brain_debugger = BrainDebugger(self.debug_session)
        self.agent_debugger = AgentDebugger(self.debug_session)
        self.world_debugger = WorldDebugger(self.debug_session)
        self.memory_debugger = MemoryDebugger(self.debug_session)
        self.performance_debugger = PerformanceDebugger(self.debug_session)
        
        # Initialize visualization tools
        self.visualization = DebugVisualization(self.debug_session)
        
        # Initialize reporter
        self.reporter = DebugReporter(self.debug_session)
        
        self.logger = self.debug_session.logger
    
    def start_debugging(self):
        """Start debugging session"""
        self.logger.info("Starting NLM debugging session")
    
    def stop_debugging(self):
        """Stop debugging session"""
        self.logger.info("Stopping NLM debugging session")
        
        # Generate debug report
        debug_report = self.debug_session.generate_debug_report()
        
        # Save debug report
        report_file = "nlm_debug_report.json"
        with open(report_file, 'w') as f:
            json.dump(debug_report, f, indent=2)
        
        self.logger.info(f"Debug report saved to: {report_file}")
        
        return debug_report
    
    def debug_neuron(self, neuron_id: str, neuron_type: str, 
                    membrane_potential: float = 0.0, firing_state: str = "resting"):
        """Debug neuron with basic information"""
        self.neuron_debugger.debug_neuron_step(
            neuron_id, membrane_potential, firing_state
        )
    
    def debug_synapse(self, synapse_id: str, source_id: str, dest_id: str,
                     weight: float, ltp: bool = False, ltd: bool = False):
        """Debug synapse with basic information"""
        self.synapse_debugger.debug_synapse_plasticity(
            synapse_id, weight, ltp, ltd
        )
    
    def debug_brain_step(self, step: int, brain_metrics: Dict[str, Any]):
        """Debug brain step"""
        self.brain_debugger.debug_brain_step(step, brain_metrics)
    
    def debug_agent_decision(self, step: int, action: Dict[str, Any], reward: float,
                           curiosity: float, novelty: float):
        """Debug agent decision"""
        self.agent_debugger.debug_agent_decision(
            step, action, reward, curiosity, novelty
        )
    
    def debug_world_step(self, step: int, world_metrics: Dict[str, Any]):
        """Debug world step"""
        self.world_debugger.debug_world_step(step, world_metrics)
    
    def record_performance_metrics(self, step: int, performance_metrics: Dict[str, Any]):
        """Record performance metrics"""
        self.performance_debugger.debug_performance_metrics(
            step, performance_metrics
        )
    
    def export_debug_data(self, filepath: str):
        """Export debug data"""
        return self.debug_session.export_debug_data(filepath)
    
    def generate_debug_report(self) -> Dict[str, Any]:
        """Generate debug report"""
        return self.debug_session.generate_debug_report()

# Convenience functions for common debugging tasks
def create_neuron_debugger() -> NeuronDebugger:
    """Create neuron debugger"""
    debug_session = DebugSession()
    return NeuronDebugger(debug_session)

def create_synapse_debugger() -> SynapseDebugger:
    """Create synapse debugger"""
    debug_session = DebugSession()
    return SynapseDebugger(debug_session)

def create_brain_debugger() -> BrainDebugger:
    """Create brain debugger"""
    debug_session = DebugSession()
    return BrainDebugger(debug_session)

def create_agent_debugger() -> AgentDebugger:
    """Create agent debugger"""
    debug_session = DebugSession()
    return AgentDebugger(debug_session)

def create_world_debugger() -> WorldDebugger:
    """Create world debugger"""
    debug_session = DebugSession()
    return WorldDebugger(debug_session)

def create_memory_debugger() -> MemoryDebugger:
    """Create memory debugger"""
    debug_session = DebugSession()
    return MemoryDebugger(debug_session)

def create_performance_debugger() -> PerformanceDebugger:
    """Create performance debugger"""
    debug_session = DebugSession()
    return PerformanceDebugger(debug_session)

def create_full_debugger() -> NLMDebugger:
    """Create full debugger with all components"""
    return NLMDebugger()

if __name__ == "__main__":
    print("NLM Development Tools and Debugging Utilities")
    print("=" * 50)
    
    # Create debugger
    debugger = create_full_debugger()
    
    # Start debugging
    debugger.start_debugging()
    
    # Example debugging session
    print("\nExample debugging session...")
    
    # Debug neuron
    debugger.debug_neuron("neuron_1", "excitatory", -65.0, "resting")
    
    # Debug synapse
    debugger.debug_synapse("synapse_1", "neuron_1", "neuron_2", 0.5, ltp=True)
    
    # Debug brain step
    brain_metrics = {
        'firing_rate': 5.2,
        'active_neurons': 12,
        'spike_count': 150,
        'average_firing_rate': 3.8,
        'e_i_ratio': 1.2
    }
    debugger.debug_brain_step(1, brain_metrics)
    
    # Debug agent decision
    debugger.debug_agent_decision(1, {'type': 'move_forward'}, 0.5, 0.3, 0.1)
    
    # Debug world step
    world_metrics = {
        'agent_x': 10.0,
        'agent_y': 5.0,
        'agent_orientation': 0.5,
        'agent_energy': 80.0,
        'agent_health': 95.0,
        'sim_time': 1.0
    }
    debugger.debug_world_step(1, world_metrics)
    
    # Debug performance metrics
    performance_metrics = {
        'step_time': 0.023,
        'memory_usage': 45.2,
        'cpu_usage': 25.0
    }
    debugger.record_performance_metrics(1, performance_metrics)
    
    # Stop debugging and generate report
    debug_report = debugger.stop_debugging()
    
    print("\nDebug session completed successfully!")
    print(f"Generated debug report with {debug_report['total_steps']} steps")
    print(f"Enabled components: {debug_report['enabled_components']}")
    
    # Export debug data
    debugger.export_debug_data("nlm_debug_output.json")
    
    print("Debug data exported to: nlm_debug_output.json")
    print("\nDevelopment tools and debugging utilities demonstration complete!")