"""
Core NLM package with enhanced Pythonic interfaces.

This module provides the main classes and functions for working with the NLM system,
including enhanced Brain, World, Config classes with Pythonic APIs, context managers,
error handling, and factory methods.
"""

# Core exceptions
from .exceptions import (
    NLMError,
    ConfigError,
    BrainError,
    WorldError,
    SensorError,
    AgentError,
    ValidationError,
    MemoryError,
    SimulationError,
    DevelopmentalError,
)

# Configuration
from .config import (
    create_default_config,
    ConfigWrapper,
    create_config_from_dict,
    create_config_with_validation,
)

# Brain classes
from .brain import (
    create_brain,
    BrainWrapper,
    brain_context,
    run_brain_simulation,
    create_brain_with_config,
)

# World classes
from .world import (
    create_simple_world,
    WorldWrapper,
    world_context,
    run_world_simulation,
    create_agent_simulation,
)

# Sensor helpers
from .sensor_helper import (
    SensorHelper,
    normalize_sensory_data,
    convert_all_to_numpy,
    process_sensory_features,
)

# Factory methods
from .factories import (
    NLMFactory,
    create_brain_from_config,
    create_brain_with_validation,
    create_world_from_config,
    create_world_with_agents,
    create_agent_simulation,
    create_experiment_config,
    create_training_config,
)

# Re-export original pynlm classes for advanced users
try:
    import pynlm
except ImportError:
    pynlm = None

# Try to expose the original classes
if pynlm:
    __all__ = [
        # Exceptions
        'NLMError', 'ConfigError', 'BrainError', 'WorldError',
        'SensorError', 'AgentError', 'ValidationError', 'MemoryError',
        'SimulationError', 'DevelopmentalError',
        
        # Configuration
        'create_default_config', 'ConfigWrapper',
        'create_config_from_dict', 'create_config_with_validation',
        
        # Brain
        'create_brain', 'BrainWrapper', 'brain_context',
        'run_brain_simulation', 'create_brain_with_config',
        
        # World
        'create_simple_world', 'WorldWrapper', 'world_context',
        'run_world_simulation', 'create_agent_simulation',
        
        # Sensor helpers
        'SensorHelper', 'normalize_sensory_data',
        'convert_all_to_numpy', 'process_sensory_features',
        
        # Factory methods
        'NLMFactory', 'create_brain_from_config', 'create_brain_with_validation',
        'create_world_from_config', 'create_world_with_agents',
        'create_experiment_config', 'create_training_config',
        
        # Original pynlm module (for advanced users)
        'pynlm',
    ]
else:
    __all__ = [
        # Exceptions
        'NLMError', 'ConfigError', 'BrainError', 'WorldError',
        'SensorError', 'AgentError', 'ValidationError', 'MemoryError',
        'SimulationError', 'DevelopmentalError',
        
        # Configuration
        'create_default_config', 'ConfigWrapper',
        'create_config_from_dict', 'create_config_with_validation',
        
        # Brain
        'create_brain', 'BrainWrapper', 'brain_context',
        'run_brain_simulation', 'create_brain_with_config',
        
        # World
        'create_simple_world', 'WorldWrapper', 'world_context',
        'run_world_simulation', 'create_agent_simulation',
        
        # Sensor helpers
        'SensorHelper', 'normalize_sensory_data',
        'convert_all_to_numpy', 'process_sensory_features',
        
        # Factory methods
        'NLMFactory', 'create_brain_from_config', 'create_brain_with_validation',
        'create_world_from_config', 'create_world_with_agents',
        'create_experiment_config', 'create_training_config',
    ]

"""
Core NLM package with enhanced Pythonic interfaces.

This module provides the main classes and functions for working with the NLM system,
including enhanced Brain, World, Config classes with Pythonic APIs, context managers,
error handling, and factory methods.
"""

import sys
from typing import Dict

# Core exceptions
from .exceptions import (
    NLMError,
    ConfigError,
    BrainError,
    WorldError,
    SensorError,
    AgentError,
    ValidationError,
    MemoryError,
    SimulationError,
    DevelopmentalError,
)

# Configuration
from .config import (
    create_default_config,
    ConfigWrapper,
    create_config_from_dict,
    create_config_with_validation,
)

# Brain classes
from .brain import (
    create_brain,
    BrainWrapper,
    brain_context,
    run_brain_simulation,
    create_brain_with_config,
)

# World classes
from .world import (
    create_simple_world,
    WorldWrapper,
    world_context,
    run_world_simulation,
    create_agent_simulation,
)

# Sensor helpers
from .sensor_helper import (
    SensorHelper,
    normalize_sensory_data,
    convert_all_to_numpy,
    process_sensory_features,
)

# Factory methods
from .factories import (
    NLMFactory,
    create_brain_from_config,
    create_brain_with_validation,
    create_world_from_config,
    create_world_with_agents,
    create_agent_simulation,
    create_experiment_config,
    create_training_config,
)

# Re-export original pynlm classes for advanced users
try:
    import pynlm
except ImportError:
    pynlm = None

# Try to expose the original classes
if pynlm:
    __all__ = [
        # Exceptions
        'NLMError', 'ConfigError', 'BrainError', 'WorldError',
        'SensorError', 'AgentError', 'ValidationError', 'MemoryError',
        'SimulationError', 'DevelopmentalError',
        
        # Configuration
        'create_default_config', 'ConfigWrapper',
        'create_config_from_dict', 'create_config_with_validation',
        
        # Brain
        'create_brain', 'BrainWrapper', 'brain_context',
        'run_brain_simulation', 'create_brain_with_config',
        
        # World
        'create_simple_world', 'WorldWrapper', 'world_context',
        'run_world_simulation', 'create_agent_simulation',
        
        # Sensor helpers
        'SensorHelper', 'normalize_sensory_data',
        'convert_all_to_numpy', 'process_sensory_features',
        
        # Factory methods
        'NLMFactory', 'create_brain_from_config', 'create_brain_with_validation',
        'create_world_from_config', 'create_world_with_agents',
        'create_experiment_config', 'create_training_config',
        
        # Original pynlm module (for advanced users)
        'pynlm',
    ]
else:
    __all__ = [
        # Exceptions
        'NLMError', 'ConfigError', 'BrainError', 'WorldError',
        'SensorError', 'AgentError', 'ValidationError', 'MemoryError',
        'SimulationError', 'DevelopmentalError',
        
        # Configuration
        'create_default_config', 'ConfigWrapper',
        'create_config_from_dict', 'create_config_with_validation',
        
        # Brain
        'create_brain', 'BrainWrapper', 'brain_context',
        'run_brain_simulation', 'create_brain_with_config',
        
        # World
        'create_simple_world', 'WorldWrapper', 'world_context',
        'run_world_simulation', 'create_agent_simulation',
        
        # Sensor helpers
        'SensorHelper', 'normalize_sensory_data',
        'convert_all_to_numpy', 'process_sensory_features',
        
        # Factory methods
        'NLMFactory', 'create_brain_from_config', 'create_brain_with_validation',
        'create_world_from_config', 'create_world_with_agents',
        'create_experiment_config', 'create_training_config',
    ]

# Convenience functions for common operations
def create_complete_simulation(brain_config: Dict = None, world_config: Dict = None):
    """
    Create a complete NLM simulation with brain and world.
    
    Args:
        brain_config: Brain configuration dictionary
        world_config: World configuration dictionary
        
    Returns:
        Tuple of (BrainWrapper, WorldWrapper)
    """
    return NLMFactory.create_agent_simulation(brain_config, world_config)
def run_full_episode(
    brain_wrapper: 'BrainWrapper',
    world_wrapper: 'WorldWrapper',
    steps: int = 100,
    timestep: float = 0.1
):
    """
    Run a complete simulation episode.
    
    Args:
        brain_wrapper: Brain wrapper instance
        world_wrapper: World wrapper instance
        steps: Number of simulation steps
        timestep: Time step size
        
    Returns:
        Simulation results dictionary
    """
    results = {
        'steps_completed': 0,
        'final_brain_stats': None,
        'final_world_stats': None,
        'brain_wrapper': brain_wrapper,
        'world_wrapper': world_wrapper,
    }
    
    for step in range(steps):
        # Get sensory input
        percept = world_wrapper.get_sensory_percept()
        
        # Process sensory input in brain
        brain_wrapper.process_sensory_input(percept)
        
        # Run brain step
        brain_wrapper.step(step)
        
        # Get action from brain
        action = brain_wrapper.produce_action()
        
        # Apply action in world
        world_wrapper.apply_motor_command(action, world_wrapper.get_simulation_time())
        
        results['steps_completed'] += 1
    
    results['final_brain_stats'] = brain_wrapper.get_stats()
    results['final_world_stats'] = world_wrapper.get_stats()
    
    return results

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Package validation
if __name__ == "__main__":
    print(f"NLM Python Package v{__version__}")
    print(f"Author: {__author__}")
    print(f"License: {__license__}")
    print(f"Total classes: {len(__all__)}")

def run_full_episode(
    brain_wrapper: 'BrainWrapper',
    world_wrapper: 'WorldWrapper',
    steps: int = 100,
    timestep: float = 0.1
):
    """
    Run a complete simulation episode.
    
    Args:
        brain_wrapper: Brain wrapper instance
        world_wrapper: World wrapper instance
        steps: Number of simulation steps
        timestep: Time step size
        
    Returns:
        Simulation results dictionary
    """
    results = {
        'steps_completed': 0,
        'final_brain_stats': None,
        'final_world_stats': None,
        'brain_wrapper': brain_wrapper,
        'world_wrapper': world_wrapper,
    }
    
    for step in range(steps):
        # Get sensory input
        percept = world_wrapper.get_sensory_percept()
        
        # Process sensory input in brain
        brain_wrapper.process_sensory_input(percept)
        
        # Run brain step
        brain_wrapper.step(step)
        
        # Get action from brain
        action = brain_wrapper.produce_action()
        
        # Apply action in world
        world_wrapper.apply_motor_command(action, world_wrapper.get_simulation_time())
        
        results['steps_completed'] += 1
    
    results['final_brain_stats'] = brain_wrapper.get_stats()
    results['final_world_stats'] = world_wrapper.get_stats()
    
    return results

# Version information
__version__ = "0.1.0"
__author__ = "NLM Authors"
__license__ = "MIT"

# Package validation
if __name__ == "__main__":
    print(f"NLM Python Package v{__version__}")
    print(f"Author: {__author__}")
    print(f"License: {__license__}")
    print(f"Total classes: {len(__all__)}")
