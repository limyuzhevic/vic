#!/usr/bin/env python3
"""
NLM Demo 2: Brain Watching a World

This demo shows how a brain interacts with a simple world. The brain receives
visual input from the world and decides what to do, while the world executes
the actions and provides feedback.

This demonstrates the core closed-loop system of NLM.
"""

import pynlm
import time

def main():
    print("=== NLM Brain Watching a World Demo ===")
    print("This demo shows a brain perceiving and acting in a virtual world.\n")
    
    # Setup phase
    print("1. Setting up the brain and world...")
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"   World size: {world.getWidth()}x{world.getHeight()}")
    print(f"   Agent vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    print(f"   Agent starting position: {world.getAgentBody().x}, {world.getAgentBody().y}")
    
    # Watching phase
    print("\n2. Watching the world for 30 steps...")
    print("   (Brain is receiving visual input each step)\n")
    
    for step in range(30):
        # Update world
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Print periodic updates
        if step % 10 == 0:
            firing = brain.getFiringNeuronCount()
            print(f"   Step {step}:")
            print(f"     Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"     Brain firing: {firing} neurons")
            print(f"     Brain firing rate: {brain.getAverageFiringRate():.2f}")
    
    # Results
    print("\n3. Results after watching...")
    print(f"   Final agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print(f"   Final firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Total reward: {agent.getCuriosityLevel():.3f}")
    
    print("\n=== Brain Watching Demo Complete ===")
    print("\nWhat you learned:")
    print("- How sensory input flows from world to brain")
    print("- How brains process visual information")
    print("- The basic perception-action loop")

if __name__ == "__main__":
    main()