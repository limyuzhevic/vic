# NLM Plugin System - Quickstart Guide

## Overview

This quickstart guide provides a step-by-step introduction to the NLM plugin system, covering the essential concepts and demonstrating how to create and use custom plugins.

## What is the NLM Plugin System?

The NLM plugin system is a modular, extensible framework for creating custom neural learning components without modifying the core NLM code. It enables users to extend the NLM functionality through custom plugins that can be loaded at runtime.

### Key Benefits

- **Modularity**: Create custom components without changing core NLM code
- **Extensibility**: Add new functionality through plugin categories
- **Security**: Sandboxed plugin execution with security policies
- **Performance**: Optimized plugin loading and execution
- **Community**: Plugin marketplace for sharing and discovery
- **Backward Compatibility**: Support for legacy plugins

## Installation

### Prerequisites

- Python 3.8 or higher
- NLM Python interface

### Install NLM Python

```bash
pip install nlm-python
```

### Clone NLM Plugin System

The plugin system is included in this environment. For a fresh clone, use:

```bash
git clone <repository-url>
cd <repository-directory>
```

## Quickstart: Creating Your First Plugin

### Step 1: Create a Simple Plugin

Create a new Python file for your plugin:

```python
# File: my_first_plugin.py

from pynlm.plugin_system import Plugin, PluginInfo, PluginType

class SimpleUtilityPlugin(Plugin):
    """A simple utility plugin for demonstration"""
    
    def __init__(self, plugin_info: PluginInfo):
        super().__init__(plugin_info)
        self.info.plugin_type = PluginType.UTILITY
        self.processed_items = []
        
    def _initialize_plugin(self):
        """Initialize plugin with default data"""
        self.processed_items = []
        self.config.setdefault('multiplier', 1.0)
        
    def _enable_plugin(self):
        """Enable plugin functionality"""
        print(f"  ✓ Plugin '{self.info.name}' enabled with multiplier: {self.config.get('multiplier')}")
        
    def process_data(self, input_data):
        """Process input data and return result"""
        if not self.initialized:
            raise Exception("Plugin not initialized")
            
        # Process data
        result = input_data * self.config.get('multiplier', 1.0)
        self.processed_items.append(input_data)
        
        return result
        
    def get_stats(self):
        """Return plugin statistics"""
        return {
            'name': self.info.name,
            'processed_count': len(self.processed_items),
            'multiplier': self.config.get('multiplier'),
            'status': 'running' if self.enabled else 'stopped'
        }
```

### Step 2: Register Your Plugin

Create a registration script:

```python
# File: register_plugin.py

from pynlm.plugin_system import initialize_plugin_system, PluginInfo

# Initialize plugin system (mock interface for demonstration)
interface = type('obj', (object,), {
    'context': type('obj', (object,), {
        'brain': type('obj', (object,), {
            'get_sensory_input_size': lambda: 256,
            'get_motor_output_size': lambda: 6,
            'setNeuromodulationLevel': lambda x: None
        })()
    })()
})()

# Initialize enhanced plugin manager
plugin_manager = initialize_plugin_system(interface, {
    'auto_load_plugins': True,
    'enable_hot_reloading': True,
    'security_level': 'medium'
})

# Create plugin info
plugin_info = PluginInfo(
    name='simple_utility',
    version='1.0.0',
    description='A simple utility plugin for demonstration',
    author='Your Name',
    plugin_type=PluginType.UTILITY,
    capabilities=['data_processing', 'multiplier_operation']
)

# Create and load plugin
from my_first_plugin import SimpleUtilityPlugin
plugin = SimpleUtilityPlugin(plugin_info)
loaded_plugin = plugin_manager.load_plugin('simple_utility')

print("✓ Plugin registered and loaded successfully")
```

### Step 3: Use Your Plugin

Create a usage script:

```python
# File: use_plugin.py

from register_plugin import loaded_plugin

print("Using Simple Utility Plugin")
print("=" * 40)

# Test basic functionality
print("\n1. Testing basic processing:")
result = loaded_plugin.process_data(5)
print(f"   Input: 5, Output: {result}")

print("\n2. Testing with custom multiplier:")
loaded_plugin.set_config('multiplier', 2.5)
result = loaded_plugin.process_data(10)
print(f"   Input: 10, Output: {result}")

print("\n3. Getting plugin statistics:")
stats = loaded_plugin.get_stats()
for key, value in stats.items():
    print(f"   {key}: {value}")

print("\n4. Processing multiple items:")
for i in range(1, 4):
    result = loaded_plugin.process_data(i * 10)
    print(f"   Input: {i*10}, Output: {result}")

print("\n✓ Plugin usage completed successfully")
```

### Step 4: Run the Demo

```bash
python use_plugin.py
```

Expected output:
```
Using Simple Utility Plugin
========================================

1. Testing basic processing:
   Input: 5, Output: 5.0

2. Testing with custom multiplier:
   Input: 10, Output: 25.0

3. Getting plugin statistics:
   name: simple_utility
   processed_count: 2
   multiplier: 2.5
   status: running

4. Processing multiple items:
   Input: 10, Output: 25.0
   Input: 20, Output: 50.0
   Input: 30, Output: 75.0

✓ Plugin usage completed successfully
```

## Next Steps

### Learn About Plugin Categories

The plugin system supports 10 plugin categories. Each category provides specialized functionality:

1. **Brain Plugins**: Custom neural network topologies and dynamics
2. **Plasticity Plugins**: Custom learning rules and synaptic modification
3. **Neuromodulation Plugins**: Custom neuromodulatory signals and effects
4. **Memory Plugins**: Custom memory systems and storage mechanisms
5. **Sensory Plugins**: Custom sensory transduction and input processing
6. **Motor Plugins**: Custom motor output and action generation
7. **Development Plugins**: Custom developmental stages and growth patterns
8. **Integration Plugins**: Custom interfaces to external systems
9. **Analysis Plugins**: Custom analysis and visualization tools
10. **Utility Plugins**: General-purpose utility functions

### Create Advanced Plugins

Check out the examples directory for advanced plugin implementations:

```bash
ls examples/
```

Each example demonstrates a different plugin category and feature set.

### Use Plugin Development Kit

The plugin development kit provides tools for efficient plugin development:

1. **Plugin Templates**: Automatic template generation
2. **Plugin Testing**: Comprehensive testing framework
3. **Plugin Building**: Plugin packaging for distribution
4. **Plugin Documentation**: Automatic documentation generation

### Join the Community

- **Plugin Marketplace**: Share and discover plugins
- **Community Forums**: Discuss plugin development
- **GitHub Repository**: Contribute to plugin system
- **Documentation**: Learn best practices

## Advanced Features

### Hot-plugging

Load and unload plugins without restarting the system:

```python
# Hot reload plugin
plugin_manager.hot_reload_plugin('simple_utility')
print("✓ Plugin hot reloaded")
```

### State Management

Save and restore plugin state:

```python
# Save plugin state
plugin_manager.save_plugin_state('simple_utility')

# Load plugin state
state = plugin_manager.load_plugin_state('simple_utility')
```

### Event System

Subscribe to plugin events:

```python
# Define event handler
def on_plugin_change(event):
    print(f"Plugin event: {event.event_type}")

# Subscribe to events
plugin_manager.subscribe('PLUGIN_LOADED', on_plugin_change)

# Emit event
plugin_manager.emit('PLUGIN_LOADED', {'plugin_name': 'simple_utility'})
```

### Performance Monitoring

Monitor plugin performance:

```python
# Get plugin performance
performance = plugin_manager.get_plugin_performance('simple_utility')
print(f"Plugin performance: {performance}")
```

## Security

The plugin system includes security features:

```python
# Configure security policy
security_config = {
    'security_level': 'medium',
    'allowed_operations': ['import', 'open', 'read'],
    'denied_operations': ['socket', 'subprocess', 'os.system'],
    'resource_limits': {
        'memory_mb': 512,
        'cpu_cores': 1.0
    }
}
```

## Multi-Language Support

Create plugins in different languages:

```python
# Create C++ plugin
from pynlm.plugin_marketplace import CPlusPlusPluginLoader

loader = CPlusPlusPluginLoader()
plugin = loader.load('path/to/plugin.so')
```

```csharp
// Create C# plugin
using System;

public class CustomPlugin : IPlugin
{
    public void Initialize(IPluginContext context, Dictionary<string, object> config)
    {
        // Custom C# implementation
    }
}
```

## Plugin Marketplace

Register your plugin in the marketplace:

```python
from pynlm.plugin_marketplace import PluginMarketplace

marketplace = PluginMarketplace()
plugin_id = marketplace.register_plugin({
    'name': 'simple_utility',
    'version': '1.0.0',
    'description': 'A simple utility plugin',
    'author': 'Your Name',
    'plugin_type': 'utility'
})
```

## Troubleshooting

### Common Issues

1. **Plugin Loading Failures**:
   - Check plugin file syntax
   - Verify plugin dependencies
   - Check plugin security permissions

2. **Plugin Performance Issues**:
   - Monitor plugin resource usage
   - Optimize plugin code
   - Use plugin caching

3. **Plugin Security Issues**:
   - Review plugin security policies
   - Validate plugin signatures
   - Monitor plugin behavior

4. **Plugin Compatibility Issues**:
   - Check NLM version compatibility
   - Verify plugin interface compliance
   - Test plugin in different environments

### Getting Help

- **Debug Mode**: Enable debug logging
- **Error Messages**: Check error messages for details
- **Plugin Status**: Check plugin status and logs
- **Community**: Ask questions in community forums
- **Support**: Contact NLM support for professional help

## Resources

- **Documentation**: `PLUGIN_ARCHITECTURE.md`
- **Getting Started**: `GETTING_STARTED.md`
- **Examples**: `examples/` directory
- **API Reference**: `python/pynlm/` directory
- **Community**: NLM Discord community
- **Support**: NLM support forums

## Conclusion

The NLM plugin system provides a comprehensive framework for creating custom neural learning systems. By following this quickstart guide, you can:

- Create a basic utility plugin
- Understand plugin system architecture
- Learn about plugin categories
- Use advanced plugin features
- Troubleshoot common issues
- Get started with plugin development

Start creating your custom plugins today and contribute to the NLM ecosystem!
