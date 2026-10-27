#!/usr/bin/env python3
"""
NLM Advanced Learning Agent - Comprehensive Example
===================================================

This example demonstrates advanced usage of the NLM framework for training
a sophisticated agent with curiosity-driven exploration, reward modulation,
and developmental learning.
"""

import pynlm
import time
import json
import os

def create_advanced_learning_config():
    """Create an advanced learning configuration with multiple features enabled."""
    config = pynlm.createDefaultConfig()
    
    # Advanced learning parameters
    config.set("brain.neuron_count", 3000)
    config.set("brain.region_count", 4)
    config.set("brain.connection_probability", 0.12)
    
    # Enable all plasticity mechanisms
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.005)
    config.set("plasticity.hebbian.learning_rate", 0.003)
    
    # Advanced neuromodulation
    config.set("neuromod.dopamine.scale", 0.8)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.prediction.enable", True)
    config.set("neuromod.prediction.error_threshold", 0.3)
    
    # Development and adaptation
    config.set("development.enable", True)
    config.set("development.stage_changes", True)
    config.set("development.plasticity_decay_rate", 0.02)
    
    # Environmental interaction
    config.set("world.max_energy", 150.0)
    config.set("world.energy_decay_rate", 0.95)
    config.set("world.enable_rewards", True)
    config.set("world.reward_scale", 1.0)
    
    # Memory and learning
    config.set("memory.working_capacity", 100)
    config.set("memory.episodic_capacity", 50)
    config.set("memory.enable_replay", True)
    config.set("memory.replay_interval", 200)
    
    return config

def run_advanced_training_episode(brain, world, agent, episode_id):
    """Run a single advanced training episode with comprehensive monitoring."""
    print(f"  Episode {episode_id}: Starting advanced training...")
    
    episode_reward = 0.0
    episode_spikes = 0
    episode_steps = 0
    max_firing_rate = 0.0
    
    for step in range(100):  # 100 steps per episode
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input through agent
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning signals
        if agent.isRewardModulationEnabled():
            # Get reward from world
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            if reward == 0.0:
                reward = 0.3  # Small baseline reward
            
            # Get predicted reward from agent's prediction system
            predicted_reward = agent.getPredictionError() if agent.getPredictionError() > 0 else 0.1
            
            agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development system
        if agent.isDevelopmentEnabled():
            agent.updateDevelopment(0.1)
        
        # Collect statistics
        episode_reward += (reward if 'reward' in locals() else 0.0)
        episode_spikes += brain.getTotalSpikeCount()
        episode_steps += 1
        
        # Track maximum firing rate
        firing_rate = brain.getAverageFiringRate()
        if firing_rate > max_firing_rate:
            max_firing_rate = firing_rate
        
        # Log progress every 20 steps
        if step % 20 == 0:
            print(f"    Step {step}: Reward={reward:.2f}, Firing Rate={firing_rate:.2f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, Novelty={agent.getNoveltyLevel():.3f}")
    
    # Create episode summary
    summary = {
        "episode_id": episode_id,
        "total_reward": episode_reward,
        "average_reward_per_step": episode_reward / episode_steps if episode_steps > 0 else 0,
        "total_spikes": episode_spikes,
        "average_firing_rate": episode_spikes / episode_steps if episode_steps > 0 else 0,
        "max_firing_rate": max_firing_rate,
        "curiosity_level": agent.getCuriosityLevel(),
        "novelty_level": agent.getNoveltyLevel(),
        "prediction_error": agent.getPredictionError(),
        "developmental_stage": agent.getDevelopmentalStage(),
        "neuromodulation_level": agent.getNeuromodulationLevel()
    }
    
    print(f"  Episode {episode_id}: Completed - Reward={episode_reward:.2f}, "
          f"Firing Rate={summary['average_firing_rate']:.2f}")
    
    return summary

def advanced_agent_training(config_file="advanced_config.json"):
    """Run advanced agent training with comprehensive monitoring and analysis."""
    print("=" * 60)
    print("NLM ADVANCED AGENT TRAINING")
    print("=" * 60)
    
    # Load or create configuration
    config = pynlm.createDefaultConfig()
    
    if os.path.exists(config_file):
        print(f"Loading configuration from {config_file}")
        config.loadFromFile(config_file)
    else:
        print("Creating advanced configuration...")
        # Create and save advanced config
        config = pynlm.createTrainingConfig(100, 1000)
        config.set("brain.neuron_count", 3000)
        config.saveToFile(config_file)
        print(f"Saved advanced configuration to {config_file}")
    
    # Create brain with advanced configuration
    print("\nCreating advanced neural substrate...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    print("Setting up interactive environment...")
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"\nAdvanced agent initialized with:")
    print(f"  - {brain.getTotalNeuronCount()} neurons")
    print(f"  - {brain.getTotalSynapseCount()} synapses")
    print(f"  - {brain.getRegionCount()} brain regions")
    print(f"  - Agent enabled features: Reward={agent.isRewardModulationEnabled()}, "
          f"Plasticity={agent.isStructuralPlasticityEnabled()}, "
          f"Development={agent.isDevelopmentEnabled()}, "
          f"Curiosity={agent.isCuriosityEnabled()}")
    
    # Run training episodes
    all_episodes = []
    learning_curve = []
    
    num_episodes = 50
    for episode in range(num_episodes):
        episode_summary = run_advanced_training_episode(brain, world, agent, episode)
        all_episodes.append(episode_summary)
        learning_curve.append(episode_summary["average_reward_per_step"])
        
        # Save checkpoint every 10 episodes
        if episode % 10 == 0:
            checkpoint_file = f"checkpoint_episode_{episode}.bin"
            brain.save(checkpoint_file)
            print(f"  Saved checkpoint to {checkpoint_file}")
    
    # Analysis and reporting
    print("\n" + "=" * 60)
    print("TRAINING ANALYSIS")
    print("=" * 60)
    
    # Calculate statistics
    total_rewards = [ep["total_reward"] for ep in all_episodes]
    avg_rewards = [ep["average_reward_per_step"] for ep in all_episodes]
    max_rewards = [ep["total_reward"] for ep in all_episodes]
    
    final_stats = {
        "total_episodes": num_episodes,
        "final_average_reward": sum(avg_rewards) / len(avg_rewards),
        "peak_reward": max(max_rewards),
        "total_learning_gain": avg_rewards[-10:] - avg_rewards[:-10] if len(avg_rewards) >= 20 else 0,
        "final_curiosity": all_episodes[-1]["curiosity_level"],
        "final_novelty": all_episodes[-1]["novelty_level"],
        "final_prediction_error": all_episodes[-1]["prediction_error"],
        "developmental_stage": all_episodes[-1]["developmental_stage"],
        "final_neuromodulation": all_episodes[-1]["neuromodulation_level"]
    }
    
    print(f"Training completed over {num_episodes} episodes:")
    print(f"  Average reward per episode: {final_stats['final_average_reward']:.2f}")
    print(f"  Peak episode reward: {final_stats['peak_reward']:.2f}")
    print(f"  Final curiosity level: {final_stats['final_curiosity']:.3f}")
    print(f"  Final novelty level: {final_stats['final_novelty']:.3f}")
    print(f"  Final prediction error: {final_stats['final_prediction_error']:.3f}")
    print(f"  Developmental stage: {final_stats['developmental_stage']}")
    print(f"  Final neuromodulation level: {final_stats['final_neuromodulation']:.3f}")
    
    # Save training results
    results = {
        "episodes": all_episodes,
        "learning_curve": learning_curve,
        "final_stats": final_stats,
        "config_file": config_file
    }
    
    with open("training_results.json", "w") as f:
        json.dump(results, f, indent=2)
    
    print(f"\nTraining results saved to training_results.json")
    
    # Create visualization data
    viz_data = {
        "firing_rates": [ep["average_firing_rate"] for ep in all_episodes],
        "rewards": [ep["total_reward"] for ep in all_episodes],
        "curiosity_levels": [ep["curiosity_level"] for ep in all_episodes],
        "novelty_levels": [ep["novelty_level"] for ep in all_episodes]
    }
    
    with open("training_viz_data.json", "w") as f:
        json.dump(viz_data, f, indent=2)
    
    print(f"Visualization data saved to training_viz_data.json")
    
    return all_episodes, final_stats

def curiosity_driven_exploration(config):
    """Demonstrate curiosity-driven exploration capabilities."""
    print("\n" + "=" * 60)
    print("CURIOSITY-DRIVEN EXPLORATION DEMONSTRATION")
    print("=" * 60)
    
    # Create agent setup
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable curiosity for exploration
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    
    print("Running curiosity-driven exploration...")
    
    exploration_results = []
    for episode in range(10):
        steps = 0
        total_reward = 0.0
        novel_actions = 0
        
        while steps < 100 and novel_actions < 5:  # Stop after 5 novel actions
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(steps)
            
            action = agent.decodeMotorCommand()
            
            # Check if this is a novel action
            prev_actions = exploration_results[-10:] if exploration_results else []
            is_novel = action not in prev_actions
            if is_novel:
                novel_actions += 1
                print(f"    Novel action at step {steps}: {action}")
            
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            if reward == 0.0:
                reward = 0.5
            total_reward += reward
            
            steps += 1
        
        exploration_results.append({
            "episode": episode,
            "steps": steps,
            "total_reward": total_reward,
            "novel_actions": novel_actions,
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel()
        })
        
        print(f"  Episode {episode}: Steps={steps}, Reward={total_reward:.2f}, "
              f"Novel={novel_actions}, Curiosity={agent.getCuriosityLevel():.3f}")
    
    return exploration_results

def memory_based_learning(config):
    """Demonstrate memory-based learning and recall."""
    print("\n" + "=" * 60)
    print("MEMORY-BASED LEARNING DEMONSTRATION")
    print("=" * 60)
    
    # Create agent setup
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all memory systems
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    print("Running memory-based learning demonstration...")
    
    memory_results = []
    for episode in range(15):
        episode_data = {
            "episode": episode,
            "steps": 0,
            "rewards": [],
            "locations": [],
            "actions": [],
            "memory_usage": 0,
            "developmental_stage": 0
        }
        
        for step in range(50):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            if reward == 0.0:
                reward = 0.3
            
            episode_data["rewards"].append(reward)
            episode_data["locations"].append((world.getAgentBody().x, world.getAgentBody().y))
            episode_data["actions"].append(action)
            episode_data["memory_usage"] = brain.getTotalSpikeCount()
            episode_data["developmental_stage"] = agent.getDevelopmentalStage()
            
            if agent.isRewardModulationEnabled():
                agent.applyRewardModulation(reward, agent.getPredictionError())
            
            if agent.isDevelopmentEnabled():
                agent.updateDevelopment(0.1)
            
            episode_data["steps"] += 1
        
        memory_results.append(episode_data)
        
        print(f"  Episode {episode}: Steps={episode_data['steps']}, "
              f"Avg Reward={sum(episode_data['rewards'])/len(episode_data['rewards']):.2f}, "
              f"Dev Stage={episode_data['developmental_stage']}")
    
    return memory_results

def main():
    """Main function demonstrating comprehensive NLM usage."""
    print("NLM Advanced Learning Framework - Comprehensive Examples")
    print("=" * 60)
    
    # Example 1: Advanced Agent Training
    print("\n1. ADVANCED AGENT TRAINING")
    print("-" * 60)
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("development.enable", True)
    config.set("neuromod.curiosity.enable", True)
    
    episodes, stats = advanced_agent_training(config)
    
    # Example 2: Curiosity-Driven Exploration
    print("\n2. CURIOSITY-DRIVEN EXPLORATION")
    print("-" * 60)
    curiosity_results = curiosity_driven_exploration(config)
    
    # Example 3: Memory-Based Learning
    print("\n3. MEMORY-BASED LEARNING")
    print("-" * 60)
    memory_results = memory_based_learning(config)
    
    # Summary
    print("\n" + "=" * 60)
    print("COMPREHENSIVE EXAMPLE SUMMARY")
    print("=" * 60)
    print(f"✓ Advanced agent training completed: {len(episodes)} episodes")
    print(f"✓ Curiosity-driven exploration completed: {len(curiosity_results)} episodes")
    print(f"✓ Memory-based learning completed: {len(memory_results)} episodes")
    print(f"✓ Final agent performance: Reward={stats['final_average_reward']:.2f}, "
          f"Curiosity={stats['final_curiosity']:.3f}")
    print("\nAll examples demonstrate the full capabilities of the NLM framework:")
    print("  - Advanced neural computation with real-time learning")
    print("  - Neuromodulation-based motivation and exploration")
    print("  - Developmental adaptation over time")
    print("  - Memory-based skill acquisition")
    print("  - Comprehensive monitoring and analysis")
    print("  - Configurable behavior through advanced settings")
    
    # Save example results
    example_results = {
        "advanced_training": stats,
        "curiosity_exploration": curiosity_results,
        "memory_learning": memory_results
    }
    
    with open("comprehensive_example_results.json", "w") as f:
        json.dump(example_results, f, indent=2)
    
    print(f"\nComprehensive example results saved to comprehensive_example_results.json")
    
    print("\n" + "=" * 60)
    print("EXAMPLE COMPLETED SUCCESSFULLY")
    print("=" * 60)

if __name__ == "__main__":
    main()