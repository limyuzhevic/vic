# python/pynlm/__init__.py
"""
Improved Python bindings for NLM (Neural Learning Machine)

This module provides a more Pythonic and user-friendly interface to the
underlying C++ neural simulation framework.
"""

import sys
from typing import Dict, List, Optional, Union, Any, Callable
import numpy as np

# Import the underlying C++ bindings
from . import pynlm_core

# Version
__version__ = pynlm_core.__version__

# Re-export the original core functionality
from .pynlm_core import (
    # Original pynlm functions and classes
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    Config,
    Brain,
    AgentBrain,
    SimpleWorld,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID
)

# Import improved core functionality from _core.py
from ._core import (
    # Core convenience functions
    create_simulation_config,
    run_brain_simulation,
    create_agent_with_world,
    SimulationRunner,
    benchmark_brain,
    analyze_spike_patterns,
    optimize_configuration,
    PerformanceMonitor,
    batch_simulate
)

# Import enhanced utilities
from . import utilities
from . import advanced_features

# Configuration presets

class ConfigurationPresets:
    """Predefined configuration presets for common use cases."""
    
    @staticmethod
    def create_simple_brain(neuron_count: int = 100) -> Config:
        """Create a simple brain with basic settings."""
        config = createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.region_count", 1)
        config.set("brain.connection_probability", 0.05)
        return config
    
    @staticmethod
    def create_advanced_brain(neuron_count: int = 1000) -> Config:
        """Create an advanced brain with all features enabled."""
        config = createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.region_count", 3)
        config.set("brain.connection_probability", 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.dopamine.scale", 1.0)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        return config
    
    @staticmethod
    def create_minimal_brain(neuron_count: int = 50) -> Config:
        """Create a minimal brain for testing."""
        config = createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.region_count", 1)
        config.set("brain.connection_probability", 0.01)
        config.set("plasticity.stdp.enable", False)
        config.set("plasticity.hebbian.enable", False)
        return config

# World configurations

class WorldPresets:
    """Predefined world configurations."""
    
    @staticmethod
    def create_simple_world(width: int = 10, height: int = 10) -> SimpleWorld:
        """Create a simple 2D world."""
        world = createSimpleWorld()
        world.configure(width, height, 8, 8)
        world.setAgentStart(width // 2, height // 2)
        return world
    
    @staticmethod
    def create_large_world(width: int = 20, height: int = 20) -> SimpleWorld:
        """Create a larger world for exploration."""
        world = createSimpleWorld()
        world.configure(width, height, 16, 16)
        world.setAgentStart(width // 2, height // 2)
        return world

# Example factories

class ExampleFactory:
    """Factory class for creating example simulations."""
    
    def create_silent_brain(self, steps: int = 10) -> Dict[str, Any]:
        """Create a simple silent brain example."""
        config = ConfigurationPresets.create_simple_brain(50)
        brain = createBrain(config)
        brain.initialize()
        
        for i in range(steps):
            brain.step(i)
        
        return {
            'type': 'silent_brain',
           inger's_count': brain.getTotalNeuronCount(),
            'firing_count': brain.getFiringNeuronCount(),
            'spike_count': brain.getTotalSpikeCount()
        }
    
    def create_agent_world_example(self) -> Dict[str, Any]:
        """Create a complete agent-world example."""
        config = ConfigurationPresets.create_advanced_brain(200)
        world = WorldPresets.create_large_world(15, 15)
        
        brain = createBrain(config)
        brain.initialize()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Run a demonstration
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        return {
            'type': 'agent_world',
            'neurons': brain.getTotalNeuronCount(),
            'steps': 50,
            'firing_rate': brain.getAverageFiringRate(),
            'final_curiosity': agent.getCuriosityLevel()
        }

# Error classes

class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class SimulationError(NLMError):
    """Simulation-related errors."""
    pass

class ConfigurationError(NLMError):
    """Configuration-related errors."""
    pass

# Validation utilities

def validate_config(config: Config) -> List[str]:
    """
    Validate a configuration and return any errors.

    Args:
        config: Configuration to validate

    Returns:
        List of error messages, empty if valid
    """
    errors = []
    
    # Check neuron count
    neuron_count = config.getOr('brain.neuron_count', 0)
    if neuron_count <= 0:
        errors.append("Neuron count must be positive")
    elif neuron_count > 10000:
        errors.append("Neuron count too large (max 10000)")
    
    # Check region count
    region_count = config.getOr('brain.region_count', 0)
    if region_count <= 0:
        errors.append("Region count must be positive")
    
    # Check connection probability
    conn_prob = config.getOr('brain.connection_probability', 0.0)
    if conn_prob < 0.0 or conn_prob > 1.0:
        errors.append("Connection probability must be between 0.0 and 1.0")
    
    return errors

def validate_world(world: SimpleWorld) -> List[str]:
    """
    Validate a world configuration and return any errors.

    Args:
        world: World to validate

    Returns:
        List of error messages, empty if valid
    """
    errors = []
    
    width = world.getWidth()
    height = world.getHeight()
    
    if width <= 0 or height <= 0:
        errors.append("World dimensions must be positive")
    
    if width > 50 or height > 50:
        errors.append("World dimensions too large (max 50x50)")
    
    return errors

# Main API functions

def get_api_info() -> Dict[str, Any]:
    """
    Get information about the NLM Python API.

    Returns:
        Dictionary with API information
    """
    return {
        'version': __version__,
        'description': 'Improved Python bindings for NLM',
        'features': [
            'Basic neural simulation',
            'Configuration presets',
            'World presets',
            'Example factories',
            'Simulation runners',
            'Benchmarking tools',
            'Performance monitoring',
            'Batch processing',
            'Validation utilities',
            'Error handling'
        ],
        'available_classes': [
            'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
            'SimulationRunner', 'PerformanceMonitor'
        ],
        'available_functions': [
            'create_simulation_config', 'run_brain_simulation',
            'create_agent_with_world', 'benchmark_brain',
            'analyze_spike_patterns', 'optimize_configuration'
        ]
    }

# Setup and initialization

def setup_numpy():
    """Setup numpy for NLM operations."""
    import numpy as np
    np.set_printoptions(precision=3, suppress=True)

# Initialize numpy setup
setup_numpy()

# Public API
__all__ = [
    # Core classes
    'Config', 'Brain', 'AgentBrain', 'SimpleWorld',
    'NeuronType', 'SynapseType', 'DevelopmentalStage',
    'ActionType', 'MotorCommand', 'WorldObjectType',
    
    # Convenience functions
    'create_simulation_config',
    'run_brain_simulation',
    'create_agent_with_world',
    'SimulationRunner',
    'benchmark_brain',
    'analyze_spike_patterns',
    'optimize_configuration',
    
    # Configuration presets
    'ConfigurationPresets',
    'WorldPresets',
    
    # Error classes
    'NLMError',
    'SimulationError',
    'ConfigurationError',
    
    # Validation utilities
    'validate_config',
    'validate_world',
    
    # Main API
    'get_api_info',
    
    # Version
    '__version__'
]

# Welcome message for interactive sessions
if hasattr(sys, 'interactive') and sys.interactive:
    print("Welcome to the improved NLM Python API!")
    print("Use help(nlm) for more information on available functions.")
    print("Use nlm.get_api_info() to see all available features.")
