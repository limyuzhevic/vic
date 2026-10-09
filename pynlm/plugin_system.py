# Python NLM Bindings Extension Points
# 
# This file provides Python bindings extension points for NLM plugins and custom functionality
# Advanced users can extend NLM through these extension points

import sys
import json
import os
from typing import Any, Dict, List, Optional, Callable
from dataclasses import dataclass
from abc import ABC, abstractmethod

if sys.version_info >= (3, 8):
    from typing import Protocol
else:
    from typing_extensions import Protocol

@dataclass
class PluginInfo:
    """Information about a plugin"""
    name: str
    version: str
    description: str
    author: str
    capabilities: List[str]
    dependencies: List[str]
    entry_point: str

class NLMExtensionPoint(Protocol):
    """Base protocol for NLM extension points"""
    
    def initialize(self, nlm_core: Any) -> bool:
        """Initialize the extension point with NLM core"""
        ...
    
    def shutdown(self) -> None:
        """Shutdown the extension point"""
        ...
    
    def is_enabled(self) -> bool:
        """Check if the extension point is enabled"""
        ...

class NeuronExtensionPoint(NLMExtensionPoint):
    """Extension point for custom neuron types"""
    
    def create_neuron(self, neuron_type: str, parameters: Dict[str, Any]) -> Any:
        """Create a custom neuron"""
        ...
    
    def neuron_factory(self, neuron_type: str, **kwargs) -> Any:
        """Factory method for neuron creation"""
        ...

class SynapseExtensionPoint(NLMExtensionPoint):
    """Extension point for custom synapse types"""
    
    def create_synapse(self, synapse_type: str, parameters: Dict[str, Any]) -> Any:
        """Create a custom synapse"""
        ...
    
    def synapse_factory(self, synapse_type: str, **kwargs) -> Any:
        """Factory method for synapse creation"""
        ...

class PlasticityExtensionPoint(NLMExtensionPoint):
    """Extension point for custom plasticity rules"""
    
    def apply_plasticity(self, synapse: Any, pre_spikes: List[float], post_spikes: List[float], 
                        timestep: float) -> float:
        """Apply custom plasticity rule"""
        ...
    
    def plasticity_factory(self, rule_type: str, **kwargs) -> Any:
        """Factory method for plasticity rule creation"""
        ...

class MemoryExtensionPoint(NLMExtensionPoint):
    """Extension point for custom memory systems"""
    
    def store_memory(self, memory_type: str, data: Dict[str, Any]) -> bool:
        """Store custom memory data"""
        ...
    
    def recall_memory(self, memory_type: str, query: Dict[str, Any]) -> Any:
        """Recall custom memory data"""
        ...

class NeuromodulationExtensionPoint(NLMExtensionPoint):
    """Extension point for custom neuromodulators"""
    
    def create_neuromodulator(self, modulator_type: str, parameters: Dict[str, Any]) -> Any:
        """Create a custom neuromodulator"""
        ...
    
    def modulate(self, neuron: Any, modulator: Any, intensity: float) -> None:
        """Apply neuromodulation effect"""
        ...

class PluginManager:
    """Manages NLM plugins and extension points"""
    
    def __init__(self):
        self.plugins: Dict[str, Any] = {}
        self.extension_points: Dict[str, NLMExtensionPoint] = {}
        self.initialized = False
    
    def load_plugin(self, plugin_path: str) -> bool:
        """Load a plugin from the given path"""
        try:
            # Import the plugin module
            spec = import_module_from_path(plugin_path)
            
            # Check if it has the required interface
            if hasattr(spec, 'plugin_info') and hasattr(spec, 'Plugin'):
                plugin_info = spec.plugin_info
                plugin_class = spec.Plugin
                
                # Create plugin instance
                plugin_instance = plugin_class()
                
                # Validate plugin
                if self._validate_plugin(plugin_instance, plugin_info):
                    self.plugins[plugin_info.name] = {
                        'instance': plugin_instance,
                        'info': plugin_info,
                        'extension_points': self._discover_extension_points(plugin_instance)
                    }
                    return True
            
            return False
        except Exception as e:
            print(f"Error loading plugin {plugin_path}: {e}")
            return False
    
    def enable_extension_point(self, extension_point_name: str, extension_point: NLMExtensionPoint) -> bool:
        """Enable an extension point"""
        try:
            # Initialize the extension point
            if extension_point.initialize(self._nlm_core):
                self.extension_points[extension_point_name] = extension_point
                return True
            return False
        except Exception as e:
            print(f"Error enabling extension point {extension_point_name}: {e}")
            return False
    
    def get_extension_point(self, extension_point_name: str) -> Optional[NLMExtensionPoint]:
        """Get an extension point by name"""
        return self.extension_points.get(extension_point_name)
    
    def register_plugin_extension_point(self, plugin_name: str, extension_point_name: str, extension_point: NLMExtensionPoint) -> bool:
        """Register an extension point for a plugin"""
        if plugin_name not in self.plugins:
            return False
        
        return self.enable_extension_point(f"{plugin_name}.{extension_point_name}", extension_point)
    
    def list_plugins(self) -> List[str]:
        """List all loaded plugins"""
        return list(self.plugins.keys())
    
    def list_extension_points(self) -> List[str]:
        """List all enabled extension points"""
        return list(self.extension_points.keys())
    
    def shutdown(self) -> None:
        """Shutdown all plugins and extension points"""
        for plugin_name, plugin_data in self.plugins.items():
            try:
                if hasattr(plugin_data['instance'], 'shutdown'):
                    plugin_data['instance'].shutdown()
            except Exception as e:
                print(f"Error shutting down plugin {plugin_name}: {e}")
        
        for extension_point_name, extension_point in self.extension_points.items():
            try:
                extension_point.shutdown()
            except Exception as e:
                print(f"Error shutting down extension point {extension_point_name}: {e}")
        
        self.plugins.clear()
        self.extension_points.clear()
        self.initialized = False

def import_module_from_path(path: str):
    """Import a module from a file path"""
    import importlib.util
    spec = importlib.util.spec_from_file_location("plugin_module", path)
    if spec is None:
        raise ImportError(f"Could not load module from {path}")
    
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module

class NLMPlugin:
    """Base class for NLM plugins"""
    
    def __init__(self):
        self.info = PluginInfo(
            name="Base Plugin",
            version="1.0.0",
            description="Base NLM plugin",
            author="Unknown",
            capabilities=[],
            dependencies=[],
            entry_point="main"
        )
    
    def initialize(self, nlm_core: Any) -> bool:
        """Initialize plugin with NLM core"""
        return True
    
    def shutdown(self) -> None:
        """Shutdown plugin"""
        pass
    
    def get_info(self) -> PluginInfo:
        """Get plugin information"""
        return self.info
    
    def set_info(self, info: PluginInfo) -> None:
        """Set plugin information"""
        self.info = info

# Extension point registry
class ExtensionPointRegistry:
    """Registry for managing extension points"""
    
    def __init__(self):
        self._extension_point_classes: Dict[str, type] = {}
    
    def register_extension_point(self, name: str, extension_point_class: type) -> None:
        """Register an extension point class"""
        if not issubclass(extension_point_class, NLMExtensionPoint):
            raise ValueError(f"Extension point class {extension_point_class.__name__} does not implement NLMExtensionPoint protocol")
        
        self._extension_point_classes[name] = extension_point_class
    
    def create_extension_point(self, name: str, **kwargs) -> Optional[NLMExtensionPoint]:
        """Create an instance of an extension point"""
        if name not in self._extension_point_classes:
            return None
        
        extension_point_class = self._extension_point_classes[name]
        return extension_point_class(**kwargs)
    
    def list_extension_points(self) -> List[str]:
        """List all registered extension points"""
        return list(self._extension_point_classes.keys())

# Global extension point registry
_extension_point_registry = ExtensionPointRegistry()

def register_extension_point(name: str, extension_point_class: type) -> None:
    """Register an extension point globally"""
    _extension_point_registry.register_extension_point(name, extension_point_class)

def create_extension_point(name: str, **kwargs) -> Optional[NLMExtensionPoint]:
    """Create an extension point instance globally"""
    return _extension_point_registry.create_extension_point(name, **kwargs)

def list_extension_points() -> List[str]:
    """List all registered extension points globally"""
    return _extension_point_registry.list_extension_points()

# Plugin loading utilities
class PluginLoader:
    """Utility class for loading plugins"""
    
    @staticmethod
    def load_from_directory(directory: str) -> List[str]:
        """Load all plugins from a directory"""
        plugin_manager = PluginManager()
        loaded_plugins = []
        
        for filename in os.listdir(directory):
            if filename.endswith('.py') and not filename.startswith('_'):
                plugin_path = os.path.join(directory, filename)
                if plugin_manager.load_plugin(plugin_path):
                    loaded_plugins.append(filename)
        
        return loaded_plugins
    
    @staticmethod
    def load_from_file(filepath: str) -> bool:
        """Load a plugin from a file"""
        plugin_manager = PluginManager()
        return plugin_manager.load_plugin(filepath)

# Configuration utilities
class PluginConfig:
    """Configuration management for plugins"""
    
    def __init__(self, config_file: Optional[str] = None):
        self.config_file = config_file
        self.config_data = {}
        
        if config_file and os.path.exists(config_file):
            with open(config_file, 'r') as f:
                self.config_data = json.load(f)
    
    def get_plugin_config(self, plugin_name: str) -> Dict[str, Any]:
        """Get configuration for a specific plugin"""
        return self.config_data.get('plugins', {}).get(plugin_name, {})
    
    def set_plugin_config(self, plugin_name: str, config: Dict[str, Any]) -> None:
        """Set configuration for a specific plugin"""
        if 'plugins' not in self.config_data:
            self.config_data['plugins'] = {}
        self.config_data['plugins'][plugin_name] = config
        
        if self.config_file:
            with open(self.config_file, 'w') as f:
                json.dump(self.config_data, f, indent=2)
    
    def save_config(self) -> None:
        """Save configuration to file"""
        if self.config_file:
            with open(self.config_file, 'w') as f:
                json.dump(self.config_data, f, indent=2)

# Example plugin implementation
class ExamplePlugin(NLMPlugin):
    """Example plugin demonstrating extension point usage"""
    
    def __init__(self):
        super().__init__()
        self.info = PluginInfo(
            name="Example Plugin",
            version="1.0.0",
            description="Example plugin for demonstrating NLM extension points",
            author="NLM Team",
            capabilities=["neuron", "synapse", "plasticity"],
            dependencies=[],
            entry_point="main"
        )
    
    def initialize(self, nlm_core: Any) -> bool:
        """Initialize plugin with NLM core"""
        if not super().initialize(nlm_core):
            return False
        
        # Register extension points
        from neuron_extension import ExampleNeuronExtension
        from synapse_extension import ExampleSynapseExtension
        from plasticity_extension import ExamplePlasticityExtension
        
        # These would be registered with the plugin manager
        return True

# Export commonly used classes and functions
__all__ = [
    'PluginInfo',
    'NLMExtensionPoint',
    'NeuronExtensionPoint',
    'SynapseExtensionPoint',
    'PlasticityExtensionPoint',
    'MemoryExtensionPoint',
    'NeuromodulationExtensionPoint',
    'PluginManager',
    'NLMPlugin',
    'ExtensionPointRegistry',
    'PluginLoader',
    'PluginConfig',
    'ExamplePlugin',
    'register_extension_point',
    'create_extension_point',
    'list_extension_points',
    'import_module_from_path'
]
