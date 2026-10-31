#!/usr/bin/env python3
# Agent in World Example
# Demonstrates NLM agent-world interaction

import sys
sys.path.insert(0, '../')

import pynlm as nlm
import time

def main():
    print("=== NLM Agent in World Example ===")
    print("This example shows how to use NLM in a simulated world environment.\n")
    
    # Setup brain
    print("1. Setting up brain...")
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    print(f"   Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # Create world
    print("2. Creating world...")
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    
    # Place agent in world
    world.setAgentStart(10, 10)
    print(f"   World created: {world.getWidth()}x{world.getHeight()}")
    print(f"   Agent start position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Create agent brain
    print("3. Creating agent brain...")
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Print initial agent state
    body = world.getAgentBody()
    print(f"   Agent body initialized:")
    print(f"     Position: ({body.x}, {body.y})")
    print(f"     Orientation: {body.orientation}°")
    print(f"     Velocity: ({body.velocityX}, {body.velocityY})")
    
    # Run simulation
    print("\n4. Running agent simulation (10 steps)...")
    
    for step in range(10):
        # Update world
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        vision_width = percept.getVisionWidth()
        vision_height = percept.getVisionHeight()
        
        print(f"   Step {step}:")
        print(f"     Agent at ({body.x:.1f}, {body.y:.1f}), orientation {body.orientation:.1f}°")
        print(f"     Senses: vision {vision_width}x{vision_height}")
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        print(f"     Action: {action.getName()}")
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update agent body reference
        body = world.getAgentBody()
    
    # Print final statistics
    print("\n5. Simulation completed!")
    print("\nFinal statistics:")
    print(f"   Total simulation time: {world.getSimulationTime():.2f}s")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Final position: ({body.x:.1f}, {body.y:.1f})")
    print(f"   Final orientation: {body.orientation:.1f}°")
    
    print("\nExample completed successfully!")

if __name__ == "__main__":
    main()
