#!/usr/bin/env python3
# Custom Configuration Example
# Demonstrates NLM configuration management

import sys
sys.path.insert(0, '../')

import pynlm as nlm
from pathlib import Path

def main():
    print("=== NLM Custom Configuration Example ===")
    print("This example demonstrates various NLM configuration management approaches.\n")
    
    # Method 1: Create configuration from scratch
    print("1. Method 1: Creating configuration from scratch...")
    config1 = nlm.createDefaultConfig()
    config1.set("neuron_count", 2000)
    config1.set("region_count", 3)
    config1.set("connection_probability", 0.2)
    config1.set("stdp_ltp_weight", 0.015)
    config1.set("stdp_ltd_weight", 0.018)
    
    print("   Custom configuration created:")
    print(f"     Neurons: {config1.getOr('neuron_count', 1000)}")
    print(f"     Regions: {config1.getOr('region_count', 1)}")
    print(f"     Connectivity: {config1.getOr('connection_probability', 0.15)}")
    print(f"     STDP LTP weight: {config1.getOr('stdp_ltp_weight', 0.02)}")
    print(f"     STDP LTD weight: {config1.getOr('stdp_ltd_weight', 0.015)}")
    
    # Method 2: Modify default configuration
    print("\n2. Method 2: Modifying default configuration...")
    config2 = nlm.createDefaultConfig()
    
    # Set multiple values
    config2.set("simulation.random_seed", 12345)
    config2.set("simulation.simulation_timestep", 0.0005)
    config2.set("neurons.threshold", -52.0)
    config2.set("neurons.resting_potential", -75.0)
    
    print("   Default configuration modified:")
    print(f"     Random seed: {config2.getOr('simulation.random_seed', 42)}")
    print(f"     Time step: {config2.getOr('simulation.simulation_timestep', 0.001)}")
    print(f"     Threshold: {config2.getOr('neurons.threshold', -55.0)}")
    print(f"     Resting potential: {config2.getOr('neurons.resting_potential', -70.0)}")
    
    # Method 3: Show configuration comparison
    print("\n3. Method 3: Configuration comparison...")
    config3 = nlm.createDefaultConfig()
    config3.set("neuron_count", 1500)
    config3.set("world.world_width", 25)
    config3.set("world.world_height", 25)
    
    print("   Comparison of configurations:")
    print(f"     Config1 - Neurons: {config1.getOr('neuron_count', 0)}")
    print(f"     Config2 - Neurons: {config2.getOr('neuron_count', 0)}")
    print(f"     Config3 - Neurons: {config3.getOr('neuron_count', 0)}")
    print(f"     Config1 - World size: {config1.getOr('world.world_width', 0)}x{config1.getOr('world.world_height', 0)}")
    print(f"     Config3 - World size: {config3.getOr('world.world_width', 0)}x{config3.getOr('world.world_height', 0)}")
    
    # Method 4: Show configuration validation
    print("\n4. Method 4: Configuration validation...")
    print("   Checking configuration values...")
    
    # Check neuron count
    neuron_count = config1.getOr("neuron_count", 1000)
    if neuron_count > 5000:
        print("   WARNING: Neuron count is very high, simulation may be slow!")
    elif neuron_count > 1000:
        print("   NOTE: Neuron count is high, consider using performance optimizations")
    else:
        print("   NOTE: Neuron count is reasonable for standard hardware")
    
    # Check world size
    world_width = config1.getOr("world.world_width", 20)
    world_height = config1.getOr("world.world_height", 20)
    print(f"   World dimensions: {world_width} x {world_height}")
    
    # Check simulation timestep
    timestep = config1.getOr("simulation.simulation_timestep", 0.001)
    if timestep < 0.0001:
        print("   NOTE: Very small timestep - high accuracy, slower performance")
    elif timestep < 0.001:
        print("   NOTE: Small timestep - good balance of accuracy and speed")
    else:
        print("   NOTE: Standard timestep - reasonable performance")
    
    # Method 5: Show configuration export
    print("\n5. Method 5: Configuration export...")
    print("   Configuration can be exported in various formats:")
    print("     - NLM format (.nlm): Easy to edit manually")
    print("     - JSON format (.json): Programmatically accessible")
    print("     - YAML format (.yaml): Human-readable")
    print("   Example export command:")
    print("     nlm_config_manager.py --save-config export_config.json --format json")
    
    # Method 6: Show configuration presets
    print("\n6. Method 6: Configuration presets...")
    print("   Presets provide convenient configuration combinations:")
    print("     - research_config: High neuron count for research")
    print("     - production_config: Optimized for production use")
    print("     - development_config: Small network for development")
    print("   To use presets:")
    print("     nlm_config_manager.py --load-preset research_config")
    
    # Show current configuration summary
    print("\n7. Current configuration summary...")
    print("   Configuration summary:")
    print(f"     Total sections: {len(config1.getKeys()) if hasattr(config1, 'getKeys') else 'N/A'}")
    print(f"     Neuron count: {config1.getOr('neuron_count', 1000)}")
    print(f"     Simulation timestep: {config1.getOr('simulation_timestep', 0.001)}")
    print(f"     World dimensions: {config1.getOr('world.world_width', 20)} x {config1.getOr('world.world_height', 20)}")
    
    print("\nCustom configuration example completed successfully!")
    
    return config1, config2, config3

if __name__ == "__main__":
    main()
