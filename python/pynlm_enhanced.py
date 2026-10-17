#!/usr/bin/env python3
"""
Enhanced NLM Python API - User-Friendly Wrapper

This module provides an enhanced, user-friendly API for the NLM (Neural Learning Machine)
framework. It wraps the existing pynlm bindings to provide:

- Context managers for resource management
- High-level convenience methods
- Better error handling and validation
- Utility functions for common operations
- Simplified configuration management
- Enhanced documentation and examples

Usage:
    import pynlm
    from pynlm_enhanced import NLM, Brain, World, Agent
    
    # Simple example
    with NLM() as nlm:
        brain = nlm.create_brain()
        brain.initialize()
        # Run simulation...
        brain.step(0, 0.0)
"""

import time
import json
import warnings
import os
from typing import Dict, Any, Optional, List, Union, ContextManager
from contextlib import contextmanager
import numpy as np

# Import the core NLM bindings
try:
    import pynlm
    NLM_AVAILABLE = True
except ImportError as e:
    NLM_AVAILABLE = False
    print(f"Warning: NLM core bindings not available: {e}")
    print("Enhanced API will not work without core NLM bindings.")

# Type aliases for better API design
NeuronType = pynlm.NeuronType if NLM_AVAILABLE else None
ActionType = pynlm.ActionType if NLM_AVAILABLE else None
WorldObjectType = pynlm.WorldObjectType if NLM_AVAILABLE else None
DevelopmentalStage = pynlm.DevelopmentalStage if NLM_AVAILABLE else None

# Configuration Templates
tEMPLATE_CONFIGS = {
    "basic": {
        "neuron_count": 500,
        "region_count": 2,
        "connection_probability": 0.15,
        "simulation_timestep": 0.001,
        "random_seed": 42,
        "enable_plasticity": True,
        "enable_neuromodulation": True,
        "enable_development": False,
        "enable_curiosity": False,
    },
    "learning": {
        "neuron_count": 1000,
        "region_count": 3,
        "connection_probability": 0.2,
        "simulation_timestep": 0.001,
        "random_seed": 123,
        "enable_plasticity": True,
        "enable_neuromodulation": True,
        "enable_development": True,
        "enable_curiosity": True,
    },
    "research": {
        "neuron_count": 5000,
        "region_count": 5,
        "connection_probability": 0.25,
        "simulation_timestep": 0.0002,
        "random_seed": 999,
        "enable_plasticity": True,
        "enable_neuromodulation": True,
        "enable_development": True,
        "enable_curiosity": True,
    }
}

class ValidationError(Exception):
    """Raised when configuration validation fails"""
    pass

class NLMError(Exception):
    """Base class for NLM-specific errors"""
    pass

class NLMSimulationError(NLMError):
    """Raised when simulation fails"""
    pass

class NLMResourceError(NLMError):
    """Raised when resource management fails"""
    pass

class NLMEnvironmentError(NLMError):
    """Raised when environment setup fails"""
    pass

def validate_config(config_dict: Dict[str, Any]) -> Dict[str, Any]:
    """
    Validate and normalize configuration dictionary.
    
    Args:
        config_dict: Configuration dictionary to validate
        
    Returns:
        Validated configuration dictionary
        
    Raises:
        ValidationError: If configuration is invalid
    """
    if not isinstance(config_dict, dict):
        raise ValidationError("Configuration must be a dictionary")
    
    # Create a copy to avoid modifying the original
    validated = config_dict.copy()
    
    # Set defaults for missing required parameters
    defaults = {
        "neuron_count": 100,
        "region_count": 1,
        "connection_probability": 0.15,
        "simulation_timestep": 0.001,
        "random_seed": 42,
        "enable_plasticity": True,
        "enable_neuromodulation": True,
        "enable_development": True,
        "enable_curiosity": False,
    }
    
    # Apply defaults
    for key, default_value in defaults.items():
        if key not in validated:
            validated[key] = default_value
    
    # Validate ranges
    if not isinstance(validated["neuron_count"], int) or validated["neuron_count"] < 1:
        raise ValidationError("neuron_count must be a positive integer")
    
    if not isinstance(validated["region_count"], int) or validated["region_count"] < 1:
        raise ValidationError("region_count must be a positive integer")
    
    if not 0.0 < validated["connection_probability"] <= 1.0:
        raise ValidationError("connection_probability must be between 0 and 1")
    
    if not 0.0 < validated["simulation_timestep"] <= 0.1:
        raise ValidationError("simulation_timestep must be between 0 and 0.1 seconds")
    
    if not isinstance(validated["random_seed"], int):
        raise ValidationError("random_seed must be an integer")
    
    # Validate boolean flags
    for flag in ["enable_plasticity", "enable_neuromodulation", 
                 "enable_development", "enable_curiosity"]:
        if not isinstance(validated.get(flag, False), bool):
            raise ValidationError(f"{flag} must be a boolean")
    
    return validated

def get_template(name: str) -> Dict[str, Any]:
    """
    Get a predefined configuration template.
    
    Args:
        name: Template name ('basic', 'learning', or 'research')
        
    Returns:
        Configuration dictionary
        
    Raises:
        ValueError: If template name is invalid
    """
    if name not in TEMPLATE_CONFIGS:
        raise ValueError(f"Unknown template: {name}. Available: {list(TEMPLATE_CONFIGS.keys())}")
    
    return TEMPLATE_CONFIGS[name].copy()

@contextmanager
def timer(description: str = "Operation"):
    """Context manager for timing operations"""
    start_time = time.time()
    try:
        yield
    finally:
        elapsed = time.time() - start_time
        print(f"{description} completed in {elapsed:.2f} seconds")

@contextmanager
def suppress_warnings():
    """Context manager to suppress warnings"""
    with warnings.catch_warnings():
        warnings.simplefilter("ignore")
        yield

class NLMResourceManager:
    """Manage NLM resources with proper cleanup"""
    
    def __init__(self):
        self.resources = []
    
    def register(self, resource: Any) -> 'NLMResourceManager':
        """Register a resource for cleanup"""
        self.resources.append(resource)
        return self
    
    def cleanup(self) -> None:
        """Clean up all registered resources"""
        for resource in self.resources:
            try:
                if hasattr(resource, 'cleanup'):
                    resource.cleanup()
                elif hasattr(resource, '__del__'):
                    resource.__del__()
                else:
                    # Try to clean up by setting to None
                    resource = None
            except Exception as e:
                print(f"Warning: Error cleaning up resource: {e}")
        
        self.resources.clear()
    
    def __enter__(self) -> 'NLMResourceManager':
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        self.cleanup()

class Config:
    """Enhanced configuration management"""
    
    def __init__(self, config_dict: Optional[Dict[str, Any]] = None, source: str = "manual"):
        """
        Initialize enhanced configuration.
        
        Args:
            config_dict: Configuration dictionary
            source: Source of configuration (manual, template, file)
        """
        if NLM_AVAILABLE:
            # Create base config
            self.base_config = pynlm.createDefaultConfig()
        else:
            self.base_config = None
        
        self.config_dict = validate_config(config_dict) if config_dict else {}
        self.source = source
        self.metadata = {
            "created": time.time(),
            "source": source,
            "version": "enhanced_1.0"
        }
    
    def apply_template(self, template_name: str) -> 'Config':
        """Apply a predefined template"""
        template = get_template(template_name)
        self.config_dict.update(template)
        self.source = f"template:{template_name}"
        return self
    
    def set(self, key: str, value: Any, source: str = "manual") -> 'Config':
        """Set configuration value"""
        self.config_dict[key] = value
        self.source = source
        return self
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value"""
        return self.config_dict.get(key, default)
    
    def has(self, key: str) -> bool:
        """Check if configuration has key"""
        return key in self.config_dict
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary"""
        return self.config_dict.copy()
    
    def apply_to_brain(self, brain: Any) -> 'Config':
        """Apply configuration to brain"""
        if not NLM_AVAILABLE:
            raise NLMResourceError("NLM bindings not available")
        
        for key, value in self.config_dict.items():
            if isinstance(value, bool):
                self.base_config.set(key, value, pynlm.ConfigSource.Runtime)
            elif isinstance(value, int):
                self.base_config.set(key, value, pynlm.ConfigSource.Runtime)
            elif isinstance(value, float):
                self.base_config.set(key, value, pynlm.ConfigSource.Runtime)
            elif isinstance(value, str):
                self.base_config.set(key, value, pynlm.ConfigSource.Runtime)
        
        return self
    
    def save(self, filepath: str) -> None:
        """Save configuration to file"""
        data = {
            "nlm_version": "enhanced_1.0",
            "config": self.config_dict,
            "metadata": self.metadata
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
    
    @classmethod
    def load(cls, filepath: str) -> 'Config':
        """Load configuration from file"""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        config_dict = data.get("config", {})
        metadata = data.get("metadata", {})
        
        instance = cls(config_dict, metadata.get("source", "file"))
        instance.metadata = metadata
        return instance
    
    def __repr__(self) -> str:
        return f"Config(source={self.source}, neurons={self.config_dict.get('neuron_count', 'N/A')})"

class EnhancedBrain:
    """Enhanced brain wrapper with user-friendly methods"""
    
    def __init__(self, config: Config, resource_manager: Optional[NLMResourceManager] = None):
        """
        Initialize enhanced brain.
        
        Args:
            config: Configuration object
            resource_manager: Resource manager for cleanup
        """
        if not NLM_AVAILABLE:
            raise NLMResourceError("NLM core bindings not available")
        
        self.config = config
        self.resource_manager = resource_manager or NLMResourceManager()
        self.brain = None
        self._initialized = False
        
        # Create brain
        self._create_brain()
        
        # Register for cleanup
        self.resource_manager.register(self)
    
    def _create_brain(self) -> None:
        """Create the underlying brain object"""
        self.brain = pynlm.createBrain(self.config.base_config)
        if not self.brain.initialize():
            raise NLMSimulationError("Failed to initialize brain")
        
        self._initialized = True
    
    def initialize(self, apply_config: bool = True) -> 'EnhancedBrain':
        """
        Initialize brain with optional configuration application.
        
        Args:
            apply_config: Whether to apply configuration settings
            
        Returns:
            Self for method chaining
        """
        if apply_config:
            self.config.apply_to_brain(self.brain)
        
        if not self.brain.initialize():
            raise NLMSimulationError("Brain initialization failed")
        
        self._initialized = True
        print(f"✓ Brain initialized with {self.brain.getTotalNeuronCount()} neurons")
        return self
    
    def step(self, step: int, timestamp: float = 0.0) -> 'EnhancedBrain':
        """
        Perform a simulation step.
        
        Args:
            step: Simulation step number
            timestamp: Current simulation time
            
        Returns:
            Self for method chaining
        """
        if not self._initialized:
            raise NLMSimulationError("Brain not initialized. Call initialize() first.")
        
        self.brain.step(step, timestamp)
        return self
    
    def inject_current(self, neuron_type: NeuronType, current: float) -> 'EnhancedBrain':
        """
        Inject current into neurons of specified type.
        
        Args:
            neuron_type: Type of neurons to inject into
            current: Current value to inject
            
        Returns:
            Self for method chaining
        """
        if not self._initialized:
            raise NLMSimulationError("Brain not initialized")
        
        self.brain.injectCurrentToNeurons(neuron_type, current)
        return self
    
    def inject_specific_neuron(self, neuron_id: Any, current: float) -> 'EnhancedBrain':
        """
        Inject current into specific neuron.
        
        Args:
            neuron_id: Specific neuron ID
            current: Current value to inject
            
        Returns:
            Self for method chaining
        """
        if not self._initialized:
            raise NLMSimulationError("Brain not initialized")
        
        self.brain.injectCurrent(neuron_id, current)
        return self
    
    def produce_action(self) -> Any:
        """
        Produce motor action based on brain activity.
        
        Returns:
            Action object or None if no action produced
        """
        if not self._initialized:
            raise NLMSimulationError("Brain not initialized")
        
        return self.brain.produceAction()
    
    def reset(self) -> 'EnhancedBrain':
        """Reset brain state."""
        if self._initialized:
            self.brain.reset()
            self._initialized = False
        return self
    
    def save(self, filepath: str) -> 'EnhancedBrain':
        """
        Save brain state to file.
        
        Args:
            filepath: File path to save brain state
            
        Returns:
            Self for method chaining
        """
        if not self._initialized:
            raise NLMSimulationError("Brain not initialized")
        
        success = self.brain.save(filepath)
        if not success:
            raise NLMSimulationError(f"Failed to save brain state to {filepath}")
        
        print(f"✓ Brain state saved to {filepath}")
        return self
    
    def load(self, filepath: str) -> 'EnhancedBrain':
        """
        Load brain state from file.
        
        Args:
            filepath: File path to load brain state from
            
        Returns:
            Self for method chaining
        """
        if not self.brain.load(filepath):
            raise NLMSimulationError(f"Failed to load brain state from {filepath}")
        
        self._initialized = True
        print(f"✓ Brain state loaded from {filepath}")
        return self
    
    def get_stats(self) -> Dict[str, Any]:
        """
        Get comprehensive brain statistics.
        
        Returns:
            Dictionary of brain statistics
        """
        if not self._initialized:
            raise NLMSimulationError("Brain not initialized")
        
        stats = {
            "neuron_count": self.brain.getTotalNeuronCount(),
            "synapse_count": self.brain.getTotalSynapseCount(),
            "active_neurons": self.brain.getActiveNeuronCount(),
            "firing_neurons": self.brain.getFiringNeuronCount(),
            "total_spikes": self.brain.getTotalSpikeCount(),
            "average_firing_rate": self.brain.getAverageFiringRate(),
            "excitatory_inhibition_ratio": self.brain.getExcitationInhibitionRatio(),
            "region_count": self.brain.getRegionCount(),
            "developmental_stage": str(self.brain.getDevelopmentalStage()),
        }
        
        return stats
    
    def log_status(self) -> None:
        """Log comprehensive brain status."""
        if not self._initialized:
            raise NLMSimulationError("Brain not initialized")
        
        stats = self.get_stats()
        
        print("Brain Status:")
        print("=" * 40)
        print(f"Total neurons: {stats['neuron_count']}")
        print(f"Total synapses: {stats['synapse_count']}")
        print(f"Active neurons: {stats['active_neurons']}")
        print(f"Firing neurons: {stats['firing_neurons']}")
        print(f"Total spikes: {stats['total_spikes']}")
        print(f"Average firing rate: {stats['average_firing_rate']:.3f} Hz")
        print(f"E/I ratio: {stats['excitatory_inhibition_ratio']:.3f}")
        print(f"Regions: {stats['region_count']}")
        print(f"Developmental stage: {stats['developmental_stage']}")
        
        # Call underlying logStatus if available
        try:
            self.brain.logStatus()
        except AttributeError:
            pass
    
    def cleanup(self) -> None:
        """Clean up brain resources."""
        if hasattr(self.brain, 'cleanup'):
            self.brain.cleanup()
        
        self.brain = None
        self._initialized = False
    
    def __enter__(self) -> 'EnhancedBrain':
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        self.cleanup()
    
    def __repr__(self) -> str:
        status = "initialized" if self._initialized else "not initialized"
        return f"EnhancedBrain(status={status}, neurons={self.config.get('neuron_count', 'N/A')})"

class EnhancedWorld:
    """Enhanced world wrapper with user-friendly methods"""
    
    def __init__(self, config: Optional[Dict[str, Any]] = None):
        """
        Initialize enhanced world.
        
        Args:
            config: World configuration
        """
        if not NLM_AVAILABLE:
            raise NLMResourceError("NLM core bindings not available")
        
        self.config = config or {}
        self.world = None
        self.resource_manager = NLMResourceManager()
        
        # Create world
        self._create_world()
        
        # Register for cleanup
        self.resource_manager.register(self)
    
    def _create_world(self) -> None:
        """Create the underlying world object"""
        self.world = pynlm.createSimpleWorld()
        
        # Configure if parameters provided
        if self.config:
            width = self.config.get("world_width", 64)
            height = self.config.get("world_height", 64)
            vision_width = self.config.get("vision_width", 16)
            vision_height = self.config.get("vision_height", 16)
            
            self.world.configure(width, height, vision_width, vision_height)
            
            if "random_seed" in self.config:
                self.world.setRandomSeed(self.config["random_seed"])
    
    def configure(self, width: int, height: int, vision_width: int = 16, 
                  vision_height: int = 16, random_seed: Optional[int] = None) -> 'EnhancedWorld':
        """
        Configure world dimensions and parameters.
        
        Args:
            width: World width
            height: World height
            vision_width: Vision input width
            vision_height: Vision input height
            random_seed: Random seed for world generation
            
        Returns:
            Self for method chaining
        """
        self.world.configure(width, height, vision_width, vision_height)
        
        if random_seed is not None:
            self.world.setRandomSeed(random_seed)
        
        # Update config
        self.config.update({
            "world_width": width,
            "world_height": height,
            "vision_width": vision_width,
            "vision_height": vision_height,
        })
        
        if random_seed is not None:
            self.config["random_seed"] = random_seed
        
        print(f"✓ World configured: {width}x{height}, vision: {vision_width}x{vision_height}")
        return self
    
    def add_object(self, obj: Any) -> 'EnhancedWorld':
        """
        Add object to world.
        
        Args:
            obj: World object to add
            
        Returns:
            Self for method chaining
        """
        if not self.world:
            raise NLMResourceError("World not created")
        
        self.world.addObject(obj)
        return self
    
    def remove_object(self, x: float, y: float) -> 'EnhancedWorld':
        """
        Remove object from world at position.
        
        Args:
            x: X coordinate
            y: Y coordinate
            
        Returns:
            Self for method chaining
        """
        if not self.world:
            raise NLMResourceError("World not created")
        
        self.world.removeObject(x, y)
        return self
    
    def get_agent_body(self) -> Any:
        """Get agent body state."""
        if not self.world:
            raise NLMResourceError("World not created")
        
        return self.world.getAgentBody()
    
    def get_sensory_percept(self) -> Any:
        """Get sensory percept from world."""
        if not self.world:
            raise NLMResourceError("World not created")
        
        return self.world.getSensoryPercept()
    
    def update(self, timestep: float) -> 'EnhancedWorld':
        """
        Update world state.
        
        Args:
            timestep: Time step to advance
            
        Returns:
            Self for method chaining
        """
        if not self.world:
            raise NLMResourceError("World not created")
        
        self.world.update(timestep)
        return self
    
    def reset(self) -> 'EnhancedWorld':
        """Reset world state."""
        if self.world:
            self.world.reset()
        return self
    
    def get_stats(self) -> Dict[str, Any]:
        """
        Get world statistics.
        
        Returns:
            Dictionary of world statistics
        """
        if not self.world:
            raise NLMResourceError("World not created")
        
        body = self.world.getAgentBody()
        
        stats = {
            "width": self.world.getWidth(),
            "height": self.world.getHeight(),
            "max_energy": self.world.getMaxEnergy(),
            "energy_decay_rate": self.world.getEnergyDecayRate(),
            "simulation_time": self.world.getSimulationTime(),
            "agent_x": body.x,
            "agent_y": body.y,
            "agent_energy": body.energy,
            "agent_age": body.age,
            "agent_is_moving": body.isMoving,
            "agent_is_turning": body.isTurning,
        }
        
        return stats
    
    def log_status(self) -> None:
        """Log world status."""
        stats = self.get_stats()
        
        print("World Status:")
        print("=" * 40)
        print(f"Dimensions: {stats['width']}x{stats['height']}")
        print(f"Agent position: ({stats['agent_x']:.1f}, {stats['agent_y']:.1f})")
        print(f"Agent energy: {stats['agent_energy']:.1f}")
        print(f"Agent age: {stats['agent_age']}")
        print(f"Agent moving: {stats['agent_is_moving']}")
        print(f"Agent turning: {stats['agent_is_turning']}")
        print(f"Simulation time: {stats['simulation_time']:.2f}")
        print(f"Max energy: {stats['max_energy']:.1f}")
        print(f"Energy decay: {stats['energy_decay_rate']:.4f}")
    
    def create_predefined_objects(self) -> 'EnhancedWorld':
        """
        Create and add predefined objects to world.
        
        Returns:
            Self for method chaining
        """
        # Add resource (food)
        resource = pynlm.WorldObject()
        resource.x = self.config.get("world_width", 64) * 0.3
        resource.y = self.config.get("world_height", 64) * 0.3
        resource.type = pynlm.WorldObjectType.Resource
        resource.value = 10.0
        self.add_object(resource)
        
        # Add hazard (wall)
        wall = pynlm.WorldObject()
        wall.x = self.config.get("world_width", 64) * 0.7
        wall.y = self.config.get("world_height", 64) * 0.7
        wall.type = pynlm.WorldObjectType.Wall
        wall.radius = 2.0
        self.add_object(wall)
        
        print("✓ Predefined objects added to world")
        return self
    
    def cleanup(self) -> None:
        """Clean up world resources."""
        if hasattr(self.world, 'cleanup'):
            self.world.cleanup()
        
        self.world = None
    
    def __enter__(self) -> 'EnhancedWorld':
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        self.cleanup()

class EnhancedAgent:
    """Enhanced agent wrapper with user-friendly methods"""
    
    def __init__(self, brain: EnhancedBrain, world: EnhancedWorld, 
                 resource_manager: Optional[NLMResourceManager] = None):
        """
        Initialize enhanced agent.
        
        Args:
            brain: Enhanced brain instance
            world: Enhanced world instance
            resource_manager: Resource manager for cleanup
        """
        if not NLM_AVAILABLE:
            raise NLMResourceError("NLM core bindings not available")
        
        self.brain = brain
        self.world = world
        self.resource_manager = resource_manager or NLMResourceManager()
        
        # Create agent brain interface
        self.agent_brain = pynlm.createAgentBrain(brain.brain)
        self.agent_brain.initialize(world.world)
        
        # Register for cleanup
        self.resource_manager.register(self)
    
    def enable_system(self, system: str, enable: bool = True) -> 'EnhancedAgent':
        """
        Enable or disable an agent system.
        
        Args:
            system: System name ('reward_modulation', 'structural_plasticity',
                   'development', 'curiosity')
            enable: Whether to enable the system
            
        Returns:
            Self for method chaining
        """
        if system == "reward_modulation":
            self.agent_brain.enableRewardModulation(enable)
        elif system == "structural_plasticity":
            self.agent_brain.enableStructuralPlasticity(enable)
        elif system == "development":
            self.agent_brain.enableDevelopment(enable)
        elif system == "curiosity":
            self.agent_brain.enableCuriosity(enable)
        else:
            raise ValueError(f"Unknown system: {system}")
        
        print(f"✓ {system} {'enabled' if enable else 'disabled'}")
        return self
    
    def process_sensory_input(self, percept: Any) -> 'EnhancedAgent':
        """
        Process sensory input.
        
        Args:
            percept: Sensory percept to process
            
        Returns:
            Self for method chaining
        """
        self.agent_brain.processSensoryInput(percept)
        return self
    
    def decode_motor_command(self) -> Any:
        """
        Decode motor command from brain activity.
        
        Returns:
            Motor command or None
        """
        return self.agent_brain.decodeMotorCommand()
    
    def apply_reward_modulation(self, reward: float, predicted_reward: float = 0.0) -> 'EnhancedAgent':
        """
        Apply reward-based neuromodulation.
        
        Args:
            reward: Actual reward
            predicted_reward: Predicted reward (prediction error)
            
        Returns:
            Self for method chaining
        """
        self.agent_brain.applyRewardModulation(reward, predicted_reward)
        return self
    
    def update_development(self, timestep: float) -> 'EnhancedAgent':
        """
        Update development system.
        
        Args:
            timestep: Time step to advance development
            
        Returns:
            Self for method chaining
        """
        self.agent_brain.updateDevelopment(timestep)
        return self
    
    def get_developmental_stage(self) -> DevelopmentalStage:
        """Get current developmental stage."""
        return self.agent_brain.getDevelopmentalStage()
    
    def get_neuromodulation_level(self) -> float:
        """Get current neuromodulation level."""
        return self.agent_brain.getNeuromodulationLevel()
    
    def get_curiosity_level(self) -> float:
        """Get curiosity level."""
        return self.agent_brain.getCuriosityLevel()
    
    def get_novelty_level(self) -> float:
        """Get novelty level."""
        return self.agent_brain.getNoveltyLevel()
    
    def get_prediction_error(self) -> float:
        """Get prediction error signal."""
        return self.agent_brain.getPredictionError()
    
    def reset(self) -> 'EnhancedAgent':
        """Reset agent state."""
        self.agent_brain.reset()
        return self
    
    def get_stats(self) -> Dict[str, Any]:
        """
        Get agent statistics.
        
        Returns:
            Dictionary of agent statistics
        """
        stats = {
            "developmental_stage": str(self.get_developmental_stage()),
            "neuromodulation_level": self.get_neuromodulation_level(),
            "curiosity_level": self.get_curiosity_level(),
            "novelty_level": self.get_novelty_level(),
            "prediction_error": self.get_prediction_error(),
        }
        
        return stats
    
    def log_status(self) -> None:
        """Log agent status."""
        stats = self.get_stats()
        
        print("Agent Status:")
        print("=" * 40)
        print(f"Developmental stage: {stats['developmental_stage']}")
        print(f"Neuromodulation level: {stats['neuromodulation_level']:.3f}")
        print(f"Curiosity level: {stats['curiosity_level']:.3f}")
        print(f"Novelty level: {stats['novelty_level']:.3f}")
        print(f"Prediction error: {stats['prediction_error']:.3f}")
    
    def cleanup(self) -> None:
        """Clean up agent resources."""
        if hasattr(self.agent_brain, 'cleanup'):
            self.agent_brain.cleanup()
    
    def __enter__(self) -> 'EnhancedAgent':
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        self.cleanup()

class NLM:
    """Main NLM interface with convenience methods"""
    
    def __init__(self, config: Optional[Union[Dict[str, Any], Config]] = None):
        """
        Initialize NLM system.
        
        Args:
            config: Configuration (dictionary or Config object)
        """
        self.resource_manager = NLMResourceManager()
        
        # Handle configuration
        if config is None:
            self.config = Config()
        elif isinstance(config, Config):
            self.config = config
        else:
            self.config = Config(config)
        
        # Initialize resources
        self.brain = None
        self.world = None
        self.agent = None
    
    def create_config(self, template: Optional[str] = None, 
                     **kwargs) -> Config:
        """
        Create configuration with optional template and overrides.
        
        Args:
            template: Template name ('basic', 'learning', 'research')
            **kwargs: Configuration overrides
            
        Returns:
            Configured Config object
        """
        if template:
            self.config.apply_template(template)
        
        for key, value in kwargs.items():
            self.config.set(key, value)
        
        return self.config
    
    def create_brain(self, config: Optional[Config] = None) -> EnhancedBrain:
        """
        Create enhanced brain.
        
        Args:
            config: Configuration (uses default if None)
            
        Returns:
            Enhanced brain instance
        """
        if config is None:
            config = self.config
        
        self.brain = EnhancedBrain(config, self.resource_manager)
        return self.brain
    
    def create_world(self, config: Optional[Dict[str, Any]] = None) -> EnhancedWorld:
        """
        Create enhanced world.
        
        Args:
            config: World configuration
            
        Returns:
            Enhanced world instance
        """
        self.world = EnhancedWorld(config)
        return self.world
    
    def create_agent(self, brain: Optional[EnhancedBrain] = None, 
                    world: Optional[EnhancedWorld] = None) -> EnhancedAgent:
        """
        Create enhanced agent.
        
        Args:
            brain: Brain instance (creates default if None)
            world: World instance (creates default if None)
            
        Returns:
            Enhanced agent instance
        """
        if brain is None:
            brain = self.create_brain()
        
        if world is None:
            world = self.create_world()
        
        self.agent = EnhancedAgent(brain, world, self.resource_manager)
        return self.agent
    
    def run_basic_simulation(self, steps: int = 100) -> Dict[str, Any]:
        """
        Run basic neural simulation.
        
        Args:
            steps: Number of simulation steps
            
        Returns:
            Simulation metrics
        """
        print("Running basic simulation...")
        
        with timer("Basic Simulation"):
            # Create components
            brain = self.create_brain()
            brain.initialize()
            
            # Run simulation
            for step in range(steps):
                brain.step(step, step * 0.001)
                
                # Add some input at specific steps
                if step == steps // 4:
                    brain.inject_current(pynlm.NeuronType.Sensory, 20.0)
                elif step == (steps // 2):
                    brain.inject_current(pynlm.NeuronType.Motor, 15.0)
                
                if (step + 1) % (steps // 10) == 0:
                    print(f"  Step {step + 1}/{steps}")
            
            # Get stats
            stats = brain.get_stats()
            stats["total_steps"] = steps
            
            print(f"✓ Basic simulation completed in {steps} steps")
            return stats
    
    def run_world_interaction(self, episodes: int = 5, 
                             steps_per_episode: int = 100) -> Dict[str, Any]:
        """
        Run world interaction simulation.
        
        Args:
            episodes: Number of episodes
            steps_per_episode: Steps per episode
            
        Returns:
            Simulation metrics
        """
        print("Running world interaction simulation...")
        
        with timer("World Interaction Simulation"):
            # Create components
            brain = self.create_brain()
            world = self.create_world()
            agent = self.create_agent(brain, world)
            
            # Enable systems
            agent.enable_system("reward_modulation", True)
            agent.enable_system("development", True)
            
            # Create objects
            world.create_predefined_objects()
            
            # Run episodes
            episode_metrics = []
            
            for episode in range(episodes):
                print(f"\nEpisode {episode + 1}/{episodes}:")
                
                # Reset
                world.reset()
                agent.reset()
                brain.reset()
                brain.initialize()
                world.create_predefined_objects()
                
                # Run episode
                for step in range(steps_per_episode):
                    # Get sensory input
                    percept = world.get_sensory_percept()
                    
                    # Process input
                    agent.process_sensory_input(percept)
                    
                    # Run neural simulation
                    for neural_step in range(5):
                        brain.step(neural_step, neural_step * 0.001)
                    
                    # Produce action
                    action = brain.produce_action()
                    
                    # Update development
                    agent.update_development(0.01)
                    
                    # Update world
                    world.update(0.01)
                    
                    # Log progress
                    if (step + 1) % (steps_per_episode // 5) == 0:
                        body = world.get_agent_body()
                        print(f"  Step {step + 1}: pos=({body.x:.1f},{body.y:.1f}), energy={body.energy:.1f}")
                
                # Record episode metrics
                world_stats = world.get_stats()
                brain_stats = brain.get_stats()
                agent_stats = agent.get_stats()
                
                episode_metric = {
                    "episode": episode + 1,
                    "world": world_stats,
                    "brain": brain_stats,
                    "agent": agent_stats,
                }
                
                episode_metrics.append(episode_metric)
            
            # Calculate overall metrics
            final_world_stats = world.get_stats()
            final_brain_stats = brain.get_stats()
            final_agent_stats = agent.get_stats()
            
            overall_metrics = {
                "episodes": episodes,
                "steps_per_episode": steps_per_episode,
                "final_world": final_world_stats,
                "final_brain": final_brain_stats,
                "final_agent": final_agent_stats,
                "episode_metrics": episode_metrics,
            }
            
            print(f"✓ World interaction simulation completed")
            return overall_metrics
    
    def run_learning_simulation(self, episodes: int = 10, 
                               steps_per_episode: int = 200) -> Dict[str, Any]:
        """
        Run learning simulation with development and plasticity.
        
        Args:
            episodes: Number of episodes
            steps_per_episode: Steps per episode
            
        Returns:
            Simulation metrics
        """
        print("Running learning simulation...")
        
        with timer("Learning Simulation"):
            # Create components with learning configuration
            self.config.apply_template("learning")
            brain = self.create_brain()
            world = self.create_world()
            agent = self.create_agent(brain, world)
            
            # Enable all systems for learning
            agent.enable_system("reward_modulation", True)
            agent.enable_system("structural_plasticity", True)
            agent.enable_system("development", True)
            agent.enable_system("curiosity", True)
            
            # Create objects
            world.create_predefined_objects()
            
            # Learning metrics
            learning_metrics = {
                "episodes": episodes,
                "steps_per_episode": steps_per_episode,
                "reward_history": [],
                "development_stages": [],
                "firing_rate_history": [],
                "position_history": [],
            }
            
            for episode in range(episodes):
                print(f"\nLearning Episode {episode + 1}/{episodes}:")
                
                # Reset and record initial state
                world.reset()
                agent.reset()
                brain.reset()
                brain.initialize()
                world.create_predefined_objects()
                
                episode_reward = 0.0
                episode_firing_rates = []
                episode_positions = []
                initial_stage = agent.get_developmental_stage()
                
                # Run episode
                for step in range(steps_per_episode):
                    # Get sensory input
                    percept = world.get_sensory_percept()
                    
                    # Process input
                    agent.process_sensory_input(percept)
                    
                    # Run neural simulation with occasional learning input
                    for neural_step in range(3):
                        brain.step(neural_step, neural_step * 0.001)
                        
                        # Inject learning signals
                        if step % 10 == 0:
                            brain.inject_current(pynlm.NeuronType.Sensory, 30.0)
                    
                    # Produce action
                    action = brain.produce_action()
                    
                    # Apply reward (simplified)
                    if action:
                        episode_reward += 1.0
                        agent.apply_reward_modulation(1.0, 0.5)
                    
                    # Update development
                    agent.update_development(0.01)
                    
                    # Update world
                    world.update(0.01)
                    
                    # Record metrics
                    episode_firing_rates.append(brain.get_average_firing_rate())
                    body = world.get_agent_body()
                    episode_positions.append((body.x, body.y))
                    
                    if (step + 1) % (steps_per_episode // 10) == 0:
                        print(f"  Step {step + 1}: reward={episode_reward:.1f}, "
                              f"firing_rate={brain.get_average_firing_rate():.3f}, "
                              f"pos=({body.x:.1f},{body.y:.1f})")
                
                # Record episode results
                learning_metrics["reward_history"].append(episode_reward)
                learning_metrics["development_stages"].append(agent.get_developmental_stage().name)
                learning_metrics["firing_rate_history"].append(episode_firing_rates)
                learning_metrics["position_history"].append(episode_positions)
                
                print(f"  Episode completed: reward={episode_reward:.1f}, "
                      f"final_stage={agent.get_developmental_stage().name}")
            
            # Calculate learning statistics
            final_reward = episode_reward
            avg_reward = sum(learning_metrics["reward_history"]) / len(learning_metrics["reward_history"])
            initial_firing = learning_metrics["firing_rate_history"][0][0]
            final_firing = learning_metrics["firing_rate_history"][-1][-1]
            
            learning_summary = {
                "total_episodes": episodes,
                "final_reward": final_reward,
                "average_reward": avg_reward,
                "initial_firing_rate": initial_firing,
                "final_firing_rate": final_firing,
                "firing_rate_change": final_firing - initial_firing,
                "development_progression": learning_metrics["development_stages"],
                "position_changes": len(set(tuple(pos) for pos in learning_metrics["position_history"][-1])),
            }
            
            print(f"✓ Learning simulation completed")
            print(f"  Average reward: {avg_reward:.2f}")
            print(f"  Firing rate change: {learning_summary['firing_rate_change']:.3f}")
            print(f"  Development stages: {learning_summary['development_progression']}")
            
            return {
                "learning_metrics": learning_metrics,
                "summary": learning_summary,
            }
    
    def save_simulation(self, filepath: str) -> 'NLM':
        """
        Save current simulation state to file.
        
        Args:
            filepath: File path to save
            
        Returns:
            Self for method chaining
        """
        if self.brain:
            self.brain.save(filepath)
        
        # Also save configuration
        config_file = filepath.replace('.nlm', '_config.json')
        self.config.save(config_file)
        
        print(f"✓ Simulation saved to {filepath}")
        print(f"✓ Configuration saved to {config_file}")
        
        return self
    
    def load_simulation(self, filepath: str) -> 'NLM':
        """
        Load simulation state from file.
        
        Args:
            filepath: File path to load
            
        Returns:
            Self for method chaining
        """
        if self.brain:
            self.brain.load(filepath)
        
        # Load configuration if available
        config_file = filepath.replace('.nlm', '_config.json')
        if os.path.exists(config_file):
            loaded_config = Config.load(config_file)
            self.config = loaded_config
        
        print(f"✓ Simulation loaded from {filepath}")
        return self
    
    def cleanup(self) -> None:
        """Clean up all resources."""
        self.resource_manager.cleanup()
        
        self.brain = None
        self.world = None
        self.agent = None
    
    def __enter__(self) -> 'NLM':
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        self.cleanup()

# Convenience functions for common operations
def create_demo_simulation(template: str = "basic", 
                          **kwargs) -> NLM:
    """
    Create demo simulation with specified template.
    
    Args:
        template: Configuration template ('basic', 'learning', 'research')
        **kwargs: Configuration overrides
        
    Returns:
        Configured NLM instance
    """
    nlm = NLM()
    nlm.create_config(template, **kwargs)
    return nlm

def run_quick_demo() -> Dict[str, Any]:
    """Run quick demonstration of NLM capabilities."""
    print("Quick NLM Demo")
    print("=" * 50)
    
    nlm = NLM()
    
    try:
        # Basic simulation
        print("1. Basic neural simulation...")
        stats1 = nlm.run_basic_simulation(50)
        
        print("\n2. Loading configuration...")
        nlm.config.apply_template("learning")
        
        print("3. World interaction simulation...")
        stats2 = nlm.run_world_interaction(3, 50)
        
        print("\n4. Learning simulation...")
        stats3 = nlm.run_learning_simulation(5, 30)
        
        print("\n✓ All demonstrations completed successfully!")
        
        return {
            "basic_simulation": stats1,
            "world_interaction": stats2,
            "learning_simulation": stats3,
        }
        
    except Exception as e:
        print(f"✗ Demo failed: {e}")
        raise
    finally:
        nlm.cleanup()

# Version information
__version__ = "1.0.0"
__author__ = "NLM Project"
__license__ = "MIT"

# For backward compatibility
if NLM_AVAILABLE:
    # Add enhanced versions to pynlm module if it exists
    try:
        pynlm.Config = Config
        pynlm.Brain = EnhancedBrain
        pynlm.World = EnhancedWorld
        pynlm.Agent = EnhancedAgent
        pynlm.NLM = NLM
    except:
        pass

if __name__ == "__main__":
    if NLM_AVAILABLE:
        run_quick_demo()
    else:
        print("Error: NLM core bindings not available.")
        print("Please build NLM with Python bindings first.")
        sys.exit(1)