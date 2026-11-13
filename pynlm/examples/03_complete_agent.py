#!/usr/bin/env python3
"""
Example 3: Complete Agent with Learning

This example demonstrates a complete agent that learns from its environment
through reward modulation, curiosity, and development.
"""

import pynlm
import time
import random

def create_complete_agent_example():
    """Create a complete learning agent example."""
    print("=== Complete Learning Agent Example ===")
    
    # Setup with custom configuration
    print("1. Setting up with custom configuration...")
    config = pynlm.createDefaultConfig()
    
    # Configure brain parameters
    config.set("brain.neuron_count", 500)
    config.set("brain.region_count", 3)
    config.set("brain.connection_probability", 0.1)
    config.set("brain.random_seed", random.randint(1, 1000))
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world environment
    print("2. Creating world environment...")
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    print("3. Creating agent interface...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Setup specific world configuration
    world.setMaxEnergy(100.0)
    world.setEnergyDecayRate(0.5)
    
    # Run agent with comprehensive learning
    print("4. Running agent with learning enabled...")
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from agent
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Simulate reward based on action result
        reward = result.reward if result else 0.0
        predicted_reward = agent.getPredictionError()
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 20 == 0:
            print(f"   Step {step}:")
            print(f"     Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"     Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"     Reward: {reward:.2f}")
            print(f"     Developmental stage: {brain.getDevelopmentalStage()}")
            print(f"     Curiosity level: {agent.getCuriosityLevel():.2f}")
            print(f"     Novelty level: {agent.getNoveltyLevel():.2f}")
    
    print("5. Simulation completed!")
    print(f"   Total steps: 100")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"   Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"   Learned preference: {brain.getLearnedPreference():.2f}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    
    return brain, world, agent

if __name__ == "__main__":
    brain, world, agent = create_complete_agent_example()
    print("\n✓ Complete agent example completed successfully!")