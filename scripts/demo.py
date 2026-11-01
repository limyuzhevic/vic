#!/usr/bin/env python3
"""
NLM Demo Script
Runs example simulations and demonstrates NLM capabilities
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm

def main():
    print("=== NLM Demo Script ===")
    print("Running example simulations...")
    
    # Example 1: Simple brain
    print("\n1. Simple Brain Demo:")
    brain = pynlm.createSimpleBrain(100)
    brain.initialize()
    
    for i in range(50):
        brain.step(i)
    
    print(f"   Brain with {brain.getTotalNeuronCount()} neurons completed {i+1} steps")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Active neurons: {brain.getFiringNeuronCount()}")
    
    # Example 2: Complete agent-world simulation
    print("\n2. Agent-World Demo:")
    brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain2.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain2)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain2.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"   Simulation completed with {brain2.getTotalSpikeCount()} total spikes")
    print(f"   Final firing rate: {brain2.getAverageFiringRate():.2f} Hz")
    
    # Example 3: Configuration help
    print("\n3. Configuration Help:")
    pynlm.printConfigHelp()
    
    print("\n=== Demo Complete ===")
    print("Try exploring more examples in the scripts directory!")

if __name__ == "__main__":
    main()
