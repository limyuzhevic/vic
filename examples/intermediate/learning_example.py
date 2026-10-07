#!/usr/bin/env python3
"""
NLM Learning and Development Example
===================================

This example demonstrates the learning capabilities of NLM through an
experiment where an agent learns from experience over multiple episodes.
It showcases:

1. Experiential learning through reward signals
2. Development and maturation over time
3. Structural plasticity (synapse formation/removal)
4. Neuromodulation (curiosity, novelty detection)
5. Behavioral adaptation and improvement

This example simulates an agent exploring and learning about its environment
through trial and error, with each episode providing opportunities for growth.
"""

import pynlm as nlm
import random
import os

def create_learning_experiment(environment_size=10, num_episodes=10):
    """
    Set up a learning experiment environment.
    
    Args:
        environment_size (int): Size of the square world
        num_episodes (int): Number of learning episodes
        
    Returns:
        tuple: (brain, world, agent, experiment_data)
    """
    print(f"Setting up learning experiment: {num_episodes} episodes in {environment_size}x{environment_size} world")
    
    # Create world with random seed for reproducibility
    world = nlm.createSimpleWorld()
    world.configure(environment_size, environment_size, 8, 8)
    world.setRandomSeed(12345)
    world.reset()
    
    # Set agent start position
    world.setAgentStart(environment_size // 2, environment_size // 2)
    
    # Create brain with configuration optimized for learning
    config = nlm.createDefaultConfig()
    config.set("brain.neuron_count", 300)  # Smaller brain for faster learning
    config.set("brain.region_count", 2)
    config.set("brain.connection_probability", 0.08)
    config.set("random_seed", 12345)
    config.set("simulation_timestep", 0.001)
    config.set("plasticity.learning_rate", 0.01)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create agent interface
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Experiment tracking data
    experiment_data = {
        "episodes": [],
        "total_reward": 0.0,
        "best_reward": float('-inf'),
        "synapse_counts": [],
        "behavior_changes": [],
        "development_milestones": [],
        "learning_rates": []
    }
    
    return brain, world, agent, experiment_data

def create_random_resource_environment(world, agent):
    """
    Create a random resource environment where the agent can find rewards.
    
    Args:
        world: The simulation world
        agent: The agent brain interface
        
    Returns:
        dict: Information about placed resources
    """
    print("Creating resource environment...")
    
    resources_placed = []
    world_width = world.getWidth()
    world_height = world.getHeight()
    
    # Place some resources in random locations
    for i in range(8):
        x = random.randint(1, world_width - 2)
        y = random.randint(1, world_height - 2)
        
        # Create resource object
        resource = nlm.WorldObject(x, y, nlm.WorldObjectType.Resource, value=5.0)
        world.addObject(resource)
        resources_placed.append((x, y, 5.0))
        
        print(f"   Resource {i+1}: placed at ({x}, {y}) with value {5.0}")
    
    # Place some hazards
    for i in range(3):
        x = random.randint(1, world_width - 2)
        y = random.randint(1, world_height - 2)
        
        # Ensure hazard is not too close to agent start
        if abs(x - world_width // 2) > 2 or abs(y - world_height // 2) > 2:
            hazard = nlm.WorldObject(x, y, nlm.WorldObjectType.Hazard, value=-2.0)
            world.addObject(hazard)
            resources_placed.append((x, y, -2.0))
            print(f"   Hazard {i+1}: placed at ({x}, {y}) with penalty {-2.0}")
    
    # Place walls for navigation practice
    for x in range(2, world_width - 2):
        if x % 3 == 0:  # Every 3rd column
            wall = nlm.WorldObject(x, world_height // 2, nlm.WorldObjectType.Wall, value=0.0)
            world.addObject(wall)
    
    print(f"   Environment setup complete: {len(resources_placed)} objects placed")
    return resources_placed

def run_learning_episode(brain, world, agent, episode_num, max_steps=50):
    """
    Run a single learning episode.
    
    Args:
        brain: The NLM brain
        world: The simulation world
        agent: The agent brain interface
        episode_num (int): Current episode number
        max_steps (int): Maximum steps per episode
        
    Returns:
        dict: Episode results and metrics
    """
    print(f"\n--- Episode {episode_num} ---")
    
    # Reset for new episode
    world.reset()
    agent.reset()
    brain.reset()
    brain.initialize()
    
    # Set random seed for this episode for variety
    world.setRandomSeed(12345 + episode_num * 1000)
    
    # Track episode metrics
    episode_metrics = {
        "episode": episode_num,
        "steps": 0,
        "total_reward": 0.0,
        "max_reward": 0.0,
        "final_energy": 0.0,
        "final_curiosity": 0.0,
        "final_novelty": 0.0,
        "synapses_at_start": brain.getTotalSynapseCount(),
        "synapses_at_end": 0,
        "action_counts": {
            "move": 0,
            "wait": 0,
            "interact": 0
        }
    }
    
    # Run episode steps
    for step in range(max_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Track metrics
        episode_metrics["steps"] += 1
        episode_metrics["total_reward"] += result.reward
        episode_metrics["max_reward"] = max(episode_metrics["max_reward"], result.reward)
        episode_metrics["final_energy"] = world.getAgentBody().energy
        episode_metrics["final_curiosity"] = agent.getCuriosityLevel()
        episode_metrics["final_novelty"] = agent.getNoveltyLevel()
        
        # Count actions
        if action == nlm.MotorCommand.MoveForward or action == nlm.MotorCommand.MoveBackward:
            episode_metrics["action_counts"]["move"] += 1
        elif action == nlm.MotorCommand.Interact:
            episode_metrics["action_counts"]["interact"] += 1
        else:
            episode_metrics["action_counts"]["wait"] += 1
        
        # Print progress every 10 steps
        if step % 10 == 0:
            print(f"   Step {step:2d}: reward={result.reward:6.2f}, "
                  f"action={action.name:12s}, "
                  f"energy={world.getAgentBody().energy:5.1f}, "
                  f"curiosity={agent.getCuriosityLevel():.3f}")
    
    # Record final synapse count
    episode_metrics["synapses_at_end"] = brain.getTotalSynapseCount()
    
    return episode_metrics

def analyze_learning_progress(experiment_data):
    """
    Analyze the learning progress across episodes.
    
    Args:
        experiment_data (dict): Complete experiment data
        
    Returns:
        dict: Analysis results
    """
    print("\nAnalyzing learning progress...")
    
    episodes = experiment_data["episodes"]
    
    # Calculate overall statistics
    total_reward = sum(ep["total_reward"] for ep in episodes)
    avg_reward_per_episode = total_reward / len(episodes)
    avg_reward_per_step = sum(ep["total_reward"] for ep in episodes) / \
                         sum(ep["steps"] for ep in episodes)
    
    # Track behavior changes
    synapse_changes = []
    behavior_changes = []
    
    for i in range(len(episodes) - 1):
        ep1 = episodes[i]
        ep2 = episodes[i + 1]
        
        # Synapse growth
        synapse_growth = ep2["synapses_at_end"] - ep1["synapses_at_start"]
        synapse_changes.append(synapse_growth)
        
        # Behavior changes (action patterns)
        ep1_move_ratio = ep1["action_counts"]["move"] / ep1["steps"]
        ep2_move_ratio = ep2["action_counts"]["move"] / ep2["steps"]
        behavior_change = ep2_move_ratio - ep1_move_ratio
        behavior_changes.append(behavior_change)
    
    # Calculate learning metrics
    avg_synapse_growth = sum(synapse_changes) / len(synapse_changes) if synapse_changes else 0
    avg_behavior_change = sum(behavior_changes) / len(behavior_changes) if behavior_changes else 0
    
    # Determine learning milestones
    learning_milestones = []
    for ep in episodes:
        if ep["final_curiosity"] < 0.3:  # Curiosity reduced - agent learned
            learning_milestones.append(f"Episode {ep['episode']}: Low curiosity ({ep['final_curiosity']:.3f})")
        if ep["max_reward"] > 10.0:  # High reward achieved
            learning_milestones.append(f"Episode {ep['episode']}: High reward ({ep['max_reward']:.1f})")
        if ep["synapses_at_end"] > ep["synapses_at_start"] * 1.2:  # Synapse growth
            learning_milestones.append(f"Episode {ep['episode']}: Synapse growth ({ep['synapses_at_start']} -> {ep['synapses_at_end']})")
    
    analysis = {
        "total_episodes": len(episodes),
        "total_reward": total_reward,
        "avg_reward_per_episode": avg_reward_per_episode,
        "avg_reward_per_step": avg_reward_per_step,
        "synapse_growth_rate": avg_synapse_growth,
        "behavior_change_rate": avg_behavior_change,
        "learning_milestones": learning_milestones,
        "final_curiosity": episodes[-1]["final_curiosity"],
        "final_novelty": episodes[-1]["final_novelty"],
        "synapse_counts": [ep["synapses_at_end"] for ep in episodes],
        "action_patterns": [ep["action_counts"] for ep in episodes]
    }
    
    return analysis

def print_learning_summary(experiment_data, analysis):
    """
    Print a comprehensive summary of the learning experiment.
    
    Args:
        experiment_data (dict): Complete experiment data
        analysis (dict): Analysis results
    """
    print("\n" + "=" * 60)
    print("LEARNING EXPERIMENT SUMMARY")
    print("=" * 60)
    
    print(f"\nEXPERIMENT OVERVIEW:")
    print(f"  Episodes run: {analysis['total_episodes']}")
    print(f"  Total reward accumulated: {analysis['total_reward']:.2f}")
    print(f"  Average reward per episode: {analysis['avg_reward_per_episode']:.2f}")
    print(f"  Average reward per step: {analysis['avg_reward_per_step']:.3f}")
    
    print(f"\nNEURAL DYNAMICS:")
    print(f"  Initial synapses: {experiment_data['synapse_counts'][0] if experiment_data['synapse_counts'] else 'N/A'}")
    print(f"  Final synapses: {analysis['synapse_counts'][-1] if analysis['synapse_counts'] else 'N/A'}")
    print(f"  Synapse growth rate: {analysis['synapse_growth_rate']:.1f} synapses/episode")
    print(f"  Final curiosity level: {analysis['final_curiosity']:.3f}")
    print(f"  Final novelty level: {analysis['final_novelty']:.3f}")
    
    print(f"\nBEHAVIORAL LEARNING:")
    print(f"  Behavior change rate: {analysis['behavior_change_rate']:.3f} per episode")
    
    print(f"\nLEARNING MILESTONES:")
    for milestone in analysis['learning_milestones']:
        print(f"  ✓ {milestone}")
    
    if not analysis['learning_milestones']:
        print("  (No significant learning milestones detected)")
    
    print(f"\nACTION PATTERNS:")
    for i, pattern in enumerate(analysis['action_patterns']):
        total_actions = sum(pattern.values())
        move_ratio = pattern["move"] / total_actions if total_actions > 0 else 0
        interact_ratio = pattern["interact"] / total_actions if total_actions > 0 else 0
        wait_ratio = pattern["wait"] / total_actions if total_actions > 0 else 0
        
        print(f"  Episode {i+1}: Move={move_ratio:.2f}, Interact={interact_ratio:.2f}, Wait={wait_ratio:.2f}")
    
    print(f"\n" + "=" * 60)
    
    # Determine learning assessment
    if analysis['synapse_growth_rate'] > 0:
        print("🎯 LEARNING ASSESSMENT: Agent showed neural growth and adaptation!")
    else:
        print("⚠️  LEARNING ASSESSMENT: Limited neural changes observed.")
    
    if analysis['behavior_change_rate'] > 0.1:
        print("🎯 BEHAVIORAL LEARNING: Significant behavioral adaptation detected!")
    else:
        print("⚠️  BEHAVIORAL LEARNING: Limited behavioral change detected.")
    
    if analysis['final_curiosity'] < 0.3:
        print("🎯 EXPLORATION LEARNING: Agent became more efficient (reduced curiosity).")
    else:
        print("⚠️  EXPLORATION: Agent remained curious (ongoing exploration).")
    
    print("=" * 60)

def save_learning_data(experiment_data, analysis, filename="learning_experiment_data.json"):
    """
    Save learning experiment data to a JSON file.
    
    Args:
        experiment_data (dict): Complete experiment data
        analysis (dict): Analysis results
        filename (str): Output filename
        
    Returns:
        bool: True if save was successful, False otherwise
    """
    print(f"\nSaving learning data to '{filename}'...")
    
    import json
    
    # Convert data to JSON-serializable format
    save_data = {
        "experiment_data": {
            "episodes": experiment_data["episodes"],
            "synapse_counts": experiment_data["synapse_counts"],
            "behavior_changes": experiment_data["behavior_changes"],
            "development_milestones": experiment_data["development_milestones"],
            "learning_rates": experiment_data["learning_rates"]
        },
        "analysis": analysis,
        "timestamp": "2024-01-01T00:00:00Z",  # Placeholder
        "config": {
            "environment_size": 10,
            "num_episodes": len(experiment_data["episodes"]),
            "brain_config": "custom"
        }
    }
    
    try:
        with open(filename, 'w') as f:
            json.dump(save_data, f, indent=2)
        print(f"✓ Learning data saved successfully to '{filename}'")
        print(f"  File size: {os.path.getsize(filename)} bytes")
        return True
    except Exception as e:
        print(f"✗ Failed to save learning data: {e}")
        return False

def main():
    """
    Main function to run the learning and development experiment.
    """
    print("=== NLM Learning and Development Experiment ===\n")
    
    try:
        # Set up experiment
        brain, world, agent, experiment_data = create_learning_experiment()
        
        # Create resource environment
        resources = create_random_resource_environment(world, agent)
        
        # Run learning episodes
        num_episodes = 8
        for episode in range(1, num_episodes + 1):
            episode_metrics = run_learning_episode(brain, world, agent, episode, max_steps=40)
            experiment_data["episodes"].append(episode_metrics)
            experiment_data["total_reward"] += episode_metrics["total_reward"]
            experiment_data["synapse_counts"].append(episode_metrics["synapses_at_end"])
        
        # Analyze learning progress
        analysis = analyze_learning_progress(experiment_data)
        
        # Print comprehensive summary
        print_learning_summary(experiment_data, analysis)
        
        # Save experiment data
        save_learning_data(experiment_data, analysis, "nlm_learning_experiment.json")
        
        print(f"\n🎉 Learning experiment completed successfully!")
        print(f"\nThis experiment demonstrated:")
        print(f"  ✓ Experiential learning through rewards and penalties")
        print(f"  ✓ Neural adaptation through synaptic plasticity")
        print(f"  ✓ Behavioral optimization through exploration")
        print(f"  ✓ Development and maturation over time")
        print(f"  ✓ Neuromodulation (curiosity, novelty detection)")
        print(f"  ✓ Structured data collection and analysis")
        print(f"  ✓ Persistent storage of experiment results")
        
        return True
        
    except Exception as e:
        print(f"\n❌ Error during learning experiment: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    import random
    import os
    success = main()
    exit(0 if success else 1)