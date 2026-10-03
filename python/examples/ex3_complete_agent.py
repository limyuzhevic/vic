#!/usr/bin/env python3
"""
Example 3: Complete Agent with Learning

This example shows a complete agent that learns from experience using
all the advanced features: reward modulation, curiosity, and development.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def main():
    print("=== Example 3: Complete Agent with Learning ===\n")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("✓ Created and initialized brain")
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    print("✓ Created and configured world")
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print("✓ Created and initialized agent")
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    print("✓ Enabled reward modulation and curiosity")
    
    # Run agent
    print("\nRunning agent simulation...")
    for step in range(100):
        world.update(0.1)
        agent.process_sensory_input(world.getSensoryPercept())
        brain.step(step)
        action = agent.decode_motor_command()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
            
            # Apply some learning based on performance
            reward = 1.0 if brain.getFiringNeuronCount() > 10 else 0.0
            predicted = agent.getPredictionError()
            agent.applyRewardModulation(reward, predicted)
    
    print("\n✓ Agent simulation complete!")
    print(f"  - Total steps: 100")
    print(f"  - Final firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  - Development stage: {brain.getDevelopmentalStage()}")
    print(f"  - Neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
    print(f"  - Curiosity level: {agent.getCuriosityLevel():.2f}")
    print()

if __name__ == "__main__":
    main()
