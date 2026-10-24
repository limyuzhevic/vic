#!/usr/bin/env python3
"""
Example 1: Basic Usage with New Convenience Methods
Demonstrates the new Config and Brain convenience methods
"""
import pynlm

def basic_convenience_methods():
    """Demonstrate the new convenience methods for Config and Brain"""
    print("=== Example 1: Basic Convenience Methods ===\n")
    
    # 1. Create configuration and use convenience methods
    print("1. Creating configuration and using convenience methods:")
    config = pynlm.createDefaultConfig()
    
    # Use new Config convenience methods
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    config.set("neuromod.dopamine.scale", 1.5)
    
    # Get configuration values using new convenience methods with error handling
    try:
        neuron_count = config.getInt("brain.neuron_count")
        print(f"   Neuron count (getInt): {neuron_count}")
        
        synapse_density = config.getFloat("brain.synapse_density", 0.05)
        print(f"   Synapse density (getFloat with default): {synapse_density}")
        
        dopamine_scale = config.getFloat("neuromod.dopamine.scale", 1.0)
        print(f"   Dopamine scale (getFloat with default): {dopamine_scale}")
        
        # Test with default values for non-existent keys
        unknown_param = config.getBool("nonexistent.key", False)
        print(f"   Non-existent bool key (with default): {unknown_param}")
        
        unknown_str = config.getString("nonexistent.key", "default_value")
        print(f"   Non-existent string key (with default): {unknown_str}")
        
    except Exception as e:
        print(f"   Error: {e}")
    
    print()
    
    # 2. Create brain and demonstrate new convenience methods
    print("2. Creating brain and using convenience methods:")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Demonstrate new Brain convenience methods
    try:
        # Get neuron count by type (new method)
        excitatory_neurons = brain.getNeuronCountByType(pynlm.NeuronType.Excitatory)
        inhibitory_neurons = brain.getNeuronCountByType(pynlm.NeuronType.Inhibitory)
        sensory_neurons = brain.getNeuronCountByType(pynlm.NeuronType.Sensory)
        motor_neurons = brain.getNeuronCountByType(pynlm.NeuronType.Motor)
        
        print(f"   Excitatory neurons: {excitatory_neurons}")
        print(f"   Inhibitory neurons: {inhibitory_neurons}")
        print(f"   Sensory neurons: {sensory_neurons}")
        print(f"   Motor neurons: {motor_neurons}")
        
        # Get comprehensive brain stats (new method)
        brain_stats = brain.getBrainStats()
        print(f"\n   Brain statistics:")
        for key, value in brain_stats.items():
            print(f"     {key}: {value}")
            
    except Exception as e:
        print(f"   Error: {e}")
    
    print()
    
    # 3. Run a few simulation steps
    print("3. Running simulation with statistics:")
    for step in range(3):
        brain.step(step)
        
        # Print stats using new convenience methods
        total_neurons = brain.getTotalNeuronCount()
        firing_neurons = brain.getFiringNeuronCount()
        firing_rate = brain.getAverageFiringRate()
        
        print(f"   Step {step}: {firing_neurons}/{total_neurons} neurons firing, rate={firing_rate:.3f}")
    
    print("\n=== Basic convenience methods example complete ===\n")

if __name__ == "__main__":
    basic_convenience_methods()