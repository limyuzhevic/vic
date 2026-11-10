"""
NLM Performance Testing and Benchmarking Tools

This module provides comprehensive performance testing capabilities for NLM (Neural Learning Machine).
It includes benchmarks for brain creation, simulation speed, plasticity operations, and memory usage.
"""

import time
import timeit
import statistics
from typing import Dict, List, Callable, Any, Optional, Tuple
from dataclasses import dataclass, field
from enum import Enum
import json
import os
import sys
from pathlib import Path

import numpy as np

import pynlm


@dataclass
class PerformanceMetrics:
    """Performance metrics for NLM operations."""
    
    # Timing metrics
    cpu_time: float = 0.0
    wall_time: float = 0.0
    user_time: float = 0.0
    system_time: float = 0.0
    
    # Memory metrics
    memory_before: float = 0.0
    memory_after: float = 0.0
    memory_peak: float = 0.0
    
    # Operation-specific metrics
    steps_per_second: float = 0.0
    neurons_per_second: float = 0.0
    synapses_per_second: float = 0.0
    spikes_per_second: float = 0.0
    
    # Quality metrics
    accuracy: Optional[float] = None
    stability: Optional[float] = None
    efficiency: Optional[float] = None
    
    @property
    def memory_delta(self) -> float:
        """Memory usage change in MB."""
        return self.memory_after - self.memory_before
    
    @property
    def cpu_efficiency(self) -> float:
        """CPU efficiency (steps per CPU second)."""
        return self.steps_per_second * self.cpu_time if self.cpu_time > 0 else 0.0
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert metrics to dictionary."""
        return {
            "cpu_time": self.cpu_time,
            "wall_time": self.wall_time,
            "user_time": self.user_time,
            "system_time": self.system_time,
            "memory_before": self.memory_before,
            "memory_after": self.memory_after,
            "memory_peak": self.memory_peak,
            "steps_per_second": self.steps_per_second,
            "neurons_per_second": self.neurons_per_second,
            "synapses_per_second": self.synapses_per_second,
            "spikes_per_second": self.spikes_per_second,
            "accuracy": self.accuracy,
            "stability": self.stability,
            "efficiency": self.efficiency,
        }


class BenchmarkStage(Enum):
    """Benchmark test stages."""
    
    CREATION = "creation"
    INITIALIZATION = "initialization"
    SIMULATION = "simulation"
    PLASTICITY = "plasticity"
    SCALING = "scaling"
    MEMORY = "memory"
    CONSOLIDATION = "consolidation"


@dataclass
class BenchmarkResult:
    """Result of a benchmark test."""
    
    name: str
    stage: BenchmarkStage
    iteration: int
    
    # Performance metrics
    metrics: PerformanceMetrics
    
    # Configuration used
    config: Dict[str, Any] = field(default_factory=dict)
    
    # Test parameters
    parameters: Dict[str, Any] = field(default_factory=dict)
    
    # Metadata
    timestamp: float = 0.0
    success: bool = True
    error_message: Optional[str] = None
    
    # Statistical analysis
    mean_time: float = 0.0
    std_time: float = 0.0
    min_time: float = 0.0
    max_time: float = 0.0
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert result to dictionary for serialization."""
        return {
            "name": self.name,
            "stage": self.stage.value,
            "iteration": self.iteration,
            "metrics": self.metrics.to_dict(),
            "config": self.config,
            "parameters": self.parameters,
            "timestamp": self.timestamp,
            "success": self.success,
            "error_message": self.error_message,
            "mean_time": self.mean_time,
            "std_time": self.std_time,
            "min_time": self.min_time,
            "max_time": self.max_time,
        }


class NLMBenchmarkSuite:
    """
    Comprehensive benchmarking suite for NLM performance testing.
    
    Provides benchmarks for:
    - Brain creation and initialization
    - Simulation speed and scalability
    - Plasticity operations
    - Memory usage and efficiency
    - Learning performance
    """
    
    def __init__(self):
        self.results: List[BenchmarkResult] = []
        self.config = pynlm.createDefaultConfig()
        self._setup_default_parameters()
    
    def _setup_default_parameters(self):
        """Setup default benchmark parameters."""
        self.default_params = {
            "brain_creation": {
                "neuron_counts": [100, 500, 1000, 2000, 5000],
                "repeat": 5,
                "warmup": 2
            },
            "simulation": {
                "neuron_counts": [500, 1000, 2000],
                "step_counts": [100, 500, 1000, 5000],
                "repeat": 3,
                "warmup": 1
            },
            "plasticity": {
                "synapse_counts": [100, 500, 1000, 2000],
                "plasticity_iterations": 50,
                "repeat": 3,
                "warmup": 1
            },
            "memory": {
                "brain_sizes": [1000, 2000, 5000],
                "operation_types": ["creation", "simulation", "plasticity"],
                "repeat": 3
            }
        }
    
    def run_brain_creation_benchmark(self, 
                                     neuron_counts: Optional[List[int]] = None,
                                     repeat: int = 5,
                                     warmup: int = 2) -> List[BenchmarkResult]:
        """
        Benchmark brain creation performance.
        
        Args:
            neuron_counts: List of neuron counts to test
            repeat: Number of repetitions for each configuration
            warmup: Number of warmup runs
            
        Returns:
            List of benchmark results
        """
        if neuron_counts is None:
            neuron_counts = self.default_params["brain_creation"]["neuron_counts"]
        
        results = []
        
        for neuron_count in neuron_counts:
            self.config.set("brain.neuron_count", neuron_count)
            self.config.set("random_seed", 42)
            
            # Warmup runs
            for _ in range(warmup):
                temp_brain = pynlm.createBrain(self.config)
                temp_brain.initialize()
                for step in range(10):
                    temp_brain.step(step)
            
            # Actual benchmark runs
            for i in range(repeat):
                start_time = time.time()
                
                brain = pynlm.createBrain(self.config)
                brain.initialize()
                
                init_time = time.time() - start_time
                
                # Additional setup time
                for step in range(5):
                    brain.step(step)
                
                total_time = time.time() - start_time
                
                metrics = PerformanceMetrics()
                metrics.wall_time = total_time
                metrics.steps_per_second = 5 / init_time if init_time > 0 else 0.0
                metrics.neurons_per_second = neuron_count / total_time if total_time > 0 else 0.0
                
                result = BenchmarkResult(
                    name=f"brain_creation_{neuron_count}",
                    stage=BenchmarkStage.CREATION,
                    iteration=i,
                    metrics=metrics,
                    config={"neuron_count": neuron_count},
                    parameters={
                        "neuron_count": neuron_count,
                        "repeat": repeat,
                        "warmup": warmup
                    },
                    timestamp=time.time()
                )
                
                results.append(result)
        
        return results
    
    def run_simulation_benchmark(self,
                                neuron_counts: Optional[List[int]] = None,
                                step_counts: Optional[List[int]] = None,
                                repeat: int = 3,
                                warmup: int = 1) -> List[BenchmarkResult]:
        """
        Benchmark simulation performance.
        
        Args:
            neuron_counts: List of neuron counts to test
            step_counts: List of step counts to test
            repeat: Number of repetitions for each configuration
            warmup: Number of warmup runs
            
        Returns:
            List of benchmark results
        """
        if neuron_counts is None:
            neuron_counts = self.default_params["simulation"]["neuron_counts"]
        
        if step_counts is None:
            step_counts = self.default_params["simulation"]["step_counts"]
        
        results = []
        
        for neuron_count in neuron_counts:
            for step_count in step_counts:
                self.config.set("brain.neuron_count", neuron_count)
                self.config.set("random_seed", 42)
                
                # Warmup runs
                for _ in range(warmup):
                    temp_brain = pynlm.createBrain(self.config)
                    temp_brain.initialize()
                    for step in range(min(step_count, 10)):
                        temp_brain.step(step)
                
                # Actual benchmark runs
                for i in range(repeat):
                    start_time = time.time()
                    
                    brain = pynlm.createBrain(self.config)
                    brain.initialize()
                    
                    # Run simulation
                    for step in range(step_count):
                        brain.step(step)
                    
                    total_time = time.time() - start_time
                    
                    metrics = PerformanceMetrics()
                    metrics.wall_time = total_time
                    metrics.steps_per_second = step_count / total_time if total_time > 0 else 0.0
                    metrics.neurons_per_second = neuron_count / total_time if total_time > 0 else 0.0
                    
                    result = BenchmarkResult(
                        name=f"simulation_{neuron_count}_neurons_{step_count}_steps",
                        stage=BenchmarkStage.SIMULATION,
                        iteration=i,
                        metrics=metrics,
                        config={"neuron_count": neuron_count, "step_count": step_count},
                        parameters={
                            "neuron_count": neuron_count,
                            "step_count": step_count,
                            "repeat": repeat,
                            "warmup": warmup
                        },
                        timestamp=time.time()
                    )
                    
                    results.append(result)
        
        return results
    
    def run_plasticity_benchmark(self,
                                 synapse_counts: Optional[List[int]] = None,
                                 plasticity_iterations: int = 50,
                                 repeat: int = 3,
                                 warmup: int = 1) -> List[BenchmarkResult]:
        """
        Benchmark plasticity operations performance.
        
        Args:
            synapse_counts: List of synapse counts to test
            plasticity_iterations: Number of plasticity iterations
            repeat: Number of repetitions for each configuration
            warmup: Number of warmup runs
            
        Returns:
            List of benchmark results
        """
        if synapse_counts is None:
            synapse_counts = self.default_params["plasticity"]["synapse_counts"]
        
        results = []
        
        for synapse_count in synapse_counts:
            self.config.set("brain.neuron_count", synapse_count)
            self.config.set("plasticity.stdp.learning_rate", 0.001)
            self.config.set("plasticity.hebbian.learning_rate", 0.01)
            self.config.set("random_seed", 42)
            
            # Warmup runs
            for _ in range(warmup):
                temp_brain = pynlm.createBrain(self.config)
                temp_brain.initialize()
                temp_agent = pynlm.createAgentBrain(temp_brain)
                temp_world = pynlm.createSimpleWorld()
                temp_world.configure(width=10, height=10)
                temp_agent.initialize(temp_world)
                
                for step in range(min(plasticity_iterations, 10)):
                    temp_world.update(0.1)
                    temp_agent.processSensoryInput(temp_world.getSensoryPercept())
                    temp_brain.step(step)
                    action = temp_agent.decodeMotorCommand()
                    temp_world.applyMotorCommand(action, temp_world.getSimulationTime())
            
            # Actual benchmark runs
            for i in range(repeat):
                start_time = time.time()
                
                brain = pynlm.createBrain(self.config)
                brain.initialize()
                
                agent = pynlm.createAgentBrain(brain)
                world = pynlm.createSimpleWorld()
                world.configure(width=10, height=10)
                agent.initialize(world)
                agent.enableStructuralPlasticity(True)
                
                # Run plasticity simulation
                for step in range(plasticity_iterations):
                    world.update(0.1)
                    agent.processSensoryInput(world.getSensoryPercept())
                    brain.step(step)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                    
                    # Apply reward modulation
                    reward = agent.getCuriosityLevel() * 0.1
                    agent.applyRewardModulation(reward, 0.05)
                
                total_time = time.time() - start_time
                
                metrics = PerformanceMetrics()
                metrics.wall_time = total_time
                metrics.steps_per_second = plasticity_iterations / total_time if total_time > 0 else 0.0
                metrics.synapses_per_second = synapse_count / total_time if total_time > UBA0 else 0.0
                
                result = BenchmarkResult(
                    name=f"plasticity_{synapse_count}_synapses",
                    stage=BenchmarkStage.PLASTICITY,
                    iteration=i,
                    metrics=metrics,
                    config={"synapse_count": synapse_count},
                    parameters={
                        "synapse_count": synapse_count,
                        "plasticity_iterations": plasticity_iterations,
                        "repeat": repeat,
                        "warmup": warmup
                    },
                    timestamp=time.time()
                )
                
                results.append(result)
        
        return results
    
    def run_memory_benchmark(self,
                            brain_sizes: Optional[List[int]] = None,
                            operation_types: Optional[List[str]] = None,
                            repeat: int = 3) -> List[BenchmarkResult]:
        """
        Benchmark memory usage and efficiency.
        
        Args:
            brain_sizes: List of brain sizes to test
            operation_types: List of operation types to benchmark
            repeat: Number of repetitions for each configuration
            
        Returns:
            List of benchmark results
        """
        if brain_sizes is None:
            brain_sizes = self.default_params["memory"]["brain_sizes"]
        
        if operation_types is None:
            operation_types = self.default_params["memory"]["operation_types"]
        
        results = []
        
        for brain_size in brain_sizes:
            for operation_type in operation_types:
                self.config.set("brain.neuron_count", brain_size)
                self.config.set("random_seed", 42)
                
                for i in range(repeat):
                    start_time = time.time()
                    
                    # Measure memory before operation
                    import psutil
                    import gc
                    process = psutil.Process()
                    gc.collect()
                    memory_before = process.memory_info().rss / 1024 / 1024  # MB
                    
                    # Perform operation
                    if operation_type == "creation":
                        brain = pynlm.createBrain(self.config)
                        brain.initialize()
                    
                    elif operation_type == "simulation":
                        brain = pynlm.createBrain(self.config)
                        brain.initialize()
                        for step in range(100):
                            brain.step(step)
                    
                    elif operation_type == "plasticity":
                        brain = pynlm.createBrain(self.config)
                        brain.initialize()
                        
                        agent = pynlm.createAgentBrain(brain)
                        world = pynlm.createSimpleWorld()
                        world.configure(width=10, height=10)
                        agent.initialize(world)
                        agent.enableStructuralPlasticity(True)
                        
                        for step in range(50):
                            world.update(0.1)
                            agent.processSensoryInput(world.getSensoryPercept())
                            brain.step(step)
                    
                    # Measure memory after operation
                    gc.collect()
                    memory_after = process.memory_info().rss / 1024 / 1024  # MB
                    memory_peak = process.memory_info().rss / 1024 / 1024  # MB
                    
                    total_time = time.time() - start_time
                    
                    metrics = PerformanceMetrics()
                    metrics.wall_time = total_time
                    metrics.memory_before = memory_before
                    metrics.memory_after = memory_after
                    metrics.memory_peak = memory_peak
                    
                    if operation_type == "creation":
                        metrics.neurons_per_second = brain_size / total_time if total_time > 0 else 0.0
                    elif operation_type == "simulation":
                        metrics.steps_per_second = 100 / total_time if total_time > 0 else 0.0
                        metrics.neurons_per_second = brain_size / total_time if total_time > 0 else 0.0
                    elif operation_type == "plasticity":
                        metrics.steps_per_second = 50 / total_time if total_time > 0 else 0.0
                        metrics.synapses_per_second = brain_size / total_time if total_time > 0 else 0.0
                    
                    result = BenchmarkResult(
                        name=f"memory_{operation_type}_{brain_size}",
                        stage=BenchmarkStage.MEMORY,
                        iteration=i,
                        metrics=metrics,
                        config={"brain_size": brain_size, "operation_type": operation_type},
                        parameters={
                            "brain_size": brain_size,
                            "operation_type": operation_type,
                            "repeat": repeat
                        },
                        timestamp=time.time()
                    )
                    
                    results.append(result)
        
        return results
    
    def run_all_benchmarks(self,
                          stages: Optional[List[BenchmarkStage]] = None) -> List[BenchmarkResult]:
        """
        Run all benchmark stages.
        
        Args:
            stages: List of benchmark stages to run (default: all)
            
        Returns:
            List of all benchmark results
        """
        if stages is None:
            stages = list(BenchmarkStage)
        
        all_results = []
        
        for stage in stages:
            print(f"\n=== Running {stage.value} benchmarks ===")
            
            if stage == BenchmarkStage.CREATION:
                results = self.run_brain_creation_benchmark()
            elif stage == BenchmarkStage.SIMULATION:
                results = self.run_simulation_benchmark()
            elif stage == BenchmarkStage.PLASTICITY:
                results = self.run_plasticity_benchmark()
            elif stage == BenchmarkStage.MEMORY:
                results = self.run_memory_benchmark()
            elif stage == BenchmarkStage.SCALING:
                results = self.run_scaling_benchmark()
            elif stage == BenchmarkStage.CONSOLIDATION:
                results = self.run_consolidation_benchmark()
            else:
                print(f"Warning: Benchmark stage {stage} not implemented")
                continue
            
            self.results.extend(results)
            all_results.extend(results)
            print(f"✓ Completed {len(results)} {stage.value} benchmarks")
        
        return all_results
    
    def run_scaling_benchmark(self,
                             neuron_counts: Optional[List[int]] = None,
                             step_counts: Optional[List[int]] = None,
                             repeat: int = 2) -> List[BenchmarkResult]:
        """
        Benchmark performance scaling with increasing complexity.
        
        Args:
            neuron_counts: List of neuron counts to test
            step_counts: List of step counts to test
            repeat: Number of repetitions
            
        Returns:
            List of benchmark results
        """
        if neuron_counts is None:
            neuron_counts = [500, 1000, 2000, 5000]
        
        if step_counts is None:
            step_counts = [100, 500, 1000, 5000]
        
        return self.run_simulation_benchmark(neuron_counts, step_counts, repeat)
    
    def run_consolidation_benchmark(self,
                                    iteration_counts: Optional[List[int]] = None,
                                    repeat: int = 2) -> List[BenchmarkResult]:
        """
        Benchmark learning consolidation and stability.
        
        Args:
            iteration_counts: List of iteration counts to test
            repeat: Number of repetitions
            
        Returns:
            List of benchmark results
        """
        if iteration_counts is None:
            iteration_counts = [100, 500, 1000]
        
        results = []
        
        for iteration_count in iteration_counts:
            self.config.set("brain.neuron_count", 1000)
            self.config.set("random_seed", 42)
            
            for i in range(repeat):
                start_time = time.time()
                
                brain = pynlm.createBrain(self.config)
                brain.initialize()
                
                agent = pynlm.createAgentBrain(brain)
                world = pynlm.createSimpleWorld()
                world.configure(width=10, height=10)
                agent.initialize(world)
                
                # Track learning stability
                initial_firing = brain.getTotalSpikeCount()
                
                for step in range(iteration_count):
                    world.update(0.1)
                    agent.processSensoryInput(world.getSensoryPercept())
                    brain.step(step)
                    
                    # Apply reward to encourage learning
                    reward = min(agent.getCuriosityLevel() * 0.1, 1.0)
                    agent.applyRewardModulation(reward, 0.05)
                    
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                
                final_firing = brain.getTotalSpikeCount()
                firing_growth = final_firing - initial_firing
                
                total_time = time.time() - start_time
                
                metrics = PerformanceMetrics()
                metrics.wall_time = total_time
                metrics.steps_per_second = iteration_count / total_time if total_time > 0 else 0.0
                metrics.spikes_per_second = firing_growth / total_time if total_time > 0 else 0.0
                metrics.efficiency = firing_growth / iteration_count if iteration_count > 0 else 0.0
                
                result = BenchmarkResult(
                    name=f"consolidation_{iteration_count}_iterations",
                    stage=BenchmarkStage.CONSOLIDATION,
                    iteration=i,
                    metrics=metrics,
                    config={"neuron_count": 1000, "iteration_count": iteration_count},
                    parameters={
                        "iteration_count": iteration_count,
                        "repeat": repeat
                    },
                    timestamp=time.time()
                )
                
                results.append(result)
        
        return results
    
    def calculate_statistics(self, results: List[BenchmarkResult]) -> Dict[str, Any]:
        """
        Calculate statistical summaries for benchmark results.
        
        Args:
            results: List of benchmark results
            
        Returns:
            Dictionary containing statistical summaries
        """
        if not results:
            return {}
        
        # Group results by name
        grouped_results = {}
        for result in results:
            if result.name not in grouped_results:
                grouped_results[result.name] = []
            grouped_results[result.name].append(result)
        
        statistics = {}
        
        for name, result_list in grouped_results.items():
            times = [r.metrics.wall_time for r in result_list]
            steps_per_second = [r.metrics.steps_per_second for r in result_list]
            neurons_per_second = [r.metrics.neurons_per_second for r in result_list]
            
            statistics[name] = {
                "mean_time": statistics.mean(times) if times else 0.0,
                "std_time": statistics.stdev(times) if len(times) > 1 else 0.0,
                "min_time": min(times) if times else 0.0,
                "max_time": max(times) if times else 0.0,
                "mean_steps_per_second": statistics.mean(steps_per_second) if steps_per_second else 0.0,
                "mean_neurons_per_second": statistics.mean(neurons_per_second) if neurons_per_second else 0.0,
                "number_of_runs": len(result_list),
                "success_rate": sum(1 for r in result_list if r.success) / len(result_list) if result_list else 0.0
            }
        
        return statistics
    
    def print_summary(self):
        """Print a comprehensive summary of all benchmark results."""
        if not self.results:
            print("No benchmark results available.")
            return
        
        print("\n" + "="*80)
        print("NLM BENCHMARK SUMMARY")
        print("="*80)
        
        # Group results by stage
        stages = {}
        for result in self.results:
            if result.stage not in stages:
                stages[result.stage] = []
            stages[result.stage].append(result)
        
        # Print statistics for each stage
        for stage in sorted(stages.keys(), key=lambda x: x.value):
            print(f"\n{stage.value.upper()} BENCHMARKS:")
            print("-" * 80)
            
            stage_results = stages[stage]
            stats = self.calculate_statistics(stage_results)
            
            for name, stat in stats.items():
                print(f"{name}:")
                print(f"  Runs: {stat['number_of_runs']}")
                print(f"  Mean time: {stat['mean_time']:.3f}s")
                print(f"  Std time: {stat['std_time']:.3f}s")
                print(f"  Speed: {stat['mean_steps_per_second']:.1f} steps/s")
                print(f"  Neuron throughput: {stat['mean_neurons_per_second']:.1f} neurons/s")
                print(f"  Success rate: {stat['success_rate']*100:.1f}%")
        
        print(f"\nTotal benchmarks run: {len(self.results)}")
        overall_stats = self.calculate_statistics(self.results)
        
        if "brain_creation_1000" in overall_stats:
            print(f"\nOverall performance benchmark (brain creation):")
            print(f"  Average creation time: {overall_stats['brain_creation_1000']['mean_time']:.3f}s")
            print(f"  Average creation speed: {overall_stats['brain_creation_1000']['mean_steps_per_second']:.1f} operations/s")
    
    def save_results(self, filepath: str):
        """
        Save benchmark results to a JSON file.
        
        Args:
            filepath: Path to save results
        """
        data = {
            "timestamp": time.time(),
            "benchmarks": [result.to_dict() for result in self.results],
            "statistics": self.calculate_statistics(self.results)
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
        
        print(f"Benchmark results saved to: {filepath}")
    
    def load_results(self, filepath: str):
        """
        Load benchmark results from a JSON file.
        
        Args:
            filepath: Path to load results from
        """
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        self.results = []
        for result_data in data["benchmarks"]:
            # Convert stage from string back to enum
            stage = BenchmarkStage(result_data["stage"])
            
            result = BenchmarkResult(
                name=result_data["name"],
                stage=stage,
                iteration=result_data["iteration"],
                metrics=PerformanceMetrics(**result_data["metrics"]),
                config=result_data["config"],
                parameters=result_data["parameters"],
                timestamp=result_data["timestamp"],
                success=result_data["success"],
                error_message=result_data.get("error_message"),
                mean_time=result_data.get("mean_time", 0.0),
                std_time=result_data.get("std_time", 0.0),
                min_time=result_data.get("min_time", 0.0),
                max_time=result_data.get("max_time", 0.0)
            )
            self.results.append(result)
        
        print(f"Benchmark results loaded from: {filepath}")
        print(f"Loaded {len(self.results)} benchmark results")


# Main benchmark execution functions

def run_comprehensive_benchmark():
    """Run comprehensive benchmarks and print summary."""
    benchmark = NLMBenchmarkSuite()
    
    print("Starting NLM comprehensive benchmarks...")
    results = benchmark.run_all_benchmarks()
    
    benchmark.print_summary()
    return results


def run_specific_benchmark(stages: List[str]):
    """
    Run specific benchmark stages.
    
    Args:
        stages: List of stage names to run
        
    Returns:
        List of benchmark results
    """
    benchmark = NLMBenchmarkSuite()
    
    # Convert stage names to enum values
    stage_mapping = {
        "creation": BenchmarkStage.CREATION,
        "simulation": BenchmarkStage.SIMULATION,
        "plasticity": BenchmarkStage.PLASTICITY,
        "memory": BenchmarkStage.MEMORY,
        "scaling": BenchmarkStage.SCALING,
        "consolidation": BenchmarkStage.CONSOLIDATION
    }
    
    selected_stages = []
    for stage_name in stages:
        if stage_name in stage_mapping:
            selected_stages.append(stage_mapping[stage_name])
        else:
            print(f"Warning: Unknown benchmark stage '{stage_name}'")
    
    print(f"Running benchmarks for stages: {[s.value for s in selected_stages]}")
    results = benchmark.run_all_benchmarks(selected_stages)
    
    benchmark.print_summary()
    return results


if __name__ == "__main__":
    # If called as main script, run comprehensive benchmarks
    run_comprehensive_benchmark()
