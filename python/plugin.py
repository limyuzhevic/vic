"""
Plugin system for NLM (Neural Learning Machine).

This module provides a plugin architecture for dynamic component loading
and management in NLM.

Key features:
- Plugin registration and loading
- Component factory system
- Dynamic component management
- Plugin lifecycle management
- Plugin validation and compatibility
- Plugin metadata management
- Plugin event handling

The Plugin system is part of NLM Phase 6 "FINAL INTEGRATION",
powering advanced artificial brain capabilities through a comprehensive
component management and extension framework.

The system supports integration with:
- Component Registry (central component management)
- Brain (central neural coordination)
- AgentBrain (agent interface)
- World (environmental interaction)
- Memory systems (working, episodic, semantic, procedural)
- Prediction systems (forward models)
- Cognitive systems (planning, attention, concept formation)
- Neuromodulation systems (dopamine, serotonin, norepinephrine, acetylcholine)
- Development systems (maturation, learning)
- Performance infrastructure (memory pools, event queues)

See src/registry/README.md for detailed documentation.
"""

from typing import Dict, Any, Optional, List, Callable
from pathlib import Path
import importlib
import logging
import sys

from .exceptions import NLMError


class Plugin:
    """Base class for NLM plugins."""
    
    def __init__(self, name: str, version: str = "1.0.0"):
        """
        Initialize plugin.
        
        Args:
            name: Plugin name
            version: Plugin version
        """
        self.name = name
        self.version = version
        self.logger = logging.getLogger(f"plugin.{name}")
        
        self._components = {}
        self._factories = {}
        self._metadata = {}
    
    def get_name(self) -> str:
        """Get plugin name."""
        return self.name
    
    def get_version(self) -> str:
        """Get plugin version."""
        return self.version
    
    def get_metadata(self) -> Dict[str, Any]:
        """Get plugin metadata."""
        return self._metadata
    
    def set_metadata(self, metadata: Dict[str, Any]):
        """
        Set plugin metadata.
        
        Args:
            metadata: Plugin metadata
        """
        self._metadata = metadata
    
    def load(self) -> bool:
        """
        Load plugin.
        
        Returns:
            True if plugin loaded successfully, False otherwise
        """
        try:
            self.logger.info(f"Loading plugin '{self.name}' version {self.version}")
            self._load_components()
            self._load_factories()
            self._load_metadata()
            self.logger.info(f"Plugin '{self.name}' loaded successfully")
            return True
        except Exception as e:
            self.logger.error(f"Failed to load plugin '{self.name}': {e}")
            return False
    
    def unload(self) -> bool:
        """
        Unload plugin.
        
        Returns:
            True if plugin unloaded successfully, False otherwise
        """
        try:
            self.logger.info(f"Unloading plugin '{self.name}'")
            self._unload_components()
            self._unload_factories()
            self._unload_metadata()
            self.logger.info(f"Plugin '{self.name}' unloaded successfully")
            return True
        except Exception as e:
            self.logger.error(f"Failed to unload plugin '{self.name}': {e}")
            return False
    
    def is_loaded(self) -> bool:
        """Check if plugin is loaded."""
        return len(self._components) > 0 or len(self._factories) > 0
    
    def get_components(self) -> Dict[str, Any]:
        """
        Get plugin components.
        
        Returns:
            Dictionary of plugin components
        """
        return self._components.copy()
    
    def get_factories(self) -> Dict[str, Callable]:
        """
        Get plugin factories.
        
        Returns:
            Dictionary of plugin factories
        """
        return self._factories.copy()
    
    def get_component(self, name: str) -> Optional[Any]:
        """
        Get plugin component by name.
        
        Args:
            name: Component name
            
        Returns:
            Component object if found, None otherwise
        """
        return self._components.get(name)
    
    def get_factory(self, name: str) -> Optional[Callable]:
        """
        Get plugin factory by name.
        
        Args:
            name: Factory name
            
        Returns:
            Factory function if found, None otherwise
        """
        return self._factories.get(name)
    
    def register_component(self, name: str, component: Any):
        """
        Register a component in plugin.
        
        Args:
            name: Component name
            component: Component object
        """
        self._components[name] = component
    
    def register_factory(self, name: str, factory: Callable):
        """
        Register a factory in plugin.
        
        Args:
            name: Factory name
            factory: Factory function
        """
        self._factories[name] = factory
    
    def unregister_component(self, name: str) -> bool:
        """
        Unregister a component from plugin.
        
        Args:
            name: Component name
            
        Returns:
            True if component unregistered successfully, False otherwise
        """
        if name in self._components:
            del self._components[name]
            return True
        return False
    
    def unregister_factory(self, name: str) -> bool:
        """
        Unregister a factory from plugin.
        
        Args:
            name: Factory name
            
        Returns:
            True if factory unregistered successfully, False otherwise
        """
        if name in self._factories:
            del self._factories[name]
            return True
        return False
    
    def _load_components(self):
        """Load components from plugin (override in subclasses)."""
        pass
    
    def _load_factories(self):
        """Load factories from plugin (override in subclasses)."""
        pass
    
    def _load_metadata(self):
        """Load metadata from plugin (override in subclasses)."""
        pass
    
    def _unload_components(self):
        """Unload components from plugin (override in subclasses)."""
        self._components.clear()
    
    def _unload_factories(self):
        """Unload factories from plugin (override in subclasses)."""
        self._factories.clear()
    
    def _unload_metadata(self):
        """Unload metadata from plugin (override in subclasses)."""
        self._metadata.clear()
    
    def validate(self) -> bool:
        """
        Validate plugin.
        
        Returns:
            True if plugin is valid, False otherwise
        """
        try:
            # Basic validation
            if not self.name or not self.version:
                return False
            
            # Call subclass validation
            return self._validate()
        except Exception as e:
            self.logger.error(f"Error validating plugin '{self.name}': {e}")
            return False
    
    def _validate(self) -> bool:
        """
        Validate plugin (override in subclasses).
        
        Returns:
            True if plugin is valid, False otherwise
        """
        return True


class PluginManager:
    """Plugin manager for NLM."""
    
    def __init__(self):
        """Initialize plugin manager."""
        self._plugins: Dict[str, Plugin] = {}
        self._logger = logging.getLogger(__name__)
    
    def load_plugin(self, plugin_path: str, plugin_name: Optional[str] = None) -> bool:
        """
        Load a plugin.
        
        Args:
            plugin_path: Path to plugin module or file
            plugin_name: Plugin name (optional, will be derived from file if not provided)
            
        Returns:
            True if plugin loaded successfully, False otherwise
        """
        try:
            # Determine plugin name
            if plugin_name is None:
                plugin_name = Path(plugin_path).stem
            
            if plugin_name in self._plugins:
                self._logger.warning(f"Plugin '{plugin_name}' already loaded")
                return False
            
            # Import plugin module
            spec = importlib.util.spec_from_file_location(plugin_name, plugin_path)
            if spec is None or spec.loader is None:
                self._logger.error(f"Failed to load plugin from '{plugin_path}'")
                return False
            
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)
            
            # Create plugin instance
            if not hasattr(module, 'create_plugin'):
                self._logger.error(f"Plugin '{plugin_name}' does not have create_plugin function")
                return False
            
            plugin = module.create_plugin()
            if not isinstance(plugin, Plugin):
                self._logger.error(f"Plugin '{plugin_name}' is not a valid Plugin instance")
                return False
            
            # Validate plugin
            if not plugin.validate():
                self._logger.error(f"Plugin '{plugin_name}' validation failed")
                return False
            
            # Load plugin
            if plugin.load():
                self._plugins[plugin_name] = plugin
                self._logger.info(f"Plugin '{plugin_name}' loaded successfully")
                return True
            else:
                self._logger.error(f"Failed to load plugin '{plugin_name}'")
                return False
            
        except Exception as e:
            self._logger.error(f"Error loading plugin '{plugin_name}': {e}")
            return False
    
    def unload_plugin(self, plugin_name: str) -> bool:
        """
        Unload a plugin.
        
        Args:
            plugin_name: Plugin name
            
        Returns:
            True if plugin unloaded successfully, False otherwise
        """
        try:
            if plugin_name not in self._plugins:
                self._logger.warning(f"Plugin '{plugin_name}' not found")
                return False
            
            plugin = self._plugins[plugin_name]
            if plugin.unload():
                del self._plugins[plugin_name]
                self._logger.info(f"Plugin '{plugin_name}' unloaded successfully")
                return True
            else:
                self._logger.error(f"Failed to unload plugin '{plugin_name}'")
                return False
            
        except Exception as e:
            self._logger.error(f"Error unloading plugin '{plugin_name}': {e}")
            return False
    
    def is_plugin_loaded(self, plugin_name: str) -> bool:
        """
        Check if plugin is loaded.
        
        Args:
            plugin_name: Plugin name
            
        Returns:
            True if plugin is loaded, False otherwise
        """
        return plugin_name in self._plugins
    
    def get_loaded_plugins(self) -> List[str]:
        """
        Get all loaded plugins.
        
        Returns:
            List of loaded plugin names
        """
        return list(self._plugins.keys())
    
    def get_plugin(self, plugin_name: str) -> Optional[Plugin]:
        """
        Get plugin by name.
        
        Args:
            plugin_name: Plugin name
            
        Returns:
            Plugin instance if found, None otherwise
        """
        return self._plugins.get(plugin_name)
    
    def get_plugin_components(self, plugin_name: str) -> Dict[str, Any]:
        """
        Get plugin components.
        
        Args:
            plugin_name: Plugin name
            
        Returns:
            Dictionary of plugin components
        """
        plugin = self._plugins.get(plugin_name)
        if plugin:
            return plugin.get_components()
        return {}
    
    def get_plugin_factories(self, plugin_name: str) -> Dict[str, Callable]:
        """
        Get plugin factories.
        
        Args:
            plugin_name: Plugin name
            
        Returns:
            Dictionary of plugin factories
        """
        plugin = self._plugins.get(plugin_name)
        if plugin:
            return plugin.get_factories()
        return {}
    
    def validate_plugin(self, plugin_name: str) -> bool:
        """
        Validate plugin.
        
        Args:
            plugin_name: Plugin name
            
        Returns:
            True if plugin is valid, False otherwise
        """
        plugin = self._plugins.get(plugin_name)
        if not plugin:
            return False
        
        return plugin.validate()
    
    def reload_plugin(self, plugin_name: str) -> bool:
        """
        Reload a plugin.
        
        Args:
            plugin_name: Plugin name
            
        Returns:
            True if plugin reloaded successfully, False otherwise
        """
        try:
            if not self.is_plugin_loaded(plugin_name):
                self._logger.warning(f"Plugin '{plugin_name}' not loaded")
                return False
            
            # Unload plugin
            if not self.unload_plugin(plugin_name):
                self._logger.error(f"Failed to unload plugin '{plugin_name}' for reload")
                return False
            
            # Find plugin file
            plugin = self._plugins.get(plugin_name)
            if not plugin:
                self._logger.error(f"Plugin '{plugin_name}' not found after unload")
                return False
            
            # Reload plugin (simplified - in practice, you'd need to track the file path)
            self._logger.warning(f"Plugin '{plugin_name}' reload not fully implemented")
            return False
            
        except Exception as e:
            self._logger.error(f"Error reloading plugin '{plugin_name}': {e}")
            return False


# Global plugin manager instance
_global_plugin_manager: Optional[PluginManager] = None


def get_plugin_manager() -> PluginManager:
    """Get global plugin manager instance."""
    global _global_plugin_manager
    if _global_plugin_manager is None:
        _global_plugin_manager = PluginManager()
    return _global_plugin_manager


def load_plugin(plugin_path: str, plugin_name: Optional[str] = None) -> bool:
    """
    Load a plugin globally.
    
    Args:
        plugin_path: Path to plugin module or file
        plugin_name: Plugin name (optional, will be derived from file if not provided)
        
    Returns:
        True if plugin loaded successfully, False otherwise
    """
    plugin_manager = get_plugin_manager()
    return plugin_manager.load_plugin(plugin_path, plugin_name)


def unload_plugin(plugin_name: str) -> bool:
    """
    Unload a plugin globally.
    
    Args:
        plugin_name: Plugin name
        
    Returns:
        True if plugin unloaded successfully, False otherwise
    """
    plugin_manager = get_plugin_manager()
    return plugin_manager.unload_plugin(plugin_name)


def is_plugin_loaded(plugin_name: str) -> bool:
    """
    Check if plugin is loaded globally.
    
    Args:
        plugin_name: Plugin name
        
    Returns:
        True if plugin is loaded, False otherwise
    """
    plugin_manager = get_plugin_manager()
    return plugin_manager.is_plugin_loaded(plugin_name)


def get_loaded_plugins() -> List[str]:
    """
    Get all loaded plugins globally.
    
    Returns:
        List of loaded plugin names
    """
    plugin_manager = get_plugin_manager()
    return plugin_manager.get_loaded_plugins()


def reload_plugin(plugin_name: str) -> bool:
    """
    Reload a plugin globally.
    
    Args:
        plugin_name: Plugin name
        
    Returns:
        True if plugin reloaded successfully, False otherwise
    """
    plugin_manager = get_plugin_manager()
    return plugin_manager.reload_plugin(plugin_name)