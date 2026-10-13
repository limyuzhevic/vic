"""
NLM Python Bindings Module

This module provides a comprehensive Python interface to the NLM (Neural Learning Machine) framework.
It includes enhanced configuration management, logging, and brain simulation capabilities.

Usage:
    import nlm
    
    # Create and configure a brain
    config = nlm.Config()
    config.set("neuron_count", 1000)
    
    brain = nlm.Brain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
"""

import sys
import os
from typing import Optional, List, Any, Callable
from enum import Enum

# Add the config module to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '.'))

# Import core modules
try:
    from config import Config, ConfigSource, NLMConfigError
    from brain import Brain
    from sensory import SensoryInput, Vision, Audio, InternalSignals
    from motor import Action
    from world import SimpleWorld
    from agent import AgentBrain
    from neuromodulation import Neuromodulator, Dopamine
    from memory import WorkingMemory, EpisodicMemory
    from prediction import PredictionSystem
    from cognition import NeuralPlanner, ConceptFormation, AttentionalSelection
    from development import DevelopmentSystem
    from plasticity import STDP, Hebbian
    from dynamics import SpikeSystem
    from core import NeuronType, SynapseType, DevelopmentalStage
    from visualization import VisualizationInterface
    from utils import Logger, LogLevel
    
    # Import from the C++ bindings if available
    try:
        from . import pynlm
        _CPP_BINDINGS_AVAILABLE = True
    except ImportError:
        _CPP_BINDINGS_AVAILABLE = False
        print("Warning: C++ bindings not available. Using pure Python implementation.")
        
except ImportError as e:
    print(f"Warning: Some NLM modules not available: {e}")
    _CPP_BINDINGS_AVAILABLE = False

# Version information
__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Neural Learning Machine - Python bindings for brain simulation"

# Global configuration and instances
_global_config = None
_global_logger = None

# Convenience functions
def get_config() -> Config:
    """Get the global configuration instance."""
    global _global_config
    if _global_config is None:
        _global_config = Config()
    return _global_config

def set_config_value(key: str, value: Any, source: ConfigSource = ConfigSource.RUNTIME):
    """Set a value in the global configuration."""
    get_config().set(key, value, source)

def get_config_value(key: str, default_value: Any = None) -> Any:
    """Get a value from the global configuration."""
    return get_config().get_or(key, default_value)

def get_logger() -> Logger:
    """Get the global logger instance."""
    global _global_logger
    if _global_logger is None:
        _global_logger = Logger()
        _global_logger.addLogger( Logger(LogLevel.INFO))
    return _global_logger

def create_brain(config: Optional[Config] = None) -> Brain:
    """
    Create a new NLM brain instance.
    
    Args:
        config: Optional configuration object. If None, uses default config.
        
    Returns:
        Brain instance
    """
    if config is None:
        config = get_config()
    return Brain(config)

def create_world(width: int = 10, height: int = 10) -> SimpleWorld:
    """
    Create a new NLM world instance.
    
    Args:
        width: World width
        height: World height
        
    Returns:
        SimpleWorld instance
    """
    world = SimpleWorld()
    world.configure(width, height, 16, 16)  # Default vision dimensions
    return world

def create_agent_brain(brain: Brain) -> AgentBrain:
    """
    Create an agent brain interface for a given brain.
    
    Args:
        brain: NLM brain instance
        
    Returns:
        AgentBrain instance
    """
    return AgentBrain(brain)

# Context manager for simulation
def simulation_context(config: Optional[Config] = None, steps: int = 100):
    """
    Context manager for running NLM simulations.
    
    Usage:
        with simulation_context(steps=50) as brain:
            for step in range(steps):
                brain.step(step)
    """
    class SimulationContext:
        def __init__(self, config, steps):
            self.config = config
            self.steps = steps
            self.brain = None
            
        def __enter__(self):
            self.brain = create_brain(self.config)
            self.brain.initialize()
            return self.brain
            
        def __exit__(self, exc_type, exc_val, exc_tb):
            if self.brain:
                self.brain.reset()
    
    return SimulationContext(config, steps)

# Error handling utilities
def handle_simulation_error(func: Callable) -> Callable:
    """
    Decorator to handle simulation errors gracefully.
    
    Usage:
        @handle_simulation_error
        def run_simulation():
            # Simulation code here
            pass
    """
    def wrapper(*args, **kwargs):
        try:
            return func(*args, **kwargs)
        except Exception as e:
            logger = get_logger()
            logger.error(f"Simulation error: {e}")
            raise SimulationError(f"Simulation failed: {e}")
    
    return wrapper

class SimulationError(Exception):
    """Exception raised for simulation errors."""
    pass

# Visualization utilities
def create_visualizer(brain: Brain) -> VisualizationInterface:
    """
    Create a visualization interface for a brain.
    
    Args:
        brain: NLM brain instance
        
    Returns:
        VisualizationInterface instance
    """
    return VisualizationInterface(brain)

# Export all public symbols
__all__ = [
    # Core classes
    'Config',
    'ConfigSource', 
    'NLMConfigError',
    'Brain',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    
    # Sensory and motor
    'SensoryInput',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    
    # World and agent
    'SimpleWorld',
    'AgentBrain',
    
    # Neuromodulation
    'Neuromodulator',
    'Dopamine',
    
    # Memory systems
    'WorkingMemory',
    'EpisodicMemory',
    
    # Prediction and cognition
    'PredictionSystem',
    'NeuralPlanner',
    'ConceptFormation',
    'AttentionalSelection',
    
    # Development
    'DevelopmentSystem',
    
    # Plasticity
    'STDP',
    'Hebbian',
    
    # Dynamics
    'SpikeSystem',
    
    # Visualization
    'VisualizationInterface',
    
    # Utilities
    'Logger',
    'LogLevel',
    
    # Convenience functions
    'get_config',
    'set_config_value',
    'get_config_value',
    'get_logger',
    'create_brain',
    'create_world',
    'create_agent_brain',
    'create_visualizer',
    
    # Context managers
    'simulation_context',
    
    # Error handling
    'handle_simulation_error',
    'SimulationError',
    
    # Version info
    '__version__',
    '__author__',
    '__description__',
]