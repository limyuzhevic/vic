#!/usr/bin/env python3
"""
Example 2: Brain Watching a World

This example shows how a brain interacts with a simulated world through sensory input
and produces motor actions.
"""

import pynlm
import time

def create_brain_world_example():
    """Create a brain and world interaction example."""
    print("=== Brain-World Interaction Example ===")
    
    # Create the brain
    print("1. Creating brain...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create the world
    print("2. Creating world...")
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create the agent interface
    print("3. Creating agent interface...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    print("4. Enabling learning features...")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    print("5. Running simulation for 30 steps...")
    for step in range(30):
        # Update world
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain what it sees
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"   Step {step}:")
            print(f"     Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"     Vision data shape: {len(percept.getVision())}x{len(percept.getVision())}")
            print(f"     Action: {action}")
            print(f"     Reward modulation: {'Enabled' if agent.isRewardModulationEnabled() else 'Disabled'}")
    
    # Display final results
    print("6. Simulation completed!")
    print(f"   Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    
    return brain, world, agent

if __name__ == "__main__":
    brain, world, agent = create_brain_world_example()
    print("\n✓ Brain-world interaction example completed successfully!")