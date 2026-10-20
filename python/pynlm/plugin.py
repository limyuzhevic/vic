#!/usr/bin/env python3
"""
Advanced NLM Plugin System

Plugin system for extending NLM with custom modules and functionality.
"""

import os
import sys
import importlib
import json
from pathlib import Path
from typing import Dict, Any, Optional, List, Protocol
from abc import ABC, abstractmethod

# Add the python directory to the path
sys.path.insert(0, str(Path(__file__).parent))

class PluginInterface(ABC):
    """Abstract base class for NLM plugins."""
    
    def __init__(self, name: str):
        self.name = name
        self.version = "1.0.0"
        self.enabled = True
        
    @abstractmethod
    def initialize(self, brain_interface) -> bool:
        """Initialize the plugin with brain interface."""
        pass
    
    @abstractmethod
    def execute(self, context: Dict[str, Any]) -> Dict[str, Any]:
        """Execute plugin functionality."""
        pass
    
    @abstractmethod
    def cleanup(self) -> bool:
        """Clean up plugin resources."""
        pass
    
    def get_info(self) -> Dict[str, Any]:
        """Get plugin information."""
        return {
            "name": self.name,
            "version": self.version,
            "enabled": self.enabled,
        }

class MemoryPlugin(PluginInterface):
    """Plugin for memory system extensions."""
    
    def __init__(self, name: str = "MemoryPlugin"):
        super().__init__(name)
        self.memory_capacity = 1000
        self.consolidation_rate = 0.1
    
    def initialize(self, brain_interface) -> bool:
        """Initialize memory plugin."""
        try:
            # Store brain reference if needed
            self.brain = brain_interface
            return True
        except Exception:
            return False
    
    def execute(self, context: Dict[str, Any]) -> Dict[str, Any]:
        """Execute memory consolidation."""
        result = {
            "patterns": [],
            "consolidations": 0,
            "memory_usage": 0
        }
        
        # Simulate memory consolidation
        for i in range(10):
            pattern = context.get(f"pattern_{i}", [])
            if pattern:
                result["patterns"].append(pattern)
                result["consolidations"] += 1
        
        # Calculate memory usage
        result["memory_usage"] = min(len(result["patterns"]) * self.memory_capacity, 1000)
        
        return result
    
    def cleanup(self) -> bool:
        """Clean up memory plugin."""
        self.brain = None
        return True

class NeuromodulationPlugin(PluginInterface):
    """Plugin for neuromodulation extensions."""
    
    def __init__(self, name: str = "NeuromodulationPlugin"):
        super().__init__(name)
        self.modulation_strength = 0.5
        self.response_delay = 0.01
    
    def initialize(self, brain_interface) -> bool:
        """Initialize neuromodulation plugin."""
        try:
            self.brain = brain_interface
            return True
        except Exception:
            return False
    
    def execute(self, context: Dict[str, Any]) -> Dict[str, Any]:
        """Execute neuromodulation."""
        result = {
            "dopamine_level": 0.0,
            "curiosity_level": 0.0,
            "novelty_level": 0.0,
            "modulations": []
        }
        
        # Simulate neuromodulation signals
        reward = context.get("reward", 0.0)
        surprise = context.get("surprise", 0.0)
        novelty = context.get("novelty", 0.0)
        
        result["dopamine_level"] = min(reward * self.modulation_strength, 1.0)
        result["curiosity_level"] = min(surprise * self.modulation_strength, 1.0)
        result["novelty_level"] = min(novelty * self.modulation_strength, 1.0)
        
        return result
    
    def cleanup(self) -> bool:
        """Clean up neuromodulation plugin."""
        self.brain = None
        return True

class DevelopmentPlugin(PluginInterface):
    """Plugin for developmental extensions."""
    
    def __init__(self, name: str = "DevelopmentPlugin"):
        super().__init__(name)
        self.growth_rate = 0.01
        self.maturity_threshold = 0.8
        self.current_maturity = 0.0
    
    def initialize(self, brain_interface) -> bool:
        """Initialize development plugin."""
        try:
            self.brain = brain_interface
            return True
        except Exception:
            return False
    
    def execute(self, context: Dict[str, Any]) -> Dict[str, Any]:
        """Execute developmental processes."""
        result = {
            "synaptogenesis": 0,
            "pruning": 0,
            "maturation": 0,
            "development_stage": "initial"
        }
        
        # Simulate developmental processes
        time = context.get("time", 0.0)
        self.current_maturity = min(self.current_maturity + self.growth_rate * time, 1.0)
        
        if self.current_maturity < self.maturity_threshold:
            result["development_stage"] = "growth"
            result["synaptogenesis"] = int(self.growth_rate * 100)
        else:
            result["development_stage"] = "maturation"
            result["pruning"] = int((1.0 - self.growth_rate) * 100)
            result["maturation"] = int(self.growth_rate * 50)
        
        return result
    
    def cleanup(self) -> bool:
        """Clean up development plugin."""
        self.brain = None
        return True

class AdvancedPluginManager:
    """Advanced plugin manager for NLM extensions."""
    
    def __init__(self):
        self.plugins: Dict[str, PluginInterface] = {}
        self.plugin_configs: Dict[str, Dict[str, Any]] = {}
    
    def load_plugin(self, plugin_path: str, config: Optional[Dict[str, Any]] = None) -> bool:
        """Load a plugin from Python file."""
        try:
            # Extract plugin name from path
            plugin_name = Path(plugin_path).stem
            
            # Import the module
            spec = importlib.util.spec_from_file_location(plugin_name, plugin_path)
            module = importlib.util.module_from_spec(spec)
            
            # Check if module has a plugin class
            plugin_classes = [attr for attr in dir(module) 
                            if isinstance(getattr(module, attr), type) and 
                            issubclass(getattr(module, attr), PluginInterface)]
            
            if not plugin_classes:
                print(f"No plugin class found in {plugin_path}")
                return False
            
            # Use first plugin class found
            plugin_class = getattr(module, plugin_classes[0])
            plugin = plugin_class()
            
            # Store configuration if provided
            if config:
                self.plugin_configs[plugin.name] = config
            
            # Register plugin
            self.plugins[plugin.name] = plugin
            
            return True
            
        except Exception as e:
            print(f"Error loading plugin {plugin_path}: {e}")
            return False
    
    def get_plugin(self, name: str) -> Optional[PluginInterface]:
        """Get a plugin by name."""
        return self.plugins.get(name)
    
    def enable_plugin(self, name: str) -> bool:
        """Enable a plugin."""
        plugin = self.plugins.get(name)
        if plugin:
            plugin.enabled = True
            return True
        return False
    
    def disable_plugin(self, name: str) -> bool:
        """Disable a plugin."""
        plugin = self.plugins.get(name)
        if plugin:
            plugin.enabled = False
            return True
        return False
    
    def execute_plugin(self, name: str, context: Dict[str, Any]) -> Dict[str, Any]:
        """Execute a plugin."""
        plugin = self.plugins.get(name)
        if plugin and plugin.enabled:
            return plugin.execute(context)
        return {}
    
    def execute_all_plugins(self, context: Dict[str, Any]) -> Dict[str, Any]:
        """Execute all enabled plugins."""
        results = {}
        for name, plugin in self.plugins.items():
            if plugin.enabled:
                results[name] = plugin.execute(context)
        return results
    
    def remove_plugin(self, name: str) -> bool:
        """Remove a plugin."""
        if name in self.plugins:
            plugin = self.plugins[name]
            plugin.cleanup()
            del self.plugins[name]
            return True
        return False
    
    def list_plugins(self) -> List[Dict[str, Any]]:
        """List all plugins."""
        return [plugin.get_info() for plugin in self.plugins.values()]
    
    def save_plugins(self, path: str):
        """Save plugin configuration to file."""
        data = {
            "plugins": {},
            "configs": self.plugin_configs
        }
        
        for name, plugin in self.plugins.items():
            data["plugins"][name] = {
                "class": plugin.__class__.__name__,
                "module": plugin.__class__.__module__,
                "enabled": plugin.enabled,
                "version": plugin.version,
            }
        
        with open(path, 'w') as f:
            json.dump(data, f, indent=2)
    
    def load_plugins(self, path: str) -> bool:
        """Load plugin configuration from file."""
        try:
            with open(path, 'r') as f:
                data = json.load(f)
            
            self.plugins.clear()
            self.plugin_configs.clear()
            
            # Load plugin metadata (actual loading would need file paths)
            for name, info in data.get("plugins", {}).items():
                self.plugin_configs[name] = data.get("configs", {}).get(name, {})
            
            return True
            
        except Exception as e:
            print(f"Error loading plugins: {e}")
            return False
# Global plugin manager instance
_global_plugin_manager: Optional[AdvancedPluginManager] = None


def get_plugin_manager() -> AdvancedPluginManager:
    """Get the global plugin manager instance."""
    global _global_plugin_manager
    if _global_plugin_manager is None:
        _global_plugin_manager = AdvancedPluginManager()
    return _global_plugin_manager


def set_plugin_manager(manager: AdvancedPluginManager):
    """Set the global plugin manager instance."""
    global _global_plugin_manager
    _global_plugin_manager = manager


def load_plugin(plugin_path: str, config: Optional[Dict[str, Any]] = None) -> bool:
    """Load a plugin using the global plugin manager."""
    return get_plugin_manager().load_plugin(plugin_path, config)


def enable_plugin(name: str) -> bool:
    """Enable a plugin using the global plugin manager."""
    return get_plugin_manager().enable_plugin(name)


def disable_plugin(name: str) -> bool:
    """Disable a plugin using the global plugin manager."""
    return get_plugin_manager().disable_plugin(name)


def execute_plugin(name: str, context: Dict[str, Any]) -> Dict[str, Any]:
    """Execute a plugin using the global plugin manager."""
    return get_plugin_manager().execute_plugin(name, context)


def execute_all_plugins(context: Dict[str, Any]) -> Dict[str, Any]:
    """Execute all plugins using the global plugin manager."""
    return get_plugin_manager().execute_all_plugins(context)


def remove_plugin(name: str) -> bool:
    """Remove a plugin using the global plugin manager."""
    return get_plugin_manager().remove_plugin(name)


def list_plugins() -> List[Dict[str, Any]]:
    """List all plugins using the global plugin manager."""
    return get_plugin_manager().list_plugins()