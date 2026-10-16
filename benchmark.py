#!/usr/bin/env python3

"""
NLM Brain Simulation Performance Profiling and Benchmarking Tool

This script provides comprehensive profiling and benchmarking capabilities
for the NLM (Neural Learning Machine) brain simulation framework.

Features:
- Memory usage profiling
- CPU performance benchmarking
- Step-by-step execution timing
- Configuration comparison
- Performance regression tracking
- Platform-specific optimizations
"""

import time
import psutil
import os
import sys
import json
import statistics
from typing import Dict, List, Any, Optional
from dataclasses import dataclass, asdict
from datetime import datetime
import subprocess

@dataclass
class BenchmarkResult:
    """Container for benchmark results"""
    test_name: str
    mean_time: float
    median_time: float
    std_dev_time: float
    min_time: float
    max_time: float
    memory_usage_mb: float
    cpu_percent: float
    steps_per_second: float
    timestamp: str
    config: Dict[str, Any]

@dataclass
class PerformanceProfile:
    """Comprehensive performance profile"""
    cpu_profile: Dict[str, float]
    memory_profile: Dict[str, float]
    simulation_metrics: Dict[str, float]
    plasticity_metrics: Dict[str, float]
    memory_system_metrics: Dict[str, float]

class PerformanceProfiler:
    """Main profiler class for NLM performance analysis"""
    
    def __init__(self, config_file: str = "configs/benchmark.cfg"):
        self.config_file = config_file
        self.results: List[BenchmarkResult] = []
        self.platform_info = self._collect_platform_info()
        
    def _collect_platform_info(self) -> Dict[str, Any]:
        """Collect platform information"""
        return {
            'platform': sys.platform,
            'cpu_count': os.cpu_count(),
            'memory_total': psutil.virtual_memory().total / (1024**3),
            'process_id': os.getpid(),
            'python_version': sys.version,
            'timestamp': datetime.now().isoformat()
        }
    
    def get_memory_usage(self) -> float:
        """Get current memory usage in MB"""
        process = psutil.Process(os.getpid())
        return process.memory_info().rss / (1024**2)
    
    def benchmark_step(self, brain, num_steps: int = 1000, 
                      description: str = "Step execution") -> BenchmarkResult:
        """Benchmark brain stepping performance"""
        
        # Warm-up
        for i in range(min(10, num_steps)):
            brain.step(i)
        
        # Actual benchmark
        start_time = time.time()
        start_memory = self.get_memory_usage()
        
        for i in range(num_steps):
            brain.step(i)
        
        end_time = time.time()
        end_memory = self.get_memory_usage()
        
        elapsed = end_time - start_time
        memory_used = max(start_memory, end_memory) - min(start_memory, end_memory)
        
        times = [elapsed / num_steps] * num_steps  # Simplified timing model
        
        return BenchmarkResult(
            test_name=description,
            mean_time=statistics.mean(times),
            median_time=statistics.median(times),
            std_dev_time=statistics.stdev(times) if len(times) > 1 else 0.0,
            min_time=min(times),
            max_time=max(times),
            memory_usage_mb=memory_used,
            cpu_percent=psutil.cpu_percent(),
            steps_per_second=num_steps / elapsed,
            timestamp=datetime.now().isoformat(),
            config={}
        )
    
    def benchmark_save_load(self, brain, description: str = "Save/Load") -> BenchmarkResult:
        """Benchmark save/load performance"""
        
        # Save benchmark
        start_time = time.time()
        save_path = f"benchmark_save_{int(time.time())}.nlm"
        saved = brain.save(save_path)
        save_time = time.time() - start_time
        
        if saved and os.path.exists(save_path):
            os.remove(save_path)
        
        # Load benchmark
        start_time = time.time()
        config = brain.getConfig()
        nlm_brain = nlm.createBrain(config)
        loaded = nlm_brain.load("benchmark_save_{int(time.time())}.nlm")
        load_time = time.time() - start_time
        
        total_time = save_time + load_time
        
        return BenchmarkResult(
            test_name=description,
            mean_time=total_time / 2,
            median_time=total_time / 2,
            std_dev_time=0.0,
            min_time=min(save_time, load_time),
            max_time=max(save_time, load_time),
            memory_usage_mb=self.get_memory_usage(),
            cpu_percent=psutil.cpu_percent(),
            steps_per_second=1.0 / (total_time / 2),
            timestamp=datetime.now().isoformat(),
            config={}
        )
    
    def benchmark_memory_usage(self, brain, num_steps: int = 1000,
                              description: str = "Memory usage pattern") -> BenchmarkResult:
        """Benchmark memory usage over time"""
        
        memory_samples = []
        time_samples = []
        
        for i in range(num_steps):
            brain.step(i)
            memory_samples.append(self.get_memory_usage())
            time_samples.append(time.time())
            
            # Small delay to allow memory sampling
            time.sleep(0.001)
        
        return BenchmarkResult(
            test_name=description,
            mean_time=statistics.mean(memory_samples),
            median_time=statistics.median(memory_samples),
            std_dev_time=statistics.stdev(memory_samples) if len(memory_samples) > 1 else 0.0,
            min_time=min(memory_samples),
            max_time=max(memory_samples),
            memory_usage_mb=statistics.mean(memory_samples),
            cpu_percent=psutil.cpu_percent(),
            steps_per_second=num_steps / (time_samples[-1] - time_samples[0]),
            timestamp=datetime.now().isoformat(),
            config={}
        )
    
    def compare_configs(self, configs: List[Dict[str, Any]], 
                       num_steps: int = 1000) -> Dict[str, BenchmarkResult]:
        """Compare performance across different configurations"""
        
        results = {}
        
        for i, config_dict in enumerate(configs):
            config_file = f"configs/compare_config_{i}.cfg"
            with open(config_file, 'w') as f:
                json.dump(config_dict, f, indent=2)
            
            print(f"Running benchmark with config {i+1}...")
            
            brain = self.create_brain_from_config(config_file)
            result = self.benchmark_step(brain, num_steps, f"Config {i+1}")
            result.config = config_dict
            results[f"Config_{i+1}"] = result
            
            os.remove(config_file)
        
        return results
    
    def create_brain_from_config(self, config_file: str):
        """Create brain from config file"""
        try:
            import nlm
            config = nlm.createDefaultConfig()
            config.loadFromFile(config_file)
            return nlm.createBrain(config)
        except ImportError:
            raise ImportError("nlm module not available for benchmarking")
        except Exception as e:
            raise RuntimeError(f"Failed to create brain from config: {e}")
    
    def generate_report(self, results: List[BenchmarkResult]) -> str:
        """Generate human-readable performance report"""
        
        report = "=" * 80 + "\n"
        report += "NLM Brain Performance Benchmark Report\n"
        report += "=" * 80 + "\n\n"
        
        report += f"Platform: {results[0].config.get('platform', 'unknown')}\n"
        report += f"Timestamp: {results[0].timestamp}\n\n"
        
        for result in results:
            report += f"Test: {result.test_name}\n"
            report += f"  Mean time: {result.mean_time:.4f}s\n"
            report += f"  Median time: {result.median_time:.4f}s\n"
            report += f"  Std dev: {result.std_dev_time:.4f}s\n"
            report += f"  Min time: {result.min_time:.4f}s\n"
            report += f"  Max time: {result.max_time:.4f}s\n"
            report += f"  Memory usage: {result.memory_usage_mb:.2f} MB\n"
            report += f"  CPU percent: {result.cpu_percent:.1f}%\n"
            report += f"  Steps/sec: {result.steps_per_second:.1f}\n\n"
        
        return report
    
    def save_results(self, results: List[BenchmarkResult], filepath: str):
        """Save benchmark results to JSON file"""
        with open(filepath, 'w') as f:
            json.dump([asdict(result) for result in results], f, indent=2)
    
    def load_results(self, filepath: str) -> List[BenchmarkResult]:
        """Load benchmark results from JSON file"""
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        return [BenchmarkResult(**item) for item in data]

def main():
    """Main benchmarking script"""
    
    print("=== NLM Brain Performance Profiler ===\n")
    
    # Create profiler
    profiler = PerformanceProfiler()
    
    # Try to import nlm module
    try:
        import nlm
        print("NLM module loaded successfully")
    except ImportError:
        print("ERROR: nlm module not found. Please install nlm.")
        return 1
    
    # Create test brain
    config = nlm.createDefaultConfig()
    config.set("neuron_count", 500, nlm.ConfigSource.Runtime)
    config.set("region_count", 2, nlm.ConfigSource.Runtime)
    config.set("connection_probability", 0.1, nlm.ConfigSource.Runtime)
    
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print("Brain initialized successfully\n")
    
    # Run benchmarks
    print("Running performance benchmarks...\n")
    
    # Step performance benchmark
    step_result = profiler.benchmark_step(brain, 1000, "Step Execution")
    print(f"Step benchmark: {step_result.steps_per_second:.1f} steps/sec")
    
    # Memory benchmark
    mem_result = profiler.benchmark_memory_usage(brain, 100, "Memory Usage Pattern")
    print(f"Memory benchmark: {mem_result.mean_time:.2f} MB average")
    
    # Save/load benchmark
    save_load_result = profiler.benchmark_save_load(brain, "Save/Load Performance")
    print(f"Save/Load benchmark: {save_load_result.mean_time:.4f}s total")
    
    # Collect all results
    results = [step_result, mem_result, save_load_result]
    
    # Generate report
    report = profiler.generate_report(results)
    print(report)
    
    # Save results
    profiler.save_results(results, "performance_benchmarks.json")
    print("Results saved to performance_benchmarks.json")
    
    # Try to create performance comparison configs
    print("\nRunning configuration comparison...")
    
    configs = [
        {"neuron_count": 1000, "region_count": 1, "connection_probability": 0.05},
        {"neuron_count": 1000, "region_count": 2, "connection_probability": 0.1},
        {"neuron_count": 1000, "region_count": 4, "connection_probability": 0.2},
    ]
    
    try:
        comparison_results = profiler.compare_configs(configs, 500)
        print(f"Comparison completed with {len(comparison_results)} configurations")
    except Exception as e:
        print(f"Comparison failed (this is expected if nlm module dependencies are missing): {e}")
    
    print("\n=== Performance profiling completed ===")
    return 0

if __name__ == "__main__":
    sys.exit(main())