from typing import Dict, Any, Optional


class Config:
    """Configuration wrapper for NLM with simplified API.
    
    This class provides a simplified interface to the underlying C++ configuration,
    with comprehensive error handling and validation.
    """
    
    def __init__(self, **kwargs):
        """Initialize a new Config instance.
        
        Args:
            **kwargs: Configuration parameters
            
        Raises:
            ValueError: If invalid parameters are provided
            TypeError: If parameter types are invalid
        """
        self._validate_config(kwargs)
        self._parameters = kwargs
        self._initialized = True
    
    def _validate_config(self, kwargs: Dict[str, Any]) -> None:
        """Validate configuration parameters.
        
        Args:
            kwargs: Configuration parameters to validate
            
        Raises:
            ValueError: If invalid parameters are provided
            TypeError: If parameter types are invalid
        """
        for key, value in kwargs.items():
            if not isinstance(key, str):
                raise TypeError(f"Parameter name must be a string, got {type(key).__name__}: {key}")
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get a configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key not found (optional)
            
        Returns:
            Configuration value or default
        """
        return self._parameters.get(key, default)
    
    def set(self, key: str, value: Any) -> None:
        """Set a configuration value.
        
        Args:
            key: Configuration key
            value: Configuration value
            
        Raises:
            ValueError: If invalid parameters are provided
            TypeError: If parameter types are invalid
        """
        if not isinstance(key, str):
            raise TypeError(f"Parameter name must be a string, got {type(key).__name__}: {key}")
        
        self._parameters[key] = value
    
    def get_parameters(self) -> Dict[str, Any]:
        """Get all configuration parameters.
        
        Returns:
            Dictionary of all parameters
        """
        return self._parameters.copy()
    
    def update(self, **kwargs) -> None:
        """Update configuration parameters.
        
        Args:
            **kwargs: New parameters to set
        """
        self._validate_config(kwargs)
        self._parameters.update(kwargs)
    
    def has_key(self, key: str) -> bool:
        """Check if a configuration key exists.
        
        Args:
            key: Configuration key to check
            
        Returns:
            True if key exists, False otherwise
        """
        return key in self._parameters
    
    def remove(self, key: str) -> None:
        """Remove a configuration key.
        
        Args:
            key: Configuration key to remove
            
        Raises:
            ValueError: If key does not exist
        """
        if key not in self._parameters:
            raise ValueError(f"Configuration key '{key}' does not exist")
        
        del self._parameters[key]
    
    def __getitem__(self, key: str) -> Any:
        """Get configuration value using dictionary-style access.
        
        Args:
            key: Configuration key
            
        Returns:
            Configuration value
            
        Raises:
            KeyError: If key does not exist
        """
        return self._parameters[key]
    
    def __setitem__(self, key: str, value: Any) -> None:
        """Set configuration value using dictionary-style access.
        
        Args:
            key: Configuration key
            value: Configuration value
        """
        self.set(key, value)
    
    def __contains__(self, key: str) -> bool:
        """Check if configuration contains a key.
        
        Args:
            key: Configuration key to check
            
        Returns:
            True if key exists, False otherwise
        """
        return self.has_key(key)
    
    def __repr__(self) -> str:
        """String representation of the Config instance.
        
        Returns:
            String representation
        """
        return f"Config(parameters={len(self._parameters)})"
    
    def __str__(self) -> str:
        """Human-readable string representation.
        
        Returns:
            Human-readable string
        """
        return f"NLM Config with {len(self._parameters)} parameters"