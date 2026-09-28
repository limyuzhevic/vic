#!/usr/bin/env python3
"""
Example 2: Brain with World Interaction

This example demonstrates how to create a complete agent-environment
system where the brain interacts with a simulated world.
"""

import pynlm

def agent_world_interaction():
    """Run agent-world interaction simulation."""
    
    print("=== Example 2: Brain with World Interaction ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20,
        height=20,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    
    # Set agent start position
    world.setAgentStart(10.0, 10.0)
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print(f"Environment setup:")
    print(f"  World size: {world.getWidth()}x{world.getHeight()}")
    print(f"  Vision range: {world.getVisionWidth()}x{world.getVisionHeight()}")
    print(f"  Agent start: ({world.getAgentPosition().x}, {world.getAgentPosition().y})")
    
    # Run simulation loop
    print("\nRunning agent simulation...")
    for step in range(200):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress
        if step % 50 == 0:
            agent_pos = world.getAgentPosition()
            curiosity = agent.getCuriosityLevel()
            reward = agent.getReward()
            
            print(f"  Step {step}: Position ({agent_pos.x:.1f}, {agent_pos.y:.1f}), "
                  f"Curiosity: {curiosity:.2f}, Reward: {reward:.2f}")
    
    # Get final statistics
    agent_pos = world.getAgentPosition()
    final_firing = brain.getFiringNeuronCount()
    total_spikes = brain.getTotalSpikeCount()
    curiosity = agent.getCuriosityLevel()
    
    print(f"\nSimulation complete!")
    print(f"  Final agent position: ({agent_pos.x:.1f}, {agent_pos.y:.1f})")
    print(f"  Brain activity: {final_firing} neurons firing, {total_spikes} total spikes")
    print(f"  Curiosity level: {curiosity:.3f}")
    
    return brain, world, agent

if __name__ == "__main__":
    agent_world_interaction()