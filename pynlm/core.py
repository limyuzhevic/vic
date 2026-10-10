"""
Pythonic interface for NLM configuration management.

This module provides a clean, Pythonic API for configuring the NLM neural
simulation system. It wraps the low-level C++ Config class with type hints,
docstrings, and convenient methods.
"""

# Import types from the C++ extension
from pynlm import NeuronId, SynapseId, RegionId, PopulationId
from pynlm import NeuronType, SynapseType, DevelopmentalStage, FiringState
from pynlm import ActionType, MotorCommand, WorldObjectType

# Import the C++ extension module directly
import pynlm as _c_extension
class ConfigError(Exception):
    """Raised when configuration operations fail."""
    pass
class Config:
    """Configuration class for NLM system.
    
    Manages brain simulation parameters including neuron counts,
    region settings, plasticity rules, and simulation behavior.
    
    The class provides both attribute-style and dictionary-style access
    to configuration values, along with comprehensive validation and
    convenience methods for common operations.
    
    Attributes:
        neuron_count: Number of neurons in the simulation
        region_count: Number of neural regions
        connection_probability: Probability of synaptic connections
        
    Examples:
        >>> config = Config()
        >>> config.neuron_count = 100
        >>> config.region_count = 2
        >>> config.connection_probability = 0.1
        >>> 
        >>> # Dictionary-style access
        >>> if config.has("neuron_count"):
        ...     print(config["neuron_count"])
        >>> 
        >>> # Save to file
        >>> config.save_to_file("brain_config.json")
        >>> 
        >>> # Load from file  
        >>> new_config = Config().load_from_file("brain_config.json")
    """
    
    def __init__(self, **kwargs) -> None:
        """Create a configuration, optionally with initial values.
        
        Args:
            **kwargs: Initial configuration values to set.
            
        Examples:
            >>> config = Config(neuron_count=100, region_count=1)
            >>> config = Config()  # Default configuration
        """
        self._config = _c_extension.Config()
        
        # Apply initial values if provided
        for key, value in kwargs.items():
            self.set(key, value)
    
    def load_from_file(self, filepath: Union[str, Path]) -> 'Config':
        """Load configuration from a JSON file.
        
        Args:
            filepath: Path to the JSON configuration file.
            
        Returns:
            Self for method chaining.
            
        Raises:
            ConfigError: If file cannot be read or parsed.
            
        Examples:
            >>> config = Config().load_from_file("config.json")
        """
        try:
            path = Path(filepath)
            if not path.exists():
                raise ConfigError(f"Configuration file not found: {filepath}")
                
            content = path.read_text()
            data = json.loads(content)
            
            for key, value in data.items():
                self.set(key, value)
                
            return self
        except (json.JSONDecodeError, OSError) as e:
            raise ConfigError(f"Failed to load configuration from {filepath}: {e}")
    
    def save_to_file(self, filepath: Union[str, Path], indent: int = 2) -> bool:
        """Save configuration to a JSON file.
        
        Args:
            filepath: Path where to save the JSON configuration.
            indent: JSON indentation level for pretty printing.
            
        Returns:
            True if successful, False otherwise.
            
        Raises:
            ConfigError: If file cannot be written.
            
        Examples:
            >>> config.save_to_file("brain_config.json")
        """
        try:
            path = Path(filepath)
            path.parent.mkdir(parents=True, exist_ok=True)
            
            data = self.to_dict()
            path.write_text(json.dumps(data, indent=indent))
            return True
        except OSError as e:
            raise ConfigError(f"Failed to save configuration to {filepath}: {e}")
    
    def has(self, key: str) -> bool:
        """Check if a configuration key exists.
        
        Args:
            key: Configuration key to check.
            
        Returns:
            True if the key exists, False otherwise.
            
        Examples:
            >>> config.has("neuron_count")  # Returns True/False
        """
        return self._config.has(key)
    
    def get_keys(self) -> List[str]:
        """Get all configuration keys.
        
        Returns:
            List of all configuration key names.
            
        Examples:
            >>> keys = config.get_keys()
            >>> print(f"Configuration keys: {keys}")
        """
        return self._config.getKeys()
    
    def clear(self) -> None:
        """Clear all configuration entries to defaults.
        
        Examples:
            >>> config.clear()
        """
        self._config.clear()
    
    def summary(self) -> str:
        """Get a formatted summary of the configuration.
        
        Returns:
            Formatted string describing current configuration.
            
        Examples:
            >>> print(config.summary())
        """
        return self._config.summary()
    
    def set(self, key: str, value: Any, source: str = "manual") -> None:
        """Set a configuration value.
        
        Args:
            key: Configuration key to set.
            value: Value to set. Will be converted to appropriate type.
            source: Source of the configuration ("manual", "default", "simulation").
            
        Raises:
            ConfigError: If key is invalid or value cannot be set.
            
        Examples:
            >>> config.set("neuron_count", 100)
            >>> config.set("connection_probability", 0.1, "simulation")
        """
        try:
            self._config.set(key, value)
        except Exception as e:
            raise ConfigError(f"Failed to set configuration key '{key}': {e}")
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value with optional default.
        
        Args:
            key: Configuration key to get.
            default: Default value if key doesn't exist.
            
        Returns:
            Configuration value or default if key not found.
            
        Examples:
            >>> count = config.get("neuron_count", 50)
        """
        if self.has(key):
            return self._config.get(key)
        return default
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary.
        
        Returns:
            Dictionary containing all configuration values.
            
        Examples:
            >>> config_dict = config.to_dict()
            >>> print(json.dumps(config_dict, indent=2))
        """
        result = {}
        for key in self.get_keys():
            try:
                result[key] = self._config.get(key)
            except Exception:
                continue
        return result
    
    def validate(self) -> List[str]:
        """Validate current configuration.
        
        Returns:
            List of validation error messages. Empty list means valid.
            
        Examples:
            >>> errors = config.validate()
            >>> if errors:
            ...     print("Validation errors:", errors)
        """
        errors = []
        
        # Check neuron count
        if self.has("neuron_count"):
            neuron_count = self.get("neuron_count")
            if not isinstance(neuron_count, (int, float)) or neuron_count <= 0:
                errors.append("neuron_count must be a positive number")
        
        # Check region count
        if self.has("region_count"):
            region_count = self.get("region_count")
            if not isinstance(region_count, (int, float)) or region_count <= 0:
                errors.append("region_count must be a positive number")
        
        # Check connection probability
        if self.has("connection_probability"):
            conn_prob = self.get("connection_probability")
            if not isinstance(conn_prob, (int, float)) or not (0 <= conn_prob <= 1):
                errors.append("connection_probability must be between 0 and 1")
        
        return errors
    
    def __getitem__(self, key: str) -> Any:
        """Get configuration value by key (dict-like access).
        
        Args:
            key: Configuration key to access.
            
        Returns:
            Configuration value.
            
        Raises:
            KeyError: If key doesn't exist.
            
        Examples:
            >>> value = config["neuron_count"]
        """
        if self.has(key):
            return self._config.get(key)
        raise KeyError(f"Configuration key '{key}' not found")
    
    def __setitem__(self, key: str, value: Any) -> None:
        """Set configuration value by key (dict-like access).
        
        Args:
            key: Configuration key to set.
            value: Value to set.
            
        Examples:
            >>> config["neuron_count"] = 100
        """
        self.set(key, value)
    
    def __contains__(self, key: str) -> bool:
        """Check if configuration key exists (in operator).
        
        Args:
            key: Configuration key to check.
            
        Returns:
            True if key exists, False otherwise.
            
        Examples:
            >>> "neuron_count" in config  # Returns True/False
        """
        return self.has(key)
    
    def __repr__(self) -> str:
        """String representation of configuration."""
        return f"<Config: {self.summary()}>"
    
    def __str__(self) -> str:
        """String representation of configuration."""
        return self.summary()

# Convenience function
def create_default_config() -> Config:
    """Create a default configuration optimized for simulation.
    
    Returns:
        New Config instance with sensible defaults for most use cases.
        
    Examples:
        >>> config = create_default_config()
        >>> print(config.neuron_count)  # 100
        >>> print(config.region_count)  # 1
    """
    config = Config()
    
    # Apply sensible defaults
    config.set("neuron_count", 100)
    config.set("region_count", 1)
    config.set("connection_probability", 0.1)
    config.set("plasticity_stdp_enabled", True)
    config.set("plasticity_hebbian_enabled", True)
    config.set("development_enabled", True)
    config.set("neuromodulation_enabled", True)
    config.set("memory_enabled", True)
    config.set("cognition_enabled", True)
    
    return config