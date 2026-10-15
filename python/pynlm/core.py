# Core NLM types and utilities
# This module contains the basic types and utility functions for the NLM framework

import numpy as np
from typing import Optional, List, Tuple, Dict, Any
import json
import time

# Type aliases for better Pythonic usage
NeuronCount = int
SynapseCount = int
PopulationCount = int
Timestep = float
Timestamp = float
MembranePotential = float

class NLMError(Exception):
    """Base exception for NLM errors"""
    pass

class ValidationError(NLMError):
    """Raised when configuration validation fails"""
    pass

class SimulationError(NLMError):
    """Raised when simulation encounters an error"""
    pass

class ConfigError(NLMError):
    """Raised when configuration operations fail"""
    pass

# Factory functions for creating optimized objects
def createOptimizedConfig(**kwargs) -> 'Config':
    """
    Create an optimized configuration for high-performance simulations.
    
    Args:
        **kwargs: Override default optimized settings
        
    Returns:
        Config object with optimized settings
    """
    config = Config()
    
    # Default optimized settings
    default_settings = {
        "performance.optimization_level": "high",
        "memory.pooling": True,
        "neural.dynamics.method": "lfp",
        "plasticity.stdp.enabled": True,
        "plasticity.hebbian.enabled": True,
        "performance.thread_count": -1,  # Use all available cores
        "performance.vectorization": True,
        "brain.boundary_check.enabled": True,
        "memory.consolidation.enabled": True,
        "neuromodulation.dopamine.enabled": True,
        "neuromodulation.curiosity.enabled": True,
        "neuromodulation.novelty.enabled": True,
    }
    
    # Apply defaults
    for key, value in default_settings.items():
        config.set(key, value)
    
    # Apply any user overrides
    for key, value in kwargs.items():
        config.set(key, value)
    
    return config

def validateConfiguration(config: 'Config') -> Tuple[List[str], List[str]]:
    """
    Validate a configuration and return errors and warnings.
    
    Args:
        config: Configuration to validate
        
    Returns:
        Tuple of (errors, warnings)
    """
    errors = []
    warnings = []
    
    # Check for required keys
    if config.has("neural.region_count") and config.get[int64_t]("neural.region_count") <= 0:
        errors.append("neural.region_count must be positive")
    
    if config.has("performance.simulation_steps") and config.get[int64_t]("performance.simulation_steps") < 0:
        errors.append("performance.simulation_steps cannot be negative")
    
    if config.has("brain.boundary_check.enabled") and not config.get[bool]("brain.boundary_check.enabled"):
        warnings.append("Boundary checking is disabled - may cause simulation errors")
    
    # Check for conflicting settings
    if config.has("neuromodulation.dopamine.enabled") and config.has("neuromodulation.curiosity.enabled"):
        if config.get[bool]("neuromodulation.dopamine.enabled") and config.get[bool]("neuromodulation.curiosity.enabled"):
            warnings.append("Both dopamine and curiosity modulation enabled - may cause conflict")
    
    return errors, warnings

def exportConfiguration(config: 'Config', filepath: str) -> bool:
    """
    Export configuration to a JSON file.
    
    Args:
        config: Configuration to export
        filepath: Path to save the JSON file
        
    Returns:
        True if export succeeded, False otherwise
    """
    try:
        return config.saveToFile(filepath)
    except Exception as e:
        raise ConfigError(f"Failed to export configuration: {e}")

def importConfiguration(filepath: str) -> 'Config':
    """
    Import configuration from a JSON file.
    
    Args:
        filepath: Path to the JSON configuration file
        
    Returns:
        Imported configuration object
        
    Raises:
        ConfigError: If import fails
    """
    try:
        config = Config()
        if not config.loadFromFile(filepath):
            raise ConfigError(f"Failed to load configuration from {filepath}")
        return config
    except Exception as e:
        raise ConfigError(f"Failed to import configuration: {e}")

def getDefaultParameters(system_type: str = "default") -> Dict[str, Any]:
    """
    Get default parameters for different system types.
    
    Args:
        system_type: Type of system ('default', 'neural', 'motor', 'cognitive', 'all')
        
    Returns:
        Dictionary of default parameters
    """
    base_params = {
        "neural": {
            "neuron_count": 100,
            "region_count": 5,
            "learning_rate": 0.01,
            "plasticity.enabled": True,
        },
        "motor": {
            "action_count": 10,
            "motor_noise": 0.1,
            "action_selection": "softmax",
        },
        "cognitive": {
            "working_memory_capacity": 20,
            "attention.enabled": True,
            "planning.horizon": 10,
        },
        "all": {}
    }
    
    if system_type == "all":
        result = {}
        for category in base_params:
            if category != "all":
                result.update(base_params[category])
        return result
    elif system_type in base_params:
        return base_params[system_type].copy()
    else:
        return {}

def benchmarkSimulation(brain, world, agent, steps: int, **kwargs) -> Dict[str, Any]:
    """
    Run a simulation and collect performance metrics.
    
    Args:
        brain: Brain object
        world: World object
        agent: AgentBrain object
        steps: Number of simulation steps
        **kwargs: Additional configuration
        
    Returns:
        Dictionary with performance metrics
    """
    start_time = time.time()
    timestep = kwargs.get('timestep', 0.1)
    
    for i in range(steps):
        world.update(timestep)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(i)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.time()
    duration = end_time - start_time
    
    stats = {
        'duration_seconds': duration,
        'steps_per_second': steps / duration if duration > 0 else 0,
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'development_stage': brain.getDevelopmentalStage(),
    }
    
    return stats