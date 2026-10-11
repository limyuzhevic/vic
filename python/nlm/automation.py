"""Automation and scripting for NLM Python API."""

import numpy as np
from typing import Dict, List, Optional, Any, Tuple
from collections import deque
import time
from pathlib import Path
import json
import threading
import queue
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor

from .brain import BrainNetwork
from .environment import WorldSimulator
from .config import nlm_config


class ExperimentRunner:
    """
    Run automated experiments for NLM systems.
    
    This class provides functionality to run automated experiments with various
    configurations, parameters, and analysis capabilities.
    """
    
    def __init__(self, n_workers: int = 1, parallelization: str = "sequential"):
        """
        Initialize experiment runner.
        
        Args:
            n_workers: Number of parallel workers (for parallel execution)
            parallelization: Type of parallelization ("sequential", "thread", "process")
        """
        self.n_workers = n_workers
        self.parallelization = parallelization
        
        # Experiment management
        self.experiments = {}
        self.current_experiment = None
        self.experiment_queue = queue.Queue()
        self.results_queue = queue.Queue()
        
        # Execution state
        self.executor = None
        self.running = False
        self.experiment_history = deque(maxlen=100)
        
        # Initialize executor based on parallelization type
        if parallelization == "thread":
            self.executor = ThreadPoolExecutor(max_workers=n_workers)
        elif parallelization == "process":
            self.executor = ProcessPoolExecutor(max_workers=n_workers)
        
    def create_experiment(self, name: str, description: str,
                         parameters: Dict[str, Any],
                         environment_config: Dict[str, Any] = None) -> Dict[str, Any]:
        """
        Create a new experiment.
        
        Args:
            name: Experiment name
            description: Experiment description
            parameters: Experiment parameters
            environment_config: Environment configuration
            
        Returns:
            Dict[str, Any]: Created experiment
        """
        experiment = {
            "name": name,
            "description": description,
            "parameters": parameters,
            "environment_config": environment_config or {},
            "created_time": time.time(),
            "version": "1.0",
            "status": "created",
            "metadata": {}
        }
        
        self.experiments[name] = experiment
        return experiment
        
    def get_experiment(self, name: str) -> Optional[Dict[str, Any]]:
        """
        Get experiment by name.
        
        Args:
            name: Experiment name
            
        Returns:
            Optional[Dict[str, Any]]: Experiment or None if not found
        """
        return self.experiments.get(name)
        
    def list_experiments(self) -> List[str]:
        """List all experiment names."""
        return list(self.experiments.keys())
        
    def update_experiment(self, name: str, **kwargs) -> bool:
        """
        Update experiment parameters.
        
        Args:
            name: Experiment name
            **kwargs: Parameters to update
            
        Returns:
            bool: True if experiment was updated
        """
        if name in self.experiments:
            self.experiments[name]["parameters"].update(kwargs)
            self.experiments[name]["last_updated"] = time.time()
            return True
            
        return False
        
    def delete_experiment(self, name: str) -> bool:
        """
        Delete experiment.
        
        Args:
            name: Experiment name
            
        Returns:
            bool: True if experiment was deleted
        """
        if name in self.experiments:
            del self.experiments[name]
            return True
            
        return False
        
    def run_experiment(self, name: str, iterations: int = 1,
                      parallel: bool = False) -> Dict[str, Any]:
        """
        Run an experiment.
        
        Args:
            name: Experiment name
            iterations: Number of iterations to run
            parallel: Whether to run iterations in parallel
            
        Returns:
            Dict[str, Any]: Experiment results
        """
        if name not in self.experiments:
            raise KeyError(f"Experiment not found: {name}")
            
        experiment = self.experiments[name]
        experiment["status"] = "running"
        experiment["start_time"] = time.time()
        
        # Prepare experiment configuration
        config = experiment["parameters"]
        env_config = experiment.get("environment_config", {})
        
        # Create environment
        world = WorldSimulator(**env_config)
        
        # Run experiment
        experiment_results = {
            "name": name,
            "description": experiment["description"],
            "parameters": config,
            "iterations": iterations,
            "parallel": parallel,
            "start_time": experiment["start_time"],
            "results": [],
            "metrics": {},
            "status": "running"
        }
        
        # Run iterations
        if parallel and iterations > 1:
            experiment_results = self._run_parallel_experiment(
                experiment_results, world, iterations
            )
        else:
            experiment_results = self._run_sequential_experiment(
                experiment_results, world, iterations
            )
        
        # Update experiment
        experiment["status"] = "completed"
        experiment["end_time"] = time.time()
        experiment["results"] = experiment_results["results"]
        
        # Store in history
        self.experiment_history.append(experiment_results)
        
        return experiment_results
        
    def _run_sequential_experiment(self, experiment_results: Dict[str, Any],
                                  world: WorldSimulator, iterations: int) -> Dict[str, Any]:
        """
        Run experiment sequentially.
        
        Args:
            experiment_results: Experiment results template
            world: World environment
            iterations: Number of iterations
            
        Returns:
            Dict[str, Any]: Updated experiment results
        """
        for iteration in range(iterations):
            # Create brain with current configuration
            config = experiment_results["parameters"]
            brain = BrainNetwork(config)
            brain.initialize()
            
            # Run iteration
            iteration_result = self._run_single_iteration(
                brain, world, iteration, experiment_results["parameters"]
            )
            
            experiment_results["results"].append(iteration_result)
            
            # Update metrics
            self._update_experiment_metrics(experiment_results["metrics"], iteration_result)
            
        experiment_results["status"] = "completed"
        experiment_results["end_time"] = time.time()
        
        return experiment_results
        
    def _run_parallel_experiment(self, experiment_results: Dict[str, Any],
                                world: WorldSimulator, iterations: int) -> Dict[str, Any]:
        """
        Run experiment in parallel.
        
        Args:
            experiment_results: Experiment results template
            world: World environment
            iterations: Number of iterations
            
        Returns:
            Dict[str, Any]: Updated experiment results
        """
        # Submit tasks to executor
        futures = []
        for iteration in range(iterations):
            future = self.executor.submit(
                self._run_single_iteration,
                BrainNetwork(experiment_results["parameters"]),
                world,
                iteration,
                experiment_results["parameters"]
            )
            futures.append(future)
            
        # Collect results
        for i, future in enumerate(futures):
            iteration_result = future.result()
            experiment_results["results"].append(iteration_result)
            self._update_experiment_metrics(experiment_results["metrics"], iteration_result)
            
        experiment_results["status"] = "completed"
        experiment_results["end_time"] = time.time()
        
        return experiment_results
        
    def _run_single_iteration(self, brain: BrainNetwork, world: WorldSimulator,
                            iteration: int, config: Dict[str, Any]) -> Dict[str, Any]:
        """
        Run a single experiment iteration.
        
        Args:
            brain: Brain to simulate
            world: World environment
            iteration: Iteration number
            config: Experiment configuration
            
        Returns:
            Dict[str, Any]: Iteration results
        """
        # Reset environment
        world.reset()
        brain.reset()
        
        # Create agent percept
        percept = world.get_sensory_percept()
        
        # Run simulation
        steps = config.get("steps", 100)
        dt = config.get("dt", 0.1)
        
        iteration_result = {
            "iteration": iteration,
            "start_time": time.time(),
            "brain_config": config,
            "world_config": {
                "width": world.width,
                "height": world.height,
                "gravity": world.gravity.tolist() if hasattr(world.gravity, "tolist") else list(world.gravity)
            },
            "simulation_results": [],
            "metrics": {},
            "status": "running"
        }
        
        # Run simulation steps
        for step in range(steps):
            # Update world
            world_result = world.update(dt)
            
            # Process percept with brain
            brain_result = brain.step(dt)
            
            # Update percept with world state
            percept.update(world_result)
            
            # Record simulation results
            iteration_result["simulation_results"].append({
                "step": step,
                "world_state": world_result,
                "brain_state": brain_result,
                "percept": percept
            })
            
        # Calculate final metrics
        iteration_result["end_time"] = time.time()
        iteration_result["duration"] = iteration_result["end_time"] - iteration_result["start_time"]
        iteration_result["metrics"] = self._calculate_iteration_metrics(iteration_result)
        iteration_result["status"] = "completed"
        
        return iteration_result
        
    def _update_experiment_metrics(self, metrics: Dict[str, Any], iteration_result: Dict[str, Any]) -> None:
        """
        Update experiment metrics with iteration results.
        
        Args:
            metrics: Experiment metrics dictionary
            iteration_result: Iteration result
        """
        # Update each metric
        for metric_name, metric_values in metrics.items():
            if metric_name in iteration_result["metrics"]:
                metric_values.append(iteration_result["metrics"][metric_name])
                
    def _calculate_iteration_metrics(self, iteration_result: Dict[str, Any]) -> Dict[str, Any]:
        """
        Calculate metrics for an iteration.
        
        Args:
            iteration_result: Iteration result
            
        Returns:
            Dict[str, Any]: Calculated metrics
        """
        metrics = {}
        
        # Extract simulation results
        simulation_results = iteration_result["simulation_results"]
        
        if not simulation_results:
            return metrics
            
        # Calculate total reward
        total_reward = 0.0
        for step_result in simulation_results:
            total_reward += step_result.get("world_state", {}).get("reward", 0.0)
            
        metrics["total_reward"] = total_reward
        metrics["average_reward_per_step"] = total_reward / len(simulation_results)
        metrics["steps_completed"] = len(simulation_results)
        metrics["duration"] = iteration_result["duration"]
        metrics["reward_efficiency"] = total_reward / (iteration_result["duration"] + 1e-8)
        
        # Calculate activity metrics
        brain_activities = []
        for step_result in simulation_results:
            brain_activities.append(step_result.get("brain_state", {}).get("activity", 0.0))
            
        if brain_activities:
            metrics["average_brain_activity"] = np.mean(brain_activities)
            metrics["max_brain_activity"] = np.max(brain_activities)
            metrics["brain_activity_variance"] = np.var(brain_activities)
            
        return metrics
        
    def run_batch_experiments(self, experiment_names: List[str],
                            iterations_per_experiment: int = 10) -> Dict[str, Any]:
        """
        Run a batch of experiments.
        
        Args:
            experiment_names: Names of experiments to run
            iterations_per_experiment: Number of iterations per experiment
            
        Returns:
            Dict[str, Any]: Batch results
        """
        batch_results = {
            "experiments": [],
            "start_time": time.time(),
            "status": "running",
            "parallel_execution": True
        }
        
        # Run experiments
        for experiment_name in experiment_names:
            experiment_result = self.run_experiment(
                experiment_name, iterations_per_experiment
            )
            batch_results["experiments"].append(experiment_result)
            
        batch_results["status"] = "completed"
        batch_results["end_time"] = time.time()
        
        return batch_results
        
    def get_experiment_statistics(self) -> Dict[str, Any]:
        """Get experiment statistics."""
        return {
            "total_experiments": len(self.experiments),
            "experiment_history": list(self.experiment_history)[-5:],
            "current_status": {name: exp["status"] for name, exp in self.experiments.items()},
            "parallelization": self.parallelization,
            "n_workers": self.n_workers
        }
        
    def stop_experiment(self, name: str) -> bool:
        """
        Stop a running experiment.
        
        Args:
            name: Experiment name
            
        Returns:
            bool: True if experiment was stopped
        """
        if name in self.experiments and self.experiments[name]["status"] == "running":
            self.experiments[name]["status"] = "stopped"
            self.experiments[name]["end_time"] = time.time()
            return True
            
        return False
        
    def cleanup(self) -> None:
        """Cleanup experiment runner."""
        if self.executor:
            self.executor.shutdown(wait=False)
            
        self.running = False
        self.experiment_queue.queue.clear()
        self.results_queue.queue.clear()
        
    def reset(self) -> None:
        """Reset experiment runner."""
        self.experiments.clear()
        self.current_experiment = None
        self.experiment_queue.queue.clear()
        self.results_queue.queue.clear()
        
        if self.executor:
            self.executor.shutdown(wait=True)
            
        self.running = False
        self.experiment_history.clear()


class BatchProcessor:
    """
    Process multiple experiment runs efficiently.
    
    This class provides batch processing capabilities for multiple experiment runs,
    including parallelization, resource management, and result aggregation.
    """
    
    def __init__(self, batch_size: int = 10, n_workers: int = 4,
                 parallelization: str = "thread"):
        """
        Initialize batch processor.
        
        Args:
            batch_size: Size of each batch
            n_workers: Number of parallel workers
            parallelization: Type of parallelization
        """
        self.batch_size = batch_size
        self.n_workers = n_workers
        self.parallelization = parallelization
        
        # Batch management
        self.batches = {}
        self.current_batch = None
        self.processing_queue = queue.Queue()
        self.completed_batches = deque(maxlen=100)
        
        # Resource management
        self.resource_usage = {
            "cpu_usage": 0.0,
            "memory_usage": 0.0,
            "active_workers": 0
        }
        
        # Initialize executor
        if parallelization == "thread":
            self.executor = ThreadPoolExecutor(max_workers=n_workers)
        elif parallelization == "process":
            self.executor = ProcessPoolExecutor(max_workers=n_workers)
            
    def create_batch(self, batch_id: str, experiment_names: List[str],
                    iterations_per_experiment: int = 10,
                    parameter_combinations: List[Dict[str, Any]] = None) -> Dict[str, Any]:
        """
        Create a new batch.
        
        Args:
            batch_id: Batch identifier
            experiment_names: Names of experiments to run
            iterations_per_experiment: Number of iterations per experiment
            parameter_combinations: Parameter combinations for experiments
            
        Returns:
            Dict[str, Any]: Created batch
        """
        if parameter_combinations is None:
            # Create default parameter combinations
            parameter_combinations = [{}]
            
        batch = {
            "batch_id": batch_id,
            "experiment_names": experiment_names,
            "iterations_per_experiment": iterations_per_experiment,
            "parameter_combinations": parameter_combinations,
            "created_time": time.time(),
            "version": "1.0",
            "status": "pending",
            "total_experiments": len(experiment_names) * len(parameter_combinations),
            "completed_experiments": 0,
            "failed_experiments": 0,
            "metadata": {}
        }
        
        self.batches[batch_id] = batch
        return batch
        
    def get_batch(self, batch_id: str) -> Optional[Dict[str, Any]]:
        """
        Get batch by ID.
        
        Args:
            batch_id: Batch identifier
            
        Returns:
            Optional[Dict[str, Any]]: Batch or None if not found
        """
        return self.batches.get(batch_id)
        
    def list_batches(self) -> List[str]:
        """List all batch IDs."""
        return list(self.batches.keys())
        
    def start_batch(self, batch_id: str) -> bool:
        """
        Start processing a batch.
        
        Args:
            batch_id: Batch identifier
            
        Returns:
            bool: True if batch was started
        """
        if batch_id not in self.batches:
            return False
            
        batch = self.batches[batch_id]
        batch["status"] = "running"
        batch["start_time"] = time.time()
        self.current_batch = batch_id
        
        # Submit batch tasks
        self._submit_batch_tasks(batch)
        
        return True
        
    def _submit_batch_tasks(self, batch: Dict[str, Any]) -> None:
        """
        Submit tasks for a batch.
        
        Args:
            batch: Batch to process
        """
        # Create experiment configurations
        for experiment_name in batch["experiment_names"]:
            for param_combination in batch["parameter_combinations"]:
                task = {
                    "batch_id": batch["batch_id"],
                    "experiment_name": experiment_name,
                    "parameters": {**param_combination},
                    "iteration_offset": batch["completed_experiments"]
                }
                
                self.processing_queue.put(task)
                
    def process_next_task(self) -> Optional[Dict[str, Any]]:
        """
        Process the next task in the queue.
        
        Returns:
            Optional[Dict[str, Any]]: Processed task result
        """
        if self.processing_queue.empty():
            return None
            
        task = self.processing_queue.get()
        batch_id = task["batch_id"]
        
        if batch_id not in self.batches:
            return None
            
        batch = self.batches[batch_id]
        
        # Create experiment runner
        from . import ExperimentRunner
        experiment_runner = ExperimentRunner()
        
        try:
            # Run experiment
            experiment_result = experiment_runner.run_experiment(
                task["experiment_name"],
                batch["iterations_per_experiment"]
            )
            
            # Update batch
            batch["completed_experiments"] += 1
            batch["last_completed_time"] = time.time()
            
            # Add to completed batches
            self.completed_batches.append({
                "batch_id": batch_id,
                "experiment_name": task["experiment_name"],
                "parameters": task["parameters"],
                "result": experiment_result,
                "completed_time": time.time()
            })
            
            return experiment_result
            
        except Exception as e:
            # Update batch
            batch["failed_experiments"] += 1
            batch["last_failed_time"] = time.time()
            
            return {"error": str(e), "batch_id": batch_id}
            
    def wait_for_batch_completion(self, batch_id: str, timeout: float = None) -> Dict[str, Any]:
        """
        Wait for batch completion.
        
        Args:
            batch_id: Batch identifier
            timeout: Timeout in seconds (None for no timeout)
            
        Returns:
            Dict[str, Any]: Batch result
        """
        if batch_id not in self.batches:
            return {"error": "Batch not found"}
            
        batch = self.batches[batch_id]
        
        if batch["status"] == "completed":
            return batch
            
        start_time = time.time()
        
        while batch["status"] == "running":
            if timeout is not None and (time.time() - start_time) > timeout:
                batch["status"] = "timeout"
                batch["end_time"] = time.time()
                break
                
            # Process next task
            result = self.process_next_task()
            
            # Update batch status
            if result and "error" not in result:
                batch["completed_experiments"] += 1
            else:
                batch["failed_experiments"] += 1
                
            # Check if batch is complete
            if batch["completed_experiments"] + batch["failed_experiments"] >= batch["total_experiments"]:
                batch["status"] = "completed"
                batch["end_time"] = time.time()
                break
                
        return batch
        
    def get_batch_statistics(self) -> Dict[str, Any]:
        """Get batch processing statistics."""
        total_tasks = sum(batch["total_experiments"] for batch in self.batches.values())
        completed_tasks = sum(batch["completed_experiments"] for batch in self.batches.values())
        failed_tasks = sum(batch["failed_experiments"] for batch in self.batches.values())
        
        return {
            "total_batches": len(self.batches),
            "total_tasks": total_tasks,
            "completed_tasks": completed_tasks,
            "failed_tasks": failed_tasks,
            "success_rate": completed_tasks / max(total_tasks, 1),
            "active_batches": len([b for b in self.batches.values() if b["status"] == "running"]),
            "pending_batches": len([b for b in self.batches.values() if b["status"] == "pending"]),
            "completed_batches": len([b for b in self.batches.values() if b["status"] == "completed"]),
            "parallelization": self.parallelization,
            "n_workers": self.n_workers,
            "batch_size": self.batch_size
        }
        
    def stop_batch(self, batch_id: str) -> bool:
        """
        Stop a running batch.
        
        Args:
            batch_id: Batch identifier
            
        Returns:
            bool: True if batch was stopped
        """
        if batch_id not in self.batches:
            return False
            
        batch = self.batches[batch_id]
        
        if batch["status"] == "running":
            batch["status"] = "stopped"
            batch["end_time"] = time.time()
            return True
            
        return False
        
    def cleanup(self) -> None:
        """Cleanup batch processor."""
        if self.executor:
            self.executor.shutdown(wait=False)
            
        self.processing_queue.queue.clear()
        self.completed_batches.clear()
        
    def reset(self) -> None:
        """Reset batch processor."""
        self.batches.clear()
        self.current_batch = None
        self.processing_queue.queue.clear()
        self.completed_batches.clear()
        
        if self.executor:
            self.executor.shutdown(wait=True)
            
        self.resource_usage = {"cpu_usage": 0.0, "memory_usage": 0.0, "active_workers": 0}


class ResultAggregator:
    """
    Aggregate experiment results.
    
    This class provides comprehensive result aggregation capabilities for
    experiment runs, including statistical analysis, comparison, and visualization.
    """
    
    def __init__(self, aggregation_methods: List[str] = None):
        """
        Initialize result aggregator.
        
        Args:
            aggregation_methods: Methods to use for aggregation
        """
        if aggregation_methods is None:
            aggregation_methods = ["mean", "std", "min", "max", "median", "percentiles"]
            
        self.aggregation_methods = aggregation_methods
        self.result_history = deque(maxlen=100)
        
    def aggregate_results(self, results: List[Dict[str, Any]],
                        experiment_names: List[str] = None) -> Dict[str, Any]:
        """
        Aggregate experiment results.
        
        Args:
            results: List of experiment results
            experiment_names: Names of experiments to include (if None, include all)
            
        Returns:
            Dict[str, Any]: Aggregated results
        """
        if not results:
            return {"error": "No results to aggregate"}
            
        # Filter by experiment names if specified
        if experiment_names:
            filtered_results = []
            for result in results:
                if result.get("name") in experiment_names:
                    filtered_results.append(result)
            results = filtered_results
            
        # Aggregate by experiment
        aggregated = {}
        
        for result in results:
            exp_name = result.get("name")
            if exp_name not in aggregated:
                aggregated[exp_name] = []
                
            aggregated[exp_name].append(result)
            
        # Calculate aggregates for each experiment
        final_aggregation = {}
        
        for exp_name, exp_results in aggregated.items():
            final_aggregation[exp_name] = self._aggregate_experiment_results(exp_results)
            
        # Calculate overall aggregates
        final_aggregation["overall"] = self._aggregate_all_results(results)
        
        # Store in history
        self.result_history.append({
            "timestamp": time.time(),
            "total_results": len(results),
            "experiments_count": len(aggregated),
            "aggregation_methods": self.aggregation_methods,
            "result": final_aggregation
        })
        
        return final_aggregation
        
    def _aggregate_experiment_results(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:n:
        """
        Aggregate results for a single experiment.
        
        Args:
            results: List of experiment results
            
        Returns:
            Dict[str, Any]: Aggregated results
        """
        if not results:
            return {"error": "No results to aggregate"}
            
        # Extract data from results
        iteration_metrics = []
        all_metrics = {}
        
        for result in results:
            for metric_name, metric_value in result.get("metrics", {}).items():
                if metric_name not in all_metrics:
                    all_metrics[metric_name] = []
                all_metrics[metric_name].append(metric_value)
                
        # Calculate aggregates for each metric
        aggregated_metrics = {}
        
        for metric_name, values in all_metrics.items():
            aggregated_metrics[metric_name] = self._aggregate_values(metric_name, values)
            
        # Calculate overall experiment statistics
        total_iterations = sum(result.get("iterations", 0) for result in results)
        
        return {
            "experiment_name": results[0].get("name"),
            "experiment_description": results[0].get("description"),
            "total_iterations": total_iterations,
            "completed_iterations": len(results),
            "success_rate": len(results) / max(total_iterations, 1),
            "aggregated_metrics": aggregated_metrics,
            "parameter_combinations": list(set(
                result.get("parameters", {}) for result in results
            ))
        }
        
    def _aggregate_all_results(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:n:
        """
        Aggregate results from all experiments.
        
        Args:
            results: List of all experiment results
            
        Returns:
            Dict[str, Any]: Overall aggregated results
        """
        if not results:
            return {"error": "No results to aggregate"}
            
        # Extract metrics from all results
        all_metrics = {}
        
        for result in results:
            for metric_name, metric_value in result.get("metrics", {}).items():
                if metric_name not in all_metrics:
                    all_metrics[metric_name] = []
                all_metrics[metric_name].append(metric_value)
                
        # Calculate overall aggregates
        overall_metrics = {}
        
        for metric_name, values in all_metrics.items():
            overall_metrics[metric_name] = self._aggregate_values(metric_name, values)
            
        # Calculate performance rankings
        performance_ranking = self._calculate_performance_ranking(results)
        
        return {
            "total_experiments": len(set(result.get("name") for result in results)),
            "total_iterations": sum(result.get("iterations", 0) for result in results),
            "overall_metrics": overall_metrics,
            "performance_ranking": performance_ranking,
            "best_experiment": self._find_best_experiment(results)
        }
        
    def _aggregate_values(self, metric_name: str, values: List[Any]) -> Dict[str, Any]:n:
        """
        Aggregate values for a specific metric.
        
        Args:
            metric_name: Name of the metric
            values: List of values
            
        Returns:
            Dict[str, Any]: Aggregated metric values
        """
        if not values:
            return {"error": "No values to aggregate"}
            
        # Filter to numeric values if possible
        numeric_values = []
        for value in values:
            try:
                if isinstance(value, (int, float)):
                    numeric_values.append(float(value))
            except (ValueError, TypeError):
                pass
                
        if not numeric_values:
            return {"error": "No numeric values to aggregate"}
            
        aggregation_result = {}
        
        # Calculate aggregates
        if "mean" in self.aggregation_methods:
            aggregation_result["mean"] = np.mean(numeric_values)
            
        if "median" in self.aggregation_methods:
            aggregation_result["median"] = np.median(numeric_values)
            
        if "std" in self.aggregation_methods:
            aggregation_result["std"] = np.std(numeric_values)
            
        if "min" in self.aggregation_methods:
            aggregation_result["min"] = np.min(numeric_values)
            
        if "max" in self.aggregation_methods:
            aggregation_result["max"] = np.max(numeric_values)
            
        if "percentiles" in self.aggregation_methods:
            aggregation_result["percentiles"] = {
                "25": np.percentile(numeric_values, 25),
                "50": np.percentile(numeric_values, 50),
                "75": np.percentile(numeric_values, 75),
                "95": np.percentile(numeric_values, 95),
                "99": np.percentile(numeric_values, 99)
            }
            
        # Calculate additional statistics
        aggregation_result["count"] = len(numeric_values)
        aggregation_result["range"] = np.max(numeric_values) - np.min(numeric_values)
        
        return aggregation_result
        
    def _calculate_performance_ranking(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:n:
        """
        Calculate performance ranking across experiments.
        
        Args:
            results: List of experiment results
            
        Returns:
            Dict[str, Any]: Performance ranking
        """
        # Calculate performance score for each experiment
        experiment_scores = {}
        
        for result in results:
            exp_name = result.get("name")
            if exp_name not in experiment_scores:
                experiment_scores[exp_name] = 0.0
                
            # Calculate score based on metrics
            metrics = result.get("metrics", {})
            score = 0.0
            
            if "total_reward" in metrics:
                score += metrics["total_reward"] * 0.4
                
            if "average_reward_per_step" in metrics:
                score += metrics["average_reward_per_step"] * 0.3
                
            if "reward_efficiency" in metrics:
                score += metrics["reward_efficiency"] * 0.3
                
            experiment_scores[exp_name] += score
            
        # Rank experiments
        sorted_experiments = sorted(experiment_scores.items(), key=lambda x: x[1], reverse=True)
        
        ranking = {}
        for i, (exp_name, score) in enumerate(sorted_experiments):
            ranking[exp_name] = {
                "rank": i + 1,
                "score": score,
                "performance_category": self._categorize_performance(score, len(sorted_experiments))
            }
            
        return ranking
        
    def _categorize_performance(self, score: float, total_experiments: int) -> str:
        """
        Categorize performance based on score.
        
        Args:
            score: Performance score
            total_experiments: Total number of experiments
            
        Returns:
            str: Performance category
        """
        if total_experiments == 0:
            return "unknown"
            
        percentile = (total_experiments - 1 - sorted(enumerate([score]*total_experiments), key=lambda x: x[1], reverse=True).index((0, score)))) / total_experiments
        
        if percentile >= 0.8:
            return "excellent"
        elif percentile >= 0.6:
            return "good"
        elif percentile >= 0.4:
            return "average"
        elif percentile >= 0.2:
            return "below_average"
        else:
            return "poor"
            
    def _find_best_experiment(self, results: List[Dict[str, Any]]) -> Dict[str, Any]:n:
        """
        Find the best experiment.
        
        Args:
            results: List of experiment results
            
        Returns:
            Dict[str, Any]: Best experiment information
        """
        if not results:
            return {"error": "No experiments to evaluate"}
            
        # Calculate performance scores
        experiment_scores = {}
        
        for result in results:
            exp_name = result.get("name")
            if exp_name not in experiment_scores:
                experiment_scores[exp_name] = 0.0
                
            # Calculate score based on metrics
            metrics = result.get("metrics", {})
            score = 0.0
            
            if "total_reward" in metrics:
                score += metrics["total_reward"] * 0.4
                
            if "average_reward_per_step" in metrics:
                score += metrics["average_reward_per_step"] * 0.3
                
            if "reward_efficiency" in metrics:
                score += metrics["reward_efficiency"] * 0.3
                
            experiment_scores[exp_name] += score
            
        # Find best experiment
        if experiment_scores:
            best_exp_name = max(experiment_scores.items(), key=lambda x: x[1])[0]
            
            best_result = next((r for r in results if r.get("name") == best_exp_name), None)
            
            if best_result:
                return {
                    "name": best_exp_name,
                    "score": experiment_scores[best_exp_name],
                    "parameters": best_result.get("parameters"),
                    "metrics": best_result.get("metrics"),
                    "iterations": best_result.get("iterations"),
                    "success_rate": best_result.get("success_rate", 0.0)
                }
                
        return {"error": "No valid experiment found"}
        
    def get_aggregation_statistics(self) -> Dict[str, Any]:
        """Get result aggregation statistics."""
        return {
            "aggregation_methods": self.aggregation_methods,
            "total_aggregations": len(self.result_history),
            "recent_aggregations": list(self.result_history)[-5:]
        }
        
    def reset(self) -> None:
        """Reset result aggregator."""
        self.result_history.clear()


class ReportGenerator:
    """
    Generate comprehensive reports.
    
    This class provides functionality to generate comprehensive reports from
    experiment results, including statistical summaries, visualizations, and
    analysis insights.
    """
    
    def __init__(self, report_format: str = "text", output_directory: str = "reports"):
        """
        Initialize report generator.
        
        Args:
            report_format: Report format ("text", "json", "markdown")
            output_directory: Directory for output files
        """
        self.report_format = report_format
        self.output_directory = Path(output_directory)
        self.output_directory.mkdir(parents=True, exist_ok=True)
        
        # Report configuration
        self.report_templates = {}
        self.report_history = deque(maxlen=100)
        
    def create_report(self, report_id: str, title: str, description: str,
                     data: Dict[str, Any], template: str = None) -> Dict[str, Any]:
        """
        Create a new report.
        
        Args:
            report_id: Report identifier
            title: Report title
            description: Report description
            data: Data for report generation
            template: Report template name
            
        Returns:
            Dict[str, Any]: Created report
        """
        if template is None:
            template = self.report_format
            
        report = {
            "report_id": report_id,
            "title": title,
            "description": description,
            "data": data,
            "template": template,
            "format": self.report_format,
            "created_time": time.time(),
            "version": "1.0",
            "metadata": {}
        }
        
        self.report_templates[report_id] = report
        return report
        
    def get_report(self, report_id: str) -> Optional[Dict[str, Any]]:
        """
        Get report by ID.
n
        Args:
            report_id: Report identifier
            
        Returns:
            Optional[Dict[str, Any]]: Report or None if not found
        """
        return self.report_templates.get(report_id)
        
    def list_reports(self) -> List[str]:
        """List all report IDs."""
        return list(self.report_templates.keys())
        
    def generate_report(self, report_id: str, output_path: str = None) -> Dict[str, Any]:n:
        """
        Generate report.
n
        Args:
            report_id: Report identifier
            output_path: Path to save report (if None, auto-generated)
            
        Returns:
            Dict[str, Any]: Generated report
        """
        if report_id not in self.report_templates:
            return {"error": "Report not found"}
            
        report = self.report_templates[report_id]
        
        # Generate report content
        if self.report_format == "text":
            report_content = self._generate_text_report(report)
        elif self.report_format == "json":
            report_content = self._generate_json_report(report)
        elif self.report_format == "markdown":
            report_content = self._generate_markdown_report(report)
        else:
            return {"error": f"Unsupported report format: {self.report_format}"}
            
        # Save report
        if output_path is None:
            output_path = self.output_directory / f"{report_id}.{self.report_format}"
            
        output_path = Path(output_path)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        with open(output_path, 'w') as f:
            if self.report_format == "json":
                json.dump(report_content, f, indent=2)
            else:
                f.write(report_content)
                
        # Update report
        report["generated_time"] = time.time()
        report["output_path"] = str(output_path)
        
        # Store in history
        self.report_history.append({
            "report_id": report_id,
            "title": report["title"],
            "format": self.report_format,
            "generated_time": report["generated_time"],
            "file_size": output_path.stat().st_size if output_path.exists() else 0
        })
        
        return {
            "report_id": report_id,
            "title": report["title"],
            "format": self.report_format,
            "output_path": str(output_path),
            "generated_time": report["generated_time"],
            "file_size": output_path.stat().st_size if output_path.exists() else 0
        }
        
    def _generate_text_report(self, report: Dict[str, Any]) -> str:
        """
        Generate text report.
n
        Args:
            report: Report data
            
        Returns:
            str: Text report content
        """
        report_content = f"Report: {report['title']}\n"
        report_content += f"Description: {report['description']}\n"
        report_content += f"Generated: {time.ctime(report['created_time'])}\n"
        report_content += "\n" + "="*50 + "\n"
        
        # Add data summary
        data = report["data"]
        report_content += f"Data Summary:\n"
        
        for key, value in data.items():
            if isinstance(value, dict):
                report_content += f"  {key}:\n"
                for sub_key, sub_value in value.items():
                    if isinstance(sub_value, list):
                        report_content += f"    {sub_key}: {len(sub_value)} items\n"
                    else:
                        report_content += f"    {sub_key}: {sub_value}\n"
            elif isinstance(value, list):
                report_content += f"  {key}: {len(value)} items\n"
            else:
                report_content += f"  {key}: {value}\n"
                
        return report_content
        
    def _generate_json_report(self, report: Dict[str, Any]) -> Dict[str, Any]:n:
        """
        Generate JSON report.
n
        Args:
            report: Report data
            
        Returns:
            Dict[str, Any]: JSON report
        """
        return {
            "report_id": report["report_id"],
            "title": report["title"],
            "description": report["description"],
            "created_time": report["created_time"],
            "data": report["data"],
            "template": report["template"]
        }
        
    def _generate_markdown_report(self, report: Dict[str, Any]) -> str:n:
        """
        Generate markdown report.
n
        Args:
            report: Report data
            
        Returns:
            str: Markdown report content
        """
        report_content = f"# {report['title']}\n\n"
        report_content += f"{report['description']}\n\n"
        report_content += f"*Generated: {time.ctime(report['created_time'])}*\n\n"
        report_content += "---\n\n"
        
        # Add data sections
        data = report["data"]
        
        for key, value in data.items():
            report_content += f"## {key}\n\n"
            
            if isinstance(value, dict):
                report_content += "| Metric | Value |\n"
                report_content += "|--------|-------|\n"
                
                for sub_key, sub_value in value.items():
                    if isinstance(sub_value, (int, float)):
                        report_content += f"| {sub_key} | {sub_value:.4f} |\n"
                    else:
                        report_content += f"| {sub_key} | {sub_value} |\n"
                        
            elif isinstance(value, list):
                report_content += f"- Total items: {len(value)}\n"
                if len(value) > 0 and isinstance(value[0], dict):
                    report_content += "\n| " + " | ".join(value[0].keys()) + " |\n"
                    report_content += "| " + " | ".join(["---" for _ in value[0].keys()]) + " |\n"
                    
                    for item in value[:10]:  # Limit to 10 items
                        report_content += "| " + " | ".join(
                            [str(item.get(key, "")) for key in item.keys()]
                        ) + " |\n"
                        
                    if len(value) > 10:
                        report_content += f"... and {len(value) - 10} more items\n"
                        
            else:
                report_content += f"{value}\n"
                
            report_content += "\n"
                
        return report_content
        
    def get_report_statistics(self) -> Dict[str, Any]:
        """Get report generation statistics."""
        return {
            "total_reports": len(self.report_templates),
            "report_history": list(self.report_history)[-5:],
            "format": self.report_format,
            "output_directory": str(self.output_directory)
        }
        
    def cleanup(self) -> None:
        """Cleanup report generator."""
        # Clean up old reports
        for report_id in list(self.report_templates.keys()):
            report = self.report_templates[report_id]
            if "output_path" in report:
                output_path = Path(report["output_path"])
                if output_path.exists():
                    output_path.unlink()
                    
        self.report_templates.clear()
        self.report_history.clear()
        
    def reset(self) -> None:
        """Reset report generator."""
        self.report_templates.clear()
        self.report_history.clear()


# Re-export commonly used classes and functions
__all__ = [
    "ExperimentRunner", "BatchProcessor", "ResultAggregator", "ReportGenerator"
]