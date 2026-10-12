#!/usr/bin/env python3
"""
example_learning_brain.py

Learning with Neuromodulation Example
====================================

This example demonstrates how the NLM Python API enables sophisticated learning
through neuromodulation systems. It showcases dopamine-driven reward learning,
curiosity-based exploration, and prediction error signals working together.

Key Concepts Covered:
- Neuromodulation system integration
- Reward-based learning with dopamine
- Curiosity-driven exploration
- Prediction error learning
- Adaptive learning rate adjustment
- Learning performance optimization

Learning Objectives:
1. Understand neuromodulation systems in NLM
2. Implement reward-modulated learning
3. Configure curiosity-driven exploration
4. Use prediction error for adaptive learning
5. Optimize learning through neuromodulation tuning

Author: NLM Documentation Team
Date: 2026-10-12
"""

import pynlm as nlm
import numpy as np
import time
from typing import List, Tuple

def create_learning_neuromodulation_config():
    """
    Create a configuration optimized for learning through neuromodulation.
    
    Returns:
        Configured brain with learning-optimized settings
    """
    print("   Creating learning neuromodulation configuration...")
    
    # Start with learning configuration
    config = nlm.createLearningConfig()
    
    # Enable all learning features
    config = nlm.enableLearningMode(config)
    
    # Configure neuromodulation parameters for optimal learning
    config.set("neuromodulation.dopamine.learning_rate", 0.15)
    config.set("neuromodulation.dopamine.baseline_dopamine", 0.5)
    config.set("neuromodulation.curiosity.novelty_weight", 0.8)
    config.set("neuromodulation.curiosity.error_weight", 0.2)
    config.set("neuromodulation.prediction_error.precision_weight", 0.6)
    config.set("neuromodulation.prediction_error.confidence_weight", 0.4)
    
    # Enable adaptive learning rate adjustment
    config.set("brain.plasticity.adaptive_learning.enabled", True)
    config.set("brain.plasticity.adaptive_learning.target_error", 0.1)
    
    return config

def setup_learning_neuromodulation(brain, agent, world):
    """
    Set up advanced neuromodulation systems for learning.
    
    Args:
        brain: The brain to configure
        agent: The agent to configure
        world: The environment for context
        
    Returns:
        Configured brain and agent
    """
    print("   Setting up neuromodulation systems...")
    
    # 1. Reward Modulation (Dopamine System)
    print("     - Configuring reward modulation...")
    nlm.setupRewardModulation(agent, learningRate=0.15)
    
    # Set dopamine baseline and prediction parameters
    brain.setDopamineBaseline(0.5)
    brain.setDopaminePredictionErrorWeight(0.3)
    brain.setDopamineLearningRate(0.15)
    
    # 2. Curiosity Modulation
    print("     - Configuring curiosity modulation...")
    nlm.setupCuriosityModulation(agent, noveltyWeight=0.8, errorWeight=0.2)
    
    # Create advanced curiosity profile
    curiosity_profile = nlm.CuriosityProfile(
        noveltyThreshold=0.6,
        errorSensitivity=0.4,
        explorationDrive=0.9,
        rewardSensitivity=0.3
    )
    nlm.createCuriosityProfile(agent, curiosity_profile)
    
    # 3. Prediction Error Modulation
    print("     - Configuring prediction error learning...")
    nlm.setupPredictionErrorModulation(agent, precisionWeight=0.6)
    
    brain.setPredictionErrorWeight(0.6)
    brain.setPredictionPrecision(0.8)
    
    # 4. Development System Integration
    print("     - Enabling developmental plasticity...")
    nlm.setupBrainForDevelopment(brain, world)
    
    # Enable critical period for enhanced learning
    nlm.enableCriticalPeriod(brain, duration=1000)
    
    # Schedule developmental stages
    nlm.scheduleDevelopmentStage(brain, nlm.DevelopmentalStage.CriticalPeriod, duration=500)
    nlm.scheduleDevelopmentStage(brain, nlm.DevelopmentalStage.Maturation, duration=1500)
    
    # 5. Advanced Plasticity Configuration
    print("     - Configuring advanced plasticity rules...")
    
    # Configure STDP for reward-modulated learning
    stdp_config = nlm.STDPConfig(
        learningRate=0.01,
        timeWindow=50.0,
        rewardModulation=True,
        dopamineInfluence=0.7
    )
    brain.setSTDPConfig(stdp_config)
    
    # Configure Hebbian learning with neuromodulation
    hebbian_config = nlm.HebbianConfig(
        learningRate=0.02,
        cooperationThreshold=0.3,
        explorationEnhancement=True,
        predictionErrorInfluence=0.5
    )
    brain.setHebbianConfig(hebbian_config)
    
    return brain, agent

def simulate_learning_episode(agent, world, brain, step_callback=None):
    """
    Simulate a learning episode with neuromodulation.
    
    Args:
        agent: The learning agent
        world: The environment
        brain: The brain
        step_callback: Optional callback for step-by-step monitoring
        
    Returns:
        Episode statistics
    """
    episode_stats = {
        'steps': 0,
        'total_reward': 0.0,
        'reward_history': [],
        'dopamine_history': [],
        'curiosity_history': [],
        'prediction_error_history': [],
        'learning_rate_history': []
    }
    
    # Reset for new episode
    nlm.resetForNewEpisode(agent)
    
    # Simulation loop
    max_steps = 200
    for step in range(max_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Process through brain with neuromodulation
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect statistics
        reward = world.getCurrentReward()
        episode_stats['reward_history'].append(reward)
        episode_stats['total_reward'] += reward
        
        # Record neuromodulation states
        episode_stats['dopamine_history'].append(agent.getDopamineLevel())
        episode_stats['curiosity_history'].append(agent.getCuriosityLevel())
        episode_stats['prediction_error_history'].append(agent.getPredictionErrorLevel())
        
        # Get adaptive learning rate
        learning_rate = brain.getCurrentLearningRate()
        episode_stats['learning_rate_history'].append(learning_rate)
        
        # Call step callback if provided
        if step_callback:
            step_callback(step, reward, learning_rate)
        
        # Check for episode termination
        if world.isEpisodeDone():
            episode_stats['steps'] = step + 1
            break
    
    return episode_stats

def analyze_neuromodulation_effectiveness(reward_history: List[float], 
                                         dopamine_history: List[float],
                                         curiosity_history: List[float]):
    """
    Analyze the effectiveness of neuromodulation systems.
    
    Args:
        reward_history: Reward values over time
        dopamine_history: Dopamine levels over time
        curiosity_history: Curiosity levels over time
        
    Returns:
        Analysis of neuromodulation effectiveness
    """
    analysis = {
        'reward_correlation_dopamine': 0.0,
        'reward_correlation_curiosity': 0.0,
        'learning_efficiency': 0.0,
        'exploration_effectiveness': 0.0,
        'adaptation_speed': 0.0
    }
    
    if len(reward_history) < 10:
        return analysis
    
    # Calculate correlations
    if len(dopamine_history) == len(reward_history):
        correlation = np.corrcoef(reward_history, dopamine_history)[0, 1]
        analysis['reward_correlation_dopamine'] = max(-1, min(1, correlation))
    
    if len(curiosity_history) == len(reward_history):
        correlation = np.corrcoef(reward_history, curiosity_history)[0, 1]
        analysis['reward_correlation_curiosity'] = max(-1, min(1, correlation))
    
    # Calculate learning efficiency
    if len(reward_history) >= 20:
        early_rewards = reward_history[:10]
        late_rewards = reward_history[-10:]
        early_avg = np.mean(early_rewards)
        late_avg = np.mean(late_rewards)
        
        if early_avg > 0:
            analysis['learning_efficiency'] = (late_avg - early_avg) / early_avg
        
        # Calculate adaptation speed
        learning_curve = nlm.trackLearningProgress(reward_history, evaluationInterval=5)
        analysis['adaptation_speed'] = np.mean(np.diff(learning_curve)) if len(learning_curve) > 1 else 0
    
    # Calculate exploration effectiveness
    high_curiosity_threshold = 0.7
    high_curiosity_episodes = sum(1 for c in curiosity_history if c > high_curiosity_threshold)
    analysis['exploration_effectiveness'] = high_curiosity_episodes / len(curiosity_history) if curiosity_history else 0
    
    return analysis

def optimize_neuromodulation_parameters(brain, agent, world, iterations=10):
    """
    Optimize neuromodulation parameters through experimentation.
    
    Args:
        brain: The brain to optimize
        agent: The agent to optimize
        world: The environment for testing
        iterations: Number of optimization iterations
        
    Returns:
        Optimized parameters
    """
    print("   Running neuromodulation optimization...")
    
    best_performance = -float('inf')
    best_params = {}
    
    for iteration in range(iterations):
        print(f"     Iteration {iteration + 1}/{iterations}...")
        
        # Test different parameter combinations
        param_sets = [
            {'lr': 0.1, 'novelty': 0.7, 'error': 0.3},
            {'lr': 0.15, 'novelty': 0.8, 'error': 0.2},
            {'lr': 0.2, 'novelty': 0.6, 'error': 0.4},
            {'lr': 0.12, 'novelty': 0.85, 'error': 0.15}
        ]
        
        for i, params in enumerate(param_sets):
            # Setup with current parameters
            nlm.setupRewardModulation(agent, learningRate=params['lr'])
            nlm.setupCuriosityModulation(agent, noveltyWeight=params['novelty'], errorWeight=params['error'])
            
            # Run test episode
            episode_stats = simulate_learning_episode(agent, world, brain, max_steps=100)
            
            # Calculate performance score
            if episode_stats['reward_history']:
                recent_rewards = episode_stats['reward_history'][-10:]
                performance = np.mean(recent_rewards)
                
                if performance > best_performance:
                    best_performance = performance
                    best_params = {
                        'learning_rate': params['lr'],
                        'novelty_weight': params['novelty'],
                        'error_weight': params['error'],
                        'performance': performance
                    }
    
    print(f"   ✓ Best parameters found: {best_params}")
    return best_params

def main():
    """Main function demonstrating learning with neuromodulation."""
    print("=" * 70)
    print("NLM Learning with Neuromodulation Example")
    print("=" * 70)
    
    # ---------------------------------------------------------------------
    # SECTION 1: SETUP AND CONFIGURATION
    # ---------------------------------------------------------------------
    print("\n1. Setting up learning neuromodulation system...")
    
    # Create environment
    world = nlm.createTrainingEnvironment()
    nlm.setupAgentStartPosition(world, x=50.0, y=50.0)
    
    # Create learning-optimized brain
    config = create_learning_neuromodulation_config()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = nlm.createLearningAgent(brain)
    
    # Setup advanced neuromodulation
    brain, agent = setup_learning_neuromodulation(brain, agent, world)
    
    print(f"   ✓ Brain created: {brain.getTotalNeuronCount()} neurons")
    print(f"   ✓ Agent configured: Reward modulation, curiosity, prediction error")
    
    # ---------------------------------------------------------------------
    # SECTION 2: LEARNING SIMULATION
    # ---------------------------------------------------------------------
    print("\n2. Running learning simulation...")
    
    # Learning parameters
    num_episodes = 15
    verbose = True
    
    # Training loop
    all_rewards = []
    all_dopamine = []
    all_curiosity = []
    episode_summaries = []
    
    for episode in range(num_episodes):
        print(f"\n  Episode {episode + 1}/{num_episodes}:")
        
        # Track learning progress
        def step_callback(step, reward, learning_rate):
            all_rewards.append(reward)
            all_dopamine.append(agent.getDopamineLevel())
            all_curiosity.append(agent.getCuriosityLevel())
        
        # Run episode
        episode_stats = simulate_learning_episode(
            agent, world, brain, step_callback if episode < num_episodes - 1 else None
        )
        
        episode_summaries.append(episode_stats)
        
        if verbose:
            print(f"    Steps: {episode_stats['steps']}")
            print(f"    Total reward: {episode_stats['total_reward']:.2f}")
            print(f"    Final dopamine: {episode_stats['dopamine_history'][-1]:.3f}")
            print(f"    Final curiosity: {episode_stats['curiosity_history'][-1]:.3f}")
    
    # ---------------------------------------------------------------------
    # SECTION 3: NEUROMODULATION ANALYSIS
    # ---------------------------------------------------------------------
    print("\n3. Analyzing neuromodulation effectiveness...")
    
    # Analyze neuromodulation systems
    neuromod_analysis = analyze_neuromodulation_effectiveness(
        all_rewards, all_dopamine, all_curiosity
    )
    
    print(f"   ✓ Reward-Dopamine correlation: {neuromod_analysis['reward_correlation_dopamine']:.3f}")
    print(f"   ✓ Reward-Curiosity correlation: {neuromod_analysis['reward_correlation_curiosity']:.3f}")
    print(f"   ✓ Learning efficiency: {neuromod_analysis['learning_efficiency']:.3f}")
    print(f"   ✓ Exploration effectiveness: {neuromod_analysis['exploration_effectiveness']:.3f}")
    print(f"   ✓ Adaptation speed: {neuromod_analysis['adaptation_speed']:.4f}")
    
    # ---------------------------------------------------------------------
    # SECTION 4: PARAMETER OPTIMIZATION
    # ---------------------------------------------------------------------
    print("\n4. Running parameter optimization...")
    
    # Optimize neuromodulation parameters
    optimized_params = optimize_neuromodulation_parameters(
        brain, agent, world, iterations=5
    )
    
    print(f"   ✓ Optimization complete: {optimized_params}")
    
    # ---------------------------------------------------------------------
    # SECTION 5: ADVANCED LEARNING METRICS
    # ---------------------------------------------------------------------
    print("\n5. Calculating advanced learning metrics...")
    
    # Calculate learning progress
    learning_progress = nlm.trackLearningProgress(all_rewards, evaluationInterval=3)
    convergence = nlm.calculateConvergence(all_rewards, threshold=0.01, windowSize=5)
    
    # Calculate burst statistics
    burst_stats = nlm.detectBursting(all_dopamine, duration=100, threshold=0.5)
    
    # Calculate curiosity patterns
    curiosity_pattern = nlm.analyzeFiringPattern(all_curiosity, duration=100)
    
    print(f"   ✓ Learning progress (smoothed): {learning_progress[-5:] if len(learning_progress) >= 5 else learning_progress}")
    print(f"   ✓ Learning converged: {'Yes' if convergence else 'No'}")
    print(f"   ✓ Burst events detected: {len(burst_stats)}")
    print(f"   ✓ Curiosity pattern metrics: {curiosity_pattern}")
    
    # ---------------------------------------------------------------------
    # SECTION 6: VALIDATION AND HEALTH CHECK
    # ---------------------------------------------------------------------
    print("\n6. Final validation...")
    
    # Validate brain state
    brain_issues = nlm.validateBrainDynamics(brain, duration=150)
    if brain_issues:
        print(f"   ⚠ Brain dynamics issues: {len(brain_issues)}")
    else:
        print("   ✓ Brain dynamics are stable")
    
    # Check neuromodulation balance
    avg_dopamine = np.mean(all_dopamine) if all_dopamine else 0
    avg_curiosity = np.mean(all_curiosity) if all_curiosity else 0
    
    dopamine_balance = abs(avg_dopamine - 0.5) < 0.2  # Should be around 0.5
    curiosity_balance = 0.4 < avg_curiosity < 0.8  # Should be in middle range
    
    print(f"   ✓ Dopamine balance: {'✓ OK' if dopamine_balance else '⚠ Imbalanced'} (avg: {avg_dopamine:.3f})")
    print(f"   ✓ Curiosity balance: {'✓ OK' if curiosity_balance else '⚠ Imbalanced'} (avg: {avg_curiosity:.3f})")
    
    # ---------------------------------------------------------------------
    # SECTION 7: PERFORMANCE BENCHMARKING
    # ---------------------------------------------------------------------
    print("\n7. Performance benchmarking...")
    
    # Benchmark brain performance
    brain_benchmark = nlm.benchmarkBrainStep(brain, numSteps=100)
    print(f"   ✓ Brain stepping: {brain_benchmark.getStepsPerSecond():.1f} steps/sec")
    
    # Benchmark simulation performance
    simulation_benchmark = nlm.benchmarkSimulation(world, brain, numSteps=50)
    print(f"   ✓ Simulation performance: {simulation_benchmark.getSimulationStepsPerSecond():.1f} steps/sec")
    
    # ---------------------------------------------------------------------
    # SECTION 8: FINAL SUMMARY
    # ---------------------------------------------------------------------
    print("\n" + "=" * 70)
    print("EXAMPLE COMPLETE - Learning with Neuromodulation Summary")
    print("=" * 70)
    
    print(f"\nLearning Performance:")
    print(f"  - Episodes completed: {num_episodes}")
    print(f"  - Total steps: {sum(episode['steps'] for episode in episode_summaries)}")
    print(f"  - Final reward: {all_rewards[-1] if all_rewards else 0:.3f}")
    print(f"  - Learning efficiency: {neuromod_analysis['learning_efficiency']:.3f}")
    print(f"  - Convergence: {'Yes' if convergence else 'No'}")
    
    print(f"\nNeuromodulation Health:")
    print(f"  - Dopamine balance: {'✓' if dopamine_balance else '⚠'} (avg: {avg_dopamine:.3f})")
    print(f"  - Curiosity balance: {'✓' if curiosity_balance else '⚠'} (avg: {avg_curiosity:.3f})")
    print(f"  - Prediction accuracy: {neuromod_analysis['reward_correlation_dopamine']:.3f}")
    print(f"  - Exploration rate: {neuromod_analysis['exploration_effectiveness']:.3f}")
    
    print(f"\nNext Steps for Learning:")
    print(f"  1. Try example_development_brain.py for developmental processes")
    print(f"  2. Try example_world_interaction.py for environment interaction")
    print(f"  3. Try example_neural_analysis.py for advanced analysis")
    print(f"  4. Try recipe_learning_optimization.py for optimization recipes")

if __name__ == "__main__":
    main()