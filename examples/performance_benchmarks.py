"""
Performance Benchmarking Examples for NLM

This module provides comprehensive performance benchmarking tools for NLM simulations.
Key features include:
- Memory usage profiling
- Execution time measurement
- Scalability analysis
- Performance comparison
- Optimization recommendations

Author: NLM Project
License: MIT
Version: 1.0
"""

import pynlm
import time
import tracemalloc
import numpy as np
from typing import Dict, List, Any, Optional
import json
import matplotlib.pyplot as plt
class PerformanceProfiler:
    """
    Performance profiler for NLM simulations.
    
    This profiler measures:
    - Memory usage and allocation patterns
    - Execution time for different operations
    - CPU utilization
    - I/O operations
    - Network traffic (if applicable)
    """
    
    def __init__(self):
        self.metrics = {
            'memory': {},
            'time': {},
            'cpu': {},
            'io': {},
            'network': {}
        }
        self.start_time = None
        self.end_time = None
        self.peak_memory = 0
        self.current_memory = 0
        
    def startProfiling(self):
        """Start profiling session."""
        tracemalloc.start()
        self.start_time = time.time()
        self.peak_memory = 0
        self.current_memory = 0
        
    def stopProfiling(self):
        """Stop profiling session."""
        if self.start_time:
            self.end_time = time.time()
            current, self.peak_memory = tracemalloc.get_traced_memory()
            tracemalloc.stop()
            
    def recordMemoryUsage(self, label: str):
        """Record memory usage at current point."""
        current, peak = tracemalloc.get_traced_memory()
        self.metrics['memory'][label] = {
            'current_mb': current / 1024 / 1024,
            'peak_mb': peak / 1024 / 1024,
            'timestamp': time.time()
        }
        
    def recordTimeMeasurement(self, label: str, duration: float):
        """Record time measurement."""
        self.metrics['time'][label] = {
            'duration': duration,
            'timestamp': time.time()
        }
        
    def recordCPUTime(self, label: str, cpu_time: float):
        """Record CPU time measurement."""
        self.metrics['cpu'][label] = {
            'cpu_time': cpu_time,
            'wall_time': time.time() - self.start_time if self.start_time else 0,
            'timestamp': time.time()
        }
        
    def getSummary(self) -> Dict[str, Any]:
        """Get profiling summary."""
        return {
            'total_duration': self.end_time - self.start_time if self.start_time and self.end_time else 0,
            'peak_memory_mb': self.peak_memory / 1024 / 1024,
            'memory_measurements': len(self.metrics['memory']),
            'time_measurements': len(self.metrics['time']),
            'cpu_measurements': len(self.metrics['cpu'])
        }
class SpeedBenchmark:
    """
    Speed benchmarking for different simulation configurations.
    
    This class benchmarks:
    - Simulation speed for different brain sizes
    - Performance impact of different parameters
    - Speed comparison across different algorithms
    """
    
    def __init__(self):
        self.results = []
        
    def benchmarkConfiguration(self, name: str, config: pynlm.Config, 
                              steps: int = 100, time_step: float = 0.01):
        """
        Benchmark a specific configuration.
        
        Args:
            name: Configuration name
            config: Brain configuration
            steps: Number of simulation steps
            time_step: Simulation time step
        """
        print(f"Benchmarking {name}...")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Start profiling
        profiler = PerformanceProfiler()
        profiler.startProfiling()
        profiler.recordMemoryUsage('initialization')
        
        # Run simulation
        simulation_start = time.time()
        for step in range(steps):
            brain.step(step, time_step)
            if step % 50 == 0:
                profiler.recordMemoryUsage(f'step_{step}')
                
        simulation_end = time.time()
        profiler.stopProfiling()
        
        # Calculate metrics
        total_time = simulation_end - simulation_start
        avg_time_per_step = total_time / steps
        
        result = {
            'name': name,
            'config': config,
            'steps': steps,
            'time_step': time_step,
            'total_time': total_time,
            'avg_time_per_step_ms': avg_time_per_step * 1000,
            'steps_per_second': steps / total_time,
            'profiler_metrics': profiler.getSummary(),
            'final_firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount()
        }
        
        self.results.append(result)
        return result
        
    def benchmarkScale(self):
        """Benchmark different scales of simulation."""
        scales = [
            {'neurons': 500, 'density': 0.05, 'name': 'Small'},
            {'neurons': 1000, 'density': 0.1, 'name': 'Medium'},
            {'neurons': 2000, 'density': 0.15, 'name': 'Large'},
            {'neurons': 5000, 'density': 0.2, 'name': 'Very Large'}
        ]
        
        print("Running scale benchmark...")
        print(f"{'Scale':<10} {'Neurons':<10} {'Density':<10} {'Time/Step (ms)':<15} {'Steps/sec':<10}")
        print("-" * 60)
        
        for scale in scales:
            config = pynlm.createDemoConfig(scale['neurons'], scale['density'])
            result = self.benchmarkConfiguration(
                name=scale['name'],
                config=config,
                steps=50,
                time_step=0.01
            )
            
            print(f"{scale['name']:<10} {scale['neurons']:<10} {scale['density']:<10} "
                  f"{result['avg_time_per_step_ms']:<15.3f} {result['steps_per_second']:<10.1f}")
            
    def benchmarkPlasticity(self):
        """Benchmark plasticity computation performance."""
        print("Benchmarking plasticity computation...")
        
        # Create brain with plasticity enabled
        config = pynlm.createDemoConfig(1000, 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Enable plasticity on all synapses
        for region in brain.getRegions():
            for syn in region->getSynapses():
                syn->enablePlasticity(True, True, False)
        
        # Benchmark plasticity computation
        profiler = PerformanceProfiler()
        profiler.startProfiling()
        
        for step in range(200):
            brain.step(step)
            if step % 50 == 0:
                # Force plasticity computation
                if brain.getSTDP():
                    brain.getSTDP()->update(None, {}, {}, 0.001)
                    
        profiler.stopProfiling()
        
        result = {
            'name': 'Plasticity Benchmark',
            'steps': 200,
            'total_time': profiler.getSummary()['total_duration'],
            'avg_time_per_step_ms': profiler.getSummary()['total_duration'] * 1000 / 200,
            'peak_memory_mb': profiler.getSummary()['peak_memory_mb'],
            'spike_rate': brain.getTotalSpikeCount() / 200
        }
        
        self.results.append(result)
        return result
        
    def generateReport(self) -> str:
        """Generate benchmark report."""
        if not self.results:
            return "No benchmark results available"
        
        report = "=== Performance Benchmark Report ===\n\n"
        
        for result in self.results:
            report += f"Configuration: {result['name']}\n"
            report += f"  Neurons: {result['config'].getOr<int64_t>('neuron_count', 1000)}\n"
            report += f"  Connection Density: {result['config'].getOr<float>('connection_probability', 0.1):.3f}\n"
            report += f"  Total Time: {result['total_time']:.3f}s\n"
            report += f"  Average Time per Step: {result['avg_time_per_step_ms']:.3f}ms\n"
            report += f"  Steps per Second: {result['steps_per_second']:.1f}\n"
            report += f"  Peak Memory: {result['profiler_metrics']['peak_memory_mb']:.2f} MB\n"
            report += f"  Final Firing Neurons: {result['final_firing_neurons']}\n"
            report += f"  Total Spikes: {result['total_spikes']}\n\n"
        
        return report
class MemoryBenchmark:
    """
    Memory benchmarking for different simulation scenarios.
    
    This class benchmarks:
    - Memory usage during different operations
    - Memory allocation patterns
    - Memory efficiency
    - Garbage collection effects
    """
    
    def __init__(self):
        self.memory_snapshots = []
        
    def takeMemorySnapshot(self, label: str):
        """Take memory snapshot."""
        tracemalloc.start()
        
        # Force garbage collection
        import gc
        gc.collect()
        
        current, peak = tracemalloc.get_traced_memory()
        
        self.memory_snapshots.append({
            'label': label,
            'current_mb': current / 1024 / 1024,
            'peak_mb': peak / 1024 / 1024,
            'timestamp': time.time()
        })
        
        tracemalloc.stop()
        
    def benchmarkMemoryUsage(self):
        """Benchmark memory usage patterns."""
        print("Benchmarking memory usage patterns...")
        
        configs = [
            {'neurons': 500, 'name': 'Small Brain'},
            {'neurons': 1000, 'name': 'Medium Brain'},
            {'neurons': 2000, 'name': 'Large Brain'}
        ]
        
        for config_spec in configs:
            print(f"\n--- {config_spec['name']} ---")
            
            # Create brain
            config = pynlm.createDemoConfig(config_spec['neurons'], 0.1)
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Take snapshots at different points
            self.takeMemorySnapshot('initialization')
            
            # Run simulation
            for step in range(100):
                brain.step(step)
                if step % 50 == 0:
                    self.takeMemorySnapshot(f'step_{step}')
                    
                # Force garbage collection occasionally
                if step % 25 == 0:
                    import gc
                    gc.collect()
                    self.takeMemorySnapshot(f'gc_step_{step}')
            
            # Display memory usage
            print(f"Memory usage pattern for {config_spec['name']}:")
            for snapshot in self.memory_snapshots:
                if config_spec['name'] in snapshot.get('label', ''):
                    print(f"  {snapshot['label']}: {snapshot['peak_mb']:.2f} MB")
            
            self.memory_snapshots.clear()
class ComprehensiveBenchmark:
    """
    Comprehensive benchmarking suite for NLM.
    
    This suite runs:
    - Speed benchmarks
    - Memory benchmarks
    - Scalability tests
    - Performance optimization analysis
    """
    
    def __init__(self):
        self.speed_benchmark = SpeedBenchmark()
        self.memory_benchmark = MemoryBenchmark()
        
    def runFullBenchmark(self):
        """Run comprehensive benchmark suite."""
        print("=== Comprehensive NLM Performance Benchmark ===\n")
        
        # Run speed benchmarks
        print("1. Speed Benchmarks")
        print("-" * 50)
        self.speed_benchmark.benchmarkScale()
        self.speed_benchmark.benchmarkPlasticity()
        
        # Generate speed report
        speed_report = self.speed_benchmark.generateReport()
        print(speed_report)
        
        # Run memory benchmarks
        print("2. Memory Benchmarks")
        print("-" * 50)
        self.memory_benchmark.benchmarkMemoryUsage()
        
        # Generate comparison report
        self.generateComparisonReport()
        
    def generateComparisonReport(self):
        """Generate comparison report across different benchmarks."""
        print("\n=== Performance Comparison Report ===")
        
        if not self.speed_benchmark.results:
            print("No benchmark data available for comparison")
            return
        
        # Create comparison table
        print(f"{'Configuration':<15} {'Neurons':<10} {'Time/Step (ms)':<15} {'Speed (steps/sec)':<18} {'Memory (MB)':<12}")
        print("-" * 85)
        
        for result in self.speed_benchmark.results:
            neurons = result['config'].getOr<int64_t>('neuron_count', 1000)
            time_per_step = result['avg_time_per_step_ms']
            speed = result['steps_per_second']
            memory = result['profiler_metrics']['peak_memory_mb']
            
            print(f"{result['name']:<15} {neurons:<10} {time_per_step:<15.3f} {speed:<18.1f} {memory:<12.2f}")
            
        # Analyze scaling
        print(f"\n=== Scaling Analysis ===")
        neurons = [r['config'].getOr<int64_t>('neuron_count', 1000) for r in self.speed_benchmark.results]
        times = [r['avg_time_per_step_ms'] for r in self.speed_benchmark.results]
        
        # Calculate scaling exponent
        if len(neurons) > 1:
            log_neurons = np.log10(neurons)
            log_times = np.log10(times)
            
            # Linear regression for scaling exponent
            coefficients = np.polyfit(log_neurons, log_times, 1)
            scaling_exponent = coefficients[0]
            
            print(f"Time complexity scaling exponent: {scaling_exponent:.2f}")
            print(f"Time complexity: O(N^{scaling_exponent:.2f})")
            
            if scaling_exponent < 1.0:
                print("✓ Good scaling performance")
            elif scaling_exponent < 2.0:
                print("⚠ Moderate scaling performance")
            else:
                print("✗ Poor scaling performance")
            
            # Predict for larger scale
            next_neuron_scale = 10000
            log_pred = np.polyval(coefficients, np.log10(next_neuron_scale))
            pred_time = 10 ** log_pred
            
            print(f"\nPrediction for {next_neuron_scale} neurons:")
            print(f"Estimated time per step: {pred_time:.2f} ms")
            print(f"Estimated simulation time for 100 steps: {pred_time * 100 / 1000:.2f} s")
def runPerformanceBenchmarkExample():
    """Run the performance benchmarking example."""
    print("=== Performance Benchmarking Example ===\n")
    
    # Create comprehensive benchmark suite
    benchmark = ComprehensiveBenchmark()
    
    # Run full benchmark suite
    benchmark.runFullBenchmark()
    
    print("\n=== Performance Benchmarking Complete ===")
if __name__ == "__main__":
    runPerformanceBenchmarkExample()