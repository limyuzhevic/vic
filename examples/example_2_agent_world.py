#!/usr/bin/env python3
"""Example 2: Agent in a World"""

import nlm

def main():
    print("=== Example 2: Agent in a World ===\n")
    
    # Create brain
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("1. Created and initialized brain, world, and agent")
    print(f"   - World size: {world.getWidth()}x{world.getHeight()}")
    print(f"   - Vision field: {world.getVisionWidth()}x{world.getVisionHeight()}")
    print(f"   - Agent start position: (10.0, 10.0)")
    print()
    
    # Run simulation steps
    print("2. Running simulation (50 steps)...")
    for step in range(50):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action and apply it
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 10 steps
        if step % 10 == 0:
            print(f"   - Step {step}: action={action}, "
                  f"neurons={brain.getFiringNeuronCount()}, "
                  f"curiosity={agent.getCuriosityLevel():.3f}")
    
    print(f"\n3. Simulation completed")
    print(f"   - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   - Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"   - Development stage: {brain.getDevelopmentalStage()}")
    
    print("\n=== Example 2 Complete ===")
    return brain, world, agent

if __name__ == "__main__":
    main()