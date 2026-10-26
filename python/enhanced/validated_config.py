"""
Validated Configuration for NLM

This module provides enhanced configuration management with validation,
strict mode, and batch operations for NLM systems.
"""

import sys
from pathlib import Path

# Add parent directory to Python path
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

class ValidatedConfig:
    """
    Enhanced configuration with validation support.
    
    This class extends the basic Config class with validation, strict mode,
    and batch operations for safer configuration management.
    """
    
    def __init__(self):
        """Initialize validated configuration"""
        try:
            from pynlm_enhanced import ValidatedConfig as EnhancedValidatedConfig
            self._config = EnhancedValidatedConfig()
        except ImportError:
            # Fallback to basic config
            try:
                from pynlm import Config
                self._config = Config()
            except ImportError:
                raise ImportError("Neither enhanced nor original NLM bindings are available")
    
    def add_validator(self, key, validator, description=""):
        """
        Add a validator for a configuration key.
        
        Args:
            key: Configuration key to validate
            validator: Function that returns True if valid, False otherwise
            description: Description of the configuration parameter
        """
        if hasattr(self._config, 'add_validator'):
            return self._config.add_validator(key, validator, description)
        else:
            raise NotImplementedError("Validation not available")
    
    def set_strict_mode(self, strict):
        """
        Enable or disable strict mode.
        
        Args:
            strict: If True, invalid configurations will raise exceptions
        """
        if hasattr(self._config, 'setStrictMode'):
            self._config.setStrictMode(strict)
    
    def set_int(self, key, value, source=None):
        """
        Set an integer configuration value.
        
        Args:
            key: Configuration key
            value: Integer value
            source: Configuration source (optional)
        """
        if hasattr(self._config, 'setInt'):
            return self._config.setInt(key, value, source)
        else:
            # Fallback to basic config
            self._config.set(key, value)
            return True
    
    def set_double(self, key, value, source=None):
        """
        Set a double configuration value.
        
        Args:
            key: Configuration key
            value: Double value
            source: Configuration source (optional)
        """
        if hasattr(self._config, 'setDouble'):
            return self._config.setDouble(key, value, source)
        else:
            self._config.set(key, value)
            return True
    
    def set_string(self, key, value, source=None):
        """
        Set a string configuration value.
        
        Args:
            key: Configuration key
            value: String value
            source: Configuration source (optional)
        """
        if hasattr(self._config, 'setString'):
            return self._config.setString(key, value, source)
        else:
            self._config.set(key, value)
            return True
    
    def set_bool(self, key, value, source=None):
        """
        Set a boolean configuration value.
        
        Args:
            key: Configuration key
            value: Boolean value
            source: Configuration source (optional)
        """
        if hasattr(self._config, 'setBool'):
            return self._config.setBool(key, value, source)
        else:
            self._config.set(key, value)
            return True
    
    def set_batch(self, batch):
        """
        Set multiple configuration values at once.
        
        Args:
            batch: Dictionary mapping keys to values
        """
        if hasattr(self._config, 'setBatch'):
            return self._config.setBatch(batch)
        else:
            # Fallback to individual settings
            success = True
            for key, value in batch.items():
                self._config.set(key, value)
            return success
    
    def get_base_config(self):
        """
        Get the underlying basic configuration.
        
        Returns:
            Base configuration object
        """
        return self._config

# Alias for backward compatibility
EnhancedConfig = ValidatedConfig