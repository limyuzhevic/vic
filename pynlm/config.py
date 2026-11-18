'''Config class for pynlm'''

from __future__ import annotations

from typing import Dict, List, Optional, Union, Any
import json
import os

from .core.enums import ConfigSource


class Config:
    """Configuration class for NLM system.
    
    The Config class manages configuration settings for the NLM system,
    supporting values from multiple sources with proper priority.
    """
    
    def __init__(self):
        """Initialize a Config instance."""
        self._entries: Dict[str, Dict[str, Any]] = {}
        self._invalid_default: ConfigValue = 0  # Default for invalid keys
    
    def loadFromFile(self, filepath: str) -> bool:
        """Load configuration from a JSON file.
        
        Args:
            filepath: Path to the JSON configuration file.
            
        Returns:
            True if configuration was loaded successfully, False otherwise.
        """
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            
            # Convert JSON data to config entries
            for key, value in data.items():
                self._set_entry(key, value, ConfigSource.FILE)
            
            return True
        except (IOError, json.JSONDecodeError) as e:
            return False
    
    def loadFromArgs(self, args: List[str]) -> bool:
        """Load configuration from command line arguments.
        
        Args:
            args: List of command line arguments.
            
        Returns:
            True if configuration was loaded successfully, False otherwise.
        """
        try:
            # Simple command line argument parsing
            # In a real implementation, this would parse --key value pairs
            i = 0
            while i < len(args):
                arg = args[i]
                if arg.startswith('--'):
                    key = arg[2:]  # Remove -- prefix
                    if i + 1 < len(args) and not args[i + 1].startswith('--'):
                        value = args[i + 1]
                        i += 1
                    else:
                        value = True
                    
                    self._set_entry(key, value, ConfigSource.COMMAND_LINE)
                i += 1
            
            return True
        except Exception:
            return False
    
    def saveToFile(self, filepath: str) -> bool:
        """Save configuration to a JSON file.
        
        Args:
            filepath: Path to the file to save to.
            
        Returns:
            True if configuration was saved successfully, False otherwise.
        """
        try:
            # Convert entries to serializable format
            data = {}
            for key, entry in self._entries.items():
                data[key] = entry['value']
            
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
            
            return True
        except IOError:
            return False
    
    def has(self, key: str) -> bool:
        """Check if a configuration key exists.
        
        Args:
            key: Configuration key to check.
            
        Returns:
            True if key exists, False otherwise.
        """
        return key in self._entries
    
    def getKeys(self) -> List[str]:
        """Get all configuration keys.
        
        Returns:
            List of all configuration keys.
        """
        return list(self._entries.keys())
    
    def clear(self):
        """Clear all configuration entries."""
        self._entries.clear()
    
    def summary(self) -> str:
        """Get a summary string of the configuration.
        
        Returns:
            Summary string of configuration.
        """
        if not self._entries:
            return "Empty configuration"
        
        lines = ["Configuration Summary:", "===================="]
        for key, entry in self._entries.items():
            lines.append(f"  {key}: {entry['value']} (source: {entry['source'].name})")
        
        return "\n".join(lines)
    
    def _set_entry(self, key: str, value: Any, source: ConfigSource):
        """Set a configuration entry.
        
        Args:
            key: Configuration key.
            value: Configuration value.
            source: Source of the configuration.
        """
        # Convert value to appropriate ConfigValue type
        if isinstance(value, (int, int64, float, bool)):
            config_value = value
        elif isinstance(value, str):
            config_value = value
        elif isinstance(value, list):
            # Convert list to appropriate type
            if all(isinstance(x, int) for x in value):
                config_value = value
            elif all(isinstance(x, float) for x in value):
                config_value = value
            elif all(isinstance(x, str) for x in value):
                config_value = value
            else:
                config_value = value  # Store as-is
        else:
            config_value = value
        
        self._entries[key] = {
            'value': config_value,
            'source': source,
            'description': f"Configuration for {key}",
        }
    
    def __repr__(self) -> str:
        """Get string representation of the configuration.
        
        Returns:
            String representation of the configuration.
        """
        return f"<Config (entries: {len(self._entries)})>"


# Type aliases for config values
ConfigValue = Union[int, float, bool, str, List[int], List[float], List[str]]


__all__ = ['Config', 'ConfigValue', 'ConfigSource']