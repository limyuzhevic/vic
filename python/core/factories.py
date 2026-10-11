"""
Factory methods for creating NLM objects with validation and convenience.

This module provides factory methods for creating NLM objects with sensible defaults,
validation, and helper methods for common use cases.
"""

import time
from typing import Dict, Any, Optional, List, Tuple
from .exceptions import ValidationError, ConfigError

# Import the original classes
import pynlm
from .brain import create_brain, BrainWrapper
from .world import create_simple_world, WorldWrapper
from .config import create_default_config, ConfigWrapper, create_config_from_dict

class NLMFactory:
    """
    Factory class for creating NLM objects with validation and convenience.
    
    This factory provides methods for creating NLM objects with sensible defaults,
    validation, and helper methods for common use cases.
    """
    
    @staticmethod
    def create_brain_from_config(config: Dict[str, Any]) -> BrainWrapper:
        """
        Create a brain from configuration dictionary.
        
        Args:
            config: Configuration dictionary
            
        Returns:
            BrainWrapper instance
            
        Raises:
            ValidationError: If configuration is invalid
        """
        if not isinstance(config, dict):
            raise ValidationError("Configuration must be a dictionary")
        
        # Convert config dict to C++ Config
        cpp_config = pynlm.createDefaultConfig()
        for key, value in config.items():
            cpp_config.set(key, value)
        
        brain = pynlm.createBrain(cpp_config)
        return BrainWrapper(brain)
    
    @staticmethod
    def create_brain_with_validation(
        neuron_count: int = 1000,
        synapse_density: float = 0.1,
        development_enabled: bool = False
    ) -> BrainWrapper:
        """
        Create a brain with validation and sensible defaults.
        
        Args:
            neuron_count: Number of neurons
            synapse_density: Synapse density
            development_enabled: Whether development is enabled
            
        Returns:
            BrainWrapper instance
        """
        # Validate parameters
        if neuron_count <= 0:
            raise ValidationError("Neuron count must be positive")
        if not 0.0 <= synapse_density <= 1.0:
            raise ValidationError("Synapse density must be between 0.0 and 1.0")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", synapse_density)
        
        if development_enabled:
            config.set("development.enabled", True)
        
        brain = pynlm.createBrain(config)
        return BrainWrapper(brain)
    
    @staticmethod
    def create_world_from_config(config: Dict[str, Any]) -> WorldWrapper:
        """
        Create a world from configuration dictionary.
        
        Args:
            config: Configuration dictionary
            
        Returns:
            WorldWrapper instance
        """
        if not isinstance(config, dict):
            raise ValidationError("World configuration must be a dictionary")
        
        world = create_simple_world()
        
        world.configure(
            config.get("width", 100),
            config.get("height", 100),
            config.get("vision_width", 20),
            config.get("vision_height", 20)
        )
        
        return WorldWrapper(world)
    
    @staticmethod
    def create_world_with_agents(
        width: int = 100,
        height: int = 100,
        vision_width: int = 20,
        vision_height: int = 20,
        agent_start: Optional[Tuple[float, float]] = None,
        random_seed: Optional[int] = None
    ) -> WorldWrapper:
        """
        Create a world with agent configuration.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision width
            vision_height: Vision height
            agent_start: Agent starting position (x, y)
            random_seed: Random seed
            
        Returns:
            WorldWrapper instance
        """
        world = create_simple_world()
        world.configure(width, height, vision_width, vision_height)
        
        if agent_start:
            world.setAgentStart(agent_start[0], agent_start[1])
        
        if random_seed is not None:
            world.setRandomSeed(random_seed)
        
        return WorldWrapper(world)
    
    @staticmethod
    def create_agent_simulation(
        brain_config: Optional[Dict[str, Any]] = None,
        world_config: Optional[Dict[str, Any]] = None,
        simulation_steps: int = 100,
        timestep: float = 0.1
    ) -> Tuple[BrainWrapper, WorldWrapper]:
        """
        Create an agent simulation with brain and world.
        
        Args:
            brain_config: Brain configuration
            world_config: World configuration
            simulation_steps: Number of simulation steps
            timestep: Time step size
            
        Returns:
            Tuple of (BrainWrapper, WorldWrapper)
        """
        # Create brain
        if brain_config:
            brain = NLMFactory.create_brain_from_config(brain_config)
        else:
            brain = NLMFactory.create_brain_with_validation()
        
        # Create world
        if world_config:
            world = NLMFactory.create_world_from_config(world_config)
        else:
            world = NLMFactory.create_world_with_agents()
        
        return brain, world
    
    @staticmethod
    def create_experiment_config(
        experiment_name: str,
        brain_config: Optional[Dict[str, Any]] = None,
        world_config: Optional[Dict[str, Any]] = None,
        sensor_types: List[str] = None
    ) -> Dict[str, Any]:
        """
        Create an experiment configuration.
        
        Args:
            experiment_name: Name of the experiment
            brain_config: Brain configuration
            world_config: World configuration
            sensor_types: List of sensor types to include
            
        Returns:
            Experiment configuration dictionary
        """
        experiment_config = {
            "experiment_name": experiment_name,
            "brain_config": brain_config or {},
            "world_config": world_config or {},
            "sensor_types": sensor_types or ["vision", "audio", "internal"],
            "timestamp": time.time(),
            "version": "0.1.0"
        }
        
        return experiment_config
    
    @staticmethod
    def create_training_config(
        learning_rate: float = 0.001,
        batch_size: int = 32,
        epochs: int = 10,
        validation_split: float = 0.2
    ) -> Dict[str, Any]:
        """
        Create a training configuration.
        
        Args:
            learning_rate: Learning rate
            batch_size: Batch size
            epochs: Number of epochs
            validation_split: Validation split ratio
            
        Returns:
            Training configuration dictionary
        """
        # Validate parameters
        if learning_rate <= 0:
            raise ValidationError("Learning rate must be positive")
        if batch_size <= 0:
            raise ValidationError("Batch size must be positive")
        if epochs <= 0:
            raise ValidationError("Epochs must be positive")
        if not 0.0 <= validation_split <= 1.0:
            raise ValidationError("Validation split must be between 0.0 and 1.0")
        
        return {
            "learning_rate": learning_rate,
            "batch_size": batch_size,
            "epochs": epochs,
            "validation_split": validation_split,
            "optimizer": "adam",
            "loss_function": "mse"
        }

# Re-export the factory class and methods
__all__ = [
    'NLMFactory',
    'create_brain_from_config',
    'create_brain_with_validation',
    'create_world_from_config',
    'create_world_with_agents',
    'create_agent_simulation',
    'create_experiment_config',
    'create_training_config',
]

# Convenience functions
import time

def create_default_simulation() -> Tuple[BrainWrapper, WorldWrapper]:
    """
    Create a default simulation setup.
    
    Returns:
        Tuple of (BrainWrapper, WorldWrapper)
    """
    return NLMFactory.create_agent_simulation()
def create_development_simulation(development_enabled: bool = True) -> Tuple[BrainWrapper, WorldWrapper]:
    """
    Create a development simulation.
    
    Args:
        development_enabled: Whether to enable development
        
    Returns:
        Tuple of (BrainWrapper, WorldWrapper)
    """
    brain_config = {}
    if development_enabled:
        brain_config["development.enabled"] = True
    
    return NLMFactory.create_agent_simulation(brain_config=brain_config)
def create_reward_simulation(
    reward_enabled: bool = True,
    curiosity_enabled: bool = True
) -> Tuple[BrainWrapper, WorldWrapper]:
    """
    Create a simulation with reward and curiosity.
    
    Args:
        reward_enabled: Whether to enable reward modulation
        curiosity_enabled: Whether to enable curiosity
        
    Returns:
        Tuple of (BrainWrapper, WorldWrapper)
    """
    brain_config = {}
    if reward_enabled:
        brain_config["neuromodulation.reward.enable"] = True
    if curiosity_enabled:
        brain_config["neuromodulation.curiosity.enable"] = True
    
    return NLMFactory.create_agent_simulation(brain_config=brain_config)
