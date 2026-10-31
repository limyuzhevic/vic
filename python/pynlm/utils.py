"""
Utility functions for NLM Python bindings.
"""

import time
import json
import math
import pickle
from typing import Dict, List, Any, Optional, Union, Callable, Tuple
import numpy as np

try:
    from pynlm.bindings import createBrain, createDefaultConfig, createSimpleWorld
    from pynlm.bindings import createAgentBrain
    BINDINGS_AVAILABLE = True
except ImportError:
    BINDINGS_AVAILABLE = False
    print("Warning: C++ bindings not available. Utils module limited.")


class Timer:
    """Simple timer for performance measurement."""
    
    def __init__(self, name: str = "timer"):
        """
        Initialize timer.
        
        Args:
            name: Timer name
        """
        self.name = name
        self.start_time = None
        self.end_time = None
        self.duration = 0.0
    
    def start(self):
        """Start timer."""
        self.start_time = time.time()
    
    def stop(self):
        """Stop timer."""
        if self.start_time:
            self.end_time = time.time()
            self.duration = self.end_time - self.start_time
            return self.duration
        return None
    
    def __enter__(self):
        self.start()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.stop()
    
    def __repr__(self) -> str:
        return f"Timer({self.name}, duration={self.duration:.3f}s)"


class PerformanceProfiler:
    """Performance profiler for simulation steps."""
    
    def __init__(self):
        """Initialize performance profiler."""
        self.step_times = []
        self.memory_usage = []
        self.cpu_usage = []
        self.start_time = time.time()
        self.samples = 0
    
    def start_step(self):
        """Start timing a step."""
        self.step_start = time.time()
    
    def end_step(self):
        """End timing a step."""
        if hasattr(self, 'step_start'):
            step_time = time.time() - self.step_start
            self.step_times.append(step_time)
            self.samples += 1
            return step_time
        return None
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get performance statistics."""
        if not self.step_times:
            return {'samples': 0, 'avg_step_time': 0.0, 'min_step_time': 0.0, 'max_step_time': 0.0}
        
        return {
            'samples': len(self.step_times),
            'avg_step_time': np.mean(self.step_times),
            'min_step_time': np.min(self.step_times),
            'max_step_time': np.max(self.step_times),
            'std_step_time': np.std(self.step_times),
            'steps_per_second': len(self.step_times) / (time.time() - self.start_time) if time.time() > self.start_time else 0,
        }
    
    def reset(self):
        """Reset profiler."""
        self.step_times.clear()
        self.samples = 0
        self.start_time = time.time()
    
    def __repr__(self) -> str:
        stats = self.get_statistics()
        return f"PerformanceProfiler(samples={stats['samples']}, avg_step_time={stats['avg_step_time']:.3f}s)"


class SimulationRecorder:
    """Recording system for simulation data."""
    
    def __init__(self):
        """Initialize simulation recorder."""
        self.recorded_data = []
        self.recording = False
        self.recording_frequency = 1.0  # Record every 1.0 steps
        self.data_handlers = {}
        self.file_writer = None
        self.format = 'json'
    
    def start_recording(self, frequency: float = 1.0, format: str = 'json', filepath: str = None):
        """
        Start recording simulation data.
        
        Args:
            frequency: Recording frequency (steps between recordings)
            format: Data format ('json', 'csv', 'pickle')
            filepath: Output file path (optional)
        """
        self.recording = True
        self.recording_frequency = frequency
        self.format = format
        self.step_counter = 0
        
        if filepath:
            self.file_writer = open(filepath, 'w')
    
    def stop_recording(self):
        """Stop recording simulation data."""
        self.recording = False
        if self.file_writer:
            self.file_writer.close()
            self.file_writer = None
    
    def record(self, data: Dict[str, Any], step: int = None):
        """
        Record simulation data.
        
        Args:
            data: Data to record
            step: Step number (optional)
        """
        if not self.recording:
            return
        
        self.step_counter += 1
        
        if step is None:
            step = self.step_counter
        
        # Add timestamp
        record = {
            'step': step,
            'timestamp': time.time(),
            'data': data,
        }
        
        self.recorded_data.append(record)
        
        # Call any registered handlers
        for handler in self.data_handlers.values():
            try:
                handler(record)
            except Exception as e:
                print(f"Data handler error: {e}")
        
        # Write to file if needed
        if self.file_writer:
            if self.format == 'json':
                json_line = json.dumps(record) + '\n'
                self.file_writer.write(json_line)
            elif self.format == 'csv':
                # Simple CSV (not implemented for nested data)
                pass
            elif self.format == 'pickle':
                pickle.dump(record, self.file_writer)
    
    def add_data_handler(self, name: str, handler: Callable):
        """
        Add data handler.
        
        Args:
            name: Handler name
            handler: Handler function
        """
        self.data_handlers[name] = handler
    
    def remove_data_handler(self, name: str):
        """Remove data handler."""
        if name in self.data_handlers:
            del self.data_handlers[name]
    
    def get_recorded_data(self, step_range: Tuple[int, int] = None) -> List[Dict[str, Any]]:
        """
        Get recorded data.
        
        Args:
            step_range: Step range (start, end) (optional)
        
        Returns:
            Recorded data
        """
        if step_range:
            start, end = step_range
            return [record for record in self.recorded_data if start <= record['step'] <= end]
        else:
            return self.recorded_data.copy()
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get recording statistics."""
        return {
            'total_records': len(self.recorded_data),
            'recording_active': self.recording,
            'recording_frequency': self.recording_frequency,
            'format': self.format,
            'step_range': (self.recorded_data[0]['step'], self.recorded_data[-1]['step']) if self.recorded_data else (0, 0),
        }
    
    def clear(self):
        """Clear recorded data."""
        self.recorded_data.clear()
        self.stop_recording()
    
    def export(self, filepath: str):
        """Export recorded data to file."""
        if self.format == 'json':
            with open(filepath, 'w') as f:
                json.dump(self.recorded_data, f, indent=2)
        elif self.format == 'pickle':
            with open(filepath, 'wb') as f:
                pickle.dump(self.recorded_data, f)
        else:
            raise ValueError(f"Unsupported format: {self.format}")
    
    def import_data(self, filepath: str):
        """Import recorded data from file."""
        if self.format == 'json':
            with open(filepath, 'r') as f:
                self.recorded_data = json.load(f)
        elif self.format == 'pickle':
            with open(filepath, 'rb') as f:
                self.recorded_data = pickle.load(f)
        else:
            raise ValueError(f"Unsupported format: {self.format}")
    
    def __repr__(self) -> str:
        return f"SimulationRecorder(records={len(self.recorded_data)}, active={self.recording})"


class ConfigurationManager:
    """Configuration management system."""
    
    def __init__(self, config_file: str = "nlm_config.json"):
        """
        Initialize configuration manager.
        
        Args:
            config_file: Configuration file path
        """
        self.config_file = config_file
        self.config = {}
        self.config_history = []
        self.auto_save = True
        self.validation_rules = {}
        
        # Load configuration
        self.load_config()
    
    def load_config(self, filepath: str = None):
        """
        Load configuration from file.
        
        Args:
            filepath: Configuration file path (uses default if None)
        """
        if filepath:
            self.config_file = filepath
        
        try:
            with open(self.config_file, 'r') as f:
                self.config = json.load(f)
        except (FileNotFoundError, json.JSONDecodeError):
            self.config = self._get_default_config()
            self.save_config()
    
    def save_config(self, filepath: str = None):
        """
        Save configuration to file.
        
        Args:
            filepath: Configuration file path (uses default if None)
        """
        if filepath:
            self.config_file = filepath
        
        with open(self.config_file, 'w') as f:
            json.dump(self.config, f, indent=2)
        
        # Save to history
        self.config_history.append({
            'timestamp': time.time(),
            'config': self.config.copy(),
        })
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get configuration value.
        
        Args:
            key: Configuration key
            default: Default value
        
        Returns:
            Configuration value
        """
        keys = key.split('.')
        value = self.config
        
        for k in keys:
            if isinstance(value, dict) and k in value:
                value = value[k]
            else:
                return default
        
        return value
    
    def set(self, key: str, value: Any, validate: bool = True):
        """
        Set configuration value.
        
        Args:
            key: Configuration key
            value: Configuration value
            validate: Whether to validate
        """
        keys = key.split('.')
        
        # Navigate to parent
        parent = self.config
        for k in keys[:-1]:
            if k not in parent:
                parent[k] = {}
            parent = parent[k]
        
        # Set value
        parent[keys[-1]] = value
        
        # Validate if requested
        if validate:
            self.validate()
        
        # Auto-save
        if self.auto_save:
            self.save_config()
    
    def validate(self) -> List[str]:
        """
        Validate configuration.
        
        Returns:
            List of validation errors
        """
        errors = []
        
        for rule_name, rule in self.validation_rules.items():
            try:
                if 'check' in rule:
                    if not rule['check'](self.config):
                        errors.append(f"Validation failed: {rule_name}")
            except Exception as e:
                errors.append(f"Validation error in {rule_name}: {e}")
        
        return errors
    
    def add_validation_rule(self, name: str, check_func: Callable, description: str = ""):
        """
        Add validation rule.
        
        Args:
            name: Rule name
            check_func: Validation function
            description: Rule description
        """
        self.validation_rules[name] = {
            'check': check_func,
            'description': description,
        }
    
    def reset_to_defaults(self):
        """Reset configuration to defaults."""
        self.config = self._get_default_config()
        self.save_config()
    
    def get_config_history(self, limit: int = 10) -> List[Dict[str, Any]]:
        """
        Get configuration history.
        
        Args:
            limit: Maximum number of history entries
        
        Returns:
            Configuration history
        """
        return self.config_history[-limit:]
    
    def _get_default_config(self) -> Dict[str, Any]:
        """Get default configuration."""
        return {
            'simulation': {
                'steps': 1000,
                'timestep': 0.1,
                'max_neurons': 1000,
                'max_synapses': 5000,
            },
            'brain': {
                'neuron_count': 1000,
                'synapse_density': 0.1,
                'connection_probability': 0.05,
                'initial_weight_mean': 0.5,
                'initial_weight_std': 0.1,
                'v_thresh': -50.0,
                'v_rest': -70.0,
                'v_reset': -75.0,
                'tau_mem': 20.0,
                'tau_ref': 2.0,
            },
            'plasticity': {
                'stdp': {
                    'enable': True,
                    'learning_rate': 0.001,
                    'tau_plus': 20.0,
                    'tau_minus': 20.0,
                },
                'hebbian': {
                    'enable': True,
                },
                'structural': {
                    'enable': True,
                },
            },
            'neuromod': {
                'dopamine': {
                    'scale': 1.0,
                },
                'curiosity': {
                    'enable': True,
                },
                'novelty': {
                    'enable': True,
                },
            },
            'world': {
                'width': 20,
                'height': 20,
                'vision_width': 8,
                'vision_height': 8,
                'max_energy': 100.0,
                'energy_decay_rate': 0.01,
            },
            'agent': {
                'reward_modulation': True,
                'structural_plasticity': True,
                'development': True,
                'curiosity': True,
            },
        }
    
    def merge_config(self, other_config: Dict[str, Any]):
        """
        Merge with another configuration.
        
        Args:
            other_config: Other configuration
        """
        def merge_dicts(d1, d2):
            for key, value in d2.items():
                if key in d1 and isinstance(d1[key], dict) and isinstance(value, dict):
                    merge_dicts(d1[key], value)
                else:
                    d1[key] = value
        
        merge_dicts(self.config, other_config)
        self.save_config()
    
    def get_section(self, section: str) -> Dict[str, Any]:
        """Get configuration section."""
        return self.config.get(section, {})
    
    def set_section(self, section: str, values: Dict[str, Any]):
        """Set configuration section."""
        for key, value in values.items():
            self.set(f"{section}.{key}", value)
    
    def __repr__(self) -> str:
        return f"ConfigurationManager(config_file={self.config_file}, keys={len(self.config)})"


class RandomSeedManager:
    """Random seed management system."""
    
    def __init__(self):
        """Initialize random seed manager."""
        self.seeds = {}
        self.default_seed = 42
        self.random_state = None
    
    def set_seed(self, key: str = 'default', seed: int = None):
        """
        Set random seed.
        
        Args:
            key: Seed key
            seed: Seed value (uses default if None)
        """
        if seed is None:
            seed = self.default_seed
        
        self.seeds[key] = seed
        
        # Set numpy seed
        np.random.seed(seed)
    
    def get_seed(self, key: str = 'default') -> int:
        """
        Get random seed.
        
        Args:
            key: Seed key
        
        Returns:
            Seed value
        """
        return self.seeds.get(key, self.default_seed)
    
    def reset_all(self):
        """Reset all seeds to defaults."""
        self.seeds.clear()
        self.set_seed()
    
    def get_random_state(self) -> Any:
        """Get random state."""
        return self.random_state
    
    def set_random_state(self, state: Any):
        """Set random state."""
        self.random_state = state
        if state is not None:
            np.random.set_state(state)
    
    def __repr__(self) -> str:
        return f"RandomSeedManager(seeds={len(self.seeds)}, default={self.default_seed})"


class BatchProcessor:
    """Batch processing system."""
    
    def __init__(self):
        """Initialize batch processor."""
        self.tasks = []
        self.results = []
        self.is_processing = False
        self.max_workers = 1
    
    def add_task(self, task: Callable, *args, **kwargs):
        """
        Add task to batch.
        
        Args:
            task: Task function
            *args: Task arguments
            **kwargs: Task keyword arguments
        """
        self.tasks.append({
            'function': task,
            'args': args,
            'kwargs': kwargs,
            'status': 'pending',
            'result': None,
            'error': None,
        })
    
    def process_batch(self):
        """Process all tasks in batch."""
        if self.is_processing:
            return
        
        self.is_processing = True
        
        for task_info in self.tasks:
            try:
                task_info['status'] = 'processing'
                task_info['result'] = task_info['function'](*task_info['args'], **task_info['kwargs'])
                task_info['status'] = 'completed'
                self.results.append(task_info)
            except Exception as e:
                task_info['status'] = 'error'
                task_info['error'] = str(e)
                self.results.append(task_info)
        
        self.is_processing = False
    
    def get_results(self) -> List[Dict[str, Any]]:
        """Get processing results."""
        return self.results.copy()
    
    def get_completed_tasks(self) -> List[Dict[str, Any]]:
        """Get completed tasks."""
        return [t for t in self.tasks if t['status'] == 'completed']
    
    def get_pending_tasks(self) -> List[Dict[str, Any]]:
        """Get pending tasks."""
        return [t for t in self.tasks if t['status'] == 'pending']
    
    def get_failed_tasks(self) -> List[Dict[str, Any]]:
        """Get failed tasks."""
        return [t for t in self.tasks if t['status'] == 'error']
    
    def clear(self):
        """Clear all tasks."""
        self.tasks.clear()
        self.results.clear()
    
    def __repr__(self) -> str:
        return f"BatchProcessor(tasks={len(self.tasks)}, completed={len(self.get_completed_tasks())}, failed={len(self.get_failed_tasks())})"


def create_timer(name: str = "timer") -> Timer:
    """Create timer."""
    return Timer(name)


def create_performance_profiler() -> PerformanceProfiler:
    """Create performance profiler."""
    return PerformanceProfiler()


def create_simulation_recorder() -> SimulationRecorder:
    """Create simulation recorder."""
    return SimulationRecorder()


def create_configuration_manager(config_file: str = "nlm_config.json") -> ConfigurationManager:
    """Create configuration manager."""
    return ConfigurationManager(config_file)


def create_random_seed_manager() -> RandomSeedManager:
    """Create random seed manager."""
    return RandomSeedManager()


def create_batch_processor() -> BatchProcessor:
    """Create batch processor."""
    return BatchProcessor()


def benchmark_simulation(brain, world, agent, steps: int = 100):
    """
    Benchmark simulation performance.
    
    Args:
        brain: Brain object
        world: World object
        agent: AgentBrain object
        steps: Number of simulation steps
    
    Returns:
        dict: Performance metrics
    """
    import time
    
    start_time = time.time()
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        agent.applyRewardModulation(0.0, 0.0)
    
    elapsed = time.time() - start_time
    return {
        'steps': steps,
        'elapsed_time': elapsed,
        'steps_per_second': steps / elapsed,
        'average_step_time': elapsed / steps,
    }


def validate_simulation_config(config: Dict[str, Any]) -> List[str]:
    """
    Validate simulation configuration.
    
    Args:
        config: Configuration to validate
    
    Returns:
        List of validation errors
    """
    errors = []
    
    # Check required sections
    required_sections = ['simulation', 'brain', 'world', 'agent']
    for section in required_sections:
        if section not in config:
            errors.append(f"Missing required section: {section}")
    
    # Check brain configuration
    if 'brain' in config:
        brain_config = config['brain']
        if 'neuron_count' not in brain_config:
            errors.append("Missing brain.neuron_count")
        elif brain_config['neuron_count'] <= 0:
            errors.append("brain.neuron_count must be positive")
        
        if 'synapse_density' not in brain_config:
            errors.append("Missing brain.synapse_density")
        elif not (0 <= brain_config['synapse_density'] <= 1):
            errors.append("brain.synapse_density must be between 0 and 1")
    
    # Check world configuration
    if 'world' in config:
        world_config = config['world']
        if 'width' not in world_config or world_config['width'] <= 0:
            errors.append("world.width must be positive")
        if 'height' not in world_config or world_config['height'] <= 0:
            errors.append("world.height must be positive")
    
    return errors


def create_preset_config(name: str) -> Dict[str, Any]:
    """
    Create preset configuration.
    
    Args:
        name: Preset name ('simple', 'complex', 'balanced')
    
    Returns:
        Configuration dictionary
    """
    presets = {
        'simple': {
            'simulation': {'steps': 100, 'timestep': 0.1},
            'brain': {'neuron_count': 500, 'synapse_density': 0.05},
            'world': {'width': 10, 'height': 10, 'vision_width': 4, 'vision_height': 4},
            'agent': {'reward_modulation': False, 'curiosity': False},
        },
        'complex': {
            'simulation': {'steps': 1000, 'timestep': 0.05},
            'brain': {'neuron_count': 5000, 'synapse_density': 0.2},
            'world': {'width': 50, 'height': 50, 'vision_width': 16, 'vision_height': 16},
            'agent': {'reward_modulation': True, 'structural_plasticity': True, 'development': True, 'curiosity': True},
        },
        'balanced': {
            'simulation': {'steps': 500, 'timestep': 0.1},
            'brain': {'neuron_count': 2000, 'synapse_density': 0.1},
            'world': {'width': 20, 'height': 20, 'vision_width': 8, 'vision_height': 8},
            'agent': {'reward_modulation': True, 'structural_plasticity': True, 'development': True, 'curiosity': True},
        },
    }
    
    return presets.get(name, presets['simple'])


def export_simulation_data(brain, world, agent, steps: int = 0, filepath: str = "nlm_simulation_data.json"):
    """Export simulation data to file."""
    import json
    
    data = {
        'brain_stats': {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'development_stage': str(brain.getDevelopmentalStage()),
        },
        'world_stats': {
            'width': world.getWidth(),
            'height': world.getHeight(),
            'simulation_time': world.getSimulationTime(),
            'agent_x': world.getAgentBody().x,
            'agent_y': world.getAgentBody().y,
            'agent_energy': world.getAgentBody().energy,
            'agent_health': world.getAgentBody().health,
        },
        'agent_stats': {
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'development_stage': str(agent.getDevelopmentalStage()),
        },
        'steps_completed': steps,
    }
    
    with open(filepath, 'w') as f:
        json.dump(data, f, indent=2)


def import_simulation_data(filepath: str = "nlm_simulation_data.json") -> Dict[str, Any]:
    """Import simulation data from file."""
    import json
    
    with open(filepath, 'r') as f:
        return json.load(f)
