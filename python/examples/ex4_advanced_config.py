#!/usr/bin/env python3
"""
Example 4: Advanced Brain Configuration

This example shows how to configure brains with different parameters
and run comparative simulations.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def create_config(neuron_count=500, connection_probability=0.1,
                  plasticity_learning_rate=0.01, development_enabled=True):
    """
    Create a custom brain configuration.
    
    Args:
        neuron_count: Number of neurons in the brain
        connection_probability: Probability of synaptic connections
        plasticity_learning_rate: Learning rate for plasticity
        development_enabled: Whether development is enabled
    
    Returns:
        Configuration object
    """
    config = pynlm.createDefaultConfig()
    
    # Custom settings
    config.set("neuron_count", neuron_count)
    config.set("connection_probability", connection_probability)
    config.set("plasticity_learning_rate", plasticity_learning_rate)
    
    # Note: Some settings might need to be set differently based on actual C++ API
    # This is just to demonstrate configuration customization
    
    return config

def run_simulation(config, name="Default"):
    """
    Run a simulation with the given configuration.
    
    Args:
        config: Brain configuration
        name: Simulation name for logging
    
    Returns:
        Dictionary with simulation results
    """
    print(f"\n=== Running {name} Simulation ===")
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get results
    results = {
        'name': name,
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'developmental_stage': brain.getDevelopmentalStage(),
        'neuromodulation_level': agent.getNeuromodulationLevel(),
        'curiosity_level': agent.getCuriosityLevel(),
    }
    
    print(f"  ✓ Total neurons: {results['total_neurons']}")
    print(f"  ✓ Total synapses: {results['total_synapses']}")
    print(f"  ✓ Firing neurons: {results['firing_neurons']}")
    print(f"  ✓ Total spikes: {results['total_spikes']}")
    print(f"  ✓ Average firing rate: {results['average_firing_rate']:.2f}")
    print(f"  ✓ Development stage: {results['developmental_stage']}")
    
    return results

def main():
    print("=== Example 4: Advanced Brain Configuration ===\n")
    
    # Run multiple simulations with different configurations
    results = []
    
    # Default configuration
    config1 = create_config(name="Small, Low Connectivity")
    results.append(run_simulation(config1, "Small, Low Connectivity"))
    
    # Larger, more connected brain
    config2 = create_config(
        neuron_count=1000,
        connection_probability=0.2,
        name="Large, High Connectivity"
    )
    results.append(run_simulation(config2, "Large, High Connectivity"))
    
    # With development
    config3 = create_config(
        neuron_count=500,
        development_enabled=True,
        name="With Development"
    )
    results.append(run_simulation(config3, "With Development"))
    
    # Summary
    print("\n=== Summary ===")
    print("All simulations completed successfully!")
    print(f"Ran {len(results)} different configurations:")
    for result in results:
        print(f"  - {result['name']}: {result['firing_neurons']} neurons firing")
    
    print("\n✓ Advanced configuration example complete!")

if __name__ == "__main__":
    main()
