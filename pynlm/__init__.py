"""
Python bindings for NLM (Neural Learning Machine)

This module provides Python bindings for the NLM C++ neural simulation framework.
It includes extension points for plugins and custom functionality.

Key Features:
- Python bindings for NLM core functionality
- Extension point system for plugins
- Custom neuron types
- Custom synapse types
- Custom plasticity rules
- Custom memory systems
- Custom neuromodulation
- Plugin management
- Configuration management

Usage:
    from pynlm import PluginManager, register_extension_point, create_extension_point
    from pynlm.plugin_system import NLMPlugin
    
    # Load plugins
    plugin_manager = PluginManager()
    plugin_manager.load_plugin("path/to/plugin.py")
    
    # Register extension points
    register_extension_point("neuron", ExampleNeuronExtension)
    
    # Create extension point instances
    neuron_extension = create_extension_point("neuron", param1=value1)
"""

from .plugin_system import (
    PluginInfo,
    NLMExtensionPoint,
    NeuronExtensionPoint,
    SynapseExtensionPoint,
    PlasticityExtensionPoint,
    MemoryExtensionPoint,
    NeuromodulationExtensionPoint,
    PluginManager,
    NLMPlugin,
    ExtensionPointRegistry,
    PluginLoader,
    PluginConfig,
    register_extension_point,
    create_extension_point,
    list_extension_points,
    import_module_from_path,
    ExamplePlugin
)

from .neuron_extension import (
    ExampleNeuronPlugin,
    ExampleNeuronExtension
)

from .synapse_extension import (
    ExampleSynapsePlugin,
    ExampleSynapseExtension
)

from .plasticity_extension import (
    ExamplePlasticityPlugin,
    ExamplePlasticityExtension
)

__all__ = [
    # Core plugin system
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
    'register_extension_point',
    'create_extension_point',
    'list_extension_points',
    'import_module_from_path',
    'ExamplePlugin',
    
    # Example plugins
    'ExampleNeuronPlugin',
    'ExampleNeuronExtension',
    'ExampleSynapsePlugin',
    'ExampleSynapseExtension',
    'ExamplePlasticityPlugin',
    'ExamplePlasticityExtension'
]
