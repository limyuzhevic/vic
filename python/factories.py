"""NLM Factory Functions"""

from typing import Dict, List, Any, Optional
from .types import (
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType
)
from .exceptions import NLMError, ConfigurationError, SimulationError, ValidationError

# Note: These are type definitions. The actual implementations are in the C++ bindings
# and exposed through the pybind11 module.

def create_default_config() -> Any:
    """
    Create a default configuration with standard NLM settings.
    
    Returns:
        Config: Default configuration with all standard settings
    """
    pass

def create_minimal_config() -> Any:
    """
    Create a minimal configuration for quick testing.
    
    Returns:
        Config: Minimal configuration with essential settings only
    """
    pass

def create_high_performance_config() -> Any:
    """
    Create a high-performance configuration for large simulations.
    
    Returns:
        Config: High-performance configuration optimized for large simulations
    """
    pass

def create_grid_world_config() -> Any:
    """
    Create a grid world configuration.
    
    Returns:
        Config: Grid world configuration suitable for navigation tasks
    """
    pass

def create_maze_world_config() -> Any:
    """
    Create a maze world configuration.
    
    Returns:
        Config: Maze world configuration suitable for problem-solving tasks
    """
    pass

def create_brain(config: Any) -> Any:
    """
    Create a brain with the specified configuration.
    
    Args:
        config: Configuration to use for the brain
        
    Returns:
        Brain: New brain instance
    """
    pass

def create_simple_world() -> Any:
    """
    Create a simple world instance.
    
    Returns:
        SimpleWorld: New world instance with default settings
    """
    pass

def create_agent_brain(brain: Any) -> Any:
    """
    Create an agent brain interface with the specified brain.
    
    Args:
        brain: Brain instance to interface with
        
    Returns:
        AgentBrain: New agent brain interface
    """
    pass

def create_simulation_controller() -> Any:
    """
    Create a simulation controller for managing simulation state.
    
    Returns:
        SimulationController: New simulation controller
    """
    pass

def create_performance_monitor() -> Any:
    """
    Create a performance monitor for tracking simulation metrics.
    
    Returns:
        PerformanceMonitor: New performance monitor
    """
    pass

def create_batch_processor(name: str = "Batch") -> Any:
    """
    Create a batch processor for managing multiple operations.
    
    Args:
        name: Name for the batch processor
        
    Returns:
        BatchProcessor: New batch processor
    """
    pass

def create_experiment_manager() -> Any:
    """
    Create an experiment manager for managing and running experiments.
    
    Returns:
        ExperimentManager: New experiment manager
    """
    pass