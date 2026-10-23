import numpy as np
import json
from typing import List, Optional, Union, Dict, Any
from contextlib import contextmanager

class NLMError(Exception):
    """Base exception for NLM errors"""
    pass

class ConfigError(NLMError):
    """Configuration error"""
    pass

class BrainError(NLMError):
    """Brain simulation error"""
    pass

class WorldError(NLMError):
    """World simulation error"""
    pass

def create_config_from_dict(config_dict: Dict[str, Any]) -> Any:
    """Create a configuration from a Python dictionary.
    
    Args:
        config_dict: Dictionary mapping configuration keys to values
        
    Returns:
        Configured brain instance
        
    Example:
        >>> config_dict = {
        ...     'brain.neuron_count': 2000,
        ...     'brain.synapse_density': 0.15,
        ...     'plasticity.stdp.enable': True,
        ...     'neuromod.dopamine.scale': 1.5
        ... }
        >>> brain = create_config_from_dict(config_dict)
    """
    config = create_default_config()
    
    for key, value in config_dict.items():
        if '.' not in key:
            raise ConfigError(f"Invalid config key '{key}'. Keys must use dot notation (e.g., 'brain.neuron_count')")
        
        config.set(key, value)
    
    return config

def save_config_as_json(config: Any, filepath: str) -> bool:
    """Save configuration as JSON file with enhanced formatting.
    
    Args:
        config: Configuration object
        filepath: Path to save JSON file
        
    Returns:
        True if successful, False otherwise
        
    Example:
        >>> config = create_default_config()
        >>> config.set('brain.neuron_count', 5000)
        >>> save_config_as_json(config, 'my_config.json')
    """
    try:
        # Get configuration as structured data
        config_keys = config.getKeys()
        config_data = {}
        
        for key in config_keys:
            # Navigate nested structure
            keys = key.split('.')
            current = config_data
            
            for i, k in enumerate(keys):
                if i == len(keys) - 1:
                    # Last key - get actual value
                    val = config.get(type_value(key))
                    if val is not None:
                        current[k] = val
                else:
                    # Intermediate key - create nested structure
                    if k not in current:
                        current[k] = {}
                    current = current[k]
        
        with open(filepath, 'w') as f:
            json.dump(config_data, f, indent=2)
        
        return True
    except Exception as e:
        raise ConfigError(f"Failed to save config as JSON: {e}")

def load_config_from_json(filepath: str) -> Any:
    """Load configuration from JSON file.
    
    Args:
        filepath: Path to JSON file
        
    Returns:
        Configured brain instance
        
    Example:
        >>> config = load_config_from_json('my_config.json')
        >>> brain = createBrain(config)
    """
    try:
        with open(filepath, 'r') as f:
            config_data = json.load(f)
        
        config = create_default_config()
        
        def flatten_dict(d, parent_key='', sep='.'):
            items = []
            for k, v in d.items():
                new_key = f"{parent_key}{sep}{k}" if parent_key else k
                if isinstance(v, dict):
                    items.extend(flatten_dict(v, new_key, sep=sep).items())
                else:
                    items.append((new_key, v))
            return dict(items)
        
        flat_config = flatten_dict(config_data)
        
        for key, value in flat_config.items():
            config.set(key, value)
        
        return config
    except Exception as e:
        raise ConfigError(f"Failed to load config from JSON: {e}")

def benchmark_brain_step(brain: Any, num_steps: int, timestep: float = 1.0) -> Dict[str, float]:
    """Benchmark brain simulation performance.
    
    Args:
        brain: Brain instance to benchmark
        num_steps: Number of steps to simulate
        timestep: Time step for simulation
        
    Returns:
        Dictionary with benchmark results
        
    Example:
        >>> brain = createBrain(create_default_config())
        >>> results = benchmark_brain_step(brain, 1000)
        >>> print(f"Steps per second: {results['steps_per_second']}")
    """
    import time
    
    start_time = time.time()
    
    for i in range(num_steps):
        brain.step(i)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    return {
        'total_steps': num_steps,
        'elapsed_time': elapsed,
        'steps_per_second': num_steps / elapsed if elapsed > 0 else 0,
        'average_step_time': elapsed / num_steps if num_steps > 0 else 0
    }

@contextmanager
def brain_simulation_context(brain: Any, steps: int, timestep: float = 1.0):
    """Context manager for controlled brain simulation.
    
    Args:
        brain: Brain instance to simulate
        steps: Number of simulation steps
        timestep: Time step for simulation
        
    Yields:
        Dictionary with simulation state information
        
    Example:
        >>> brain = createBrain(create_default_config())
        >>> with brain_simulation_context(brain, 100) as sim:
        ...     print(f"Started simulation with {sim['neuron_count']} neurons")
        ...     # Simulation running...
        ... print(f"Simulation completed in {sim['steps_completed']} steps")
    """
    initial_state = {
        'total_neuron_count': brain.getTotalNeuronCount(),
        'total_synapse_count': brain.getTotalSynapseCount(),
        'total_spike_count': brain.getTotalSpikeCount(),
        'firing_neuron_count': brain.getFiringNeuronCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'developmental_stage': brain.getDevelopmentalStage()
    }
    
    try:
        for i in range(steps):
            brain.step(i)
            yield {
                'step': i,
                'total_neuron_count': brain.getTotalNeuronCount(),
                'total_synapse_count': brain.getTotalSynapseCount(),
                'total_spike_count': brain.getTotalSpikeCount(),
                'firing_neuron_count': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'developmental_stage': brain.getDevelopmentalStage()
            }
    except Exception as e:
        raise BrainError(f"Simulation failed at step {i}: {e}")
    finally:
        final_state = {
            'total_neuron_count': brain.getTotalNeuronCount(),
            'total_synapse_count': brain.getTotalSynapseCount(),
            'total_spike_count': brain.getTotalSpikeCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'developmental_stage': brain.getDevelopmentalStage()
        }
        yield {'initial_state': initial_state, 'final_state': final_state, 'steps_completed': steps}