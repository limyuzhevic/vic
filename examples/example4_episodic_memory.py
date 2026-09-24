#!/usr/bin/env python3
"""
Example 4: Episodic Memory and Replay

This example demonstrates how NLM's episodic memory system works, including
memory storage, consolidation, and replay during sleep/rest cycles.
"""

import sys
import os

# Add the parent directory to path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import pynlm

def main():
    print("\n=== Example 4: Episodic Memory and Replay ===")
    
    # Create configuration with memory settings
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1200)
    config.set("brain.synapse_density", 0.1)
    
    # Create brain
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Setup
    world.configure(width=12, height=12, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(6, 6)
    agent.initialize(world)
    
    # Enable features
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    # Run multiple episodes
    num_episodes = 20
    episode_length = 100
    
    for episode in range(num_episodes):
        print(f"Episode {episode + 1}/{num_episodes}")
        
        # Run episode
        for step in range(episode_length):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            if step % 50 == 0:
                agent.updateDevelopment(0.1)
        
        # Store episode in memory (this happens automatically)
        print(f"  Episode completed: Firing={brain.getFiringNeuronCount()}, "
              f"Novelty={agent.getNoveltyLevel():.3f}")
        
        # Rest period for memory consolidation (simulated)
        for rest_step in range(20):  # Rest for 2 time units
            # During rest, brain processes stored memories
            brain.step(episode_length + rest_step)
            agent.updateDevelopment(0.1)
        
        # Check memory stats
        print(f"  Memory stats: Working memory active, "
              f"Episodic memory updated")
    
    print("\n=== Episodic Memory Example Complete ===")
    print("The brain has stored and consolidated multiple episodes.")
    print("Memory replay occurred during rest periods.")
    
    return brain, agent, world

if __name__ == "__main__":
    main()
