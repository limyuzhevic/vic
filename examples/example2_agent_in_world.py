#!/usr/bin/env python3
"""
Example 2: Agent in a 2D World

This example demonstrates creating a complete agent simulation where a brain
interacts with a simple 2D environment. This is the most common use case.
"""

import sys
import os
import json
import time
import matplotlib.pyplot as plt

# Add the parent directory to path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import pynlm

def main():
    print("=== Example 2: Agent in a 2D World ===")
    
    # 1. Create complete agent setup
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("brain.synapse_density", 0.1)
    
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure world
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Created agent in {world.getWidth()}x{world.getHeight()} world")
    print(f"Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    
    # 2. Run simulation loop
    num_steps = 200
    metrics = {
        'steps': [],
        'firing_rates': [],
        'curiosity': [],
        'novelty': [],
        'actions': []
    }
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply neuromodulation
        internal = percept.getInternal()
        reward = internal[0] if internal else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development periodically
        if step % 50 == 0:
            agent.updateDevelopment(0.1)
        
        # Record metrics
        metrics['steps'].append(step)
        metrics['firing_rates'].append(brain.getFiringNeuronCount())
        metrics['curiosity'].append(agent.getCuriosityLevel())
        metrics['novelty'].append(agent.getNoveltyLevel())
        metrics['actions'].append(str(action))
        
        if step % 50 == 0:
            print(f"Step {step}: Action={action}, "
                  f"Firing={brain.getFiringNeuronCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}")
    
    # 3. Print final results
    print("\n=== Simulation Results ===")
    print(f"Total steps: {num_steps}")
    print(f"Final firing rate: {brain.getFiringNeuronCount()} neurons")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty: {agent.getNoveltyLevel():.3f}")
    print(f"Final dopamine: {agent.getNeuromodulationLevel():.3f}")
    
    # 4. Save metrics to file
    with open('agent_world_metrics.json', 'w') as f:
        json.dump(metrics, f, indent=2)
    print("Metrics saved to agent_world_metrics.json")
    
    # 5. Simple plot (if matplotlib available)
    try:
        plot_results(metrics)
        print("Plot saved to agent_world_plot.png")
    except ImportError:
        print("Matplotlib not available, skipping plots")
    
    return brain, agent, world, metrics

def plot_results(metrics):
    """Plot simulation results."""
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Firing rate over time
    axes[0, 0].plot(metrics['steps'], metrics['firing_rates'])
    axes[0, 0].set_title('Firing Rate Over Time')
    axes[0, 0].set_xlabel('Step')
    axes[0, 0].set_ylabel('Firing Neurons')
    
    # Curiosity level
    axes[0, 1].plot(metrics['steps'], metrics['curiosity'], label='Curiosity')
    axes[0, 1].plot(metrics['steps'], metrics['novelty'], label='Novelty')
    axes[0, 1].set_title('Motivation Levels')
    axes[0, 1].set_xlabel('Step')
    axes[0, 1].set_ylabel('Level')
    axes[0, 1].legend()
    
    # Action distribution
    from collections import Counter
    action_counts = Counter(metrics['actions'])
    axes[1, 0].bar(action_counts.keys(), action_counts.values())
    axes[1, 0].set_title('Action Distribution')
    axes[1, 0].set_xlabel('Action')
    axes[1, 0].set_ylabel('Count')
    
    # Learning curves
    axes[1, 1].plot(metrics['steps'][:100], 
                   [metrics['curiosity'][i] * metrics['novelty'][i] 
                    for i in range(min(100, len(metrics['steps'])) )])
    axes[1, 1].set_title('Learning Progress (Curiosity × Novelty)')
    axes[1, 1].set_xlabel('Step')
    axes[1, 1].set_ylabel('Product')
    
    plt.tight_layout()
    plt.savefig('agent_world_plot.png', dpi=150)
    plt.close()

if __name__ == "__main__":
    main()
