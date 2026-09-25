from typing import Optional, List, Dict, Any, Union, Iterator
from pathlib import Path
import json
import warnings
from contextlib import contextmanager

from .brain import Brain
from .config import Config
from .world import World
from .agent import Agent
from .exceptions import NLMErrors

__version__ = "0.1.0"
__author__ = "NLM Project"

__all__ = [
    "Brain",
    "Config",
    "World",
    "Agent",
    "NLMErrors",
    "run_simulation",
    "configure_brain",
    "create_config",
    "__version__",
]

# Main simulation function
def run_simulation(
    brain_config: Optional[Dict[str, Any]] = None,
    world_config: Optional[Dict[str, Any]] = None,
    agent_config: Optional[Dict[str, Any]] = None,
    num_steps: int = 1000,
    timestep: float = 0.1,
    verbose: bool = False,
) -> Dict[str, Any]:
    """Run a complete simulation episode.
    
    Args:
        brain_config: Brain configuration parameters
        world_config: World configuration parameters  
        agent_config: Agent configuration parameters
        num_steps: Number of simulation steps to run
        timestep: Time step duration
        verbose: Enable verbose logging
        
    Returns:
        Dictionary containing simulation results
        
    Raises:
        NLMError: If simulation fails to initialize
        NLMConfigurationError: If configuration is invalid
    """
    pass

# Configuration helper functions
def create_config(name: str = "default", **kwargs) -> Config:
    """Create a configuration with common presets.
    
    Args:
        name: Preset configuration name
        **kwargs: Additional configuration parameters
        
    Returns:
        Configured Config object
    """
    pass

def configure_brain(
    neuron_count: Optional[int] = None,
    synapse_density: Optional[float] = None,
    learning_rate: Optional[float] = None,
    development_enabled: Optional[bool] = None,
    curiosity_enabled: Optional[bool] = None,
    **kwargs,
) -> Dict[str, Any]:
    """Helper function to configure a brain with common parameters.
    
    Args:
        neuron_count: Number of neurons
        synapse_density: Synaptic connection density
        learning_rate: Learning rate for plasticity
        development_enabled: Enable development system
        curiosity_enabled: Enable curiosity-driven exploration
        **kwargs: Additional parameters
        
    Returns:
        Configuration dictionary
    """
    pass

# Example utilities
@contextmanager
def simulation_episode(
    brain: Brain,
    world: Optional[World] = None,
    agent: Optional[Agent] = None,
    save_path: Optional[Union[str, Path]] = None,
    load_path: Optional[Union[str, Path]] = None,
) -> Iterator[Dict[str, Any]]:
    """Context manager for running a simulation episode.
    
    Automatically handles initialization, execution, and cleanup.
    Optionally saves or loads brain state.
    
    Args:
        brain: Brain instance to simulate
        world: World instance for environment interaction
        agent: Agent instance for sensory-motor integration
        save_path: Path to save brain state at episode end
        load_path: Path to load initial brain state
        
    Yields:
        Dictionary with episode statistics
        
    Example:
        >>> with simulation_episode(brain, world, agent, save_path="checkpoint")
        ...     for step in range(100):
        ...         # run simulation steps
        ...         pass
    """
    pass

# Simulation utilities
def analyze_brain_activity(brain: Brain, window_size: int = 100) -> Dict[str, Any]:
    """Analyze brain activity patterns.
    
    Args:
        brain: Brain instance to analyze
        window_size: Analysis window size in steps
        
    Returns:
        Dictionary with activity metrics
    """
    pass

def generate_stats_report(brain: Brain, agent: Optional[Agent] = None) -> str:
    """Generate a human-readable statistics report.
    
    Args:
        brain: Brain instance to report on
        agent: Agent instance (if available)
        
    Returns:
        Formatted statistics report string
    """
    pass
