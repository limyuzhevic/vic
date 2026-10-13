#!/usr/bin/env python3
"""
Plugin system for NLM neural simulation framework.

This module provides a plugin architecture for extending NLM functionality
with custom components, algorithms, and features.
"""

import importlib
import inspect
import sys
import time
from abc import ABC, abstractmethod
from typing import Dict, Any, List, Optional, Type, Union, Callable
import json
import os

from .enhanced_api import NLMError

class PluginInfo:
    """Metadata for a plugin."""
    
    def __init__(self, name: str, version: str, author: str, 
                 description: str, plugin_type: str):
        self.name = name
        self.version = version
        self.author = author
        self.description = description
        self.plugin_type = plugin_type
        self.dependencies = []
        self.optional_dependencies = []
    
    def add_dependency(self, dependency: str, optional: bool = False):
        """Add a plugin dependency."""
        self.dependencies.append(dependency)
        if optional:
            self.optional_dependencies.append(dependency)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert plugin info to dictionary."""
        return {
            'name': self.name,
            'version': self.version,
            'author': self.author,
            'description': self.description,
            'plugin_type': self.plugin_type,
            'dependencies': self.dependencies,
            'optional_dependencies': self.optional_dependencies,
        }
class Plugin(ABC):
    """Base class for all NLM plugins."""
    
    def __init__(self, plugin_manager):
        self.plugin_manager = plugin_manager
        self.info = PluginInfo(
            name=self.__class__.__name__,
            version='1.0.0',
            author='NLM Team',
            description='NLM Plugin',
            plugin_type='base'
        )
    
    @abstractmethod
    def initialize(self, **kwargs):
        """Initialize the plugin."""
        pass
    
    @abstractmethod
    def shutdown(self):
        """Shutdown the plugin."""
        pass
    
    def get_info(self) -> PluginInfo:
        """Get plugin information."""
        return self.info
    
    def update_info(self, **kwargs):
        """Update plugin information."""
        for key, value in kwargs.items():
            if hasattr(self.info, key):
                setattr(self.info, key, value)
    
    def get_dependencies(self) -> List[str]:
        """Get plugin dependencies."""
        return self.info.dependencies
    
    def get_optional_dependencies(self) -> List[str]:
        """Get optional plugin dependencies."""
        return self.info.optional_dependencies
    
    def check_dependencies(self) -> Dict[str, bool]:
        """Check if all dependencies are available."""
        status = {}
        for dep in self.info.dependencies:
            status[dep] = self._is_dependency_available(dep)
        
        for dep in self.info.optional_dependencies:
            status[dep] = self._is_dependency_available(dep)
        
        return status
    
    def _is_dependency_available(self, dependency: str) -> bool:
        """Check if a dependency is available."""
        # Try to import the dependency
        try:
            importlib.import_module(dependency)
            return True
        except ImportError:
            return False
class PluginManager:
    """
    Plugin manager for NLM plugins.
    
    Manages plugin discovery, loading, initialization, and shutdown.
    """
    
    def __init__(self):
        self.plugins: Dict[str, Plugin] = {}
        self.plugin_info: Dict[str, PluginInfo] = {}
        self.loaded_paths: List[str] = []
    
    def register_plugin(self, plugin_class: Type[Plugin]):
        """Register a plugin class."""
        if not issubclass(plugin_class, Plugin):
            raise NLMError(f"Plugin class {plugin_class} must inherit from Plugin")
        
        plugin_name = plugin_class.__name__
        self.plugins[plugin_name] = plugin_class(self)
        self.plugin_info[plugin_name] = plugin_class(self).get_info()
    
    def load_plugin(self, plugin_class: Type[Plugin], **kwargs):
        """Load and initialize a plugin."""
        if plugin_class.__name__ not in self.plugins:
            self.register_plugin(plugin_class)
        
        plugin = self.plugins[plugin_class.__name__]
        
        # Check dependencies
        dependencies = plugin.check_dependencies()
        missing_dependencies = [dep for dep, available in dependencies.items() if not available]
        
        if missing_dependencies:
            print(f"Warning: Plugin {plugin_class.__name__} has missing dependencies: {missing_dependencies}")
        
        # Initialize plugin
        plugin.initialize(**kwargs)
        return plugin
    
    def unload_plugin(self, plugin_name: str):
        """Unload and shutdown a plugin."""
        if plugin_name not in self.plugins:
            raise NLMError(f"Plugin {plugin_name} not found")
        
        plugin = self.plugins[plugin_name]
        plugin.shutdown()
        
        # Remove from registry
        del self.plugins[plugin_name]
        del self.plugin_info[plugin_name]
    
    def get_plugin(self, plugin_name: str) -> Optional[Plugin]:
        """Get a plugin by name."""
        return self.plugins.get(plugin_name)
    
    def get_all_plugins(self) -> List[Plugin]:
        """Get all registered plugins."""
        return list(self.plugins.values())
    
    def get_plugin_info(self, plugin_name: str) -> Optional[PluginInfo]:
        """Get plugin information by name."""
        return self.plugin_info.get(plugin_name)
    
    def get_all_plugin_info(self) -> List[PluginInfo]:
        """Get information about all plugins."""
        return list(self.plugin_info.values())
    
    def has_plugin(self, plugin_name: str) -> bool:
        """Check if a plugin is registered."""
        return plugin_name in self.plugins
    
    def list_plugins(self) -> List[str]:
        """List all registered plugin names."""
        return list(self.plugins.keys())
    
    def save_plugin_state(self, plugin_name: str, filepath: str):
        """Save plugin state to file."""
        if plugin_name not in self.plugins:
            raise NLMError(f"Plugin {plugin_name} not found")
        
        plugin = self.plugins[plugin_name]
        
        # Create state data
        state = {
            'plugin_name': plugin_name,
            'timestamp': time.time(),
            'plugin_info': plugin.get_info().to_dict(),
            'state': plugin.get_state() if hasattr(plugin, 'get_state') else {},
        }
        
        # Save to file
        with open(filepath, 'w') as f:
            json.dump(state, f, indent=2, default=str)
    
    def load_plugin_state(self, filepath: str) -> Plugin:
        """Load plugin state from file."""
        # Load state
        with open(filepath, 'r') as f:
            state = json.load(f)
        
        plugin_name = state['plugin_name']
        
        # Check if plugin is registered
        if plugin_name not in self.plugins:
            raise NLMError(f"Plugin {plugin_name} not found in registry")
        
        # Load state into plugin
        plugin = self.plugins[plugin_name]
        if hasattr(plugin, 'set_state'):
            plugin.set_state(state.get('state', {}))
        
        return plugin
class NeuronDynamicsPlugin(Plugin):
    """
    Plugin for customizing neuron dynamics.
    
    Allows users to modify neuron behavior, such as changing neuron types,
    firing dynamics, or synaptic properties.
    """
    
    def __init__(self, plugin_manager):
        super().__init__(plugin_manager)
        
        self.update_info(
            name='NeuronDynamicsPlugin',
            description='Custom neuron dynamics and behavior',
            plugin_type='neuron_dynamics'
        )
        
        # Custom neuron types
        self.custom_neuron_types = {}
        
        # Custom dynamics parameters
        self.custom_dynamics_params = {}
    
    def initialize(self, **kwargs):
        """Initialize neuron dynamics plugin."""
        # Load custom neuron types
        custom_types = kwargs.get('custom_neuron_types', {})
        self.custom_neuron_types.update(custom_types)
        
        # Load custom dynamics parameters
        dynamics_params = kwargs.get('custom_dynamics_params', {})
        self.custom_dynamics_params.update(dynamics_params)
        
        print(f"NeuronDynamicsPlugin initialized with {len(self.custom_neuron_types)} custom neuron types")
    
    def shutdown(self):
        """Shutdown neuron dynamics plugin."""
        self.custom_neuron_types.clear()
        self.custom_dynamics_params.clear()
        print("NeuronDynamicsPlugin shut down")
    
    def register_neuron_type(self, neuron_type_name: str, params: Dict[str, Any]):
        """Register a custom neuron type."""
        self.custom_neuron_types[neuron_type_name] = params
    
    def get_neuron_type_params(self, neuron_type_name: str) -> Optional[Dict[str, Any]]:
        """Get parameters for a neuron type."""
        return self.custom_neuron_types.get(neuron_type_name)
    
    def modify_neuron_behavior(self, neuron, step_data: Dict[str, Any]) -> Dict[str, Any]:
        """Modify neuron behavior based on custom dynamics."""
        neuron_type = getattr(neuron, 'neuron_type', None)
        
        if neuron_type and neuron_type in self.custom_neuron_types:
            # Apply custom dynamics
            custom_params = self.custom_neuron_types[neuron_type]
            
            # Modify neuron behavior
            modified_step_data = step_data.copy()
            
            # Apply custom modifications
            if 'firing_threshold' in custom_params:
                modified_step_data['firing_threshold'] = custom_params['firing_threshold']
            
            if 'decay_rate' in custom_params:
                modified_step_data['decay_rate'] = custom_params['decay_rate']
            
            if 'plasticity_factor' in custom_params:
                modified_step_data['plasticity_factor'] = custom_params['plasticity_factor']
            
            return modified_step_data
        
        return step_data
class PlasticityPlugin(Plugin):
    """
    Plugin for customizing plasticity rules.
    
    Allows users to modify synaptic plasticity rules, such as STDP, Hebbian,
    or custom plasticity mechanisms.
    """
    
    def __init__(self, plugin_manager):
        super().__init__(plugin_manager)
        
        self.update_info(
            name='PlasticityPlugin',
            description='Custom synaptic plasticity rules and mechanisms',
            plugin_type='plasticity'
        )
        
        # Custom plasticity rules
        self.custom_plasticity_rules = {}
        
        # Plasticity parameters
        self.plasticity_params = {}
    
    def initialize(self, **kwargs):
        """Initialize plasticity plugin."""
        # Load custom plasticity rules
        custom_rules = kwargs.get('custom_plasticity_rules', {})
        self.custom_plasticity_rules.update(custom_rules)
        
        # Load plasticity parameters
        params = kwargs.get('plasticity_params', {})
        self.plasticity_params.update(params)
        
        print(f"PlasticityPlugin initialized with {len(self.custom_plasticity_rules)} custom rules")
    
    def shutdown(self):
        """Shutdown plasticity plugin."""
        self.custom_plasticity_rules.clear()
        self.plasticity_params.clear()
        print("PlasticityPlugin shut down")
    
    def register_plasticity_rule(self, rule_name: str, rule_function: Callable):
        """Register a custom plasticity rule."""
        self.custom_plasticity_rules[rule_name] = rule_function
    
    def apply_plasticity_rule(self, synapse, pre_spike_time: float, post_spike_time: float,
                            rule_name: str = None) -> float:
        """Apply a plasticity rule to a synapse."""
        if rule_name and rule_name in self.custom_plasticity_rules:
            # Apply custom rule
            rule_function = self.custom_plasticity_rules[rule_name]
            return rule_function(synapse, pre_spike_time, post_spike_time)
        
        return 0.0
    
    def get_plasticity_parameter(self, param_name: str) -> Any:
        """Get plasticity parameter value."""
        return self.plasticity_params.get(param_name)
class EnvironmentPlugin(Plugin):
    """
    Plugin for customizing world environments.
    
    Allows users to modify world behavior, such as adding new objects,
    changing world physics, or creating custom environments.
    """
    
    def __init__(self, plugin_manager):
        super().__init__(plugin_manager)
        
        self.update_info(
            name='EnvironmentPlugin',
            description='Custom world environments and physics',
            plugin_type='environment'
        )
        
        # Custom world objects
        self.custom_objects = []
        
        # Custom world physics
        self.world_physics = {}
    
    def initialize(self, **kwargs):
        """Initialize environment plugin."""
        # Load custom objects
        custom_objects = kwargs.get('custom_objects', [])
        self.custom_objects.extend(custom_objects)
        
        # Load world physics
        physics = kwargs.get('world_physics', {})
        self.world_physics.update(physics)
        
        print(f"EnvironmentPlugin initialized with {len(self.custom_objects)} custom objects")
    
    def shutdown(self):
        """Shutdown environment plugin."""
        self.custom_objects.clear()
        self.world_physics.clear()
        print("EnvironmentPlugin shut down")
    
    def add_custom_object(self, obj):
        """Add a custom world object."""
        self.custom_objects.append(obj)
    
    def remove_custom_object(self, obj_id: str):
        """Remove a custom world object."""
        self.custom_objects = [obj for obj in self.custom_objects if getattr(obj, 'id', None) != obj_id]
    
    def apply_world_physics(self, position: Any, velocity: Any, forces: Any) -> Any:
        """Apply custom world physics."""
        # Get custom physics parameters
        physics_params = self.world_physics.get('gravity', 9.81)
        
        # Apply custom physics
        new_velocity = velocity.copy()
        new_position = position.copy()
        
        # Apply gravity
        new_velocity[1] -= physics_params  # Gravity in Y direction
        
        # Apply other custom physics
        for force_type, force_func in self.world_physics.items():
            if force_type != 'gravity':
                force = force_func(position, velocity)
                new_velocity = [new_velocity[i] + force[i] for i in range(len(new_velocity))]
        
        # Update position
        new_position = [new_position[i] + new_velocity[i] for i in range(len(new_position))]
        
        return {'position': new_position, 'velocity': new_velocity}
class VisualizationPlugin(Plugin):
    """
    Plugin for customizing visualization.
    
    Allows users to add custom visualization components, such as new
    visualization formats, interactive elements, or custom displays.
    """
    
    def __init__(self, plugin_manager):
        super().__init__(plugin_manager)
        
        self.update_info(
            name='VisualizationPlugin',
            description='Custom visualization components and displays',
            plugin_type='visualization'
        )
        
        # Custom visualizers
        self.visualizers = {}
        
        # Visualization parameters
        self.viz_params = {}
    
    def initialize(self, **kwargs):
        """Initialize visualization plugin."""
        # Load custom visualizers
        visualizers = kwargs.get('visualizers', {})
        self.visualizers.update(visualizers)
        
        # Load visualization parameters
        params = kwargs.get('viz_params', {})
        self.viz_params.update(params)
        
        print(f"VisualizationPlugin initialized with {len(self.visualizers)} visualizers")
    
    def shutdown(self):
        """Shutdown visualization plugin."""
        self.visualizers.clear()
        self.viz_params.clear()
        print("VisualizationPlugin shut down")
    
    def register_visualizer(self, name: str, visualizer_function: Callable):
        """Register a custom visualizer."""
        self.visualizers[name] = visualizer_function
    
    def get_visualizer(self, name: str) -> Optional[Callable]:
        """Get a visualizer by name."""
        return self.visualizers.get(name)
    
    def render_visualization(self, data: Any, viz_name: str = None, **kwargs) -> Any:
        """Render visualization with specified visualizer."""
        if viz_name and viz_name in self.visualizers:
            visualizer = self.visualizers[viz_name]
            return visualizer(data, **kwargs)
        
        # Default visualization
        return self._default_visualization(data, **kwargs)
    
    def _default_visualization(self, data: Any, **kwargs) -> Any:
        """Default visualization."""
        return {
            'type': 'default',
            'data': data,
            'parameters': kwargs,
        }
class AnalyticsPlugin(Plugin):
    """
    Plugin for analytics and data analysis.
    
    Allows users to add custom analytics components, such as statistical
    analysis, pattern detection, or machine learning integration.
    """
    
    def __init__(self, plugin_manager):
        super().__init__(plugin_manager)
        
        self.update_info(
            name='AnalyticsPlugin',
            description='Custom analytics and data analysis components',
            plugin_type='analytics'
        )
        
        # Analytics functions
        self.analytics_functions = {}
        
        # Analysis parameters
        self.analysis_params = {}
    
    def initialize(self, **kwargs):
        """Initialize analytics plugin."""
        # Load analytics functions
        functions = kwargs.get('analytics_functions', {})
        self.analytics_functions.update(functions)
        
        # Load analysis parameters
        params = kwargs.get('analysis_params', {})
        self.analysis_params.update(params)
        
        print(f"AnalyticsPlugin initialized with {len(self.analytics_functions)} analytics functions")
    
    def shutdown(self):
        """Shutdown analytics plugin."""
        self.analytics_functions.clear()
        self.analysis_params.clear()
        print("AnalyticsPlugin shut down")
    
    def register_analytics_function(self, name: str, analytics_function: Callable):
        """Register a custom analytics function."""
        self.analytics_functions[name] = analytics_function
    
    def get_analytics_function(self, name: str) -> Optional[Callable]:
        """Get an analytics function by name."""
        return self.analytics_functions.get(name)
    
    def perform_analysis(self, data: Any, analytics_name: str = None, **kwargs) -> Any:
        """Perform analysis with specified analytics function."""
        if analytics_name and analytics_name in self.analytics_functions:
            analytics_function = self.analytics_functions[analytics_name]
            return analytics_function(data, **kwargs)
        
        # Default analytics
        return self._default_analysis(data, **kwargs)
    
    def _default_analysis(self, data: Any, **kwargs) -> Any:
        """Default analysis."""
        return {
            'type': 'default',
            'data': data,
            'parameters': kwargs,
            'summary': f'Analysis of {type(data).__name__} data',
        }
# Global plugin manager
_plugin_manager = PluginManager()

def get_plugin_manager() -> PluginManager:
    """Get the global plugin manager."""
    return _plugin_manager

def register_plugin(plugin_class: Type[Plugin]):
    """Register a plugin globally."""
    _plugin_manager.register_plugin(plugin_class)

def load_plugin(plugin_class: Type[Plugin], **kwargs):
    """Load a plugin globally."""
    return _plugin_manager.load_plugin(plugin_class, **kwargs)

def unload_plugin(plugin_name: str):
    """Unload a plugin globally."""
    _plugin_manager.unload_plugin(plugin_name)

def get_plugin(plugin_name: str) -> Optional[Plugin]:
    """Get a plugin globally."""
    return _plugin_manager.get_plugin(plugin_name)

# Plugin file utilities

def load_plugins_from_directory(directory: str, plugin_manager: PluginManager = None):
    """
    Load plugins from a directory.
    
    Args:
        directory: Directory containing plugin modules
        plugin_manager: Plugin manager to use (creates new if None)
    """
    if plugin_manager is None:
        plugin_manager = PluginManager()
    
    # Get all Python files in directory
    for filename in os.listdir(directory):
        if filename.endswith('.py') and filename != '__init__.py':
            plugin_name = filename[:-3]  # Remove .py extension
            
            try:
                # Import the module
                module_spec = importlib.util.spec_from_file_location(
                    plugin_name, os.path.join(directory, filename)
                )
                module = importlib.util.module_from_spec(module_spec)
                
                # Find plugin classes
                for attr_name in dir(module):
                    attr = getattr(module, attr_name)
                    if (inspect.isclass(attr) and 
                        issubclass(attr, Plugin) and 
                        attr is not Plugin):
                        
                        # Register the plugin
                        plugin_manager.register_plugin(attr)
                
            except Exception as e:
                print(f"Warning: Failed to load plugin from {filename}: {e}")
    
    return plugin_manager
