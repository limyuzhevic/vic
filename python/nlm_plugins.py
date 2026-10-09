import sys
import importlib
import importlib.util
import pkgutil
import inspect
from typing import Dict, List, Any, Optional, Callable
from abc import ABC, abstractmethod
import threading
import os

class NLMPlugin(ABC):
    """Abstract base class for NLM plugins"""
    
    def __init__(self, name: str):
        self.name = name
        self.api = None
        self._enabled = True
    
    @abstractmethod
    def setup(self, api: Any) -> None:
        """Setup plugin with NLM API"""
        pass
    
    @abstractmethod
    def teardown(self) -> None:
        """Cleanup plugin"""
        pass
    
    def handle_event(self, event_name: str, data: Dict[str, Any]) -> None:
        """Handle an event - override in subclass"""
        pass
    
    def enable(self) -> None:
        """Enable plugin"""
        self._enabled = True
    
    def disable(self) -> None:
        """Disable plugin"""
        self._enabled = False
    
    def is_enabled(self) -> bool:
        """Check if plugin is enabled"""
        return self._enabled
    
    @property
    def is_running(self) -> bool:
        """Check if plugin is running"""
        return self._enabled
class PluginManager:
    """Manager for NLM plugins"""
    
    def __init__(self):
        self._plugins: Dict[str, NLMPlugin] = {}
        self._plugin_classes: Dict[str, type] = {}
        self._lock = threading.Lock()
        self._loaded_modules = {}
    
    def register_plugin_class(self, plugin_class: type) -> None:
        """Register a plugin class for dynamic instantiation"""
        if not issubclass(plugin_class, NLMPlugin):
            raise ValueError("Plugin class must inherit from NLMPlugin")
        
        plugin_name = plugin_class.__name__
        with self._lock:
            self._plugin_classes[plugin_name] = plugin_class
    
    def create_plugin(self, plugin_class_name: str, *args, **kwargs) -> NLMPlugin:
        """Create a plugin instance from registered class"""
        with self._lock:
            if plugin_class_name not in self._plugin_classes:
                raise ValueError(f"Unknown plugin class: {plugin_class_name}")
            
            plugin_class = self._plugin_classes[plugin_class_name]
            plugin = plugin_class(*args, **kwargs)
            
            # Store the class for type checking
            plugin.__plugin_class_name__ = plugin_class_name
            
            return plugin
    
    def load_plugin_from_module(self, module_name: str, module_path: Optional[str] = None) -> NLMPlugin:
        """Load a plugin from a Python module"""
        if module_name in self._loaded_modules:
            # Return existing plugin instance
            return self._loaded_modules[module_name]
        
        try:
            if module_path:
                # Load from specific file path
                spec = importlib.util.spec_from_file_location(module_name, module_path)
                module = importlib.util.module_from_spec(spec)
                sys.modules[module_name] = module
                spec.loader.exec_module(module)
            else:
                # Load as regular module
                module = importlib.import_module(module_name)
            
            # Find plugin classes in the module
            plugin_classes = []
            for name, obj in inspect.getmembers(module):
                if (inspect.isclass(obj) and 
                    issubclass(obj, NLMPlugin) and 
                    obj is not NLMPlugin):
                    plugin_classes.append(obj)
            
            if not plugin_classes:
                raise ValueError(f"No plugin classes found in module {module_name}")
            
            # Create plugin instance from first found class
            plugin_class = plugin_classes[0]
            plugin = self.create_plugin(plugin_class.__name__)
            
            with self._lock:
                self._loaded_modules[module_name] = plugin
            
            return plugin
            
        except Exception as e:
            raise RuntimeError(f"Failed to load plugin from {module_name}: {e}")
    
    def unload_plugin(self, module_name: str) -> bool:
        """Unload a plugin"""
        with self._lock:
            if module_name in self._loaded_modules:
                plugin = self._loaded_modules[module_name]
                if plugin.is_running:
                    plugin.teardown()
                del self._loaded_modules[module_name]
                return True
            return False
    
    def get_plugin_info(self, module_name: str) -> Dict[str, Any]:
        """Get information about a loaded plugin"""
        with self._lock:
            if module_name not in self._loaded_modules:
                return {}
            
            plugin = self._loaded_modules[module_name]
            return {
                'name': plugin.name,
                'enabled': plugin.is_enabled(),
                'class_name': getattr(plugin, '__plugin_class_name__', 'Unknown'),
                'module': module_name,
            }
    
    def list_loaded_plugins(self) -> List[str]:
        """List all loaded plugin modules"""
        with self._lock:
            return list(self._loaded_modules.keys())
    
    def list_registered_plugin_classes(self) -> List[str]:
        """List all registered plugin classes"""
        with self._lock:
            return list(self._plugin_classes.keys())
    
    def add_plugin(self, plugin: NLMPlugin) -> None:
        """Add a plugin instance to the manager"""
        with self._lock:
            plugin_name = plugin.name
            self._plugins[plugin_name] = plugin
    
    def remove_plugin(self, plugin_name: str) -> bool:
        """Remove a plugin"""
        with self._lock:
            if plugin_name in self._plugins:
                plugin = self._plugins[plugin_name]
                if plugin.is_running:
                    plugin.teardown()
                del self._plugins[plugin_name]
                return True
            return False
    
    def get_plugin(self, plugin_name: str) -> Optional[NLMPlugin]:
        """Get a plugin by name"""
        with self._lock:
            return self._plugins.get(plugin_name)
    
    def enable_plugin(self, plugin_name: str) -> bool:
        """Enable a plugin"""
        plugin = self.get_plugin(plugin_name)
        if plugin and not plugin.is_enabled():
            plugin.enable()
            return True
        return False
    
    def disable_plugin(self, plugin_name: str) -> bool:
        """Disable a plugin"""
        plugin = self.get_plugin(plugin_name)
        if plugin and plugin.is_enabled():
            plugin.disable()
            return True
        return False
    
    def start_all_plugins(self) -> None:
        """Start all plugins"""
        with self._lock:
            for plugin in self._plugins.values():
                if plugin.is_enabled():
                    plugin.setup(None)  # Will be setup with actual API later
    
    def stop_all_plugins(self) -> None:
        """Stop all plugins"""
        with self._lock:
            for plugin in self._plugins.values():
                if plugin.is_running:
                    plugin.teardown()
class NLMMixin:
    """Mixin class for adding plugin functionality to NLM objects"""
    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._plugin_manager = PluginManager()
    
    def add_plugin(self, plugin: NLMPlugin) -> None:
        """Add a plugin to this object"""
        self._plugin_manager.add_plugin(plugin)
    
    def remove_plugin(self, plugin_name: str) -> bool:
        """Remove a plugin from this object"""
        return self._plugin_manager.remove_plugin(plugin_name)
    
    def get_plugin(self, plugin_name: str) -> Optional[NLMPlugin]:
        """Get a plugin from this object"""
        return self._plugin_manager.get_plugin(plugin_name)
    
    def enable_plugin(self, plugin_name: str) -> bool:
        """Enable a plugin for this object"""
        return self._plugin_manager.enable_plugin(plugin_name)
    
    def disable_plugin(self, plugin_name: str) -> bool:
        """Disable a plugin for this object"""
        return self._plugin_manager.disable_plugin(plugin_name)
    
    def get_plugin_manager(self) -> PluginManager:
        """Get the plugin manager for this object"""
        return self._plugin_manager

# Global plugin manager instance
_plugin_manager = PluginManager()

# Decorator for easy plugin registration
def register_plugin(plugin_class: Optional[type] = None) -> Callable:
    """Decorator to register a plugin class"""
    def decorator(cls: type) -> type:
        if plugin_class is not None:
            # Use provided class name if available
            class_name = plugin_class.__name__ if hasattr(plugin_class, '__name__') else cls.__name__
            _plugin_manager.register_plugin_class(plugin_class)
        else:
            # Use the decorated class
            class_name = cls.__name__
            _plugin_manager.register_plugin_class(cls)
        return cls
    return decorator

# Public API
__all__ = [
    'NLMPlugin',
    'PluginManager',
    'NLMMixin',
    'register_plugin',
    'load_plugin_from_module',
    'unload_plugin',
    'get_plugin_info',
    'list_loaded_plugins',
    'list_registered_plugin_classes',
    'create_plugin',
]

# Convenience functions
def load_plugin_from_module(module_name: str, module_path: Optional[str] = None) -> NLMPlugin:
    """Load a plugin from a Python module"""
    return _plugin_manager.load_plugin_from_module(module_name, module_path)

def unload_plugin(module_name: str) -> bool:
    """Unload a plugin"""
    return _plugin_manager.unload_plugin(module_name)

def get_plugin_info(module_name: str) -> Dict[str, Any]:
    """Get information about a loaded plugin"""
    return _plugin_manager.get_plugin_info(module_name)

def list_loaded_plugins() -> List[str]:
    """List all loaded plugin modules"""
    return _plugin_manager.list_loaded_plugins()

def list_registered_plugin_classes() -> List[str]:
    """List all registered plugin classes"""
    return _plugin_manager.list_registered_plugin_classes()

def create_plugin(plugin_class_name: str, *args, **kwargs) -> NLMPlugin:
    """Create a plugin instance"""
    return _plugin_manager.create_plugin(plugin_class_name, *args, **kwargs)