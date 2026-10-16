#!/usr/bin/env python3
"""
Example: Advanced NLM Usage

This file demonstrates advanced usage patterns and best practices for the NLM Python API.
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt
import json
import time
from typing import List, Dict, Optional, Callable, Any

def create_advanced_brain(config_overrides: Optional[Dict[str, Any]] = None):
    """
    Create an advanced brain with specific configuration.
    
    Args:
        config_overrides: Dictionary of configuration parameter overrides
        
    Returns:
        Configured and initialized brain instance
    """
    config = pynlm.createDefaultConfig()
    
    # Apply common advanced configurations
    if config_overrides:
        for key, value in config_overrides.items():
            config.set(key, value)
    
    # Set some sensible defaults for advanced usage
    if not config.has("brain.neuron_count"):
        config.set("brain.neuron_count", 5000)
    if not config.has("brain.synapse_density"):
        config.set("brain.synapse_density", 0.1)
    if not config.has("brain.region_count"):
        config.set("brain.region_count", 10)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain

def create_training_agent(brain, world, enable_learning=True):
    """
    Create a complete training agent with all learning systems enabled.
    
    Args:
        brain: The neural brain
        world: The simulation world
        enable_learning: Whether to enable learning systems
        
    Returns:
        Configured agent brain interface
    """
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    if enable_learning:
        # Enable all learning systems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    
    return agent

def run_episode(brain, world, agent, num_steps: int, 
               with_reward: bool = True, with_curiosity: bool = False):
    """
    Run a complete episode with the given agent.
    
    Args:
        brain: Neural brain
        world: Simulation world
        agent: Agent brain interface
        num_steps: Number of simulation steps
        with_reward: Whether to apply reward modulation
        with_curiosity: Whether to track curiosity
        
    Returns:
        Dictionary containing episode statistics
    """
    episode_data = {
        'steps': num_steps,
        'initial_neurons': brain.getTotalNeuronCount(),
        'initial_spikes': brain.getTotalSpikeCount(),
        'firing_rates': [],
        'rewards': [],
        'curiosity_levels': [],
        'actions_taken': 0,
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
        
        # Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward if enabled
        if with_reward:
            # Simple reward based on action success
            reward = 0.1  # Small positive reward
            predicted = 0.05  # Predicted reward
            agent.applyRewardModulation(reward, predicted)
            episode_data['rewards'].append(reward)
        
        # Track curiosity if enabled
        if with_curiosity:
            curiosity = agent.getCuriosityLevel()
            episode_data['curiosity_levels'].append(curiosity)
        
        # Record firing rate
        firing_rate = brain.getAverageFiringRate()
        episode_data['firing_rates'].append(firing_rate)
        
        # Count actions
        if action != pynlm.MotorCommand.Wait:
            episode_data['actions_taken'] += 1
    
    episode_data['final_neurons'] = brain.getTotalNeuronCount()
    episode_data['final_spikes'] = brain.getTotalSpikeCount()
    
    return episode_data

def train_agent(training_episodes: int, brain_config: Optional[Dict] = None,
               render_interval: int = 10):
    """
    Train an agent using the NLM framework.
    
    Args:
        training_episodes: Number of training episodes
        brain_config: Brain configuration parameters
        render_interval: Render interval for visualization
        
    Returns:
        Trained brain and training statistics
    """
    # Create brain
    brain = create_advanced_brain(brain_config)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    # Create agent
    agent = create_training_agent(brain, world, enable_learning=True)
    
    # Training loop
    training_stats = {
        'episode': [],
        'total_spikes': [],
        'average_firing_rate': [],
        'total_actions': [],
        'final_development_stage': [],
    }
    
    for episode in range(training_episodes):
        episode_data = run_episode(
            brain, world, agent, 
            num_steps=100,
            with_reward=True,
            with_curiosity=True
        )
        
        # Record statistics
        training_stats['episode'].append(episode)
        training_stats['total_spikes'].append(episode_data['final_spikes'] - episode_data['initial_spikes'])
        training_stats['average_firing_rate'].append(np.mean(episode_data['firing_rates']))
        training_stats['total_actions'].append(episode_data['actions_taken'])
        training_stats['final_development_stage'].append(agent.getDevelopmentalStage())
        
        # Reset for next episode
        brain.reset()
        world.reset()
        agent.reset()
        
        # Print progress
        if episode % render_interval == 0:
            print(f"Episode {episode}: "
                  f"Spikes: {episode_data['final_spikes'] - episode_data['initial_spikes']}, "
                  f"Firing Rate: {np.mean(episode_data['firing_rates']):.3f}")
    
    return brain, training_stats

def save_training_results(brain, stats: Dict, filename: str):
    """
    Save training results to file.
    
    Args:
        brain: Trained brain
        stats: Training statistics
        filename: Output filename
    """
    results = {
        'brain_state': {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'developmental_stage': brain.getDevelopmentalStage(),
        },
        'training_stats': stats
    }
    
    with open(filename, 'w') as f:
        json.dump(results, f, indent=2)

def plot_training_stats(stats: Dict):
    """
    Plot training statistics.
    
    Args:
        stats: Training statistics dictionary
    """
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Plot 1: Total spikes per episode
    axes[0, 0].plot(stats['episode'], stats['total_spikes'])
    axes[0, 0].set_title('Total Spikes per Episode')
    axes[0, 0].set_xlabel('Episode')
    axes[0, 0].set_ylabel('Spikes')
    axes[0, 0].grid(True)
    
    # Plot 2: Average firing rate per episode
    axes[0, 1].plot(stats['episode'], stats['average_firing_rate'])
    axes[0, 1].set_title('Average Firing Rate per Episode')
    axes[0, 1].set_xlabel('Episode')
    axes[0, 1].set_ylabel('Firing Rate')
    axes[0, 1].grid(True)
    
    # Plot 3: Total actions per episode
    axes[1, 0].plot(stats['episode'], stats['total_actions'])
    axes[1, 0].set_title('Total Actions per Episode')
    axes[1, 0].set_xlabel('Episode')
    axes[1, 0].set_ylabel('Actions')
    axes[1, 0].grid(True)
    
    # Plot 4: Development stage per episode
    stage_names = ['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging']
    stage_values = [pynlm.DevelopmentalStage.Initial, pynlm.DevelopmentalStage.CriticalPeriod,
                   pynlm.DevelopmentalStage.Maturation, pynlm.DevelopmentalStage.Adult,
                   pynlm.DevelopmentalStage.Aging]
    
    # Count development stages
    stage_counts = {stage: 0 for stage in stage_values}
    for stage in stats['final_development_stage']:
        if stage in stage_counts:
            stage_counts[stage] += 1
    
    axes[1, 1].bar(stage_names, [stage_counts[stage] for stage in stage_values])
    axes[1, 1].set_title('Development Stage Distribution')
    axes[1, 1].set_xlabel('Development Stage')
    axes[1, 1].set_ylabel('Count')
    axes[1, 1].tick_params(axis='x', rotation=45)
    
    plt.tight_layout()
    plt.show()

def create_neural_network_visualization(brain, output_file: str = "brain_network.png"):
    """
    Create a visualization of the neural network.
    
    Args:
        brain: Neural brain to visualize
        output_file: Output filename
    """
    try:
        import networkx as nx
        import matplotlib.pyplot as plt
        
        # Get all regions
        regions = brain.getRegions()
        region_ids = brain.getRegionIds()
        
        # Create a network graph
        G = nx.Graph()
        
        # Add nodes for regions
        for region_id in region_ids:
            region = brain.getRegion(region_id)
            if region:
                G.add_node(region_id, label=region.getName())
        
        # Get neuron connections (simplified for visualization)
        total_neurons = brain.getTotalNeuronCount()
        
        # Create a simple layout
        pos = nx.spring_layout(G, k=2, iterations=50)
        
        # Draw the graph
        plt.figure(figsize=(12, 8))
        
        # Draw nodes
        nx.draw_networkx_nodes(G, pos, node_color='lightblue', 
                              node_size=500, alpha=0.8)
        
        # Draw edges
        nx.draw_networkx_edges(G, pos, alpha=0.5, width=1)
        
        # Draw labels
        labels = nx.get_node_attributes(G, 'label')
        nx.draw_networkx_labels(G, pos, labels, font_size=8)
        
        # Add title and info
        plt.title(f'NLM Neural Network Visualization\n'
                 f'Total Neurons: {total_neurons}, '
                 f'Total Regions: {len(regions)}')
        
        plt.axis('off')
        plt.tight_layout()
        plt.savefig(output_file, dpi=150, bbox_inches='tight')
        plt.close()
        
        print(f"Neural network visualization saved to {output_file}")
        
    except ImportError:
        print("networkx or matplotlib not available for visualization")

if __name__ == "__main__":
    print("Advanced NLM Usage Examples")
    print("=" * 50)
    
    # Example 1: Basic brain creation
    print("\n1. Creating an advanced brain...")
    brain = create_advanced_brain({
        "brain.neuron_count": 2000,
        "brain.synapse_density": 0.05
    })
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Example 2: Training loop
    print("\n2. Training agent...")
    trained_brain, stats = train_agent(
        training_episodes=5,
        brain_config={"brain.neuron_count": 1000},
        render_interval=1
    )
    
    # Save results
    save_training_results(trained_brain, stats, "training_results.json")
    
    # Plot statistics
    plot_training_stats(stats)
    
    # Visualize network
    create_neural_network_visualization(trained_brain)
    
    print("\nAdvanced examples completed successfully!")
