#!/usr/bin/env python3
"""
NLM Experiment Manager - Experiment Management

This module provides comprehensive experiment management with tracking,
comparison, and analysis capabilities.
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

from typing import Dict, List, Optional, Any, Union, Callable
import json
import os
import time
from pathlib import Path
from datetime import datetime
from copy import deepcopy

from nlm.core.brain import Brain
from nlm.core.config import Config
from nlm.benchmark.performance import PerformanceMonitor

class ExperimentError(Exception):
    """Experiment error."""
    
    def __init__(self, message: str, experiment_name: Optional[str] = None):
        super().__init__(message)
        self.experiment_name = experiment_name
        self.timestamp = datetime.now().isoformat()

class ExperimentConfig:
    """Configuration for experiment."""
    
    def __init__(self):
        self.name = "default_experiment"
        self.description = ""
        self.recording_interval = 100
        self.max_steps = 1000
        self.random_seed = 42
        self.checkpoint_enabled = True
        self.checkpoint_interval = 1000
        self.metrics = {}
        self.parameters = {}
    
    def validate(self):
        """Validate configuration."""
        if not self.name:
            raise ExperimentError("Experiment name cannot be empty", "")
        if self.recording_interval <= 0:
            raise ExperimentError("Recording interval must be positive", "")
        if self.max_steps <= 0:
            raise ExperimentError("Max steps must be positive", "")
        return True

class ExperimentResult:
    """Results from an experiment run."""
    
    def __init__(self, experiment_name: str):
        self.experiment_name = experiment_name
        self.timestamp = datetime.now().isoformat()
        self.metrics = {}
        self.raw_data = {}
        self.parameters = {}
        self.duration = 0.0
        self.success = False
        self.error_message = ""
        self.neural_metrics = {}
        self.performance_metrics = {}
    
    def add_metric(self, name: str, value: Any, unit: str = "", description: str = ""):
        """Add a metric to the experiment results.
        
        Args:
            name: Metric name
            value: Metric value
            unit: Unit of measurement
            description: Metric description
        """
        self.metrics[name] = {
            "value": value,
            "unit": unit,
            "description": description,
            "timestamp": time.time()
        }
    
    def add_neural_metrics(self, brain: Brain):
        """Add neural system metrics from brain.
        
        Args:
            brain: Brain object to analyze
        """
        self.neural_metrics = {
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "active_neurons": brain.getActiveNeuronCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "avg_firing_rate": brain.getAverageFiringRate(),
            "e_i_ratio": brain.getExcitationInhibitionRatio(),
            "total_spikes": brain.getTotalSpikeCount(),
            "pending_spikes": brain.getPendingSpikeEventCount(),
            "developmental_stage": brain.getDevelopmentalStage(),
            "timestamp": time.time()
        }
    
    def add_performance_metrics(self, monitor: PerformanceMonitor):
        """Add performance metrics from monitor.
        
        Args:
            monitor: PerformanceMonitor instance
        """
        self.performance_metrics = monitor.get_current_metrics()
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert experiment result to dictionary."""
        return {
            "experiment_name": self.experiment_name,
            "timestamp": self.timestamp,
            "metrics": self.metrics,
            "raw_data": self.raw_data,
            "neural_metrics": self.neural_metrics,
            "performance_metrics": self.performance_metrics,
            "duration": self.duration,
            "success": self.success,
            "error_message": self.error_message,
            "parameters": self.parameters
        }

class Experiment:
    """Experiment definition and execution.
    
    Provides experiment execution, monitoring, and result collection.
    """
    
    def __init__(self, name: str, config: Optional[ExperimentConfig] = None):
        self.name = name
        self.config = config or ExperimentConfig()
        self.config.name = name
        self.config.validate()
        
        self.is_running = False
        self.current_step = 0
        self.results = ExperimentResult(name)
        self.brain = None
        self.performance_monitor = None
        self.checkpoint_path = None
        
        print(f"Experiment initialized: {name}")
    
    def configure(self, **kwargs):
        """Configure experiment parameters.
        
        Args:
            **kwargs: Configuration parameters
        """
        for key, value in kwargs.items():
            if hasattr(self.config, key):
                setattr(self.config, key, value)
        
        self.config.validate()
        print(f"Experiment configured: {self.name}")
    
    def set_brain(self, brain: Brain):
        """Set brain for experiment.
        
        Args:
            brain: Brain object
        """
        self.brain = brain
        print(f"Brain set for experiment: {self.name}")
    
    def set_performance_monitor(self, monitor: PerformanceMonitor):
        """Set performance monitor for experiment.
        
        Args:
            monitor: PerformanceMonitor instance
        """
        self.performance_monitor = monitor
        print(f"Performance monitor set for experiment: {self.name}")
    
    def start(self):
        """Start experiment execution."""
        if not self.brain:
            raise ExperimentError("Brain not set for experiment", self.name)
        
        print(f"Starting experiment: {self.name}")
        self.is_running = True
        self.current_step = 0
        
        # Initialize performance monitor
        if self.performance_monitor:
            self.performance_monitor.start()
        
        # Reset brain
        self.brain.reset()
        
        # Initialize results
        self.results = ExperimentResult(self.name)
        self.results.parameters = self.config.__dict__.copy()
        
        # Record initial metrics
        self.results.add_neural_metrics(self.brain)
        if self.performance_monitor:
            self.results.add_performance_metrics(self.performance_monitor)
    
    def run_step(self, step: int, world=None, agent=None):
        """Run a single simulation step.
        
        Args:
            step: Current step number
            world: World object (optional)
            agent: Agent object (optional)
            
        Returns:
            Step results
        """
        if not self.is_running:
            raise ExperimentError("Experiment not running", self.name)
        
        self.current_step = step
        
        # Record metrics at intervals
        if step % self.config.recording_interval == 0:
            self._record_metrics(step, world, agent)
        
        # Execute simulation step
        self.brain.step(step)
        
        # Record performance metrics
        if self.performance_monitor:
            # Get current brain state for monitoring
            self.performance_monitor.record_step(
                step_time_ms=10.0,  # Placeholder
                spikes=self.brain.getTotalSpikeCount(),
                active_neurons=self.brain.getActiveNeuronCount()
            )
        
        # Save checkpoint if enabled
        if self.config.checkpoint_enabled and step % self.config.checkpoint_interval == 0:
            self._save_checkpoint(step)
        
        return {
            "step": step,
            "brain_metrics": self.results.neural_metrics,
            "performance_metrics": self.results.performance_metrics if self.performance_monitor else None,
            "timestamp": time.time()
        }
    
    def run_episode(self, world=None, agent=None, steps: int = None):
        """Run complete simulation episode.
        
        Args:
            world: World object (optional)
            agent: Agent object (optional)
            steps: Number of steps (uses config.max_steps if None)
            
        Returns:
            Episode results
        """
        if steps is None:
            steps = self.config.max_steps
        
        print(f"Running experiment episode: {self.name} ({steps} steps)")
        
        start_time = time.time()
        
        for step in range(steps):
            step_result = self.run_step(step, world, agent)
        
        # Calculate duration
        self.results.duration = time.time() - start_time
        
        # Mark as successful
        self.results.success = True
        
        # Record final metrics
        self._record_final_metrics()
        
        # Stop monitoring
        if self.performance_monitor:
            self.performance_monitor.stop()
        
        print(f"Experiment episode completed: {self.name} in {self.results.duration:.2f}s")
        
        return self.results
    
    def pause(self):
        """Pause experiment execution."""
        self.is_running = False
        print(f"Experiment paused: {self.name}")
    
    def resume(self):
        """Resume experiment execution."""
        if not self.is_running:
            print(f"Experiment resumed: {self.name}")
        else:
            raise ExperimentError("Experiment already running", self.name)
    
    def stop(self):
        """Stop experiment execution."""
        self.is_running = False
        
        # Stop monitoring
        if self.performance_monitor:
            self.performance_monitor.stop()
        
        print(f"Experiment stopped: {self.name}")
    
    def record_metric(self, name: str, value: Any, unit: str = "", description: str = ""):
        """Record a metric.
        
        Args:
            name: Metric name
            value: Metric value
            unit: Unit of measurement
            description: Metric description
        """
        self.results.add_metric(name, value, unit, description)
    
    def get_results(self) -> Dict[str, Any]:
        """Get experiment results.
        
        Returns:
            Experiment results
        """
        return self.results.to_dict()
    
    def save_results(self, filepath: str):
        """Save experiment results to file.
        
        Args:
            filepath: File path to save results
        """
        output_path = Path(filepath)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        results = self.get_results()
        
        with open(output_path, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"Experiment results saved to: {output_path.absolute()}")
    
    def load_results(self, filepath: str) -> Dict[str, Any]:
        """Load experiment results from file.
        
        Args:
            filepath: File path to load results from
            
        Returns:
            Loaded experiment results
        """
        input_path = Path(filepath)
        
        with open(input_path, 'r') as f:
            results = json.load(f)
        
        # Create new results object
        self.results = ExperimentResult(results["experiment_name"])
        
        # Load results data
        self.results.__dict__.update(results)
        
        print(f"Experiment results loaded from: {input_path.absolute()}")
        return results
    
    def _record_metrics(self, step: int, world=None, agent=None):
        """Record metrics at current step."""
        # Record neural metrics
        self.results.add_neural_metrics(self.brain)
        
        # Record performance metrics
        if self.performance_monitor:
            self.results.add_performance_metrics(self.performance_monitor)
        
        # Record world metrics if available
        if world:
            world_metrics = self._get_world_metrics(world)
            self.results.raw_data[f"step_{step}_world"] = world_metrics
        
        # Record agent metrics if available
        if agent:
            agent_metrics = self._get_agent_metrics(agent)
            self.results.raw_data[f"step_{step}_agent"] = agent_metrics
    
    def _record_final_metrics(self):
        """Record final metrics."""
        if self.brain:
            self.results.add_neural_metrics(self.brain)
        
        if self.performance_monitor:
            self.results.add_performance_metrics(self.performance_monitor)
    
    def _get_world_metrics(self, world) -> Dict[str, Any]:
        """Get world metrics."""
        # Placeholder implementation
        return {
            "step": self.current_step,
            "timestamp": time.time(),
            "placeholder": "World metrics implementation needed"
        }
    
    def _get_agent_metrics(self, agent) -> Dict[str, Any]:
        """Get agent metrics."""
        # Placeholder implementation
        return {
            "step": self.current_step,
            "timestamp": time.time(),
            "placeholder": "Agent metrics implementation needed"
        }
    
    def _save_checkpoint(self, step: int):
        """Save checkpoint at current step."""
        if not self.brain:
            return
        
        # Create checkpoint data
        checkpoint_data = {
            "step": step,
            "brain_state": self.brain.getCurrentState(),  # Placeholder
            "experiment_name": self.name,
            "timestamp": time.time(),
            "config": self.config.__dict__
        }
        
        # Save to file
        checkpoint_filename = f"{self.name}_step_{step}.json"
        checkpoint_path = Path("checkpoints") / checkpoint_filename
        checkpoint_path.parent.mkdir(parents=True, exist_ok=True)
        
        with open(checkpoint_path, 'w') as f:
            json.dump(checkpoint_data, f, indent=2)
        
        self.checkpoint_path = str(checkpoint_path)
        
        print(f"Checkpoint saved: {checkpoint_path}")
    
    def __repr__(self):
        return f"Experiment(name='{self.name}', running={self.is_running}, steps={self.current_step}/{self.config.max_steps})"

if __name__ == "__main__":
    # Example usage
    print("Running Experiment Manager Example")
    
    # Create experiment
    experiment = Experiment("learning_experiment")
    
    # Configure experiment
    experiment.configure(
        description="Neural network learning experiment",
        recording_interval=100,
        max_steps=500,
        random_seed=42,
        checkpoint_enabled=True
    )
    
    # Create mock brain
    class MockBrain:
        def reset(self):
            pass
        
        def step(self, step):
            pass
        
        def getTotalNeuronCount(self):
            return 1000
        
        def getTotalSynapseCount(self):
            return 10000
        
        def getActiveNeuronCount(self):
            return 150
        
        def getFiringNeuronCount(self):
            return 120
        
        def getAverageFiringRate(self):
            return 4.5
        
        def getExcitationInhibitionRatio(self):
            return 0.6
        
        def getTotalSpikeCount(self):
            return 5000
        
        def getPendingSpikeEventCount(self):
            return 50
        
        def getDevelopmentalStage(self):
            return "Adult"
        
        def getCurrentState(self):
            return {"placeholder": "brain state"}
    
    mock_brain = MockBrain()
    experiment.set_brain(mock_brain)
    
    # Create performance monitor
    monitor = PerformanceMonitor()
    experiment.set_performance_monitor(monitor)
    
    # Start experiment
    experiment.start()
    
    # Run experiment episode
    results = experiment.run_episode(steps=200)
    
    # Get experiment results
    print(f"\nExperiment completed!")
    print(f"Duration: {results['duration']:.2f}s")
    print(f"Success: {results['success']}")
    print(f"Neural metrics: {results['neural_metrics']}")
    
    # Save results
    experiment.save_results("experiment_results.json")
    
    # Record additional metrics
    experiment.record_metric("learning_curve", 0.85, "", "Learning performance")
    experiment.record_metric("reward", 150.5, "points", "Total reward accumulated")
    experiment.record_metric("success_rate", 0.85, "", "Success rate")
    
    # Stop experiment
    experiment.stop()
    
    print("Experiment manager example completed!")
