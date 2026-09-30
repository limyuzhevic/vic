# NLM Plugin System - Getting Started Guide

This guide provides an introduction to the NLM plugin architecture and helps you get started with creating custom plugins.

## Quick Start

### 1. Understanding the Plugin System

The NLM plugin system allows you to create custom neural learning components without modifying the core NLM code. The system is organized into:

- **Core Plugin Framework**: Plugin manager, interfaces, security sandboxing
- **Plugin Categories**: Brain, Plasticity, Neuromodulation, Memory, Sensory, Motor, Development, Integration, Analysis, Utility
- **Plugin Development Kit**: SDK, templates, testing framework, documentation generator
- **Plugin Runtime Features**: Hot-plugging, state management, event systems
- **Plugin Marketplace**: Repository, rating system, certification, distribution

### 2. Installation

The NLM plugin system is built on top of the existing NLM Python interface. Ensure you have:

```bash
pip install nlm-python
```

### 3. Basic Plugin Creation

Let's create a simple plugin to get started:

```python
# Step 1: Create a new Python file for your plugin
# File: my_custom_plugin.py

from pynlm.plugin_system import Plugin, PluginInfo, PluginType

class CustomUtilityPlugin(Plugin):
    """A custom utility plugin that provides additional functionality"""
    
    def __init__(self, plugin_info: PluginInfo):
        super().__init__(plugin_info)
        self.info.plugin_type = PluginType.UTILITY
        self.custom_data = {}
        
    def _initialize_plugin(self):
        """Initialize plugin with default data"""
        self.custom_data = {
            'initialized': True,
            'version': '1.0.0',
            'features': ['custom_processing', 'data_analysis']
        }
        
    def _enable_plugin(self):
        """Enable plugin functionality"""
        pass
        
    def process_data(self, input_data):
        """Process input data using custom logic"""
        if not self.initialized:
            raise Exception("Plugin not initialized")
            
        # Custom processing logic
        result = input_data * 2  # Example: double the input
        self.custom_data['last_processed'] = input_data
        
        return result
        
    def get_plugin_info(self):
        """Return plugin information"""
        return self.custom_data
```

### 4. Plugin Registration

To use your plugin with the NLM system:

```python
# Step 2: Register your plugin
from pynlm.plugin_system import PluginManager

# Create plugin manager
plugin_manager = PluginManager(interface)

# Create plugin info
plugin_info = PluginInfo(
    name='custom_utility',
    version='1.0.0',
    description='A custom utility plugin for NLM',
    author='Your Name',
    plugin_type=PluginType.UTILITY,
    capabilities=['data_processing', 'analysis']
)

# Create and load plugin
plugin = CustomUtilityPlugin(plugin_info)
loaded_plugin = plugin_manager.load_plugin('custom_utility')
```

### 5. Using Your Plugin

```python
# Step 3: Use your plugin
# Create some input data
input_data = 42

# Process data using your plugin
result = loaded_plugin.process_data(input_data)

print(f"Input: {input_data}")
print(f"Output: {result}")
print(f"Plugin info: {loaded_plugin.get_plugin_info()}")
```

### 6. Advanced Plugin Categories

The plugin system supports 10 different plugin categories. Here are examples of each:

#### Brain Plugin Example
```python
from pynlm.plugin_categories import BrainPlugin

class CustomBrainPlugin(BrainPlugin):
    def customize_neural_dynamics(self, brain, config):
        # Add custom neuron types
        self.register_neuron_type(CustomNeuron())
        
    def add_custom_plasticity(self, plasticity_rule):
        # Add custom plasticity rules
        pass
```

#### Plasticity Plugin Example
```python
from pynlm.plugin_categories import PlasticityPlugin

class CustomLearningRulePlugin(PlasticityPlugin):
    def register_learning_rule(self, rule_name: str, rule_func):
        self.register_learning_rule(rule_name, rule_func)
        
    def calculate_weight_change(self, pre_neuron, post_neuron, activity_diff, time):
        # Use custom learning rule
        if self.learning_rule:
            return self.learning_rule['rule'](pre_neuron, post_neuron, activity_diff, time)
        return activity_diff * 0.1
```

#### Sensory Plugin Example
```python
from pynlm.plugin_categories import SensoryPlugin

class CustomSensoryPlugin(SensoryPlugin):
    def add_sensory_processor(self, name: str, processor_func):
        self.add_sensory_processor(name, processor_func)
        
    def process_sensory_input(self, input_data):
        # Process through custom channels
        return super().process_sensory_input(input_data)
```

### 7. Plugin Development Kit

The plugin system includes a comprehensive development kit:

#### Creating Plugin Templates
```python
from pynlm.plugin_development_kit import PluginSDK

# Create plugin SDK
sdk = PluginSDK(plugin_manager)

# Generate plugin template
template_dir = sdk.create_plugin_template(
    'brain_extension',
    '/path/to/templates',
    {'plugin_name': 'my_brain_plugin', 'version': '1.0.0'}
)
```

#### Testing Plugins
```python
# Run plugin tests
results = sdk.test_plugin('/path/to/plugin', {
    'test_mode': 'unit',
    'timeout': 30,
    'coverage': True
})

print(f"Test results: {results}")
```

#### Building Plugins
```python
# Build plugin for distribution
package_path = sdk.build_plugin('/path/to/plugin', 'zip')
print(f"Plugin package created: {package_path}")
```

### 8. Plugin Marketplace

Register your plugin in the marketplace:

```python
from pynlm.plugin_marketplace import PluginMarketplace

# Create marketplace
marketplace = PluginMarketplace()

# Register plugin
plugin_id = marketplace.register_plugin({
    'name': 'custom_utility',
    'version': '1.0.0',
    'description': 'A custom utility plugin for NLM',
    'author': 'Your Name',
    'plugin_type': 'utility',
    'files': [
        {'name': 'plugin.py', 'type': 'text', 'content': open('my_custom_plugin.py').read()},
        {'name': 'README.md', 'type': 'text', 'content': '# Custom Utility Plugin\n\nThis plugin provides custom utility functions for NLM.'}
    ]
})

print(f"Plugin registered with ID: {plugin_id}")

# Search for plugins
results = marketplace.search_plugins('utility', {
    'categories': ['general'],
    'min_rating': 3.0
})

print(f"Found {len(results['plugins'])} plugins")
```

### 9. Advanced Features

#### Hot-plugging
```python
# Hot reload a plugin
plugin_manager.hot_reload_plugin('custom_utility')
print("Plugin hot reloaded successfully")
```

#### State Management
```python
# Save plugin state
plugin_manager.save_plugin_state('custom_utility')

# Load plugin state
state = plugin_manager.load_plugin_state('custom_utility')
```

#### Event System
```python
# Subscribe to events
def on_simulation_start(event):
    print("Simulation started")

plugin_manager.subscribe(EventType.SIMULATION_STARTED, on_simulation_start)

# Emit event
plugin_manager.emit(EventType.SIMULATION_STARTED, {'timestamp': time.time()})
```

#### Performance Monitoring
```python
# Get plugin performance
performance = plugin_manager.get_plugin_performance('custom_utility')
print(f"Plugin performance: {performance}")
```

### 10. Multi-Language Support

Create plugins in different languages:

#### C++ Plugin
```cpp
// C++ plugin implementation
class CustomBrainExtension : public BrainExtension {
public:
    void customizeNeuralDynamics(Brain* brain, Config* config) override {
        // Custom C++ implementation
    }
};
```

#### C# Plugin
```csharp
// C# plugin implementation
public class CustomUtilityPlugin : IPlugin
{
    public void Initialize(IPluginContext context, Dictionary<string, object> config)
    {
        // Custom C# implementation
    }
    
    public void ProcessData(object inputData)
    {
        // Custom C# processing
    }
}
```

### 11. Security and Sandbox

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

# Create plugin with security policy
plugin = CustomUtilityPlugin(plugin_info, security_config)
```

### 12. Plugin Certification

Certify your plugin:

```python
from pynlm.plugin_marketplace import CertificationSystem

# Create certification system
certification = CertificationSystem(marketplace)

# Certify plugin
certification.certify_plugin(
    'custom_utility',
    'certifier_id',
    CertificationLevel.VERIFIED,
    ['security', 'performance', 'documentation']
)
```

## Next Steps

1. **Learn More**: Read the comprehensive plugin architecture documentation
2. **Explore Examples**: Check out the examples directory for more plugin implementations
3. **Join Community**: Join the NLM community to share and discover plugins
4. **Contribute**: Contribute your plugins to the marketplace
5. **Get Help**: Use the plugin system for troubleshooting

## Resources

- **Documentation**: `PLUGIN_ARCHITECTURE.md`
- **Plugin System**: `python/pynlm/` directory
- **Examples**: `examples/` directory
- **API Reference**: `python/pynlm/` directory
- **Community**: NLM Discord community
- **Support**: NLM support forums

## Troubleshooting

### Common Issues and Solutions

1. **Plugin Loading Failures**
   - Check plugin file syntax
   - Verify plugin dependencies
   - Check plugin security permissions

2. **Plugin Performance Issues**
   - Monitor plugin resource usage
   - Optimize plugin code
   - Use plugin caching

3. **Plugin Security Issues**
   - Review plugin security policies
   - Validate plugin signatures
   - Monitor plugin behavior

4. **Plugin Compatibility Issues**
   - Check NLM version compatibility
   - Verify plugin interface
   - Test plugin in different environments

### Getting Help

- **Debug Mode**: Enable debug logging
- **Error Messages**: Check error messages for details
- **Plugin Status**: Check plugin status and logs
- **Community**: Ask questions in community forums
- **Support**: Contact NLM support for professional help

## Conclusion

The NLM plugin architecture provides a comprehensive framework for creating custom neural learning systems. By following this guide, you can:

- Create plugins for any of the 10 plugin categories
- Use the development kit for efficient plugin development
- Leverage marketplace features for plugin distribution
- Utilize runtime features for advanced plugin management
- Ensure security and performance with built-in safeguards

Start creating your custom plugins today and contribute to the NLM ecosystem!
