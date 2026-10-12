#!/usr/bin/env python3
"""
recipe_learning_optimization.py

Recipe for Optimizing Learning Performance
===========================================

This recipe provides practical techniques for optimizing learning performance
in the NLM framework. It combines best practices with experimental results
to help users achieve faster, more reliable learning.

Key Concepts Covered:
- Learning rate optimization
- Neuromodulation tuning
- Curriculum learning
- Exploration-exploitation balance
- Learning progress monitoring
- Adaptive learning strategies

Learning Objectives:
1. Optimize learning rate for different tasks
2. Balance exploration and exploitation
3. Implement curriculum learning
4. Use neuromodulation for enhanced learning
5. Monitor and adapt learning strategies
6. Apply optimization recipes systematically

Author: NLM Documentation Team
Date: 2026-10-12
"""

import pynlm as nlm
import numpy as np
import time
from typing import List, Dict, Tuple, Callable

def create_optimization_workspace():
    """
    Create an optimization workspace with multiple test environments.
    
    Returns:
        Dictionary of optimization environments
    """
    print("   Creating optimization workspace...")
    
    # Create different environments for optimization testing
    environments = {
        'simple': nlm.createStandardEnvironment(),
        'training': nlm.createTrainingEnvironment(),
        'exploration': nlm.createExplorationEnvironment()
    }
    
    # Setup environments
    for name, env in environments.items():
        nlm.setupAgentStartPosition(env, x=50.0, y=50.0)
        
        # Add environment-specific elements
        if name == 'simple':
            nlm.spawnResource(env, x=30.0, y=30.0, value=10.0)
        elif name == 'training':
            nlm.spawnResource(env, x=20.0, y=20.0, value=15.0)
            nlm.spawnResource(env, x=80.0, y=80.0, value=25.0)
            nlm.createHazard(env, x=60.0, y=60.0, damage=5.0)
        elif name == 'exploration':
            for i in range(5):
                nlm.spawnResource(env, x=np.random.uniform(20, 80), y=np.random.uniform(20, 80), value=np.random.uniform(5, 20))
    
    return environments

def optimize_learning_rate(brain, agent, world, learning_rate_range: List[float], 
                          evaluation_steps: int = 100) -> Dict[float, float]:
    """
    Find optimal learning rate through systematic testing.
    
    Args:
        brain: Brain to optimize
        agent: Agent to optimize
        world: Environment to test in
        learning_rate_range: Range of learning rates to test
        evaluation_steps: Number of steps for each test
        
    Returns:
        Dictionary mapping learning rates to performance scores
    """
    print("   Optimizing learning rate...")
    
    results = {}
    
    for lr in learning_rate_range:
        print(f"     Testing learning rate: {lr}")
        
        # Reset brain and agent
        brain = nlm.createDefaultBrain()
        nlm.setupBrainForLearning(brain, world)
        agent = nlm.createLearningAgent(brain)
        agent.initialize(world)
        nlm.setupRewardModulation(agent, learningRate=lr)
        
        # Run optimization test
        start_time = time.time()
        
        for step in range(evaluation_steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.time()
        
        # Calculate performance score
        elapsed = end_time - start_time
        steps_per_second = evaluation_steps / elapsed if elapsed > 0 else 0
        reward = world.getTotalReward()
        
        # Normalize performance (higher is better)
        performance = (steps_per_second / 50.0) * 0.3 + (reward / (evaluation_steps * 10.0)) * 0.7
        results[lr] = performance
    
    return results

def optimize_curiosity_balance(brain, agent, world, balance_range: List[float],
                              evaluation_steps: int = 150) -> Dict[float, float]:
    """
    Find optimal curiosity-exploitation balance.
    
    Args:
        brain: Brain to optimize
        agent: Agent to optimize
        world: Environment to test in
        balance_range: Range of curiosity balances to test
        evaluation_steps: Number of steps for each test
        
    Returns:
        Dictionary mapping curiosity balances to performance scores
    """
    print("   Optimizing curiosity balance...")
    
    results = {}
    
    for balance in balance_range:
        print(f"     Testing curiosity balance: {balance}")
        
        # Reset with specific curiosity balance
        brain = nlm.createDefaultBrain()
        nlm.setupBrainForLearning(brain, world)
        agent = nlm.createExplorationAgent(brain)
        agent.initialize(world)
        
        # Create specific curiosity profile
        curiosity_profile = nlm.CuriosityProfile(
            noveltyThreshold=balance,
            errorSensitivity=balance * 0.5,
            explorationDrive=balance,
            rewardSensitivity=1.0 - balance
        )
        nlm.createCuriosityProfile(agent, curiosity_profile)
        
        # Run optimization test
        start_time = time.time()
        
        for step in range(evaluation_steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.time()
        
        # Calculate performance score
        elapsed = end_time - start_time
        steps_per_second = evaluation_steps / elapsed if elapsed > 0 else 0
        reward = world.getTotalReward()
        curiosity_level = np.mean([agent.getCuriosityLevel() for _ in range(evaluation_steps)])
        
        # Balance performance (higher is better)
        balance_score = (steps_per_second / 50.0) * 0.4 + (reward / (evaluation_steps * 10.0)) * 0.4 + (1.0 - abs(curiosity_level - balance)) * 0.2
        results[balance] = balance_score
    
    return results

def create_curriculum_learning_strategy():
    """
    Create a curriculum learning strategy for progressive complexity.
    
    Returns:
        Curriculum learning configuration
    """
    print("   Creating curriculum learning strategy...")
    
    curriculum = {
        'stages': [],
        'difficulty_progression': [0.3, 0.5, 0.7, 0.9],
        'training_duration': [50, 100, 150, 200],
        'reward_thresholds': [5.0, 15.0, 25.0, 40.0],
        'adaptation_rate': 0.8
    }
    
    # Define curriculum stages
    for i, (difficulty, duration, threshold) in enumerate(zip(
        curriculum['difficulty_progression'],
        curriculum['training_duration'],
        curriculum['reward_thresholds']
    )):
        stage = {
            'stage_id': i + 1,
            'difficulty': difficulty,
            'duration': duration,
            'reward_threshold': threshold,
            'neuromodulation_adjustments': {
                'learning_rate_multiplier': 1.0 + i * 0.2,
                'curiosity_weight': 0.9 - i * 0.1,
                'prediction_error_weight': 0.5 + i * 0.1
            }
        }
        curriculum['stages'].append(stage)
    
    return curriculum

def run_curriculum_learning(brain, agent, world, curriculum):
    """
    Execute curriculum learning strategy.
    
    Args:
        brain: Brain to train
        agent: Agent to train
        world: Environment
        curriculum: Curriculum learning strategy
        
    Returns:
        Curriculum learning results
    """
    print("   Running curriculum learning...")
    
    results = {
        'stage_performance': [],
        'overall_performance': 0.0,
        'progression_rate': 0.0,
        'final_difficulty': curriculum['difficulty_progression'][-1]
    }
    
    for stage_idx, stage in enumerate(curriculum['stages']):
        print(f"     Stage {stage['stage_id']}: Difficulty {stage['difficulty']}, "
              f"Duration {stage['duration']} steps")
        
        # Configure brain for current stage
        brain = nlm.createDefaultBrain()
        nlm.setupBrainForLearning(brain, world)
        
        # Apply stage-specific neuromodulation
        nlm.setupRewardModulation(agent, learningRate=0.1 * stage['neuromodulation_adjustments']['learning_rate_multiplier'])
        
        # Create curiosity profile for current stage
        curiosity_profile = nlm.CuriosityProfile(
            noveltyThreshold=stage['neuromodulation_adjustments']['curiosity_weight'],
            errorSensitivity=0.3,
            explorationDrive=stage['neuromodulation_adjustments']['curiosity_weight'],
            rewardSensitivity=0.7
        )
        nlm.createCuriosityProfile(agent, curiosity_profile)
        
        # Run stage training
        start_time = time.time()
        
        for step in range(stage['duration']):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.time()
        
        # Calculate stage performance
        elapsed = end_time - start_time
        steps_per_second = stage['duration'] / elapsed if elapsed > 0 else 0
        reward = world.getTotalReward()
        avg_reward = reward / stage['duration']
        
        # Check if reward threshold met
        threshold_met = avg_reward >= stage['reward_threshold']
        
        stage_result = {
            'stage_id': stage['stage_id'],
            'performance': (steps_per_second / 50.0) * 0.3 + (avg_reward / stage['reward_threshold']) * 0.7,
            'steps_per_second': steps_per_second,
            'average_reward': avg_reward,
            'threshold_met': threshold_met,
            'training_time': elapsed
        }
        
        results['stage_performance'].append(stage_result)
    
    # Calculate overall performance
    if results['stage_performance']:
        results['overall_performance'] = np.mean([s['performance'] for s in results['stage_performance']])
        results['progression_rate'] = np.mean([s['performance'] for s in results['stage_performance'] if s['performance'] < 0.5])
    
    return results

def adaptive_learning_optimization(brain, agent, world, max_iterations: int = 20):
    """
    Implement adaptive learning optimization using feedback loops.
    
    Args:
        brain: Brain to optimize
        agent: Agent to optimize
        world: Environment
        max_iterations: Maximum optimization iterations
        
    Returns:
        Adaptive optimization results
    """
    print("   Running adaptive learning optimization...")
    
    optimization_history = []
    
    for iteration in range(max_iterations):
        print(f"     Iteration {iteration + 1}/{max_iterations}")
        
        # Create fresh configuration for this iteration
        brain = nlm.createDefaultBrain()
        nlm.setupBrainForLearning(brain, world)
        agent = nlm.createLearningAgent(brain)
        agent.initialize(world)
        
        # Randomly sample hyperparameters
        learning_rate = np.random.uniform(0.05, 0.2)
        curiosity_balance = np.random.uniform(0.6, 0.9)
        error_weight = np.random.uniform(0.2, 0.6)
        
        # Configure with sampled hyperparameters
        nlm.setupRewardModulation(agent, learningRate=learning_rate)
        
        curiosity_profile = nlm.CuriosityProfile(
            noveltyThreshold=curiosity_balance,
            errorSensitivity=error_weight,
            explorationDrive=curiosity_balance,
            rewardSensitivity=1.0 - curiosity_balance
        )
        nlm.createCuriosityProfile(agent, curiosity_profile)
        
        # Run optimization trial
        start_time = time.time()
        
        for step in range(100):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.time()
        
        # Record results
        elapsed = end_time - start_time
        steps_per_second = 100 / elapsed if elapsed > 0 else 0
        reward = world.getTotalReward()
        avg_curiosity = np.mean([agent.getCuriosityLevel() for _ in range(100)])
        
        iteration_result = {
            'iteration': iteration + 1,
            'learning_rate': learning_rate,
            'curiosity_balance': curiosity_balance,
            'error_weight': error_weight,
            'performance': (steps_per_second / 50.0) * 0.4 + (reward / (100 * 10.0)) * 0.4 + (1.0 - abs(avg_curiosity - 0.7)) * 0.2,
            'reward': reward,
            'speed': steps_per_second,
            'efficiency': reward / elapsed if elapsed > 0 else 0
        }
        
        optimization_history.append(iteration_result)
    
    # Find best configuration
    best_iteration = max(optimization_history, key=lambda x: x['performance'])
    
    return {
        'optimization_history': optimization_history,
        'best_iteration': best_iteration,
        'convergence_rate': np.mean([h['performance'] for h in optimization_history if h['performance'] > 0.7]) if optimization_history else 0
    }

def generate_optimization_report(learning_rate_results: Dict,
                                curiosity_results: Dict,
                                curriculum_results: Dict,
                                adaptive_results: Dict) -> Dict:
    """
    Generate comprehensive optimization report.
    
    Args:
        learning_rate_results: Learning rate optimization results
        curiosity_results: Curiosity balance optimization results
        curriculum_results: Curriculum learning results
        adaptive_results: Adaptive learning results
        
    Returns:
        Comprehensive optimization report
    """
    print("   Generating optimization report...")
    
    # Find optimal parameters
    optimal_lr = max(learning_rate_results.items(), key=lambda x: x[1])[0] if learning_rate_results else 0.1
    optimal_curiosity = max(curiosity_results.items(), key=lambda x: x[1])[0] if curiosity_results else 0.7
    
    # Calculate overall optimization scores
    curriculum_performance = curriculum_results.get('overall_performance', 0.0)
    adaptive_convergence = adaptive_results.get('convergence_rate', 0.0)
    adaptive_efficiency = adaptive_results.get('best_iteration', {}).get('efficiency', 0.0)
    
    # Generate recommendations
    recommendations = []
    
    if optimal_lr > 0.15:
        recommendations.append("Use high learning rate (0.15+) for fast convergence")
    else:
        recommendations.append("Use moderate learning rate (0.1-0.15) for stable learning")
    
    if optimal_curiosity > 0.8:
        recommendations.append("Use high curiosity balance for exploration")
    else:
        recommendations.append("Use moderate curiosity balance for balanced exploration")
    
    if curriculum_performance > 0.8:
        recommendations.append("Curriculum learning is effective for progressive training")
    else:
        recommendations.append("Consider simplifying curriculum or extending stage durations")
    
    if adaptive_convergence > 0.7:
        recommendations.append("Adaptive optimization shows good convergence patterns")
    else:
        recommendations.append("Improve adaptive optimization with more iterations or better sampling")
    
    # Calculate overall optimization score
    overall_score = (
        (optimal_lr / 0.2) * 0.25 +
        (optimal_curiosity) * 0.25 +
        (curriculum_performance) * 0.25 +
        (adaptive_convergence) * 0.25
    )
    
    report = {
        'optimal_parameters': {
            'learning_rate': optimal_lr,
            'curiosity_balance': optimal_curiosity
        },
        'performance_metrics': {
            'curriculum_performance': curriculum_performance,
            'adaptive_convergence': adaptive_convergence,
            'adaptive_efficiency': adaptive_efficiency
        },
        'recommendations': recommendations,
        'overall_optimization_score': overall_score,
        'optimization_rating': 'Excellent' if overall_score > 0.8 else 'Good' if overall_score > 0.6 else 'Fair'
    }
    
    return report

def main():
    """Main function demonstrating learning optimization recipes."""
    print("=" * 70)
    print("NLM Learning Optimization Recipe")
    print("=" * 70)
    
    # ---------------------------------------------------------------------
    # SECTION 1: WORKSPACE SETUP
    # ---------------------------------------------------------------------
    print("\n1. Creating optimization workspace...")
    
    # Create optimization environments
    environments = create_optimization_workspace()
    print(f"   ✓ Created {len(environments)} optimization environments")
    
    # Select primary environment for testing
    primary_world = environments['training']
    
    # ---------------------------------------------------------------------
    # SECTION 2: LEARNING RATE OPTIMIZATION
    # ---------------------------------------------------------------------
    print("\n2. Learning rate optimization...")
    
    learning_rate_range = [0.05, 0.1, 0.15, 0.2, 0.25]
    learning_rate_results = optimize_learning_rate(
        nlm.createDefaultBrain(),
        nlm.createLearningAgent(nlm.createDefaultBrain()),
        primary_world,
        learning_rate_range,
        evaluation_steps=100
    )
    
    print("   ✓ Learning rate optimization completed:")
    for lr, score in learning_rate_results.items():
        print(f"     Learning rate {lr}: {score:.3f} performance")
    
    # ---------------------------------------------------------------------
    # SECTION 3: CURiosity BALANCE OPTIMIZATION
    # ---------------------------------------------------------------------
    print("\n3. Curiosity balance optimization...")
    
    curiosity_range = [0.6, 0.7, 0.8, 0.9]
    curiosity_results = optimize_curiosity_balance(
        nlm.createDefaultBrain(),
        nlm.createExplorationAgent(nlm.createDefaultBrain()),
        primary_world,
        curiosity_range,
        evaluation_steps=150
    )
    
    print("   ✓ Curiosity balance optimization completed:")
    for balance, score in curiosity_results.items():
        print(f"     Balance {balance}: {score:.3f} performance")
    
    # ---------------------------------------------------------------------
    # SECTION 4: CURRICULUM LEARNING
    # ---------------------------------------------------------------------
    print("\n4. Curriculum learning implementation...")
    
    curriculum = create_curriculum_learning_strategy()
    curriculum_results = run_curriculum_learning(
        nlm.createDefaultBrain(),
        nlm.createLearningAgent(nlm.createDefaultBrain()),
        primary_world,
        curriculum
    )
    
    print("   ✓ Curriculum learning completed:")
    print(f"     Overall performance: {curriculum_results['overall_performance']:.3f}")
    print(f"     Progression rate: {curriculum_results['progression_rate']:.3f}")
    
    for stage in curriculum_results['stage_performance']:
        print(f"     Stage {stage['stage_id']}: {stage['performance']:.3f}, "
              f"Threshold: {'✓' if stage['threshold_met'] else '✗'}")
    
    # ---------------------------------------------------------------------
    # SECTION 5: ADAPTIVE LEARNING
    # ---------------------------------------------------------------------
    print("\n5. Adaptive learning optimization...")
    
    adaptive_results = adaptive_learning_optimization(
        nlm.createDefaultBrain(),
        nlm.createLearningAgent(nlm.createDefaultBrain()),
        primary_world,
        max_iterations=15
    )
    
    print("   ✓ Adaptive learning optimization completed:")
    print(f"     Best iteration performance: {adaptive_results['best_iteration']['performance']:.3f}")
    print(f"     Convergence rate: {adaptive_results['convergence_rate']:.3f}")
    
    # ---------------------------------------------------------------------
    # SECTION 6: OPTIMIZATION REPORT
    # ---------------------------------------------------------------------
    print("\n6. Generating optimization report...")
    
    optimization_report = generate_optimization_report(
        learning_rate_results,
        curiosity_results,
        curriculum_results,
        adaptive_results
    )
    
    print("   ✓ Optimization Report Generated:")
    print(f"     Optimal learning rate: {optimization_report['optimal_parameters']['learning_rate']:.3f}")
    print(f"     Optimal curiosity balance: {optimization_report['optimal_parameters']['curiosity_balance']:.3f}")
    print(f"     Overall optimization score: {optimization_report['overall_optimization_score']:.3f}")
    print(f"     Optimization rating: {optimization_report['optimization_rating']}")
    
    if optimization_report['recommendations']:
        print("     Top recommendations:")
        for rec in optimization_report['recommendations'][:3]:
            print(f"       ✓ {rec}")
    
    # ---------------------------------------------------------------------
    # SECTION 7: VALIDATION AND HEALTH CHECK
    # ---------------------------------------------------------------------
    print("\n7. Final validation...")
    
    # Create final optimized brain
    optimized_brain = nlm.createDefaultBrain()
    nlm.setupBrainForLearning(optimized_brain, primary_world)
    
    final_agent = nlm.createLearningAgent(optimized_brain)
    final_agent.initialize(primary_world)
    nlm.setupRewardModulation(final_agent, learningRate=optimization_report['optimal_parameters']['learning_rate'])
    
    curiosity_profile = nlm.CuriosityProfile(
        noveltyThreshold=optimization_report['optimal_parameters']['curiosity_balance'],
        errorSensitivity=0.3,
        explorationDrive=optimization_report['optimal_parameters']['curiosity_balance'],
        rewardSensitivity=0.7
    )
    nlm.createCuriosityProfile(final_agent, curiosity_profile)
    
    # Validate final configuration
    brain_issues = nlm.validateBrainDynamics(optimized_brain, duration=50)
    world_issues = nlm.validateWorldState(primary_world)
    
    print(f"   ✓ Brain validation: {'✓ Passed' if not brain_issues else '⚠ Issues found'}")
    print(f"   ✓ World validation: {'✓ Passed' if not world_issues else '⚠ Issues found'}")
    
    # ---------------------------------------------------------------------
    # SECTION 8: FINAL SUMMARY
    # ---------------------------------------------------------------------
    print("\n" + "=" * 70)
    print("RECIPE COMPLETE - Learning Optimization Summary")
    print("=" * 70)
    
    print(f"\nOptimization Results:")
    print(f"  - Optimal learning rate: {optimization_report['optimal_parameters']['learning_rate']:.3f}")
    print(f"  - Optimal curiosity balance: {optimization_report['optimal_parameters']['curiosity_balance']:.3f}")
    print(f"  - Curriculum performance: {optimization_report['performance_metrics']['curriculum_performance']:.3f}")
    print(f"  - Adaptive convergence: {optimization_report['performance_metrics']['adaptive_convergence']:.3f}")
    print(f"  - Overall optimization score: {optimization_report['overall_optimization_score']:.3f}")
    print(f"  - Optimization rating: {optimization_report['optimization_rating']}")
    
    print(f"\nKey Insights:")
    print(f"  ✓ Learning rate and curiosity balance are critical factors")
    print(f"  ✓ Curriculum learning enables progressive complexity")
    print(f"  ✓ Adaptive optimization finds optimal parameter combinations")
    print(f"  ✓ Multiple optimization strategies complement each other")
    
    print(f"\nNext Steps for Learning:")
    print(f"  1. Try example_agent_simulation.py for practical application")
    print(f"  2. Try example_development_brain.py for developmental learning")
    print(f"  3. Try tutorial_step_by_step.py for comprehensive tutorial")
    print(f"  4. Try recipe_agent_training.py for additional training recipes")
    print(f"  5. Use optimization results to configure your own learning systems")

if __name__ == "__main__":
    main()