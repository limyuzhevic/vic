#!/usr/bin/env python3
"""
Plugin usage examples for NLM framework.

This script demonstrates how to use the NLM plugin system to:
1. Discover and load plugins
2. Use plugin instances
3. Configure plugins
4. Manage plugin lifecycles
"""

import json
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from nlm.plugin import PluginManager, PluginRegistryEntry

def main():
    print("NLM Plugin System Usage Examples")
    print("=" * 50)
    
    # Example 1: Basic plugin manager usage
    print("\n1. Basic Plugin Manager Usage:")
    print("-" * 30)
    
    # Create plugin manager
    config = {
        "pluginDirectory": "./plugins",
        "enableAutoLoad": True,
        "enableAutoStart": False,
        "maxConcurrentLoads": 4
    }
    
    manager = PluginManager(config)
    
    # Scan for plugins
    print("Scanning plugin directory...")
    if manager.scanPluginDirectory():
        print("Found plugins:", manager.getPluginNames())
    else:
        print("No plugins found or scan failed")
    
    # Example 2: Loading a specific plugin
    print("\n2. Loading a Specific Plugin:")
    print("-" * 30)
    
    # Check if plugin exists
    pluginName = "lif-neuron"
    if manager.pluginExists(pluginName):
        print(f"Plugin '{pluginName}' exists in registry")
        
        # Load the plugin
        if manager.loadPlugin(pluginName):
            print(f"Successfully loaded plugin '{pluginName}'")
            print(f"Status: {manager.getPluginStatus(pluginName)}")
        else:
            print(f"Failed to load plugin '{pluginName}'")
            print("Error:", manager.getLastError(pluginName))
    else:
        print(f"Plugin '{pluginName}' not found in registry")
    
    # Example 3: Starting and stopping plugins
    print("\n3. Starting and Stopping Plugins:")
    print("-" * 30)
    
    # Start a plugin
    if manager.startPlugin("lif-neuron"):
        print("Successfully started 'lif-neuron' plugin")
        print("Current status:", manager.getPluginStatus("lif-neuron"))
    else:
        print("Failed to start 'lif-neuron' plugin")
    
    # Stop the plugin
    if manager.stopPlugin("lif-neuron"):
        print("Successfully stopped 'lif-neuron' plugin")
        print("Current status:", manager.getPluginStatus("lif-neuron"))
    else:
        print("Failed to stop 'lif-neuron' plugin")
    
    # Example 4: Plugin configuration
    print("\n4. Plugin Configuration:")
    print("-" * 30)
    
    # Get default configuration for a plugin
    if manager.pluginExists("stdp-rule"):
        defaultConfig = manager.getPluginConfig("stdp-rule")
        print("Default configuration for 'stdp-rule':")
        print(json.dumps(defaultConfig, indent=2))
        
        # Update plugin configuration
        customConfig = {
            "ltpWeight": 0.02,
            "ltdWeight": 0.025,
            "timeConstant": 25.0
        }
        
        if manager.updatePluginConfig("stdp-rule", customConfig):
            print("Successfully updated configuration for 'stdp-rule'")
        else:
            print("Failed to update configuration")
    
    # Example 5: Plugin dependency management
    print("\n5. Plugin Dependency Management:")
    print("-" * 30)
    
    if manager.pluginExists("adaptive-exponential"):
        dependencies = manager.getPluginDependencies("adaptive-exponential")
        print(f"Plugin 'adaptive-exponential' depends on: {dependencies}")
        
        if manager.resolveDependencies("adaptive-exponential"):
            print("Dependencies successfully resolved")
        else:
            print("Failed to resolve dependencies")
    
    # Example 6: Plugin state management
    print("\n6. Plugin State Management:")
    print("-" * 30)
    
    if manager.pluginExists("dopamine-modulator"):
        # Get plugin info
        pluginInfo = manager.getPluginInfo("dopamine-modulator")
        print(f"Plugin info: {json.dumps(pluginInfo, indent=2)}")
        
        # Get all plugin info
        allInfo = manager.getAllPluginInfos()
        print(f"All plugins info ({len(allInfo)} plugins):")
        for info in allInfo:
            print(f"  - {info.get('name', 'Unknown')}: {info.get('status', 'Unknown')}")
    
    # Example 7: Plugin lifecycle callbacks
    print("\n7. Plugin Lifecycle Callbacks:")
    print("-" * 30)
    
    def onPluginLoaded(name):
        print(f"Callback: Plugin '{name}' was loaded")
    
    def onPluginStatusChanged(name, status):
        print(f"Callback: Plugin '{name}' status changed to {status}")
    
    # Set callbacks
    manager.setPluginLoadedCallback(onPluginLoaded)
    manager.setPluginStatusChangedCallback(onPluginStatusChanged)
    
    # Example 8: Plugin statistics
    print("\n8. Plugin Statistics:")
    print("-" * 30)
    
    stats = manager.getManagerStats()
    print(f"Plugin Manager Statistics:")
    print(json.dumps(stats, indent=2))
    
    # Example 9: Configuration-driven loading
    print("\n9. Configuration-Driven Loading:")
    print("-" * 30)
    
    # Load plugins from global configuration
    if manager.loadPluginsFromConfig():
        print("Successfully loaded plugins from global configuration")
        print("Loaded plugins:", manager.getLoadedPluginNames())
    else:
        print("Failed to load plugins from configuration")
    
    # Example 10: Plugin registry management
    print("\n10. Plugin Registry Management:")
    print("-" * 30)
    
    # Save plugins to registry
    registryFile = "plugin_registry.json"
    if manager.savePluginsToRegistry(registryFile):
        print(f"Successfully saved plugin registry to '{registryFile}'")
    else:
        print(f"Failed to save plugin registry")
    
    # Load plugins from registry
    if manager.loadPluginsFromRegistry(registryFile):
        print(f"Successfully loaded plugins from '{registryFile}'")
    else:
        print(f"Failed to load plugins from '{registryFile}'")
    
    print("\n" + "=" * 50)
    print("Plugin usage examples completed!")
    print("=" * 50)

if __name__ == "__main__":
    main()
