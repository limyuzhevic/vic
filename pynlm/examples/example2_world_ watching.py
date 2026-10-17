#!/usr/bin/env python3
"""
Example 2: Brain Watching a World

This example shows how to create a brain, world, and agent,
and have the brain observe and interact with the world.
"""

import sys
import pynlm

def main():
    print("=" * 50)
    print("Example 2: Brain Watching a World")
    print("=" * 50)
    
    try:
        # Setup
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Watch the world for 30 steps
        for i in range(30):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(i)
            
            if i % 10 == 0:
                print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
        
        print("✓ Watched world for 30 steps!")
        print(f"  - Final firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
        
        # Show what the agent saw
        percept = world.getSensoryPercept()
        print(f"  - Vision width: {percept.getVisionWidth()}")
        print(f"  - Vision height: {percept.getVisionHeight()}")
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return 1
    
    print("=" * 50)
    return 0
if __name__ == "__main__":
    sys.exit(main())