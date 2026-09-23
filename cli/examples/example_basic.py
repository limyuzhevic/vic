#!/usr/bin/env python3
"""
Basic NLM Example

This script demonstrates the basic usage of NLM with a simple simulation.
"""

import sys
sys.path.insert(0, "../src")

from core.Config.Config import Config
from brain.Brain import Brain
from brain.NeuronType import NeuronType

def main():
    print("=== NLM Basic Example ===\n")
    
    # Create configuration
    config = Config()
    config.load_from_file("configs/basic_config.yaml")
    
    print(f"Configuration loaded:")
    print(f"  Max steps: {config.get_or('max_steps', 1000)}")
    print(f"  Time step: {config.get_or('time_step', 0.001)}")
    print(f"  Leak conductance: {config.get_or('leak_conductance', 10.0)}")
    
    # Create brain
    brain = Brain()
    brain.set_max_steps(config.get_or("max_steps", 1000))
    brain.set_time_step(config.get_or("time_step", 0.001))
    brain.set_leak_conductance(config.get_or("leak_conductance", 10.0))
    
    # Create regions
    brain.create_region("input", 5, NeuronType.EXCITATORY)
    brain.create_region("hidden", 10, NeuronType.EXCITATORY)
    brain.create_region("output", 3, NeuronType.EXCITATORY)
    
    # Create memory systems
    brain.create_working_memory(capacity=config.get_or("working_memory_capacity", 100))
    brain.create_episodic_memory(max_episodes=config.get_or("episodic_memory_capacity", 1000))
    brain.create_semantic_memory()
    brain.create_procedural_memory()
    brain.create_associative_memory()
    
    # Setup connections
    input_neurons = brain.get_region("input").get_all_neurons()
    hidden_neurons = brain.get_region("hidden").get_all_neurons()
    output_neurons = brain.get_region("output").get_all_neurons()
    
    # Connect input to hidden
    for input_neuron in input_neurons:
        for hidden_neuron in hidden_neurons:
            brain.create_synapse(input_neuron, hidden_neuron, 
                               weight=config.get_or("default_synaptic_weight", 0.5))
    
    # Connect hidden to output
    for hidden_neuron in hidden_neurons:
        for output_neuron in output_neurons:
            brain.create_synapse(hidden_neuron, output_neuron,
                               weight=config.get_or("hidden_output_weight", 0.3))
    
    print(f"\nNetwork setup complete:")
    print(f"  Input neurons: {len(input_neurons)}")
    print(f"  Hidden neurons: {len(hidden_neurons)}")
    print(f"  Output neurons: {len(output_neurons)}")
    print(f"  Total synapses: {len(brain.get_all_synapses())}")
    
    # Run simulation
    print(f"\nRunning simulation for {brain.get_max_steps()} steps...")
    
    for step in range(brain.get_max_steps()):
        # Apply input (simulated pattern)
        import math
        input_pattern = 0.8 * (1 + math.sin(2 * math.pi * step / 50)) / 2
        
        # Apply to input region
        for neuron in input_neurons:
            neuron.injectCurrent(input_pattern * 2.0)
        
        # Step brain
        brain.step()
        
        # Print progress
        if step % 200 == 0:
            stats = brain.get_statistics()
            print(f"  Step {step}: {stats.average_firing_rate:.2f} Hz firing rate, {stats.total_spikes} total spikes")
    
    # Final statistics
    print(f"\nSimulation complete!")
    final_stats = brain.get_statistics()
    mem_stats = brain.get_memory_statistics()
    
    print(f"\nFinal Statistics:")
    print(f"  Total simulation time: {final_stats.simulation_time:.2f} ms")
    print(f"  Total spikes: {final_stats.total_spikes}")
    print(f"  Average firing rate: {final_stats.average_firing_rate:.2f} Hz")
    print(f"  Max firing rate: {final_stats.max_firing_rate:.2f} Hz")
    
    print(f"\nMemory Usage:")
    print(f"  Working memory: {mem_stats.working_memory_usage}/{mem_stats.working_memory_capacity}")
    print(f"  Episodic memory: {mem_stats.episodic_memory_episodes} episodes")
    print(f"  Semantic memory: {len(brain.get_semantic_memory().get_all_facts())} facts")
    print(f"  Procedural memory: {len(brain.get_procedural_memory().get_all_skills())} skills")
    print(f"  Associative memory: {mem_stats.associative_memory_associations} associations")
    
    # Save results
    brain.save_results("basic_example_results.txt")
    print(f"\nResults saved to basic_example_results.txt")
    
    print(f"\n=== Example Complete ===")

if __name__ == "__main__":
    main()