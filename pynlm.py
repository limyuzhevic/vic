# pynlm.py - Enhanced Python API for NLM
# This file represents the enhanced Python API that would be generated
# from improved C++ bindings with better error handling, context managers,
# and Pythonic interfaces.

"""
NLM (Neural Learning Machine) - Enhanced Python API
===================================================

This is the enhanced Python interface for the NLM neural simulation framework.
It provides a more Pythonic, robust, and feature-complete API compared to
the basic bindings.

Key improvements:
• Context managers for resource management
• Better error handling with Python exceptions
• Performance monitoring tools
• Advanced simulation control
• Neural recording and playback
• Configuration management
• Visualization and analysis tools
"""

import time
import threading
import numpy as np
from contextlib import contextmanager
from typing import List, Dict, Optional, Callable, Any, Union
import json

# === Base Classes and Exceptions ===

class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Error in configuration."""
    pass

class SimulationError(NLMError):
    """Error during simulation."""
    pass

class ResourceError(NLMError):
    """Resource management error."""
    pass

# === Enhanced Configuration ===

class EnhancedConfig:
    """
    Enhanced configuration class with better Pythonic interfaces
    and validation capabilities.
    """
    
    def __init__(self):
        self._config = pynlm.createDefaultConfig()
        self._schema = self._get_default_schema()
    
    def _get_default_schema(self):
        """Get the default configuration schema."""
        return {
            'neuron_count': {'type': int, 'min': 10, 'max': 10000, 'default': 500},
            'region_count': {'type': int, 'min': 1, 'max': 10, 'default': 1},
            'connection_probability': {'type': float, 'min': 0.0, 'max': 1.0, 'default': 0.15},
            'stdp_ltp_weight': {'type': float, 'min': 0.0, 'default': 0.02},
            'stdp_ltd_weight': {'type': float, 'min': 0.0, 'default': 0.015},
            'development_enabled': {'type': bool, 'default': True},
            'curiosity_enabled': {'type': bool, 'default': True},
            'reward_modulation_enabled': {'type': bool, 'default': True},
            'structural_plasticity_enabled': {'type': bool, 'default': True},
            'simulation_timestep': {'type': float, 'min': 0.0001, 'max': 0.1, 'default': 0.001},
            'random_seed': {'type': int, 'default': 42}
        }
    
    def validate_value(self, key: str, value: Any) -> bool:
        """Validate a configuration value against schema."""
        if key not in self._schema:
            return True  # Unknown keys are allowed for flexibility
        
        schema = self._schema[key]
        
        # Type validation
        if 'type' in schema:
            expected_type = schema['type']
            if not isinstance(value, expected_type):
                if expected_type == float and isinstance(value, int):
                    value = float(value)  # Allow int to float conversion
                else:
                    return False
        
        # Range validation
        if 'min' in schema and value < schema['min']:
            return False
        if 'max' in schema and value > schema['max']:
            return False
        
        return True
    
    def set(self, key: str, value: Any, validate: bool = True):
        """
        Set a configuration value with optional validation.
        
        Args:
            key: Configuration key
            value: Configuration value
            validate: Whether to validate the value
        
        Raises:
            ConfigurationError: If validation fails
        """
        if validate and not self.validate_value(key, value):
            raise ConfigurationError(
                f"Invalid value for '{key}': {value}. "
                f"Must be {self._schema.get(key, {}).get('type', 'unknown')}"
            )
        
        # Use the underlying config
        if isinstance(value, bool):
            self._config.set(key, value)
        elif isinstance(value, int):
            self._config.set(key, value)
        elif isinstance(value, float):
            self._config.set(key, value)
        elif isinstance(value, str):
            self._config.set(key, value)
        else:
            raise ConfigurationError(f"Unsupported configuration type: {type(value)}")
    
    def get(self, key: str, default=None):
        """Get a configuration value."""
        return self._config.getOr(key, default)
    
    def get_all(self) -> Dict[str, Any]:
        """Get all configuration values."""
        keys = self._config.getKeys()
        return {key: self.get(key) for key in keys}
    
    def to_dict(self) -> Dict[str, Any]:
        """Export configuration as dictionary."""
        return self.get_all()
    
    def from_dict(self, config_dict: Dict[str, Any]):
        """Load configuration from dictionary."""
        for key, value in config_dict.items():
            self.set(key, value, validate=True)
    
    def save(self, filepath: str):
        """Save configuration to file."""
        # Export to JSON-serializable format
        config_dict = self.to_dict()
        
        # Convert numpy values to Python types
        def convert_value(v):
            if isinstance(v, np.ndarray):
                return v.tolist()
            elif isinstance(v, np.floating):
                return float(v)
            elif isinstance(v, np.integer):
                return int(v)
            else:
                return v
        
        config_dict = {k: convert_value(v) for k, v in config_dict.items()}
        
        with open(filepath, 'w') as f:
            json.dump(config_dict, f, indent=2)
    
    def load(self, filepath: str):
        """Load configuration from file."""
        with open(filepath, 'r') as f:
            config_dict = json.load(f)
        
        self.from_dict(config_dict)
    
    def __repr__(self):
        return f"EnhancedConfig({len(self.get_all())} parameters)"

# === Enhanced Brain with Context Manager ===

class EnhancedBrain:
    """
    Enhanced brain class with context manager support,
    performance monitoring, and advanced APIs.
    """
    
    def __init__(self, config=None):
        self._config = config or EnhancedConfig()
        self._brain = pynlm.createBrain(self._config._config)
        self._monitor = None
        self._recording = None
        self._is_active = False
    
    def __enter__(self):
        """Context manager entry."""
        self.initialize()
        self._is_active = True
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.reset()
        self._is_active = False
    
    def initialize(self):
        """Initialize the brain with current configuration."""
        if self._is_active:
            raise ResourceError("Brain is already active. Call reset() first.")
        
        self._brain.initialize()
        self._is_active = True
        return self
    
    def reset(self):
        """Reset the brain to initial state."""
        if not self._is_active:
            return self
        
        self._brain.reset()
        self._is_active = False
        return self
    
    def step(self, step_num: int, timestep: float = None):
        """
        Perform a simulation step.
        
        Args:
            step_num: Current step number
            timestep: Time step (uses config if None)
        """
        if not self._is_active:
            raise SimulationError("Brain is not active. Call initialize() first.")
        
        if timestep is None:
            timestep = self._config.get('simulation_timestep', 0.001)
        
        self._brain.step(step_num, timestep)
        
        # Update monitoring
        if self._monitor:
            self._monitor.step(step_num)
        
        return self
    
    def get_total_spike_count(self) -> int:
        """Get total spike count."""
        return self._brain.getTotalSpikeCount()
    
    def get_average_firing_rate(self) -> float:
        """Get average firing rate."""
        return self._brain.getAverageFiringRate()
    
    def get_firing_neuron_count(self) -> int:
        """Get count of currently firing neurons."""
        return self._brain.getFiringNeuronCount()
    
    def get_total_neuron_count(self) -> int:
        """Get total neuron count."""
        return self._brain.getTotalNeuronCount()
    
    def get_total_synapse_count(self) -> int:
        """Get total synapse count."""
        return self._brain.getTotalSynapseCount()
    
    def get_active_neuron_count(self) -> int:
        """Get count of active neurons."""
        return self._brain.getActiveNeuronCount()
    
    def get_development_stage(self):
        """Get current developmental stage."""
        return self._brain.getDevelopmentalStage()
    
    def get_config(self) -> EnhancedConfig:
        """Get the configuration."""
        return self._config
    
    def start_monitoring(self):
        """Start performance monitoring."""
        self._monitor = BrainMonitor(self)
        self._monitor.start_monitoring()
        return self._monitor
    
    def stop_monitoring(self):
        """Stop performance monitoring."""
        if self._monitor:
            self._monitor.stop_monitoring()
            self._monitor = None
        return self
    
    def start_recording(self, name: str = "recording"):
        """Start recording neural activity."""
        self._recording = NeuralRecorder(self)
        self._recording.start_recording(name)
        return self._recording
    
    def stop_recording(self):
        """Stop recording and return the recording."""
        if self._recording:
            recording = self._recording.stop_recording()
            self._recording = None
            return recording
        return None
    
    def run_simulation(self, num_steps: int, render_callback=None):
        """
        Run a complete simulation.
        
        Args:
            num_steps: Number of simulation steps
            render_callback: Optional callback for each step
        """
        self.initialize()
        
        try:
            for step in range(num_steps):
                self.step(step)
                
                if render_callback:
                    render_callback(step, self)
            
            return self
        finally:
            self.reset()
    
    def __repr__(self):
        return f"EnhancedBrain(neurons={self.get_total_neuron_count()}, " \
               f"synapses={self.get_total_synapse_count()}, " \
               f"active={self.get_active_neuron_count()})"

# === Brain Monitor ===

class BrainMonitor:
    """
    Monitor brain performance and collect metrics.
    """
    
    def __init__(self, brain: EnhancedBrain):
        self.brain = brain
        self.metrics = []
        self.step_times = []
        self.start_time = None
        self.is_running = False
    
    def start_monitoring(self):
        """Start monitoring."""
        self.start_time = time.time()
        self.step_times = []
        self.metrics = []
        self.is_running = True
        print("✓ Brain monitoring started")
        return self
    
    def step(self, step_num: int):
        """
        Monitor a single step.
        
        Args:
            step_num: Current step number
        """
        if not self.is_running:
            return
        
        step_start = time.time()
        
        # Get metrics before step
        before_metrics = {
            'step': step_num,
            'spike_count': self.brain.get_total_spike_count(),
            'firing_rate': self.brain.get_average_firing_rate(),
            'active_neurons': self.brain.get_active_neuron_count()
        }
        
        # Perform the step (monitoring happens in EnhancedBrain)
        self.brain.step(step_num)
        
        # Get metrics after step
        after_metrics = {
            'step': step_num,
            'spike_count': self.brain.get_total_spike_count(),
            'firing_rate': self.brain.get_average_firing_rate(),
            'active_neurons': self.brain.get_active_neuron_count()
        }
        
        step_time = time.time() - step_start
        self.step_times.append(step_time)
        
        # Combine metrics
        step_metrics = {
            'step': step_num,
            'spike_count_change': after_metrics['spike_count'] - before_metrics['spike_count'],
            'firing_rate_change': after_metrics['firing_rate'] - before_metrics['firing_rate'],
            'step_time': step_time,
            'spike_count': after_metrics['spike_count'],
            'firing_rate': after_metrics['firing_rate'],
            'active_neurons': after_metrics['active_neurons']
        }
        
        self.metrics.append(step_metrics)
        
        # Print progress every 100 steps
        if step_num % 100 == 0:
            print(f"  Step {step_num}: {after_metrics['spike_count']} spikes, "
                  f"{after_metrics['firing_rate']:.2f} Hz avg, "
                  f"{step_time*1000:.2f} ms step")
    
    def stop_monitoring(self):
        """Stop monitoring and return statistics."""
        if not self.is_running:
            return self.get_statistics()
        
        self.is_running = False
        
        stats = self.get_statistics()
        
        print(f"\n✓ Brain Monitoring Summary:")
        print(f"  Total steps monitored: {len(self.metrics)}")
        print(f"  Total time: {stats['total_time']:.2f} seconds")
        print(f"  Average step time: {stats['avg_step_time']*1000:.2f} ms")
        print(f"  Steps per second: {stats['steps_per_second']:.2f}")
        print(f"  Final spike count: {stats['final_spike_count']}")
        print(f"  Final firing rate: {stats['final_firing_rate']:.2f} Hz")
        print(f"  Spike count change: {stats['total_spike_change']}")
        
        return stats
    
    def get_statistics(self):
        """Get current monitoring statistics."""
        if not self.metrics:
            return {
                'total_time': 0,
                'avg_step_time': 0,
                'steps_per_second': 0,
                'final_spike_count': 0,
                'final_firing_rate': 0,
                'total_spike_change': 0,
                'spike_rate': 0
            }
        
        total_time = time.time() - self.start_time if self.start_time else 0
        
        if self.step_times:
            avg_step_time = sum(self.step_times) / len(self.step_times)
            steps_per_second = len(self.step_times) / sum(self.step_times) if sum(self.step_times) > 0 else 0
        else:
            avg_step_time = 0
            steps_per_second = 0
        
        final_metrics = self.metrics[-1] if self.metrics else {}
        
        # Calculate spike rate (spikes per second)
        spike_rate = 0
        if total_time > 0 and final_metrics.get('spike_count', 0) > 0:
            spike_rate = final_metrics['spike_count'] / total_time
        
        return {
            'total_time': total_time,
            'avg_step_time': avg_step_time,
            'steps_per_second': steps_per_second,
            'final_spike_count': final_metrics.get('spike_count', 0),
            'final_firing_rate': final_metrics.get('firing_rate', 0),
            'total_spike_change': sum(m.get('spike_count_change', 0) for m in self.metrics),
            'spike_rate': spike_rate
        }
    
    def get_spike_times(self):
        """Get spike times (simplified - would need actual spike tracking)."""
        # This would require actual spike tracking infrastructure
        return []
    
    def analyze_firing_patterns(self):
        """Analyze firing patterns."""
        spike_times = self.get_spike_times()
        duration = self.get_statistics()['total_time']
        
        if not spike_times or duration <= 0:
            return {'rate': 0, 'consistency': 0, 'pattern': 'none'}
        
        # Simple analysis
        rates = []
        for i in range(len(spike_times) - 1):
            interval = spike_times[i + 1] - spike_times[i]
            if interval > 0:
                rates.append(1.0 / interval)
        
        if not rates:
            return {'rate': 0, 'consistency': 0, 'pattern': 'none'}
        
        avg_rate = sum(rates) / len(rates)
        rate_variance = np.var(rates) if len(rates) > 1 else 0
        consistency = max(0, 1 - rate_variance / (avg_rate * avg_rate + 0.001))
        
        if avg_rate < 0.1:
            pattern = 'quiescent'
        elif avg_rate < 1.0:
            pattern = 'irregular'
        elif rate_variance < avg_rate * 0.1:
            pattern = 'burst'
        else:
            pattern = 'regular'
        
        return {
            'avg_firing_rate': avg_rate,
            'consistency': consistency,
            'pattern': pattern,
            'total_spikes': len(spike_times)
        }

# === Neural Recorder ===

class NeuralRecorder:
    """
    Record and playback neural activity patterns.
    """
    
    def __init__(self, brain: EnhancedBrain):
        self.brain = brain
        self.recordings = {}
        self.current_recording = None
    
    def start_recording(self, name: str = "recording"):
        """Start recording neural activity."""
        if self.current_recording:
            raise ResourceError(f"Already recording under name '{self.current_recording['name']}'")
        
        self.current_recording = {
            'brain': self.brain,
            'name': name,
            'data': [],
            'start_time': time.time()
        }
        
        print(f"✓ Started recording '{name}'")
        return self.current_recording
    
    def record_step(self, step_num: int):
        """Record a single step."""
        if not self.current_recording:
            raise ResourceError("No active recording")
        
        step_data = {
            'step': step_num,
            'timestamp': time.time() - self.current_recording['start_time'],
            'spike_count': self.brain.get_total_spike_count(),
            'firing_rate': self.brain.get_average_firing_rate(),
            'active_neurons': self.brain.get_active_neuron_count(),
            'total_neurons': self.brain.get_total_neuron_count(),
            'total_synapses': self.brain.get_total_synapse_count(),
            'development_stage': self.brain.get_development_stage()
        }
        
        self.current_recording['data'].append(step_data)
        self.brain.step(step_num)
    
    def stop_recording(self):
        """Stop recording and return the recording."""
        if not self.current_recording:
            raise ResourceError("No active recording")
        
        recording = self.current_recording
        self.recordings[recording['name']] = recording
        
        duration = time.time() - recording['start_time']
        print(f"✓ Stopped recording '{recording['name']}' "
              f"(recorded {len(recording['data'])} steps, {duration:.2f}s)")
        
        self.current_recording = None
        return recording
    
    def play_recording(self, name: str):
        """Play back a recorded neural pattern."""
        if name not in self.recordings:
            raise ValueError(f"Recording '{name}' not found")
        
        recording = self.recordings[name]
        brain = recording['brain']
        data = recording['data']
        
        print(f"✓ Playing back recording '{name}' ({len(data)} steps)")
        
        for step_data in data:
            brain.step(step_data['step'])
        
        return brain
    
    def save_recording(self, name: str, filepath: str):
        """Save a recording to file."""
        if name not in self.recordings:
            raise ValueError(f"Recording '{name}' not found")
        
        recording = self.recordings[name]
        
        # Convert to JSON-serializable format
        serializable_data = {
            'name': recording['name'],
            'start_time': recording['start_time'],
            'duration': time.time() - recording['start_time'],
            'data': recording['data']
        }
        
        with open(filepath, 'w') as f:
            json.dump(serializable_data, f, indent=2)
        
        print(f"✓ Saved recording '{name}' to '{filepath}'")
    
    def load_recording(self, filepath: str):
        """Load a recording from file."""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        name = data['name']
        if name in self.recordings:
            print(f"⚠ Recording '{name}' already exists, overwriting")
        
        # Create a new recording object
        recording = {
            'name': data['name'],
            'start_time': data['start_time'],
            'data': data['data']
        }
        
        self.recordings[name] = recording
        print(f"✓ Loaded recording '{name}' from '{filepath}' "
              f"({len(recording['data'])} steps)")
        
        return recording

# === Enhanced Agent ===

class EnhancedAgent:
    """
    Enhanced agent with advanced features and better control.
    """
    
    def __init__(self, brain: EnhancedBrain, world=None):
        self.brain = brain
        self.world = world or EnhancedWorld()
        self.agent = pynlm.createAgentBrain(brain._brain)
        self.agent.initialize(self.world._world)
        
        # Enable advanced features
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        # Performance tracking
        self.action_history = []
        self.reward_history = []
        self.step_count = 0
        self.episode_start_time = None
    
    def run_episode(self, max_steps: int = 1000, 
                    render_callback: Optional[Callable] = None,
                    terminate_callback: Optional[Callable] = None):
        """
        Run a complete simulation episode with advanced control.
        
        Args:
            max_steps: Maximum number of steps
            render_callback: Callback for visualization
            terminate_callback: Callback to check termination conditions
        """
        self.episode_start_time = time.time()
        self.action_history = []
        self.reward_history = []
        self.step_count = 0
        
        print(f"✓ Starting episode (max_steps={max_steps})")
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.get_sensory_percept()
            self.agent.processSensoryInput(percept)
            
            # Brain processes
            self.brain.step(step)
            
            # Decode action
            action = self.agent.decodeMotorCommand()
            
            # Apply action in world
            result = self.world.apply_motor_command(action, self.world.get_simulation_time())
            
            # Record action
            action_record = {
                'step': step,
                'action': action,
                'reward': result.reward,
                'success': result.success,
                'timestamp': time.time() - self.episode_start_time
            }
            self.action_history.append(action_record)
            
            # Apply reward modulation
            self.agent.applyRewardModulation(result.reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Optional rendering
            if render_callback:
                render_callback(step, percept, action, result)
            
            # Check termination conditions
            if terminate_callback and terminate_callback(self):
                print(f"  Episode terminated by callback at step {step}")
                break
            
            # Auto-termination based on activity
            if self.brain.get_total_spike_count() > 10000:
                print(f"  Terminating due to high activity ({self.brain.get_total_spike_count()} spikes)")
                break
            
            self.step_count += 1
        
        episode_duration = time.time() - self.episode_start_time
        print(f"✓ Episode completed: {self.step_count} steps, "
              f"{len(self.action_history)} actions taken in {episode_duration:.2f}s")
        
        return self.get_episode_stats()
    
    def get_episode_stats(self) -> Dict[str, Any]:
        """Get statistics about the episode."""
        if not self.action_history:
            return {}
        
        rewards = [a['reward'] for a in self.action_history]
        successes = [a['success'] for a in self.action_history]
        actions = [a['action'] for a in self.action_history]
        
        # Count actions
        action_counts = {}
        for action in actions:
            action_counts[action] = action_counts.get(action, 0) + 1
        
        stats = {
            'total_steps': self.step_count,
            'total_actions': len(self.action_history),
            'total_reward': sum(rewards),
            'avg_reward': sum(rewards) / len(rewards) if rewards else 0,
            'success_rate': sum(successes) / len(successes) if successes else 0,
            'max_reward': max(rewards) if rewards else 0,
            'min_reward': min(rewards) if rewards else 0,
            'final_spike_count': self.brain.get_total_spike_count(),
            'final_firing_rate': self.brain.get_average_firing_rate(),
            'action_distribution': action_counts,
            'duration': time.time() - self.episode_start_time if self.episode_start_time else 0
        }
        
        print(f"Episode Stats:")
        print(f"  Total steps: {stats['total_steps']}")
        print(f"  Total actions: {stats['total_actions']}")
        print(f"  Total reward: {stats['total_reward']:.2f}")
        print(f"  Average reward: {stats['avg_reward']:.3f}")
        print(f"  Success rate: {stats['success_rate']*100:.1f}%")
        print(f"  Final spike count: {stats['final_spike_count']}")
        print(f"  Final firing rate: {stats['final_firing_rate']:.2f} Hz")
        print(f"  Duration: {stats['duration']:.2f}s")
        print(f"  Action distribution: {stats['action_distribution']}")
        
        return stats

# === Enhanced World ===

class EnhancedWorld:
    """
    Enhanced world class with better interfaces and features.
    """
    
    def __init__(self):
        self._world = pynlm.createSimpleWorld()
        self._world.configure(20, 20, 16, 16)
        self._world.reset()
    
    def update(self, timestep: float = 0.1):
        """Update world state."""
        self._world.update(timestep)
    
    def get_sensory_percept(self):
        """Get current sensory percept."""
        return self._world.getSensoryPercept()
    
    def apply_motor_command(self, action, current_time: float = 0.0):
        """Apply motor command."""
        return self._world.applyMotorCommand(action, current_time)
    
    def get_simulation_time(self) -> float:
        """Get simulation time."""
        return self._world.getSimulationTime()
    
    def get_agent_body(self):
        """Get agent body state."""
        return self._world.getAgentBody()
    
    def add_object(self, obj):
        """Add object to world."""
        self._world.addObject(obj)
    
    def remove_object(self, x: float, y: float):
        """Remove object at position."""
        self._world.removeObject(x, y)
    
    def configure(self, width: float, height: float, 
                  vision_width: int, vision_height: int):
        """Configure world dimensions."""
        self._world.configure(width, height, vision_width, vision_height)

# === Analysis and Visualization Tools ===

class BrainAnalyzer:
    """
    Analyze brain data and generate insights.
    """
    
    @staticmethod
    def compare_brains(brain1: EnhancedBrain, brain2: EnhancedBrain, 
                       metric: str = 'spiking') -> Dict[str, Any]:
        """
        Compare two brains based on specified metric.
        
        Args:
            brain1: First brain
            brain2: Second brain
            metric: Metric to compare ('spiking', 'firing_rate', 'activity', 'synapses')
        """
        if metric == 'spiking':
            val1 = brain1.get_total_spike_count()
            val2 = brain2.get_total_spike_count()
        elif metric == 'firing_rate':
            val1 = brain1.get_average_firing_rate()
            val2 = brain2.get_average_firing_rate()
        elif metric == 'activity':
            val1 = brain1.get_active_neuron_count()
            val2 = brain2.get_active_neuron_count()
        elif metric == 'synapses':
            val1 = brain1.get_total_synapse_count()
            val2 = brain2.get_total_synapse_count()
        else:
            raise ValueError(f"Unknown metric: {metric}")
        
        difference = abs(val1 - val2)
        average = (val1 + val2) / 2
        similarity = max(0, 1 - difference / (average + 1))
        
        return {
            'metric': metric,
            'brain1_value': val1,
            'brain2_value': val2,
            'difference': difference,
            'similarity': similarity,
            'better_brain': 1 if val1 > val2 else 2 if val2 > val1 else 0
        }
    
    @staticmethod
    def analyze_firing_patterns(spike_times: List[float], duration: float) -> Dict[str, Any]:
        """
        Analyze firing rate patterns over time.
        
        Args:
            spike_times: List of spike times
            duration: Duration of recording
        """
        if not spike_times:
            return {'rate': 0, 'consistency': 0, 'pattern': 'none'}
        
        # Calculate instantaneous firing rate
        rates = []
        for i in range(len(spike_times) - 1):
            interval = spike_times[i + 1] - spike_times[i]
            if interval > 0:
                rates.append(1.0 / interval)
        
        if not rates:
            return {'rate': 0, 'consistency': 0, 'pattern': 'none'}
        
        avg_rate = sum(rates) / len(rates)
        rate_variance = np.var(rates) if len(rates) > 1 else 0
        consistency = max(0, 1 - rate_variance / (avg_rate * avg_rate + 0.001))
        
        # Pattern detection
        if avg_rate < 0.1:
            pattern = 'quiescent'
        elif avg_rate < 1.0:
            pattern = 'irregular'
        elif rate_variance < avg_rate * 0.1:
            pattern = 'burst'
        else:
            pattern = 'regular'
        
        return {
            'avg_firing_rate': avg_rate,
            'consistency': consistency,
            'pattern': pattern,
            'total_spikes': len(spike_times),
            'irregularity': rate_variance / (avg_rate * avg_rate + 0.001)
        }

# === Configuration Manager ===

class ConfigManager:
    """
    Manage multiple configurations and run experiments.
    """
    
    def __init__(self):
        self.configurations = {}
        self.current_config = None
    
    def add_config(self, name: str, params: Dict[str, Any]):
        """
        Add a new configuration.
        
        Args:
            name: Configuration name
            params: Configuration parameters
        """
        config = EnhancedConfig()
        
        # Apply parameters
        for key, value in params.items():
            if isinstance(value, dict):
                # Nested parameters
                for subkey, subvalue in value.items():
                    config.set(f"{key}.{subkey}", subvalue)
            else:
                config.set(key, value)
        
        self.configurations[name] = config
        print(f"✓ Added configuration '{name}'")
        return config
    
    def set_current(self, name: str):
        """
        Set the current configuration.
        
        Args:
            name: Configuration name
        """
        if name not in self.configurations:
            raise ValueError(f"Configuration '{name}' not found")
        
        self.current_config = self.configurations[name]
        print(f"✓ Current configuration set to '{name}'")
        return self.current_config
    
    def run_experiments(self, brain_factory: Callable, 
                       experiment_func: Callable,
                       iterations: int = 5,
                       experiment_name: str = "experiment") -> List[Dict[str, Any]]:
        """
        Run multiple experiments with different configurations.
        
        Args:
            brain_factory: Function that creates a brain from config
            experiment_func: Function that runs the experiment
            iterations: Number of iterations per configuration
            experiment_name: Name of the experiment
        """
        if not self.current_config:
            raise RuntimeError("No current configuration set")
        
        results = []
        
        print(f"✓ Running experiment '{experiment_name}' ({iterations} iterations)")
        
        for i in range(iterations):
            # Create brain with current config
            brain = brain_factory(self.current_config)
            brain.initialize()
            
            # Run experiment
            result = experiment_func(brain, i)
            result['iteration'] = i
            result['config_name'] = list(self.configurations.keys())[
                list(self.configurations.values()).index(self.current_config)
            ]
            
            results.append(result)
            
            # Reset for next iteration
            brain.reset()
        
        print(f"✓ Experiment '{experiment_name}' completed")
        return results

# === Main API Functions ===

def create_enhanced_config() -> EnhancedConfig:
    """Create an enhanced configuration."""
    return EnhancedConfig()

def create_enhanced_brain(config=None) -> EnhancedBrain:
    """Create an enhanced brain."""
    return EnhancedBrain(config)

def create_enhanced_world() -> EnhancedWorld:
    """Create an enhanced world."""
    return EnhancedWorld()

def create_enhanced_agent(brain: EnhancedBrain, world=None) -> EnhancedAgent:
    """Create an enhanced agent."""
    return EnhancedAgent(brain, world)

# === Example Usage ===

def example_basic_usage():
    """Example of basic usage."""
    print("Example 1: Basic Usage")
    print("-" * 40)
    
    with create_enhanced_brain() as brain:
        for i in range(100):
            brain.step(i)
        
        print(f"Spike count: {brain.get_total_spike_count()}")
        print(f"Average firing rate: {brain.get_average_firing_rate():.2f} Hz")

def example_advanced_simulation():
    """Example of advanced simulation."""
    print("\nExample 2: Advanced Simulation")
    print("-" * 40)
    
    # Create agent with monitoring
    brain = create_enhanced_brain()
    world = create_enhanced_world()
    agent = create_enhanced_agent(brain, world)
    
    # Run episode with monitoring
    stats = agent.run_episode(max_steps=200)
    print(f"Episode completed with {stats['total_reward']:.2f} total reward")

def example_configuration_management():
    """Example of configuration management."""
    print("\nExample 3: Configuration Management")
    print("-" * 40)
    
    # Create configuration manager
    config_manager = ConfigManager()
    
    # Add configurations
    config1 = {
        'neuron_count': 500,
        'development_enabled': True,
        'curiosity_enabled': True
    }
    
    config2 = {
        'neuron_count': 1000,
        'development_enabled': False,
        'curiosity_enabled': True
    }
    
    config_manager.add_config('config1', config1)
    config_manager.add_config('config2', config2)
    
    # Set current config
    config_manager.set_current('config1')
    
    # Run experiments
    def brain_factory(config):
        return create_enhanced_brain(config)
    
    def experiment_func(brain, iteration):
        brain.run_simulation(100)
        return {
            'spike_count': brain.get_total_spike_count(),
            'firing_rate': brain.get_average_firing_rate()
        }
    
    results = config_manager.run_experiments(
        brain_factory, experiment_func, iterations=3
    )
    
    print(f"Experiment results: {len(results)} iterations completed")

def example_recording_playback():
    """Example of neural recording and playback."""
    print("\nExample 4: Recording and Playback")
    print("-" * 40)
    
    brain = create_enhanced_brain()
    
    # Start recording
    recorder = brain.start_recording("test_recording")
    
    # Run simulation with recording
    for i in range(50):
        recorder.record_step(i)
    
    # Stop recording
    recording = brain.stop_recording()
    
    # Play back recording
    brain.play_recording("test_recording")
    
    print(f"Recording played back with {len(recording['data'])} steps")

# === Version and Info ===

__version__ = "2.0.0"
__author__ = "Enhanced NLM Team"
__description__ = "Enhanced Python API for NLM Neural Learning Machine"

# Make the main functions available
__all__ = [
    'EnhancedConfig',
    'EnhancedBrain',
    'EnhancedWorld',
    'EnhancedAgent',
    'BrainMonitor',
    'NeuralRecorder',
    'ConfigManager',
    'BrainAnalyzer',
    'create_enhanced_config',
    'create_enhanced_brain',
    'create_enhanced_world',
    'create_enhanced_agent',
    'example_basic_usage',
    'example_advanced_simulation',
    'example_configuration_management',
    'example_recording_playback'
]

print(f"Enhanced NLM API v{__version__} loaded successfully!")
print(f"Available modules: {len(__all__)} main components")
