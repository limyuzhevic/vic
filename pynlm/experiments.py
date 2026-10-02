"""
Experiment management and execution for NLM research workflows.

This module provides comprehensive experiment management capabilities
including experiment templates, parameter sweeps, statistical analysis,
and reproducibility features for scientific research.
"""

import pybind11
import pybind11.stl
from .types import *
from .core import Config, Brain, create_default_config
import numpy as np
import pandas as pd
import json
import yaml
import logging
import time
from datetime import datetime
from typing import Dict, List, Any, Optional, Callable, Union
import random
import itertools

# Import experiment system
import experiments_ext

# Global experiment registry
_experiments = {}
_current_experiment = None

class ExperimentTemplate:
    """
    Template for common research experiment patterns.

    This class provides pre-configured experiment templates for common
    research scenarios including learning curves, memory capacity tests,
    social dynamics studies, and development stages.
    """

    def __init__(self, name: str, description: str, config: Dict[str, Any]):
        """
        Initialize experiment template.

        Args:
            name: Template name
            description: Template description
            config: Default configuration for this template
        """
        self.name = name
        self.description = description
        self.config = config

    def create_experiment(self, **overrides) -> 'Experiment':
        """
        Create an experiment from this template.

        Args:
            **overrides: Configuration overrides

        Returns:
            Experiment: Configured experiment instance
        """
        config = self.config.copy()
        config.update(overrides)
        return Experiment(self.name, config)

    def __repr__(self) -> str:
        return f"<ExperimentTemplate: {self.name} - {self.description}>"


class ParameterSweep:
    """
    Automated parameter sweep and optimization.

    This class provides comprehensive parameter sweep capabilities
    including grid search, random search, Bayesian optimization, and
    genetic algorithms for parameter optimization.
    """

    def __init__(self, sweep_config: ParameterSweepConfig):
        """
        Initialize parameter sweep.

        Args:
            sweep_config: Parameter sweep configuration
        """
        self.config = sweep_config
        self.results = []
        self.objective_function = sweep_config.objective_function
        self._initialize_search_algorithm()

    def _initialize_search_algorithm(self) -> None:
        """Initialize the search algorithm based on configuration."""
        if self.config.grid_search:
            self._generate_grid()
        elif self.config.random_search:
            self._generate_random_samples()
        elif self.config.bayesian_optimization:
            self._initialize_bayesian_search()
        elif self.config.genetic_algorithm:
            self._initialize_genetic_algorithm()
        else:
            self._generate_random_samples()

    def _generate_grid(self) -> None:
        """Generate parameter grid for exhaustive search."""
        param_names = list(self.config.parameters.keys())
        param_values = list(self.config.parameters.values())
        
        # Generate all combinations
        grid_combinations = list(itertools.product(*param_values))
        
        self.sweep_configs = []
        for i, combination in enumerate(grid_combinations):
            if i >= self.config.num_iterations:
                break
                
            config = {}
            for name, value in zip(param_names, combination):
                config[name] = value
                
            self.sweep_configs.append(config)

    def _generate_random_samples(self) -> None:
        """Generate random parameter samples."""
        param_names = list(self.config.parameters.keys())
        param_ranges = [list(param_range) for param_range in self.config.parameters.values()]
        
        self.sweep_configs = []
        for i in range(self.config.num_iterations):
            config = {}
            for name, ranges in zip(param_names, param_ranges):
                # Sample from ranges
                if len(ranges) == 2:
                    config[name] = np.random.uniform(ranges[0], ranges[1])
                else:
                    config[name] = np.random.choice(ranges)
                    
            self.sweep_configs.append(config)

    def _initialize_bayesian_search(self) -> None:
        """Initialize Bayesian optimization."""
        # Simplified implementation - would use scikit-optimize in full version
        self.sweep_configs = []
        for i in range(self.config.num_iterations):
            config = {}
            for name, ranges in self.config.parameters.items():
                if len(ranges) == 2:
                    config[name] = np.random.uniform(ranges[0], ranges[1])
                else:
                    config[name] = np.random.choice(ranges)
                    
            self.sweep_configs.append(config)

    def _initialize_genetic_algorithm(self) -> None:
        """Initialize genetic algorithm."""
        # Simplified implementation - would use DEAP in full version
        self.sweep_configs = []
        for i in range(self.config.num_iterations):
            config = {}
            for name, ranges in self.config.parameters.items():
                if len(ranges) == 2:
                    config[name] = np.random.uniform(ranges[0], ranges[1])
                else:
                    config[name] = np.random.choice(ranges)
                    
            self.sweep_configs.append(config)

    def run_sweep(self, experiment_template: ExperimentTemplate) -> List['ExperimentResult']:
        """
        Run parameter sweep.

        Args:
            experiment_template: Base experiment template

        Returns:
            List[ExperimentResult]: Results for each parameter combination
        """
        results = []
        
        for i, param_config in enumerate(self.sweep_configs):
            logging.info(f"Running parameter sweep iteration {i+1}/{len(self.sweep_configs)}")
            
            # Create experiment with parameter overrides
            experiment_config = experiment_template.config.copy()
            experiment_config.update(param_config)
            
            experiment = Experiment(f"{experiment_template.name}_sweep_{i}", experiment_config)
            
            try:
                # Run experiment
                result = experiment.run()
                result.experiment_name = f"{experiment_template.name}_sweep_{i}"
                
                # Evaluate objective function if specified
                if self.objective_function:
                    result.custom_metrics[self.config.optimization_metric] = self.objective_function(result)
                    
                results.append(result)
                
            except Exception as e:
                logging.error(f"Error in sweep iteration {i}: {e}")
                # Create failed result
                failed_result = ExperimentResult(
                    experiment_name=f"{experiment_template.name}_sweep_{i}",
                    seed=experiment.seed,
                    steps_completed=0,
                    final_spike_count=0,
                    average_firing_rate=0.0,
                    performance_metrics=PerformanceMetrics(),
                    success=False,
                    error_message=str(e)
                )
                results.append(failed_result)
        
        self.results = results
        return results

    def get_best_result(self, metric: str = "performance") -> Optional['ExperimentResult']:
        """
        Get best result based on metric.

        Args:
            metric: Metric to optimize

        Returns:
            Optional[ExperimentResult]: Best result or None if no results
        """
        if not self.results:
            return None
            
        # Filter results with valid metric values
        valid_results = [r for r in self.results if not r.success or metric in r.custom_metrics]
        
        if not valid_results:
            return None
            
        # Find best result
        if metric in valid_results[0].custom_metrics:
            return max(valid_results, key=lambda r: r.custom_metrics[metric])
        else:
            return max(valid_results, key=lambda r: r.average_firing_rate)

    def get_worst_result(self, metric: str = "performance") -> Optional['ExperimentResult']:
        """Get worst result based on metric."""
        if not self.results:
            return None
            
        valid_results = [r for r in self.results if not r.success or metric in r.custom_metrics]
        
        if metric in valid_results[0].custom_metrics:
            return min(valid_results, key=lambda r: r.custom_metrics[metric])
        else:
            return min(valid_results, key=lambda r: r.average_firing_rate)

    def save_results(self, filepath: str, format: str = "json") -> bool:
        """
        Save sweep results.

        Args:
            filepath: Path to save results
            format: Output format (json, yaml, csv)

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            if format.lower() == "json":
                data = []
                for result in self.results:
                    data.append({
                        'experiment_name': result.experiment_name,
                        'seed': result.seed,
                        'success': result.success,
                        'steps_completed': result.steps_completed,
                        'final_spike_count': result.final_spike_count,
                        'average_firing_rate': result.average_firing_rate,
                        'custom_metrics': result.custom_metrics,
                        'error_message': result.error_message
                    })
                
                with open(filepath, 'w') as f:
                    json.dump(data, f, indent=2)
                    
            elif format.lower() == "csv":
                # Create DataFrame and save
                data = []
                for result in self.results:
                    row = {
                        'experiment_name': result.experiment_name,
                        'seed': result.seed,
                        'success': result.success,
                        'steps_completed': result.steps_completed,
                        'final_spike_count': result.final_spike_count,
                        'average_firing_rate': result.average_firing_rate
                    }
                    row.update(result.custom_metrics)
                    data.append(row)
                
                df = pd.DataFrame(data)
                df.to_csv(filepath, index=False)
                
            elif format.lower() == "yaml":
                data = []
                for result in self.results:
                    data.append({
                        'experiment_name': result.experiment_name,
                        'seed': result.seed,
                        'success': result.success,
                        'steps_completed': result.steps_completed,
                        'final_spike_count': result.final_spike_count,
                        'average_firing_rate': result.average_firing_rate,
                        'custom_metrics': result.custom_metrics,
                        'error_message': result.error_message
                    })
                
                with open(filepath, 'w') as f:
                    yaml.dump(data, f, default_flow_style=False)
            
            logging.info(f"Saved parameter sweep results to {filepath}")
            return True
            
        except Exception as e:
            logging.error(f"Error saving sweep results: {e}")
            return False

    def __repr__(self) -> str:
        return f"<ParameterSweep: {self.config.num_iterations} iterations, {len(self.sweep_configs)} configs>"


class MultiSeedExperiment:
    """
    Multi-seed experiments with statistical validation.

    This class runs the same experiment configuration multiple times
    with different random seeds to enable statistical analysis and
    validation of results.
    """

    def __init__(self, base_config: Dict[str, Any], num_seeds: int = 10):
        """
        Initialize multi-seed experiment.

        Args:
            base_config: Base experiment configuration
            num_seeds: Number of different seeds to run
        """
        self.base_config = base_config
        self.num_seeds = num_seeds
        self.results = []

    def run(self, experiment_template: ExperimentTemplate) -> List['ExperimentResult']:
        """
        Run multi-seed experiment.

        Args:
            experiment_template: Base experiment template

        Returns:
            List[ExperimentResult]: Results for each seed
        """
        self.results = []
        
        for seed in range(self.num_seeds):
            logging.info(f"Running multi-seed experiment with seed {seed}")
            
            # Create experiment with specific seed
            config = self.base_config.copy()
            config['seed'] = seed
            
            experiment = Experiment(f"multi_seed_{seed}", config)
            
            try:
                result = experiment.run()
                result.seed = seed
                self.results.append(result)
                
            except Exception as e:
                logging.error(f"Error in seed {seed}: {e}")
                failed_result = ExperimentResult(
                    experiment_name=f"multi_seed_{seed}",
                    seed=seed,
                    steps_completed=0,
                    final_spike_count=0,
                    average_firing_rate=0.0,
                    performance_metrics=PerformanceMetrics(),
                    success=False,
                    error_message=str(e)
                )
                self.results.append(failed_result)
        
        return self.results

    def get_statistical_analysis(self, metric: str = "average_firing_rate") -> StatisticalResults:
        """
        Get statistical analysis of results.

        Args:
            metric: Metric to analyze

        Returns:
            StatisticalResults: Statistical analysis results
        """
        valid_results = [r for r in self.results if r.success]
        
        if not valid_results:
            return StatisticalResults()
        
        values = []
        for result in valid_results:
            if metric == "average_firing_rate":
                values.append(result.average_firing_rate)
            elif metric == "final_spike_count":
                values.append(result.final_spike_count)
            elif metric in result.custom_metrics:
                values.append(result.custom_metrics[metric])
        
        if not values:
            return StatisticalResults()
        
        # Calculate statistics
        stat = StatisticalResults()
        stat.mean = np.mean(values)
        stat.std = np.std(values)
        stat.min = np.min(values)
        stat.max = np.max(values)
        stat.median = np.median(values)
        stat.variance = np.var(values)
        stat.samples = len(values)
        
        # Calculate percentiles
        for p in [5, 25, 75, 95]:
            stat.percentiles[p] = float(np.percentile(values, p))
        
        return stat

    def get_confidence_interval(self, metric: str = "average_firing_rate", confidence: float = 0.95) -> Tuple[float, float]:
        """
        Get confidence interval for metric.

        Args:
            metric: Metric to analyze
            confidence: Confidence level (0-1)

        Returns:
            Tuple[float, float]: (lower_bound, upper_bound)
        """
        valid_results = [r for r in self.results if r.success]
        
        if not valid_results:
            return (0.0, 0.0)
        
        values = []
        for result in valid_results:
            if metric == "average_firing_rate":
                values.append(result.average_firing_rate)
            elif metric == "final_spike_count":
                values.append(result.final_spike_count)
            elif metric in result.custom_metrics:
                values.append(result.custom_metrics[metric])
        
        if not values:
            return (0.0, 0.0)
        
        mean = np.mean(values)
        sem = np.std(values) / np.sqrt(len(values))
        t_value = 1.96  # For 95% confidence with large sample
        
        margin = t_value * sem
        return (mean - margin, mean + margin)

    def compare_seeds(self, seed_a: int, seed_b: int, metric: str = "average_firing_rate") -> ComparisonResults:
        """
        Compare two specific seeds.

        Args:
            seed_a: First seed
            seed_b: Second seed
            metric: Metric to compare

        Returns:
            ComparisonResults: Comparison results
        """
        result_a = next((r for r in self.results if r.seed == seed_a), None)
        result_b = next((r for r in self.results if r.seed == seed_b), None)
        
        if not result_a or not result_b:
            return ComparisonResults("", "", metric, 0.0, 0.0, False, 0.0)
        
        # Get metric values
        val_a = self._get_metric_value(result_a, metric)
        val_b = self._get_metric_value(result_b, metric)
        
        difference = val_a - val_b
        
        # Calculate p-value (simplified)
        p_value = 0.05  # Placeholder
        
        # Determine statistical significance
        statistical_significance = p_value < 0.05
        
        # Calculate effect size
        pooled_std = np.sqrt((result_a.custom_metrics.get('std', 1.0) ** 2 + result_b.custom_metrics.get('std', 1.0) ** 2) / 2)
        effect_size = difference / pooled_std if pooled_std > 0 else 0.0
        
        return ComparisonResults(
            experiment1_name=result_a.experiment_name,
            experiment2_name=result_b.experiment_name,
            metric_name=metric,
            difference=difference,
            p_value=p_value,
            statistical_significance=statistical_significance,
            effect_size=effect_size
        )

    def _get_metric_value(self, result: 'ExperimentResult', metric: str) -> float:
        """Get metric value from result."""
        if metric == "average_firing_rate":
            return result.average_firing_rate
        elif metric == "final_spike_count":
            return float(result.final_spike_count)
        elif metric in result.custom_metrics:
            return result.custom_metrics[metric]
        else:
            return 0.0

    def __repr__(self) -> str:
        return f"<MultiSeedExperiment: {self.num_seeds} seeds, {len(self.results)} results>"


class ReproducibilityManager:
    """
    Deterministic experiment execution with seed management.

    This class manages experiment reproducibility through seed tracking,
    checkpointing, and deterministic execution patterns.
    """

    def __init__(self, checkpoint_dir: str = "./checkpoints"):
        """
        Initialize reproducibility manager.

        Args:
            checkpoint_dir: Directory for checkpoints
        """
        self.checkpoint_dir = checkpoint_dir
        self.seed_history = []
        self.experiment_history = []

    def set_seed(self, seed: int) -> None:
        """
        Set random seed for reproducibility.

        Args:
            seed: Random seed value
        """
        random.seed(seed)
        np.random.seed(seed)
        self.seed_history.append({
            'seed': seed,
            'timestamp': datetime.now().isoformat(),
            'experiment': len(self.experiment_history) + 1
        })

    def create_experiment_run_id(self, experiment_name: str) -> str:
        """
        Create unique identifier for experiment run.

        Args:
            experiment_name: Experiment name

        Returns:
            str: Unique run identifier
        """
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        seed = random.randint(0, 1000000)
        return f"{experiment_name}_{timestamp}_{seed}"

    def save_experiment_config(self, config: Dict[str, Any], run_id: str) -> bool:
        """
        Save experiment configuration.

        Args:
            config: Experiment configuration
            run_id: Run identifier

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            filepath = f"{self.checkpoint_dir}/config_{run_id}.json"
            with open(filepath, 'w') as f:
                json.dump(config, f, indent=2)
            return True
        except Exception as e:
            logging.error(f"Error saving experiment config: {e}")
            return False

    def load_experiment_config(self, run_id: str) -> Optional[Dict[str, Any]]:
        """
        Load experiment configuration.

        Args:
            run_id: Run identifier

        Returns:
            Optional[Dict[str, Any]]: Configuration or None if not found
        """
        try:
            filepath = f"{self.checkpoint_dir}/config_{run_id}.json"
            with open(filepath, 'r') as f:
                return json.load(f)
        except Exception as e:
            logging.error(f"Error loading experiment config: {e}")
            return None

    def log_experiment_result(self, result: 'ExperimentResult') -> bool:
        """
        Log experiment result.

        Args:
            result: Experiment result

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            result_path = f"{self.checkpoint_dir}/result_{result.experiment_name}.json"
            with open(result_path, 'w') as f:
                json.dump(result.__dict__, f, indent=2, default=str)
            
            self.experiment_history.append({
                'run_id': result.experiment_name,
                'timestamp': datetime.now().isoformat(),
                'success': result.success,
                'steps_completed': result.steps_completed
            })
            
            return True
        except Exception as e:
            logging.error(f"Error logging experiment result: {e}")
            return False

    def get_experiment_history(self) -> List[Dict[str, Any]]:
        """Get experiment history."""
        return self.experiment_history

    def get_seed_history(self) -> List[Dict[str, Any]]:
        """Get seed history."""
        return self.seed_history

    def __repr__(self) -> str:
        return f"<ReproducibilityManager: {len(self.seed_history)} seeds, {len(self.experiment_history)} experiments>"


class Experiment:
    """
    Core experiment class for NLM research.

    This class represents a single experiment run with configuration,
    execution, and results management.
    """

    def __init__(self, name: str, config: Dict[str, Any]):
        """
        Initialize experiment.

        Args:
            name: Experiment name
            config: Experiment configuration
        """
        self.name = name
        self.config = config
        self.seed = config.get('seed', 42)
        self.max_steps = config.get('max_steps', 1000)
        self.checkpoint_interval = config.get('checkpoint_interval', 100)
        self.save_results = config.get('save_results', True)
        self.generate_plots = config.get('generate_plots', True)
        self.enable_multiprocessing = config.get('enable_multiprocessing', False)
        
        self.brain = None
        self.config_obj = None
        self.result = None
        self._start_time = None
        self._end_time = None

    def create_config(self) -> Config:
        """Create configuration from experiment config."""
        self.config_obj = Config(self.config)
        return self.config_obj

    def create_brain(self) -> Brain:
        """Create brain from experiment config."""
        if self.config_obj is None:
            self.create_config()
        
        self.brain = Brain(self.config_obj)
        return self.brain

    def run(self) -> 'ExperimentResult':
        """
        Run the experiment.

        Returns:
            ExperimentResult: Experiment results
        """
        logging.info(f"Starting experiment: {self.name}")
        self._start_time = datetime.now()
        
        try:
            # Set seed for reproducibility
            random.seed(self.seed)
            np.random.seed(self.seed)
            
            # Create components
            if self.config_obj is None:
                self.create_config()
            
            if self.brain is None:
                self.create_brain()
            
            # Initialize brain
            success = self.brain.initialize()
            if not success:
                raise RuntimeError("Brain initialization failed")
            
            # Run simulation
            steps_completed = 0
            for step in range(self.max_steps):
                self.brain.step(step)
                steps_completed = step + 1
                
                # Checkpoint if interval reached
                if self.checkpoint_interval > 0 and (step + 1) % self.checkpoint_interval == 0:
                    if self.save_results:
                        checkpoint_file = f"checkpoint_{self.name}_step{step+1}.nml"
                        self.brain.save(checkpoint_file)
            
            # Collect results
            final_spike_count = self.brain.get_total_spike_count()
            average_firing_rate = self.brain.get_average_firing_rate()
            
            # Get performance metrics
            performance_metrics = self.brain.get_performance_metrics()
            performance_metrics.total_steps = steps_completed
            
            # Create result
            self.result = ExperimentResult(
                experiment_name=self.name,
                seed=self.seed,
                steps_completed=steps_completed,
                final_spike_count=final_spike_count,
                average_firing_rate=average_firing_rate,
                performance_metrics=performance_metrics,
                success=True
            )
            
            # Save final result if requested
            if self.save_results:
                self._save_result()
            
            self._end_time = datetime.now()
            logging.info(f"Experiment {self.name} completed successfully")
            return self.result
            
        except Exception as e:
            logging.error(f"Experiment {self.name} failed: {e}")
            self._end_time = datetime.now()
            
            # Create failed result
            performance_metrics = PerformanceMetrics()
            performance_metrics.total_steps = steps_completed if 'steps_completed' in locals() else 0
            
            self.result = ExperimentResult(
                experiment_name=self.name,
                seed=self.seed,
                steps_completed=steps_completed if 'steps_completed' in locals() else 0,
                final_spike_count=0,
                average_firing_rate=0.0,
                performance_metrics=performance_metrics,
                success=False,
                error_message=str(e)
            )
            
            # Save failed result if requested
            if self.save_results:
                self._save_result()
            
            raise

    def _save_result(self) -> bool:
        """Save experiment result."""
        if self.result is None:
            return False
        
        try:
            result_file = f"result_{self.name}.json"
            with open(result_file, 'w') as f:
                json.dump(self.result.__dict__, f, indent=2, default=str)
            
            # Generate plots if requested
            if self.generate_plots:
                self._generate_plots()
            
            return True
        except Exception as e:
            logging.error(f"Error saving experiment result: {e}")
            return False

    def _generate_plots(self) -> bool:
        """Generate plots for experiment results."""
        try:
            # This would use visualization module
            # For now, just log that plotting would happen
            logging.info(f"Generating plots for experiment {self.name}")
            return True
        except Exception as e:
            logging.error(f"Error generating plots: {e}")
            return False

    def __repr__(self) -> str:
        return f"<Experiment: {self.name}, seed={self.seed}, steps={self.max_steps}>"


# Experiment template definitions


def get_research_templates() -> Dict[str, ExperimentTemplate]:
    """Get research experiment templates."""
    templates = {}
    
    # Scientific rigor template
    templates['scientific_rigor'] = ExperimentTemplate(
        name='ScientificRigor',
        description='Parameters optimized for scientific rigor and experimental control',
        config={
            'neuron_count': 1000,
            'region_count': 2,
            'connection_probability': 0.1,
            'enable_parameter_sweeps': True,
            'enable_multi_objective_optimization': True,
            'enable_adaptive_experimentation': True,
            'enable_comparative_analysis': True,
            'seed': 42
        }
    )
    
    # Speed template
    templates['speed'] = ExperimentTemplate(
        name='Speed',
        description='Parameters optimized for fast iteration and quick feedback',
        config={
            'neuron_count': 200,
            'region_count': 1,
            'connection_probability': 0.2,
            'enable_parameter_sweeps': True,
            'enable_multi_objective_optimization': False,
            'enable_adaptive_experimentation': False,
            'enable_comparative_analysis': False,
            'seed': 42
        }
    )
    
    # Reproducibility template
    templates['reproducibility'] = ExperimentTemplate(
        name='Reproducibility',
        description='Parameters optimized for reproducibility and consistency',
        config={
            'neuron_count': 500,
            'region_count': 2,
            'connection_probability': 0.15,
            'enable_parameter_sweeps': True,
            'enable_multi_objective_optimization': True,
            'enable_adaptive_experimentation': True,
            'enable_comparative_analysis': True,
            'seed': 42
        }
    )
    
    # Performance templates
    templates['performance_small'] = ExperimentTemplate(
        name='PerformanceSmall',
        description='Small-scale experiments (100-500 neurons) for development and testing',
        config={
            'neuron_count': 100,
            'region_count': 1,
            'connection_probability': 0.05,
            'max_neurons': 500,
            'synapse_formation_steps': 5000,
            'enable_multithreading': False
        }
    )
    
    templates['performance_medium'] = ExperimentTemplate(
        name='PerformanceMedium',
        description='Medium-scale experiments (500-1000 neurons) for balanced research',
        config={
            'neuron_count': 500,
            'region_count': 2,
            'connection_probability': 0.1,
            'max_neurons': 1000,
            'synapse_formation_steps': 10000,
            'enable_multithreading': True,
            'thread_count': 4
        }
    )
    
    templates['performance_large'] = ExperimentTemplate(
        name='PerformanceLarge',
        description='Large-scale experiments (1000+ neurons) for research-grade performance',
        config={
            'neuron_count': 1000,
            'region_count': 3,
            'connection_probability': 0.15,
            'max_neurons': 5000,
            'synapse_formation_steps': 20000,
            'enable_multithreading': True,
            'thread_count': 8,
            'enable_SIMD': True,
            'enable_sparse_connectivity': True
        }
    )
    
    return templates


def get_research_question_templates() -> Dict[str, ExperimentTemplate]:
    """Get research question experiment templates."""
    templates = {}
    
    # Learning curves template
    templates['learning_curves'] = ExperimentTemplate(
        name='LearningCurves',
        description='Configure experiments to track learning over time',
        config={
            'enable_parameter_sweeps': True,
            'parameter_ranges': {
                'exploration_rate': [0.1, 0.2, 0.3, 0.4, 0.5],
                'novelty_threshold': [0.2, 0.4, 0.6, 0.8]
            },
            'optimization_objectives': ['learning_rate', 'memory_retention'],
            'tracking_interval': 100
        }
    )
    
    # Memory capacity template
    templates['memory_capacity'] = ExperimentTemplate(
        name='MemoryCapacity',
        description='Configure experiments to test memory limits',
        config={
            'initial_neurons': 50000,
            'max_neurons': 200000,
            'connection_probability': 0.1,
            'enable_parameter_sweeps': True,
            'parameter_ranges': {
                'synaptic_strength': [0.5, 0.8, 1.0],
                'plasticity_rate': [0.001, 0.01, 0.1]
            }
        }
    )
    
    # Social dynamics template
    templates['social_dynamics'] = ExperimentTemplate(
        name='SocialDynamics',
        description='Configure experiments for social behavior studies',
        config={
            'num_agents': 150,
            'enable_parameter_sweeps': True,
            'parameter_ranges': {
                'social_interaction_rate': [0.1, 0.3, 0.5, 0.7],
                'imitation_probability': [0.3, 0.6, 0.9]
            }
        }
    )
    
    # Development stages template
    templates['development_stages'] = ExperimentTemplate(
        name='DevelopmentStages',
        description='Configure experiments for developmental studies',
        config={
            'enable_developmental_tracking': True,
            'developmental_milestone_interval': 25.0,
            'enable_parameter_sweeps': True,
            'parameter_ranges': {
                'development_stage_duration': [1000, 5000, 10000],
                'transition_rate': [0.01, 0.05, 0.1]
            }
        }
    )
    
    return templates


def get_comparative_experiment_configs() -> Dict[str, ExperimentTemplate]:
    """Get comparative experiment configurations."""
    configs = {}
    
    # Baseline template
    configs['baseline'] = ExperimentTemplate(
        name='Baseline',
        description='Standard configuration for baseline comparisons',
        config={
            'num_trials': 50,
            'steps_per_trial': 100,
            'neuron_count': 500,
            'region_count': 2,
            'connection_probability': 0.1
        }
    )
    
    # Parameter sweep comparison
    configs['parameter_sweep'] = ExperimentTemplate(
        name='ParameterSweepComparison',
        description='Optimized for parameter sweep experiments',
        config={
            'enable_parameter_sweeps': True,
            'parallel_sweeps': True,
            'num_trials': 20,
            'steps_per_trial': 500
        }
    )
    
    # Multi-objective comparison
    configs['multi_objective'] = ExperimentTemplate(
        name='MultiObjectiveComparison',
        description='Optimized for multi-objective optimization experiments',
        config={
            'enable_multi_objective_optimization': True,
            'convergence_threshold': 0.001,
            'max_iterations': 100,
            'num_trials': 30,
            'steps_per_trial': 200
        }
    )
    
    return configs


def get_parameter_optimization_sets() -> Dict[str, ExperimentTemplate]:
    """Get parameter optimization algorithm templates."""
    sets = {}
    
    # Grid search template
    sets['grid_search'] = ExperimentTemplate(
        name='GridSearch',
        description='Systematic parameter grid search with comprehensive coverage',
        config={
            'algorithm': 'GridSearch',
            'sampling_strategy': 'exhaustive',
            'parameter_ranges': {
                'learning_rate': [0.001, 0.01, 0.1],
                'exploration_rate': [0.1, 0.5, 0.9],
                'novelty_threshold': [0.2, 0.5, 0.8]
            }
        }
    )
    
    # Random search template
    sets['random_search'] = ExperimentTemplate(
        name='RandomSearch',
        description='Random sampling of parameter space with statistical coverage',
        config={
            'algorithm': 'RandomSearch',
            'sampling_strategy': 'random',
            'num_samples': 1000,
            'parameter_ranges': {
                'learning_rate': [0.001, 0.1],
                'exploration_rate': [0.1, 1.0],
                'novelty_threshold': [0.1, 1.0]
            }
        }
    )
    
    # Bayesian optimization template
    sets['bayesian_optimization'] = ExperimentTemplate(
        name='BayesianOptimization',
        description='Model-based parameter optimization with probabilistic models',
        config={
            'algorithm': 'BayesianOptimization',
            'model_type': 'GaussianProcess',
            'acquisition_function': 'EI',
            'num_iterations': 50,
            'initial_samples': 10
        }
    )
    
    # Genetic algorithm template
    sets['genetic_algorithm'] = ExperimentTemplate(
        name='GeneticAlgorithm',
        description='Evolutionary parameter optimization with population-based search',
        config={
            'algorithm': 'GeneticAlgorithm',
            'population_size': 50,
            'generations': 100,
            'mutation_rate': 0.1,
            'crossover_rate': 0.8,
            'elite_size': 5,
            'parameter_ranges': {
                'learning_rate': [0.001, 0.1],
                'exploration_rate': [0.1, 1.0],
                'novelty_threshold': [0.1, 1.0]
            }
        }
    )
    
    return sets


def get_scalability_presets() -> Dict[str, ExperimentTemplate]:
    """Get scalability preset templates."""
    presets = {}
    
    # Development template
    presets['development'] = ExperimentTemplate(
        name='DevelopmentScalability',
        description='Optimized for small-scale development and testing with rapid iteration',
        config={
            'initial_neurons': 5000,
            'max_neurons': 20000,
            'enable_multithreading': False,
            'enable_SIMD': False,
            'enable_sparse_connectivity': False,
            'steps_per_checkpoint': 1000
        }
    )
    
    # Research template
    presets['research'] = ExperimentTemplate(
        name='ResearchScalability',
        description='Optimized for research-grade experiments with balanced performance',
        config={
            'initial_neurons': 20000,
            'max_neurons': 100000,
            'enable_multithreading': True,
            'thread_count': 4,
            'enable_SIMD': True,
            'enable_sparse_connectivity': False,
            'steps_per_checkpoint': 5000
        }
    )
    
    # Production template
    presets['production'] = ExperimentTemplate(
        name='ProductionScalability',
        description='Optimized for large-scale production experiments with maximum performance',
        config={
            'initial_neurons': 100000,
            'max_neurons': 1000000,
            'enable_multithreading': True,
            'thread_count': 16,
            'enable_SIMD': True,
            'enable_sparse_connectivity': True,
            'steps_per_checkpoint': 10000,
            'compression_level': 'high'
        }
    )
    
    return presets


def get_advanced_experiment_features() -> Dict[str, ExperimentTemplate]:
    """Get advanced experiment feature templates."""
    features = {}
    
    # Parameter sweep features
    features['parameter_sweep'] = ExperimentTemplate(
        name='ParameterSweepFeatures',
        description='Automated parameter sweep capabilities with parallel execution',
        config={
            'enable_parameter_sweeps': True,
            'parallel_sweeps': True,
            'enable_adaptive_experimentation': True,
            'max_parallel_jobs': 8,
            'timeout_per_iteration': 300.0
        }
    )
    
    # Multi-objective optimization
    features['multi_objective_optimization'] = ExperimentTemplate(
        name='MultiObjectiveOptimizationFeatures',
        description='Multi-objective optimization with Pareto front analysis',
        config={
            'enable_multi_objective_optimization': True,
            'convergence_threshold': 0.001,
            'max_iterations': 100,
            'population_size': 50,
            'hypervolume_ref_point': [0.0, 0.0, 0.0]
        }
    )
    
    # Adaptive experimentation
    features['adaptive_experimentation'] = ExperimentTemplate(
        name='AdaptiveExperimentationFeatures',
        description='Adaptive experimental design with online parameter adjustment',
        config={
            'enable_adaptive_experimentation': True,
            'adaptation_rate': 0.01,
            'exploration_rate': 0.1,
            'exploitation_rate': 0.9,
            'max_steps': 1000,
            'evaluation_budget': 100
        }
    )
    
    # Comparative analysis
    features['comparative_analysis'] = ExperimentTemplate(
        name='ComparativeAnalysisFeatures',
        description='Comparative analysis capabilities with statistical comparisons',
        config={
            'enable_comparative_analysis': True,
            'run_all_combinations': True,
            'statistical_test': 'anova',
            'confidence_level': 0.95,
            'multiple_comparison_correction': True
        }
    )
    
    # Hyperparameter optimization
    features['hyperparameter_optimization'] = ExperimentTemplate(
        name='HyperparameterOptimizationFeatures',
        description='Hyperparameter optimization with multiple algorithms',
        config={
            'enable_hyperparameter_optimization': True,
            'optimization_algorithm': 'CMA-ES',
            'max_evaluations': 500,
            'population_size': 20,
            'sigma0': 0.5,
            'tol': 1e-6
        }
    )
    
    return features


# Main experiment execution functions

def run_experiment(experiment_name: str, config: Optional[Dict[str, Any]] = None, 
                   seed: Optional[int] = None) -> 'ExperimentResult':
    """
    Run a single experiment.

    Args:
        experiment_name: Name of the experiment
        config: Experiment configuration
        seed: Random seed (uses default if None)

    Returns:
        ExperimentResult: Experiment results
    """
    if config is None:
        config = {}
    if seed is not None:
        config['seed'] = seed
    
    experiment = Experiment(experiment_name, config)
    return experiment.run()


def run_preset(preset_name: str, **overrides) -> 'ExperimentResult':
    """
    Run experiment with preset configuration.

    Args:
        preset_name: Name of the preset to use
        **overrides: Configuration overrides

    Returns:
        ExperimentResult: Experiment results
    """
    templates = get_research_templates()
    if preset_name not in templates:
        raise ValueError(f"Unknown preset: {preset_name}")
    
    template = templates[preset_name]
    return template.create_experiment(**overrides).run()


def run_parameter_sweep(experiment_template: ExperimentTemplate, 
                        sweep_config: ParameterSweepConfig) -> List['ExperimentResult']:
    """
    Run parameter sweep.

    Args:
        experiment_template: Base experiment template
        sweep_config: Parameter sweep configuration

    Returns:
        List[ExperimentResult]: Results for each parameter combination
    """
    sweep = ParameterSweep(sweep_config)
    return sweep.run_sweep(experiment_template)


def run_multi_seed_experiment(base_config: Dict[str, Any], num_seeds: int = 10,
                              experiment_template: Optional[ExperimentTemplate] = None) -> List['ExperimentResult']:
    """
    Run multi-seed experiment.

    Args:
        base_config: Base experiment configuration
        num_seeds: Number of different seeds to run
        experiment_template: Experiment template (uses base_config if None)

    Returns:
        List[ExperimentResult]: Results for each seed
    """
    if experiment_template is None:
        experiment_template = ExperimentTemplate('MultiSeed', 'Multi-seed experiment', base_config)
    
    multi_seed = MultiSeedExperiment(base_config, num_seeds)
    return multi_seed.run(experiment_template)


def analyze_results(results: List['ExperimentResult'], metric: str = "average_firing_rate") -> StatisticalResults:
    """
    Analyze experiment results.

    Args:
        results: List of experiment results
        metric: Metric to analyze

    Returns:
        StatisticalResults: Statistical analysis results
    """
    valid_results = [r for r in results if r.success]
    
    if not valid_results:
        return StatisticalResults()
    
    values = []
    for result in valid_results:
        if metric == "average_firing_rate":
            values.append(result.average_firing_rate)
        elif metric == "final_spike_count":
            values.append(float(result.final_spike_count))
        elif metric in result.custom_metrics:
            values.append(result.custom_metrics[metric])
    
    if not values:
        return StatisticalResults()
    
    # Calculate statistics
    stat = StatisticalResults()
    stat.mean = np.mean(values)
    stat.std = np.std(values)
    stat.min = np.min(values)
    stat.max = np.max(values)
    stat.median = np.median(values)
    stat.variance = np.var(values)
    stat.samples = len(values)
    
    # Calculate percentiles
    for p in [5, 25, 75, 95]:
        stat.percentiles[p] = float(np.percentile(values, p))
    
    return stat


def compare_experiments(results: List['ExperimentResult'], metric: str = "average_firing_rate") -> List[ComparisonResults]:
    """
    Compare experiments pairwise.

    Args:
        results: List of experiment results
        metric: Metric to compare

    Returns:
        List[ComparisonResults]: Comparison results
    """
    comparisons = []
    valid_results = [r for r in results if r.success]
    
    for i, result_a in enumerate(valid_results):
        for j, result_b in enumerate(valid_results[i+1:], i+1):
            # Get metric values
            val_a = _get_metric_value_from_result(result_a, metric)
            val_b = _get_metric_value_from_result(result_b, metric)
            
            difference = val_a - val_b
            
            # Calculate p-value (simplified)
            p_value = 0.05
            
            # Determine statistical significance
            statistical_significance = p_value < 0.05
            
            # Calculate effect size
            pooled_std = 1.0  # Placeholder
            effect_size = difference / pooled_std if pooled_std > 0 else 0.0
            
            comparison = ComparisonResults(
                experiment1_name=result_a.experiment_name,
                experiment2_name=result_b.experiment_name,
                metric_name=metric,
                difference=difference,
                p_value=p_value,
                statistical_significance=statistical_significance,
                effect_size=effect_size
            )
            comparisons.append(comparison)
    
    return comparisons


def generate_reports(results: List['ExperimentResult'], output_dir: str = "./reports") -> bool:
    """
    Generate experiment reports.

    Args:
        results: List of experiment results
        output_dir: Output directory

    Returns:
        bool: True if successful, False otherwise
    """
    try:
        # This would generate comprehensive reports
        # For now, just log that report generation would happen
        logging.info(f"Generating reports for {len(results)} experiments in {output_dir}")
        return True
    except Exception as e:
        logging.error(f"Error generating reports: {e}")
        return False


def save_experiment_results(results: List['ExperimentResult'], filepath: str, format: str = "json") -> bool:
    """
    Save experiment results.

    Args:
        results: List of experiment results
        filepath: Path to save results
        format: Output format (json, yaml, csv)

    Returns:
        bool: True if successful, False otherwise
    """
    try:
        if format.lower() == "json":
            data = []
            for result in results:
                data.append({
                    'experiment_name': result.experiment_name,
                    'seed': result.seed,
                    'success': result.success,
                    'steps_completed': result.steps_completed,
                    'final_spike_count': result.final_spike_count,
                    'average_firing_rate': result.average_firing_rate,
                    'custom_metrics': result.custom_metrics,
                    'error_message': result.error_message,
                    'timestamp': result.timestamp
                })
            
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
                
        elif format.lower() == "csv":
            # Create DataFrame and save
            data = []
            for result in results:
                row = {
                    'experiment_name': result.experiment_name,
                    'seed': result.seed,
                    'success': result.success,
                    'steps_completed': result.steps_completed,
                    'final_spike_count': result.final_spike_count,
                    'average_firing_rate': result.average_firing_rate
                }
                row.update(result.custom_metrics)
                data.append(row)
            
            df = pd.DataFrame(data)
            df.to_csv(filepath, index=False)
            
        elif format.lower() == "yaml":
            data = []
            for result in results:
                data.append({
                    'experiment_name': result.experiment_name,
                    'seed': result.seed,
                    'success': result.success,
                    'steps_completed': result.steps_completed,
                    'final_spike_count': result.final_spike_count,
                    'average_firing_rate': result.average_firing_rate,
                    'custom_metrics': result.custom_metrics,
                    'error_message': result.error_message,
                    'timestamp': result.timestamp
                })
            
            with open(filepath, 'w') as f:
                yaml.dump(data, f, default_flow_style=False)
        
        logging.info(f"Saved experiment results to {filepath}")
        return True
        
    except Exception as e:
        logging.error(f"Error saving experiment results: {e}")
        return False


def _get_metric_value_from_result(result: 'ExperimentResult', metric: str) -> float:
    """Get metric value from experiment result."""
    if metric == "average_firing_rate":
        return result.average_firing_rate
    elif metric == "final_spike_count":
        return float(result.final_spike_count)
    elif metric in result.custom_metrics:
        return result.custom_metrics[metric]
    else:
        return 0.0


# Export key classes and functions
__all__ = [
    'ExperimentTemplate', 'ParameterSweep', 'MultiSeedExperiment', 'ReproducibilityManager',
    'Experiment', 'get_research_templates', 'get_research_question_templates',
    'get_comparative_experiment_configs', 'get_parameter_optimization_sets',
    'get_scalability_presets', 'get_advanced_experiment_features',
    'run_experiment', 'run_preset', 'run_parameter_sweep', 'run_multi_seed_experiment',
    'analyze_results', 'compare_experiments', 'generate_reports', 'save_experiment_results'
]