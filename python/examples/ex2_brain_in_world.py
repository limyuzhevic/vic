#!/usr/bin/env python3
"""
Example 2: Brain in a Simple World

This example shows how to create a complete brain-world-agent system
where the brain interacts with a simulated environment.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def main():
    print("=== Example 2: Brain in a Simple World ===\n")
    
    # Create everything
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    print("✓ Created and initialized brain")
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    print("✓ Created and configured world")
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print("✓ Created and initialized agent")
    
    # Run for 50 steps
    print("\nRunning simulation...")
    for step in range(50):
        world.update(0.1)  # Update world
        percept = world.getSensoryPercept()  # What does the agent see?
        agent.processSensoryInput(percept)  # Brain sees it
        brain.step(step)  # Brain thinks
        action = agent.decodeMotorCommand()  # Brain decides action
        world.applyMotorCommand(action, world.getSimulationTime())  # Do action
        
        if step % 10 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print("\n✓ Simulation finished!")
    print(f"  - Total steps: 50")
    print(f"  - Final firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  - Development stage: {brain.getDevelopmentalStage()}")
    print()

if __name__ == "__main__":
    main()
