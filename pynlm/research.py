"""
Research workflow automation for NLM scientific experiments.

This module provides high-level APIs for common research tasks including
experiment templates, parameter sweeps, statistical analysis, and reproducibility
features specifically designed for scientific research workflows.
"""

import pybind11
import pybind11.stl
from .types import *
from .experiments import (
    ExperimentTemplate, ParameterSweep, MultiSeedExperiment, ReproducibilityManager,
    get_research_templates, get_research_question_templates, get_comparative_experiment_configs,
    get_parameter_optimization_sets, get_scalability_presets, get_advanced_experiment_features
)
from .core import Config, Brain, create_default_config, create_brain, create_simple_world
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

# Global reproducibility manager for research workflows
_reproducibility_manager = ReproducibilityManager()

class ResearchWorkflow:
    """
    High-level research workflow automation.

    This class provides comprehensive research workflow automation including
    experiment design, execution, analysis, and reporting for scientific research.
    """

    def __init__(self, workflow_name: str, description: str):
        """
        Initialize research workflow.

        Args:
            workflow_name: Name of the workflow
            description: Description of the workflow
        """
        self.name = workflow_name
        self.description = description
        self.steps = []
        self.results = {}
        self.config = {}
        self.metadata = {
            'created': datetime.now().isoformat(),
            'version': '1.0',
            'author': 'NLM Research Platform'
        }

    def add_step(self, step_name: str, step_type: str, config: Dict[str, Any]) -> None:
        """
        Add a step to the workflow.

        Args:
            step_name: Name of the step
            step_type: Type of step (experiment, sweep, analysis, etc.)
            config: Configuration for the step
        """
        step = {
            'name': step_name,
            'type': step_type,
            'config': config,
            'status': 'pending',
            'result': None,
            'timestamp': None,
            'duration': None
        }
        self.steps.append(step)

    def configure(self, config: Dict[str, Any]) -> None:
        """
        Configure the workflow.

        Args:
            config: Workflow configuration
        """
        self.config.update(config)

    def set_seed(self, seed: int) -> None:
        """
        Set random seed for reproducibility.

        Args:
            seed: Random seed value
        """
        _reproducibility_manager.set_seed(seed)

    def run_step(self, step_name: str) -> Any:
        """
        Run a specific step.

        Args:
            step_name: Name of the step to run

        Returns:
            Step result
        """
        step = next((s for s in self.steps if s['name'] == step_name), None)
        if not step:
            raise ValueError(f"Step not found: {step_name}")

        logging.info(f"Running step: {step_name}")
        step['status'] = 'running'
        step['timestamp'] = datetime.now().isoformat()

        try:
            result = self._execute_step(step)
            step['result'] = result
            step['status'] = 'completed'
            step['duration'] = (datetime.now() - datetime.fromisoformat(step['timestamp'])).total_seconds()
            self.results[step_name] = result
            return result
        except Exception as e:
            step['status'] = 'failed'
            step['error'] = str(e)
            step['duration'] = (datetime.now() - datetime.fromisoformat(step['timestamp'])).total_seconds()
            logging.error(f"Step {step_name} failed: {e}")
            raise

    def _execute_step(self, step: Dict[str, Any]) -> Any:
        """Execute a workflow step."""
        step_type = step['type']
        config = step['config']

        if step_type == 'experiment':
            return self._run_experiment_step(config)
        elif step_type == 'parameter_sweep':
            return self._run_parameter_sweep_step(config)
        elif step_type == 'multi_seed':
            return self._run_multi_seed_step(config)
        elif step_type == 'statistical_analysis':
            return self._run_statistical_analysis_step(config)
        elif step_type == 'comparative_analysis':
            return self._run_comparative_analysis_step(config)
        elif step_type == 'data_management':
            return self._run_data_management_step(config)
        elif step_type == 'report_generation':
            return self._run_report_generation_step(config)
        else:
            raise ValueError(f"Unknown step type: {step_type}")

    def _run_experiment_step(self, config: Dict[str, Any]) -> 'ExperimentResult':
        """Run an experiment step."""
        experiment_name = config.get('experiment_name', 'research_experiment')
        experiment_config = config.get('experiment_config', {})

        # Create and run experiment
        experiment = ExperimentTemplate('Research', 'Research experiment', experiment_config)
        run_id = _reproducibility_manager.create_experiment_run_id(experiment_name)

        # Save experiment config
        _reproducibility_manager.save_experiment_config(experiment_config, run_id)

        # Run experiment
        experiment_instance = experiment.create_experiment(**config.get('overrides', {}))
        result = experiment_instance.run()

        # Log result
        _reproducibility_manager.log_experiment_result(result)

        return result

    def _run_parameter_sweep_step(self, config: Dict[str, Any]) -> List['ExperimentResult']:
        """Run parameter sweep step."""
        experiment_template = config['experiment_template']
        sweep_config = config['sweep_config']

        sweep = ParameterSweep(sweep_config)
        return sweep.run_sweep(experiment_template)

    def _run_multi_seed_step(self, config: Dict[str, Any]) -> List['ExperimentResult']:
        """Run multi-seed step."""
        base_config = config['base_config']
        num_seeds = config['num_seeds']
        experiment_template = config.get('experiment_template')

        multi_seed = MultiSeedExperiment(base_config, num_seeds)
        return multi_seed.run(experiment_template)

    def _run_statistical_analysis_step(self, config: Dict[str, Any]) -> StatisticalResults:
        """Run statistical analysis step."""
        results = config['results']
        metric = config.get('metric', 'average_firing_rate')

        return analyze_results(results, metric)

    def _run_comparative_analysis_step(self, config: Dict[str, Any]) -> List[ComparisonResults]:
        """Run comparative analysis step."""
        results = config['results']
        metric = config.get('metric', 'average_firing_rate')

        return compare_experiments(results, metric)

    def _run_data_management_step(self, config: Dict[str, Any]) -> bool:
        """Run data management step."""
        results = config['results']
        output_dir = config.get('output_dir', './data')
        format = config.get('format', 'json')

        # Create output directory
        import os
        os.makedirs(output_dir, exist_ok=True)

        # Save results
        filepath = f"{output_dir}/experiment_results.{format}"
        return save_experiment_results(results, filepath, format)

    def _run_report_generation_step(self, config: Dict[str, Any]) -> bool:
        """Run report generation step."""
        results = config['results']
        output_dir = config.get('output_dir', './reports')

        # Create output directory
        import os
        os.makedirs(output_dir, exist_ok=True)

        # Generate reports
        return generate_reports(results, output_dir)

    def run_all_steps(self) -> Dict[str, Any]:
        """
        Run all steps in the workflow.

        Returns:
            Dict[str, Any]: Workflow results
        """
        results = {}

        for step in self.steps:
            if step['status'] in ['pending', 'failed']:
                try:
                    result = self.run_step(step['name'])
                    results[step['name']] = result
                except Exception as e:
                    results[step['name']] = {'error': str(e), 'status': 'failed'}

        return results

    def get_results_summary(self) -> Dict[str, Any]:
        """Get workflow results summary."""
        summary = {
            'workflow_name': self.name,
            'description': self.description,
            'steps': len(self.steps),
            'completed_steps': len([s for s in self.steps if s['status'] == 'completed']),
            'failed_steps': len([s for s in self.steps if s['status'] == 'failed']),
            'pending_steps': len([s for s in self.steps if s['status'] == 'pending']),
            'results': self.results,
            'metadata': self.metadata
        }
        return summary

    def __repr__(self) -> str:
        return f"<ResearchWorkflow: {self.name}, {len(self.steps)} steps>"


class ScientificExperimentDesigner:
    """
    Scientific experiment design with templates and best practices.

    This class provides scientific experiment design capabilities including
    template management, best practices validation, and experimental design
    recommendations.
    """

    def __init__(self):
        """Initialize scientific experiment designer."""
        self.templates = self._load_default_templates()
        self.best_practices = self._load_default_best_practices()

    def _load_default_templates(self) -> Dict[str, ExperimentTemplate]:
        """Load default experiment templates."""
        templates = {}

        # Learning experiment template
        templates['learning'] = ExperimentTemplate(
            name='LearningExperiment',
            description='Template for learning experiments with controlled variables',
            config={
                'neuron_count': 1000,
                'region_count': 2,
                'connection_probability': 0.1,
                'enable_parameter_sweeps': True,
                'enable_multi_objective_optimization': True,
                'enable_adaptive_experimentation': True,
                'enable_comparative_analysis': True,
                'seed': 42,
                'tracking_interval': 100,
                'evaluation_metrics': ['accuracy', 'learning_rate', 'memory_retention']
            }
        )

        # Memory experiment template
        templates['memory'] = ExperimentTemplate(
            name='MemoryExperiment',
            description='Template for memory experiments with statistical validation',
            config={
                'neuron_count': 2000,
                'region_count': 3,
                'connection_probability': 0.15,
                'enable_parameter_sweeps': True,
                'enable_multi_objective_optimization': False,
                'enable_adaptive_experimentation': False,
                'enable_comparative_analysis': True,
                'seed': 123,
                'num_seeds': 10,
                'evaluation_metrics': ['memory_capacity', 'recall_accuracy', 'forgetting_curve']
            }
        )

        # Social experiment template
        templates['social'] = ExperimentTemplate(
            name='SocialExperiment',
            description='Template for social behavior experiments',
            config={
                'num_agents': 100,
                'region_count': 2,
                'connection_probability': 0.2,
                'enable_parameter_sweeps': True,
                'enable_multi_objective_optimization': True,
                'enable_adaptive_experimentation': True,
                'enable_comparative_analysis': True,
                'seed': 456,
                'evaluation_metrics': ['cooperation_rate', 'imitation_probability', 'social_influence']
            }
        )

        return templates

    def _load_default_best_practices(self) -> List[Dict[str, Any]]:
        """Load default best practices."""
        return [
            {
                'name': 'Random Seed Management',
                'description': 'Always set and document random seeds for reproducibility',
                'impact': 'high',
                'implementation': 'Use reproducible random number generators and seed documentation'
            },
            {
                'name': 'Multi-seed Validation',
                'description': 'Run experiments with multiple seeds for statistical validation',
                'impact': 'high',
                'implementation': 'Run at least 5-10 seeds with different random states'
            },
            {
                'name': 'Parameter Sweep Design',
                'description': 'Design parameter sweeps with meaningful ranges and grid density',
                'impact': 'medium',
                'implementation': 'Use logarithmic scaling for parameter ranges and ensure coverage'
            },
            {
                'name': 'Statistical Analysis',
                'description': 'Apply appropriate statistical tests and report confidence intervals',
                'impact': 'high',
                'implementation': 'Use t-tests, ANOVA, or non-parametric tests as appropriate'
            },
            {
                'name': 'Experiment Documentation',
                'description': 'Document all experimental procedures, configurations, and results',
                'impact': 'high',
                'implementation': 'Maintain detailed experiment logs and configuration backups'
            },
            {
                'name': 'Performance Monitoring',
                'description': 'Monitor and report computational performance and resource usage',
                'impact': 'medium',
                'implementation': 'Track timing, memory usage, and system resource consumption'
            },
            {
                'name': 'Version Control',
                'description': 'Version control all experimental code and configurations',
                'impact': 'high',
                'implementation': 'Use git with detailed commit messages and tags for experiments'
            }
        ]

    def get_template(self, template_name: str) -> Optional[ExperimentTemplate]:
        """
        Get experiment template by name.

        Args:
            template_name: Name of the template

        Returns:
            Optional[ExperimentTemplate]: Template or None if not found
        """
        return self.templates.get(template_name)

    def get_all_templates(self) -> List[str]:
        """Get all template names."""
        return list(self.templates.keys())

    def create_workflow_from_template(self, template_name: str, workflow_name: str, **overrides) -> ResearchWorkflow:
        """
        Create research workflow from template.

        Args:
            template_name: Name of the template
            workflow_name: Name of the workflow
            **overrides: Configuration overrides

        Returns:
            ResearchWorkflow: Created workflow
        """
        template = self.get_template(template_name)
        if not template:
            raise ValueError(f"Unknown template: {template_name}")

        # Create workflow
        workflow = ResearchWorkflow(workflow_name, f"Workflow based on {template_name} template")

        # Configure with template config and overrides
        config = template.config.copy()
        config.update(overrides)
        workflow.configure(config)

        # Add default steps
        workflow.add_step('create_config', 'experiment', {
            'experiment_name': f"{workflow_name}_config",
            'experiment_config': config
        })

        workflow.add_step('run_experiment', 'experiment', {
            'experiment_name': f"{workflow_name}_experiment",
            'experiment_config': config,
            'overrides': overrides
        })

        workflow.add_step('analyze_results', 'statistical_analysis', {
            'results': 'previous_step_result',
            'metric': 'average_firing_rate'
        })

        workflow.add_step('save_results', 'data_management', {
            'results': 'previous_step_result',
            'format': 'json'
        })

        return workflow

    def validate_experiment_config(self, config: Dict[str, Any]) -> List[str]:
        """
        Validate experiment configuration against best practices.

        Args:
            config: Experiment configuration to validate

        Returns:
            List[str]: List of validation issues (empty if valid)
        """
        issues = []

        # Check for seed
        if 'seed' not in config:
            issues.append("Experiment configuration should include a random seed for reproducibility")

        # Check for seed range
        if 'seed' in config and (config['seed'] < 0 or config['seed'] > 1000000):
            issues.append("Seed value should be in reasonable range (0-1000000)")

        # Check neuron count
        if 'neuron_count' in config and config['neuron_count'] > 100000:
            issues.append("Large neuron counts (>100,000) may cause performance issues")

        # Check for multi-seed validation
        if 'seed' in config and 'num_seeds' not in config:
            issues.append("Consider using multiple seeds for statistical validation")

        # Check for parameter sweeps
        if 'enable_parameter_sweeps' not in config:
            issues.append("Consider enabling parameter sweeps for comprehensive exploration")

        # Check for multi-objective optimization
        if 'enable_multi_objective_optimization' not in config:
            issues.append("Consider enabling multi-objective optimization for better solutions")

        return issues

    def get_best_practice(self, practice_name: str) -> Optional[Dict[str, Any]]:
        """
        Get best practice by name.

        Args:
            practice_name: Name of the best practice

        Returns:
            Optional[Dict[str, Any]]: Best practice or None if not found
        """
        for practice in self.best_practices:
            if practice['name'].lower() == practice_name.lower():
                return practice
        return None

    def get_all_best_practices(self) -> List[Dict[str, Any]]:
        """Get all best practices."""
        return self.best_practices

    def __repr__(self) -> str:
        return f"<ScientificExperimentDesigner: {len(self.templates)} templates, {len(self.best_practices)} best practices>"


class ExperimentReproducibilityManager:
    """
    Advanced experiment reproducibility and version control.

    This class provides advanced reproducibility features including
    experiment versioning, parameter tracking, and result preservation.
    """

    def __init__(self, data_dir: str = "./experiment_data"):
        """
        Initialize reproducibility manager.

        Args:
            data_dir: Directory for experiment data
        """
        self.data_dir = data_dir
        self.experiment_registry = {}
        self.parameter_registry = {}
        self.result_registry = {}

    def register_experiment(self, experiment_id: str, config: Dict[str, Any],
                            metadata: Optional[Dict[str, Any]] = None) -> bool:
        """
        Register an experiment in the registry.

        Args:
            experiment_id: Unique experiment identifier
            config: Experiment configuration
            metadata: Optional metadata

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            experiment_record = {
                'id': experiment_id,
                'config': config,
                'metadata': metadata or {},
                'created': datetime.now().isoformat(),
                'last_modified': datetime.now().isoformat(),
                'status': 'registered',
                'results': []
            }

            self.experiment_registry[experiment_id] = experiment_record

            # Create experiment directory
            exp_dir = f"{self.data_dir}/{experiment_id}"
            import os
            os.makedirs(exp_dir, exist_ok=True)

            # Save experiment configuration
            config_file = f"{exp_dir}/config.json"
            with open(config_file, 'w') as f:
                json.dump(config, f, indent=2)

            logging.info(f"Registered experiment: {experiment_id}")
            return True

        except Exception as e:
            logging.error(f"Error registering experiment: {e}")
            return False

    def register_parameter_set(self, parameter_id: str, parameters: Dict[str, Any],
                               description: str = "") -> bool:
        """
        Register a parameter set.

        Args:
            parameter_id: Unique parameter set identifier
            parameters: Parameter set configuration
            description: Description of parameter set

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            parameter_record = {
                'id': parameter_id,
                'parameters': parameters,
                'description': description,
                'created': datetime.now().isoformat(),
                'usage_count': 0
            }

            self.parameter_registry[parameter_id] = parameter_record
            return True

        except Exception as e:
            logging.error(f"Error registering parameter set: {e}")
            return False

    def register_result(self, experiment_id: str, result: 'ExperimentResult') -> bool:
        """
        Register experiment result.

        Args:
            experiment_id: Experiment identifier
            result: Experiment result

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            if experiment_id not in self.experiment_registry:
                logging.error(f"Experiment not registered: {experiment_id}")
                return False

            # Create result record
            result_record = {
                'experiment_id': experiment_id,
                'result': result.__dict__,
                'created': datetime.now().isoformat(),
                'status': 'completed'
            }

            self.experiment_registry[experiment_id]['results'].append(result_record)
            self.experiment_registry[experiment_id]['last_modified'] = datetime.now().isoformat()

            # Save result
            exp_dir = f"{self.data_dir}/{experiment_id}"
            result_file = f"{exp_dir}/result_{len(self.experiment_registry[experiment_id]['results'])}.json"
            with open(result_file, 'w') as f:
                json.dump(result_record, f, indent=2, default=str)

            # Update parameter registry usage count
            if 'parameter_set_id' in result.config:
                param_id = result.config['parameter_set_id']
                if param_id in self.parameter_registry:
                    self.parameter_registry[param_id]['usage_count'] += 1

            logging.info(f"Registered result for experiment: {experiment_id}")
            return True

        except Exception as e:
            logging.error(f"Error registering result: {e}")
            return False

    def get_experiment(self, experiment_id: str) -> Optional[Dict[str, Any]]:
        """
        Get experiment by ID.

        Args:
            experiment_id: Experiment identifier

        Returns:
            Optional[Dict[str, Any]]: Experiment record or None
        """
        return self.experiment_registry.get(experiment_id)

    def get_parameter_set(self, parameter_id: str) -> Optional[Dict[str, Any]]:
        """
        Get parameter set by ID.

        Args:
            parameter_id: Parameter set identifier

        Returns:
            Optional[Dict[str, Any]]: Parameter set or None
        """
        return self.parameter_registry.get(parameter_id)

    def get_experiment_results(self, experiment_id: str) -> List[Dict[str, Any]]:
        """
        Get all results for an experiment.

        Args:
            experiment_id: Experiment identifier

        Returns:
            List[Dict[str, Any]]: Experiment results
        """
        experiment = self.get_experiment(experiment_id)
        if not experiment:
            return []
        return experiment['results']

    def compare_experiments(self, experiment_ids: List[str], metric: str = "average_firing_rate") -> Dict[str, Any]:
        """
        Compare multiple experiments.

        Args:
            experiment_ids: List of experiment identifiers
            metric: Metric to compare

        Returns:
            Dict[str, Any]: Comparison results
        """
        experiments = []
        for exp_id in experiment_ids:
            experiment = self.get_experiment(exp_id)
            if experiment:
                experiments.append(experiment)

        if not experiments:
            return {}

        # Extract results for each experiment
        comparison_data = {}
        for experiment in experiments:
            exp_id = experiment['id']
            results = []
            
            for result_record in experiment['results']:
                if result_record['status'] == 'completed':
                    result = result_record['result']
                    if metric == "average_firing_rate":
                        results.append(result.get('average_firing_rate', 0.0))
                    elif metric == "final_spike_count":
                        results.append(result.get('final_spike_count', 0))
                    elif metric in result.get('custom_metrics', {}):
                        results.append(result['custom_metrics'][metric])

            if results:
                comparison_data[exp_id] = {
                    'mean': np.mean(results),
                    'std': np.std(results),
                    'min': np.min(results),
                    'max': np.max(results),
                    'count': len(results)
                }

        return comparison_data

    def __repr__(self) -> str:
        return f"<ExperimentReproducibilityManager: {len(self.experiment_registry)} experiments, {len(self.parameter_registry)} parameter sets>"


# Research workflow management


def create_research_workflow(workflow_name: str, description: str) -> ResearchWorkflow:
    """
    Create a research workflow.

    Args:
        workflow_name: Name of the workflow
        description: Description of the workflow

    Returns:
        ResearchWorkflow: Created workflow
    """
    return ResearchWorkflow(workflow_name, description)


def create_experiment_designer() -> ScientificExperimentDesigner:
    """Create scientific experiment designer."""
    return ScientificExperimentDesigner()


def create_reproducibility_manager(data_dir: str = "./experiment_data") -> ExperimentReproducibilityManager:
    """
    Create experiment reproducibility manager.

    Args:
        data_dir: Directory for experiment data

    Returns:
        ExperimentReproducibilityManager: Created manager
    """
    return ExperimentReproducibilityManager(data_dir)


def run_research_workflow(workflow: ResearchWorkflow) -> Dict[str, Any]:
    """
    Run research workflow.

    Args:
        workflow: Research workflow to run

    Returns:
        Dict[str, Any]: Workflow results
    """
    return workflow.run_all_steps()


def analyze_research_workflow(workflow: ResearchWorkflow) -> Dict[str, Any]:
    """
    Analyze research workflow results.

    Args:
        workflow: Research workflow

    Returns:
        Dict[str, Any]: Analysis results
    """
    return workflow.get_results_summary()


# Global research workflow managers

_research_designer = None
_reproducibility_manager = None


def get_research_designer() -> ScientificExperimentDesigner:
    """Get global research designer."""
    global _research_designer
    if _research_designer is None:
        _research_designer = ScientificExperimentDesigner()
    return _research_designer


def get_reproducibility_manager() -> ExperimentReproducibilityManager:
    """Get global reproducibility manager."""
    global _reproducibility_manager
    if _reproducibility_manager is None:
        _reproducibility_manager = ExperimentReproducibilityManager()
    return _reproducibility_manager


def reset_reproducibility_manager() -> None:
    """Reset global reproducibility manager."""
    global _reproducibility_manager
    _reproducibility_manager = ExperimentReproducibilityManager()


# Export key classes and functions
__all__ = [
    'ResearchWorkflow', 'ScientificExperimentDesigner', 'ExperimentReproducibilityManager',
    'create_research_workflow', 'create_experiment_designer', 'create_reproducibility_manager',
    'run_research_workflow', 'analyze_research_workflow',
    'get_research_designer', 'get_reproducibility_manager', 'reset_reproducibility_manager'
]