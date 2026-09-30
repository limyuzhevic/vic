#!/usr/bin/env python3
"""
Experiment management framework for NLM Python API

This module provides comprehensive experiment management capabilities
for NLM neural simulation research and testing.

Usage:
    import nlm
    from nlm.experiments import Experiment, ExperimentSuite, ExperimentResults
    
    # Create an experiment
    experiment = Experiment("my_experiment")
    experiment.run()
    
    # Create experiment suite
    suite = ExperimentSuite("research_suite")
    suite.add_experiment(experiment1)
    suite.add_experiment(experiment2)
    suite.run_all()
"""

import json
import yaml
import numpy as np
from typing import Dict, List, Any, Optional, Callable, Union
from datetime import datetime
import os
import uuid
from dataclasses import dataclass, asdict
from enum import Enum

# Import NLM core classes
from .pynlm import (
    Config, 
    Brain, 
    SimpleWorld, 
    Action,
    SensoryPercept,
    ActionType,
    Vision,
    Audio,
    InternalSignals
)

class ExperimentStatus(Enum):
    """Experiment execution status."""
    PENDING = "pending"
    RUNNING = "running"
    COMPLETED = "completed"
    FAILED = "failed"
    CANCELLED = "cancelled"

class ExperimentType(Enum):
    """Experiment types."""
    BASIC = "basic"
    PARAMETER_SWEEP = "parameter_sweep"
    MULTI_ENVIRONMENT = "multi_environment"
    AGENT_RESEARCH = "agent_research"
    BENCHMARK = "benchmark"
    COMPARISON = "comparison"

@dataclass
class ExperimentConfig:
    """Experiment configuration."""
    name: str
    description: str
    experiment_type: ExperimentType
    config_file: Optional[str] = None
    results_file: Optional[str] = None
    parameters: Dict[str, Any] = None
    created_at: str = None
    last_modified: str = None
    
    def __post_init__(self):
        if self.parameters is None:
            self.parameters = {}
        if self.created_at is None:
            self.created_at = datetime.now().isoformat()
        if self.last_modified is None:
            self.last_modified = datetime.now().isoformat()

@dataclass
class ExperimentResult:
    """Experiment result data."""
    experiment_name: str
    status: ExperimentStatus
    start_time: Optional[str]
    end_time: Optional[str]
    duration: Optional[float]
    results: Dict[str, Any]
    metadata: Dict[str, Any] = None
    error_message: Optional[str] = None
    
    def __post_init__(self):
        if self.metadata is None:
            self.metadata = {}

class Experiment:
    """Base experiment class for NLM simulations.
    
    This class provides a foundation for creating and running NLM experiments
    with comprehensive configuration management, execution tracking, and
    results analysis.
    
    Attributes:
        config (ExperimentConfig): Experiment configuration
        results (ExperimentResult): Experiment results
        brain_factory (callable): Function to create brain instances
        world_factory (callable): Function to create world instances
        status (ExperimentStatus): Current experiment status
    """
    
    def __init__(self, config: ExperimentConfig):
        """Initialize experiment.
        
        Args:
            config (ExperimentConfig): Experiment configuration
        """
        self.config = config
        self.results = None
        self.brain_factory = None
        self.world_factory = None
        self.status = ExperimentStatus.PENDING
        
        # Update last modified time
        self.config.last_modified = datetime.now().isoformat()
    
    def set_factories(self, brain_factory: Callable, world_factory: Callable):
        """Set brain and world factories.
        
        Args:
            brain_factory (callable): Function to create brain instances
            world_factory (callable): Function to create world instances
        """
        self.brain_factory = brain_factory
        self.world_factory = world_factory
    
    def load_config(self, filepath: str):
        """Load experiment configuration from file.
        
        Args:
            filepath (str): Path to configuration file
        """
        if filepath.endswith('.json'):
            with open(filepath, 'r') as f:
                config_data = json.load(f)
            
            # Convert to ExperimentConfig
            self.config = ExperimentConfig(
                name=config_data.get('name', self.config.name),
                description=config_data.get('description', self.config.description),
                experiment_type=ExperimentType(config_data.get('experiment_type', 'basic')),
                config_file=config_data.get('config_file'),
                results_file=config_data.get('results_file'),
                parameters=config_data.get('parameters', {}),
                created_at=config_data.get('created_at', self.config.created_at),
                last_modified=datetime.now().isoformat()
            )
        
        elif filepath.endswith('.yaml') or filepath.endswith('.yml'):
            with open(filepath, 'r') as f:
                config_data = yaml.safe_load(f)
            
            # Convert to ExperimentConfig
            self.config = ExperimentConfig(
                name=config_data.get('name', self.config.name),
                description=config_data.get('description', self.config.description),
                experiment_type=ExperimentType(config_data.get('experiment_type', 'basic')),
                config_file=config_data.get('config_file'),
                results_file=config_data.get('results_file'),
                parameters=config_data.get('parameters', {}),
                created_at=config_data.get('created_at', self.config.created_at),
                last_modified=datetime.now().isoformat()
            )
        
        self.config.last_modified = datetime.now().isoformat()
        print(f"Experiment configuration loaded from: {filepath}")
    
    def save_config(self, filepath: str):
        """Save experiment configuration to file.
        
        Args:
            filepath (str): Path to save configuration
        """
        config_data = {
            'name': self.config.name,
            'description': self.config.description,
            'experiment_type': self.config.experiment_type.value,
            'config_file': self.config.config_file,
            'results_file': self.config.results_file,
            'parameters': self.config.parameters,
            'created_at': self.config.created_at,
            'last_modified': self.config.last_modified
        }
        
        # Create directory if it doesn't exist
        os.makedirs(os.path.dirname(filepath) if os.path.dirname(filepath) else '.', exist_ok=True)
        
        # Save as JSON
        with open(filepath, 'w') as f:
            json.dump(config_data, f, indent=2, default=str)
        
        print(f"Experiment configuration saved to: {filepath}")
    
    def run(self) -> ExperimentResult:
        """Run the experiment.
        
        Returns:
            ExperimentResult: Experiment results
        """
        start_time = datetime.now()
        self.status = ExperimentStatus.RUNNING
        
        print(f"Starting experiment: {self.config.name}")
        print(f"Experiment type: {self.config.experiment_type.value}")
        print(f"Parameters: {self.config.parameters}")
        
        try:
            if self.config.experiment_type == ExperimentType.BASIC:
                result = self._run_basic_experiment()
            elif self.config.experiment_type == ExperimentType.PARAMETER_SWEEP:
                result = self._run_parameter_sweep_experiment()
            elif self.config.experiment_type == ExperimentType.MULTI_ENVIRONMENT:
                result = self._run_multi_environment_experiment()
            elif self.config.experiment_type == ExperimentType.AGENT_RESEARCH:
                result = self._run_agent_research_experiment()
            elif self.config.experiment_type == ExperimentType.BENCHMARK:
                result = self._run_benchmark_experiment()
            elif self.config.experiment_type == ExperimentType.COMPARISON:
                result = self._run_comparison_experiment()
            else:
                raise ValueError(f"Unsupported experiment type: {self.config.experiment_type}")
            
            end_time = datetime.now()
            duration = (end_time - start_time).total_seconds()
            
            self.results = ExperimentResult(
                experiment_name=self.config.name,
                status=ExperimentStatus.COMPLETED,
                start_time=start_time.isoformat(),
                end_time=end_time.isoformat(),
                duration=duration,
                results=result,
                metadata={
                    'parameters': self.config.parameters,
                    'experiment_type': self.config.experiment_type.value
                }
            )
            
            print(f"Experiment completed: {self.config.name}")
            print(f"Duration: {duration:.2f} seconds")
            print(f"Results: {len(str(result))} characters")
            
            return self.results
            
        except Exception as e:
            end_time = datetime.now()
            duration = (end_time - start_time).total_seconds()
            
            self.results = ExperimentResult(
                experiment_name=self.config.name,
                status=ExperimentStatus.FAILED,
                start_time=start_time.isoformat(),
                end_time=end_time.isoformat(),
                duration=duration,
                results={},
                metadata={
                    'parameters': self.config.parameters,
                    'experiment_type': self.config.experiment_type.value
                },
                error_message=str(e)
            )
            
            print(f"Experiment failed: {self.config.name}")
            print(f"Error: {e}")
            
            return self.results
    
    def _run_basic_experiment(self) -> Dict[str, Any]:
        """Run a basic experiment."""
        # Create brain and world using factories
        brain = self.brain_factory(self.config.parameters)
        world = self.world_factory(self.config.parameters)
        
        # Run simulation
        num_steps = self.config.parameters.get('num_steps', 100)
        
        results = []
        for step in range(num_steps):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process input (simplified)
            if hasattr(brain, 'processSensoryInput'):
                brain.processSensoryInput(percept)
            
            # Get action
            action = Action(ActionType.Wait)  # Default action
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Update world
            world.update(0.01)
            
            # Record step results
            step_result = {
                'step': step,
                'percept_type': str(type(percept)),
                'action_type': str(action.getType()),
                'brain_stats': {
                    'total_neurons': brain.getTotalNeuronCount(),
                    'total_synapses': brain.getTotalSynapseCount(),
                    'active_neurons': brain.getActiveNeuronCount(),
                    'firing_neurons': brain.getFiringNeuronCount(),
                    'average_firing_rate': brain.getAverageFiringRate(),
                    'total_spikes': brain.getTotalSpikeCount()
                }
            }
            
            results.append(step_result)
        
        return {
            'steps': results,
            'summary': {
                'total_steps': len(results),
                'final_brain_stats': results[-1]['brain_stats'] if results else None
            }
        }
    
    def _run_parameter_sweep_experiment(self) -> Dict[str, Any]:
        """Run a parameter sweep experiment."""
        param_grid = self.config.parameters.get('parameter_grid', {})
        
        if not param_grid:
            raise ValueError("Parameter sweep experiment requires 'parameter_grid' in parameters")
        
        results = []
        
        # Generate parameter combinations
        param_combinations = self._generate_parameter_combinations(param_grid)
        
        for i, params in enumerate(param_combinations):
            print(f"Running parameter set {i+1}/{len(param_combinations)}: {params}")
            
            try:
                # Create brain with specific parameters
                brain = self.brain_factory(params)
                world = self.world_factory(params)
                
                # Run simulation
                num_steps = params.get('num_steps', 100)
                
                experiment_results = []
                for step in range(num_steps):
                    # Get sensory input
                    percept = world.getSensoryPercept()
                    
                    # Process input
                    if hasattr(brain, 'processSensoryInput'):
                        brain.processSensoryInput(percept)
                    
                    # Get action
                    action = Action(ActionType.Wait)
                    
                    # Apply action
                    world.applyMotorCommand(action, world.getSimulationTime())
                    
                    # Update world
                    world.update(0.01)
                    
                    # Record step results
                    step_result = {
                        'step': step,
                        'brain_stats': {
                            'total_neurons': brain.getTotalNeuronCount(),
                            'total_synapses': brain.getTotalSynapseCount(),
                            'active_neurons': brain.getActiveNeuronCount(),
                            'firing_neurons': brain.getFiringNeuronCount(),
                            'average_firing_rate': brain.getAverageFiringRate(),
                            'total_spikes': brain.getTotalSpikeCount()
                        }
                    }
                    
                    experiment_results.append(step_result)
                
                # Calculate parameter set results
                final_stats = experiment_results[-1]['brain_stats'] if experiment_results else None
                
                results.append({
                    'parameters': params,
                    'results': experiment_results,
                    'final_stats': final_stats,
                    'success': True
                })
                
            except Exception as e:
                print(f"Parameter set failed: {params}, error: {e}")
                results.append({
                    'parameters': params,
                    'results': None,
                    'final_stats': None,
                    'success': False,
                    'error': str(e)
                })
        
        return {
            'parameter_sets': results,
            'summary': {
                'total_combinations': len(param_combinations),
                'successful_combinations': len([r for r in results if r['success']]),
                'failed_combinations': len([r for r in results if not r['success']]),
                'success_rate': len([r for r in results if r['success']]) / len(results) if results else 0
            }
        }
    
    def _generate_parameter_combinations(self, param_grid: Dict[str, List]) -> List[Dict]:
        """Generate all combinations of parameter values."""
        from itertools import product
        
        keys = list(param_grid.keys())
        values = list(param_grid.values())
        
        combinations = []
        for combination in product(*values):
            combinations.append(dict(zip(keys, combination)))
        
        return combinations
    
    def _run_multi_environment_experiment(self) -> Dict[str, Any]:
        """Run multi-environment experiment."""
        environments = self.config.parameters.get('environments', [])
        
        if not environments:
            raise ValueError("Multi-environment experiment requires 'environments' in parameters")
        
        results = []
        
        for i, env_config in enumerate(environments):
            print(f"Running environment {i+1}/{len(environments)}: {env_config.get('name', f'env_{i}')}")
            
            try:
                # Create brain with environment parameters
                brain = self.brain_factory(env_config)
                world = self.world_factory(env_config)
                
                # Configure world for this environment
                if 'width' in env_config:
                    world.configure(
                        env_config.get('width', 100),
                        env_config.get('height', 100),
                        env_config.get('vision_width', 10),
                        env_config.get('vision_height', 10)
                    )
                
                if 'start_x' in env_config and 'start_y' in env_config:
                    world.setAgentStart(env_config['start_x'], env_config['start_y'])
                
                # Run simulation
                num_steps = env_config.get('steps', 100)
                
                env_results = []
                for step in range(num_steps):
                    # Get sensory input
                    percept = world.getSensoryPercept()
                    
                    # Process input
                    if hasattr(brain, 'processSensoryInput'):
                        brain.processSensoryInput(percept)
                    
                    # Get action
                    action = Action(ActionType.Wait)
                    
                    # Apply action
                    world.applyMotorCommand(action, world.getSimulationTime())
                    
                    # Update world
                    world.update(0.01)
                    
                    # Record step results
                    step_result = {
                        'step': step,
                        'brain_stats': {
                            'total_neurons': brain.getTotalNeuronCount(),
                            'total_synapses': brain.getTotalSynapseCount(),
                            'active_neurons': brain.getActiveNeuronCount(),
                            'firing_neurons': brain.getFiringNeuronCount(),
                            'average_firing_rate': brain.getAverageFiringRate(),
                            'total_spikes': brain.getTotalSpikeCount()
                        }
                    }
                    
                    env_results.append(step_result)
                
                # Calculate environment statistics
                final_stats = env_results[-1]['brain_stats'] if env_results else None
                
                results.append({
                    'environment_config': env_config,
                    'results': env_results,
                    'final_stats': final_stats,
                    'success': True
                })
                
            except Exception as e:
                print(f"Environment failed: {env_config.get('name', f'env_{i}')}, error: {e}")
                results.append({
                    'environment_config': env_config,
                    'results': None,
                    'final_stats': None,
                    'success': False,
                    'error': str(e)
                })
        
        return {
            'environments': results,
            'summary': {
                'total_environments': len(environments),
                'successful_environments': len([r for r in results if r['success']]),
                'failed_environments': len([r for r in results if not r['success']]),
                'success_rate': len([r for r in results if r['success']]) / len(results) if results else 0
            }
        }
    
    def _run_agent_research_experiment(self) -> Dict[str, Any]:
        """Run agent research experiment."""
        # This is a simplified example of agent research
        # In practice, you would implement custom agent logic here
        
        results = []
        num_episodes = self.config.parameters.get('num_episodes', 100)
        
        for episode in range(num_episodes):
            try:
                # Create brain and world
                brain = self.brain_factory(self.config.parameters)
                world = self.world_factory(self.config.parameters)
                
                # Run episode
                episode_results = []
                for step in range(self.config.parameters.get('steps_per_episode', 100)):
                    # Get sensory input
                    percept = world.getSensoryPercept()
                    
                    # Process input
                    if hasattr(brain, 'processSensoryInput'):
                        brain.processSensoryInput(percept)
                    
                    # Get action (simplified - you would implement custom policy)
                    action = self._get_custom_action(brain, percept, episode, step)
                    
                    # Apply action
                    world.applyMotorCommand(action, world.getSimulationTime())
                    
                    # Update world
                    world.update(0.01)
                    
                    # Record step results
                    step_result = {
                        'step': step,
                        'action_type': str(action.getType()),
                        'brain_stats': {
                            'total_neurons': brain.getTotalNeuronCount(),
                            'total_synapses': brain.getTotalSynapseCount(),
                            'active_neurons': brain.getActiveNeuronCount(),
                            'firing_neurons': brain.getFiringNeuronCount(),
                            'average_firing_rate': brain.getAverageFiringRate(),
                            'total_spikes': brain.getTotalSpikeCount()
                        }
                    }
                    
                    episode_results.append(step_result)
                
                # Calculate episode statistics
                final_stats = episode_results[-1]['brain_stats'] if episode_results else None
                
                results.append({
                    'episode': episode,
                    'results': episode_results,
                    'final_stats': final_stats,
                    'success': True
                })
                
                # Periodic checkpoint
                if episode % 20 == 0:
                    self._save_checkpoint(episode)
                
            except Exception as e:
                print(f"Episode {episode} failed: {e}")
                results.append({
                    'episode': episode,
                    'results': None,
                    'final_stats': None,
                    'success': False,
                    'error': str(e)
                })
        
        return {
            'episodes': results,
            'summary': {
                'total_episodes': num_episodes,
                'successful_episodes': len([r for r in results if r['success']]),
                'failed_episodes': len([r for r in results if not r['success']]),
                'success_rate': len([r for r in results if r['success']]) / len(results) if results else 0
            }
        }
    
    def _get_custom_action(self, brain: Brain, percept: SensoryPercept, episode: int, step: int) -> Action:
        """Get custom action for agent (example implementation)."""
        # This is a simplified example
        # In practice, you would implement sophisticated action selection
        
        # Example: Random action for demonstration
        import random
        action_types = list(ActionType)
        return Action(random.choice(action_types))
    
    def _run_benchmark_experiment(self) -> Dict[str, Any]:
        """Run benchmark experiment."""
        # This would run various benchmark tests
        # For now, return a placeholder
        
        return {
            'benchmarks': {
                'simulation_speed': 'placeholder',
                'memory_usage': 'placeholder',
                'cpu_efficiency': 'placeholder'
            },
            'summary': {
                'total_benchmarks': 3,
                'execution_time': 0.0,
                'status': 'completed'
            }
        }
    
    def _run_comparison_experiment(self) -> Dict[str, Any]:
        """Run comparison experiment."""
        # This would compare different configurations or algorithms
        # For now, return a placeholder
        
        return {
            'comparisons': {
                'configuration_comparison': 'placeholder',
                'algorithm_comparison': 'placeholder'
            },
            'summary': {
                'total_comparisons': 2,
                'best_configuration': 'placeholder',
                'execution_time': 0.0,
                'status': 'completed'
            }
        }
    
    def _save_checkpoint(self, episode: int):
        """Save checkpoint for experiment recovery."""
        checkpoint_file = f"checkpoint_{self.config.name}_{episode}.json"
        
        # Save current results to checkpoint
        checkpoint_data = {
            'experiment_name': self.config.name,
            'episode': episode,
            'timestamp': datetime.now().isoformat(),
            'config': asdict(self.config),
            'results': self.results.__dict__ if self.results else None
        }
        
        with open(checkpoint_file, 'w') as f:
            json.dump(checkpoint_data, f, indent=2, default=str)
    
    def save_results(self, filepath: str):
        """Save experiment results to file.
        
        Args:
            filepath (str): Path to save results file
        """
        if not self.results:
            print("No results to save")
            return
        
        results_file = {
            'experiment_config': asdict(self.config),
            'experiment_result': asdict(self.results) if self.results else None,
            'timestamp': datetime.now().isoformat()
        }
        
        # Create directory if it doesn't exist
        os.makedirs(os.path.dirname(filepath) if os.path.dirname(filepath) else '.', exist_ok=True)
        
        # Save as JSON
        with open(filepath, 'w') as f:
            json.dump(results_file, f, indent=2, default=str)
        
        print(f"Experiment results saved to: {filepath}")
    
    def load_results(self, filepath: str):
        """Load experiment results from file.
        
        Args:
            filepath (str): Path to results file
        """
        with open(filepath, 'r') as f:
            results_file = json.load(f)
        
        # Load configuration
        config_data = results_file.get('experiment_config', {})
        self.config = ExperimentConfig(**config_data)
        
        # Load results
        if results_file.get('experiment_result'):
            result_data = results_file['experiment_result']
            self.results = ExperimentResult(**result_data)
        
        print(f"Experiment results loaded from: {filepath}")
    
    def analyze_results(self) -> Dict[str, Any]:
        """Analyze experiment results and generate statistics.
        
        Returns:
            dict: Analysis results
        """
        if not self.results:
            return {'error': 'No results to analyze'}
        
        analysis = {
            'experiment_name': self.config.name,
            'analysis_timestamp': datetime.now().isoformat(),
            'status': self.results.status.value,
            'summary': self._generate_summary(),
            'detailed_analysis': self._generate_detailed_analysis()
        }
        
        return analysis
    
    def _generate_summary(self) -> Dict[str, Any]:
        """Generate experiment summary."""
        if not self.results:
            return {'error': 'No results available'}
        
        summary = {
            'experiment_name': self.config.name,
            'status': self.results.status.value,
            'duration_seconds': self.results.duration,
            'num_steps_total': 0,
            'brain_stats_final': None
        }
        
        # Calculate summary based on experiment type
        if self.config.experiment_type == ExperimentType.BASIC:
            if self.results.results and 'steps' in self.results.results:
                summary['num_steps_total'] = len(self.results.results['steps'])
                if self.results.results['steps']:
                    summary['brain_stats_final'] = self.results.results['steps'][-1]['brain_stats']
        
        return summary
    
    def _generate_detailed_analysis(self) -> Dict[str, Any]:
        """Generate detailed analysis."""
        if not self.results:
            return {'error': 'No results available for detailed analysis'}
        
        analysis = {
            'experiment_type': self.config.experiment_type.value,
            'parameters_used': self.config.parameters,
            'performance_metrics': {},
            'quality_metrics': {}
        }
        
        # Generate analysis based on experiment type
        if self.config.experiment_type == ExperimentType.BASIC:
            if self.results.results and 'steps' in self.results.results:
                steps = self.results.results['steps']
                
                # Analyze performance
                if steps:
                    analysis['performance_metrics'] = {
                        'total_steps': len(steps),
                        'final_neuron_count': steps[-1]['brain_stats']['total_neurons'],
                        'final_spike_count': steps[-1]['brain_stats']['total_spikes'],
                        'final_firing_rate': steps[-1]['brain_stats']['average_firing_rate'],
                        'final_efficiency': steps[-1]['brain_stats']['average_firing_rate'] / max(steps[-1]['brain_stats']['total_neurons'], 1)
                    }
        
        return analysis
    
    def __repr__(self):
        return f"Experiment(name='{self.config.name}', type='{self.config.experiment_type.value}', status='{self.status.value}')"
class ExperimentSuite:
    """Suite of experiments for batch processing.
    
    This class provides functionality to run multiple experiments together,
    manage experiment execution order, and aggregate results.
    
    Attributes:
        name (str): Name of the experiment suite
        experiments (list): List of experiments in the suite
        status (ExperimentStatus): Current suite status
    """
    
    def __init__(self, name: str):
        """Initialize experiment suite.
        
        Args:
            name (str): Name of the experiment suite
        """
        self.name = name
        self.experiments: List[Experiment] = []
        self.status = ExperimentStatus.PENDING
        self.suite_results: Dict[str, ExperimentResult] = {}
    
    def add_experiment(self, experiment: Experiment):
        """Add experiment to suite.
        
        Args:
            experiment (Experiment): Experiment to add
        """
        self.experiments.append(experiment)
        print(f"Added experiment '{experiment.config.name}' to suite '{self.name}'")
    
    def remove_experiment(self, experiment_name: str):
        """Remove experiment from suite.
        
        Args:
            experiment_name (str): Name of experiment to remove
        """>
        self.experiments = [e for e in self.experiments if e.config.name != experiment_name]
        print(f"Removed experiment '{experiment_name}' from suite '{self.name}'")
    
    def run_all(self) -> Dict[str, ExperimentResult]:
        """Run all experiments in the suite.
        
        Returns:
            dict: Results from all experiments
        """
        self.status = ExperimentStatus.RUNNING
        
        print(f"Running experiment suite: {self.name}")
        print(f"Total experiments: {len(self.experiments)}")
        
        suite_results = {}
        
        for i, experiment in enumerate(self.experiments):
            print(f"\nRunning experiment {i+1}/{len(self.experiments)}: {experiment.config.name}")
            
            try:
                # Run experiment
                result = experiment.run()
                suite_results[experiment.config.name] = result
                
                print(f"Experiment '{experiment.config.name}' completed with status: {result.status.value}")
                
            except Exception as e:
                print(f"Experiment '{experiment.config.name}' failed: {e}")
                
                # Create failed result
                failed_result = ExperimentResult(
                    experiment_name=experiment.config.name,
                    status=ExperimentStatus.FAILED,
                    start_time=datetime.now().isoformat(),
                    end_time=datetime.now().isoformat(),
                    duration=0.0,
                    results={},
                    error_message=str(e)
                )
                
                suite_results[experiment.config.name] = failed_result
        
        self.status = ExperimentStatus.COMPLETED
        self.suite_results = suite_results
        
        print(f"\nExperiment suite completed: {self.name}")
        
        return suite_results
    
    def run_selected(self, experiment_names: List[str]) -> Dict[str, ExperimentResult]:
        """Run selected experiments.
        
        Args:
            experiment_names (list): List of experiment names to run
            
        Returns:
            dict: Results from selected experiments
        """>
        self.status = ExperimentStatus.RUNNING
        
        print(f"Running selected experiments in suite: {self.name}")
        print(f"Selected experiments: {experiment_names}")
        
        suite_results = {}
        
        for experiment_name in experiment_names:
            # Find experiment
            experiment = next((e for e in self.experiments if e.config.name == experiment_name), None)
            
            if not experiment:
                print(f"Experiment '{experiment_name}' not found in suite")
                continue
            
            print(f"\nRunning experiment: {experiment_name}")
            
            try:
                # Run experiment
                result = experiment.run()
                suite_results[experiment_name] = result
                
                print(f"Experiment '{experiment_name}' completed with status: {result.status.value}")
                
            except Exception as e:
                print(f"Experiment '{experiment_name}' failed: {e}")
                
                # Create failed result
                failed_result = ExperimentResult(
                    experiment_name=experiment_name,
                    status=ExperimentStatus.FAILED,
                    start_time=datetime.now().isoformat(),
                    end_time=datetime.now().isoformat(),
                    duration=0.0,
                    results={},
                    error_message=str(e)
                )
                
                suite_results[experiment_name] = failed_result
        
        self.status = ExperimentStatus.COMPLETED
        self.suite_results = suite_results
        
        print(f"\nSelected experiments completed: {self.name}")
        
        return suite_results
    
    def get_suite_summary(self) -> Dict[str, Any]:
        """Get suite summary.
        
        Returns:
            dict: Suite summary
        """
        if not self.experiments:
            return {'error': 'No experiments in suite'}
        
        total_experiments = len(self.experiments)
        successful_experiments = len([r for r in self.suite_results.values() if r.status == ExperimentStatus.COMPLETED])
        failed_experiments = len([r for r in self.suite_results.values() if r.status == ExperimentStatus.FAILED])
        
        summary = {
            'suite_name': self.name,
            'total_experiments': total_experiments,
            'successful_experiments': successful_experiments,
            'failed_experiments': failed_experiments,
            'success_rate': successful_experiments / total_experiments if total_experiments > 0 else 0,
            'status': self.status.value
        }
        
        return summary
    
    def save_suite_results(self, filepath: str):
        """Save suite results to file.
        
        Args:
            filepath (str): Path to save results file
        """>
        if not self.suite_results:
            print("No suite results to save")
            return
        
        suite_file = {
            'suite_name': self.name,
            'timestamp': datetime.now().isoformat(),
            'experiments': []
        }
        
        for experiment_name, result in self.suite_results.items():
            suite_file['experiments'].append({
                'experiment_name': experiment_name,
                'result': result
            })
        
        # Create directory if it doesn't exist
        os.makedirs(os.path.dirname(filepath) if os.path.dirname(filepath) else '.', exist_ok=True)
        
        # Save as JSON
        with open(filepath, 'w') as f:
            json.dump(suite_file, f, indent=2, default=str)
        
        print(f"Suite results saved to: {filepath}")
    
    def load_suite_results(self, filepath: str):
        """Load suite results from file.
        
        Args:
            filepath (str): Path to results file
        """
        with open(filepath, 'r') as f:
            suite_file = json.load(f)
        
        self.name = suite_file.get('suite_name', self.name)
        self.suite_results = suite_file.get('experiments', {})
        
        print(f"Suite results loaded from: {filepath}")
    
    def __repr__(self):
        return f"ExperimentSuite(name='{self.name}', experiments={len(self.experiments)}, status='{self.status.value}')"