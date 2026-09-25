"""
Configuration management for NLM (Neural Learning Machine).

This module provides enhanced configuration management with type hints,
validation, and builder patterns for easier setup and management.
"""

from typing import Dict, Any, Optional, List, Union, get_type_hints
from pathlib import Path
import json
from dataclasses import dataclass, field
import warnings

from .._core import Config as CConfig
from .exceptions import NLMConfigurationError


@dataclass
class BrainConfig:
    """Brain configuration parameters with type hints and validation."""
    
    # Core parameters
    neuron_count: int = 1000
    synapse_density: float = 0.1
    connection_probability: float = 0.05
    
    # Neuron properties
    v_thresh: float = -50.0
    v_rest: float = -70.0
    v_reset: float = -75.0
    tau_mem: float = 20.0
    tau_ref: float = 2.0
    
    # Learning parameters
    learning_rate: float = 0.001
    learning_enabled: bool = True
    plasticity_stdp_enabled: bool = True
    plasticity_hebbian_enabled: bool = True
    plasticity_structural_enabled: bool = True
    
    # Development
    development_enabled: bool = True
    critical_period_enabled: bool = True
    structural_plasticity_enabled: bool = True
    
    # Neuromodulation
    dopamine_scale: float = 1.0
    curiosity_enabled: bool = True
    novelty_enabled: bool = True
    
    # Simulation
    simulation_timestep: float = 0.1
    max_energy: float = 100.0
    energy_decay_rate: float = 0.01
    
    # Validation
    min_neuron_count: int = 10
    max_neuron_count: int = 100000
    min_learning_rate: float = 0.0
    max_learning_rate: float = 1.0
    
    def validate(self) -> None:
        """Validate configuration parameters."""
        if self.neuron_count < self.min_neuron_count:
            raise NLMConfigurationError(
                f"neuron_count ({self.neuron_count}) must be >= {self.min_neuron_count}"
            )
        if self.neuron_count > self.max_neuron_count:
            raise NLMConfigurationError(
                f"neuron_count ({self.neuron_count}) must be <= {self.max_neuron_count}"
            )
        
        if not (self.min_learning_rate <= self.learning_rate <= self.max_learning_rate):
            raise NLMConfigurationError(
                f"learning_rate ({self.learning_rate}) must be between "
                f"{self.min_learning_rate} and {self.max_learning_rate}"
            )
        
        if self.synapse_density < 0 or self.synapse_density > 1:
            raise NLMConfigurationError(
                f"synapse_density ({self.synapse_density}) must be between 0 and 1"
            )
        
        if self.connection_probability < 0 or self.connection_probability > 1:
            raise NLMConfigurationError(
                f"connection_probability ({self.connection_probability}) must be between 0 and 1"
            )
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        result = {}
        for field_name, field_value in self.__dict__.items():
            if not field_name.startswith('_'):
                result[field_name] = field_value
        return result
    
    @classmethod
    def from_dict(cls, config_dict: Dict[str, Any]) -> 'BrainConfig':
        """Create BrainConfig from dictionary."""
        # Filter out only valid fields
        valid_fields = {f.name for f in cls.__dataclass_fields__.values()}
        filtered_dict = {k: v for k, v in config_dict.items() if k in valid_fields}
        return cls(**filtered_dict)
    
    @classmethod
    def preset(cls, name: str) -> 'BrainConfig':
        """Get a predefined configuration preset."""
        presets = {
            'minimal': cls(neuron_count=100, learning_enabled=False),
            'standard': cls(),
            'large': cls(neuron_count=5000, synapse_density=0.2),
            'high_learning': cls(learning_rate=0.01, learning_enabled=True),
            'development': cls(development_enabled=True, critical_period_enabled=True),
            'curiosity': cls(curiosity_enabled=True, novelty_enabled=True),
        }
        
        if name not in presets:
            raise NLMConfigurationError(f"Unknown preset: {name}")
        
        return presets[name]


@dataclass
class WorldConfig:
    """World configuration parameters."""
    
    # World dimensions
    width: float = 20.0
    height: float = 20.0
    vision_width: int = 8
    vision_height: int = 8
    
    # Vision settings
    vision_enabled: bool = True
    vision_noisy: bool = False
    vision_angle_range: float = 90.0
    
    # Agent settings
    agent_start_x: float = 10.0
    agent_start_y: float = 10.0
    agent_max_energy: float = 100.0
    agent_health: float = 1.0
    agent_age_max: float = 1000.0
    
    # Resources and hazards
    resource_density: float = 0.1
    hazard_density: float = 0.05
    resource_value_range: tuple = (1.0, 10.0)
    hazard_damage_range: tuple = (0.1, 5.0)
    
    # World dynamics
    energy_decay_rate: float = 0.01
    max_energy: float = 100.0
    timestep: float = 0.1
    
    # Random seed
    random_seed: Optional[int] = None
    
    def validate(self) -> None:
        """Validate configuration parameters."""
        if self.width <= 0:
            raise NLMConfigurationError("width must be positive")
        if self.height <= 0:
            raise NLMConfigurationError("height must be positive")
        if self.vision_width <= 0:
            raise NLMConfigurationError("vision_width must be positive")
        if self.vision_height <= 0:
            raise NLMConfigurationError("vision_height must be positive")
        
        if self.agent_start_x < 0 or self.agent_start_x > self.width:
            raise NLMConfigurationError("agent_start_x must be within world width")
        if self.agent_start_y < 0 or self.agent_start_y > self.height:
            raise NLMConfigurationError("agent_start_y must be within world height")
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        result = {}
        for field_name, field_value in self.__dict__.items():
            if not field_name.startswith('_'):
                result[field_name] = field_value
        return result


class ConfigBuilder:
    """Builder pattern for creating configurations with method chaining."""
    
    def __init__(self):
        self._brain_config = BrainConfig()
        self._world_config = WorldConfig()
        self._agent_config = {}
        self._custom_params = {}
    
    def brain(self) -> 'BrainConfigBuilder':
        """Get brain configuration builder."""
        return BrainConfigBuilder(self._brain_config)
    
    def world(self) -> 'WorldConfigBuilder':
        """Get world configuration builder."""
        return WorldConfigBuilder(self._world_config)
    
    def agent(self, **kwargs) -> 'ConfigBuilder':
        """Set agent configuration parameters."""
        self._agent_config.update(kwargs)
        return self
    
    def custom(self, section: str, **kwargs) -> 'ConfigBuilder':
        """Set custom configuration parameters."""
        if section not in self._custom_params:
            self._custom_params[section] = {}
        self._custom_params[section].update(kwargs)
        return self
    
    def preset(self, name: str) -> 'ConfigBuilder':
        """Apply a configuration preset."""
        if name == 'minimal':
            self.brain().neuron_count(100).learning_enabled(False)
            self.world().resource_density(0.05).hazard_density(0.02)
        elif name == 'standard':
            self.brain().preset('standard')
            self.world().preset('standard')
        elif name == 'large':
            self.brain().neuron_count(5000).synapse_density(0.2)
            self.world().resource_density(0.2).hazard_density(0.1)
        elif name == 'development':
            self.brain().development_enabled(True).critical_period_enabled(True)
            self.agent().development_enabled(True)
        elif name == 'curiosity':
            self.brain().curiosity_enabled(True).novelty_enabled(True)
            self.agent().curiosity_enabled(True)
        else:
            raise NLMConfigurationError(f"Unknown preset: {name}")
        
        return self
    
    def build(self) -> Dict[str, Any]:
        """Build the complete configuration dictionary."""
        config = {
            'brain': self._brain_config.to_dict(),
            'world': self._world_config.to_dict(),
            'agent': self._agent_config,
            'custom': self._custom_params,
        }
        
        # Validate configurations
        self._brain_config.validate()
        self._world_config.validate()
        
        return config


class BrainConfigBuilder:
    """Builder for brain configuration with method chaining."""
    
    def __init__(self, brain_config: BrainConfig):
        self._brain_config = brain_config
    
    def neuron_count(self, count: int) -> 'BrainConfigBuilder':
        """Set number of neurons."""
        self._brain_config.neuron_count = count
        return self
    
    def synapse_density(self, density: float) -> 'BrainConfigBuilder':
        """Set synapse density."""
        self._brain_config.synapse_density = density
        return self
    
    def connection_probability(self, prob: float) -> 'BrainConfigBuilder':
        """Set connection probability."""
        self._brain_config.connection_probability = prob
        return self
    
    def learning_rate(self, rate: float) -> 'BrainConfigBuilder':
        """Set learning rate."""
        self._brain_config.learning_rate = rate
        return self
    
    def learning_enabled(self, enabled: bool) -> 'BrainConfigBuilder':
        """Enable/disable learning."""
        self._brain_config.learning_enabled = enabled
        return self
    
    def development_enabled(self, enabled: bool) -> 'BrainConfigBuilder':
        """Enable/disable development."""
        self._brain_config.development_enabled = enabled
        return self
    
    def curiosity_enabled(self, enabled: bool) -> 'BrainConfigBuilder':
        """Enable/disable curiosity."""
        self._brain_config.curiosity_enabled = enabled
        return self
    
    def novelty_enabled(self, enabled: bool) -> 'BrainConfigBuilder':
        """Enable/disable novelty detection."""
        self._brain_config.novelty_enabled = enabled
        return self
    
    def set(self, **kwargs) -> 'BrainConfigBuilder':
        """Set multiple parameters at once."""
        for key, value in kwargs.items():
            if hasattr(self._brain_config, key):
                setattr(self._brain_config, key, value)
            else:
                raise NLMConfigurationError(f"Unknown parameter: {key}")
        return self
    
    def get_config(self) -> BrainConfig:
        """Get the configured BrainConfig object."""
        return self._brain_config


class WorldConfigBuilder:
    """Builder for world configuration with method chaining."""
    
    def __init__(self, world_config: WorldConfig):
        self._world_config = world_config
    
    def dimensions(self, width: float, height: float) -> 'WorldConfigBuilder':
        """Set world dimensions."""
        self._world_config.width = width
        self._world_config.height = height
        return self
    
    def vision(self, width: int, height: int) -> 'WorldConfigBuilder':
        """Set vision dimensions."""
        self._world_config.vision_width = width
        self._world_config.vision_height = height
        return self
    
    def agent_start(self, x: float, y: float) -> 'WorldConfigBuilder':
        """Set agent starting position."""
        self._world_config.agent_start_x = x
        self._world_config.agent_start_y = y
        return self
    
    def resources(self, density: float) -> 'WorldConfigBuilder':
        """Set resource density."""
        self._world_config.resource_density = density
        return self
    
    def hazards(self, density: float) -> 'WorldConfigBuilder':
        """Set hazard density."""
        self._world_config.hazard_density = density
        return self
    
    def set(self, **kwargs) -> 'WorldConfigBuilder':
        """Set multiple parameters at once."""
        for key, value in kwargs.items():
            if hasattr(self._world_config, key):
                setattr(self._world_config, key, value)
            else:
                raise NLMConfigurationError(f"Unknown parameter: {key}")
        return self
    
    def get_config(self) -> WorldConfig:
        """Get the configured WorldConfig object."""
        return self._world_config


class Config:
    """Enhanced configuration manager with type hints and validation."""
    
    def __init__(
        self,
        brain_config: Optional[Union[BrainConfig, Dict[str, Any]]] = None,
        world_config: Optional[Union[WorldConfig, Dict[str, Any]]] = None,
        agent_config: Optional[Dict[str, Any]] = None,
        custom_params: Optional[Dict[str, Any]] = None,
    ):
        """Initialize configuration.
        
        Args:
            brain_config: Brain configuration (BrainConfig object or dict)
            world_config: World configuration (WorldConfig object or dict)
            agent_config: Agent configuration dictionary
            custom_params: Additional custom parameters
        """
        # Convert inputs to proper types
        self._brain_config = self._ensure_brain_config(brain_config)
        self._world_config = self._ensure_world_config(world_config)
        self._agent_config = agent_config or {}
        self._custom_params = custom_params or {}
        
        # Validate configurations
        self._brain_config.validate()
        self._world_config.validate()
    
    def _ensure_brain_config(
        self, config: Optional[Union[BrainConfig, Dict[str, Any]]]
    ) -> BrainConfig:
        """Ensure brain config is a BrainConfig object."""
        if config is None:
            return BrainConfig()
        elif isinstance(config, BrainConfig):
            return config
        elif isinstance(config, dict):
            return BrainConfig.from_dict(config)
        else:
            raise NLMConfigurationError(
                f"brain_config must be BrainConfig or dict, got {type(config)}"
            )
    
    def _ensure_world_config(
        self, config: Optional[Union[WorldConfig, Dict[str, Any]]]
    ) -> WorldConfig:
        """Ensure world config is a WorldConfig object."""
        if config is None:
            return WorldConfig()
        elif isinstance(config, WorldConfig):
            return config
        elif isinstance(config, dict):
            return WorldConfig.from_dict(config)
        else:
            raise NLMConfigurationError(
                f"world_config must be WorldConfig or dict, got {type(config)}"
            )
    
    # Properties for easy access
    @property
    def brain(self) -> BrainConfig:
        """Get brain configuration."""
        return self._brain_config
    
    @property
    def world(self) -> WorldConfig:
        """Get world configuration."""
        return self._world_config
    
    @property
    def agent(self) -> Dict[str, Any]:
        """Get agent configuration."""
        return self._agent_config.copy()
    
    @property
    def custom(self) -> Dict[str, Any]:
        """Get custom parameters."""
        return self._custom_params.copy()
    
    # Configuration methods
    def set_brain(self, **kwargs) -> 'Config':
        """Set brain configuration parameters."""
        self._brain_config.set(**kwargs)
        self._brain_config.validate()
        return self
    
    def set_world(self, **kwargs) -> 'Config':
        """Set world configuration parameters."""
        self._world_config.set(**kwargs)
        self._world_config.validate()
        return self
    
    def set_agent(self, **kwargs) -> 'Config':
        """Set agent configuration parameters."""
        self._agent_config.update(kwargs)
        return self
    
    def set_custom(self, section: str, **kwargs) -> 'Config':
        """Set custom configuration parameters."""
        if section not in self._custom_params:
            self._custom_params[section] = {}
        self._custom_params[section].update(kwargs)
        return self
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value with dot notation.
        
        Args:
            key: Configuration key (e.g., 'brain.neuron_count')
            default: Default value if key not found
            
        Returns:
            Configuration value
        """
        if '.' not in key:
            # Top-level key
            if key == 'brain':
                return self._brain_config.to_dict()
            elif key == 'world':
                return self._world_config.to_dict()
            elif key == 'agent':
                return self._agent_config.copy()
            elif key == 'custom':
                return self._custom_params.copy()
            else:
                return default
        
        # Nested key
        parts = key.split('.')
        current = self
        
        for part in parts:
            if hasattr(current, part):
                current = getattr(current, part)
            elif isinstance(current, dict) and part in current:
                current = current[part]
            else:
                return default
        
        return current

    def has(self, key: str) -> bool:
        """Check if configuration has a key.
        
        Args:
            key: Configuration key (e.g., 'brain.neuron_count')
            
        Returns:
            True if key exists, False otherwise
        """
        return self.get(key) is not None

    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        return {
            'brain': self._brain_config.to_dict(),
            'world': self._world_config.to_dict(),
            'agent': self._agent_config.copy(),
            'custom': self._custom_params.copy(),
        }

    def save(self, filepath: Union[str, Path]) -> None:
        """Save configuration to file.
        
        Args:
            filepath: Path to save configuration file
        """
        filepath = Path(filepath)
        
        # Create parent directories if they don't exist
        filepath.parent.mkdir(parents=True, exist_ok=True)
        
        # Save as JSON
        with open(filepath, 'w') as f:
            json.dump(self.to_dict(), f, indent=2)
    
    def load(self, filepath: Union[str, Path]) -> 'Config':
        """Load configuration from file.
        
        Args:
            filepath: Path to load configuration from
            
        Returns:
            Self for method chaining
        """
        filepath = Path(filepath)
        
        if not filepath.exists():
            raise NLMConfigurationError(f"Configuration file not found: {filepath}")
        
        # Load from JSON
        with open(filepath, 'r') as f:
            config_dict = json.load(f)
        
        # Reconstruct configuration
        brain_config = config_dict.get('brain')
        world_config = config_dict.get('world')
        agent_config = config_dict.get('agent', {})
        custom_params = config_dict.get('custom', {})
        
        return Config(
            brain_config=brain_config,
            world_config=world_config,
            agent_config=agent_config,
            custom_params=custom_params,
        )

    def summary(self) -> str:
        """Get configuration summary as string."""
        lines = ["Configuration Summary:"]
        lines.append(f"Brain: {self._brain_config.neuron_count} neurons")
        lines.append(f"  - Learning: {'enabled' if self._brain_config.learning_enabled else 'disabled'}")
        lines.append(f"  - Development: {'enabled' if self._brain_config.development_enabled else 'disabled'}")
        lines.append(f"World: {self._world_config.width}x{self._world_config.height}")
        lines.append(f"  - Vision: {self._world_config.vision_width}x{self._world_config.vision_height}")
        
        if self._agent_config:
            lines.append(f"Agent: {len(self._agent_config)} parameters")
        
        if self._custom_params:
            lines.append(f"Custom: {len(self._custom_params)} sections")
        
        return '\n'.join(lines)

    def __repr__(self) -> str:
        """Get string representation."""
        return f"Config(brain_neurons={self._brain_config.neuron_count}, " \
               f"world_size={self._world_config.width}x{self._world_config.height})"

    def __eq__(self, other: Any) -> bool:
        """Check equality with another configuration."""
        if not isinstance(other, Config):
            return False
        
        return (
            self._brain_config.to_dict() == other._brain_config.to_dict() and
            self._world_config.to_dict() == other._world_config.to_dict() and
            self._agent_config == other._agent_config and
            self._custom_params == other._custom_params
        )
