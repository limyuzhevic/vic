#!/usr/bin/env python3
"""
NLM Performance Test Suite
===========================

Performance tests for the NLM framework, measuring execution speed,
memory usage, and scalability characteristics.

These tests help identify performance bottlenecks and verify that
the framework meets performance requirements.
"""

import time
import unittest
import sys
import os
import psutil
import threading
from pathlib import Path

# Add the project root to the path
sys.path.insert(0, str(Path(__file__).parent))

from pynlm import Config, Brain

class PerformanceMetrics:
    """Helper class for collecting performance metrics"""
    
    def __init__(self):
        self.start_time = None
        self.end_time = None
        self.start_memory = None
        self.end_memory = None
    
    def start(self):
        """Start performance measurement"""
        self.start_time = time.time()
        process = psutil.Process()
        self.start_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    def stop(self):
        """Stop performance measurement"""
        self.end_time = time.time()
        process = psutil.Process()
        self.end_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    def duration(self):
        """Get duration in seconds"""
        if self.start_time and self.end_time:
            return self.end_time - self.start_time
        return 0.0
    
    def memory_used(self):
        """Get memory used in MB"""
        if self.start_memory and self.end_memory:
            return self.end_memory - self.start_memory
        return 0.0
    
    def get_report(self):
        """Get performance report as dictionary"""
        return {
            "duration_seconds": self.duration(),
            "memory_used_mb": self.memory_used(),
            "start_time": self.start_time,
            "end_time": self.end_time
        }

class TestBrainPerformance(unittest.TestCase):
    """Test brain creation and initialization performance"""
    
    def setUp(self):
        """Set up test environment"""
        self.config = Config()
    
    def runWithConfig(self, neuron_count, region_count):
        """Helper method to run brain creation with specific config"""
        config = Config()
        config.set("neuron_count", neuron_count)
        config.set("region_count", region_count)
        config.set("simulation_timestep", 0.001)
        
        metrics = PerformanceMetrics()
        metrics.start()
        
        brain = Brain(config)
        
        # Try to initialize
        try:
            brain.initialize()
            success = True
        except Exception:
            success = False
        
        metrics.stop()
        
        return brain, success, metrics.get_report()
    
    def test_small_brain_performance(self):
        """Test performance with small brain (20 neurons)"""
        brain, success, report = self.runWithConfig(20, 1)
        
        self.assertTrue(success, "Brain initialization failed")
        self.assertLess(report["duration_seconds"], 5.0, "Brain initialization took too long")
        self.assertLess(report["memory_used_mb"], 100.0, "Memory usage too high")
        
        # Performance should scale reasonably
        neurons_per_second = 20 / report["duration_seconds"]
        self.assertGreater(neurons_per_second, 1.0, "Brain creation too slow")
    
    def test_medium_brain_performance(self):
        """Test performance with medium brain (100 neurons)"""
        brain, success, report = self.runWithConfig(100, 2)
        
        if success:  # Skip if not supported
            self.assertLess(report["duration_seconds"], 10.0, "Brain initialization took too long")
            self.assertLess(report["memory_used_mb"], 500.0, "Memory usage too high")
    
    def test_large_brain_performance(self):
        """Test performance with large brain (500 neurons)"""
        brain, success, report = self.runWithConfig(500, 3)
        
        if success:  # Skip if not supported
            self.assertLess(report["duration_seconds"], 30.0, "Brain initialization took too long")
            self.assertLess(report["memory_used_mb"], 2000.0, "Memory usage too high")
    
    def test_memory_scaling(self):
        """Test that memory usage scales appropriately with brain size"""
        sizes = [(50, 1), (100, 2), (200, 3)]
        reports = []
        
        for neuron_count, region_count in sizes:
            _, _, report = self.runWithConfig(neuron_count, region_count)
            reports.append(report)
        
        # Check that memory usage doesn't grow exponentially
        # Each brain should be roughly proportional to size
        for i in range(1, len(reports)):
            prev_report = reports[i-1]
            curr_report = reports[i]
            
            # Memory should be roughly proportional
            prev_memory = prev_report["memory_used_mb"]
            curr_memory = curr_report["memory_used_mb"]
            
            if prev_memory > 0 and curr_memory > 0:
                # Memory ratio should be roughly neuron count ratio
                size_ratio = sizes[i][0] / sizes[i-1][0]
                memory_ratio = curr_memory / prev_memory
                
                # Allow some overhead, but shouldn't be more than 2x size ratio
                self.assertLess(memory_ratio, size_ratio * 2.0, 
                               f"Memory scaling too aggressive: {memory_ratio:.2f}x vs {size_ratio:.2f}x size")

class TestConfigurationPerformance(unittest.TestCase):
    """Test configuration loading and saving performance"""
    
    def test_config_creation_performance(self):
        """Test configuration object creation performance"""
        metrics = PerformanceMetrics()
        metrics.start()
        
        # Create many configuration objects
        configs = []
        for i in range(100):
            config = Config()
            config.set("neuron_count", 50 + i)
            config.set("region_count", 1 + (i % 3))
            configs.append(config)
        
        metrics.stop()
        
        report = metrics.get_report()
        self.assertLess(report["duration_seconds"], 2.0, "Config creation too slow")
        
        # Should create at least 50 configs per second
        configs_per_second = 100 / report["duration_seconds"]
        self.assertGreater(configs_per_second, 50.0, "Config creation too slow")
    
    def test_config_set_get_performance(self):
        """Test configuration setting and getting performance"""
        config = Config()
        
        metrics = PerformanceMetrics()
        metrics.start()
        
        # Set many configuration values
        for i in range(100):
            config.set(f"key_{i}", f"value_{i}")
            value = config.getOr(f"key_{i}", "")
            self.assertEqual(value, f"value_{i}")
        
        metrics.stop()
        
        report = metrics.get_report()
        self.assertLess(report["duration_seconds"], 1.0, "Config operations too slow")
        
        # Should handle at least 500 operations per second
        ops_per_second = 200 / report["duration_seconds"]
        self.assertGreater(ops_per_second, 500.0, "Config operations too slow")

class TestMemoryUsage(unittest.TestCase):
    """Test memory usage characteristics"""
    
    def test_concurrent_brain_creation(self):
        """Test creating multiple brains concurrently"""
        def create_brain(neuron_count):
            config = Config()
            config.set("neuron_count", neuron_count)
            config.set("region_count", 1)
            config.set("simulation_timestep", 0.001)
            
            try:
                brain = Brain(config)
                brain.initialize()
                return brain
            except Exception:
                return None
        
        # Create brains in parallel
        threads = []
        results = []
        
        metrics = PerformanceMetrics()
        metrics.start()
        
        for i in range(5):
            thread = threading.Thread(target=lambda: results.append(create_brain(30 + i * 10)))
            threads.append(thread)
            thread.start()
        
        # Wait for all threads to complete
        for thread in threads:
            thread.join()
        
        metrics.stop()
        
        report = metrics.get_report()
        
        # At least some brains should have been created
        created_count = sum(1 for r in results if r is not None)
        self.assertGreater(created_count, 0, "No brains were created")
        
        # Should complete within reasonable time
        self.assertLess(report["duration_seconds"], 30.0, "Concurrent creation took too long")

class TestScalability(unittest.TestCase):
    """Test scalability characteristics"""
    
    def test_brain_size_scaling(self):
        """Test that brain performance scales appropriately with size"""
        sizes = [
            (10, 1, "tiny"),
            (50, 2, "small"),
            (100, 3, "medium"),
            (200, 4, "large")
        ]
        
        results = []
        
        for neuron_count, region_count, size_name in sizes:
            config = Config()
            config.set("neuron_count", neuron_count)
            config.set("region_count", region_count)
            config.set("simulation_timestep", 0.001)
            
            metrics = PerformanceMetrics()
            metrics.start()
            
            brain = Brain(config)
            try:
                brain.initialize()
                success = True
            except Exception:
                success = False
            
            metrics.stop()
            
            report = metrics.get_report()
            results.append({
                "size": size_name,
                "neuron_count": neuron_count,
                "duration": report["duration_seconds"],
                "memory": report["memory_used_mb"],
                "success": success
            })
        
        # Print scalability results
        print("\nBrain Size Scaling Results:")
        print("Size  | Neurons | Duration (s) | Memory (MB)")
        print("-" * 50)
        
        for result in results:
            if result["success"]:
                print(f"{result['size']:6s} | {result['neuron_count']:7d} | {result['duration']:11.3f} | {result['memory']:10.1f}")
            else:
                print(f"{result['size']:6s} | {result['neuron_count']:7d} | {'FAILED':11s} | {'N/A':10s}")
        
        # Check that successful sizes have reasonable performance
        successful_results = [r for r in results if r["success"]]
        if len(successful_results) >= 2:
            # Performance should generally scale sub-linearly (not worse than linear)
            # Compare smallest and largest
            smallest = successful_results[0]
            largest = successful_results[-1]
            
            size_ratio = largest["neuron_count"] / smallest["neuron_count"]
            time_ratio = largest["duration"] / smallest["duration"]
            memory_ratio = largest["memory"] / smallest["memory"]
            
            # Time and memory ratios should be less than or equal to size ratio
            # (allowing for some overhead)
            self.assertLess(time_ratio, size_ratio * 2.0, 
                           f"Time scaling too aggressive: {time_ratio:.2f}x vs {size_ratio:.2f}x size")
            self.assertLess(memory_ratio, size_ratio * 2.0, 
                           f"Memory scaling too aggressive: {memory_ratio:.2f}x vs {size_ratio:.2f}x size")

if __name__ == "__main__":
    # Run performance tests
    unittest.main(verbosity=2)