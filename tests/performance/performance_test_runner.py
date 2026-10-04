#!/usr/bin/env python3
"""
NLM Performance Tests

This module contains performance tests for the NLM framework.
It tests speed, memory usage, and scalability of the neural simulation.
"""

import sys
import time
import tracemalloc
import threading
from pathlib import Path
import json

sys.path.insert(0, str(Path(__file__).parent.parent))

# Import NLM modules
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm not available, performance tests will be skipped")

class NLMPerformanceProfiler:
    """Performance profiler for NLM operations."""
    
    def __init__(self):
        self.results = []
        self.benchmark_dir = Path(__file__).parent / "benchmarks"
        self.benchmark_dir.mkdir(exist_ok=True)
    
    def profile_operation(self, name: str, operation, *args, **kwargs):
        """Profile a single operation."""
        # Time the operation
        start_time = time.perf_counter()
        
        # Measure memory if possible
        tracemalloc.start()
        
        try:
            # Execute the operation
            result = operation(*args, **kwargs)
            
            # Calculate metrics
            execution_time = time.perf_counter() - start_time
            current, peak = tracemalloc.get_traced_memory()
            tracemalloc.stop()
            
            # Store results
            perf_data = {
                "name": name,
                "execution_time_ms": execution_time * 1000,
                "peak_memory_kb": peak / 1024,
                "timestamp": time.time(),
                "success": True
            }
            
            self.results.append(perf_data)
            
            return result, perf_data
            
        except Exception as e:
            execution_time = time.perf_counter() - start_time
            tracemalloc.stop()
            
            perf_data = {
                "name": name,
                "execution_time_ms": execution_time * 1000,
                "peak_memory_kb": 0,
                "timestamp": time.time(),
                "success": False,
                "error": str(e)
            }
            
            self.results.append(perf_data)
            raise
    
    def save_results(self, filename: str = None):
        """Save performance results to file."""
        if filename is None:
            filename = f"performance_results_{int(time.time())}.json"
        
        filepath = self.benchmark_dir / filename
        with open(filepath, 'w') as f:
            json.dump(self.results, f, indent=2)
        
        print(f"Performance results saved to {filepath}")
        return filepath
    
    def get_summary(self):
        """Get performance summary."""
        if not self.results:
            return None
        
        successful_results = [r for r in self.results if r["success"]]
        
        summary = {
            "total_operations": len(self.results),
            "successful_operations": len(successful_results),
            "failed_operations": len(self.results) - len(successful_results),
            "avg_execution_time_ms": sum(r["execution_time_ms"] for r in successful_results) / len(successful_results) if successful_results else 0,
            "min_execution_time_ms": min(r["execution_time_ms"] for r in successful_results) if successful_results else 0,
            "max_execution_time_ms": max(r["execution_time_ms"] for r in successful_results) if successful_results else 0,
            "avg_memory_kb": sum(r["peak_memory_kb"] for r in successful_results) / len(successful_results) if successful_results else 0,
            "min_memory_kb": min(r["peak_memory_kb"] for r in successful_results) if successful_results else 0,
            "max_memory_kb": max(r["peak_memory_kb"] for r in successful_results) if successful_results else 0
        }
        
        return summary

class SpeedBenchmarks:
    """Speed benchmarks for NLM operations."""
    
    def __init__(self, profiler: NLMPerformanceProfiler):
        self.profiler = profiler
    
    def benchmark_config_operations(self):
        """Benchmark configuration operations."""
        if not PYNLM_AVAILABLE:
            print("Skipping config benchmarks: pynlm not available")
            return
        
        print("Running config operation benchmarks...")
        
        # Create a config object
        config, perf_data = self.profiler.profile_operation("create_config", pynlm.Config)
        print(f"  Config creation: {perf_data['execution_time_ms']:.2f} ms")
        
        # Test setting and getting values
        iterations = 1000
        
        def test_set_get():
            test_config = pynlm.Config()
            for i in range(iterations):
                test_config.set(f"key_{i}", i)
                _ = test_config.get(f"key_{i}")
        
        _, perf_data = self.profiler.profile_operation(f"config_set_get_{iterations}", test_set_get)
        print(f"  Config set/get {iterations} operations: {perf_data['execution_time_ms']:.2f} ms")
        
        # Test getOr operations
        def test_get_or():
            test_config = pynlm.Config()
            for i in range(iterations):
                _ = test_config.getOr(f"key_{i}", 0)
                _ = test_config.getOr(f"missing_{i}", i)
        
        _, perf_data = self.profiler.profile_operation(f"config_get_or_{iterations}", test_get_or)
        print(f"  Config getOr {iterations} operations: {perf_data['execution_time_ms']:.2f} ms")
    
    def benchmark_creation_operations(self):
        """Benchmark object creation operations."""
        if not PYNLM_AVAILABLE:
            print("Skipping creation benchmarks: pynlm not available")
            return
        
        print("Running creation operation benchmarks...")
        
        iterations = 100
        
        # Test config creation
        def test_config_creation():
            for i in range(iterations):
                pynlm.Config()
        
        _, perf_data = self.profiler.profile_operation(f"config_creation_{iterations}", test_config_creation)
        print(f"  Config creation {iterations} times: {perf_data['execution_time_ms']:.2f} ms")
        
        # Test brain creation
        def test_brain_creation():
            for i in range(iterations):
                pynlm.createBrain(pynlm.createDefaultConfig())
        
        _, perf_data = self.profiler.profile_operation(f"brain_creation_{iterations}", test_brain_creation)
        print(f"  Brain creation {iterations} times: {perf_data['execution_time_ms']:.2f} ms")
        
        # Test world creation
        def test_world_creation():
            for i in range(iterations):
                pynlm.createSimpleWorld()
        
        _, perf_data = self.profiler.profile_operation(f"world_creation_{iterations}", test_world_creation)
        print(f"  World creation {iterations} times: {perf_data['execution_time_ms']:.2f} ms")

class MemoryUsageBenchmarks:
    """Memory usage benchmarks for NLM operations."""
    
    def __init__(self, profiler: NLMPerformanceProfiler):
        self.profiler = profiler
    
    def benchmark_memory_growth(self):
        """Benchmark memory usage growth."""
        if not PYNLM_AVAILABLE:
            print("Skipping memory benchmarks: pynlm not available")
            return
        
        print("Running memory usage benchmarks...")
        
        iterations = 500
        
        # Test memory growth with config objects
        def test_config_memory_growth():
            configs = []
            for i in range(iterations):
                config = pynlm.Config()
                config.set(f"key_{i}", i)
                config.set(f"float_val_{i}", i * 0.1)
                config.set(f"string_val_{i}", f"value_{i}")
                config.set(f"bool_val_{i}", i % 2 == 0)
                configs.append(config)
            
            # Clean up
            del configs
        
        _, perf_data = self.profiler.profile_operation(f"config_memory_growth_{iterations}", test_config_memory_growth)
        print(f"  Config memory growth {iterations} objects: {perf_data['peak_memory_kb']:.2f} KB peak")
    
    def benchmark_large_objects(self):
        """Benchmark large object handling."""
        if not PYNLM_AVAILABLE:
            print("Skipping large object benchmarks: pynlm not available")
            return
        
        print("Running large object benchmarks...")
        
        # Test with large data structures
        def test_large_config():
            config = pynlm.Config()
            
            # Add many entries
            for i in range(10000):
                config.set(f"large_key_{i}", f"large_value_{i}" * 10)  # Larger values
        
        _, perf_data = self.profiler.profile_operation("large_config_creation", test_large_config)
        print(f"  Large config creation (10000 entries): {perf_data['execution_time_ms']:.2f} ms, {perf_data['peak_memory_kb']:.2f} KB")

class ScalabilityTests:
    """Scalability tests for NLM operations."""
    
    def __init__(self, profiler: NLMPerformanceProfiler):
        self.profiler = profiler
    
    def test_scalability(self):
        """Test scalability with increasing workload."""
        if not PYNLM_AVAILABLE:
            print("Skipping scalability tests: pynlm not available")
            return
        
        print("Running scalability tests...")
        
        # Test with different numbers of operations
        operation_counts = [100, 500, 1000, 5000]
        
        for count in operation_counts:
            def test_scale(config_count):
                configs = []
                for i in range(config_count):
                    config = pynlm.Config()
                    config.set(f"key_{i}", i)
                    config.set(f"float_val_{i}", i * 0.1)
                    configs.append(config)
                return configs
            
            _, perf_data = self.profiler.profile_operation(f"scalability_{count}_configs", 
                                                         lambda: test_scale(count))
            print(f"  Scalability {count} configs: {perf_data['execution_time_ms']:.2f} ms, {perf_data['peak_memory_kb']:.2f} KB")

def run_performance_tests():
    """Run all performance tests."""
    print("NLM Performance Tests")
    print("=" * 50)
    
    # Create profiler
    profiler = NLMPerformanceProfiler()
    
    # Create benchmarkers
    speed_benchmarker = SpeedBenchmarks(profiler)
    memory_benchmarker = MemoryUsageBenchmarks(profiler)
    scalability_tester = ScalabilityTests(profiler)
    
    # Run benchmarks
    try:
        speed_benchmarker.benchmark_config_operations()
        speed_benchmarker.benchmark_creation_operations()
        
        memory_benchmarker.benchmark_memory_growth()
        memory_benchmarker.benchmark_large_objects()
        
        scalability_tester.test_scalability()
    except Exception as e:
        print(f"Error during performance tests: {e}")
        import traceback
        traceback.print_exc()
    
    # Get and display summary
    summary = profiler.get_summary()
    if summary:
        print("\nPerformance Test Summary:")
        print(f"  Total operations: {summary['total_operations']}")
        print(f"  Successful operations: {summary['successful_operations']}")
        print(f"  Failed operations: {summary['failed_operations']}")
        print(f"  Average execution time: {summary['avg_execution_time_ms']:.2f} ms")
        print(f"  Min execution time: {summary['min_execution_time_ms']:.2f} ms")
        print(f"  Max execution time: {summary['max_execution_time_ms']:.2f} ms")
        print(f"  Average memory usage: {summary['avg_memory_kb']:.2f} KB")
        print(f"  Min memory usage: {summary['min_memory_kb']:.2f} KB")
        print(f"  Max memory usage: {summary['max_memory_kb']:.2f} KB")
    
    # Save results
    profiler.save_results()
    
    return summary

if __name__ == "__main__":
    print("NLM Performance Test Suite")
    print("=" * 50)
    
    # Check if pynlm is available
    if not PYNLM_AVAILABLE:
        print("Warning: pynlm not available, some performance tests will be skipped")
        print("Please ensure the Python bindings are properly installed.")
    
    # Run performance tests
    summary = run_performance_tests()
    
    if summary:
        print(f"\nPerformance testing completed successfully!")
        print(f"Overall average execution time: {summary['avg_execution_time_ms']:.2f} ms")
        print(f"Overall average memory usage: {summary['avg_memory_kb']:.2f} KB")
    else:
        print(f"\nPerformance testing completed with warnings.")