# NLM Examples - Beyond the Basics

This document provides additional examples that build upon the basic usage guide and demonstrate more advanced features of the NLM (Neural Learning Machine) system.

## Table of Contents

1. [Building Your First Learning Agent](#building-your-first-learning-agent)
2. [Working with Multiple Regions](#working-with-multiple-regions)
3. [Checkpointing and Persistence](#checkpointing-and-persistence)
4. [Advanced Neuromodulation](#advanced-neuromodulation)
5. [Memory Replay and Consolidation](#memory-replay-and-consolidation)
6. [Development and Maturation](#development-and-maturation)
7. [Performance Optimization](#performance-optimization)
8. [Debugging and Troubleshooting](#debugging-and-troubleshooting)

---

## Building Your First Learning Agent

This example shows how to create a complete learning agent that can adapt to its environment through reward-based learning.

```python
import pynlm
import time

def create_learning_agent(world_size=20, neuron_count=200):
    """Create a learning agent with optimal configuration."""
    
    # Create configuration with learning-focused settings
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(
        width=world_size,
        height=world_size,
        visionWidth=world_size//2,
        visionHeight=world_size//2
    )
    world.reset()
    world.setAgentStart(world_size//2, world_size//2)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    return brain, world, agent

def run_learning_episode(brain, world, agent, max_steps=1000):
    """Run a complete learning episode and return statistics."""
    
    episode_stats = {
        'steps': 0,
        'total_reward': 0.0,
        'max_curiosity': 0.0,
        'max_novelty': 0.0,
        'development_stage': 0,
        'spike_count': 0,
        'firing_neuron_count': 0
    }
    
    print(f"Starting learning episode with {max_steps} steps...")
    print("Step | Reward | Curiosity | Novelty | Dev Stage | Spikes | Firing")
    print("-" * 75)
    
    for step in range(max_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Get action from agent
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward based on agent's state
        body = world.getAgentBody()
        reward = calculate_reward(body, world)
        
        # Apply reward modulation for learning
        predicted_reward = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect statistics
        episode_stats['steps'] += 1
        episode_stats['total_reward'] += reward
        episode_stats['max_curiosity'] = max(episode_stats['max_curiosity'], agent.getCuriosityLevel())
        episode_stats['max_novelty'] = max(episode_stats['max_novelty'], agent.getNoveltyLevel())
        episode_stats['development_stage'] = brain.getDevelopmentalStage()
        episode_stats['spike_count'] += brain.getTotalSpikeCount()
        episode_stats['firing_neuron_count'] += brain.getFiringNeuronCount()
        
        # Log progress every 100 steps
        if (step + 1) % 100 == 0:
            print(f"{step+1:4d} | {reward:6.2f} | {agent.getCuriosityLevel():8.3f} | "
                  f"{agent.getNoveltyLevel():7.3f} | {brain.getDevelopmentalStage():9d} | "
                  f"{brain.getTotalSpikeCount():5d} | {brain.getFiringNeuronCount():5d}")
        
        # Early stopping if agent is stuck
        if step > 100 and agent.getCuriosityLevel() < 0.01 and agent.getNoveltyLevel() < 0.01:
            print(f"Agent curiosity and novelty are low - episode may be completed")
            break
    
    print("-" * 75)
    print(f"Episode completed: {episode_stats['steps']} steps")
    print(f"Total reward: {episode_stats['total_reward']:.2f}")
    print(f"Average reward: {episode_stats['total_reward']/episode_stats['steps']:.3f}")
    print(f"Max curiosity: {episode_stats['max_curiosity']:.3f}")
    print(f"Max novelty: {episode_stats['max_novelty']:.3f}")
    print(f"Final development stage: {episode_stats['development_stage']}")
    
    return episode_stats

def calculate_reward(body, world):
    """Calculate reward based on agent's state and environment."""
    reward = 0.0
    
    # Positive reward for good energy
    if body.energy > 0.7:
        reward += 1.0
    elif body.energy > 0.4:
        reward += 0.5
    
    # Positive reward for good health
    if body.health > 0.8:
        reward += 1.0
    
    # Negative reward for danger
    if body.health < 0.3:
        reward -= 2.0
    
    # Positive reward for exploration (high curiosity triggers this)
    # This creates a positive feedback loop for exploration
    
    # Check for resources in the world
    percept = world.getSensoryPercept()
    vision = percept.getVision()
    if vision.size() > 0:
        max_vision_value = max(vision)
        if max_vision_value > 0.5:  # Resource found
            reward += 2.0
        elif max_vision_value > 0.2:  # Something detected
            reward += 0.5
    
    return reward

def example_learning_agent():
    """Complete example of using the learning agent."""
    print("=== Example: Building a Learning Agent ===\n")
    
    # Create agent
    brain, world, agent = create_learning_agent(world_size=15, neuron_count=300)
    
    print("Agent created successfully!")
    print(f"- Neurons: {brain.getTotalNeuronCount()}")
    print(f"- Total synapses: {brain.getTotalSynapseCount()}")
    print(f"- Initial developmental stage: {brain.getDevelopmentalStage()}")
    print()
    
    # Run learning episode
    stats = run_learning_episode(brain, world, agent, max_steps=500)
    
    print(f"\n=== Learning Results ===")
    print(f"Learning efficiency: {stats['max_curiosity']:.3f} curiosity, {stats['max_novelty']:.3f} novelty")
    print(f"Behavioral adaptation: {brain.getDevelopmentalStage()} development stage")
    print(f"Survival reward: {stats['total_reward']:.2f} over {stats['steps']} steps")
    
    return brain, world, agent, stats

if __name__ == "__main__":
    example_learning_agent()
