#!/usr/bin/env python3
"""
example_agent_simulation.py

Complete Agent Simulation Example
================================

This example demonstrates a complete agent simulation using the NLM Python API,
showcasing how an agent processes sensory input, makes decisions, and learns from
its environment through interaction.

Key Concepts Covered:
- Complete agent setup (brain + agent)
- Sensory-motor loop integration
- Reward-based learning
- Episode management
- Performance tracking
- Learning curve analysis

Learning Objectives:
1. Set up a complete agent simulation environment
2. Implement sensory processing loop
3. Manage agent-environment interaction
4. Track and analyze learning progress
5. Implement episode-based training

Author: NLM Documentation Team
Date: 2026-10-12
"""

import pynlm as nlm
import time
import matplotlib.pyplot as plt
from typing import List, Tuple

def create_agent_simulation_config():
    """Create an optimal configuration for agent simulation."""
    config = nlm.createLearningConfig()
    nlm.enableLearningMode(config)
    return config

def setup_simulation_environment():
    """Set up the training environment with resources and hazards."""
    world = nlm.createTrainingEnvironment()
    
    # Setup agent start position
    nlm.setupAgentStartPosition(world, x=50.0, y=50.0, orientation=0.0)
    
    # Add training resources
    nlm.spawnResource(world, x=20.0, y=20.0, value=10.0)
    nlm.spawnResource(world, x=80.0, y=20.0, value=15.0)
    nlm.spawnResource(world, x=20.0, y=80.0, value=20.0)
    
    # Add hazards for risk assessment
    nlm.createHazard(world, x=60.0, y=60.0, damage=5.0)
    
    return world

def create_and_setup_agent(world):
    """Create and configure an agent for learning."""
    # Create brain optimized for learning
    brain = nlm.createDefaultBrain()
    nlm.setupBrainForLearning(brain, world)
    
    # Create learning agent with neuromodulation
    agent = nlm.createLearningAgent(brain)
    
    # Setup neuromodulation systems
    nlm.setupRewardModulation(agent, learningRate=0.1)
    nlm.setupCuriosityModulation(agent, noveltyWeight=0.7, errorWeight=0.3)
    nlm.setupPredictionErrorModulation(agent, precisionWeight=0.5)
    
    # Enable development for enhanced learning
    nlm.enableDevelopmentMode(brain)
    
    return brain, agent

def run_episode(agent, world, brain, max_steps=200, verbose=True):
    """
    Run a single episode of agent simulation.
    
    Args:
        agent: The learning agent
        world: The environment
        brain: The agent's brain
        max_steps: Maximum steps per episode
        verbose: Whether to print step-by-step progress
        
    Returns:
        Tuple of (episode_stats, reward_history, curiosity_history)
    """
    episode_stats = {
        'steps': 0,
        'total_reward': 0.0,
        'max_reward': float('-inf'),
        'min_reward': float('inf'),
        'avg_reward': 0.0,
        'final_curiosity': 0.0,
        'noveltes_detected': 0
    }
    
    reward_history = []
    curiosity_history = []
    
    # Initialize episode
    nlm.resetForNewEpisode(agent)
    
    if verbose:
        print(f"  Episode started: Agent at ({world.getAgentPosition()[0]:.1f}, {world.getAgentPosition()[1]:.1f})")
    
    # Run episode steps
    for step in range(max_steps):
        # 1. Update world dynamics
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # 3. Brain processes input and makes decisions
        brain.step(step)
        
        # 4. Decode motor command
        action = agent.decodeMotorCommand()
        
        # 5. Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 6. Get reward and update statistics
        reward = world.getCurrentReward()
        episode_stats['total_reward'] += reward
        episode_stats['max_reward'] = max(episode_stats['max_reward'], reward)
        episode_stats['min_reward'] = min(episode_stats['min_reward'], reward)
        
        reward_history.append(reward)
        
        # 7. Check curiosity and novelty
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        episode_stats['noveltes_detected'] += 1 if novelty > 0.5 else 0
        
        curiosity_history.append(curiosity)
        
        # 8. Check episode termination
        if world.isEpisodeDone():
            episode_stats['steps'] = step + 1
            if verbose:
                print(f"  Episode ended at step {step + 1}: Total reward = {episode_stats['total_reward']:.2f}")
            break
        
        if verbose and (step + 1) % 50 == 0:
            print(f"  Step {step + 1}: Reward = {reward:.2f}, Curiosity = {curiosity:.3f}")
    
    # Calculate episode statistics
    if episode_stats['steps'] > 0:
        episode_stats['avg_reward'] = episode_stats['total_reward'] / episode_stats['steps']
    episode_stats['final_curiosity'] = curiosity_history[-1] if curiosity_history else 0.0
    
    return episode_stats, reward_history, curiosity_history

def analyze_learning_progress(reward_history: List[float], episode_num: int):
    """
    Analyze learning progress and provide insights.
    
    Args:
        reward_history: List of rewards from previous episodes
        episode_num: Current episode number
        
    Returns:
        Dictionary containing learning analysis
    """
    if len(reward_history) < 2:
        return {'converged': False, 'improvement': 0.0, 'trend': 'insufficient_data'}
    
    # Calculate recent average performance
    recent_episodes = min(10, len(reward_history))
    recent_rewards = reward_history[-recent_episodes:]
    recent_avg = sum(recent_rewards) / len(recent_rewards)
    
    # Calculate baseline average
    baseline_episodes = min(5, len(reward_history) // 2)
    baseline_rewards = reward_history[:baseline_episodes] if len(reward_history) >= baseline_episodes * 2 else []
    
    if baseline_rewards:
        baseline_avg = sum(baseline_rewards) / len(baseline_rewards)
        improvement = recent_avg - baseline_avg
    else:
        improvement = 0.0
    
    # Detect convergence
    convergence = nlm.calculateConvergence(reward_history, threshold=0.01, windowSize=5)
    
    # Analyze trend
    if len(reward_history) >= 5:
        first_half = reward_history[:len(reward_history)//2]
        second_half = reward_history[len(reward_history)//2:]
        first_avg = sum(first_half) / len(first_half)
        second_avg = sum(second_half) / len(second_half)
        
        if second_avg > first_avg * 1.1:
            trend = 'improving'
        elif second_avg < first_avg * 0.9:
            trend = 'declining'
        else:
            trend = 'stable'
    else:
        trend = 'insufficient_data'
    
    return {
        'converged': convergence,
        'improvement': improvement,
        'trend': trend,
        'recent_avg': recent_avg,
        'episode_count': len(reward_history)
    }

def main():
    """Main function demonstrating complete agent simulation."""
    print("=" * 70)
    print("NLM Agent Simulation Example - Complete Learning Agent")
    print("=" * 70)
    
    # ---------------------------------------------------------------------
    # SECTION 1: SETUP AND INITIALIZATION
    # ---------------------------------------------------------------------
    print("\n1. Setting up agent simulation environment...")
    
    # Create optimal configuration
    config = create_agent_simulation_config()
    print(f"   ✓ Created learning-optimized configuration")
    
    # Setup training environment
    world = setup_simulation_environment()
    print(f"   ✓ Created training environment ({world.getWidth()}x{world.getHeight()} units)")
    
    # Create and setup agent
    brain, agent = create_and_setup_agent(world)
    print(f"   ✓ Created learning agent with neuromodulation")
    
    # Initialize brain
    brain.initialize()
    print(f"   ✓ Brain initialized ({brain.getTotalNeuronCount()} neurons)")
    
    # ---------------------------------------------------------------------
    # SECTION 2: EPISODE-BASED TRAINING
    # ---------------------------------------------------------------------
    print("\n2. Running agent training episodes...")
    
    # Training parameters
    num_episodes = 20
    max_steps_per_episode = 200
    verbose_training = True
    
    all_rewards = []
    all_curiosities = []
    episode_summaries = []
    
    # Run training episodes
    for episode in range(num_episodes):
        print(f"\n  Episode {episode + 1}/{num_episodes}:")
        
        # Run single episode
        episode_stats, rewards, curiosities = run_episode(
            agent, world, brain, max_steps_per_episode, verbose_training
        )
        
        # Store episode results
        all_rewards.extend(rewards)
        all_curiosities.extend(curiosities)
        episode_summaries.append(episode_stats)
        
        # Print episode summary
        if verbose_training:
            print(f"    Episode {episode + 1} Summary:")
            print(f"      Steps: {episode_stats['steps']}")
            print(f"      Total Reward: {episode_stats['total_reward']:.2f}")
            print(f"      Average Reward: {episode_stats['avg_reward']:.2f}")
            print(f"      Final Curiosity: {episode_stats['final_curiosity']:.3f}")
            print(f"      Novelties Detected: {episode_stats['noveltes_detected']}")
        
        # Analyze learning progress
        learning_analysis = analyze_learning_progress(all_rewards, episode + 1)
        
        if verbose_training:
            print(f"      Learning Analysis: {learning_analysis['trend']}, Improvement: {learning_analysis['improvement']:.3f}")
            
            if learning_analysis['converged']:
                print(f"      ✓ Learning converged!")
                break
    
    # ---------------------------------------------------------------------
    # SECTION 3: ADVANCED ANALYSIS
    # ---------------------------------------------------------------------
    print("\n3. Advanced analysis of agent behavior...")
    
    # Calculate overall learning statistics
    total_steps = sum(episode['steps'] for episode in episode_summaries)
    total_reward = sum(episode['total_reward'] for episode in episode_summaries)
    overall_avg_reward = total_reward / num_episodes if num_episodes > 0 else 0
    
    # Analyze curiosity patterns
    avg_curiosity = sum(all_curiosities) / len(all_curiosities) if all_curiosities else 0
    max_curiosity = max(all_curiosities) if all_curiosities else 0
    
    # Analyze reward patterns
    learning_progress = nlm.trackLearningProgress(all_rewards, evaluationInterval=5)
    convergence_detected = nlm.calculateConvergence(all_rewards, threshold=0.01, windowSize=10)
    
    print(f"   ✓ Total training completed: {num_episodes} episodes, {total_steps} steps")
    print(f"   ✓ Overall average reward: {overall_avg_reward:.3f}")
    print(f"   ✓ Curiosity levels: Avg={avg_curiosity:.3f}, Max={max_curiosity:.3f}")
    print(f"   ✓ Learning progress: {'Converged' if convergence_detected else 'Ongoing'}")
    
    # ---------------------------------------------------------------------
    # SECTION 4: PERFORMANCE BENCHMARKING
    # ---------------------------------------------------------------------
    print("\n4. Performance benchmarking...")
    
    # Benchmark brain performance
    brain_benchmark = nlm.benchmarkBrainStep(brain, numSteps=100)
    print(f"   ✓ Brain performance: {brain_benchmark.getStepsPerSecond():.1f} steps/sec")
    
    # Benchmark simulation performance
    simulation_benchmark = nlm.benchmarkSimulation(world, brain, numSteps=50)
    print(f"   ✓ Simulation performance: {simulation_benchmark.getSimulationStepsPerSecond():.1f} steps/sec")
    
    # ---------------------------------------------------------------------
    # SECTION 5: VALIDATION AND HEALTH CHECK
    # ---------------------------------------------------------------------
    print("\n5. Final validation and health check...")
    
    # Validate brain state
    brain_issues = nlm.validateBrainDynamics(brain, duration=200)
    if brain_issues:
        print(f"   ⚠ Brain dynamics issues ({len(brain_issues)}):")
        for issue in brain_issues[:3]:
            print(f"     - {issue}")
    else:
        print("   ✓ Brain dynamics are stable")
    
    # Check world state
    world_issues = nlm.validateWorldState(world)
    if world_issues:
        print(f"   ⚠ World state issues ({len(world_issues)}):")
        for issue in world_issues[:3]:
            print(f"     - {issue}")
    else:
        print("   ✓ World state is valid")
    
    # ---------------------------------------------------------------------
    # SECTION 6: VISUALIZATION (if matplotlib available)
    # ---------------------------------------------------------------------
    try:
        # Simple text-based visualization of learning progress
        print("\n6. Learning progress visualization (text-based):")
        
        # Show learning curve
        if len(all_rewards) >= 10:
            print("   Recent rewards by episode (last 10 episodes):")
            for i in range(max(0, len(episode_summaries) - 10), len(episode_summaries)):
                episode = episode_summaries[i]
                if episode['steps'] > 0:
                    print(f"     Episode {i + 1}: Avg reward = {episode['avg_reward']:.3f}")
        
        # Show curiosity trend
        if len(all_curiosities) >= 50:
            print(f"   Curiosity trend: {avg_curiosity:.3f} (avg), {max_curiosity:.3f} (max)")
            
    except Exception as e:
        print(f"   ⚠ Visualization skipped: {str(e)}")
    
    # ---------------------------------------------------------------------
    # SECTION 7: FINAL SUMMARY AND RECOMMENDATIONS
    # ---------------------------------------------------------------------
    print("\n" + "=" * 70)
    print("EXAMPLE COMPLETE - Agent Simulation Summary")
    print("=" * 70)
    
    print(f"\nTraining Statistics:")
    print(f"  - Episodes completed: {num_episodes}")
    print(f"  - Total steps: {total_steps}")
    print(f"  - Overall average reward: {overall_avg_reward:.3f}")
    print(f"  - Total curiosity: {avg_curiosity:.3f}")
    print(f"  - Learning convergence: {'Yes' if convergence_detected else 'No'}")
    
    print(f"\nPerformance Metrics:")
    print(f"  - Brain speed: {brain_benchmark.getStepsPerSecond():.1f} steps/sec")
    print(f"  - Simulation speed: {simulation_benchmark.getSimulationStepsPerSecond():.1f} steps/sec")
    
    print(f"\nHealth Status:")
    print(f"  - Brain dynamics: {'✓ Stable' if not brain_issues else '⚠ Issues'}")
    print(f"  - World state: {'✓ Valid' if not world_issues else '⚠ Issues'}")
    
    print(f"\nNext Steps for Learning:")
    print(f"  1. Try example_learning_brain.py for deep learning demonstrations")
    print(f"  2. Try example_development_brain.py for developmental processes")
    print(f"  3. Try example_neural_analysis.py for advanced analysis techniques")

if __name__ == "__main__":
    main()