#!/usr/bin/env python3
"""
NLM Example Scripts for Beginners

This file contains simple, beginner-friendly examples of NLM usage.
Copy and paste these examples into your own scripts!

Run: python beginner_examples.py
"""

import pynlm

def simplest_brain_example():
    """The simplest possible brain example"""
    print("=== Simplest Brain Example ===")
    
    # One line to create a virtual brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Initialize it
    brain.initialize()
    
    # Make it active
    brain.step(0)
    
    print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
    print()

def brain_with_world_example():
    """Example: Brain watching a world"""
    print("=== Brain Watching a World Example ===")
    
    # Setup
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Watch the world for 30 steps
    for i in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        
    print(f"Watched world for 30 steps")
    print(f"Firing rate: {brain.getAverageFiringRate()}")
    print()

def complete_agent_example():
    """Example: Complete agent with learning"""
    print("=== Complete Agent Example ===")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run agent
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print("Agent simulation complete!")
    print()

def experimental_example():
    """Example: Experimental setup"""
    print("=== Experimental Example ===")
    
    # Create brain with specific configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation with statistics
    for i in range(50):
        brain.step(i)
    
    print(f"Experiment results:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print()

def main():
    """Run all beginner examples"""
    print("NLM Beginner Examples")
    print("=" * 30)
    print()
    
    simplest_brain_example()
    brain_with_world_example()
    complete_agent_example()
    experimental_example()
    
    print("All beginner examples completed!")
    print("\nThese examples demonstrate:")
    print("  ✓ Basic brain creation and usage")
    print("  ✓ World interaction with sensory input")
    print("  ✓ Complete agent simulation with learning")
    print("  ✓ Basic experimental setup")

if __name__ == "__main__":
    main()
