#!/usr/bin/env python3
"""
Performance Monitoring and Benchmarking Tools for NLM

This module provides comprehensive performance monitoring and benchmarking
tools for the NLM (Neural Learning Machine) neural simulation framework.
It includes:
- Real-time performance monitoring
- Benchmark suites for different test scenarios
- Performance analysis and reporting
- Resource usage tracking
- Comparative performance analysis
"""

import time
import threading
import psutil
import numpy as np
from typing import Dict, List, Optional, Callable, Any, Tuple
from dataclasses import dataclass, field
from datetime import datetime
from enum import Enum
import json
import os

# Import NLM bindings if available
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False

class PerformanceMetric(Enum):
    """Enumeration of performance metric types"""
    SIMULATION_SPEED = "simulation_speed"
    MEMORY_USAGE = "memory_usage"
    CPU_USAGE = "cpu_usage"
    SPIKE_RATE = "spike_rate"
    ENERGY_CONSUMPTION = "energy_consumption"
    LATENCY = "latency"
    THROUGHPUT = "throughput"
    PLasticity_SPEED = "plasticity_speed"

@dataclass
class PerformanceSample:
    """Single performance measurement sample"""
    timestamp: datetime
    metric_type: PerformanceMetric
    value: float
    unit: str
    context: Dict[str, Any] = field(default_factory=dict)

@dataclass
class BenchmarkResult:
    """Result of a benchmark test"""
    test_name: str
    test_category: str
    configuration: Dict[str, Any]
    metrics: Dict[str, List[PerformanceSample]]
    execution_time: float
    memory_peak: float
    cpu_peak: float
    success: bool
    error_message: Optional[str] = None
    timestamp: datetime = field(default_factory=datetime.now)

class ResourceMonitor:
    """System resource monitoring"""
    
    def __init__(self):
        self.monitoring = False
        self.samples: List[PerformanceSample] = []
        self.monitor_thread: Optional[threading.Thread] = None
    
    def start_monitoring(self, interval: float = 0.1):
        """Start continuous resource monitoring"""
        if self.monitoring:
            return
        
        self.monitoring = True
        self.samples.clear()
        
        def monitor_loop():
            while self.monitoring:
                self._collect_sample()
                time.sleep(interval)
        
        self.monitor_thread = threading.Thread(target=monitor_loop, daemon=True)
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        """Stop resource monitoring"""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
            self.monitor_thread = None
    
    def _collect_sample(self):
        """Collect a single resource sample"""
        process = psutil.Process()
        
        # CPU usage
        cpu_percent = process.cpu_percent()
        self.samples.append(PerformanceSample(
            timestamp=datetime.now(),
            metric_type=PerformanceMetric.CPU_USAGE,
            value=cpu_percent,
            unit="%"
        ))
        
        # Memory usage
        memory_info = process.memory_info()
        memory_mb = memory_info.rss / 1024 / 1024
        self.samples.append(PerformanceSample(
            timestamp=datetime.now(),
            metric_type=PerformanceMetric.MEMORY_USAGE,
            value=memory_mb,
            unit="MB"
        ))
    
    def get_samples(self, metric_type: Optional[PerformanceMetric] = None,
                   start_time: Optional[datetime] = None,
                   end_time: Optional[datetime] = None) -> List[PerformanceSample]:
        """Get monitoring samples"""
        samples = self.samples
        
        if metric_type:
            samples = [s for s in samples if s.metric_type == metric_type]
        
        if start_time:
            samples = [s for s in samples if s.timestamp >= start_time]
        
        if end_time:
            samples = [s for s in samples if s.timestamp <= end_time]
        
        return sorted(samples, key=lambda x: x.timestamp)
    
    def get_summary(self, metric_type: Optional[PerformanceMetric] = None) -> Dict[str, float]:
        """Get statistical summary of samples"""
        samples = self.get_samples(metric_type)
        
        if not samples:
            return {}
        
        values = [s.value for s in samples]
        
        return {
            "mean": np.mean(values),
            "median": np.median(values),
            "std": np.std(values),
            "min": np.min(values),
            "max": np.max(values),
            "count": len(values)
        }

class SimulationProfiler:
    """Performance profiler for NLM simulations"""
    
    def __init__(self):
        self.profiles: Dict[str, Dict[str, Any]] = {}
        self.resource_monitor = ResourceMonitor()
    
    def profile_simulation(self, brain, steps: int, input_pattern: Callable,
                          profile_name: str, **kwargs) -> Dict[str, Any]:
        """
        Profile a simulation run
        
        Args:
            brain: NLM brain instance
            steps: Number of simulation steps
            input_pattern: Function generating input for each step
            profile_name: Name for the profile
            **kwargs: Additional configuration
            
        Returns:
            Dictionary with profile results
        """
        # Reset resource monitoring
        self.resource_monitor.stop_monitoring()
        self.resource_monitor.samples.clear()
        
        # Start monitoring
        self.resource_monitor.start_monitoring(0.01)  # 10ms interval
        
        # Record start time and memory
        start_time = time.time()
        process = psutil.Process()
        initial_memory = process.memory_info().rss / 1024 / 1024
        
        # Run simulation
        brain.reset()
        brain.initialize()
        
        spike_count = brain.getTotalSpikeCount()
        
        for step in range(steps):
            input_data = input_pattern(step)
            brain.inject_input(input_data, step * 0.001)
            brain.step(step * 0.001)
            spike_count = brain.getTotalSpikeCount()
        
        end_time = time.time()
        final_memory = process.memory_info().rss / 1024 / 1024
        
        # Stop monitoring
        self.resource_monitor.stop_monitoring()
        
        # Calculate performance metrics
        execution_time = end_time - start_time
        memory_peak = self.resource_monitor.get_summary(PerformanceMetric.MEMORY_USAGE)
        cpu_peak = self.resource_monitor.get_summary(PerformanceMetric.CPU_USAGE)
        
        # Simulation performance metrics
        simulation_metrics = {
            "steps": steps,
            "execution_time": execution_time,
            "time_per_step": execution_time / steps if steps > 0 else 0,
            "final_spike_count": spike_count,
            "spike_rate": spike_count / execution_time if execution_time > 0 else 0,
            "memory_initial": initial_memory,
            "memory_final": final_memory,
            "memory_delta": final_memory - initial_memory,
            "memory_peak_mean": memory_peak.get("mean", 0),
            "memory_peak_max": memory_peak.get("max", 0),
            "cpu_peak_mean": cpu_peak.get("mean", 0),
            "cpu_peak_max": cpu_peak.get("max", 0),
            "cpu_time_percent": cpu_peak.get("mean", 0)
        }
        
        # Store profile
        self.profiles[profile_name] = {
            "simulation_metrics": simulation_metrics,
            "resource_samples": self.resource_monitor.samples,
            "timestamp": datetime.now().isoformat(),
            "configuration": kwargs
        }
        
        return simulation_metrics
    
    def profile_training(self, brain, training_config: Dict[str, Any]) -> Dict[str, Any]:
        """
        Profile training session
        
        Args:
            brain: NLM brain instance
            training_config: Training configuration
            
        Returns:
            Dictionary with training profile results
        """
        episodes = training_config.get("episodes", 100)
        max_steps = training_config.get("max_steps", 1000)
        input_generator = training_config.get("input_generator")
        reward_function = training_config.get("reward_function")
        
        # Reset resource monitoring
        self.resource_monitor.stop_monitoring()
        self.resource_monitor.samples.clear()
        
        # Start monitoring
        self.resource_monitor.start_monitoring(0.05)  # 50ms interval
        
        # Record start time
        start_time = time.time()
        process = psutil.Process()
        initial_memory = process.memory_info().rss / 1024 / 1024
        
        # Run training
        brain.reset()
        brain.initialize()
        
        training_metrics = {
            "episodes": episodes,
            "max_steps": max_steps,
            "total_reward": 0.0,
            "episode_rewards": [],
            "spike_counts": [],
            "active_neurons": [],
            "firing_rates": []
        }
        
        for episode in range(episodes):
            episode_reward = 0.0
            episode_spikes = 0
            episode_active = 0
            
            for step in range(max_steps):
                # Generate input
                input_data = input_generator(step, episode)
                brain.inject_input(input_data, step * 0.001)
                
                # Get action and apply
                action = brain.produce_action()
                brain.step(step * 0.001)
                
                # Calculate and apply reward
                reward = reward_function(brain, action, step, episode)
                episode_reward += reward
                brain.apply_reward(reward)
                
                # Record metrics
                episode_spikes = brain.getTotalSpikeCount()
                episode_active = brain.getActiveNeuronCount()
                episode_firing_rate = brain.getAverageFiringRate()
                
                if episode == 0:  # Only record for first episode to save memory
                    training_metrics["spike_counts"].append(episode_spikes)
                    training_metrics["active_neurons"].append(episode_active)
                    training_metrics["firing_rates"].append(episode_firing_rate)
            
            training_metrics["total_reward"] += episode_reward
            training_metrics["episode_rewards"].append(episode_reward)
        
        end_time = time.time()
        final_memory = process.memory_info().rss / 1024 / 1024
        
        # Stop monitoring
        self.resource_monitor.stop_monitoring()
        
        # Calculate performance metrics
        execution_time = end_time - start_time
        memory_peak = self.resource_monitor.get_summary(PerformanceMetric.MEMORY_USAGE)
        cpu_peak = self.resource_monitor.get_summary(PerformanceMetric.CPU_USAGE)
        
        # Training performance metrics
        training_results = {
            "episodes": episodes,
            "execution_time": execution_time,
            "time_per_episode": execution_time / episodes if episodes > 0 else 0,
            "total_reward": training_metrics["total_reward"],
            "avg_reward_per_episode": training_metrics["total_reward"] / episodes,
            "max_reward": max(training_metrics["episode_rewards"]),
            "min_reward": min(training_metrics["episode_rewards"]),
            "reward_std": np.std(training_metrics["episode_rewards"]) if len(training_metrics["episode_rewards"]) > 1 else 0,
            "memory_initial": initial_memory,
            "memory_final": final_memory,
            "memory_delta": final_memory - initial_memory,
            "memory_peak_mean": memory_peak.get("mean", 0),
            "memory_peak_max": memory_peak.get("max", 0),
            "cpu_peak_mean": cpu_peak.get("mean", 0),
            "cpu_peak_max": cpu_peak.get("max", 0),
            "spike_count_efficiency": sum(training_metrics["spike_counts"]) / len(training_metrics["spike_counts"]) if training_metrics["spike_counts"] else 0,
            "active_efficiency": sum(training_metrics["active_neurons"]) / len(training_metrics["active_neurons"]) if training_metrics["active_neurons"] else 0
        }
        
        return training_results
    
    def compare_profiles(self, profile_names: List[str]) -> Dict[str, Any]:
        """
        Compare multiple profiles
        
        Args:
            profile_names: List of profile names to compare
            
        Returns:
            Dictionary with comparison results
        """
        comparison = {
            "profiles": {},
            "summary": {}
        }
        
        for profile_name in profile_names:
            if profile_name in self.profiles:
                profile = self.profiles[profile_name]
                
                comparison["profiles"][profile_name] = {
                    "simulation_metrics": profile["simulation_metrics"],
                    "timestamp": profile["timestamp"],
                    "configuration": profile["configuration"]
                }
        
        # Generate comparison summary
        if len(comparison["profiles"]) >= 2:
            profile_names_list = list(comparison["profiles"].keys())
            
            # Compare execution times
            execution_times = []
            for profile_name in profile_names_list:
                time_per_step = comparison["profiles"][profile_name]["simulation_metrics"]["time_per_step"]
                execution_times.append(time_per_step)
            
            if execution_times:
                comparison["summary"]["execution_time"] = {
                    "fastest": min(execution_times),
                    "slowest": max(execution_times),
                    "mean": np.mean(execution_times),
                    "std": np.std(execution_times),
                    "speedup_factor": max(execution_times) / min(execution_times) if min(execution_times) > 0 else float('inf')
                }
            
            # Compare spike rates
            spike_rates = []
            for profile_name in profile_names_list:
                spike_rate = comparison["profiles"][profile_name]["simulation_metrics"]["spike_rate"]
                spike_rates.append(spike_rate)
            
            if spike_rates:
                comparison["summary"]["spike_rate"] = {
                    "highest": max(spike_rates),
                    "lowest": min(spike_rates),
                    "mean": np.mean(spike_rates),
                    "std": np.std(spike_rates)
                }
            
            # Compare memory efficiency
            memory_deltas = []
            for profile_name in profile_names_list:
                memory_delta = comparison["profiles"][profile_name]["simulation_metrics"]["memory_delta"]
                memory_deltas.append(memory_delta)
            
            if memory_deltas:
                comparison["summary"]["memory_efficiency"] = {
                    "most_efficient": min(memory_deltas),  # Most negative is best (less memory used)
                    "least_efficient": max(memory_deltas),
                    "mean": np.mean(memory_deltas),
                    "std": np.std(memory_deltas)
                }
        
        return comparison
    
    def export_profiles(self, filepath: str):
        """Export all profiles to JSON file"""
        data = {
            "profiles": self.profiles,
            "export_timestamp": datetime.now().isoformat()
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
    
    def import_profiles(self, filepath: str):
        """Import profiles from JSON file"""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        self.profiles = data.get("profiles", {})

class BenchmarkSuite:
    """Comprehensive benchmark suite for NLM"""
    
    def __init__(self):
        self.profiler = SimulationProfiler()
        self.results: List[BenchmarkResult] = []
    
    def add_brain_configuration(self, name: str, **kwargs) -> 'BenchmarkSuite':
        """Add a brain configuration to benchmark"""
        return self
    
    def run_brain_benchmark(self, brain, test_cases: List[Dict[str, Any]],
                           benchmark_name: str, category: str = "default") -> List[BenchmarkResult]:
        """
        Run benchmark tests on brain
        
        Args:
            brain: NLM brain instance
            test_cases: List of test case configurations
            benchmark_name: Name of the benchmark suite
            category: Test category
            
        Returns:
            List of benchmark results
        """
        suite_results = []
        
        for i, test_case in enumerate(test_cases):
            test_name = test_case.get("name", f"Test_{i}")
            steps = test_case.get("steps", 1000)
            verbose = test_case.get("verbose", False)
            input_pattern = test_case.get("input_pattern")
            
            if verbose:
                print(f"Running benchmark: {benchmark_name} - {test_name}")
            
            try:
                # Start timing
                start_time = time.time()
                process = psutil.Process()
                initial_memory = process.memory_info().rss / 1024 / 1024
                
                # Run test
                brain.reset()
                brain.initialize()
                
                spike_count = brain.getTotalSpikeCount()
                
                for step in range(steps):
                    if input_pattern:
                        input_data = input_pattern(step)
                        brain.inject_input(input_data, step * 0.001)
                    
                    brain.step(step * 0.001)
                    spike_count = brain.getTotalSpikeCount()
                
                # Calculate metrics
                end_time = time.time()
                final_memory = process.memory_info().rss / 1024 / 1024
                
                execution_time = end_time - start_time
                
                # Create benchmark result
                result = BenchmarkResult(
                    test_name=test_name,
                    test_category=category,
                    configuration=test_case,
                    metrics={
                        "performance": [
                            PerformanceSample(
                                timestamp=datetime.now(),
                                metric_type=PerformanceMetric.SIMULATION_SPEED,
                                value=steps / execution_time if execution_time > 0 else 0,
                                unit="steps/second"
                            ),
                            PerformanceSample(
                                timestamp=datetime.now(),
                                metric_type=PerformanceMetric.MEMORY_USAGE,
                                value=final_memory,
                                unit="MB"
                            )
                        ]
                    },
                    execution_time=execution_time,
                    memory_peak=final_memory - initial_memory,
                    cpu_peak=0.0,  # Would need continuous monitoring
                    success=True
                )
                
                suite_results.append(result)
                self.results.append(result)
                
                if verbose:
                    print(f"  Completed in {execution_time:.4f}s, Spike rate: {spike_count} spikes")
                
            except Exception as e:
                # Record failed test
                result = BenchmarkResult(
                    test_name=test_name,
                    test_category=category,
                    configuration=test_case,
                    metrics={},
                    execution_time=0.0,
                    memory_peak=0.0,
                    cpu_peak=0.0,
                    success=False,
                    error_message=str(e)
                )
                
                suite_results.append(result)
                self.results.append(result)
                
                if verbose:
                    print(f"  Failed: {e}")
        
        return suite_results
    
    def run_comparison_benchmark(self, brain_configs: List[Dict[str, Any]],
                                test_case: Dict[str, Any], benchmark_name: str) -> Dict[str, Any]:
        """
        Run comparison benchmark across different brain configurations
        
        Args:
            brain_configs: List of brain configuration dictionaries
            test_case: Test case configuration
            benchmark_name: Name of the benchmark suite
            
        Returns:
            Dictionary with comparison results
        """
        comparison_results = {
            "benchmark_name": benchmark_name,
            "test_case": test_case,
            "configurations": {},
            "summary": {}
        }
        
        for i, config in enumerate(brain_configs):
            config_name = config.get("name", f"Config_{i}")
            
            try:
                # Create brain with specific configuration
                brain = create_brain(**config)
                
                # Run benchmark
                steps = test_case.get("steps", 1000)
                start_time = time.time()
                
                brain.reset()
                brain.initialize()
                
                for step in range(steps):
                    input_data = test_case.get("input_pattern", lambda s: {})()
                    brain.inject_input(input_data, step * 0.001)
                    brain.step(step * 0.001)
                
                end_time = time.time()
                execution_time = end_time - start_time
                
                config_result = {
                    "execution_time": execution_time,
                    "steps_per_second": steps / execution_time if execution_time > 0 else 0,
                    "final_spike_count": brain.getTotalSpikeCount(),
                    "success": True
                }
                
                comparison_results["configurations"][config_name] = config_result
                
            except Exception as e:
                comparison_results["configurations"][config_name] = {
                    "execution_time": 0.0,
                    "steps_per_second": 0.0,
                    "final_spike_count": 0,
                    "success": False,
                    "error": str(e)
                }
        
        # Generate summary
        successful_configs = {k: v for k, v in comparison_results["configurations"].items() if v["success"]}
        
        if successful_configs:
            execution_times = [v["execution_time"] for v in successful_configs.values()]
            steps_per_second = [v["steps_per_second"] for v in successful_configs.values()]
            
            comparison_results["summary"] = {
                "total_configurations": len(brain_configs),
                "successful_configurations": len(successful_configs),
                "fastest_configuration": min(execution_times),
                "slowest_configuration": max(execution_times),
                "mean_execution_time": np.mean(execution_times) if execution_times else 0,
                "std_execution_time": np.std(execution_times) if len(execution_times) > 1 else 0,
                "best_steps_per_second": max(steps_per_second) if steps_per_second else 0,
                "average_steps_per_second": np.mean(steps_per_second) if steps_per_second else 0,
                "performance_improvement_factor": max(steps_per_second) / min(steps_per_second) if min(steps_per_second) > 0 else float('inf')
            }
        
        return comparison_results
    
    def export_results(self, filepath: str):
        """Export all benchmark results to JSON file"""
        data = {
            "benchmark_name": "NLM Benchmark Suite",
            "export_timestamp": datetime.now().isoformat(),
            "results": [vars(result) for result in self.results]
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
    
    def generate_report(self) -> str:
        """Generate human-readable benchmark report"""
        if not self.results:
            return "No benchmark results available"
        
        report = []
        report.append("NLM Benchmark Suite Report")
        report.append("=" * 50)
        report.append(f"Total Tests: {len(self.results)}")
        
        successful_results = [r for r in self.results if r.success]
        failed_results = [r for r in self.results if not r.success]
        
        report.append(f"Successful Tests: {len(successful_results)}")
        report.append(f"Failed Tests: {len(failed_results)}")
        
        if successful_results:
            avg_time = np.mean([r.execution_time for r in successful_results])
            report.append(f"Average Execution Time: {avg_time:.4f}s")
            
            # Add detailed results
            report.append("\nDetailed Results:")
            report.append("-" * 50)
            
            for result in successful_results:
                report.append(f"Test: {result.test_name} ({result.test_category})")
                report.append(f"  Execution Time: {result.execution_time:.4f}s")
                report.append(f"  Success: Yes")
                report.append("")
        
        if failed_results:
            report.append("\nFailed Tests:")
            report.append("-" * 50)
            
            for result in failed_results:
                report.append(f"Test: {result.test_name}")
                report.append(f"  Error: {result.error_message}")
                report.append("")
        
        return "\n".join(report)

# Convenience functions for common benchmarking scenarios

def create_default_brain_configs() -> List[Dict[str, Any]]:
    """Create default set of brain configurations for benchmarking"""
    return [
        {
            "name": "Small_Brain",
            "neuron_count": 100,
            "region_count": 1,
            "connection_probability": 0.1
        },
        {
            "name": "Medium_Brain",
            "neuron_count": 500,
            "region_count": 2,
            "connection_probability": 0.15
        },
        {
            "name": "Large_Brain",
            "neuron_count": 1000,
            "region_count": 4,
            "connection_probability": 0.2
        }
    ]


def create_test_scenarios() -> List[Dict[str, Any]]:
    """Create common test scenarios for benchmarking"""
    return [
        {
            "name": "Random_Input",
            "steps": 1000,
            "input_pattern": lambda step: {
                "vision": [np.random.random() for _ in range(256)],
                "audio": [np.random.random() for _ in range(100)]
            }
        },
        {
            "name": "Structured_Input",
            "steps": 1000,
            "input_pattern": lambda step: {
                "vision": [np.sin(step * 0.1) for _ in range(256)],
                "audio": [np.cos(step * 0.05) for _ in range(100)]
            }
        },
        {
            "name": "Sparse_Input",
            "steps": 1000,
            "input_pattern": lambda step: {
                "vision": [0.0] * 256,
                "audio": [0.0] * 100
            }
        }
    ]

# Main execution
if __name__ == "__main__":
    print("NLM Performance Monitoring and Benchmarking Tools")
    print("=" * 60)
    print("This module provides comprehensive performance monitoring")
    print("and benchmarking tools for NLM neural simulation framework.")
    print("=" * 60)
    
    # Create benchmark suite
    benchmark_suite = BenchmarkSuite()
    
    # Create brain and test scenarios
    brain = create_brain(neuron_count=200)
    test_scenarios = create_test_scenarios()
    
    print("Running benchmark suite...")
    print(f"Testing with {brain.brain.getTotalNeuronCount()} neurons")
    print(f"Test scenarios: {[s['name'] for s in test_scenarios]}")
    print()
    
    # Run benchmarks
    for scenario in test_scenarios:
        print(f"Benchmarking scenario: {scenario['name']}")
        results = benchmark_suite.run_brain_benchmark(
            brain, [scenario], 
            benchmark_name="NLM Performance Tests",
            category=scenario['name']
        )
        
        for result in results:
            if result.success:
                print(f"  ✓ {result.test_name}: {result.execution_time:.4f}s")
            else:
                print(f"  ✗ {result.test_name}: {result.error_message}")
        
        print()
    
    # Generate report
    report = benchmark_suite.generate_report()
    print(report)
    
    # Export results
    benchmark_suite.export_results("nlm_benchmark_results.json")
    print(f"\nResults exported to: nlm_benchmark_results.json")
