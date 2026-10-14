#!/usr/bin/env python3

"""
Example usage of NLM Python bindings demonstrating best practices and convenience methods.
This file shows how users should interact with the pynlm library after improvements.
"""

import pynlm
import numpy as np
import json

def basic_example():
    """Basic example of creating and running a brain simulation."""
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Configure with some parameters
    config.set("neuron_count", 500, pynlm.ConfigSource.Runtime)
    config.set("region_count", 3, pynlm.ConfigSource.Runtime)
    config.set("connection_probability", 0.15, pynlm.ConfigSource.Runtime)
    
    # Create brain with configuration
    brain = pynlm.createBrain(config)
    
    # Initialize the brain
    brain.initialize()
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"Number of regions: {brain.getRegionCount()}")
    
    # Run a simple simulation
    for step in range(10):
        brain.step(step)
        
    print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    return brain

def agent_example():
    """Example using agent brain interface for interaction with world."""
    # Create brain and agent components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(width=100, height=100, visionWidth=21, visionHeight=21)
    world.setAgentStart(50, 50)
    
    # Create agent brain
    agent_brain = pynlm.createAgentBrain(brain)
    agent_brain.initialize(world)
    
    # Create percept and action objects
    percept = pynlm.SensoryPercept()
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    
    print("Agent example setup complete")
    return agent_brain, world

def memory_example():
    """Example demonstrating memory system usage."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get brain regions for memory integration
    regions = brain.getRegions()
    
    print(f"Brain has {len(regions)} regions")
    
    # Example of episodic memory (if available)
    if hasattr(brain, 'getEpisodicMemory'):
        memory = brain.getEpisodicMemory()
        print(f"Episodic memory capacity: {memory.getCapacity()}")
    
    return brain

def config_example():
    """Example showing configuration management."""
    # Create config from file or programmatically
    config = pynlm.createDefaultConfig()
    
    # Set various configuration options
    config.set("simulation.timestep", 0.001, pynlm.ConfigSource.Default)
    config.set("neuron.LIF.v_threshold", -55.0, pynlm.ConfigSource.Default)
    config.set("neuron.LIF.v_reset", -65.0, pynlm.ConfigSource.Default)
    config.set("plasticity.STDP.learning_rate", 0.01, pynlm.ConfigSource.Default)
    
    # Save configuration to file
    config.saveToFile("brain_config.json")
    
    # Load from file
    config2 = pynlm.createDefaultConfig()
    config2.loadFromFile("brain_config.json")
    
    # Show config summary
    print("Configuration summary:")
    print(config.summary())
    
    return config

def visualization_example():
    """Example showing visualization capabilities."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some simulation
    for i in range(20):
        brain.step(i)
    
    # Get visualization interface
    if hasattr(brain, 'getVisualizationInterface'):
        viz = brain.getVisualizationInterface()
        
        # Export neural activity
        activity_data = viz.getNeuralActivity()
        
        # Export spike times
        spike_times = viz.getSpikeTimes()
        
        print(f"Visualization data collected:")
        print(f"  Neural activity samples: {len(activity_data)}")
        print(f"  Spike records: {len(spike_times)}")
        
        # Save to file for external visualization
        viz.saveToFile("brain_activity.json")
    
    return brain

def advanced_simulation_example():
    """Example of advanced simulation with multiple features."""
    # Create simulation with all systems enabled
    config = pynlm.createDefaultConfig()
    
    # Enable all advanced features
    config.set("neuromodulation.reward.enabled", True, pynlm.ConfigSource.Default)
    config.set("neuromodulation.curiosity.enabled", True, pynlm.ConfigSource.Default)
    config.set("neuromodulation.novelty.enabled", True, pynlm.ConfigSource.Default)
    config.set("memory.episodic.enabled", True, pynlm.ConfigSource.Default)
    config.set("memory.associative.enabled", True, pynlm.ConfigSource.Default)
    config.set("prediction.enabled", True, pynlm.ConfigSource.Default)
    config.set("development.enabled", True, pynlm.ConfigSource.Default)
    config.set("structural_plasticity.enabled", True, pynlm.ConfigSource.Default)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run extended simulation
    for episode in range(3):
        print(f"Episode {episode + 1}:")
        
        # Run simulation steps
        for step in range(30):
            brain.step(step)
            
            # Check for developmental stage changes
            stage = brain.getDevelopmentalStage()
            if stage.name != "Adult":
                print(f"  Stage: {stage.name}")
        
        # Save checkpoint
        checkpoint_file = f"brain_checkpoint_ep{episode + 1}.json"
        brain.save(checkpoint_file)
        print(f"  Checkpoint saved: {checkpoint_file}")
    
    print("Advanced simulation complete")
    return brain

if __name__ == "__main__":
    print("=== NLM Python Bindings Usage Examples ===\n")
    
    print("1. Basic Example:")
    brain1 = basic_example()
    print()
    
    print("2. Agent Example:")
    agent_brain, world = agent_example()
    print()
    
    print("3. Memory Example:")
    brain2 = memory_example()
    print()
    
    print("4. Configuration Example:")
    config = config_example()
    print()
    
    print("5. Visualization Example:")
    brain3 = visualization_example()
    print()
    
    print("6. Advanced Simulation Example:")
    brain4 = advanced_simulation_example()
    print()
    
    print("=== All examples completed successfully ===")