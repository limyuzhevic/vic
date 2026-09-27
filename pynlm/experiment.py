from dataclasses import dataclass, field
"""
Experiment Management - Experiment Lifecycle and Data Collection
===============================================================

Implementation of experiment management system:
- Experiment lifecycle management
- Parameter sweep execution
- Data collection and storage
- Experiment comparison
- Result visualization
"""

from typing import List, Dict, Any, Optional, Tuple
import numpy as np
import time
import json
import os
import pandas as pd
from dataclasses import dataclass, field
from abc import ABC, abstractmethod
@dataclass
class ExperimentResult:
    """Result from a single experiment run."""
    
    experiment_id: str
    parameters: Dict[str, Any]
    results: Dict[str, Any]
    start_time: float
    end_time: float
    duration: float
    metrics: Dict[str, List[float]] = field(default_factory=dict)
    metadata: Dict[str, Any] = field(default_factory=dict)
class Experiment:
    """
    Experiment management system for controlled simulations.
    
    Manages experiment lifecycle with parameter sweep execution,
    data collection and storage, and experiment comparison:
    - Experiment definition and configuration
    - Parameter sweep execution
    - Data collection and storage
    - Experiment comparison and analysis
    - Result visualization
    
    Attributes:
        experiment_id (str): Experiment identifier
        brain (Any): Brain instance for simulation
        parameters (Dict[str, Any]): Experiment parameters
        data_dir (str): Data directory
    """
    
    def __init__(self, experiment_id: str, brain: Any = None,
                 parameters: Dict[str, Any] = None, data_dir: str = None):
        """
        Initialize experiment.
        
        Args:
            experiment_id (str): Experiment identifier
            brain (Any, optional): Brain instance
            parameters (Dict[str, Any], optional): Experiment parameters
            data_dir (str, optional): Data directory
        """
        self.experiment_id = experiment_id
        self.brain = brain
        self.parameters = parameters or {}
        self.data_dir = data_dir or f"experiments/{experiment_id}"
        
        # Ensure data directory exists
        os.makedirs(self.data_dir, exist_ok=True)
        
        # Experiment state
        self.is_running = False
        self.current_step = 0
        self.start_time = 0.0
        self.end_time = 0.0
        self.results: List[ExperimentResult] = []
        
    def run(self, run_id: str = None) -> ExperimentResult:
        """
        Run experiment.
        
        Args:
            run_id (str, optional): Run identifier
            
        Returns:
            ExperimentResult: Experiment result
        """
        if not self.brain:
            raise ValueError("Brain not set. Cannot run experiment.")
            
        run_id = run_id or f"run_{len(self.results)}"
        self.is_running = True
        self.current_step = 0
        self.start_time = time.time()
        
        try:
            # Store initial configuration
            initial_config = self.brain.config.to_dict() if hasattr(self.brain.config, 'to_dict') else {}
            
            # Run simulation
            while self.current_step < self.parameters.get("max_steps", 1000):
                # Check if we should stop early
                if self._should_stop():
                    break
                    
                # Perform simulation step
                self._simulation_step()
                self.current_step += 1
                
                # Collect data periodically
                if self.current_step % self.parameters.get("data_collection_interval", 10) == 0:
                    self._collect_data()
                    
            self.end_time = time.time()
            duration = self.end_time - self.start_time
            
            # Collect final data
            self._collect_data()
            
            # Get final metrics
            metrics = self._get_current_metrics()
            
            # Create result
            result = ExperimentResult(
                experiment_id=self.experiment_id,
                parameters=self.parameters.copy(),
                results=metrics,
                start_time=self.start_time,
                end_time=self.end_time,
                duration=duration,
                metrics=metrics,
                metadata={"run_id": run_id}
            )
            
            self.results.append(result)
            
            return result
            
        finally:
            self.is_running = False
            
    def _simulation_step(self):
        """Perform one simulation step."""
        if not self.brain:
            return
            
        # Update brain state
        self.brain.step(self.current_step)
        
        # Apply any experiment-specific modifications
        self._apply_experiment_modifications()
        
    def _should_stop(self) -> bool:
        """
        Check if simulation should stop.
        
        Returns:
            bool: True if should stop
        """
        max_steps = self.parameters.get("max_steps", 1000)
        duration = self.parameters.get("duration", 100.0)
        dt = self.parameters.get("timestep", 0.001)
        
        return (self.current_step >= max_steps or
                self.state.time_since_start >= duration)
        
    def _collect_data(self):
        """Collect simulation data."""
        # This would normally collect detailed data
        # For now, just get current metrics
        metrics = self._get_current_metrics()
        
        # Store metrics for each parameter combination
        for param_key, param_values in self._get_parameter_sweeps().items():
            for value in param_values:
                # Store metrics with parameter combination
                pass
                
    def _get_current_metrics(self) -> Dict[str, Any]:
        """
        Get current simulation metrics.
        
        Returns:
            Dict[str, Any]: Current metrics
        """
        if not self.brain:
            return {}
            
        brain_stats = self.brain.get_stats() if hasattr(self.brain, 'get_stats') else {}
        
        # Add experiment-specific metrics
        metrics = {
            "steps_completed": self.current_step,
            "simulation_time": self.end_time - self.start_time,
            "brain_stats": brain_stats
        }
        
        return metrics
        
    def _apply_experiment_modifications(self):
        """Apply experiment-specific modifications to brain."""
        pass
        
    def _get_parameter_sweeps(self) -> Dict[str, List[Any]]:
        """
        Get parameter sweeps for this experiment.
        
        Returns:
            Dict[str, List[Any]]: Parameter sweeps
        """
        return self.parameters.get("parameter_sweeps", {})
        
    def add_parameter_sweep(self, param_name: str, values: List[Any]):
        """
        Add parameter sweep.
        
        Args:
            param_name (str): Parameter name
            values (List[Any]): Parameter values
        """
        if "parameter_sweeps" not in self.parameters:
            self.parameters["parameter_sweeps"] = {}
            
        self.parameters["parameter_sweeps"][param_name] = values
        
    def run_parameter_sweep(self) -> List[ExperimentResult]:
        """
        Run parameter sweep experiment.
        
        Returns:
            List[ExperimentResult]: All experiment results
        """
        parameter_sweeps = self.parameters.get("parameter_sweeps", {})
        
        if not parameter_sweeps:
            return [self.run()]
            
        # Generate all parameter combinations
        import itertools
        param_names = list(parameter_sweeps.keys())
        param_values = list(parameter_sweeps.values())
        
        all_combinations = list(itertools.product(*param_values))
        
        results = []
        for i, combination in enumerate(all_combinations):
            # Create parameter combination
            run_params = self.parameters.copy()
            run_params.update(dict(zip(param_names, combination)))
            
            # Run experiment with these parameters
            run_params["parameter_combination"] = i
            run_result = self.run(f"run_{i}")
            run_result.parameters.update(run_params)
            
            results.append(run_result)
            
        return results
        
    def compare_experiments(self, experiment_results: List[ExperimentResult],
                           metric: str = "brain_stats") -> Dict[str, Any]:
        """
        Compare multiple experiment results.
        
        Args:
            experiment_results (List[ExperimentResult]): Experiment results
            metric (str): Metric to compare
            
        Returns:
            Dict[str, Any]: Comparison results
        """
        if not experiment_results:
            return {}
            
        # Extract metrics from all results
        metric_values = []
        parameter_values = []
        
        for result in experiment_results:
            if metric in result.metrics:
                metric_values.append(result.metrics[metric])
                
            # Extract parameter combination
            param_combo = {}
            for param_key, param_value in result.parameters.items():
                if param_key not in ["parameter_sweeps", "parameter_combination"]:
                    param_combo[param_key] = param_value
                    
            parameter_values.append(param_combo)
            
        # Perform comparison analysis
        comparison = {
            "metric": metric,
            "mean_value": np.mean(metric_values) if metric_values else None,
            "std_value": np.std(metric_values) if metric_values else None,
            "min_value": np.min(metric_values) if metric_values else None,
            "max_value": np.max(metric_values) if metric_values else None,
            "num_experiments": len(experiment_results),
            "parameter_combinations": parameter_values
        }
        
        return comparison
        
    def save_results(self, filepath: Optional[str] = None):
        """
        Save experiment results.
        
        Args:
            filepath (str, optional): File path
        """
        if not filepath:
            filepath = f"{self.data_dir}/results.json"
            
        results_data = []
        for result in self.results:
            result_dict = result.__dict__.copy()
            # Convert non-serializable objects
            result_dict["parameters"] = json.dumps(result_dict["parameters"])
            results_data.append(result_dict)
            
        with open(filepath, 'w') as f:
            json.dump(results_data, f, indent=2, default=str)
            
    def load_results(self, filepath: str):
        """
        Load experiment results.
        
        Args:
            filepath (str): File path
        """
        with open(filepath, 'r') as f:
            results_data = json.load(f)
            
        self.results.clear()
        for result_dict in results_data:
            # Convert string parameters back to appropriate types
            result_dict["parameters"] = json.loads(result_dict["parameters"])
            result = ExperimentResult(**result_dict)
            self.results.append(result)
            
    def get_results(self) -> List[ExperimentResult]:
        """Get experiment results."""
        return self.results.copy()
        
    def clear_results(self):
        """Clear experiment results."""
        self.results.clear()
        
    def get_result_count(self) -> int:
        """Get number of results."""
        return len(self.results)
        
    def __repr__(self) -> str:
        return f"<Experiment id={self.experiment_id} runs={len(self.results)}>"
