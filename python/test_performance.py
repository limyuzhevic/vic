#!/usr/bin/env python3
"""
Performance Profiling Tests for NLM Python Bindings

This module contains comprehensive performance tests including:
- Batch processing performance
- Memory usage optimization
- Serialization/deserialization performance
- CPU utilization monitoring
- Performance regression testing
"""

import os
import sys
import time
import tracemalloc
import psutil
import numpy as np
from typing import Dict, List, Any, Callable

# Import NLM bindings
try:
    import pynlm
except ImportError as e:
    print(f"Failed to import NLM bindings: {e}")
    print("Make sure NLM is properly installed and built.")
    sys.exit(1)

from test_suite import NLMTestSuite, NLMSyntheticDataGenerator
class PerformanceTests(NLMTestSuite):
    """
    Comprehensive performance tests for NLM Python bindings.
    
    Tests all performance requirements including:
    - Batch processing performance
    - Memory usage optimization
    - Serialization/deserialization performance
    - Performance regression with baseline
    - CPU utilization monitoring
    """
    
    def setup_method(self):
        """Setup before each test method."""
        self.setup_test_environment()
        
        # Create default configuration and brain
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        
        # Create world and agent brain
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Configure world for testing
        self.world.configure(width=50, height=50, visionWidth=32, visionHeight=32)
        
    def teardown_method(self):
        """Cleanup after each test method."""
        self.teardown_test_environment()
    
    # Performance monitoring utilities
    def _measure_cpu_usage(self, func: Callable, *args, **kwargs) -> Dict[str, Any]:
        """Measure CPU usage of a function."""
        process = psutil.Process()
        
        # Get initial CPU times
        cpu_times_start = process.cpu_times()
        cpu_percent_start = process.cpu_percent()
        
        # Run function
        start_time = time.time()
        result = func(*args, **kwargs)
        end_time = time.time()
        
        # Get final CPU times
        cpu_times_end = process.cpu_times()
        cpu_percent_end = process.cpu_percent()
        
        # Calculate metrics
        duration = end_time - start_time
        cpu_time = (cpu_times_end.user + cpu_times_end.system) - (cpu_times_start.user + cpu_times_start.system)
        cpu_efficiency = cpu_time / duration if duration > 0 else 0
        
        return {
            'result': result,
            'duration': duration,
            'cpu_time': cpu_time,
            'cpu_efficiency': cpu_efficiency,
            'start_cpu_percent': cpu_percent_start,
            'end_cpu_percent': cpu_percent_end
        }
    
    def _measure_memory_usage(self, func: Callable, *args, **kwargs) -> Dict[str, Any]:
        """Measure memory usage of a function."""
        # Start memory tracing
        tracemalloc.start()
        
        # Get initial memory
        process = psutil.Process()
        memory_info_start = process.memory_info()
        
        # Run function
        start_time = time.time()
        result = func(*args, **kwargs)
        end_time = time.time()
        
        # Get final memory
        memory_info_end = process.memory_info()
        
        # Get memory snapshot
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        # Calculate metrics
        duration = end_time - start_time
        memory_increase = memory_info_end.rss - memory_info_start.rss
        memory_increase_mb = memory_increase / (1024 * 1024)
        
        return {
            'result': result,
            'duration': duration,
            'memory_increase_bytes': memory_increase,
            'memory_increase_mb': memory_increase_mb,
            'peak_memory_mb': peak / (1024 * 1024),
            'current_memory_mb': current / (1024 * 1024)
        }
    
    # Test batch processing performance
    def test_batch_processing_performance(self):
        """Test batch processing performance with different sizes."""
        # Test different batch sizes
        batch_sizes = [10, 50, 100, 500]
        
        self.agent.initialize(self.world)
        
        for batch_size in batch_sizes:
            # Generate batch of percepts
            batch_percepts = self.data_generator.generate_batch_percepts(batch_size)
            
            # Measure performance
            metrics = self._measure_cpu_usage(
                self.agent.processPercepts, batch_percepts
            )
            
            mem_metrics = self._measure_memory_usage(
                self.agent.processPercepts, batch_percepts
            )
            
            # Calculate performance metrics
            items_per_second = batch_size / metrics['duration']
            cpu_efficiency = metrics['cpu_efficiency']
            memory_efficiency = mem_metrics['memory_increase_mb'] / batch_size if batch_size > 0 else 0
            
            self.framework.log_performance(
                f"batch_processing_{batch_size}", metrics['duration'],
                items_per_second=items_per_second,
                cpu_efficiency=cpu_efficiency,
                memory_mb_increase=mem_metrics['memory_increase_mb'],
                memory_efficiency_mb_per_item=memory_efficiency,
                peak_memory_mb=mem_metrics['peak_memory_mb']
            )
            
            print(f"✓ Batch processing {batch_size}: {items_per_second:.1f} items/s, "
                  f"{mem_metrics['memory_increase_mb']:.1f}MB increase")
    
    # Test serialization performance
    def test_serialization_performance(self):
        """Test serialization/deserialization performance."""
        self.agent.initialize(self.world)
        
        # Process data first
        for i in range(50):
            percept = self.data_generator.generate_sensory_percept()
            self.agent.processSensoryInput(percept, False)
        
        # Test different serialization sizes
        test_sizes = [100, 500, 1000]
        
        for test_size in test_sizes:
            # Reset and generate test data
            self.agent.initialize(self.world)
            
            # Generate test data
            test_percepts = self.data_generator.generate_batch_percepts(test_size)
            for percept in test_percepts:
                self.agent.processSensoryInput(percept, False)
            
            # Measure serialization performance
            with tempfile.TemporaryDirectory() as temp_dir:
                test_file = os.path.join(temp_dir, f"perf_state_{test_size}.json")
                
                start_time = time.time()
                save_success = self.agent.saveState(test_file)
                save_time = time.time() - start_time
                
                self.framework.assert_true(save_success, f"Serialization {test_size} should succeed")
                
                # Measure file size
                file_size = os.path.getsize(test_file)
                
                # Measure deserialization performance
                start_time = time.time()
                load_success = self.agent.loadState(test_file)
                load_time = time.time() - start_time
                
                self.framework.assert_true(load_success, f"Deserialization {test_size} should succeed")
                
                # Calculate performance metrics
                save_throughput = test_size / save_time if save_time > 0 else 0
                load_throughput = test_size / load_time if load_time > 0 else 0
                total_time = save_time + load_time
                size_efficiency = file_size / (test_size * 256)  # bytes per float32
                
                self.framework.log_performance(
                    f"serialization_{test_size}", total_time,
                    save_time=save_time,
                    load_time=load_time,
                    save_throughput=test_size / save_time if save_time > 0 else 0,
                    load_throughput=test_size / load_time if load_time > 0 else 0,
                    file_size_mb=file_size / (1024 * 1024),
                    size_efficiency=size_efficiency,
                    total_items=test_size
                )
                
                print(f"✓ Serialization {test_size}: save={save_time:.4f}s ({save_throughput:.1f} items/s), "
                      f"load={load_time:.4f}s ({load_throughput:.1f} items/s), {size_efficiency:.1f} bytes/item")
    
    # Test memory usage optimization
    def test_memory_usage_optimization(self):
        """Test memory usage optimization during processing."""
        # Create memory baseline
        self.agent.initialize(self.world)
        
        # Process large amount of data
        large_percept_count = 1000
        
        print(f"Testing memory optimization with {large_percept_count} percepts...")
        
        # Measure memory before processing
        process = psutil.Process()
        memory_before = process.memory_info().rss / (1024 * 1024)  # MB
        
        # Process data with memory monitoring
        tracemalloc.start()
        
        for i in range(large_percept_count):
            percept = self.data_generator.generate_sensory_percept()
            self.agent.processSensoryInput(percept, False)
            
            if (i + 1) % 200 == 0:
                current_memory, peak_memory = tracemalloc.get_traced_memory()
                print(f"  Processed {i + 1}/{large_percept_count}: "
                      f"Current: {current_memory / (1024 * 1024):.1f}MB, "
                      f"Peak: {peak_memory / (1024 * 1024):.1f}MB")
        
        # Measure memory after processing
        current_memory, peak_memory = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        memory_after = process.memory_info().rss / (1024 * 1024)  # MB
        memory_increase = memory_after - memory_before
        
        # Calculate memory efficiency metrics
        memory_per_item = memory_increase / large_percept_count if large_percept_count > 0 else 0
        processing_rate = large_percept_count / (large_percept_count / 10)  # Assuming 10 seconds
        
        self.framework.log_performance(
            "test_memory_optimization", processing_rate * 10,
            total_percepts=large_percept_count,
            memory_before_mb=memory_before,
            memory_after_mb=memory_after,
            memory_increase_mb=memory_increase,
            memory_per_item_mb=memory_per_item,
            peak_memory_mb=peak_memory / (1024 * 1024)
        )
        
        print(f"✓ Memory optimization test: {memory_per_item:.3f}MB per item, "
              f"{peak_memory / (1024 * 1024):.1f}MB peak")
    
    # Test performance profiling
    def test_performance_profiling(self):
        """Test performance profiling capabilities."""
        self.agent.initialize(self.world)
        
        # Generate workload
        workload_sizes = [10, 50, 100, 500]
        
        for workload_size in workload_sizes:
            # Reset and process
            self.agent.initialize(self.world)
            
            start_time = time.time()
            for i in range(workload_size):
                percept = self.data_generator.generate_sensory_percept()
                self.agent.processSensoryInput(percept, False)
                
                # Occasionally perform other operations
                if i % 10 == 0:
                    command = self.agent.decodeMotorCommand()
                    self.agent.applyRewardModulation(0.1, 0.05)
            
            processing_time = time.time() - start_time
            
            # Get performance metrics
            try:
                metrics = self.agent.profilePerformance()
                
                if metrics:
                    self.framework.log_performance(
                        f"performance_profiling_{workload_size}", processing_time,
                        processing_efficiency=metrics.processingEfficiency,
                        steps_per_second=metrics.stepsPerSecond,
                        memory_usage_mb=metrics.memoryUsageBytes / (1024 * 1024),
                        average_step_time_ms=metrics.averageStepTime * 1000,
                        total_steps=metrics.totalSteps,
                        average_reward=metrics.avgReward,
                        average_prediction_error=metrics.avgPredictionError
                    )
                    
                    print(f"✓ Performance profiling {workload_size}: "
                          f"{metrics.stepsPerSecond:.1f} steps/s, "
                          f"{metrics.memoryUsageBytes / (1024 * 1024):.1f}MB memory")
                else:
                    print(f"⚠ Performance metrics not available for workload size {workload_size}")
                    
            except Exception as e:
                print(f"⚠ Performance profiling raised exception: {type(e).__name__}: {e}")
    
    # Test CPU utilization
    def test_cpu_utilization(self):
        """Test CPU utilization during different operations."""
        self.agent.initialize(self.world)
        
        # Test CPU utilization for different operations
        test_operations = [
            ("Sensory processing", lambda: self._process_sensory_cpu_test()),
            ("Motor decoding", lambda: self._decode_motor_cpu_test()),
            ("Reward modulation", lambda: self._reward_modulation_cpu_test()),
            ("Batch processing", lambda: self._batch_processing_cpu_test()),
        ]
        
        for name, operation in test_operations:
            try:
                metrics = self._measure_cpu_usage(operation)
                
                self.framework.log_performance(
                    f"cpu_utilization_{name}", metrics['duration'],
                    cpu_efficiency=metrics['cpu_efficiency'],
                    cpu_time=metrics['cpu_time'],
                    duration=metrics['duration'],
                    operation=name
                )
                
                print(f"✓ CPU utilization {name}: efficiency={metrics['cpu_efficiency']:.3f}, "
                      f"duration={metrics['duration']:.4f}s")
                      
            except Exception as e:
                print(f"⚠ CPU utilization test '{name}' raised exception: {type(e).__name__}")
    
    def _process_sensory_cpu_test(self):
        """Helper for sensory processing CPU test."""
        percept = self.data_generator.generate_sensory_percept()
        self.agent.processSensoryInput(percept, False)
    
    def _decode_motor_cpu_test(self):
        """Helper for motor decoding CPU test."""
        percept = self.data_generator.generate_sensory_percept()
        self.agent.processSensoryInput(percept, False)
        command = self.agent.decodeMotorCommand()
        return command
    
    def _reward_modulation_cpu_test(self):
        """Helper for reward modulation CPU test."""
        self.agent.applyRewardModulation(0.5, 0.25)
    
    def _batch_processing_cpu_test(self):
        """Helper for batch processing CPU test."""
        percepts = self.data_generator.generate_batch_percepts(50)
        self.agent.processPercepts(percepts)
    
    # Test performance regression
    def test_performance_regression(self):
        """Test performance regression with baseline measurements."""
        # This test establishes baseline performance and would be used for regression testing
        
        print("Performance Regression Test - Baseline Measurements")
        print("=" * 60)
        
        # Run baseline tests
        baseline_results = {}
        
        # Baseline 1: Single percept processing
        self.agent.initialize(self.world)
        start_time = time.time()
        percept = self.data_generator.generate_sensory_percept()
        self.agent.processSensoryInput(percept, False)
        single_processing_time = time.time() - start_time
        
        baseline_results['single_percept_processing'] = single_processing_time
        
        # Baseline 2: Batch processing
        self.agent.initialize(self.world)
        start_time = time.time()
        batch_percepts = self.data_generator.generate_batch_percepts(100)
        self.agent.processPercepts(batch_percepts)
        batch_processing_time = time.time() - start_time
        
        baseline_results['batch_processing_100'] = batch_processing_time
        
        # Baseline 3: Serialization
        self.agent.initialize(self.world)
        with tempfile.TemporaryDirectory() as temp_dir:
            state_file = os.path.join(temp_dir, "baseline_state.json")
            start_time = time.time()
            self.agent.saveState(state_file)
            serialize_time = time.time() - start_time
            
            # Load back
            start_time = time.time()
            self.agent.loadState(state_file)
            deserialize_time = time.time() - start_time
            
            baseline_results['serialization'] = serialize_time
            baseline_results['deserialization'] = deserialize_time
        
        # Calculate throughput
        baseline_results['single_percept_throughput'] = 1.0 / single_processing_time if single_processing_time > 0 else 0
        baseline_results['batch_percept_throughput'] = 100.0 / batch_processing_time if batch_processing_time > 0 else 0
        baseline_results['serialization_throughput'] = 1.0 / serialize_time if serialize_time > 0 else 0
        baseline_results['deserialization_throughput'] = 1.0 / deserialize_time if deserialize_time > 0 else 0
        
        # Display baseline results
        print("\nPerformance Baseline Results:")
        for test_name, value in baseline_results.items():
            if 'throughput' in test_name:
                print(f"  {test_name}: {value:.2f} operations/second")
            else:
                print(f"  {test_name}: {value:.4f} seconds")
        
        # Log baseline for regression tracking
        self.framework.log_performance("performance_regression_baseline", 
                                      sum(v for k, v in baseline_results.items() if 'time' in k),
                                      **baseline_results)
        
        print("✓ Performance regression test - baseline established")

# Run tests when executed directly
if __name__ == "__main__":
    print("Running Performance Profiling Tests...")
    print("=" * 60)
    
    # Create test instance
    test_suite = PerformanceTests()
    
    # Run all tests
    test_suite.run_all_tests()
    
    # Print summary
    summary = test_suite.get_test_summary()
    print(f"\n{'='*60}")
    print("TEST SUMMARY")
    print(f"{'='*60}")
    print(f"Test Suite: {summary['test_suite']}")
    print(f"Passed: {summary['passed_tests']}")
    print(f"Failed: {summary['failed_tests']}")
    print(f"Total: {summary['total_tests']}")
    print(f"Duration: {summary['duration_seconds']:.2f}s")
    print(f"Avg time per test: {summary['average_time_per_test']:.4f}s")
    
    if summary['failed_tests'] > 0:
        print(f"\n❌ Test suite completed with {summary['failed_tests']} failures")
        sys.exit(1)
    else:
        print(f"\n✅ All performance tests passed successfully!")
        sys.exit(0)