#!/usr/bin/env python3
"""
Performance Benchmark Example
=============================

This example demonstrates performance testing with new NLM Python API methods.
It showcases large network simulation, memory and CPU profiling, and performance
validation using the new API capabilities.

Key Features Demonstrated:
- Performance testing with new API methods
- Configuration performance validation
- Large network simulation
- Memory and CPU profiling

File: examples/performance_benchmark.py
"""

import time
import sys
import os
import tracemalloc
import gc
from datetime import datetime

# Add the python bindings directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

try:
    import pynlm as nlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Error: pynlm module not available: {e}")
    sys.exit(1)

class PerformanceProfiler:
    """Comprehensive performance profiling for NLM simulations."""
    
    def __init__(self):
        self.benchmark_results = {}
        self.memory_samples = []
        self.timing_samples = []
        self.cpu_samples = []
    
    def start_memory_tracing(self):
        """Start memory tracing for performance measurements."""
        tracemalloc.start()
        gc.collect()
        return tracemalloc.get_traced_memory()
    
    def stop_memory_tracing(self, start_memory):
        """
        Stop memory tracing and record results.
        
        Args:
            start_memory: Initial memory snapshot from start_memory_tracing()
            
        Returns:
            dict: Memory usage statistics
        """
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        memory_stats = {
            "start_memory": start_memory,
            "current_memory": current,
            "peak_memory": peak,
            "memory_delta": current - start_memory[0],
            "memory_peak_delta": peak - start_memory[0]
        }
        
        self.memory_samples.append(memory_stats)
        return memory_stats
    
    def record_timing(self, operation_name: str, start_time: float, end_time: float):
        """
        Record timing measurements for an operation.
        
        Args:
            operation_name: Name of the operation
            start_time: Start time (from time.time())
            end_time: End time (from time.time())
        """
        duration = end_time - start_time
        timing_stats = {
            "operation": operation_name,
            "start_time": start_time,
            "end_time": end_time,
            "duration": duration,
            "timestamp": datetime.now().isoformat()
        }
        
        self.timing_samples.append(timing_stats)
        
        if operation_name not in self.benchmark_results:
            self.benchmark_results[operation_name] = {
                "total_duration": 0.0,
                "sample_count": 0,
                "min_duration": float('inf'),
                "max_duration": 0.0
            }
        
        op_result = self.benchmark_results[operation_name]
        op_result["total_duration"] += duration
        op_result["sample_count"] += 1
        op_result["min_duration"] = min(op_result["min_duration"], duration)
        op_result["max_duration"] = max(op_result["max_duration"], duration)
    
    def get_benchmark_summary(self) -> dict:
        """
        Get comprehensive benchmark summary.
        
        Returns:
            dict: Performance benchmark results
        """
        summary = {
            "total_benchmarks": len(self.benchmark_results),
            "total_operations": len(self.timing_samples),
            "total_memory_samples": len(self.memory_samples),
            "overall_stats": {}
        }
        
        # Calculate overall performance statistics
        total_time = sum(op["total_duration"] for op in self.benchmark_results.values())
        if total_time > 0:
            summary["overall_stats"]["total_duration_seconds"] = total_time
            summary["overall_stats"]["operations_per_second"] = (
                len(self.timing_samples) / total_time if total_time > 0 else 0
            )
        
        # Get best and worst performing operations
        if self.benchmark_results:
            best_op = min(self.benchmark_results.items(), 
                         key=lambda x: x[1]["min_duration"])
            worst_op = max(self.benchmark_results.items(),
                          key=lambda x: x[1]["max_duration"])
            
            summary["overall_stats"]["best_operation"] = {
                "name": best_op[0],
                "min_duration": best_op[1]["min_duration"]
            }
            summary["overall_stats"]["worst_operation"] = {
                "name": worst_op[0],
                "max_duration": worst_op[1]["max_duration"]
            }
        
        return summary
    
    def print_benchmark_report(self):
        """Print formatted benchmark report."""
        print("\n=== Performance Benchmark Report ===")
        print(f"Total operations benchmarked: {len(self.timing_samples)}")
        print(f"Total memory measurements: {len(self.memory_samples)}")
        print(f"Unique operations: {len(self.benchmark_results)}")
        
        if self.benchmark_results:
            print("\nOperation Performance:")
            for op_name, stats in self.benchmark_results.items():
                if stats["sample_count"] > 0:
                    avg_duration = stats["total_duration"] / stats["sample_count"]
                    print(f"  {op_name}:")
                    print(f"    Samples: {stats['sample_count']}")
                    print(f"    Total time: {stats['total_duration']:.4f}s")
                    print(f"    Average time: {avg_duration:.6f}s")
                    print(f"    Min time: {stats['min_duration']:.6f}s")
                    print(f"    Max time: {stats['max_duration']:.6f}s")
        
        if self.memory_samples:
            print("\nMemory Usage:")
            for i, mem_stats in enumerate(self.memory_samples):
                print(f"  Measurement {i + 1}:")
                print(f"    Memory delta: {mem_stats['memory_delta'] / 1024 / 1024:.2f} MB")
                print(f"    Peak delta: {mem_stats['memory_peak_delta'] / 1024 / 1024:.2f} MB")
        
        # Print overall summary
        summary = self.get_benchmark_summary()
        if "overall_stats" in summary:
            print(f"\nOverall Performance:")
            print(f"  Total duration: {summary['overall_stats'].get('total_duration_seconds', 0):.4f}s")
            if "operations_per_second" in summary["overall_stats"]:
                ops_per_sec = summary["overall_stats"]["operations_per_second"]
                print(f"  Operations per second: {ops_per_sec:.2f}")
            
            if "best_operation" in summary["overall_stats"]:
                best = summary["overall_stats"]["best_operation"]
                print(f"  Fastest operation: {best['name']} ({best['min_duration']:.6f}s)")
            
            if "worst_operation" in summary["overall_stats"]:
                worst = summary["overall_stats"]["worst_operation"]
                print(f"  Slowest operation: {worst['name']} ({worst['max_duration']:.6f}s)")

class BrainSizeValidator:
    """Validates brain configurations for performance testing."""
    
    @staticmethod
    def get_recommended_config_for_neuron_count(neuron_count: int) -> dict:
        """
        Get recommended configuration parameters for a given neuron count.
        
        Args:
            neuron_count: Number of neurons in the brain
            
        Returns:
            dict: Recommended configuration
        """
        if neuron_count <= 1000:
            return {
                "region_count": 1,
                "connection_probability": 0.05,
                "synapse_density": 0.1,
                "expected_syns_per_step": 50
            }
        elif neuron_count <= 5000:
            return {
                "region_count": 2,
                "connection_probability": 0.02,
                "synapse_density": 0.05,
                "expected_syns_per_step": 100
            }
        elif neuron_count <= 20000:
            return {
                "region_count": 4,
                "connection_probability": 0.01,
                "synapse_density": 0.02,
                "expected_syns_per_step": 200
            }
        else:
            return {
                "region_count": 8,
                "connection_probability": 0.005,
                "synapse_density": 0.01,
                "expected_syns_per_step": 400
            }
    
    @staticmethod
    def validate_performance_config(config) -> tuple:
        """
        Validate configuration for performance testing.
        
        Args:
            config: Configuration to validate
            
        Returns:
            tuple: (is_valid, issues_list)
        """
        issues = []
        
        # Check neuron count is reasonable
        if config.has("brain.neuron_count"):
            neuron_count = config.get("brain.neuron_count")
            if neuron_count < 100:
                issues.append("Neuron count too small for meaningful performance testing")
            elif neuron_count > 100000:
                issues.append("Neuron count too large - may exceed memory limits")
        
        # Check connection probability is reasonable
        if config.has("brain.connection_probability"):
            prob = config.get("brain.connection_probability")
            if prob <= 0.0:
                issues.append("Connection probability must be positive")
            elif prob >= 1.0:
                issues.append("Connection probability too high - may create too many synapses")
        
        return len(issues) == 0, issues

class LargeNetworkSimulator:
    """Simulates large neural networks for performance testing."""
    
    def __init__(self, profiler=None):
        self.profiler = profiler or PerformanceProfiler()
    
    def run_large_network_simulation(self, neuron_count: int, num_steps: int,
                                     validation: bool = True) -> dict:
        """
        Run a large network simulation with performance profiling.
        
        Args:
            neuron_count: Number of neurons in the network
            num_steps: Number of simulation steps to run
            validation: Whether to validate configuration
            
        Returns:
            dict: Simulation results and performance metrics
        """
        print(f"\n=== Large Network Performance Test ===")
        print(f"Neuron count: {neuron_count:,}")
        print(f"Simulation steps: {num_steps:,}")
        
        # Create configuration for performance testing
        config = nlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.connection_probability", 0.01)
        
        # Get recommendations for this network size
        recommendations = BrainSizeValidator.get_recommended_config_for_neuron_count(neuron_count)
        for key, value in recommendations.items():
            config.set(f"brain.{key}", value)
        
        # Validate configuration
        if validation:
            is_valid, issues = BrainSizeValidator.validate_performance_config(config)
            if not is_valid:
                print("Configuration validation issues:")
                for issue in issues:
                    print(f"  - {issue}")
            else:
                print("✓ Configuration validated successfully")
        
        # Create brain
        print("Creating brain...")
        start_time = time.time()
        brain = nlm.createBrain(config)
        brain_creation_time = time.time() - start_time
        self.profiler.record_timing("brain_creation", start_time, time.time())
        
        brain.initialize()
        print(f"Brain initialized with {brain.getTotalNeuronCount():,} neurons")
        print(f"Brain has {brain.getTotalSynapseCount():,} synapses")
        
        # Start memory profiling
        memory_start = self.profiler.start_memory_tracing()
        
        # Run simulation
        print(f"\nRunning simulation ({num_steps:,} steps)...")
        sim_start = time.time()
        
        for step in range(num_steps):
            step_start = time.time()
            
            # Perform brain step
            brain.step(step, time.time())
            
            # Collect performance metrics periodically
            if step % 100 == 0 and step > 0:
                step_duration = time.time() - step_start
                neurons_firing = brain.getFiringNeuronCount()
                synapses_count = brain.getTotalSynapseCount()
                
                print(f"  Step {step:,}/{num_steps:,}: "
                      f"{step_duration*1000:.2f}ms, "
                      f"Firing: {neurons_firing:,}, "
                      f"Total synapses: {synapses_count:,}")
                
                # Record performance metrics
                self.profiler.record_timing(
                    "brain_step", step_start, time.time()
                )
        
        sim_duration = time.time() - sim_start
        self.profiler.record_timing("simulation", sim_start, time.time())
        
        # Stop memory profiling
        memory_stats = self.profiler.stop_memory_tracing(memory_start)
        
        # Final performance metrics
        final_neurons_firing = brain.getFiringNeuronCount()
        total_synapses = brain.getTotalSynapseCount()
        avg_spike_count = brain.getTotalSpikeCount()
        
        print(f"\n=== Simulation Complete ===")
        print(f"Simulation duration: {sim_duration:.2f}s")
        print(f"Average step time: {sim_duration/num_steps*1000:.2f}ms")
        print(f"Steps per second: {num_steps/sim_duration:.2f}")
        
        # Results
        results = {
            "config": {
                "neuron_count": neuron_count,
                "connection_probability": 0.01,
                "expected_syns_per_step": recommendations["expected_syns_per_step"]
            },
            "performance": {
                "total_duration": sim_duration,
                "steps_per_second": num_steps / sim_duration,
                "avg_step_time_ms": sim_duration / num_steps * 1000,
                "brain_creation_time": brain_creation_time
            },
            "neural_metrics": {
                "final_firing_neurons": final_neurons_firing,
                "total_synapses": total_synapses,
                "total_spikes": avg_spike_count,
                "average_firing_rate": brain.getAverageFiringRate()
            },
            "memory": memory_stats,
            "profiling_data": self.profiler.benchmark_results
        }
        
        return results

def print_performance_summary(results: dict):
    """
    Print formatted performance summary.
    
    Args:
        results: Performance results dictionary
    """
    print("\n=== Performance Benchmark Summary ===")
    
    config = results["config"]
    perf = results["performance"]
    neural = results["neural_metrics"]
    memory = results["memory"]
    
    print(f"\nConfiguration:")
    print(f"  Neuron count: {config['neuron_count']:,}")
    print(f"  Connection probability: {config['connection_probability']}")
    print(f"  Expected synapses per step: {config['expected_syns_per_step']:,}")
    
    print(f"\nPerformance Metrics:")
    print(f"  Total duration: {perf['total_duration']:.2f}s")
    print(f"  Steps per second: {perf['steps_per_second']:.2f}")
    print(f"  Average step time: {perf['avg_step_time_ms']:.2f}ms")
    print(f"  Brain creation time: {perf['brain_creation_time']:.3f}s")
    
    print(f"\nNeural Network Metrics:")
    print(f"  Final firing neurons: {neural['final_firing_neurons']:,}")
    print(f"  Total synapses: {neural['total_synapses']:,}")
    print(f"  Total spikes: {neural['total_spikes']:,}")
    print(f"  Average firing rate: {neural['average_firing_rate']:.4f} Hz")
    
    print(f"\nMemory Usage:")
    delta_mb = memory['memory_delta'] / 1024 / 1024
    peak_delta_mb = memory['memory_peak_delta'] / 1024 / 1024
    print(f"  Memory delta: {delta_mb:.2f} MB")
    print(f"  Peak memory delta: {peak_delta_mb:.2f} MB")
    
    # Performance assessment
    print(f"\nPerformance Assessment:")
    if perf['steps_per_second'] > 100:
        print(f"  ✓ Excellent performance (>100 steps/sec)")
    elif perf['steps_per_second'] > 50:
        print(f"  ✓ Good performance (>50 steps/sec)")
    elif perf['steps_per_second'] > 10:
        print(f"  ⚠ Moderate performance (10-50 steps/sec)")
    else:
        print(f"  ⚠ Slow performance (<10 steps/sec)")
    
    if delta_mb < 100:
        print(f"  ✓ Low memory usage (<100 MB)")
    elif delta_mb < 500:
        print(f"  ⚠ Moderate memory usage (100-500 MB)")
    else:
        print(f"  ⚠ High memory usage (>500 MB)")


def run_performance_benchmark():
    """Run comprehensive performance benchmarks."""
    print("=== NLM Performance Benchmark Example ===")
    print(f"Started: {datetime.now()}")
    print()
    
    # Initialize profiler
    profiler = PerformanceProfiler()
    
    # Test with different network sizes
    test_cases = [
        {"neurons": 1000, "steps": 500, "description": "Small network"},
        {"neurons": 5000, "steps": 1000, "description": "Medium network"},
        {"neurons": 10000, "steps": 1500, "description": "Large network"}
    ]
    
    simulator = LargeNetworkSimulator(profiler)
    all_results = []
    
    for test_case in test_cases:
        print(f"\n{'='*60}")
        neurons = test_case["neurons"]
        steps = test_case["steps"]
        desc = test_case["description"]
        
        print(f"Running: {desc}")
        print(f"  Neurons: {neurons:,}")
        print(f"  Steps: {steps:,}")
        
        # Run simulation
        results = simulator.run_large_network_simulation(
            neuron_count=neurons,
            num_steps=steps,
            validation=True
        )
        
        # Print summary for this run
        print_performance_summary(results)
        all_results.append(results)
    
    # Print comprehensive profiler report
    print(f"\n{'='*60}")
    profiler.print_benchmark_report()
    
    # Generate comparison summary
    print(f"\n{'='*60}")
    print("=== Cross-Network Comparison ===")
    
    for i, result in enumerate(all_results):
        neurons = result["config"]["neuron_count"]
        steps_per_sec = result["performance"]["steps_per_second"]
        memory_mb = result["memory"]["memory_delta"] / 1024 / 1024
        
        print(f"Network {i + 1}: {neurons:,} neurons, "
              f"{steps_per_sec:.2f} steps/sec, {memory_mb:.2f} MB")

if __name__ == "__main__":
    run_performance_benchmark()
