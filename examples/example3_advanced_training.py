#!/usr/bin/env python3
"""
Example 3: Training with Advanced API
Demonstrates advanced training capabilities using the Training API.
"""

import pynlm
from nlm.training import AdvancedAgentTrainer, TrainingConfig
import time
import numpy as np

def advanced_training_example():
    """Example of advanced training using the Advanced Agent Trainer."""
    print("=== Advanced NLM Training Example ===")
    
    # Initialize the advanced trainer
    trainer = AdvancedAgentTrainer()
    print("✓ Advanced trainer initialized")
    
    # Create training configuration
    config = trainer.create_training_config(
        max_episodes=300,
        max_steps_per_episode=200,
        evaluation_interval=50,
        checkpoint_interval=75,
        use_curriculum=True,
        use_adaptive_controls=True,
        enable_reward_modulation=True,
        enable_curiosity=True,
        enable_structural_plasticity=True,
        enable_development=True,
        success_threshold=0.5,
        patience=30,
    )
    print("✓ Training configuration created")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    print("✓ World configured")
    
    # Define training callback
    def training_callback(session, episode_result):
        if (session['episode'] + 1) % 50 == 0:
            print(f"Episode {session['episode'] + 1:3d}: "
                  f"Reward: {episode_result['final_reward']:6.2f}, "
                  f"Curiosity: {episode_result['avg_curiosity']:5.3f}, "
                  f"Survival: {episode_result['survival_time']:5.1f}s")
    
    print("\nStarting advanced training...")
    start_time = time.time()
    
    # Train the agent
    results = trainer.train_agent(
        config, 
        world, 
        callback=training_callback
    )
    
    end_time = time.time()
    print(f"\n✓ Training completed in {end_time - start_time:.2f} seconds")
    
    # Analyze training results
    print("\n=== Training Analysis ===")
    analysis = results['analysis']
    
    print(f"Training Dynamics:")
    print(f"  Convergence Rate: {analysis['training_dynamics']['convergence_rate']:.3f}")
    print(f"  Stability Score: {analysis['training_dynamics']['stability_score']:.3f}")
    print(f"  Exploration Efficiency: {analysis['training_dynamics']['exploration_efficiency']:.3f}")
    print(f"  Neural Stability: {analysis['training_dynamics']['neural_stability']:.3f}")
    
    print(f"\nPerformance Trajectory:")
    reward_trend = analysis['performance_trajectory']['reward_trend']
    print(f"  Reward Trend: {reward_trend['trend']} (rate: {reward_trend['rate']:.4f})")
    print(f"  Reward R-squared: {reward_trend['r_squared']:.3f}")
    
    curiosity_trend = analysis['performance_trajectory']['curiosity_trend']
    print(f"  Curiosity Trend: {curiosity_trend['trend']} (rate: {curiosity_trend['rate']:.4f})")
    
    # Show learning insights
    print(f"\nLearning Insights:")
    for insight in analysis['learning_insights']:
        print(f"  • {insight}")
    
    # Show optimization recommendations
    print(f"\nOptimization Recommendations:")
    for recommendation in analysis['optimization_recommendations']:
        print(f"  • {recommendation}")
    
    # Final evaluation
    print(f"\n=== Final Evaluation ===")
    final_eval = results['final_evaluation']
    
    print(f"Reward Performance:")
    print(f"  Mean: {final_eval['reward_statistics']['mean']:.3f}")
    print(f"  Std: {final_eval['reward_statistics']['std']:.3f}")
    print(f"  Success Rate: {final_eval['reward_statistics']['success_rate']:.1%}")
    
    print(f"\nExploration Performance:")
    print(f"  Mean Curiosity: {final_eval['curiosity_statistics']['mean']:.3f}")
    print(f"  Exploration Efficiency: {final_eval['curiosity_statistics']['coefficient_of_variation']:.3f}")
    
    # Check success criteria
    success_criteria = final_eval['success_criteria']
    print(f"\nSuccess Criteria:")
    for criterion, met in success_criteria.items():
        status = "✓" if met else "✗"
        print(f"  {status} {criterion.replace('_', ' ').title()}")
    
    overall_success = success_criteria['overall_success']
    print(f"\nOverall Success: {'✓ PASS' if overall_success else '✗ FAIL'}")
    
    # Save training results
    checkpoint_manager = trainer.checkpoint_manager
    checkpoint_manager.save_checkpoint(results['agent'], results['world'], 'final_training')
    
    print(f"\n✓ Training checkpoint saved")
    
    return results, analysis

def hyperparameter_optimization_example():
    """Example of hyperparameter optimization."""
    print("\n" + "="*60)
    print("=== Hyperparameter Optimization Example ===")
    
    # Define hyperparameter space
    hyperparameter_space = {
        'learning_rate': [0.0001, 0.001, 0.01],
        'curiosity_threshold': [0.3, 0.5, 0.7],
        'reward_scale': [0.5, 1.0, 2.0],
        'exploration_rate': [0.1, 0.3, 0.5],
    }
    
    print("Hyperparameter space:")
    for param, values in hyperparameter_space.items():
        print(f"  {param}: {values}")
    
    # Simple grid search example
    best_score = -float('inf')
    best_params = {}
    
    param_combinations = []
    for lr in hyperparameter_space['learning_rate']:
        for ct in hyperparameter_space['curiosity_threshold']:
            for rs in hyperparameter_space['reward_scale']:
                for er in hyperparameter_space['exploration_rate']:
                    param_combinations.append({
                        'learning_rate': lr,
                        'curiosity_threshold': ct,
                        'reward_scale': rs,
                        'exploration_rate': er,
                    })
    
    print(f"\nTesting {len(param_combinations)} hyperparameter combinations...")
    
    for i, params in enumerate(param_combinations):
        print(f"  Testing {i+1}/{len(param_combinations)}: {params}")
        
        # Create configuration
        config = TrainingConfig()
        config.neuron_count = 1000
        config.curiosity_threshold = params['curiosity_threshold']
        config.dopamine_scale = params['reward_scale']
        config.epsilon_start = params['exploration_rate']
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        
        # Train and evaluate
        trainer = AdvancedAgentTrainer()
        results = trainer.train_agent(config, world, max_episodes=50)
        
        # Evaluate performance
        from nlm.training import EvaluationMetrics
        eval_metrics = EvaluationMetrics.calculate_comprehensive_metrics(
            results['agent'], world, num_episodes=3
        )
        
        current_score = eval_metrics['reward_metrics']['mean']
        
        if current_score > best_score:
            best_score = current_score
            best_params = params.copy()
        
        print(f"    Score: {current_score:.4f}, Best: {best_score:.4f}")
    
    print(f"\n=== Hyperparameter Optimization Results ===")
    print(f"Best parameters: {best_params}")
    print(f"Best score: {best_score:.4f}")
    
    return best_params, best_score

if __name__ == "__main__":
    print("NLM Advanced Training and Evaluation Examples")
    print("=" * 60)
    
    # Example 1: Advanced training
    results1, analysis1 = advanced_training_example()
    
    # Example 2: Hyperparameter optimization
    best_params, best_score = hyperparameter_optimization_example()
    
    print("\n" + "="*60)
    print("All examples completed successfully!")
    print(f"\nSummary:")
    print(f"  - Advanced training completed")
    print(f"  - Best hyperparameters found: {best_params}")
    print(f"  - Best performance score: {best_score:.4f}")
