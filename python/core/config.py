"""
NLM Configuration system

This module provides configuration classes for the NLM system.
"""

import json
from typing import Optional, Union, List, Any
from enum import Enum

class ConfigSource(Enum):
    Default = "default"
    File = "file"
    CommandLine = "commandline"
    Runtime = "runtime"

class ConfigEntry:
    def __init__(self, key: str, value: Any, source: ConfigSource = ConfigSource.Default, description: str = ""):
        self.key = key
        self.value = value
        self.source = source
        self.description = description

    def __repr__(self):
        return f"ConfigEntry(key={self.key}, value={self.value}, source={self.source})"

class Config:
    def __init__(self):
        self._entries = {}
        self._default_values = {}

    def loadFromFile(self, filepath: str) -> bool:
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
                if not isinstance(data, dict):
                    return False
                for key, value in data.items():
                    if not isinstance(key, str):
                        continue
                    self.set(key, value, ConfigSource.File)
            return True
        except (IOError, json.JSONDecodeError, UnicodeDecodeError) as e:
            return False
        except Exception:
            return False

    def loadFromArgs(self, argc: int, argv: List[str]) -> bool:
        # Simple command line argument parsing with better error handling
        if not isinstance(argv, list):
            return False
        
        i = 1
        while i < len(argv):
            try:
                arg = str(argv[i])
                if arg.startswith("--"):
                    key = arg[2:]
                    if not key:
                        i += 1
                        continue
                    if i + 1 < len(argv) and not str(argv[i + 1]).startswith("--"):
                        value = str(argv[i + 1])
                        i += 1
                        self.set(key, value, ConfigSource.CommandLine)
                    else:
                        self.set(key, True, ConfigSource.CommandLine)
            except (IndexError, AttributeError):
                i += 1
                continue
            i += 1
        return True

    def saveToFile(self, filepath: str) -> bool:
        try:
            if not filepath or not isinstance(filepath, str):
                return False
            data = {}
            for key, entry in self._entries.items():
                if isinstance(key, str):
                    data[key] = entry.value
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2, default=str)
            return True
        except (IOError, TypeError, ValueError):
            return False
        except Exception:
            return False

    def set(self, key: str, value: Any, source: ConfigSource = ConfigSource.Runtime):
        if not key or not isinstance(key, str):
            return
        try:
            if isinstance(value, (int, float, str, bool)):
                self._entries[key] = ConfigEntry(key, value, source)
            elif isinstance(value, list) and value and all(isinstance(x, (int, float, str)) for x in value):
                self._entries[key] = ConfigEntry(key, value, source)
            else:
                raise ValueError(f"Unsupported value type for config key '{key}': {type(value)}")
        except (ValueError, TypeError):
            # Log error and continue
            pass

    def clear(self):
        self._entries.clear()
        self._default_values.clear()

    def get(self, key: str) -> Optional[Any]:
        if not key or not isinstance(key, str):
            return None
        if key in self._entries:
            return self._entries[key].value
        return None

    def getOr(self, key: str, defaultValue: Any) -> Any:
        if not key or not isinstance(key, str):
            return defaultValue
        if key in self._entries:
            return self._entries[key].value
        self._default_values[key] = defaultValue
        return defaultValue

    def has(self, key: str) -> bool:
        return bool(key) and key in self._entries

    def getKeys(self) -> List[str]:
        return list(self._entries.keys())

# Create default config function
def createDefaultConfig():
    return Config()

# Global config instance
_default_config = createDefaultConfig()

def get_default_config():
    return _default_config

# Create convenience functions for common config types
def createConfig():
    return createDefaultConfig()

__all__ = [
    'Config',
    'ConfigSource',
    'ConfigEntry',
    'createDefaultConfig',
    'createConfig',
    'get_default_config',
]