#!/usr/bin/env python3
"""Example 2: Brain Watching a World

This example shows how a brain interacts with a world environment, receiving
sensory input and producing actions. The brain learns from its experiences
through neuromodulation.
"""

import pynlm

def watching_world_example():
    """Setup and run a brain in a simple world."""
    print("=== Brain Watching World Example ===")
    
    # Setup
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning capabilities
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Watch the world for 30 steps
    for i in range(30):
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain what it sees
        agent.processSensoryInput(percept)
        
        # Brain thinks/recomputes
        brain.step(i)
        
        # Brain decides what to do
        action = agent.decodeMotorCommand()
        
        # Do the action in the world
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print("Watched world for 30 steps")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")

if __name__ == "__main__":
    watching_world_example()