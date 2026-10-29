# Advanced NLM Python Usage Examples
# Comprehensive examples demonstrating advanced usage patterns for NLM Python bindings

import pynlm
import time
from typing import List, Dict, Any

print("NLM Advanced Usage Examples")
print("=" * 50)

def example_1_basic_learning_simulation():
    """
    Example 1: Basic Learning Simulation
    
    Demonstrates a complete learning simulation with reward-based plasticity.
    This example shows how to set up a learning environment and track progress.
    """
    print("\n1. Basic Learning Simulation")
    print("-" * 30)
    
    # Create configuration for learning
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("plasticity_learning_rate", 0.01)
    config.set("random_seed", 42)
    
    # Initialize brain and agent
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure world for the task
    world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run learning simulation
    brain.initialize()
    world.reset()
    agent.initialize(world)
    
    learning_history = []
    
    for episode in range(10):
        world.reset()
        episode_reward = 0
        
        for step in range(100):
            # Environment dynamics
            world.update(0.1)
            
            # Perception-action loop
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward for learning
            if hasattr(world, 'getReward'):
                reward = world.getReward()
                predicted = agent.getPredictedReward()
                agent.applyRewardModulation(reward, predicted)
                episode_reward += reward
        
        learning_history.append({
            'episode': episode,
            'total_reward': episode_reward,
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount()
        })
    
    # Analyze learning progress
    rewards = [h['total_reward'] for h in learning_history]
    avg_reward = sum(rewards) / len(rewards)
    
    print(f"Learning completed over {len(learning_history)} episodes")
    print(f"Average reward per episode: {avg_reward:.2f}")
    print(f"Reward trend: {'Improving' if rewards[-1] > rewards[0] else 'Declining'}")
    
    return brain, learning_history

def example_2_development_simulation():
    """
    Example 2: Development Simulation
    
    Demonstrates neural development and maturation over time. This example
    shows how brains develop and change their capabilities through age.
    """
    print("\n2. Development Simulation")
    print("-" * 30)
    
    # Create developing brain
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 300)
    config.set("development_synaptogenesis_rate", 0.001)
    config.set("development_pruning_rate", 0.0001)
    
    brain = pynlm.createBrain(config)
    brain.enableDevelopment(True)
    
    # Track development metrics
    development_metrics = []
    
    brain.initialize()
    
    for day in range(1, 31):  # Simulate 30 days of development
        # Simulate daily learning
        for step in range(50):
            brain.step(step)
        
        # Record metrics
        metrics = {
            'day': day,
            'neuron_count': brain.getTotalNeuronCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'developmental_stage': brain.getDevelopmentalStage()
        }
        development_metrics.append(metrics)
        
        print(f"Day {day}: {metrics['developmental_stage']}, "
              f"Firing: {metrics['firing_neuron_count']}, "
              f"Avg Rate: {metrics['avg_firing_rate']:.2f} Hz")
    
    # Analyze development patterns
    final_stage = development_metrics[-1]['developmental_stage']
    growth_rate = (development_metrics[-1]['avg_firing_rate'] - 
                  development_metrics[0]['avg_firing_rate']) / 30
    
    print(f"\nDevelopment Summary:")
    print(f"Final stage: {final_stage}")
    print(f"Firing rate growth: {growth_rate:.4f} Hz/day")
    
    return brain, development_metrics

# [Rest of the examples continue...]

if __name__ == "__main__":
    print("This file contains advanced NLM Python examples.")
    print("Run specific examples by importing and calling the functions.")
    print("Example: from advanced_examples import example_1_basic_learning_simulation")
    print("Then call: example_1_basic_learning_simulation()")