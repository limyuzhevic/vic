#!/usr/bin/env python3
# NLM Configuration Examples
# Pre-defined configuration presets for different use cases

import pynlm

def create_preset_config(preset_name):
    """Create a configuration preset"""
    config = pynlm.createDefaultConfig()
    
    if preset_name == "minimal":
        # Small network for quick testing
        config.set("neuron_count", 100)
        config.set("region_count", 1)
        config.set("connection_probability", 0.05)
        config.set("random_seed", 42)
        
    elif preset_name == "medium":
        # Medium-sized network for normal testing
        config.set("neuron_count", 1000)
        config.set("region_count", 2)
        config.set("connection_probability", 0.1)
        config.set("random_seed", 123)
        
    elif preset_name == "large":
        # Larger network for more realistic simulation
        config.set("neuron_count", 5000)
        config.set("region_count", 4)
        config.set("connection_probability", 0.15)
        config.set("random_seed", 456)
        
    elif preset_name == "research":
        # Research-oriented configuration with high plasticity
        config.set("neuron_count", 3000)
        config.set("region_count", 3)
        config.set("connection_probability", 0.2)
        config.set("random_seed", 789)
        config.set("stdp_ltp_weight", 0.03)
        config.set("stdp_ltd_weight", 0.02)
        config.set("stdp_tau", 25.0)
        config.set("synaptogenesis_rate", 0.0002)
        config.set("pruning_rate", 0.00002)
        
    else:
        raise ValueError(f"Unknown preset: {preset_name}")
    
    return config

def create_world_config(width, height, fov_size):
    """Create a world configuration"""
    world = pynlm.createSimpleWorld()
    world.configure(width, height, fov_size, fov_size)
    return world

def example_usage():
    """Demonstrate different configuration presets"""
    print("=== NLM Configuration Presets ===\n")
    
    presets = ["minimal", "medium", "large", "research"]
    
    for preset in presets:
        print(f"Preset: {preset.upper()}")
        config = create_preset_config(preset)
        print(f"  Neurons: {config.getOr('neuron_count', 0)}")
        print(f"  Regions: {config.getOr('region_count', 0)}")
        print(f"  Connection Probability: {config.getOr('connection_probability', 0.0)}")
        print(f"  Random Seed: {config.getOr('random_seed', 0)}")
        print()
    
    print("=== World Configuration Example ===")
    world = create_world_config(20, 20, 8)
    print(f"World size: {world.getWidth()}x{world.getHeight()}")
    print(f"Vision field of view: {world.getVisionWidth()}x{world.getVisionHeight()}")
    print(f"Max energy: {world.getMaxEnergy()}")
    print(f"Energy decay rate: {world.getEnergyDecayRate()}")
    
    return True

if __name__ == "__main__":
    try:
        example_usage()
        print("\nConfiguration examples completed successfully!")
    except Exception as e:
        print(f"\nError: {e}")
        exit(1)
