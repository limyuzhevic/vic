"""
NLM Python Bindings - Basic Tutorial for Beginners
================================================

This tutorial introduces you to the NLM (Neural Learning Machine) Python bindings.
It covers basic concepts and provides simple examples to get you started.
"""

import pynlm

def basic_tutorial():
    """Run the basic tutorial examples."""
    
    print("=== NLM Python Bindings - Basic Tutorial ===\n")
    
    # Example 1: Creating a simple brain with default configuration
    print("1. Creating a brain with default configuration:")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"   and {brain.getTotalSynapseCount()} synapses\n")
    
    # Example 2: Running a simple simulation
    print("2. Running a simple simulation:")
    print("   Simulating 100 time steps...")
    for step in range(100):
        brain.step(step)
        if step % 20 == 0:
            print(f"   Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    print("   Simulation complete!\n")
    
    # Example 3: Using the improved ConfigBuilder
    print("3. Using the improved ConfigBuilder for custom configuration:")
    config = (pynlm.ConfigBuilder()
              .setNeuronCount(2000)
              .setRegionCount(2)
              .setConnectionProbability(0.15)
              .setSTDPParameters(0.01, 0.012, 20.0)
              .setMemoryCapacity(1000, 500)
              .build())
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    print(f"   Created custom brain with {brain2.getTotalNeuronCount()} neurons")
    print(f"   and {brain2.getTotalSynapseCount()} synapses\n")
    
    # Example 4: Getting statistics
    print("4. Getting brain statistics:")
    stats = brain2.getStatistics()
    print(f"   Total neurons: {stats['neuron_count']}")
    print(f"   Total synapses: {stats['synapse_count']}")
    print(f"   Active neurons: {stats['active_neurons']}")
    print(f"   Firing neurons: {stats['firing_neurons']}")
    print(f"   Total spikes: {stats['total_spikes']}")
    print(f"   Average firing rate: {stats['average_firing_rate']:.2f} Hz")
    print(f"   E/I ratio: {stats['e_i_ratio']:.2f}\n")
    
    print("=== Tutorial Complete ===")
    return brain2

if __name__ == "__main__":
    brain = basic_tutorial()