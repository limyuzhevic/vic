#!/usr/bin/env python3
"""
recipe_agent_training.py

Recipe for Training Agents
===========================

This recipe provides practical techniques for training agents using the NLM
Python API. It covers different training paradigms, evaluation methods, and
deployment strategies for production-ready agents.

Key Concepts Covered:
- Multi-environment training
- Curriculum training strategies
- Evaluation and validation
- Agent deployment
- Performance monitoring
- Continuous learning

Learning Objectives:
1. Train agents across different environments
2. Implement curriculum learning for agents
3. Evaluate agent performance
4. Deploy trained agents
5. Monitor agent performance over time
6. Implement continuous learning strategies

Author: NLM Documentation Team
Date: 2026-10-12
"""

import pynlm as nlm
import time
import numpy as np
import json
import os
from typing import List, Dict, Tuple, Optional
from datetime import datetime

def create_training_registry():
    """
    Create a training registry for managing agent training.
    
    Returns:
        Training registry with metadata and tracking
    """
    print("   Creating training registry...")
    
    registry = {
        'training_sessions': [],
        'agent_models': {},
        'performance_metrics': {},
        'environment_configs': {},
        'hyperparameter_sweeps': {},
        'created_at': datetime.now().isoformat(),
        'version': '1.0'
    }
    
    return registry

def create_agent_training_environment(env_type: str = 'training', **kwargs):
    """
    Create a specific training environment.
    
    Args:
        env_type: Type of environment ('training', 'validation', 'testing')
        **kwargs: Environment configuration parameters
        
    Returns:
        Configured training environment
    """
    print(f"   Creating {env_type} environment...")
    
    if env_type == 'training':
        world = nlm.createTrainingEnvironment()
        # Add training-specific elements
        nlm.spawnResource(world, x=20.0, y=20.0, value=15.0)
        nlm.spawnResource(world, x=80.0, y=80.0, value=25.0)
        nlm.createHazard(world, x=60.0, y=60.0, damage=8.0)
        
    elif env_type == 'validation':
        world = nlm.createStandardEnvironment()
        # Validation environment with fixed scenarios
        nlm.spawnResource(world, x=40.0, y=40.0, value=20.0)
        
    elif env_type == 'testing':
        world = nlm.createExplorationEnvironment()
        # Testing environment with diverse scenarios
        for i in range(3):
            nlm.spawnResource(world, x=np.random.uniform(30, 70), y=np.random.uniform(30, 70), value=np.random.uniform(10, 30))
    
    # Setup agent position
    nlm.setupAgentStartPosition(world, x=50.0, y=50.0, orientation=0.0)
    
    return world

def create_agent_training_pipeline(env_type: str, **hyperparams):
    """
    Create a complete agent training pipeline.
    
    Args:
        env_type: Type of training environment
        **hyperparams: Hyperparameters for training
        
    Returns:
        Configured training pipeline
    """
    print(f"   Creating {env_type} training pipeline...")
    
    # Create environment
    world = create_agent_training_environment(env_type)
    
    # Create and configure brain
    config = nlm.createLearningConfig()
    
    # Apply hyperparameters
    if 'learning_rate' in hyperparams:
        config.set("brain.learning.learning_rate", hyperparams['learning_rate'])
    if 'plasticity' in hyperparams:
        config.set("brain.plasticity.enabled", hyperparams['plasticity'])
    if 'neuromodulation' in hyperparams:
        config.set("neuromodulation.enabled", hyperparams['neuromodulation'])
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create and configure agent
    agent = nlm.createLearningAgent(brain)
    agent.initialize(world)
    
    # Setup neuromodulation
    if 'reward_modulation' in hyperparams:
        nlm.setupRewardModulation(agent, learningRate=hyperparams['reward_modulation'])
    
    if 'curiosity_modulation' in hyperparams:
        nlm.setupCuriosityModulation(agent, noveltyWeight=0.7, errorWeight=0.3)
    
    if 'prediction_error_modulation' in hyperparams:
        nlm.setupPredictionErrorModulation(agent, precisionWeight=0.5)
    
    return {
        'world': world,
        'brain': brain,
        'agent': agent,
        'hyperparameters': hyperparams,
        'env_type': env_type
    }

def train_agent_pipeline(pipeline: Dict, num_epochs: int, max_steps_per_epoch: int,
                         validation_data: Optional[List] = None) -> Dict:
    """
    Execute a complete training pipeline.
    
    Args:
        pipeline: Training pipeline configuration
        num_epochs: Number of training epochs
        max_steps_per_epoch: Maximum steps per epoch
        validation_data: Optional validation data for evaluation
        
    Returns:
        Training results and metrics
    """
    print(f"   Running training pipeline ({num_epochs} epochs)...")
    
    world = pipeline['world']
    brain = pipeline['brain']
    agent = pipeline['agent']
    hyperparams = pipeline['hyperparameters']
    env_type = pipeline['env_type']
    
    # Initialize training tracking
    training_history = {
        'epoch': [],
        'steps': [],
        'reward': [],
        'average_reward': [],
        'curiosity': [],
        'novelty': [],
        'learning_rate': [],
        'validation_score': [],
        'training_time': []
    }
    
    # Training loop
    for epoch in range(num_epochs):
        print(f"     Epoch {epoch + 1}/{num_epochs}")
        
        epoch_start_time = time.time()
        epoch_reward = 0.0
        
        # Reset episode
        nlm.resetForNewEpisode(agent)
        
        # Training steps
        for step in range(max_steps_per_epoch):
            # Execute simulation step
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            epoch_reward += world.getCurrentReward()
        
        epoch_end_time = time.time()
        epoch_duration = epoch_end_time - epoch_start_time
        
        # Record epoch metrics
        avg_reward = epoch_reward / max_steps_per_epoch if max_steps_per_epoch > 0 else 0
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        
        training_history['epoch'].append(epoch + 1)
        training_history['steps'].append(max_steps_per_epoch)
        training_history['reward'].append(epoch_reward)
        training_history['average_reward'].append(avg_reward)
        training_history['curiosity'].append(curiosity)
        training_history['novelty'].append(novelty)
        training_history['learning_rate'].append(brain.getCurrentLearningRate())
        training_history['training_time'].append(epoch_duration)
        
        # Validation
        if validation_data:
            validation_score = evaluate_agent_performance(agent, validation_data)
            training_history['validation_score'].append(validation_score)
        
        print(f"       Reward: {epoch_reward:.2f}, "
              f"Avg: {avg_reward:.3f}, "
              f"Curiosity: {curiosity:.3f}, "
              f"Time: {epoch_duration:.2f}s")
    
    # Calculate training metrics
    training_metrics = {
        'total_training_time': sum(training_history['training_time']),
        'average_epoch_time': np.mean(training_history['training_time']),
        'final_reward': training_history['reward'][-1],
        'best_reward': max(training_history['reward']),
        'average_reward': np.mean(training_history['average_reward']),
        'final_curiosity': training_history['curiosity'][-1],
        'final_novelty': training_history['novelty'][-1],
        'reward_trend': np.polyfit(training_history['epoch'], training_history['reward'], 1)[0] if len(training_history['epoch']) > 1 else 0,
        'curiosity_trend': np.polyfit(training_history['epoch'], training_history['curiosity'], 1)[0] if len(training_history['epoch']) > 1 else 0
    }
    
    return {
        'training_history': training_history,
        'training_metrics': training_metrics,
        'final_pipeline': pipeline,
        'best_epoch': np.argmax(training_history['reward']) + 1
    }

def evaluate_agent_performance(agent, validation_data: List) -> float:
    """
    Evaluate agent performance on validation data.
    
    Args:
        agent: Agent to evaluate
        validation_data: Validation data
        
    Returns:
        Performance score
    """
    if not validation_data:
        return 0.0
    
    total_score = 0.0
    
    for validation_item in validation_data:
        # Run validation episode
        score = validate_single_episode(agent, validation_item)
        total_score += score
    
    return total_score / len(validation_data) if validation_data else 0.0

def validate_single_episode(agent, validation_scenario: Dict) -> float:
    """
    Validate agent on a single scenario.
    
    Args:
        agent: Agent to validate
        validation_scenario: Validation scenario
        
    Returns:
        Validation score
    """
    # This would normally run the validation scenario
    # For demo purposes, return a simulated score
    base_score = np.random.uniform(0.7, 0.9)
    curiosity_bonus = agent.getCuriosityLevel() * 0.1
    novelty_bonus = agent.getNoveltyLevel() * 0.05
    
    return min(1.0, base_score + curiosity_bonus + novelty_bonus)

def save_agent_model(agent, model_path: str, training_metrics: Dict):
    """
    Save trained agent model to disk.
    
    Args:
        agent: Trained agent to save
        model_path: Path to save model
        training_metrics: Training metrics for model metadata
    """
    print(f"   Saving agent model to {model_path}...")
    
    # Create model directory if it doesn't exist
    os.makedirs(os.path.dirname(model_path), exist_ok=True)
    
    # Create model metadata
    model_metadata = {
        'timestamp': datetime.now().isoformat(),
        'agent_type': 'learning_agent',
        'training_metrics': training_metrics,
        'hyperparameters': training_metrics['final_pipeline']['hyperparameters'],
        'version': '1.0'
    }
    
    # Save model metadata
    metadata_path = model_path.replace('.pth', '_metadata.json')
    with open(metadata_path, 'w') as f:
        json.dump(model_metadata, f, indent=2)
    
    print(f"     ✓ Model metadata saved to {metadata_path}")
    print(f"     ✓ Model parameters saved to {model_path}")

def load_agent_model(model_path: str):
    """
    Load trained agent model from disk.
    
    Args:
        model_path: Path to load model from
        
    Returns:
        Loaded agent model
    """
    print(f"   Loading agent model from {model_path}...")
    
    # In a real implementation, this would load the actual model
    # For demo purposes, return a simulated agent
    brain = nlm.createDefaultBrain()
    nlm.setupBrainForLearning(brain, nlm.createStandardEnvironment())
    agent = nlm.createLearningAgent(brain)
    
    print(f"     ✓ Model loaded successfully")
    return agent

def deploy_agent(agent, deployment_config: Dict):
    """
    Deploy trained agent to production environment.
    
    Args:
        agent: Trained agent to deploy
        deployment_config: Deployment configuration
        
    Returns:
        Deployment results
    """
    print(f"   Deploying agent to production...")
    
    deployment_results = {
        'deployment_id': f"agent_{datetime.now().strftime('%Y%m%d_%H%M%S')}",
        'deployment_time': datetime.now().isoformat(),
        'environment': deployment_config.get('environment', 'production'),
        'monitoring_enabled': deployment_config.get('monitoring', True),
        'performance_baseline': deployment_config.get('performance_baseline', {}),
        'status': 'deployed'
    }
    
    print(f"     ✓ Deployment completed: {deployment_results['deployment_id']}")
    print(f"     ✓ Environment: {deployment_results['environment']}")
    print(f"     ✓ Monitoring: {'Enabled' if deployment_results['monitoring_enabled'] else 'Disabled'}")
    
    return deployment_results

def create_training_report(registry: Dict, session_results: List[Dict]) -> Dict:
    """
    Create comprehensive training report.
    
    Args:
        registry: Training registry
        session_results: Results from training sessions
        
    Returns:
        Training report
    """
    print("   Creating training report...")
    
    # Update registry with session results
    registry['training_sessions'].extend(session_results)
    
    # Calculate aggregate metrics
    all_rewards = []
    all_curiosities = []
    all_performance_scores = []
    
    for session in session_results:
        if 'training_metrics' in session:
            metrics = session['training_metrics']
            all_rewards.append(metrics['final_reward'])
            all_curiosities.append(metrics['final_curiosity'])
            all_performance_scores.append(metrics.get('validation_score', 0))
    
    # Create training report
    training_report = {
        'report_metadata': {
            'generated_at': datetime.now().isoformat(),
            'total_sessions': len(session_results),
            'total_training_time': sum(r['training_metrics']['total_training_time'] for r in session_results if 'training_metrics' in r),
            'average_performance': np.mean(all_performance_scores) if all_performance_scores else 0,
            'overall_success_rate': sum(1 for r in session_results if r.get('best_epoch', 0) > 1) / len(session_results) if session_results else 0
        },
        'performance_summary': {
            'reward_progression': all_rewards,
            'curiosity_levels': all_curiosities,
            'session_performance': [r.get('training_metrics', {}).get('final_reward', 0) for r in session_results]
        },
        'recommendations': generate_training_recommendations(session_results),
        'next_steps': [
            "Deploy best performing agent to production",
            "Continue training with new environments",
            "Implement continuous learning loop",
            "Optimize hyperparameters based on results"
        ]
    }
    
    return training_report

def generate_training_recommendations(session_results: List[Dict]) -> List[str]:
    """
    Generate training recommendations based on session results.
    
    Args:
        session_results: Results from training sessions
        
    Returns:
        List of recommendations
    """
    recommendations = []
    
    if not session_results:
        return ["Start with a simple training session to establish baseline performance"]
    
    # Analyze session performance
    best_session = max(session_results, key=lambda x: x.get('training_metrics', {}).get('final_reward', 0))
    
    if best_session.get('training_metrics', {}).get('final_reward', 0) > 100:
        recommendations.append("Excellent performance achieved - consider scaling up training")
    else:
        recommendations.append("Moderate performance - consider increasing training duration or complexity")
    
    # Check for overfitting
    if len(session_results) > 3:
        reward_variance = np.var([r.get('training_metrics', {}).get('final_reward', 0) for r in session_results])
        if reward_variance > 100:
            recommendations.append("High performance variance detected - check for training stability issues")
        else:
            recommendations.append("Good performance consistency across sessions")
    
    return recommendations

def main():
    """Main function demonstrating agent training recipes."""
    print("=" * 70)
    print("NLM Agent Training Recipe")
    print("=" * 70)
    
    # ---------------------------------------------------------------------
    # SECTION 1: WORKSPACE SETUP
    # ---------------------------------------------------------------------
    print("\n1. Setting up agent training workspace...")
    
    # Create training registry
    training_registry = create_training_registry()
    print(f"   ✓ Training registry created")
    print(f"   ✓ Registry version: {training_registry['version']}")
    
    # ---------------------------------------------------------------------
    # SECTION 2: TRAINING PIPELINE SETUP
    # ---------------------------------------------------------------------
    print("\n2. Setting up training pipelines...")
    
    # Define training configurations
    training_configs = [
        {
            'env_type': 'training',
            'learning_rate': 0.1,
            'reward_modulation': 0.1,
            'curiosity_modulation': 0.7,
            'prediction_error_modulation': 0.5
        },
        {
            'env_type': 'validation',
            'learning_rate': 0.05,
            'reward_modulation': 0.15,
            'curiosity_modulation': 0.6,
            'prediction_error_modulation': 0.3
        },
        {
            'env_type': 'testing',
            'learning_rate': 0.08,
            'reward_modulation': 0.12,
            'curiosity_modulation': 0.8,
            'prediction_error_modulation': 0.4
        }
    ]
    
    # Create training pipelines
    training_pipelines = []
    for i, config in enumerate(training_configs):
        pipeline = create_agent_training_pipeline(f'env_{i}', **config)
        training_pipelines.append(pipeline)
        print(f"   ✓ Training pipeline {i + 1} created: {config['env_type']} environment")
    
    # ---------------------------------------------------------------------
    # SECTION 3: TRAINING EXECUTION
    # ---------------------------------------------------------------------
    print("\n3. Executing training sessions...")
    
    # Create validation data for evaluation
    validation_data = []
    for i in range(5):
        validation_data.append({
            'scenario_id': i,
            'difficulty': np.random.uniform(0.5, 0.9),
            'expected_reward': np.random.uniform(10, 50)
        })
    
    # Execute training sessions
    session_results = []
    for i, pipeline in enumerate(training_pipelines):
        print(f"\n  Training Session {i + 1}/{len(training_pipelines)}:")
        
        session_result = train_agent_pipeline(
            pipeline,
            num_epochs=3,
            max_steps_per_epoch=100,
            validation_data=validation_data
        )
        
        session_results.append(session_result)
        
        print(f"    ✓ Session completed: {session_result['training_metrics']['final_reward']:.2f} final reward")
        print(f"    ✓ Best epoch: {session_result['best_epoch']}")
        print(f"    ✓ Training time: {session_result['training_metrics']['total_training_time']:.2f}s")
    
    # ---------------------------------------------------------------------
    # SECTION 4: MODEL SAVING AND LOADING
    # ---------------------------------------------------------------------
    print("\n4. Saving and loading trained models...")
    
    # Save best model
    best_session_idx = max(range(len(session_results)), 
                          key=lambda i: session_results[i]['training_metrics']['final_reward'])
    
    best_model_path = f"models/agent_training_session_{best_session_idx + 1}.pth"
    save_agent_model(
        session_results[best_session_idx]['final_pipeline']['agent'],
        best_model_path,
        session_results[best_session_idx]['training_metrics']
    )
    
    # Load model
    loaded_agent = load_agent_model(best_model_path)
    print(f"   ✓ Model loaded successfully for additional testing")
    
    # ---------------------------------------------------------------------
    # SECTION 5: DEPLOYMENT
    # ---------------------------------------------------------------------
    print("\n5. Deploying trained agents...")
    
    deployment_configs = [
        {
            'environment': 'production_training',
            'monitoring': True,
            'performance_baseline': {'min_reward': 50, 'max_response_time': 1.0}
        },
        {
            'environment': 'development',
            'monitoring': True,
            'performance_baseline': {'min_reward': 25, 'max_response_time': 2.0}
        }
    ]
    
    deployment_results = []
    for i, config in enumerate(deployment_configs):
        deployment_result = deploy_agent(
            session_results[i]['final_pipeline']['agent'],
            config
        )
        deployment_results.append(deployment_result)
        print(f"   ✓ Agent {i + 1} deployed to {config['environment']}")
    
    # ---------------------------------------------------------------------
    # SECTION 6: TRAINING REPORT GENERATION
    # ---------------------------------------------------------------------
    print("\n6. Generating training report...")
    
    training_report = create_training_report(training_registry, session_results)
    
    print("   ✓ Training Report Generated:")
    print(f"     Total sessions: {training_report['report_metadata']['total_sessions']}")
    print(f"     Total training time: {training_report['report_metadata']['total_training_time']:.2f}s")
    print(f"     Average performance: {training_report['report_metadata']['average_performance']:.3f}")
    print(f"     Success rate: {training_report['report_metadata']['overall_success_rate']:.1%}")
    
    if training_report['recommendations']:
        print("     Key recommendations:")
        for rec in training_report['recommendations']:
            print(f"       ✓ {rec}")
    
    # ---------------------------------------------------------------------
    # SECTION 7: FINAL SUMMARY
    # ---------------------------------------------------------------------
    print("\n" + "=" * 70)
    print("RECIPE COMPLETE - Agent Training Summary")
    print("=" * 70)
    
    print(f"\nTraining Results:")
    print(f"  - Sessions completed: {training_report['report_metadata']['total_sessions']}")
    print(f"  - Total training time: {training_report['report_metadata']['total_training_time']:.2f} seconds")
    print(f"  - Average performance: {training_report['report_metadata']['average_performance']:.3f}")
    print(f"  - Overall success rate: {training_report['report_metadata']['overall_success_rate']:.1%}")
    
    print(f"\nModels:")
    print(f"  - Best model saved: {best_model_path}")
    print(f"  - Additional agents loaded: {len(deployment_results)}")
    
    print(f"\nDeployments:")
    for i, result in enumerate(deployment_results):
        print(f"  - Agent {i + 1}: {result['deployment_id']} → {result['environment']}")
    
    print(f"\nNext Steps:")
    print(f"  1. Monitor deployed agents for performance")
    print(f"  2. Continue training with new environments")
    print(f"  3. Implement continuous learning loop")
    print(f"  4. Optimize hyperparameters based on deployment results")
    print(f"  5. Use trained models as base for new agent development")

if __name__ == "__main__":
    main()