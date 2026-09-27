"""
Performance Benchmarking Examples for NLM

This module provides examples of using the NLM performance monitoring
and optimization tools for benchmarking neural simulation performance.
"""

import time
import sys
import os
from pathlib import Path
from typing import Dict, List, Optional, Callable
import json
import csv
import threading
from datetime import datetime

# Add the parent directory to the path so we can import NLM modules
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    from nlm.performance import (
        PerformanceMonitor, MemoryTracker, CPUMonitor, StepProfiler,
        ResourceOptimizer, PerformanceReporter, PerformanceProfiler
    )
    NLM_AVAILABLE = True
except ImportError:
    NLM_AVAILABLE = False
    print("NLM modules not available. Running in demonstration mode.")

class PerformanceBenchmark:
    """Performance benchmarking class for NLM simulations."""
    
    def __init__(self, benchmark_name: str, description: str = ""):
        self.benchmark_name = benchmark_name
        self.description = description
        self.start_time = None
        self.end_time = None
        self.metrics = {}
        self.step_count = 0
        
        # Initialize NLM performance monitoring components if available
        if NLM_AVAILABLE:
            self.performance_monitor = PerformanceMonitor()
            self.memory_tracker = MemoryTracker()
            self.cpu_monitor = CPUMonitor()
            self.step_profiler = StepProfiler()
            self.resource_optimizer = ResourceOptimizer()
            self.performance_reporter = PerformanceReporter()
            self.performance_profiler = PerformanceProfiler()
            
            # Start monitoring
            self.performance_monitor.start()
            self.memory_tracker.start()
            self.cpu_monitor.start()
            self.step_profiler.start()
            self.resource_optimizer.start()
    
    def start_benchmark(self):
        """Start the performance benchmark."""
        self.start_time = time.time()
        print(f"Starting benchmark: {self.benchmark_name}")
        print(f"Description: {self.description}")
        print(f"Start time: {datetime.fromtimestamp(self.start_time)}")
        
        # Record initial metrics
        if NLM_AVAILABLE:
            self._record_initial_metrics()
    
    def end_benchmark(self):
        """End the performance benchmark."""
        self.end_time = time.time()
        print(f"\nBenchmark completed: {self.benchmark_name}")
        print(f"End time: {datetime.fromtimestamp(self.end_time)}")
        print(f"Total duration: {self.get_duration():.2f} seconds")
        print(f"Total steps: {self.step_count}")
        
        # Stop monitoring
        if NLM_AVAILABLE:
            self._stop_monitoring()
    
    def record_step(self, step_data: Dict):
        """Record a simulation step."""
        self.step_count += 1
        
        if NLM_AVAILABLE:
            # Record step metrics
            step_time = step_data.get('step_time', 0.0)
            spikes_count = step_data.get('spikes_count', 0)
            neurons_updated = step_data.get('neurons_updated', 0)
            
            self.step_profiler.record_step_metrics(
                self.step_count,
                std::chrono::duration<double>(step_time),
                spikes_count,
                neurons_updated
            )
            
            # Update throughput
            throughput = {
                'spikes_per_second': spikes_count,
                'neurons_per_second': neurons_updated,
                'real_time_factor': step_time / 0.01 if step_time > 0 else 1.0,  # Assuming 10ms per step
                'total_steps': self.step_count,
                'total_spikes': self._get_total_spikes(),
                'start_time': self.start_time
            }
            
            self.performance_monitor.recordThroughput(throughput)
        
        # Store step data
        self.metrics[self.step_count] = {
            **step_data,
            'timestamp': time.time(),
            'cumulative_spikes': self._get_total_spikes(),
            'cumulative_neurons': self._get_total_neurons()
        }
        
        print(f"Step {self.step_count}: {step_data.get('description', 'No description')}")
    
    def get_duration(self) -> float:
        """Get benchmark duration in seconds."""
        if self.start_time is None:
            return 0.0
        if self.end_time is None:
            return time.time() - self.start_time
        return self.end_time - self.start_time
    
    def get_performance_summary(self) -> Dict:
        """Get performance summary for the benchmark."""
        duration = self.get_duration()
        
        summary = {
            'benchmark_name': self.benchmark_name,
            'description': self.description,
            'start_time': self.start_time,
            'end_time': self.end_time,
            'duration': duration,
            'total_steps': self.step_count,
            'steps_per_second': self.step_count / duration if duration > 0 else 0,
            'metrics': self.metrics
        }
        
        if NLM_AVAILABLE:
            # Add NLM performance metrics
            summary['resource_allocation'] = self.performance_monitor.getResourceAllocation()
            summary['throughput_metrics'] = self.performance_monitor.getThroughputMetrics()
            summary['memory_usage'] = self.memory_tracker.getCurrentMemoryStats()
            summary['cpu_usage'] = self.cpu_monitor.getOverallCPUUsage()
            summary['optimization_suggestions'] = self.resource_optimizer.getOptimizationSuggestions()
        
        return summary
    
    def save_results(self, filename: str):
        """Save benchmark results to file."""
        summary = self.get_performance_summary()
        
        if filename.endswith('.json'):
            with open(filename, 'w') as f:
                json.dump(summary, f, indent=2, default=str)
        elif filename.endswith('.csv'):
            with open(filename, 'w', newline='') as f:
                if self.metrics:
                    # Write header
                    header = ['step', 'timestamp', 'description', 'step_time', 'spikes_count', 
                             'neurons_updated', 'cumulative_spikes', 'cumulative_neurons']
                    writer = csv.DictWriter(f, fieldnames=header)
                    writer.writeheader()
                    
                    # Write data
                    for step, data in self.metrics.items():
                        row = {
                            'step': step,
                            'timestamp': data['timestamp'],
                            'description': data.get('description', ''),
                            'step_time': data.get('step_time', 0.0),
                            'spikes_count': data.get('spikes_count', 0),
                            'neurons_updated': data.get('neurons_updated', 0),
                            'cumulative_spikes': data.get('cumulative_spikes', 0),
                            'cumulative_neurons': data.get('cumulative_neurons', 0)
                        }
                        writer.writerow(row)
        
        print(f"Results saved to: {filename}")
    
    def generate_report(self, output_dir: str):
        """Generate performance report."""
        if NLM_AVAILABLE:
            # Generate report using NLM PerformanceReporter
            report = self.performance_reporter.generateReport()
            report_path = self.performance_reporter.saveReport(report)
            print(f"Performance report generated: {report_path}")
    
    def _record_initial_metrics(self):
        """Record initial performance metrics."""
        if NLM_AVAILABLE:
            # Record initial memory usage
            initial_memory = self._get_memory_usage()
            self.performance_monitor.recordMemoryUsage(
                initial_memory['total'], initial_memory['used'], initial_memory['peak']
            )
            
            # Record initial CPU usage
            self.performance_monitor.recordCPUUsage(
                self.cpu_monitor.getOverallCPUUsage(),
                self.cpu_monitor.getCoreCount(),
                std::chrono::duration<double>(0.1)
            )
    
    def _stop_monitoring(self):
        """Stop all monitoring components."""
        if NLM_AVAILABLE:
            self.performance_monitor.stop()
            self.memory_tracker.stop()
            self.cpu_monitor.stop()
            self.step_profiler.stop()
            self.resource_optimizer.stop()
    
    def _get_memory_usage(self) -> Dict:
        """Get current memory usage."""
        # Simplified memory usage calculation
        import psutil
        process = psutil.Process()
        memory_info = process.memory_info()
        
        return {
            'total': memory_info.rss,
            'used': memory_info.rss,
            'peak': memory_info.vms,
            'percent': process.memory_percent()
        }
    
    def _get_total_spikes(self) -> int:
        """Get total number of spikes recorded."""
        total = 0
        for step_data in self.metrics.values():
            total += step_data.get('spikes_count', 0)
        return total
    
    def _get_total_neurons(self) -> int:
        """Get total number of neurons updated."""
        total = 0
        for step_data in self.metrics.values():
            total += step_data.get('neurons_updated', 0)
        return total
class MicroBenchmark(PerformanceBenchmark):
    """Micro benchmark for small-scale testing."""
    
    def __init__(self):
        super().__init__("Micro Benchmark", "Small-scale performance testing (1000 neurons)")
        self.target_neurons = 1000
        self.target_steps = 1000
class SmallBenchmark(PerformanceBenchmark):
    """Small benchmark for medium-scale testing."""
    
    def __init__(self):
        super().__init__("Small Benchmark", "Medium-scale performance testing (10000 neurons)")
        self.target_neurons = 10000
        self.target_steps = 1000
class LargeBenchmark(PerformanceBenchmark):
    """Large benchmark for large-scale testing."""
    
    def __init__(self):
        super().__init__("Large Benchmark", "Large-scale performance testing (100000 neurons)")
        self.target_neurons = 100000
        self.target_steps = 100

def simulate_neural_network_step(step_num: int, neuron_count: int) -> Dict:
    """
    Simulate a neural network step.
    
    This is a simplified simulation of neural network computation
    that mimics the behavior of NLM neural simulations.
    """
    # Simulate computation time (varies with neuron count)
    base_time = 0.001  # 1 ms base time
    neuron_factor = neuron_count / 1000.0  # Scale with neuron count
    step_time = base_time * (1.0 + neuron_factor * 0.01)  # 1-10% additional time
    
    # Simulate spike generation (Poisson process)
    import random
    spike_rate = 0.05  # 5% of neurons fire per step
    spikes_count = int(neuron_count * spike_rate * random.random())
    
    # Simulate neuron updates
    neurons_updated = neuron_count
    
    # Simulate synaptic operations
    synaptic_ops = neuron_count * 10  # 10 synapses per neuron
    
    return {
        'step_number': step_num,
        'description': f"Neural step {step_num}",
        'step_time': step_time,
        'spikes_count': spikes_count,
        'neurons_updated': neurons_updated,
        'synaptic_operations': synaptic_ops
    }

def main():
    """Main function for running performance benchmarks."""
    print("NLM Performance Benchmark Suite")
    print("=" * 50)
    
    benchmarks = [
        MicroBenchmark(),
        SmallBenchmark(),
        LargeBenchmark()
    ]
    
    for benchmark in benchmarks:
        print(f"\n{'='*20} {benchmark.benchmark_name} {'='*20}")
        
        benchmark.start_benchmark()
        
        # Run simulation
        neuron_count = benchmark.target_neurons
        for i in range(benchmark.target_steps):
            step_data = simulate_neural_network_step(i + 1, neuron_count)
            benchmark.record_step(step_data)
        
        benchmark.end_benchmark()
        
        # Print summary
        summary = benchmark.get_performance_summary()
        print(f"\nBenchmark Summary:")
        print(f"  Steps/sec: {summary['steps_per_second']:.2f}")
        print(f"  Total duration: {summary['duration']:.2f} seconds")
        
        # Save results
        benchmark.save_results(f"results/{benchmark.benchmark_name.lower().replace(' ', '_')}.csv")
    
    print(f"\n{'='*50}")
    print("All benchmarks completed!")
    
    # Generate overall report
    if NLM_AVAILABLE:
        try:
            reporter = PerformanceReporter()
            report = reporter.generateReport("comprehensive", True, True)
            reporter.saveReport(report)
            print(f"Comprehensive performance report generated")
        except Exception as e:
            print(f"Could not generate comprehensive report: {e}")

if __name__ == "__main__":
    main()
