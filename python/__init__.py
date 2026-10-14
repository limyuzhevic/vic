"""
pynlm - Python bindings for NLM (Neural Learning Machine)

This module provides a comprehensive Python interface to the NLM C++ neural
simulation framework with:
- High-level convenience APIs for common use cases
- Comprehensive error handling and validation
- Extensive documentation and examples
- Type hints and IDE support
- Simulation utilities and helpers

The bindings are built using pybind11 and provide full access to the power
of the NLM neural simulation framework while maintaining a user-friendly Python interface.
"""

import sys
import os
from typing import Dict, Any, Optional, List, Union

# Import the pybind11 module
try:
    import pynlm as pynlm_module
    _HAS_BINDINGS = True
except ImportError:
    _HAS_BINDINGS = False
    pynlm_module = None

# Core exception classes
class NLMError(Exception):
    """Base exception class for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Error in configuration."""
    pass

class SimulationError(NLMError):
    """Error during simulation."""
    pass

class ValidationError(NLMError):
    """Validation error."""
    pass

# Re-export all symbols from the pybind11 module
try:
    from .python_bindings import (
        NeuronId, SynapseId, RegionId, PopulationId,
        NeuronType, SynapseType, DevelopmentalStage, FiringState,
        ActionType, MotorCommand, WorldObjectType,
        Config, SensoryInput, Vision, Audio, InternalSignals,
        Action, WorldObject, AgentBody, ActionResult,
        SensoryPercept, SimpleWorld, Brain, AgentBrain,
        createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
        INVALID_NEURON_ID, INVALID_SYNAPSE_ID,
        INVALID_REGION_ID, INVALID_POPULATION_ID
    )
except ImportError:
    # Fallback for when the module doesn't exist
    NeuronId = SynapseId = RegionId = PopulationId = None
    NeuronType = SynapseType = DevelopmentalStage = FiringState = None
    ActionType = MotorCommand = WorldObjectType = None
    Config = SensoryInput = Vision = Audio = InternalSignals = None
    Action = WorldObject = AgentBody = ActionResult = None
    SensoryPercept = SimpleWorld = Brain = AgentBrain = None
    createDefaultConfig = createBrain = createSimpleWorld = createAgentBrain = None
    INVALID_NEURON_ID = INVALID_SYNAPSE_ID = INVALID_REGION_ID = INVALID_POPULATION_ID = None

# Convenience aliases
brain = createBrain
config = createDefaultConfig
world = createSimpleWorld
agent = createAgentBrain
environment = createSimpleWorld

# Module metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__email__ = "contact@nlm.org"
__description__ = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Convenience functions that provide a higher-level API
def create_simulation(config_dict: Optional[Dict[str, Any]] = None):
    """
    Create a complete simulation setup with brain, world, and agent.
    
    This is a convenience function that creates all the necessary components
    for a complete simulation and returns them in a dictionary.
    
    Args:
        config_dict: Optional dictionary of configuration parameters.
    
    Returns:
        Dictionary containing 'brain', 'world', 'agent', and 'config'.
    
    Example:
        >>> sim = create_simulation({'brain.neuron_count': 500})
        >>> brain = sim['brain']
        >>> world = sim['world']
        >>> agent = sim['agent']
    """
    if not _HAS_BINDINGS:
        raise NLMError("NLM Python bindings not available")
    
    # Create configuration
    config = createDefaultConfig()
    
    if config_dict:
        for key, value in config_dict.items():
            try:
                config.set(key, value)
            except Exception as e:
                raise ConfigurationError(f"Failed to set configuration '{key}': {e}")
    
    # Create components
    brain_obj = createBrain(config)
    brain_obj.initialize()
    
    world_obj = createSimpleWorld()
    world_obj.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world_obj.reset()
    
    agent_obj = createAgentBrain(brain_obj)
    agent_obj.initialize(world_obj)
    
    return {
        'brain': brain_obj,
        'world': world_obj,
        'agent': agent_obj,
        'config': config
    }

def run_agent_episode(steps: int = 100, enable_features: bool = True):
    """
    Run a complete agent simulation episode.
    
    This function creates a simulation and runs it through a complete episode,
    optionally enabling various brain features.
    
    Args:
        steps: Number of simulation steps to run.
        enable_features: Whether to enable learning features (reward, curiosity, etc.).
    
    Returns:
        Dictionary containing the simulation components and statistics.
    
    Example:
        >>> results = run_agent_episode(200, enable_features=True)
        >>> print(f"Final curiosity: {results['final_curiosity']}")
    """
    if not _HAS_BINDINGS:
        raise NLMError("NLM Python bindings not available")
    
    sim = create_simulation()
    brain_obj = sim['brain']
    world_obj = sim['world']
    agent_obj = sim['agent']
    
    # Enable learning features if requested
    if enable_features:
        agent_obj.enableRewardModulation(True)
        agent_obj.enableCuriosity(True)
        agent_obj.enableDevelopment(True)
        agent_obj.enableStructuralPlasticity(True)
    
    # Store initial stats
    initial_stats = {
        'neurons_firing': brain_obj.getFiringNeuronCount(),
        'total_spikes': brain_obj.getTotalSpikeCount(),
        'curiosity': agent_obj.getCuriosityLevel(),
        'development_stage': brain_obj.getDevelopmentalStage()
    }
    
    # Run simulation
    for step in range(steps):
        world_obj.update(0.1)
        
        percept = world_obj.getSensoryPercept()
        agent_obj.processSensoryInput(percept)
        brain_obj.step(step)
        
        action = agent_obj.decodeMotorCommand()
        world_obj.applyMotorCommand(action, world_obj.getSimulationTime())
        
        # Apply reward modulation if enabled
        if enable_features:
            internal_signals = percept.getInternal()
            reward = internal_signals[0] if internal_signals and len(internal_signals) > 0 else 0.0
            agent_obj.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent_obj.updateDevelopment(0.1)
    
    # Store final stats
    final_stats = {
        'neurons_firing': brain_obj.getFiringNeuronCount(),
        'total_spikes': brain_obj.getTotalSpikeCount(),
        'curiosity': agent_obj.getCuriosityLevel(),
        'development_stage': brain_obj.getDevelopmentalStage(),
        'avg_firing_rate': brain_obj.getAverageFiringRate(),
        'e_i_ratio': brain_obj.getExcitationInhibitionRatio()
    }
    
    return {
        'simulation': sim,
        'initial_stats': initial_stats,
        'final_stats': final_stats,
        'steps_run': steps
    }

def quick_test():
    """
    Quick test of basic NLM functionality.
    
    This function runs a minimal test to verify that the bindings are working
    correctly and provides basic output statistics.
    
    Returns:
        The brain object for further testing.
    
    Example:
        >>> brain = quick_test()
        >>> print(f"Brain has {brain.getTotalNeuronCount()} neurons")
    """
    if not _HAS_BINDINGS:
        raise NLMError("NLM Python bindings not available")
    
    # Create default brain
    brain_obj = createBrain(createDefaultConfig())
    brain_obj.initialize()
    
    # Run for 50 steps
    for i in range(50):
        brain_obj.step(i)
    
    print(f"Quick test passed! Brain has {brain_obj.getTotalNeuronCount()} neurons")
    print(f"Total spikes: {brain_obj.getTotalSpikeCount()}")
    print(f"Currently firing: {brain_obj.getFiringNeuronCount()}")
    
    return brain_obj

# Public API items
__all__ = [
    # Core types and classes
    'NeuronId', 'SynapseId', 'RegionId', 'PopulationId',
    'NeuronType', 'SynapseType', 'DevelopmentalStage', 'FiringState',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    'Config', 'SensoryInput', 'Vision', 'Audio', 'InternalSignals',
    'Action', 'WorldObject', 'AgentBody', 'ActionResult',
    'SensoryPercept', 'SimpleWorld', 'Brain', 'AgentBrain',
    
    # Functions
    'createDefaultConfig', 'createBrain', 'createSimpleWorld', 'createAgentBrain',
    'create_simulation', 'run_agent_episode', 'quick_test',
    
    # Constants
    'INVALID_NEURON_ID', 'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID', 'INVALID_POPULATION_ID',
    
    # Shortcuts
    'brain', 'config', 'world', 'agent', 'environment',
    
    # Exceptions
    'NLMError', 'ConfigurationError', 'SimulationError', 'ValidationError',
]

# Create the main module object
pynlm = pynlm_module
