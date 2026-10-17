#!/usr/bin/env python3
"""
Plugin Architecture for Enhanced NLM Python Bindings

This module provides a comprehensive plugin architecture for the Enhanced
NLM Python bindings, enabling extension modules, custom callback systems,
and plugin-based functionality for neural simulation research and development.
"""

import sys
import os
import importlib
import inspect
from typing import List, Dict, Tuple, Optional, Any, Callable, Union
from abc import ABC, abstractmethod
import warnings
import json

warnings.filterwarnings('ignore')

class PluginType(Enum):
    """Plugin types"""
    EXTENSION = "extension"
    CALLBACK = "callback"
    ANALYZER = "analyzer"
    VISUALIZER = "visualizer"
    TRANSFORMER = "transformer"
    VALIDATOR = "validator"
    EXPORTER = "exporter"
    IMPORTER = "importer"

class PluginStatus(Enum):
    """Plugin status"""
    DISABLED = "disabled"
    ENABLED = "enabled"
    LOADED = "loaded"
    ERROR = "error"
    UNAVAILABLE = "unavailable"

@dataclass
class PluginMetadata:
    """Plugin metadata"""
    id: str
    name: str
    version: str
    description: str
    author: str
    type: PluginType
    status: PluginStatus
    dependencies: List[str] = None
    capabilities: List[str] = None
    config: Dict[str, Any] = None
    
    def __post_init__(self):
        if self.dependencies is None:
            self.dependencies = []
        if self.capabilities is None:
            self.capabilities = []
        if self.config is None:
            self.config = {}

class PluginInterface(ABC):
    """Abstract base class for all plugins"""
    
    @abstractmethod
    def initialize(self, config: Dict[str, Any]) -> bool:
        """Initialize plugin with configuration"""
        pass
    
    @abstractmethod
    def cleanup(self) -> bool:
        """Clean up plugin resources"""
        pass
    
    @abstractmethod
    def get_metadata(self) -> PluginMetadata:
        """Get plugin metadata"""
        pass
    
    def validate_dependencies(self, plugin_manager: 'PluginManager') -> bool:
        """Validate plugin dependencies"""
        return True
    
    def on_simulation_start(self, simulation_context: Dict[str, Any]) -> Dict[str, Any]:
        """Called when simulation starts"""
        return {}
    
    def on_simulation_step(self, step_data: Dict[str, Any]) -> Dict[str, Any]:
        """Called on each simulation step"""
        return {}
    
    def on_simulation_end(self, simulation_context: Dict[str, Any]) -> Dict[str, Any]:
        """Called when simulation ends"""
        return {}

class ExtensionPlugin(PluginInterface):
    """Plugin that extends NLM functionality"""
    
    def __init__(self):
        self.extension_functions: Dict[str, Callable] = {}
        self.extension_classes: Dict[str, type] = {}
        self.extension_constants: Dict[str, Any] = {}
    
    def add_function(self, name: str, func: Callable):
        """Add a new function to the plugin"""
        self.extension_functions[name] = func
    
    def add_class(self, name: str, cls: type):
        """Add a new class to the plugin"""
        self.extension_classes[name] = cls
    
    def add_constant(self, name: str, value: Any):
        """Add a new constant to the plugin"""
        self.extension_constants[name] = value
    
    def get_available_functions(self) -> List[str]:
        """Get list of available extension functions"""
        return list(self.extension_functions.keys())
    
    def get_available_classes(self) -> List[str]:
        """Get list of available extension classes"""
        return list(self.extension_classes.keys())
    
    def get_available_constants(self) -> List[str]:
        """Get list of available extension constants"""
        return list(self.extension_constants.keys())
class CallbackPlugin(PluginInterface):
    """Plugin that implements callback system"""
    
    def __init__(self):
        self.pre_simulation_hooks: List[Callable[[Dict[str, Any]], Dict[str, Any]]] = []
        self.post_simulation_hooks: List[Callable[[Dict[str, Any]], Dict[str, Any]]] = []
        self.step_hooks: List[Callable[[Dict[str, Any]], Dict[str, Any]]] = []
        self.error_handlers: List[Callable[[Exception], bool]] = []
    
    def add_pre_simulation_hook(self, hook: Callable[[Dict[str, Any]], Dict[str, Any]]):
        """Add pre-simulation hook"""
        self.pre_simulation_hooks.append(hook)
    
    def add_post_simulation_hook(self, hook: Callable[[Dict[str, Any]], Dict[str, Any]]):
        """Add post-simulation hook"""
        self.post_simulation_hooks.append(hook)
    
    def add_step_hook(self, hook: Callable[[Dict[str, Any]], Dict[str, Any]]):
        """Add step hook"""
        self.step_hooks.append(hook)
    
    def add_error_handler(self, handler: Callable[[Exception], bool]):
        """Add error handler"""
        self.error_handlers.append(handler)
    
    def execute_hooks(self, hook_type: str, context: Dict[str, Any]) -> Dict[str, Any]:
        """Execute hooks of specified type"""
        results = {}
        
        if hook_type == 'pre_simulation':
            hooks = self.pre_simulation_hooks
        elif hook_type == 'post_simulation':
            hooks = self.post_simulation_hooks
        elif hook_type == 'step':
            hooks = self.step_hooks
        else:
            return results
        
        for hook in hooks:
            try:
                result = hook(context)
                results.update(result)
            except Exception:
                pass
        
        return results
class AnalyzerPlugin(PluginInterface):
    """Plugin for neural data analysis"""
    
    def __init__(self):
        self.analysis_functions: Dict[str, Callable] = {}
        self.analysis_algorithms: List[str] = []
    
    def add_analysis_function(self, name: str, func: Callable):
        """Add analysis function"""
        self.analysis_functions[name] = func
    
    def add_analysis_algorithm(self, algorithm: str):
        """Add analysis algorithm"""
        self.analysis_algorithms.append(algorithm)
    
    def perform_analysis(self, data: Dict[str, Any], algorithm: str) -> Dict[str, Any]:
        """Perform analysis on data using specified algorithm"""
        if algorithm not in self.analysis_algorithms:
            return {'error': f'Algorithm {algorithm} not supported'}
        
        if algorithm in self.analysis_functions:
            return self.analysis_functions[algorithm](data)
        
        return {'error': f'No implementation for algorithm {algorithm}'}
class VisualizerPlugin(PluginInterface):
    """Plugin for neural visualization"""
    
    def __init__(self):
        self.visualization_methods: Dict[str, Callable] = {}
        self.plot_types: List[str] = []
    
    def add_visualization_method(self, name: str, method: Callable):
        """Add visualization method"""
        self.visualization_methods[name] = method
    
    def add_plot_type(self, plot_type: str):
        """Add plot type"""
        self.plot_types.append(plot_type)
    
    def create_visualization(self, data: Dict[str, Any], plot_type: str,
                            config: Optional[Dict[str, Any]] = None) -> Any:
        """Create visualization of data"""
        if plot_type not in self.plot_types:
            return None
        
        if plot_type in self.visualization_methods:
            return self.visualization_methods[plot_type](data, config or {})
        
        return None
class TransformerPlugin(PluginInterface):
    """Plugin for data transformation"""
    
    def __init__(self):
        self.transformations: Dict[str, Callable] = {}
        self.preprocessors: List[str] = []
    
    def add_transformation(self, name: str, transformation: Callable):
        """Add data transformation"""
        self.transformations[name] = transformation
    
    def add_preprocessor(self, preprocessor: str):
        """Add data preprocessor"""
        self.preprocessors.append(preprocessor)
    
    def transform_data(self, data: Dict[str, Any], transformation: str) -> Dict[str, Any]:
        """Transform data using specified transformation"""
        if transformation not in self.transformations:
            return {'error': f'Transformation {transformation} not supported'}
        
        try:
            return self.transformations[transformation](data)
        except Exception as e:
            return {'error': f'Transformation failed: {e}'}
class ValidatorPlugin(PluginInterface):
    """Plugin for data validation"""
    
    def __init__(self):
        self.validation_rules: Dict[str, Callable] = {}
        self.validators: List[str] = []
    
    def add_validation_rule(self, name: str, rule: Callable):
        """Add validation rule"""
        self.validation_rules[name] = rule
    
    def add_validator(self, validator: str):
        """Add validator"""
        self.validators.append(validator)
    
    def validate_data(self, data: Dict[str, Any], validator: str) -> Dict[str, Any]:
        """Validate data using specified validator"""
        if validator not in self.validators:
            return {'error': f'Validator {validator} not supported'}
        
        if validator in self.validation_rules:
            return self.validation_rules[validator](data)
        
        return {'error': f'No implementation for validator {validator}'}
class ExporterPlugin(PluginInterface):
    """Plugin for data export"""
    
    def __init__(self):
        self.exporters: Dict[str, Callable] = {}
        self.export_formats: List[str] = []
    
    def add_exporter(self, name: str, exporter: Callable):
        """Add data exporter"""
        self.exporters[name] = exporter
    
    def add_export_format(self, format_name: str):
        """Add export format"""
        self.export_formats.append(format_name)
    
    def export_data(self, data: Dict[str, Any], format_name: str,
                   config: Optional[Dict[str, Any]] = None) -> Any:
        """Export data in specified format"""
        if format_name not in self.export_formats:
            return {'error': f'Format {format_name} not supported'}
        
        if format_name in self.exporters:
            return self.exporters[format_name](data, config or {})
        
        return {'error': f'No implementation for format {format_name}'}
class ImporterPlugin(PluginInterface):
    """Plugin for data import"""
    
    def __init__(self):
        self.importers: Dict[str, Callable] = {}
        self.import_formats: List[str] = []
    
    def add_importer(self, name: str, importer: Callable):
        """Add data importer"""
        self.importers[name] = importer
    
    def add_import_format(self, format_name: str):
        """Add import format"""
        self.import_formats.append(format_name)
    
    def import_data(self, source: Any, format_name: str,
                   config: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Import data from specified format"""
        if format_name not in self.import_formats:
            return {'error': f'Format {format_name} not supported'}
        
        if format_name in self.importers:
            return self.importers[format_name](source, config or {})
        
        return {'error': f'No implementation for format {format_name}'}
class PluginManager:
    """Manages plugins for Enhanced NLM Python bindings"""
    
    def __init__(self):
        self.plugins: Dict[str, PluginInterface] = {}
        self.plugin_metadata: Dict[str, PluginMetadata] = {}
        self.active_plugins: List[str] = []
        self.plugin_registry: List[str] = []
        self.plugin_configs: Dict[str, Dict[str, Any]] = {}
    
    def register_plugin(self, plugin_path: str, config: Optional[Dict[str, Any]] = None):
        """Register a plugin from file"""
        try:
            # Import plugin module
            spec = importlib.util.spec_from_file_location("plugin", plugin_path)
            if spec is None or spec.loader is None:
                raise ImportError(f"Cannot load plugin from {plugin_path}")
            
            plugin_module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(plugin_module)
            
            # Find plugin class
            plugin_class = None
            for attr_name in dir(plugin_module):
                attr = getattr(plugin_module, attr_name)
                if (inspect.isclass(attr) and 
                    issubclass(attr, PluginInterface) and 
                    attr != PluginInterface and
                    attr != ExtensionPlugin and
                    attr != CallbackPlugin and
                    attr != AnalyzerPlugin and
                    attr != VisualizerPlugin and
                    attr != TransformerPlugin and
                    attr != ValidatorPlugin and
                    attr != ExporterPlugin and
                    attr != ImporterPlugin):
                    
                    plugin_class = attr
                    break
            
            if plugin_class is None:
                # Try to find plugin class in module globals
                if hasattr(plugin_module, 'plugin_class'):
                    plugin_class = plugin_module.plugin_class
                elif hasattr(plugin_module, 'default_plugin'):
                    plugin_class = plugin_module.default_plugin
                else:
                    raise ImportError(f"No plugin class found in {plugin_path}")
            
            # Create plugin instance
            plugin_instance = plugin_class()
            
            # Get plugin metadata
            metadata = plugin_instance.get_metadata()
            
            # Store plugin
            self.plugins[metadata.id] = plugin_instance
            self.plugin_metadata[metadata.id] = metadata
            self.plugin_registry.append(metadata.id)
            
            # Store config
            if config:
                self.plugin_configs[metadata.id] = config
            
            # Initialize plugin
            plugin_config = self.plugin_configs.get(metadata.id, {})
            if plugin_instance.initialize(plugin_config):
                metadata.status = PluginStatus.LOADED
                self.active_plugins.append(metadata.id)
                self.plugin_configs[metadata.id] = plugin_config
                return True
            else:
                metadata.status = PluginStatus.ERROR
                return False
            
        except Exception as e:
            self._log_plugin_error(plugin_path, e)
            return False
    
    def unregister_plugin(self, plugin_id: str) -> bool:
        """Unregister a plugin"""
        if plugin_id not in self.plugins:
            return False
        
        plugin = self.plugins[plugin_id]
        
        # Clean up plugin
        plugin.cleanup()
        
        # Remove from registries
        del self.plugins[plugin_id]
        del self.plugin_metadata[plugin_id]
        if plugin_id in self.active_plugins:
            self.active_plugins.remove(plugin_id)
        if plugin_id in self.plugin_configs:
            del self.plugin_configs[plugin_id]
        
        if plugin_id in self.plugin_registry:
            self.plugin_registry.remove(plugin_id)
        
        return True
    
    def enable_plugin(self, plugin_id: str, config: Optional[Dict[str, Any]] = None) -> bool:
        """Enable a plugin"""
        if plugin_id not in self.plugin_metadata:
            return False
        
        metadata = self.plugin_metadata[plugin_id]
        
        if metadata.status == PluginStatus.LOADED:
            # Initialize plugin
            plugin_config = config or self.plugin_configs.get(plugin_id, {})
            if self.plugins[plugin_id].initialize(plugin_config):
                metadata.status = PluginStatus.ENABLED
                if plugin_id not in self.active_plugins:
                    self.active_plugins.append(plugin_id)
                self.plugin_configs[plugin_id] = plugin_config
                return True
            else:
                metadata.status = PluginStatus.ERROR
                return False
        
        return False
    
    def disable_plugin(self, plugin_id: str) -> bool:
        """Disable a plugin"""
        if plugin_id not in self.plugin_metadata:
            return False
        
        metadata = self.plugin_metadata[plugin_id]
        
        if metadata.status in [PluginStatus.ENABLED, PluginStatus.LOADED]:
            metadata.status = PluginStatus.DISABLED
            if plugin_id in self.active_plugins:
                self.active_plugins.remove(plugin_id)
            return True
        
        return False
    
    def get_plugin(self, plugin_id: str) -> Optional[PluginInterface]:
        """Get plugin by ID"""
        return self.plugins.get(plugin_id)
    
    def get_plugin_metadata(self, plugin_id: str) -> Optional[PluginMetadata]:
        """Get plugin metadata by ID"""
        return self.plugin_metadata.get(plugin_id)
    
    def get_active_plugins(self) -> List[str]:
        """Get list of active plugin IDs"""
        return self.active_plugins.copy()
    
    def get_plugin_capabilities(self, plugin_id: str) -> List[str]:
        """Get plugin capabilities"""
        metadata = self.get_plugin_metadata(plugin_id)
        return metadata.capabilities if metadata else []
    
    def get_all_capabilities(self) -> List[str]:
        """Get all capabilities from all plugins"""
        capabilities = []
        for plugin_id in self.active_plugins:
            capabilities.extend(self.get_plugin_capabilities(plugin_id))
        return list(set(capabilities))
    
    def execute_plugin_hooks(self, hook_type: str, context: Dict[str, Any]) -> Dict[str, Any]:
        """Execute plugin hooks of specified type"""
        results = {}
        
        for plugin_id in self.active_plugins:
            plugin = self.get_plugin(plugin_id)
            if plugin:
                try:
                    if hasattr(plugin, 'execute_hooks'):
                        hook_results = plugin.execute_hooks(hook_type, context)
                        results.update(hook_results)
                except Exception as e:
                    self._log_plugin_error(plugin_id, e)
        
        return results
    
    def get_extension_functions(self) -> Dict[str, Callable]:
        """Get all extension functions from all plugins"""
        extension_functions = {}
        
        for plugin_id in self.active_plugins:
            plugin = self.get_plugin(plugin_id)
            if isinstance(plugin, ExtensionPlugin):
                extension_functions.update(plugin.extension_functions)
        
        return extension_functions
    
    def get_analyzer_functions(self) -> Dict[str, Callable]:
        """Get all analyzer functions from all plugins"""
        analyzer_functions = {}
        
        for plugin_id in self.active_plugins:
            plugin = self.get_plugin(plugin_id)
            if isinstance(plugin, AnalyzerPlugin):
                analyzer_functions.update(plugin.analysis_functions)
        
        return analyzer_functions
    
    def get_visualization_methods(self) -> Dict[str, Callable]:
        """Get all visualization methods from all plugins"""
        visualization_methods = {}
        
        for plugin_id in self.active_plugins:
            plugin = self.get_plugin(plugin_id)
            if isinstance(plugin, VisualizerPlugin):
                visualization_methods.update(plugin.visualization_methods)
        
        return visualization_methods
    
    def get_exporter_functions(self) -> Dict[str, Callable]:
        """Get all exporter functions from all plugins"""
        exporter_functions = {}
        
        for plugin_id in self.active_plugins:
            plugin = self.get_plugin(plugin_id)
            if isinstance(plugin, ExporterPlugin):
                exporter_functions.update(plugin.exporters)
        
        return exporter_functions
    
    def get_importer_functions(self) -> Dict[str, Callable]:
        """Get all importer functions from all plugins"""
        importer_functions = {}
        
        for plugin_id in self.active_plugins:
            plugin = self.get_plugin(plugin_id)
            if isinstance(plugin, ImporterPlugin):
                importer_functions.update(plugin.importers)
        
        return importer_functions
    
    def export_plugin(self, plugin_id: str, format_name: str,
                     config: Optional[Dict[str, Any]] = None) -> Any:
        """Export plugin to file"""
        plugin = self.get_plugin(plugin_id)
        if not plugin:
            return {'error': f'Plugin {plugin_id} not found'}
        
        if isinstance(plugin, ExporterPlugin):
            return plugin.export_data({}, format_name, config or {})
        
        return {'error': f'Plugin {plugin_id} does not support export'}
    
    def import_plugin(self, source: Any, format_name: str,
                     config: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Import plugin from source"""
        importer_functions = self.get_importer_functions()
        
        if format_name in importer_functions:
            return importer_functions[format_name](source, config or {})
        
        return {'error': f'No importer for format {format_name}'}
    
    def get_all_plugins(self) -> Dict[str, PluginMetadata]:
        """Get all plugin metadata"""
        return self.plugin_metadata.copy()
    
    def _log_plugin_error(self, plugin_id: str, error: Exception):
        """Log plugin error"""
        print(f"Plugin {plugin_id} error: {error}")
        if hasattr(self, 'logger'):
            self.logger.error(f"Plugin {plugin_id} error: {error}")
    
    def export_plugin_registry(self, filepath: str):
        """Export plugin registry to file"""
        registry = {
            'plugins': {},
            'active_plugins': self.active_plugins,
            'plugin_configs': self.plugin_configs
        }
        
        for plugin_id, metadata in self.plugin_metadata.items():
            registry['plugins'][plugin_id] = {
                'metadata': asdict(metadata),
                'type': metadata.type.value,
                'status': metadata.status.value,
                'name': metadata.name,
                'version': metadata.version,
                'description': metadata.description,
                'author': metadata.author,
                'dependencies': metadata.dependencies,
                'capabilities': metadata.capabilities
            }
        
        with open(filepath, 'w') as f:
            json.dump(registry, f, indent=2)
        
        return True
    
    def import_plugin_registry(self, filepath: str):
        """Import plugin registry from file"""
        try:
            with open(filepath, 'r') as f:
                registry = json.load(f)
            
            # Import plugins
            for plugin_id, plugin_data in registry['plugins'].items():
                # Create plugin metadata
                metadata = PluginMetadata(
                    id=plugin_id,
                    name=plugin_data['name'],
                    version=plugin_data['version'],
                    description=plugin_data['description'],
                    author=plugin_data['author'],
                    type=PluginType(plugin_data['type']),
                    status=PluginStatus(plugin_data['status']),
                    dependencies=plugin_data.get('dependencies', []),
                    capabilities=plugin_data.get('capabilities', []),
                    config=plugin_data.get('config', {})
                )
                
                self.plugin_metadata[plugin_id] = metadata
                self.plugin_registry.append(plugin_id)
                self.plugin_configs[plugin_id] = plugin_data.get('config', {})
            
            # Set active plugins
            self.active_plugins = registry.get('active_plugins', [])
            
            return True
            
        except Exception as e:
            self._log_plugin_error("registry", e)
            return False

# Convenience functions

def create_plugin_manager() -> PluginManager:
    """Convenience function to create a plugin manager"""
    return PluginManager()

def load_plugin(plugin_manager: PluginManager, plugin_path: str, config: Optional[Dict[str, Any]] = None) -> bool:
    """Convenience function to load a plugin"""
    return plugin_manager.register_plugin(plugin_path, config)

def enable_plugin(plugin_manager: PluginManager, plugin_id: str, config: Optional[Dict[str, Any]] = None) -> bool:
    """Convenience function to enable a plugin"""
    return plugin_manager.enable_plugin(plugin_id, config)

def disable_plugin(plugin_manager: PluginManager, plugin_id: str) -> bool:
    """Convenience function to disable a plugin"""
    return plugin_manager.disable_plugin(plugin_id)

# Export all classes and functions
__all__ = [
    'PluginType', 'PluginStatus', 'PluginMetadata', 'PluginInterface',
    'ExtensionPlugin', 'CallbackPlugin', 'AnalyzerPlugin', 'VisualizerPlugin',
    'TransformerPlugin', 'ValidatorPlugin', 'ExporterPlugin', 'ImporterPlugin',
    'PluginManager', 'create_plugin_manager', 'load_plugin',
    'enable_plugin', 'disable_plugin'
]
