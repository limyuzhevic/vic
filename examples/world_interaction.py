#!/usr/bin/env python3
"""
World Interaction Example
Demonstrates how NLM brains interact with simulated worlds.
"""

import pynlm
import numpy as np
import time

def print_separator(title):
    print("\n" + "="*60)
    print(title)
    print("="*60)

def main():
    print("=== World Interaction Demo ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 150)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=15,
        height=15,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print_separator("Setup Complete")
    print(f"Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"World: {world.getWidth()}x{world.getHeight()} cells")
    print(f"Agent start: {world.getAgentPosition()}")
    
    # Simulation loop
    print_separator("Starting Simulation")
    
    episode_steps = 0
    max_episode_steps = 200
    
    while episode_steps < max_episode_steps:
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(episode_steps)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = agent.getCurrentReward()
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if episode_steps % 50 == 0:
            print(f"Step {episode_steps}:")
            print(f"  Position: ({world.getAgentPosition()[0]:.1f}, {world.getAgentPosition()[1]:.1f})")
            print(f"  Reward: {reward:.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
            print(f"  Firing: {brain.getFiringNeuronCount()}")
        
        episode_steps += 1
        
        # Check for termination
        if agent.getCurrentReward() > 10.0:  # High reward suggests task completion
            break
    
    print_separator("Episode Complete")
    print(f"Total steps: {episode_steps}")
    print(f"Final position: ({world.getAgentPosition()[0]:.1f}, {world.getAgentPosition()[1]:.1f})")
    print(f"Final development stage: {brain.getDevelopmentalStage()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
    
    # Demonstrate different action types
    print_separator("Demonstrating Action Types")
    
    # Show available actions
    print("Available action types:")
    for action_type in range(4):
        action_name = ["MoveForward", "TurnLeft", "TurnRight", "Interact"][action_type]
        print(f"  {action_type}: {action_name}")
    
    # Demonstrate checkpoint functionality
    checkpoint_file = "agent_checkpoint.nlm"
    brain.save(checkpoint_file)
    print(f"\nBrain state saved to: {checkpoint_file}")
    
    # Load checkpoint (demonstration)
    new_brain = pynlm.createBrain(config)
    new_brain.initialize()
    new_brain.load(checkpoint_file)
    print(f"Brain state loaded successfully")
    
    print("\n=== World Interaction Demo Complete ===")

if __name__ == "__main__":
    main()