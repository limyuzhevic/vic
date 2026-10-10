#!/usr/bin/env python3
"""
Example: Custom Brain Region Plugin for NLM Plugin System

This example demonstrates how to create and use a custom brain region plugin
using the NLM plugin system.
"""

import sys
import os
import json

# Add the plugin directory to the Python path
plugin_path = os.path.join(os.path.dirname(__file__), 'plugin')
sys.path.insert(0, plugin_path)

# Import NLM plugin classes
try:
    from plugin import PluginManager, PluginRegistry, PluginFactory
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Import error: {e}")
    print("Please ensure NLM is properly installed.")
    PYNLM_AVAILABLE = False

def main():
    print("=== NLM Plugin System Example: Custom Brain Region ===\n")
    
    if not PYNLM_AVAILABLE:
        print("Skipping example due to import errors.")
        return
    
    # Create plugin configuration
    plugin_config = {
        "region_type": "cortex",
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "specialization": "visual_processing",
        "plasticity_enabled": True,
        "activity_threshold": 0.5
    }
    
    print("1. Creating Plugin Manager...")
    plugin_manager = PluginManager()
    
    print("\n2. Creating Plugin Registry...")
    plugin_registry = PluginRegistry()
    
    print("\n3. Creating Custom Brain Region Plugin...")
    brain_region_plugin = PluginFactory.create_plugin(
        "CustomBrainRegion",
        plugin_config
    )
    
    if brain_region_plugin is None:
        print("Failed to create plugin. Is the plugin system properly installed?")
        return
    
    print(f"   Plugin Name: {brain_region_plugin.get_name()}")
    print(f"   Plugin Version: {brain_region_plugin.get_version()}")
    print(f"   Plugin Description: {brain_region_plugin.get_description()}")
    print(f"   Plugin Author: {brain_region_plugin.get_author()}")
    
    print("\n4. Initializing Plugin...")
    success = brain_region_plugin.initialize(plugin_config)
    
    if not success:
        print(f"   Failed to initialize plugin: {brain_region_plugin.get_error_message()}")
        return
    
    print("   Plugin initialized successfully.")
    
    print("\n5. Using Plugin Functionality...")
    
    # In a real implementation, you would use the plugin with the NLM brain
    # For this example, we'll demonstrate the plugin interface
    print("   Plugin Type:", brain_region_plugin.get_plugin_type())
    print("   Is Initialized:", brain_region_plugin.is_initialized())
    print("   Is Valid:", brain_region_plugin.validate())
    
    # Example: Create neurons and synapses
    # In a real implementation, you would pass actual config objects
    print("\n6. Plugin Statistics...")
    print("   Plugin count:", plugin_manager.get_plugin_count())
    print("   Brain region plugin count:", plugin_manager.get_plugin_count("brain_region"))
    
    print("\n7. Serializing Plugin...")
    serialized_data = brain_region_plugin.serialize()
    print(f"   Serialized data length: {len(serialized_data)} characters")
    
    print("\n8. Deserializing Plugin...")
    # Create a new plugin instance and deserialize
    brain_region_plugin2 = PluginFactory.create_plugin(
        "CustomBrainRegion",
        plugin_config
    )
    
    if brain_region_plugin2:
        success = brain_region_plugin2.deserialize(serialized_data)
        if success:
            print("   Plugin deserialized successfully.")
            print("   Plugin Name (deserialized):", brain_region_plugin2.get_name())
        else:
            print(f"   Failed to deserialize plugin: {brain_region_plugin2.get_error_message()}")
    
    print("\n9. Shutting down Plugin...")
    brain_region_plugin.shutdown()
    
    print("\n=== Example Complete ===\n")

if __name__ == "__main__":
    main()