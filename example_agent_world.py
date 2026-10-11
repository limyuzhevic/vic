#!/usr/bin/env python3
"""
NLM Example: Agent-World Interaction
=====================================

This example demonstrates how an NLM agent interacts with a simulated world,
including sensory perception, action selection, and learning.

Expected output:
- Agent created with sensory and motor capabilities
- World configured with vision and movement capabilities
- Simulation shows agent learning and adapting
- Displays curiosity and novelty levels
"""

import pynlm

def main():
    print("NLM Agent-World Interaction Example")
    print("=" * 50)
    
    # Create brain and agent
    print("\n1. Creating brain and agent...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    print("2. Setting up simulated world...")
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)  # Center of world
    
    # Initialize agent with world
    print("3. Initializing agent with world...")
    agent.initialize(world)
    
    # Enable learning features
    print("4. Enabling learning features...")
    agent.enableRewardModulation(True)  // Reward-based learning
    agent.enableCuriosity(True)          // Curiosity-driven exploration
    agent.enableDevelopment(True)        // Developmental changes
    
    print(f"   - Sensory input size: {agent.getSensoryInputSize()} values")
    print(f"   - Motor output size: {agent.getMotorOutputSize()} actions")
    
    # Run simulation
    print("\n5. Running simulation...")
    print("Step | Firing | Curiosity | Novelty | Action")
    print("-----|--------|-----------|---------|-------")
    
    for step in range(60):
        // Update world state
        world.update(0.1)
        
        // Get sensory input
        percept = world.getSensoryPercept()
        
        // Process sensory input
        agent.processSensoryInput(percept)
        
        // Brain computation
        brain.step(step)
        
        // Get motor command
        action = agent.decodeMotorCommand()
        
        // Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        // Print progress every 10 steps
        if step % 10 == 0:
            action_name = "Unknown"
            if action == pynlm.MotorCommand.MoveForward:
                action_name = "Move Forward"
            elif action == pynlm.MotorCommand.TurnLeft:
                action_name = "Turn Left"
            elif action == pynlm.MotorCommand.TurnRight:
                action_name = "Turn Right"
            elif action == pynlm.MotorCommand.Interact:
                action_name = "Interact"
            
            print(f"  {step:4d} |   {brain.getFiringNeuronCount():6d} |  " +
                  f"{agent.getCuriosityLevel():6.3f} |  " +
                  f"{agent.getNoveltyLevel():6.3f} |  {action_name}")
    
    print("\n" + "=" * 50)
    print("Simulation Results:")
    print(f"  Total steps completed: 60")
    print(f"  Average firing neurons: {brain.getAverageFiringRate():.3f}")
    print(f"  Maximum curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Maximum novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  Developmental stage: {agent.getDevelopmentalStage()}")
    
    print("\n✓ Agent-world interaction example completed successfully!")
    
    return 0

if __name__ == "__main__":
    import sys
    sys.exit(main())
