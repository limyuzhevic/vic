"""
Experiment management and automation tools for NLM Python bindings.
"""

from typing import Dict, Any, Optional, List, Callable
from dataclasses import dataclass, field
from enum import Enum
import time
import json
import threading
import queue

class ExperimentStatus(Enum):
    PENDING = "pending"
    RUNNING = "running"
    COMPLETED = "completed"
    FAILED = "failed"
    CANCELLED = "cancelled"

@dataclass
class ExperimentResult:
    """Result of a single experiment run."""
    experiment_name: str
    status: ExperimentStatus
    start_time: float
    end_time: float
    duration: float
    metrics: Dict[str, Any] = field(default_factory=dict)
    error_message: str = ""
    metadata: Dict[str, Any] = field(default_factory=dict)
    
    @property
    def success(self) -> bool:
        return self.status == ExperimentStatus.COMPLETED

class ExperimentRunner:
    """Runner for individual experiments."""
    
    def __init__(self, name: str, config: Dict[str, Any]):
        self.name = name
        self.config = config
        self.status = ExperimentStatus.PENDING
        self.result = None
    
    def run(self, brain_callback: Callable, **kwargs) -> ExperimentResult:
        """Run the experiment."""
        self.status = ExperimentStatus.RUNNING
        start_time = time.time()
        
        try:
            # Prepare brain
            brain = brain_callback(self.config)
            
            # Run experiment
            steps = self.config.get("steps", 1000)
            
            for step in range(steps):
                brain.step(step, step * self.config.get("timestep", 0.001))
                
                # Record metrics at intervals
                if step % 100 == 0:
                    self._record_metrics(brain, step, kwargs)
            
            # Collect final metrics
            metrics = self._collect_metrics(brain, kwargs)
            
            end_time = time.time()
            duration = end_time - start_time
            
            self.status = ExperimentStatus.COMPLETED
            self.result = ExperimentResult(
                experiment_name=self.name,
                status=self.status,
                start_time=start_time,
                end_time=end_time,
                duration=duration,
                metrics=metrics,
                metadata=self.config
            )
            
            return self.result
            
        except Exception as e:
            end_time = time.time()
            duration = end_time - start_time
            
            self.status = ExperimentStatus.FAILED
            self.result = ExperimentResult(
                experiment_name=self.name,
                status=self.status,
                start_time=start_time,
                end_time=end_time,
                duration=duration,
                error_message=str(e),
                metadata=self.config
            )
            
            return self.result
    
    def _record_metrics(self, brain, step: int, kwargs):
        """Record metrics during experiment."""
        pass
    
    def _collect_metrics(self, brain, kwargs) -> Dict[str, Any]:
        """Collect final metrics from brain."""
        return {
            "total_spikes": brain.getTotalSpikeCount(),
            "active_neurons": brain.getActiveNeuronCount(),
            "firing_rate": brain.getAverageFiringRate(),
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "steps_completed": kwargs.get("steps", 1000)
        }

class ExperimentSuite:
    """Suite of related experiments."""
    
    def __init__(self, name: str):
        self.name = name
        self.experiments: Dict[str, ExperimentRunner] = {}
        self.results: Dict[str, ExperimentResult] = {}
    
    def add_experiment(self, name: str, config: Dict[str, Any]):
        """Add an experiment to the suite."""
        self.experiments[name] = ExperimentRunner(name, config)
    
    def run_all(self, brain_callback: Callable, **kwargs) -> Dict[str, ExperimentResult]:
        """Run all experiments in the suite."""
        self.results = {}
        
        for name, experiment in self.experiments.items():
            self.results[name] = experiment.run(brain_callback, **kwargs)
        
        return self.results
    
    def run_sequential(self, brain_callback: Callable, **kwargs) -> Dict[str, ExperimentResult]:
        """Run experiments in sequence, resetting brain between each."""
        self.results = {}
        
        for name, experiment in self.experiments.items():
            self.results[name] = experiment.run(brain_callback, **kwargs)
            # Reset brain for next experiment if it supports it
            if hasattr(experiment.result.metrics.get("brain"), 'reset'):
                experiment.result.metrics.get("brain").reset()
        
        return self.results
    
    def run_parallel(self, brain_callback: Callable, max_workers: int = 4, **kwargs) -> Dict[str, ExperimentResult]:
        """Run experiments in parallel using threads."""
        import concurrent.futures
        
        self.results = {}
        
        with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
            # Submit all experiments
            future_to_name = {
                executor.submit(exp.run, brain_callback, **kwargs): name
                for name, exp in self.experiments.items()
            }
            
            # Collect results as they complete
            for future in concurrent.futures.as_completed(future_to_name):
                name = future_to_name[future]
                try:
                    self.results[name] = future.result()
                except Exception as e:
                    self.results[name] = ExperimentResult(
                        experiment_name=name,
                        status=ExperimentStatus.FAILED,
                        start_time=time.time(),
                        end_time=time.time(),
                        duration=0.0,
                        error_message=str(e),
                        metadata=self.experiments[name].config
                    )
        
        return self.results
    
    def get_summary(self) -> str:
        """Get a summary of all experiment results."""
        if not self.results:
            return "No experiments have been run yet."
        
        summary = f"Experiment Suite: {self.name}\n"
        summary += f"Total experiments: {len(self.experiments)}\n"
        summary += f"Successful: {sum(1 for r in self.results.values() if r.success)}\n"
        summary += f"Failed: {sum(1 for r in self.results.values() if not r.success)}\n\n"
        
        for name, result in self.results.items():
            status_icon = "✓" if result.success else "✗"
            summary += f"{status_icon} {name}: {result.duration:.2f}s"
            if result.error_message:
                summary += f" (Error: {result.error_message})"
            summary += "\n"
        
        return summary

class ParameterSweep:
    """Automated parameter sweep experiments."""
    
    def __init__(self, base_config: Dict[str, Any]):
        self.base_config = base_config
        self.results = []
        self.param_combinations = []
    
    def add_parameter_range(self, param_name: str, values: List[Any]):
        """Add a parameter to sweep over multiple values."""
        if not self.param_combinations:
            # Initialize with one combination
            self.param_combinations = [{}]
        
        new_combinations = []
        for combination in self.param_combinations:
            for value in values:
                new_combination = combination.copy()
                new_combination[param_name] = value
                new_combinations.append(new_combination)
        
        self.param_combinations = new_combinations
    
    def run_sweep(self, brain_callback: Callable, **kwargs) -> List[Dict[str, Any]]:
        """Run parameter sweep."""
        self.results = []
        
        for i, param_combination in enumerate(self.param_combinations):
            print(f"Running parameter combination {i+1}/{len(self.param_combinations)}: {param_combination}")
            
            # Merge base config with parameter combination
            config = self.base_config.copy()
            config.update(param_combination)
            
            # Create experiment runner
            experiment = ExperimentRunner(
                f"sweep_{i+1}",
                config
            )
            
            # Run experiment
            result = experiment.run(brain_callback, **kwargs)
            self.results.append(result)
        
        return self.results
    
    def get_best_config(self) -> Optional[Dict[str, Any]]:
        """Get the configuration that produced the best results."""
        if not self.results:
            return None
        
        # Sort by some metric (e.g., firing rate or total spikes)
        self.results.sort(key=lambda r: r.metrics.get("total_spikes", 0), reverse=True)
        
        # Get the config from the best result
        best_experiment = self.results[0]
        return best_experiment.metrics.get("config", best_experiment.metadata)