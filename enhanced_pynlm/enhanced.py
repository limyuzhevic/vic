# enhanced_pynlm/enhanced.py
"""
Enhanced NLM Python API

This module provides enhanced versions of the NLM Python API with:
• Better error handling and validation
• Context managers for resource safety
• Performance monitoring tools
• Neural recording and playback
• Configuration management
• Advanced agent simulation
• Analysis and visualization tools
"""

import time
import json
import numpy as np
from contextlib import contextmanager
from typing import List, Dict, Optional, Callable, Any

# Import the original NLM bindings
import pynlm as original_pynlm

# === Base Classes ===

class SimulationError(Exception):
    """Base exception for simulation errors."""
    pass

class ConfigurationError(Exception):
    """Exception for configuration errors."""
    pass

class ResourceError(Exception):
    """Exception for resource management errors."""
    pass

# === Enhanced Configuration ===

class EnhancedConfig:
    """
    Enhanced configuration class with validation and better Pythonic interfaces.
    """
    
    def __init__(self, original_config=None):
        if original_config is None:
            self._original_config = original_pynlm.createDefaultConfig()
        else:
            self._original_config = original_config
        
        self._schema = self._get_default_schema()
        self._validation_enabled = True
    
    def _get_default_schema(self):
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
        if not self._validation_enabled:
            return True
        
        if key not in self._schema:
            return True
        
        schema = self._schema[key]
        
        if 'type' in schema:
            expected_type = schema['type']
            if expected_type == int and not isinstance(value, int):
                return False
            elif expected_type == float and not isinstance(value, (int, float)):
                return False
            elif expected_type == bool and not isinstance(value, bool):
                return False
            elif expected_type == str and not isinstance(value, str):
                return False
        
        if 'min' in schema and value < schema['min']:
            return False
        if 'max' in schema and value > schema['max']:
            return False
        
        return True
    
    def set(self, key: str, value: Any, validate: bool = True):
        if validate and not self.validate_value(key, value):
            raise ConfigurationError(f"Invalid value for '{key}': {value}")
        
        self._original_config.set(key, value)
    
    def get(self, key: str, default=None):
        return self._original_config.getOr(key, default)
    
    def get_all(self) -> Dict[str, Any]:
        keys = self._original_config.getKeys()
        return {key: self.get(key) for key in keys}
    
    def to_dict(self) -> Dict[str, Any]:
        return self.get_all()
    
    def from_dict(self, config_dict: Dict[str, Any]):
        for key, value in config_dict.items():
            self.set(key, value, validate=True)
    
    def save(self, filepath: str):
        config_dict = self.to_dict()
        
        with open(filepath, 'w') as f:
            json.dump(config_dict, f, indent=2)
    
    def load(self, filepath: str):
        with open(filepath, 'r') as f:
            config_dict = json.load(f)
        
        self.from_dict(config_dict)
    
    def __repr__(self):
        return f"EnhancedConfig({len(self.get_all())} parameters)"

# === Enhanced Brain ===

class EnhancedBrain:
    """
    Enhanced brain class with context manager support,
    performance monitoring, and advanced APIs.
    """
    
    def __init__(self, config=None):
        self._config = config or EnhancedConfig()
        self._brain = original_pynlm.createBrain(self._config._original_config)
        self._monitor = None
        self._recording = None
        self._is_active = False
    
    def __enter__(self):
        self.initialize()
        self._is_active = True
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.reset()
        self._is_active = False
    
    def initialize(self):
        if self._is_active:
            raise ResourceError("Brain is already active. Call reset() first.")
        
        self._brain.initialize()
        self._is_active = True
        return self
    
    def reset(self):
        if not self._is_active:
            return self
        
        self._brain.reset()
        self._is_active = False
        return self
    
    def step(self, step_num: int, timestep: float = None):
        if not self._is_active:
            raise SimulationError("Brain is not active. Call initialize() first.")
        
        if timestep is None:
            timestep = self._config.get('simulation_timestep', 0.001)
        
        self._brain.step(step_num, timestep)
        
        if self._monitor:
            self._monitor.step(step_num)
        
        return self
    
    def get_total_spike_count(self) -> int:
        return self._brain.getTotalSpikeCount()
    
    def get_average_firing_rate(self) -> float:
        return self._brain.getAverageFiringRate()
    
    def get_firing_neuron_count(self) -> int:
        return self._brain.getFiringNeuronCount()
    
    def get_total_neuron_count(self) -> int:
        return self._brain.getTotalNeuronCount()
    
    def get_total_synapse_count(self) -> int:
        return self._brain.getTotalSynapseCount()
    
    def get_active_neuron_count(self) -> int:
        return self._brain.getActiveNeuronCount()
    
    def get_development_stage(self):
        return self._brain.getDevelopmentalStage()
    
    def get_config(self) -> EnhancedConfig:
        return self._config
    
    def start_monitoring(self):
        self._monitor = BrainMonitor(self)
        self._monitor.start_monitoring()
        return self._monitor
    
    def stop_monitoring(self):
        if self._monitor:
            self._monitor.stop_monitoring()
            self._monitor = None
        return self
    
    def start_recording(self, name: str = "recording"):
        self._recording = NeuralRecorder(self)
        self._recording.start_recording(name)
        return self._recording
    
    def stop_recording(self):
        if self._recording:
            recording = self._recording.stop_recording()
            self._recording = None
            return recording
        return None
    
    def run_simulation(self, num_steps: int, render_callback=None):
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
        self.start_time = time.time()
        self.step_times = []
        self.metrics = []
        self.is_running = True
        return self
    
    def step(self, step_num: int):
        if not self.is_running:
            return
        
        step_start = time.time()
        
        before_metrics = {
            'step': step_num,
            'spike_count': self.brain.get_total_spike_count(),
            'firing_rate': self.brain.get_average_firing_rate(),
            'active_neurons': self.brain.get_active_neuron_count()
        }
        
        self.brain.step(step_num)
        
        after_metrics = {
            'step': step_num,
            'spike_count': self.brain.get_total_spike_count(),
            'firing_rate': self.brain.get_average_firing_rate(),
            'active_neurons': self.brain.get_active_neuron_count()
        }
        
        step_time = time.time() - step_start
        self.step_times.append(step_time)
        
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
    
    def stop_monitoring(self):
        if not self.is_running:
            return self.get_statistics()
        
        self.is_running = False
        return self.get_statistics()
    
    def get_statistics(self):
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
        if self.current_recording:
            raise ResourceError(f"Already recording under name '{self.current_recording['name']}'")
        
        self.current_recording = {
            'brain': self.brain,
            'name': name,
            'data': [],
            'start_time': time.time()
        }
        
        return self.current_recording
    
    def record_step(self, step_num: int):
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
        if not self.current_recording:
            raise ResourceError("No active recording")
        
        recording = self.current_recording
        self.recordings[recording['name']] = recording
        
        self.current_recording = None
        return recording
    
    def play_recording(self, name: str):
        if name not in self.recordings:
            raise ValueError(f"Recording '{name}' not found")
        
        recording = self.recordings[name]
        brain = recording['brain']
        data = recording['data']
        
        for step_data in data:
            brain.step(step_data['step'])
        
        return brain

# === Enhanced World ===

class EnhancedWorld:
    """
    Enhanced world class with better interfaces.
    """
    
    def __init__(self):
        self._world = original_pynlm.createSimpleWorld()
        self._world.configure(20, 20, 16, 16)
        self._world.reset()
    
    def update(self, timestep: float = 0.1):
        self._world.update(timestep)
    
    def get_sensory_percept(self):
        return self._world.getSensoryPercept()
    
    def apply_motor_command(self, action, current_time: float = 0.0):
        return self._world.applyMotorCommand(action, current_time)
    
    def get_simulation_time(self) -> float:
        return self._world.getSimulationTime()
    
    def configure(self, width: float, height: float, 
                  vision_width: int, vision_height: int):
        self._world.configure(width, height, vision_width, vision_height)

# === Enhanced Agent ===

class EnhancedAgent:
    """
    Enhanced agent with advanced features.
    """
    
    def __init__(self, brain: EnhancedBrain, world=None):
        self.brain = brain
        self.world = world or EnhancedWorld()
        self.agent = original_pynlm.createAgentBrain(brain._brain)
        self.agent.initialize(self.world._world)
        
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        self.action_history = []
        self.reward_history = []
        self.step_count = 0
        self.episode_start_time = None
    
    def run_episode(self, max_steps: int = 1000, 
                    render_callback: Optional[Callable] = None,
                    terminate_callback: Optional[Callable] = None):
        self.episode_start_time = time.time()
        self.action_history = []
        self.reward_history = []
        self.step_count = 0
        
        for step in range(max_steps):
            self.world.update(0.1)
            
            percept = self.world.get_sensory_percept()
            self.agent.processSensoryInput(percept)
            
            self.brain.step(step)
            
            action = self.agent.decodeMotorCommand()
            
            result = self.world.apply_motor_command(action, self.world.get_simulation_time())
            
            action_record = {
                'step': step,
                'action': action,
                'reward': result.reward,
                'success': result.success,
                'timestamp': time.time() - self.episode_start_time
            }
            self.action_history.append(action_record)
            
            self.agent.applyRewardModulation(result.reward, 0.0)
            
            self.agent.updateDevelopment(0.1)
            
            if render_callback:
                render_callback(step, percept, action, result)
            
            if terminate_callback and terminate_callback(self):
                break
            
            if self.brain.get_total_spike_count() > 10000:
                break
            
            self.step_count += 1
        
        return self.get_episode_stats()
    
    def get_episode_stats(self):
        if not self.action_history:
            return {}
        
        rewards = [a['reward'] for a in self.action_history]
        successes = [a['success'] for a in self.action_history]
        actions = [a['action'] for a in self.action_history]
        
        action_counts = {}
        for action in actions:
            action_counts[action] = action_counts.get(action, 0) + 1
        
        stats = {
            'total_steps': self.step_count,
            'total_actions': len(self.action_history),
            'total_reward': sum(rewards),
            'avg_reward': sum(rewards) / len(rewards),
            'success_rate': sum(successes) / len(successes),
            'max_reward': max(rewards),
            'min_reward': min(rewards),
            'final_spike_count': self.brain.get_total_spike_count(),
            'final_firing_rate': self.brain.get_average_firing_rate(),
            'action_distribution': action_counts,
            'duration': time.time() - self.episode_start_time if self.episode_start_time else 0
        }
        
        return stats

# === Brain Analyzer ===

class BrainAnalyzer:
    """
    Analyze brain data and generate insights.
    """
    
    @staticmethod
    def compare_brains(brain1: EnhancedBrain, brain2: EnhancedBrain, 
                       metric: str = 'spiking') -> Dict[str, Any]:
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
        if not spike_times:
            return {'rate': 0, 'consistency': 0, 'pattern': 'none'}
        
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
            'total_spikes': len(spike_times),
            'irregularity': rate_variance / (avg_rate * avg_rate + 0.001)
        }

# === Config Manager ===

class ConfigManager:
    """
    Manage multiple configurations and run experiments.
    """
    
    def __init__(self):
        self.configurations = {}
        self.current_config = None
    
    def add_config(self, name: str, params: Dict[str, Any]):
        config = EnhancedConfig()
        
        for key, value in params.items():
            if isinstance(value, dict):
                for subkey, subvalue in value.items():
                    config.set(f"{key}.{subkey}", subvalue)
            else:
                config.set(key, value)
        
        self.configurations[name] = config
        return config
    
    def set_current(self, name: str):
        if name not in self.configurations:
            raise ValueError(f"Configuration '{name}' not found")
        
        self.current_config = self.configurations[name]
        return self.current_config
    
    def run_experiments(self, brain_factory: Callable, 
                       experiment_func: Callable,
                       iterations: int = 5,
                       experiment_name: str = "experiment") -> List[Dict[str, Any]]:
        if not self.current_config:
            raise RuntimeError("No current configuration set")
        
        results = []
        
        for i in range(iterations):
            brain = brain_factory(self.current_config)
            brain.initialize()
            
            result = experiment_func(brain, i)
            result['iteration'] = i
            result['config_name'] = list(self.configurations.keys())[
                list(self.configurations.values()).index(self.current_config)
            ]
            
            results.append(result)
            
            brain.reset()
        
        return results

# === Global Functions ===

def create_enhanced_config() -> EnhancedConfig:
    return EnhancedConfig()

def create_enhanced_brain(config=None) -> EnhancedBrain:
    return EnhancedBrain(config)

def create_enhanced_world() -> EnhancedWorld:
    return EnhancedWorld()

def create_enhanced_agent(brain: EnhancedBrain, world=None) -> EnhancedAgent:
    return EnhancedAgent(brain, world)

# === Quick Usage Functions ===

def quick_simulation(neuron_count: int = 100, steps: int = 100) -> EnhancedBrain:
    config = EnhancedConfig()
    config.set("neuron_count", neuron_count)
    config.set("random_seed", 42)
    
    with EnhancedBrain(config) as brain:
        for step in range(steps):
            brain.step(step)
        
        return brain

# === Module-level exports ===

__all__ = [
    'EnhancedConfig',
    'EnhancedBrain',
    'EnhancedWorld',
    'EnhancedAgent',
    'BrainAnalyzer',
    'ConfigManager',
    'BrainMonitor',
    'NeuralRecorder',
    'SimulationError',
    'ConfigurationError',
    'ResourceError',
    'create_enhanced_config',
    'create_enhanced_brain',
    'create_enhanced_world',
    'create_enhanced_agent',
    'quick_simulation'
]

print("Enhanced NLM Python API v2.0.0 loaded successfully!")
print(f"Available: {len(__all__)} enhanced classes and functions")
