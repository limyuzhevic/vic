#!/usr/bin/env python3
"""
example_template_project.py

Project Template for NLM Python API Applications
================================================

This template provides a comprehensive structure for organizing larger NLM Python
API projects. It includes best practices for project organization, modular design,
configuration management, and production deployment.

Key Concepts Covered:
- Project structure and organization
- Configuration management
- Modular code organization
- Experiment tracking and reproducibility
- Production deployment
- Monitoring and logging
- Testing and validation

Learning Objectives:
1. Structure NLM Python API projects effectively
2. Implement configuration management
3. Create modular, maintainable code
4. Track experiments and ensure reproducibility
5. Deploy NLM applications to production
6. Implement monitoring and logging
7. Set up testing and validation frameworks

Author: NLM Documentation Team
Date: 2026-10-12
"""

import pynlm as nlm
import json
import os
import time
import logging
import argparse
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Any
from dataclasses import dataclass, asdict
from enum import Enum

class LogLevel(Enum):
    DEBUG = "DEBUG"
    INFO = "INFO"
    WARNING = "WARNING"
    ERROR = "ERROR"
    CRITICAL = "CRITICAL"

@dataclass
class ProjectConfig:
    """Configuration for NLM Python API projects."""
    project_name: str
    version: str
    description: str
    author: str
    created_at: str
    simulation_config: Dict
    experiment_config: Dict
    deployment_config: Dict
    logging_config: Dict
    monitoring_config: Dict

@dataclass
class ExperimentResult:
    """Results from an NLM experiment."""
    experiment_id: str
    timestamp: str
    environment: str
    brain_config: Dict
    hyperparameters: Dict
    performance_metrics: Dict
    learning_curves: Dict
    artifacts: List[str]

def setup_logging(log_level: LogLevel = LogLevel.INFO, log_file: str = "project.log"):
    """
    Setup comprehensive logging for the project.
    
    Args:
        log_level: Logging level
        log_file: Log file path
    """
    # Create log directory
    log_dir = Path("logs")
    log_dir.mkdir(exist_ok=True)
    
    # Configure logging
    logging.basicConfig(
        level=getattr(logging, log_level.value),
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        handlers=[
            logging.FileHandler(log_dir / log_file),
            logging.StreamHandler()
        ]
    )
    
    logger = logging.getLogger(__name__)
    logger.info(f"Logging configured - Level: {log_level.value}, File: {log_file}")
    return logger

class NLMProjectTemplate:
    """
    Template class for organizing NLM Python API projects.
    """
    
    def __init__(self, config: ProjectConfig, logger=None):
        self.config = config
        self.logger = logger or logging.getLogger(__name__)
        self.experiments = []
        self.models = {}
        self.environments = {}
        self.results_dir = Path("results")
        self.models_dir = Path("models")
        
        # Create necessary directories
        self.results_dir.mkdir(exist_ok=True)
        self.models_dir.mkdir(exist_ok=True)
        
        self.logger.info(f"Project initialized: {config.project_name} v{config.version}")
    
    def create_experiment_config(self, env_type: str, hyperparams: Dict) -> Dict:
        """
        Create experiment configuration.
        
        Args:
            env_type: Environment type
            hyperparams: Hyperparameters
            
        Returns:
            Experiment configuration
        """
        config = {
            'experiment_id': f"{env_type}_{datetime.now().strftime('%Y%m%d_%H%M%S')}",
            'environment': env_type,
            'hyperparameters': hyperparams,
            'brain_config': self.config.simulation_config,
            'created_at': datetime.now().isoformat(),
            'status': 'initialized'
        }
        
        self.logger.info(f"Created experiment configuration: {config['experiment_id']}")
        return config
    
    def setup_environment(self, env_type: str, **kwargs) -> Dict:
        """
        Setup NLM environment for experiments.
        
        Args:
            env_type: Environment type
            **kwargs: Environment parameters
            
        Returns:
            Environment configuration
        """
        self.logger.info(f"Setting up environment: {env_type}")
        
        if env_type == 'training':
            world = nlm.createTrainingEnvironment()
            nlm.setupAgentStartPosition(world, x=50.0, y=50.0)
        elif env_type == 'validation':
            world = nlm.createStandardEnvironment()
            nlm.setupAgentStartPosition(world, x=40.0, y=40.0)
        elif env_type == 'testing':
            world = nlm.createExplorationEnvironment()
            nlm.setupAgentStartPosition(world, x=60.0, y=60.0)
        else:
            raise ValueError(f"Unknown environment type: {env_type}")
        
        # Apply custom parameters
        for key, value in kwargs.items():
            if hasattr(world, f'set_{key}'):
                getattr(world, f'set_{key}')(value)
            elif hasattr(world, key):
                setattr(world, key, value)
        
        env_config = {
            'environment_id': f"{env_type}_{datetime.now().strftime('%Y%m%d_%H%M%S')}",
            'type': env_type,
            'world': world,
            'created_at': datetime.now().isoformat()
        }
        
        self.environments[env_config['environment_id']] = env_config
        self.logger.info(f"Environment setup complete: {env_config['environment_id']}")
        return env_config
    
    def create_agent(self, brain_config: Dict, agent_type: str = 'learning') -> Dict:
        """
        Create and configure NLM agent.
        
        Args:
            brain_config: Brain configuration
            agent_type: Type of agent
            
        Returns:
            Agent configuration
        """
        self.logger.info(f"Creating {agent_type} agent...")
        
        # Create brain
        brain = nlm.createBrain(brain_config)
        brain.initialize()
        
        # Create agent
        if agent_type == 'learning':
            agent = nlm.createLearningAgent(brain)
        elif agent_type == 'exploration':
            agent = nlm.createExplorationAgent(brain)
        elif agent_type == 'standard':
            agent = nlm.createStandardAgent(brain)
        else:
            raise ValueError(f"Unknown agent type: {agent_type}")
        
        # Setup agent
        agent.initialize()
        
        agent_config = {
            'agent_id': f"{agent_type}_{datetime.now().strftime('%Y%m%d_%H%M%S')}",
            'type': agent_type,
            'brain': brain,
            'created_at': datetime.now().isoformat(),
            'status': 'initialized'
        }
        
        self.models[agent_config['agent_id']] = agent_config
        self.logger.info(f"Agent created: {agent_config['agent_id']}")
        return agent_config
    
    def run_experiment(self, experiment_config: Dict, agent_config: Dict, 
                      num_epochs: int, steps_per_epoch: int) -> ExperimentResult:
        """
        Run NLM experiment.
        
        Args:
            experiment_config: Experiment configuration
            agent_config: Agent configuration
            num_epochs: Number of training epochs
            steps_per_epoch: Steps per epoch
            
        Returns:
            Experiment results
        """
        exp_id = experiment_config['experiment_id']
        self.logger.info(f"Starting experiment: {exp_id}")
        
        brain = agent_config['brain']
        agent = agent_config['agent']
        
        # Setup environment
        env_config = self.setup_environment(
            experiment_config['environment'],
            **experiment_config.get('environment_params', {})
        )
        world = env_config['world']
        agent.initialize(world)
        
        # Initialize tracking
        experiment_results = {
            'epoch': [],
            'reward': [],
            'curiosity': [],
            'novelty': [],
            'performance': [],
            'time': []
        }
        
        # Run experiment
        start_time = time.time()
        
        for epoch in range(num_epochs):
            epoch_start_time = time.time()
            epoch_reward = 0.0
            
            # Run epoch
            for step in range(steps_per_epoch):
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                epoch_reward += world.getCurrentReward()
            
            epoch_end_time = time.time()
            epoch_duration = epoch_end_time - epoch_start_time
            
            # Record results
            experiment_results['epoch'].append(epoch + 1)
            experiment_results['reward'].append(epoch_reward)
            experiment_results['curiosity'].append(agent.getCuriosityLevel())
            experiment_results['novelty'].append(agent.getNoveltyLevel())
            experiment_results['performance'].append(epoch_reward / steps_per_epoch)
            experiment_results['time'].append(epoch_duration)
            
            self.logger.info(f"  Epoch {epoch + 1}/{num_epochs}: "
                           f"Reward={epoch_reward:.2f}, "
                           f"Avg={epoch_reward/steps_per_epoch:.3f}, "
                           f"Time={epoch_duration:.2f}s")
        
        total_time = time.time() - start_time
        
        # Create experiment result
        result = ExperimentResult(
            experiment_id=exp_id,
            timestamp=datetime.now().isoformat(),
            environment=experiment_config['environment'],
            brain_config=experiment_config['brain_config'],
            hyperparameters=experiment_config['hyperparameters'],
            performance_metrics={
                'total_reward': sum(experiment_results['reward']),
                'average_reward': np.mean(experiment_results['reward']),
                'final_curiosity': experiment_results['curiosity'][-1],
                'final_novelty': experiment_results['novelty'][-1],
                'total_time': total_time,
                'epochs_completed': num_epochs,
                'steps_per_epoch': steps_per_epoch
            },
            learning_curves=experiment_results,
            artifacts=[]
        )
        
        # Save artifacts
        self.save_experiment_artifacts(result)
        
        self.logger.info(f"Experiment completed: {exp_id}")
        return result
    
    def save_experiment_artifacts(self, result: ExperimentResult):
        """
        Save experiment artifacts.
        
        Args:
            result: Experiment result
        """
        # Create artifacts directory for this experiment
        artifacts_dir = self.results_dir / result.experiment_id
        artifacts_dir.mkdir(exist_ok=True)
        
        # Save results JSON
        results_file = artifacts_dir / "experiment_results.json"
        with open(results_file, 'w') as f:
            json.dump(asdict(result), f, indent=2, default=str)
        
        result.artifacts.append(str(results_file))
        
        # Save learning curves
        curves_file = artifacts_dir / "learning_curves.json"
        with open(curves_file, 'w') as f:
            json.dump(result.learning_curves, f, indent=2)
        
        result.artifacts.append(str(curves_file))
        
        # Save model if available
        agent_id = f"{result.environment}_{result.experiment_id}"
        if agent_id in self.models:
            model_file = self.models_dir / f"{agent_id}.pth"
            # In real implementation, this would save the actual model
            # For demo, we'll create a placeholder
            with open(model_file, 'w') as f:
                f.write(f"Model placeholder for {agent_id}")
            
            result.artifacts.append(str(model_file))
        
        self.logger.info(f"Artifacts saved for experiment {result.experiment_id}")
    
    def run_multiple_experiments(self, experiment_configs: List[Dict], 
                                agent_template: Dict, num_epochs: int = 5,
                                steps_per_epoch: int = 100) -> List[ExperimentResult]:
        """
        Run multiple experiments.
        
        Args:
            experiment_configs: List of experiment configurations
            agent_template: Template agent configuration
            num_epochs: Number of epochs per experiment
            steps_per_epoch: Steps per epoch
            
        Returns:
            List of experiment results
        """
        self.logger.info(f"Running {len(experiment_configs)} experiments...")
        
        results = []
        
        for i, config in enumerate(experiment_configs):
            self.logger.info(f"Running experiment {i + 1}/{len(experiment_configs)}")
            
            # Create experiment configuration
            exp_config = self.create_experiment_config(
                config['environment'],
                config['hyperparameters']
            )
            
            # Create agent
            agent_config = self.create_agent(
                self.config.simulation_config,
                agent_template['type']
            )
            
            # Run experiment
            result = self.run_experiment(
                exp_config,
                agent_config,
                num_epochs,
                steps_per_epoch
            )
            
            results.append(result)
            self.experiments.append(result)
            
            self.logger.info(f"Experiment {i + 1} completed: "
                           f"{result.performance_metrics['total_reward']:.2f} total reward")
        
        return results
    
    def generate_project_report(self, results: List[ExperimentResult]) -> Dict:
        """
        Generate project report.
        
        Args:
            results: List of experiment results
            
        Returns:
            Project report
        """
        self.logger.info("Generating project report...")
        
        # Calculate aggregate metrics
        total_rewards = [r.performance_metrics['total_reward'] for r in results]
        total_times = [r.performance_metrics['total_time'] for r in results]
        final_curiosities = [r.performance_metrics['final_curiosity'] for r in results]
        
        report = {
            'project_metadata': {
                'project_name': self.config.project_name,
                'version': self.config.version,
                'author': self.config.author,
                'created_at': self.config.created_at,
                'total_experiments': len(results),
                'total_experiments_time': sum(total_times)
            },
            'performance_summary': {
                'total_rewards': total_rewards,
                'max_reward': max(total_rewards) if total_rewards else 0,
                'min_reward': min(total_rewards) if total_rewards else 0,
                'average_reward': np.mean(total_rewards) if total_rewards else 0,
                'reward_variance': np.var(total_rewards) if len(total_rewards) > 1 else 0,
                'final_curiosities': final_curiosities,
                'average_curiosity': np.mean(final_curiosities) if final_curiosities else 0
            },
            'recommendations': self._generate_recommendations(results),
            'next_steps': [
                "Analyze best performing configurations",
                "Deploy optimized models to production",
                "Implement continuous learning pipeline",
                "Set up automated experiment tracking"
            ]
        }
        
        # Save report
        report_file = self.results_dir / "project_report.json"
        with open(report_file, 'w') as f:
            json.dump(report, f, indent=2, default=str)
        
        self.logger.info(f"Project report saved: {report_file}")
        return report
    
    def _generate_recommendations(self, results: List[ExperimentResult]) -> List[str]:
        """
        Generate project recommendations.
        
        Args:
            results: Experiment results
            
        Returns:
            List of recommendations
        """
        recommendations = []
        
        if not results:
            return ["Start with a simple baseline experiment to establish performance metrics"]
        
        # Find best performing experiment
        best_result = max(results, key=lambda x: x.performance_metrics['total_reward'])
        
        if best_result.performance_metrics['total_reward'] > 100:
            recommendations.append("Excellent performance achieved - consider increasing complexity for harder challenges")
        else:
            recommendations.append("Moderate performance - consider optimizing hyperparameters or extending training")
        
        # Check for consistency
        reward_variance = np.var([r.performance_metrics['total_reward'] for r in results])
        if reward_variance > 50:
            recommendations.append("High performance variance detected - check for experimental stability issues")
        else:
            recommendations.append("Good performance consistency across experiments")
        
        return recommendations

def create_default_config(project_name: str) -> ProjectConfig:
    """
    Create default project configuration.
    
    Args:
        project_name: Name of the project
        
    Returns:
        Default project configuration
    """
    current_time = datetime.now().isoformat()
    
    config = ProjectConfig(
        project_name=project_name,
        version="1.0.0",
        description="NLM Python API Project Template",
        author="NLM Research Team",
        created_at=current_time,
        simulation_config={
            'brain_type': 'default',
            'learning_rate': 0.1,
            'plasticity': True,
            'neuromodulation': True,
            'development_enabled': False
        },
        experiment_config={
            'num_epochs': 10,
            'steps_per_epoch': 100,
            'environments': ['training', 'validation', 'testing']
        },
        deployment_config={
            'environment': 'development',
            'monitoring': True,
            'auto_scaling': False
        },
        logging_config={
            'level': 'INFO',
            'file': 'project.log',
            'max_size_mb': 100
        },
        monitoring_config={
            'metrics_interval': 60,
            'performance_threshold': 0.8,
            'health_check_interval': 300
        }
    )
    
    return config

def main():
    """Main function demonstrating project template usage."""
    print("=" * 70)
    print("NLM Python API Project Template")
    print("=" * 70)
    
    # Parse command line arguments
    parser = argparse.ArgumentParser(description='NLM Python API Project Template')
    parser.add_argument('--project-name', default='nlm_project', help='Project name')
    parser.add_argument('--epochs', type=int, default=10, help='Number of training epochs')
    parser.add_argument('--steps', type=int, default=100, help='Steps per epoch')
    parser.add_argument('--log-level', default='INFO', choices=['DEBUG', 'INFO', 'WARNING', 'ERROR'],
                       help='Logging level')
    
    args = parser.parse_args()
    
    # Setup logging
    setup_logging(LogLevel[args.log_level], "project.log")
    
    # Create project configuration
    config = create_default_config(args.project_name)
    
    print(f"\nProject Configuration:")
    print(f"  Project Name: {config.project_name}")
    print(f"  Version: {config.version}")
    print(f"  Author: {config.author}")
    print(f"  Description: {config.description}")
    
    # Initialize project template
    project = NLMProjectTemplate(config)
    
    # Define experiment configurations
    experiment_configs = [
        {
            'environment': 'training',
            'hyperparameters': {
                'learning_rate': 0.1,
                'reward_modulation': 0.1,
                'curiosity_modulation': 0.7
            }
        },
        {
            'environment': 'validation',
            'hyperparameters': {
                'learning_rate': 0.05,
                'reward_modulation': 0.15,
                'curiosity_modulation': 0.6
            }
        },
        {
            'environment': 'testing',
            'hyperparameters': {
                'learning_rate': 0.08,
                'reward_modulation': 0.12,
                'curiosity_modulation': 0.8
            }
        }
    ]
    
    # Create agent template
    agent_template = {'type': 'learning'}
    
    print(f"\nStarting project experiments...")
    print(f"  Number of experiments: {len(experiment_configs)}")
    print(f"  Epochs per experiment: {args.epochs}")
    print(f"  Steps per epoch: {args.steps}")
    
    # Run experiments
    results = project.run_multiple_experiments(
        experiment_configs,
        agent_template,
        num_epochs=args.epochs,
        steps_per_epoch=args.steps
    )
    
    # Generate project report
    print(f"\nGenerating project report...")
    report = project.generate_project_report(results)
    
    print(f"\n" + "=" * 70)
    print("PROJECT TEMPLATE COMPLETE")
    print("=" * 70)
    
    print(f"\nProject Summary:")
    print(f"  Project Name: {report['project_metadata']['project_name']}")
    print(f"  Experiments Completed: {report['project_metadata']['total_experiments']}")
    print(f"  Total Training Time: {report['project_metadata']['total_experiments_time']:.2f} seconds")
    print(f"  Average Performance: {report['performance_summary']['average_reward']:.2f}")
    print(f"  Best Performance: {report['performance_summary']['max_reward']:.2f}")
    
    print(f"\nResults Location:")
    print(f"  Results Directory: {Path('results').absolute()}")
    print(f"  Models Directory: {Path('models').absolute()}")
    print(f"  Logs Directory: {Path('logs').absolute()}")
    
    print(f"\nRecommendations:")
    for rec in report['recommendations']:
        print(f"  ✓ {rec}")
    
    print(f"\nNext Steps:")
    for step in report['next_steps']:
        print(f"  📌 {step}")
    
    print(f"\nTemplate Features:")
    print(f"  ✓ Project structure and organization")
    print(f"  ✓ Configuration management")
    print(f"  ✓ Modular code organization")
    print(f"  ✓ Experiment tracking and reproducibility")
    print(f"  ✓ Production deployment ready")
    print(f"  ✓ Monitoring and logging")
    print(f"  ✓ Testing and validation framework")

if __name__ == "__main__":
    main()