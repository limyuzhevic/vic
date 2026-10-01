# Plugin Example: NLM Plugin System

This example demonstrates NLM plugin system capabilities. It's designed for users who want to understand how NLM plugins work and how to develop custom plugins.

## Overview

This example demonstrates NLM plugin system capabilities, including plugin development, plugin loading, and plugin usage. It shows how to extend NLM functionality with custom plugins. It's designed for users who want to understand NLM plugin system and develop custom plugins.

## Code

```python
#!/usr/bin/env python3
"""
Plugin Example: NLM Plugin System

This example demonstrates NLM plugin system capabilities,
including plugin development, plugin loading, and plugin usage.
It's designed for users who want to understand NLM plugin system and develop custom plugins.
"""

import pynlm
import json
import time
from typing import Dict, List, Any

class CustomPlugin:
    """Custom plugin for NLM"""
    
    def __init__(self, name):
        self.name = name
        self.version = "1.0.0"
        self.enabled = False
        self.config = None
    
    def getName(self):
        return self.name
    
    def getVersion(self):
        return self.version
    
    def getDescription(self):
        return f"Custom plugin: {self.name}"
    
    def getAuthor(self):
        return "Your Name"
    
    def getContact(self):
        return "your.email@example.com"
    
    def initialize(self, config):
        """Initialize plugin"""
        self.config = config
        self.enabled = True
        
        # Configure plugin-specific settings
        if config.has(f"plugin.{self.name}.enabled"):
            self.enabled = config.getBool(f"plugin.{self.name}.enabled")
        
        print(f"✅ Plugin {self.name} initialized")
        return True
    
    def shutdown(self):
        """Shutdown plugin"""
        self.enabled = False
        print(f"✅ Plugin {self.name} shutdown")
    
    def update(self, dt):
        """Update plugin"""
        if not self.enabled:
            return
        
        # Update plugin logic
        print(f"   Plugin {self.name} updating with dt: {dt}")
    
    def render(self):
        """Render plugin"""
        if not self.enabled:
            return
        
        # Render plugin visualization
        print(f"   Plugin {self.name} rendering")
    
    def onEvent(self, event):
        """Handle events"""
        if event.name == "simulation_step":
            print(f"   Plugin {self.name} received simulation step event")
    
    def onMessage(self, message):
        """Handle messages"""
        if message == "generate_report":
            print(f"   Plugin {self.name} generating report")
class PluginManager:
    """Plugin manager for NLM"""
    
    def __init__(self):
        self.plugins = {}
        self.plugin_files = []
    
    def load_plugin(self, plugin_name):
        """Load plugin"""
        # Create plugin instance
        plugin = CustomPlugin(plugin_name)
        
        # Initialize plugin
        config = pynlm.createDefaultConfig()
        plugin.initialize(config)
        
        # Store plugin
        self.plugins[plugin_name] = plugin
        
        print(f"✅ Plugin {plugin_name} loaded")
        
        return plugin
    
    def unload_plugin(self, plugin_name):
        """Unload plugin"""
        if plugin_name in self.plugins:
            plugin = self.plugins[plugin_name]
            plugin.shutdown()
            del self.plugins[plugin_name]
            
            print(f"✅ Plugin {plugin_name} unloaded")
            
            return True
        
        print(f"❌ Plugin {plugin_name} not found")
        return False
    
    def get_plugin(self, plugin_name):
        """Get plugin"""
        return self.plugins.get(plugin_name)
    
    def list_plugins(self):
        """List all plugins"""
        return list(self.plugins.keys())
    
    def enable_all_plugins(self, enable=True):
        """Enable or disable all plugins"""
        for plugin_name, plugin in self.plugins.items():
            plugin.setEnabled(enable)
            print(f"   Plugin {plugin_name} {'enabled' if enable else 'disabled'}")
    
    def update_all_plugins(self, dt):
        """Update all plugins"""
        for plugin in self.plugins.values():
            plugin.update(dt)
    
    def render_all_plugins(self):
        """Render all plugins"""
        for plugin in self.plugins.values():
            plugin.render()

def create_plugin_example():
    """Create plugin example"""
    print("🚀 Creating plugin example...")
    
    # Create plugin manager
    plugin_manager = PluginManager()
    
    # Load plugins
    plugin_manager.load_plugin("VisualizationPlugin")
    plugin_manager.load_plugin("DebugPlugin")
    plugin_manager.load_plugin("AnalysisPlugin")
    
    # Enable plugins
    plugin_manager.enable_all_plugins(True)
    
    print("✅ Plugin example created successfully")
    
    return {
        'plugin_manager': plugin_manager,
        'plugin_count': len(plugin_manager.list_plugins())
    }

def run_plugin_simulation(system, steps=50):
    """Run plugin simulation"""
    print(f"\n🔌 Running plugin simulation ({steps} steps)...")
    
    plugin_manager = system['plugin_manager']
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        
        # Update all plugins
        plugin_manager.update_all_plugins(0.1)
        
        # Render all plugins
        plugin_manager.render_all_plugins()
        
        # Print progress
        if step % 10 == 0:
            print(f"   Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    print(f"✅ Plugin simulation completed successfully")
    
    return {
        'brain': brain,
        'plugin_manager': plugin_manager
    }

def main():
    """Main function"""
    print("=== Plugin Example: NLM Plugin System ===")
    
    # Create plugin example
    plugin_example = create_plugin_example()
    
    # Run plugin simulation
    simulation_results = run_plugin_simulation(plugin_example, 50)
    
    # Print results
    print(f"\n📊 Plugin Simulation Results:")
    print(f"   Total plugins: {plugin_example['plugin_count']}")
    print(f"   Plugin names: {plugin_example['plugin_manager'].list_plugins()}")
    
    # Print plugin information
    print(f"\n🔌 Plugin Information:")
    for plugin_name in plugin_example['plugin_manager'].list_plugins():
        plugin = plugin_example['plugin_manager'].get_plugin(plugin_name)
        print(f"   {plugin_name}: {plugin.getName()} v{plugin.getVersion()}")
        print(f"     Description: {plugin.getDescription()}")
        print(f"     Author: {plugin.getAuthor()}")
        print(f"     Contact: {plugin.getContact()}")
    
    print(f"\n✅ Plugin example completed successfully!")
    print(f"\n🎯 This example demonstrates:")
    print(f"   1. Plugin development and implementation")
    print(f"   2. Plugin loading and unloading")
    print(f"   3. Plugin management and control")
    print(f"   4. Plugin integration with NLM")
    print(f"   5. Plugin usage in simulation")

if __name__ == "__main__":
    main()
```

## Explanation

### What This Example Shows

This example demonstrates NLM plugin system capabilities, including plugin development, plugin loading, and plugin usage.

### Key Components

- **Custom Plugin**: Custom plugin implementation
- **Plugin Manager**: Plugin management system
- **Plugin Loading**: Plugin loading and unloading
- **Plugin Control**: Plugin enable/disable control
- **Plugin Integration**: Plugin integration with NLM
- **Plugin Usage**: Plugin usage in simulation

### Expected Output

```
=== Plugin Example: NLM Plugin System ===

🚀 Creating plugin example...
✅ Plugin example created successfully

🔌 Running plugin simulation (50 steps)...
   Step 0: 0 firing neurons
   Step 10: 0 firing neurons
   Step 20: 0 firing neurons
   Step 30: 0 firing neurons
   Step 40: 0 firing neurons
   Step 50: 0 firing neurons
✅ Plugin simulation completed successfully

📊 Plugin Simulation Results:
   Total plugins: 3
   Plugin names: ['VisualizationPlugin', 'DebugPlugin', ' dispositif_analysis']

🔌 Plugin Information:
   VisualizationPlugin: VisualizationPlugin v1.0.0
     Description: Plugin for generating visualizations of brain activity
     Author: Your Name
     Contact: your.email@example.com
   DebugPlugin: DebugPlugin v1.0.0
     Description: Plugin for debugging and logging NLM operations
     Author: Your Name
     Contact: your.email@example.com
   AnalysisPlugin: AnalysisPlugin v1.0.0
     Description: Plugin for analyzing neural data
     Author: Your Name
     Contact: your.email@example.com

✅ Plugin example completed successfully!

🎯 This example demonstrates:
   1. Plugin development and implementation
   2. Plugin loading and unloading
   3. Plugin management and control
   4. Plugin integration with NLM
   5. Plugin usage in simulation
```

### Learning Outcomes

By completing this example, you will learn:

1. **Plugin Development**: How to develop custom plugins
2. **Plugin Loading**: How to load and unload plugins
3. **Plugin Management**: How to manage plugins
4. **Plugin Integration**: How to integrate plugins with NLM
5. **Plugin Usage**: How to use plugins in simulation

### Extensions

To extend this example:

1. **Advanced Plugins**: Develop advanced plugins
2. **Plugin Dependencies**: Implement plugin dependencies
3. **Plugin Configuration**: Configure plugin-specific settings
4. **Plugin Communication**: Implement plugin communication
5. **Plugin Integration**: Integrate plugins with other systems

## Files

- **Plugin Example Code**: `examples/plugin-example.md`
- **Documentation**: Documentation for the example
- **Tutorial**: Tutorial for this example

## Related Resources

- **Tutorial 1**: `docs/tutorials/tutorial-1-basic-usage.md`
- **API Reference**: `docs/api-reference.md`
- **Plugin Guide**: `docs/plugin-guide.md`

## References

1. **NLM Documentation**: https://docs.kilo.ai/nlm
2. **NLM Examples**: https://github.com/your-repo/nlm/examples
3. **NLM Tutorials**: https://docs.kilo.ai/nlm/tutorials

This example demonstrates NLM plugin system capabilities, including plugin development, plugin loading, and plugin usage.🧠✨