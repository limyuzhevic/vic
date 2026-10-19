"""
NLM (Neural Learning Machine) Python Package

A sophisticated Python interface for the NLM neural simulation framework,
offering both high-level research tools and detailed control for neural
simulation and experimentation.

Key Features:
- Pythonic API with method chaining and fluent interfaces
- Comprehensive sensory-motor integration with numpy compatibility
- Advanced visualization and plotting capabilities
- Experiment design and execution utilities
- Type hints and comprehensive documentation
- Direct access to brain regions and internal metrics

Example usage:
    >>> from nlm import Brain, Config
    >>> config = Config()
    >>> brain = Brain(config)
    >>> brain.configure(neuron_count=1000, timestep=0.001)
    >>> brain.run_simulation(steps=100)
    >>> state = brain.get_state(as_numpy=True)
    >>> brain.save_brain("simulation_state.nml")
"""

import json
import os
from typing import Dict, Any, Optional, Union, List
import warnings

try:
    import numpy as np
    HAS_NUMPY = True
except ImportError:
    HAS_NUMPY = False

try:
    import matplotlib.pyplot as plt
    HAS_MATPLOTLIB = True
except ImportError:
    HAS_MATPLOTLIB = False

# Import the core C++ extensions
from .pynlm import (
    # Type definitions
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    
    # Core classes
    Config, Vision, Audio, InternalSignals, SensoryInput,
    Action, WorldObject, AgentBody, ActionResult, SensoryPercept,
    SimpleWorld, Brain, AgentBrain,
    
    # Factory functions
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID, INVALID_POPULATION_ID
)

__all__ = [
    # Type definitions
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Core classes
    'Config', 'Vision', 'Audio', 'InternalSignals', 'SensoryInput',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult', 'SensoryPercept',
    'SimpleWorld', 'Brain', 'AgentBrain',
    
    # Factory functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID', 'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    
    # Package utilities
    'NLMError', 'validate_config', 'export_results', 'import_results',
]


class NLMError(Exception):
    """Base exception class for NLM errors"""
    pass


class NLMWarning(Warning):
    """Base warning class for NLM warnings"""
    pass


def validate_config(config: Config, config_type: str = 'default') -> bool:
    """
    Validate configuration parameters.
    
    Args:
        config: The configuration object to validate
        config_type: Type of configuration validation ('default', 'strict', 'basic')
        
    Returns:
        bool: True if configuration is valid, raises exception otherwise
        
    Raises:
        NLMError: If configuration is invalid
    """
    if config_type not in ('default', 'strict', 'basic'):
        raise NLMError(f"Invalid config_type: {config_type}")
    
    # Basic validation checks
    if config.has('neuron_count') and config.getOr('neuron_count', 0) <= 0:
        raise NLMError("neuron_count must be positive")
    
    if config.has('timestep') and config.getOr('timestep', 0.0) <= 0.0:
        raise NLMError("timestep must be positive")
    
    if config.has('simulation_duration') and config.getOr('simulation_duration', 0.0) < 0.0:
        raise NLMError("simulation_duration must be non-negative")
    
    return True


def export_results(results: Dict[str, Any], filename: str, format: str = 'json') -> bool:
    """
    Export simulation results to file.
    
    Args:
        results: Dictionary containing results to export
        filename: Output filename
        format: Export format ('json', 'pickle', 'csv')
        
    Returns:
        bool: True if export successful
        
    Raises:
        NLMError: If export fails
    """
    try:
        if format == 'json':
            with open(filename, 'w') as f:
                json.dump(results, f, indent=2, default=str)
        elif format == 'pickle':
            import pickle
            with open(filename, 'wb') as f:
                pickle.dump(results, f)
        elif format == 'csv':
            import pandas as pd
            df = pd.DataFrame(results)
            df.to_csv(filename, index=False)
        else:
            raise NLMError(f"Unsupported format: {format}")
        
        return True
    except Exception as e:
        raise NLMError(f"Failed to export results: {e}")


def import_results(filename: str, format: str = 'json') -> Dict[str, Any]:
    """
    Import simulation results from file.
    
    Args:
        filename: Input filename
        format: Import format ('json', 'pickle', 'csv')
        
    Returns:
        Dict[str, Any]: Imported results
        
    Raises:
        NLMError: If import fails
    """
    if not os.path.exists(filename):
        raise NLMError(f"File not found: {filename}")
    
    try:
        if format == 'json':
            with open(filename, 'r') as f:
                results = json.load(f)
        elif format == 'pickle':
            import pickle
            with open(filename, 'rb') as f:
                results = pickle.load(f)
        elif format == 'csv':
            import pandas as pd
            df = pd.read_csv(filename)
            results = df.to_dict(orient='records')
        else:
            raise NLMError(f"Unsupported format: {format}")
        
        return results
    except Exception as e:
        raise NLMError(f"Failed to import results: {e}")


# Version information
__version__ = "0.1.0"
__author__ = "NLM Project"
__email__ = "contact@nlm.org"
__license__ = "MIT"

# Performance and feature checks
PERFORMANCE_FEATURES = {
    'numpy_support': HAS_NUMPY,
    'matplotlib_support': HAS_MATPLOTLIB,
    'pandas_support': False  # Not required, available optionally
}

if not HAS_NUMPY:
    warnings.warn("NumPy not available. Some features may not work correctly.", NLMWarning)
if not HAS_MATPLOTLIB:
    warnings.warn("Matplotlib not available. Plotting features will be limited.", NLMWarning)
