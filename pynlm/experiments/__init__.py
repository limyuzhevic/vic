"""
pynlm.experiments - Experiment helpers for NLM project

This module provides experiment helpers including:
- Experiment templates and runners
- Data collection and logging
- Parameter sweep utilities
- Baseline and comparison setups
"""

import time
import json
import os
import warnings
from typing import Dict, Any, List, Optional, Callable, Union
import numpy as np
import pandas as pd
from pathlib import Path
import logging
from dataclasses import dataclass, field
from abc import ABC, abstractmethod

# Optional dependency handling
try:
    import yaml
    HAS_YAML = True
except ImportError:
    HAS_YAML = False
    yaml = None
    warnings.warn("PyYAML is not installed. YAML export functionality is disabled.")

logger = logging.getLogger(__name__)


@dataclass
class ExperimentConfig:
    """Configuration for an experiment."""
    name: str
    description: Optional[str] = None
    parameters: Dict[str, Any] = field(default_factory=dict)
    tags: List[str] = field(default_factory=list)
    metadata: Dict[str, Any] = field(default_factory=dict)
    created_at: str = field(default_factory=lambda: pd.Timestamp.now().isoformat())
    author: Optional[str] = None
    version: str = "1.0.0"
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert config to dictionary."""
        return {
            "name": self.name,
            "description": self.description,
            "parameters": self.parameters,
            "tags": self.tags,
            "metadata": self.metadata,
            "created_at": self.created_at,
            "author": self.author,
            "version": self.version
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'ExperimentConfig':
        """Create config from dictionary."""
        return cls(**data)


class Experiment(ABC):
    """Base class for all experiments."""
    
    def __init__(self, config: ExperimentConfig):
        """
        Initialize experiment.
        
        Parameters
        ----------
        config : ExperimentConfig
            Experiment configuration
        """
        self.config = config
        self.results = {}
        self.metrics = {}
        self.logs = []
        self.start_time = None
        self.end_time = None
        self.status = "initialized"
        
        logger.info(f"Initialized experiment: {config.name}")
    
    @abstractmethod
    def run(self) -> Dict[str, Any]:
        """Run the experiment. Must be implemented by subclasses."""
        pass
    
    def get_duration(self) -> Optional[float]:
        """Get experiment duration in seconds."""
        if self.start_time and self.end_time:
            return self.end_time - self.start_time
        return None
    
    def get_results(self) -> Dict[str, Any]:
        """Get experiment results."""
        return self.results.copy()
    
    def add_result(self, key: str, value: Any) -> None:
        """Add a result entry."""
        self.results[key] = value
        logger.debug(f"Added result: {key} = {value}")
    
    def add_metric(self, key: str, value: Any) -> None:
        """Add a metric entry."""
        self.metrics[key] = value
        logger.debug(f"Added metric: {key} = {value}")
    
    def log(self, message: str, level: str = "info") -> None:
        """
        Log a message.
        
        Parameters
        ----------
        message : str
            Log message
        level : str, optional
            Log level (debug, info, warning, error), by default "info"
        """
        log_entry = {
            "timestamp": pd.Timestamp.now().isoformat(),
            "message": message,
            "level": level
        }
        self.logs.append(log_entry)
        
        if level == "debug":
            logger.debug(message)
        elif level == "info":
            logger.info(message)
        elif level == "warning":
            logger.warning(message)
        elif level == "error":
            logger.error(message)
    
    def save_results(self, path: str) -> None:
        """
        Save experiment results to file.
        
        Parameters
        ----------
        path : str
            Path to save results
        """
        path = Path(path)
        path.parent.mkdir(parents=True, exist_ok=True)
        
        results = {
            "config": self.config.to_dict(),
            "results": self.results,
            "metrics": self.metrics,
            "logs": self.logs,
            "start_time": self.start_time,
            "end_time": self.end_time,
            "status": self.status,
            "duration": self.get_duration()
        }
        
        if path.suffix.lower() in ['.json', '.yaml', '.yml']:
            if path.suffix.lower() == '.json':
                with open(path, 'w') as f:
                    json.dump(results, f, indent=2)
            else:
                if not HAS_YAML:
                    raise ImportError("YAML export requires PyYAML. Install it with 'pip install pyyaml'.")
                with open(path, 'w') as f:
                    yaml.dump(results, f, default_flow_style=False)
        else:
            with open(path, 'w') as f:
                json.dump(results, f, indent=2)
        
        logger.info(f"Saved experiment results to {path}")


class ExperimentRunner:
    """Runner for experiments."""
    
    def __init__(self):
        self.experiments = []
        self.results = []
        
    def add_experiment(self, experiment: Experiment) -> None:
        """Add an experiment to the runner."""
        self.experiments.append(experiment)
        logger.info(f"Added experiment: {experiment.config.name}")
    
    def run_all(self) -> List[Dict[str, Any]]:
        """Run all experiments and return results."""
        experiment_results = []
        
        for experiment in self.experiments:
            try:
                experiment.status = "running"
                experiment.start_time = time.time()
                
                experiment.log(f"Starting experiment: {experiment.config.name}")
                result = experiment.run()
                
                experiment.end_time = time.time()
                experiment.status = "completed"
                
                experiment.log(f"Completed experiment: {experiment.config.name}", "info")
                experiment_results.append(result)
                
            except Exception as e:
                experiment.end_time = time.time()
                experiment.status = "failed"
                experiment.log(f"Experiment failed: {str(e)}", "error")
                raise
        
        self.results = experiment_results
        return experiment_results
    
    def get_summary(self) -> Dict[str, Any]:
        """Get summary of all experiments."""
        summary = {
            "total_experiments": len(self.experiments),
            "successful_experiments": len([e for e in self.experiments if e.status == "completed"]),
            "failed_experiments": len([e for e in self.experiments if e.status == "failed"]),
            "total_time": sum([e.get_duration() or 0 for e in self.experiments if e.get_duration()]),
            "results": self.results
        }
        return summary


class ParameterSweep:
    """Parameter sweep utility."""
    
    def __init__(self, base_config: Dict[str, Any]):
        """
        Initialize parameter sweep.
        
        Parameters
        ----------
        base_config : Dict[str, Any]
            Base configuration
        """
        self.base_config = base_config
        self.parameter_grid = []
        
    def add_parameter(self, name: str, values: List[Any]) -> None:
        """
        Add a parameter to the sweep.
        
        Parameters
        ----------
        name : str
            Parameter name
        values : List[Any]
            Parameter values
        """
        self.parameter_grid.append({"name": name, "values": values})
    
    def generate_configs(self) -> List[Dict[str, Any]]:
        """
        Generate all parameter combinations.
        
        Returns
        -------
        List[Dict[str, Any]]
            List of configuration dictionaries
        """
        if not self.parameter_grid:
            return [self.base_config.copy()]
        
        # Generate all combinations
        param_combinations = self._generate_combinations()
        
        configs = []
        for combination in param_combinations:
            config = self.base_config.copy()
            config.update(combination)
            configs.append(config)
        
        logger.info(f"Generated {len(configs)} parameter combinations")
        return configs
    
    def _generate_combinations(self) -> List[Dict[str, Any]]:
        """Generate all combinations of parameter values."""
        if len(self.parameter_grid) == 0:
            return [{}]
        
        current_param = self.parameter_grid[0]
        remaining_params = self.parameter_grid[1:]
        
        combinations = []
        
        for value in current_param["values"]:
            for sub_combination in self._generate_combinations_from_params(remaining_params):
                combinations.append({current_param["name"]: value, **sub_combination})
        
        return combinations
    
    def _generate_combinations_from_params(self, params: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Generate combinations from parameter list."""
        if len(params) == 0:
            return [{}]
        
        current_param = params[0]
        remaining_params = params[1:]
        
        combinations = []
        
        for value in current_param["values"]:
            for sub_combination in self._generate_combinations_from_params(remaining_params):
                combinations.append({current_param["name"]: value, **sub_combination})
        
        return combinations
    
    def run_sweep(self, experiment_class: type, **kwargs) -> ExperimentRunner:
        """
        Run a parameter sweep.
        
        Parameters
        ----------
        experiment_class : type
            Experiment class to instantiate
        **kwargs
            Additional arguments for experiment runner
            
        Returns
        -------
        ExperimentRunner
            Experiment runner with results
        """
        configs = self.generate_configs()
        runner = ExperimentRunner(**kwargs)
        
        for i, config in enumerate(configs):
            experiment_name = f"{self.base_config.get('name', 'sweep')}_param_{i}"
            config["name"] = experiment_name
            
            experiment = experiment_class(ExperimentConfig(**config))
            runner.add_experiment(experiment)
        
        results = runner.run_all()
        return runner


class BaselineSetup:
    """Setup for baseline experiments."""
    
    def __init__(self):
        self.baselines = {}
        self.comparisons = {}
    
    def add_baseline(self, name: str, config: Dict[str, Any]) -> None:
        """
        Add a baseline.
        
        Parameters
        ----------
        name : str
            Baseline name
        config : Dict[str, Any]
            Baseline configuration
        """
        self.baselines[name] = config
        logger.info(f"Added baseline: {name}")
    
    def add_comparison(self, name: str, config: Dict[str, Any], baseline_name: str) -> None:
        """
        Add a comparison experiment.
        
        Parameters
        ----------
        name : str
            Comparison name
        config : Dict[str, Any]
            Comparison configuration
        baseline_name : str
            Name of baseline to compare against
        """
        if baseline_name not in self.baselines:
            raise ValueError(f"Baseline not found: {baseline_name}")
        
        self.comparisons[name] = {
            "config": config,
            "baseline_name": baseline_name
        }
        logger.info(f"Added comparison: {name}")
    
    def run_experiment(self, experiment_class: type, name: str, 
                      config: Dict[str, Any]) -> Experiment:
        """
        Run a single experiment.
        
        Parameters
        ----------
        experiment_class : type
            Experiment class
        name : str
            Experiment name
        config : Dict[str, Any]
            Experiment configuration
            
        Returns
        -------
        Experiment
            Completed experiment
        """
        experiment_config = ExperimentConfig(
            name=name,
            parameters=config
        )
        
        experiment = experiment_class(experiment_config)
        experiment.log(f"Running experiment: {name}")
        experiment.run()
        
        return experiment
    
    def run_all_baselines(self, experiment_class: type) -> Dict[str, Experiment]:
        """
        Run all baselines.
        
        Parameters
        ----------
        experiment_class : type
            Experiment class
            
        Returns
        -------
        Dict[str, Experiment]
            Dictionary of baseline experiments
        """
        results = {}
        
        for name, config in self.baselines.items():
            experiment = self.run_experiment(experiment_class, name, config)
            results[name] = experiment
        
        return results


class DataLogger:
    """Data collection and logging utility."""
    
    def __init__(self, log_dir: str = "./logs"):
        """
        Initialize data logger.
        
        Parameters
        ----------
        log_dir : str, optional
            Directory for logs, by default "./logs"
        """
        self.log_dir = Path(log_dir)
        self.log_dir.mkdir(parents=True, exist_ok=True)
        self.experiment_data = {}
        
    def log_experiment(self, experiment_name: str, data: Dict[str, Any]) -> None:
        """
        Log experiment data.
        
        Parameters
        ----------
        experiment_name : str
            Experiment name
        data : Dict[str, Any]
            Experiment data
        """
        if experiment_name not in self.experiment_data:
            self.experiment_data[experiment_name] = []
        
        self.experiment_data[experiment_name].append(data)
        
        # Log to file
        log_file = self.log_dir / f"{experiment_name}.json"
        with open(log_file, 'w') as f:
            json.dump(self.experiment_data, f, indent=2)
        
        logger.info(f"Logged data for experiment: {experiment_name}")
    
    def get_experiment_data(self, experiment_name: str) -> List[Dict[str, Any]]:
        """
        Get experiment data.
        
        Parameters
        ----------
        experiment_name : str
            Experiment name
            
        Returns
        -------
        List[Dict[str, Any]]
            Experiment data
        """
        return self.experiment_data.get(experiment_name, [])
    
    def create_dataframe(self, experiment_name: str) -> pd.DataFrame:
        """
        Create DataFrame from experiment data.
        
        Parameters
        ----------
        experiment_name : str
            Experiment name
            
        Returns
        -------
        pd.DataFrame
            DataFrame containing experiment data
        """
        data = self.get_experiment_data(experiment_name)
        
        if not data:
            return pd.DataFrame()
        
        return pd.DataFrame(data)


class ResultCollector:
    """Collect and analyze experiment results."""
    
    def __init__(self):
        self.results = {}
        self.metrics = {}
        
    def add_experiment_result(self, experiment_name: str, result: Dict[str, Any]) -> None:
        """
        Add experiment result.
        
        Parameters
        ----------
        experiment_name : str
            Experiment name
        result : Dict[str, Any]
            Experiment result
        """
        self.results[experiment_name] = result
        
        # Extract metrics if present
        if "metrics" in result:
            self.metrics[experiment_name] = result["metrics"]
        
        logger.info(f"Added result for experiment: {experiment_name}")
    
    def get_result(self, experiment_name: str) -> Optional[Dict[str, Any]]:
        """
        Get experiment result.
        
        Parameters
        ----------
        experiment_name : str
            Experiment name
            
        Returns
        -------
        Optional[Dict[str, Any]]
            Experiment result or None if not found
        """
        return self.results.get(experiment_name)
    
    def compare_results(self, experiment_names: List[str], metric: str) -> Dict[str, Any]:
        """
        Compare results across experiments.
        
        Parameters
        ----------
        experiment_names : List[str]
            Experiment names to compare
        metric : str
            Metric to compare
            
        Returns
        -------
        Dict[str, Any]
            Comparison results
        """
        comparison = {}
        
        for experiment_name in experiment_names:
            if experiment_name in self.metrics:
                comparison[experiment_name] = self.metrics[experiment_name].get(metric)
        
        return comparison
    
    def get_summary_statistics(self) -> Dict[str, Any]:
        """
        Get summary statistics of all results.
        
        Returns
        -------
        Dict[str, Any]
            Summary statistics
        """
        summary = {
            "total_experiments": len(self.results),
            "metrics_summary": {}
        }
        
        # Calculate summary statistics for each metric
        for experiment_name, metrics in self.metrics.items():
            for metric_name, value in metrics.items():
                if metric_name not in summary["metrics_summary"]:
                    summary["metrics_summary"][metric_name] = []
                
                if isinstance(value, (int, float)):
                    summary["metrics_summary"][metric_name].append(value)
        
        # Calculate statistics
        for metric_name, values in summary["metrics_summary"].items():
            if values:
                summary["metrics_summary"][metric_name] = {
                    "mean": np.mean(values),
                    "std": np.std(values),
                    "min": np.min(values),
                    "max": np.max(values),
                    "median": np.median(values)
                }
        
        return summary


class ExperimentTemplate:
    """Template for common experiment setups."""
    
    @staticmethod
    def create_training_experiment(name: str, model_type: str, 
                                 dataset_name: str) -> Dict[str, Any]:
        """
        Create a training experiment configuration.
        
        Parameters
        ----------
        name : str
            Experiment name
        model_type : str
            Model type (e.g., "transformer", "cnn", "rnn")
        dataset_name : str
            Dataset name
            
        Returns
        -------
        Dict[str, Any]
            Experiment configuration
        """
        return {
            "name": name,
            "type": "training",
            "model": {
                "type": model_type,
                "parameters": {}
            },
            "dataset": {
                "name": dataset_name,
                "parameters": {}
            },
            "training": {
                "epochs": 100,
                "batch_size": 32,
                "learning_rate": 0.001,
                "optimizer": "adam",
                "loss_function": "crossentropy"
            },
            "evaluation": {
                "metrics": ["accuracy", "perplexity", "bleu"],
                "split": "validation"
            }
        }
    
    @staticmethod
    def create_evaluation_experiment(name: str, model_path: str,
                                   test_dataset: str) -> Dict[str, Any]:
        """
        Create an evaluation experiment configuration.
        
        Parameters
        ----------
        name : str
            Experiment name
        model_path : str
            Path to model file
        test_dataset : str
            Test dataset name
            
        Returns
        -------
        Dict[str, Any]
            Experiment configuration
        """
        return {
            "name": name,
            "type": "evaluation",
            "model": {
                "path": model_path,
                "parameters": {}
            },
            "dataset": {
                "name": test_dataset,
                "parameters": {}
            },
            "evaluation": {
                "metrics": ["accuracy", "perplexity", "bleu", "rouge"],
                "batch_size": 32,
                "split": "test"
            }
        }
    
    @staticmethod
    def create_ablation_experiment(name: str, baseline_name: str,
                                 modifications: List[Dict[str, Any]]) -> Dict[str, Any]:
        """
        Create an ablation experiment configuration.
        
        Parameters
        ----------
        name : str
            Experiment name
        baseline_name : str
            Baseline experiment name
        modifications : List[Dict[str, Any]]
            List of modifications to apply
            
        Returns
        -------
        Dict[str, Any]
            Experiment configuration
        """
        return {
            "name": name,
            "type": "ablation",
            "baseline": baseline_name,
            "modifications": modifications,
            "evaluation": {
                "metrics": ["accuracy", "perplexity", "bleu"],
                "baseline_comparison": True
            }
        }


# Alias for backward compatibility
BaselineExperiment = BaselineSetup
ExperimentRunnerAlias = ExperimentRunner