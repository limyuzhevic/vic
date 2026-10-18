"""
Custom Configuration - Tailored Brain Setup
===========================================

This example demonstrates how to create custom brain configurations
with specific parameters for different use cases.

Key Concepts Demonstrated:
- Configuration creation with custom parameters
- Parameter validation
- Different brain sizes and connectivity
- Configuration comparison

Expected Output:
---------------
Creating custom configurations...

Configuration 1: Default settings
Neuron count: 500
Connection probability: 0.150

Configuration 2: Custom settings (Seed 12345)
Neuron count: 1000
Connection probability: 0.200

Brain 1 (Default): 500 neurons, 125 expected synapses
Brain 2 (Custom): 1000 neurons, 200 expected synapses

Configuration management complete!
"""

import pynlm

def main():
    print("Creating custom configurations...")
    
    # Configuration 1: Use default settings
    print("\nConfiguration 1: Default settings")
    config1 = pynlm.createDefaultConfig()  # This uses Phase 2 defaults
    
    neuron_count1 = config1.getOr("neuron_count", 500)
    conn_prob1 = config1.getOr("connection_probability", 0.15)
    
    print(f"Neuron count: {neuron_count1}")
    print(f"Connection probability: {conn_prob1}")
    
    # Create brain with default config
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    expected_synapses1 = neuron_count1 * neuron_count1 * conn_prob1 / 2  # Approximate
    
    print(f"Brain 1 (Default): {neuron_count1} neurons, ~{expected_synapses1:.0f} expected synapses")
    
    # Configuration 2: Create custom settings
    print("\nConfiguration 2: Custom settings (Seed 12345)")
    config2 = pynlm.create_config_with_seed(12345)
    
    # Customize parameters
    config2.set("neuron_count", 1000, pynlm.ConfigSource::User)
    config2.set("connection_probability", 0.2, pynlm.ConfigSource::User)
    config2.set("region_count", 2, pynlm.ConfigSource::User)
    
    neuron_count2 = config2.getOr("neuron_count", 1000)
    conn_prob2 = config2.getOr("connection_probability", 0.2)
    region_count2 = config2.getOr("region_count", 2)
    
    print(f"Neuron count: {neuron_count2}")
    print(f"Connection probability: {conn_prob2}")
    print(f"Region count: {region_count2}")
    
    # Create brain with custom config
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    expected_synapses2 = neuron_count2 * neuron_count2 * conn_prob2 / 2  # Approximate
    print(f"Brain 2 (Custom): {neuron_count2} neurons, ~{expected_synapses2:.0f} expected synapses")
    
    # Configuration 3: Simple config for testing
    print("\nConfiguration 3: Simple config for testing")
    config3 = pynlm.create_config()
    config3.set("random_seed", 999, pynlm.ConfigSource::User)
    
    brain3 = pynlm.createBrain(config3)
    brain3.initialize()
    
    print(f"Brain 3 (Simple): {brain3.getTotalNeuronCount()} neurons")
    
    # Show config summaries
    print("\nConfiguration summaries:")
    print(f"Config 1: {pynlm.get_config_summary(config1)}")
    print(f"Config 2: {pynlm.get_config_summary(config2)}")
    
    print("\nConfiguration management complete!")

if __name__ == "__main__":
    main()