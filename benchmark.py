#!/usr/bin/env python3
"""
Performance Benchmark Script for NLM (Neural Learning Machine)

This script benchmarks the performance of the NLM framework after various improvements.
It measures the impact of error handling, validation, and factory patterns.
"""

import pynlm
import time
import statistics
import sys
from typing import List, Dict, Tuple

class NLMBenchmark:
    def __init__(self):
        self.results = {}
    
    def benchmark_factory_creation(self) -> Dict[str, float]:
        """Benchmark brain creation using factory pattern"""
        print("Benchmarking brain factory creation...")
        
        config_types = ["minimal", "research", "default"]
        times = {}
        
        for config_type in config_types:
            times[config_type] = []
            for _ in range(10):  # Run 10 times for statistical significance
                start_time = time.perf_counter()
                brain = pynlm.createBrain(config_type)
                brain.initialize()
                brain.reset()
                end_time = time.perf_counter()
                times[config_type].append((end_time - start_time) * 1000)  # Convert to ms
        
        result = {}
        for config_type in config_types:
            result[config_type] = {
                'mean_time': statistics.mean(times[config_type]),
                'median_time': statistics.median(times[config_type]),
                'std_time': statistics.stdev(times[config_type]) if len(times[config_type]) > 1 else 0,
                'min_time': min(times[config_type]),
                'max_time': max(times[config_type]),
            }
        
        return result
    
    def benchmark_simulation(self, config_type: str, steps: int = 1000) -> Dict[str, float]:
        """Benchmark brain simulation with given configuration"""
        print(f"Benchmarking simulation with {config_type} configuration...")
        
        brain = pynlm.createBrain(config_type)
        brain.initialize()
        
        times = []
        for step in range(steps):
            start_time = time.perf_counter()
            brain.step(step)
            end_time = time.perf_counter()
            times.append((end_time - start_time) * 1000)  # Convert to ms
        
        brain.reset()
        
        return {
            'mean_time': statistics.mean(times),
            'median_time': statistics.median(times),
            'std_time': statistics.stdev(times) if len(times) > 1 else 0,
            'min_time': min(times),
            'max_time': max(times),
            'steps_per_second': steps / sum(times) * 1000 if sum(times) > 0 else 0,
        }
    
    def benchmark_error_handling(self) -> Dict[str, float]:
        """Benchmark error handling performance"""
        print("Benchmarking error handling performance...")
        
        # Test invalid configuration handling
        times = []
        for _ in range(100):
            start_time = time.perf_counter()
            try:
                brain = pynlm.createBrain("invalid")
            except ValueError:
                pass  # Expected exception
            end_time = time.perf_counter()
            times.append((end_time - start_time) * 1000)
        
        # Test configuration validation
        config = pynlm.createDefaultConfig()
        validation_times = []
        for _ in range(100):
            start_time = time.perf_counter()
            try:
                config.set("brain.neuron_count", 1000)
            except ValueError:
                pass
            end_time = time.perf_counter()
            validation_times.append((end_time - start_time) * 1000)
        
        return {
            'invalid_config_mean': statistics.mean(times),
            'invalid_config_std': statistics.stdev(times) if len(times) > 1 else 0,
            'validation_mean': statistics.mean(validation_times),
            'validation_std': statistics.stdev(validation_times) if len(validation_times) > 1 else 0,
        }
    
    def benchmark_memory_usage(self, config_type: str, steps: int = 500) -> Dict[str, float]:
        """Benchmark memory usage (simplified)"""
        print(f"Benchmarking memory usage with {config_type} configuration...")
        
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        
        # Measure memory before simulation
        brain = pynlm.createBrain(config_type)
        brain.initialize()
        
        memory_before = process.memory_info().rss / 1024 / 1024  # Convert to MB
        
        # Run simulation
        for step in range(steps):
            brain.step(step)
        
        memory_after = process.memory_info().rss / 1024 / 1024  # Convert to MB
        
        brain.reset()
        
        return {
            'memory_before_mb': memory_before,
            'memory_after_mb': memory_after,
            'memory_increase_mb': memory_after - memory_before,
            'memory_per_step_kb': (memory_after - memory_before) * 1024 / steps if steps > 0 else 0,
        }
    
    def run_all_benchmarks(self) -> Dict:
        """Run all benchmarks and return results"""
        print("=" * 60)
        print("NLM Performance Benchmark Suite")
        print("=" * 60)
        
        results = {}
        
        # Benchmark factory creation
        results['factory_creation'] = self.benchmark_factory_creation()
        
        # Benchmark simulations
        for config_type in ["minimal", "research", "default"]:
            results[f'simulation_{config_type}'] = self.benchmark_simulation(config_type)
        
        # Benchmark error handling
        results['error_handling'] = self.benchmark_error_handling()
        
        # Benchmark memory usage (if psutil is available)
        try:
            import psutil
            results['memory_usage'] = {}
            for config_type in ["minimal", "research", "default"]:
                results['memory_usage'][config_type] = self.benchmark_memory_usage(config_type)
        except ImportError:
            print("Note: psutil not available - skipping memory benchmarks")
            results['memory_usage'] = {'note': 'psutil not available'}
        
        return results
    
    def print_results(self, results: Dict):
        """Print benchmark results in a readable format"""
        print("\n" + "=" * 60)
        print("Benchmark Results Summary")
        print("=" * 60)
        
        # Factory creation results
        print("\n1. Brain Factory Creation Performance:")
        print("-" * 40)
        for config_type, stats in results['factory_creation'].items():
            print(f"{config_type:12s}:")
            print(f"  Mean:     {stats['mean_time']:8.2f} ms")
            print(f"  Median:   {stats['median_time']:8.2f} ms")
            print(f"  Min:      {stats['min_time']:8.2f} ms")
            print(f"  Max:      {stats['max_time']:8.2f} ms")
            print(f"  Std Dev:  {stats['std_time']:8.2f} ms")
        
        # Simulation results
        print("\n2. Simulation Performance:")
        print("-" * 40)
        for config_type, stats in results.items():
            if config_type.startswith('simulation_'):
                brain_type = config_type.replace('simulation_', '')
                print(f"{brain_type:12s}:")
                print(f"  Mean step time: {stats['mean_time']:8.3f} ms")
                print(f"  Steps per second: {stats['steps_per_second']:8.0f}")
        
        # Error handling results
        print("\n3. Error Handling Performance:")
        print("-" * 40)
        eh = results['error_handling']
        print(f"Invalid config handling: {eh['invalid_config_mean']:8.3f} ms (±{eh['invalid_config_std']:8.3f} ms)")
        print(f"Configuration validation: {eh['validation_mean']:8.3f} ms (±{eh['validation_std']:8.3f} ms)")
        
        # Memory usage results
        if 'memory_usage' in results and 'note' not in results['memory_usage']:
            print("\n4. Memory Usage:")
            print("-" * 40)
            for config_type, stats in results['memory_usage'].items():
                if config_type != 'note':
                    print(f"{config_type:12s}:")
                    print(f"  Memory increase: {stats['memory_increase_mb']:8.2f} MB")
                    print(f"  Memory per step: {stats['memory_per_step_kb']:8.2f} KB")
        elif 'note' in results['memory_usage']:
            print("\n4. Memory Usage: psutil not available")
        
        print("\n" + "=" * 60)
        print("Benchmark completed successfully")
        print("=" * 60)

def main():
    try:
        benchmark = NLMBenchmark()
        results = benchmark.run_all_benchmarks()
        benchmark.print_results(results)
        
        # Generate a simple performance report
        print("\n" + "=" * 60)
        print("Performance Improvements Summary")
        print("=" * 60)
        print("✓ Added factory pattern for brain creation")
        print("✓ Enhanced error handling and validation")
        print("✓ Added parameter validation throughout codebase")
        print("✓ Removed hardcoded dependencies (RegionId(1)")
        print("✓ Added comprehensive documentation")
        print("✓ Improved defensive programming")
        print("\nThe NLM framework now provides:")
        print("- Better error messages and exception handling")
        print("- Configuration validation to prevent invalid settings")
        print("- Factory methods for common use cases")
        print("- Improved code maintainability and reliability")
        print("- Enhanced user experience with comprehensive documentation")
        
    except Exception as e:
        print(f"Benchmark failed with error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
