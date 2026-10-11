# Config module for NLM
import json
import os
from typing import Any, Dict, List, Optional
from pathlib import Path

class Config:
    """
    Configuration class for NLM system.
    
    Provides methods to load, save, and manage NLM configuration parameters.
    """
    
    def __init__(self):
        self._config: Dict[str, Any] = {}
    
    def loadFromFile(self, filepath: str) -> None:
        """
        Load configuration from a JSON file.
        
        Args:
            filepath: Path to the JSON configuration file
        """
        with open(filepath, 'r') as f:
            self._config = json.load(f)
    
    def loadFromArgs(self, argc: int, argv: list) -> bool:
        """
        Load configuration from command line arguments.
        
        Args:
            argc: Argument count (for compatibility)
            argv: Argument vector (for compatibility)
            
        Returns:
            bool: True if configuration was loaded successfully
        """
        # Implementation would parse command line arguments
        # For now, this is a placeholder
        return False
    
    def saveToFile(self, filepath: str) -> None:
        """
        Save configuration to a JSON file.
        
        Args:
            filepath: Path where to save the configuration file
        """
        with open(filepath, 'w') as f:
            json.dump(self._config, f, indent=2)
    
    def has(self, key: str) -> bool:
        """
        Check if a configuration key exists.
        
        Args:
            key: Configuration key to check
            
        Returns:
            bool: True if the key exists
        """
        return key in self._config
    
    def getKeys(self) -> List[str]:
        """
        Get all configuration keys.
        
        Returns:
            List[str]: List of all configuration keys
        """
        return list(self._config.keys())
    
    def clear(self) -> None:
        """
        Clear all configuration entries.
        """
        self._config.clear()
    
    def summary(self) -> str:
        """
        Get a summary string of the configuration.
        
        Returns:
            str: Summary of configuration
        """
        if not self._config:
            return "Empty configuration"
        
        return f"Configuration with {len(self._config)} entries:\n" + \
               "\n".join(f"  {k}: {v}" for k, v in self._config.items())
    
    def __repr__(self) -> str:
        return f"<Config: {self.summary()}>"
    
    def set(self, key: str, value: Any) -> None:
        """
        Set a configuration value.
        
        Args:
            key: Configuration key
            value: Configuration value
        """
        self._config[key] = value
    
    def get(self, key: str, default: Any = None) -> Any:
        """
        Get a configuration value.
        
        Args:
            key: Configuration key
            default: Default value if key doesn't exist
            
        Returns:
            Any: Configuration value or default
        """
        return self._config.get(key, default)

# Factory function for backward compatibility
def createDefaultConfig() -> Config:
    """Create a default configuration."""
    return Config()
