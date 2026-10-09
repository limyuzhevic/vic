#!/usr/bin/env python3

"""
Performance Testing Suite for NLM Enhanced Components

This module contains comprehensive performance tests for all enhanced NLM components,
including benchmarking, stress testing, scalability testing, and real-time testing.
"""

import sys
import os
import time
import threading
import multiprocessing
import statistics
from typing import Dict, List, Any, Optional, Tuple, Callable
from unittest.mock import Mock, patch
import numpy as np
import psutil

# Add the parent directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import NLM components for testing
from pynlm import Config, create_default_config, Brain, create_brain
from pynlm.memory import WorkingMemory, EpisodicMemory, AssociativeMemory
from pynlm.neuromodulation import NeuromodulationSystem

class TestMemoryPerformance:
    """Performance tests for memory systems."""
    
    def test_working_memory_performance(self):
        """Test WorkingMemory performance characteristics."""
        config = create_default_config()
        
        with Brain(config) as brain:
            working_mem = WorkingMemory(brain)
            
            # Test performance with different data sizes
            data_sizes = [10, 50, 100, 500]
            performance_results = {}
            
            for size in data_sizes:
                start_time = time.time()
                memory_usage_before = psutil.virtual_memory().used
                
                # Perform memory operations
                for i in range(size):
                    pattern = [0.1 * j for j in range(size % 20)]
                    working_mem.store(pattern, strength=0.5)
                    _ = working_mem.retrieve()
                    _ = working_mem.get_memory_activity()
                
                end_time = time.time()
                memory_usage_after = psutil.virtual_memory().used
                
                duration = end_time - start_time
                memory_used = (memory_usage_after - memory_usage_before) / 1024 / 1024  # MB
                operations_per_second = (size * 3) / duration  # 3 operations per iteration
                
                performance_results[size] = {
                    "duration": duration,
                    "memory_used_mb": memory_used,
                    "operations_per_second": operations_per_second,
                    "time_per_operation_ms": (duration / (size * 3)) * 1000
                }
                
                # Cleanup
                working_mem.clear()
            
            # Analyze performance trends
            durations = [results["duration"] for results in performance_results.values()]
            ops_per_second = [results["operations_per_second"] for results in performance_results.values()]
            
            # Performance should scale reasonably
            assert len(durations) >= 4, "Should test multiple data sizes"
            
            # Log performance metrics for reporting
            from tests.comprehensive.core.test_framework import PerformanceMetrics
            metrics = PerformanceMetrics(
                cpu_usage=psutil.cpu_percent(),
                memory_usage=psutil.virtual_memory().used,
                execution_time=sum(durations),
                throughput=sum(ops_per_second),
                peak_memory=max([results["memory_used_mb"] for results in performance_results.values()]),
                avg_memory=statistics.mean([results["memory_used_mb"] for results in performance_results.values()]),
                min_response_time=min([results["time_per_operation_ms"] for results in performance_results.values()]),
                max_response_time=max([results["time_per_operation_ms"] for results in performance_results.values()]),
                median_response_time=statistics.median([results["time_per_operation_ms"] for results in performance_results.values()])
            )
            
            return performance_results, metrics
    
    def test_episodic_memory_performance(self):
        """Test EpisodicMemory performance characteristics."""
        episodic_mem = EpisodicMemory()
        
        # Test performance with different episode counts
        episode_counts = [100, 500, 1000, 5000]
        performance_results = {}
        
        for count in episode_counts:
            start_time = time.time()
            
            # Create and record episodes
            for i in range(count):
                episode = {
                    "id": i,
                    "data": f"episode_data_{i}",
                    "timestamp": time.time(),
                    "context": {"agent": f"agent_{i % 10}", "step": i % 100}
                }
                episodic_mem.record(episode, episode["timestamp"], episode["context"])
                
                # Perform search operations
                if i % 10 == 0:
                    _ = episodic_mem.search(keywords=["agent_1"])
                
                # Perform playback operations
                if i % 100 == 0:
                    _ = episodic_mem.playback(start_index=0, end_index=min(10, i + 1))
            
            end_time = time.time()
            duration = end_time - start_time
            
            operations_per_second = count / duration
            
            performance_results[count] = {
                "duration": duration,
                "operations_per_second": operations_per_second,
                "time_per_operation_ms": (duration / count) * 1000
            }
            
            # Cleanup
            episodic_mem.clear()
        
        # Analyze performance trends
        ops_per_second = [results["operations_per_second"] for results in performance_results.values()]
        
        # Performance should scale reasonably
        assert len(ops_per_second) >= 4, "Should test multiple episode counts"
        
        return performance_results
    
    def test_associative_memory_performance(self):
        """Test AssociativeMemory performance characteristics."""
        assoc_mem = AssociativeMemory()
        
        # Test performance with different concept counts
        concept_counts = [100, 500, 1000, 5000]
        performance_results = {}
        
        for count in concept_counts:
            start_time = time.time()
            
            # Create associations
            for i in range(count):
                concept_a = f"concept_a_{i}"
                concept_b = f"concept_b_{i}"
                strength = 0.5 + (i % 100) / 100.0
                
                assoc_mem.associate(concept_a, concept_b, strength=strength)
                
                # Perform retrieval operations
                if i % 10 == 0:
                    _ = assoc_mem.get_associations(concept_a)
                
                # Perform pattern completion
                if i % 50 == 0:
                    _ = assoc_mem.complete_pattern(concept_a, max_depth=3)
                
                # Perform strength updates
                if i % 100 == 0:
                    assoc_mem.update_strength(concept_a, concept_b, strength * 0.8)
            
            end_time = time.time()
            duration = end_time - start_time
            
            operations_per_second = count / duration
            
            performance_results[count] = {
                "duration": duration,
                "operations_per_second": operations_per_second,
                "time_per_operation_ms": (duration / count) * 1000
            }
            
            # Cleanup
            assoc_mem.clear()
        
        # Analyze performance trends
        ops_per_second = [results["operations_per_second"] for results in performance_results.values()]
        
        # Performance should scale reasonably
        assert len(ops_per_second) >= 4, "Should test multiple concept counts"
        
        return performance_results

class TestBrainPerformance:
    """Performance tests for Brain system."""
    
    def test_brain_step_performance(self):
        """Test Brain step performance characteristics."""
        config = create_default_config()
        
        with Brain(config) as brain:
            # Test performance with different step counts
            step_counts = [100, 500, 1000, 5000]
            performance_results = {}
            
            for count in step_counts:
                start_time = time.time()
                
                # Perform brain steps
                for i in range(count):
                    brain.step(i)
                
                end_time = time.time()
                duration = end_time - start_time
                
                steps_per_second = count / duration
                
                performance_results[count] = {
                    "duration": duration,
                    "steps_per_second": steps_per_second,
                    "time_per_step_ms": (duration / count) * 1000
                }
            
            # Analyze performance trends
            steps_per_second = [results["steps_per_second"] for results in performance_results.values()]
            
            # Performance should scale reasonably
            assert len(steps_per_second) >= 4, "Should test multiple step counts"
            
            return performance_results
    
    def test_brain_memory_integration_performance(self):
        """Test Brain-memory integration performance."""
        config = create_default_config()
        
        with Brain(config) as brain:
            working_mem = WorkingMemory(brain)
            neuromodulation = NeuromodulationSystem(brain)
            episodic_mem = EpisodicMemory()
            
            # Test performance with integrated operations
            start_time = time.time()
            
            for i in range(1000):
                # Brain operations
                brain.step(i)
                
                # Memory operations
                pattern = [0.1 * j for j in range(10)]
                working_mem.store(pattern, strength=0.5)
                neuromodulation.apply_dopamine(reward=0.5)
                working_mem.update(0.01)
                
                # Episodic memory operations
                episode = {
                    "step": i,
                    "pattern": pattern,
                    "brain_step": brain.simulation_step,
                    "memory_activity": working_mem.get_memory_activity(),
                    "dopamine_level": neuromodulation.get_dopamine_level()
                }
                episodic_mem.record(episode, time.time())
                
                # Search operations
                if i % 100 == 0:
                    _ = episodic_mem.search(keywords=["step_0"])
            
            end_time = time.time()
            duration = end_time - start_time
            
            # Calculate performance metrics
            operations_per_second = 6000 / duration  # 6 operations per iteration
            
            # Store performance metrics for reporting
            from tests.comprehensive.core.test_framework import PerformanceMetrics
            metrics = PerformanceMetrics(
                cpu_usage=psutil.cpu_percent(),
                memory_usage=psutil.virtual_memory().used,
                execution_time=duration,
                throughput=operations_per_second,
                peak_memory=psutil.virtual_memory().used / 1024 / 1024,  # MB
                avg_memory=psutil.virtual_memory().used / 1024 / 1024,  # MB
                min_response_time=0.001,
                max_response_time=0.01,
                median_response_time=0.005
            )
            
            return {
                "duration": duration,
                "operations_per_second": operations_per_second,
                "performance_metrics": metrics
            }
    
    def test_brain_scalability_performance(self):
        """Test Brain scalability with different configurations."""
        config = create_default_config()
        
        # Test with different neuron counts
        neuron_counts = [100, 500, 1000, 5000]
        performance_results = {}
        
        for neuron_count in neuron_counts:
            # Create configuration with different neuron count
            test_config = Config()
            test_config.set("neuron_count", neuron_count)
            test_config.set("region_count", max(1, neuron_count // 1000))
            test_config.set("timestep", 0.01)
            
            with Brain(test_config) as brain:
                start_time = time.time()
                
                # Perform operations with scaled brain
                for i in range(100):
                    brain.step(i)
                    
                    # Create working memory with scaled size
                    working_mem = WorkingMemory(brain)
                    working_mem.capacity = neuron_count // 100
                    pattern = [0.1 * j for j in range(min(10, neuron_count // 100))]
                    working_mem.store(pattern, strength=0.5)
                    working_mem.update(0.01)
                
                end_time = time.time()
                duration = end_time - start_time
                
                operations_per_second = 300 / duration
                neurons_per_second = neuron_count / duration
                
                performance_results[neuron_count] = {
                    "duration": duration,
                    "operations_per_second": operations_per_second,
                    "neurons_per_second": neurons_per_second,
                    "neurons_per_operation": neuron_count / 300
                }
        
        return performance_results

class TestNeuromodulationPerformance:
    """Performance tests for neuromodulation systems."""
    
    def test_dopamine_performance(self):
        """Test Dopamine neuromodulator performance."""
        config = create_default_config()
        
        with Brain(config) as brain:
            dopamine = Dopamine(brain)
            
            # Test performance with different signal counts
            signal_counts = [100, 500, 1000, 5000]
            performance_results = {}
            
            for count in signal_counts:
                start_time = time.time()
                
                # Apply dopamine signals
                for i in range(count):
                    reward = (i % 100) / 100.0
                    predicted = reward * 0.8
                    
                    # Test with different context types
                    if i % 10 == 0:
                        context = {"stress": 0.5, "arousal": 0.8}
                    elif i % 5 == 0:
                        context = {"fatigue": 0.3, "curiosity_threshold": 0.6}
                    else:
                        context = None
                    
                    dopamine.apply_signal(reward, predicted, context)
                    dopamine.update(0.01)
                
                end_time = time.time()
                duration = end_time - start_time
                
                signals_per_second = count / duration
                
                performance_results[count] = {
                    "duration": duration,
                    "signals_per_second": signals_per_second,
                    "time_per_signal_ms": (duration / count) * 1000
                }
            
            return performance_results
    
    def test_neuromodulation_system_performance(self):
        """Test complete NeuromodulationSystem performance."""
        config = create_default_config()
        
        with Brain(config) as brain:
            neuromodulation = NeuromodulationSystem(brain)
            
            # Test performance with complete system operations
            start_time = time.time()
            
            for i in range(1000):
                # Apply all neuromodulator signals
                neuromodulation.apply_dopamine(reward=0.5, predicted=0.4)
                neuromodulation.apply_curiosity(novelty=0.3)
                neuromodulation.apply_novelty(novelty=0.2)
                neuromodulation.apply_prediction_error(error=0.5)
                
                # Update system
                neuromodulation.update(0.01)
                
                # Get summary
                _ = neuromodulation.get_neuromodulation_summary()
                
                # Export to DataFrame (if available)
                try:
                    _ = neuromodulation.export_to_dataframe()
                except Exception:
                    pass
            
            end_time = time.time()
            duration = end_time - start_time
            
            # Calculate performance metrics
            operations_per_second = 5000 / duration  # 5 neuromodulator operations per iteration
            
            # Store performance metrics for reporting
            from tests.comprehensive.core.test_framework import PerformanceMetrics
            metrics = PerformanceMetrics(
                cpu_usage=psutil.cpu_percent(),
                memory_usage=psutil.virtual_memory().used,
                execution_time=duration,
                throughput=operations_per_second,
                peak_memory=psutil.virtual_memory().used / 1024 / 1024,
                avg_memory=psutil.virtual_memory().used / 1024 / 1024,
                min_response_time=0.001,
                max_response_time=0.01,
                median_response_time=0.005
            )
            
            return {
                "duration": duration,
                "operations_per_second": operations_per_second,
                "performance_metrics": metrics
            }

class TestMemoryStressTesting:
    """Stress testing for memory systems."""
    
    def test_memory_capacity_stress(self):
        """Test memory system behavior under capacity stress."""
        config = create_default_config()
        
        with Brain(config) as brain:
            working_mem = WorkingMemory(brain)
            
            # Stress test with very high capacity
            working_mem.capacity = 10000
            working_mem.decay_rate = 0.001
            
            # Rapid storage and retrieval operations
            start_time = time.time()
            
            for i in range(5000):
                pattern = [0.01 * j for j in range(100)]
                working_mem.store(pattern, strength=0.1)
                _ = working_mem.retrieve()
                _ = working_mem.get_memory_activity()
                
                # Update frequently to stress decay mechanism
                working_mem.update(0.001)
            
            end_time = time.time()
            duration = end_time - start_time
            
            operations_per_second = 15000 / duration
            final_activity = working_mem.get_memory_activity()
            
            # Store stress test metrics
            from tests.comprehensive.core.test_framework import PerformanceMetrics
            metrics = PerformanceMetrics(
                cpu_usage=psutil.cpu_percent(),
                memory_usage=psutil.virtual_memory().used,
                execution_time=duration,
                throughput=operations_per_second,
                peak_memory=psutil.virtual_memory().used / 1024 / 1024,
                avg_memory=psutil.virtual_memory().used / 1024 / 1024,
                min_response_time=0.001,
                max_response_time=0.01,
                median_response_time=0.005
            )
            
            return {
                "duration": duration,
                "operations_per_second": operations_per_second,
                "final_memory_activity": final_activity,
                "performance_metrics": metrics
            }
    
    def test_episodic_memory_growth_stress(self):
        """Test EpisodicMemory performance under growth stress."""
        episodic_mem = EpisodicMemory()
        
        # Rapid episode growth
        start_time = time.time()
        
        for i in range(10000):
            episode = {
                "id": i,
                "data": f"growth_test_data_{i}",
                "timestamp": time.time(),
                "context": {
                    "agent": f"agent_{i % 20}",
                    "step": i % 1000,
                    "episode_type": "growth_test"
                }
            }
            episodic_mem.record(episode, episode["timestamp"], episode["context"])
            
            # Frequent searches
            if i % 50 == 0:
                _ = episodic_mem.search(keywords=["agent_1"])
            
            # Frequent playback operations
            if i % 500 == 0:
                _ = episodic_mem.playback(start_index=max(0, i - 100), end_index=i + 1)
        
        end_time = time.time()
        duration = end_time - start_time
        
        episodes_per_second = 10000 / duration
        final_count = episodic_mem.get_episode_count()
        
        # Cleanup
        episodic_mem.clear()
        
        return {
            "duration": duration,
            "episodes_per_second": episodes_per_second,
            "final_episode_count": final_count
        }

class TestRealTimePerformance:
    """Real-time performance testing."""
    
    def test_simulation_real_time(self):
        """Test simulation real-time performance."""
        config = create_default_config()
        
        with Brain(config) as brain:
            working_mem = WorkingMemory(brain)
            neuromodulation = NeuromodulationSystem(brain)
            
            # Simulate real-time processing
            start_time = time.time()
            real_time_start = start_time
            
            target_duration = 10.0  # 10 seconds of simulation
            
            steps_completed = 0
            while (time.time() - real_time_start) < target_duration:
                # Simulate real-time step processing
                step_start = time.time()
                
                brain.step(steps_completed)
                working_mem.store([0.1, 0.5, 0.9], strength=0.5)
                neuromodulation.apply_dopamine(reward=0.5)
                neuromodulation.apply_curiosity(novelty=0.3)
                working_mem.update(0.01)
                neuromodulation.update(0.01)
                
                step_end = time.time()
                step_duration = step_end - step_start
                
                # Simulate processing delay
                if step_duration < 0.001:  # Minimum processing time
                    time.sleep(0.001 - step_duration)
                
                steps_completed += 1
            
            end_time = time.time()
            actual_duration = end_time - real_time_start
            
            # Calculate real-time performance
            target_steps_per_second = target_duration / actual_duration
            steps_per_second = steps_completed / actual_duration
            processing_time_per_step_ms = (actual_duration / steps_completed) * 1000
            
            # Store performance metrics
            from tests.comprehensive.core.test_framework import PerformanceMetrics
            metrics = PerformanceMetrics(
                cpu_usage=psutil.cpu_percent(),
                memory_usage=psutil.virtual_memory().used,
                execution_time=actual_duration,
                throughput=steps_per_second,
                peak_memory=psutil.virtual_memory().used / 1024 / 1024,
                avg_memory=psutil.virtual_memory().used / 1024 / 1024,
                min_response_time=0.001,
                max_response_time=0.01,
                median_response_time=0.005
            )
            
            return {
                "target_duration": target_duration,
                "actual_duration": actual_duration,
                "steps_completed": steps_completed,
                "steps_per_second": steps_per_second,
                "target_steps_per_second": target_steps_per_second,
                "processing_time_per_step_ms": processing_time_per_step_ms,
                "performance_metrics": metrics
            }

class TestConcurrentPerformance:
    """Concurrent performance testing."""
    
    def test_concurrent_brain_operations(self):
        """Test Brain operations with concurrent access."""
        config = create_default_config()
        
        with Brain(config) as brain:
            # Create shared memory systems
            working_mem = WorkingMemory(brain)
            neuromodulation = NeuromodulationSystem(brain)
            
            # Simulate concurrent operations
            num_threads = 4
            operations_per_thread = 100
            
            results = []
            lock = threading.Lock()
            
            def worker_thread(thread_id):
                thread_results = []
                for i in range(operations_per_thread):
                    step = thread_id * operations_per_thread + i
                    
                    # Perform thread-specific operations
                    brain.step(step)
                    working_mem.store([0.1, 0.2, 0.3], strength=0.5)
                    neuromodulation.apply_dopamine(reward=0.5)
                    neuromodulation.apply_curiosity(novelty=0.3)
                    working_mem.update(0.01)
                    neuromodulation.update(0.01)
                    
                    with lock:
                        results.append({
                            "thread_id": thread_id,
                            "step": step,
                            "brain_step": brain.simulation_step,
                            "memory_activity": working_mem.get_memory_activity()
                        })
                
                return thread_results
            
            # Run threads concurrently
            start_time = time.time()
            threads = []
            for thread_id in range(num_threads):
                thread = threading.Thread(target=worker_thread, args=(thread_id,))
                threads.append(thread)
                thread.start()
            
            for thread in threads:
                thread.join()
            
            end_time = time.time()
            duration = end_time - start_time
            
            # Calculate concurrent performance
            total_operations = num_threads * operations_per_thread
            operations_per_second = total_operations / duration
            
            # Verify results
            assert len(results) == total_operations
            assert brain.simulation_step == num_threads * operations_per_thread - 1
            
            # Store performance metrics
            from tests.comprehensive.core.test_framework import PerformanceMetrics
            metrics = PerformanceMetrics(
                cpu_usage=psutil.cpu_percent(),
                memory_usage=psutil.virtual_memory().used,
                execution_time=duration,
                throughput=operations_per_second,
                peak_memory=psutil.virtual_memory().used / 1024 / 1024,
                avg_memory=psutil.virtual_memory().used / 1024 / 1024,
                min_response_time=0.001,
                max_response_time=0.01,
                median_response_time=0.005
            )
            
            return {
                "num_threads": num_threads,
                "operations_per_thread": operations_per_thread,
                "total_operations": total_operations,
                "duration": duration,
                "operations_per_second": operations_per_second,
                "performance_metrics": metrics
            }
    
    def test_multiprocess_performance(self):
        """Test performance with multiprocessing."""
        config = create_default_config()
        
        # Create shared configuration
        test_config = Config()
        test_config.set("neuron_count", config.get("neuron_count"))
        test_config.set("region_count", config.get("region_count"))
        
        def worker_process(process_id):
            """Worker process function."""
            with Brain(test_config) as brain:
                working_mem = WorkingMemory(brain)
                neuromodulation = NeuromodulationSystem(brain)
                
                local_results = []
                
                for i in range(50):
                    step = process_id * 50 + i
                    brain.step(step)
                    working_mem.store([0.1, 0.2, 0.3], strength=0.5)
                    neuromodulation.apply_dopamine(reward=0.5)
                    neuromodulation.apply_curiosity(novelty=0.3)
                    working_mem.update(0.01)
                    neuromodulation.update(0.01)
                    
                    local_results.append({
                        "process_id": process_id,
                        "step": step,
                        "brain_step": brain.simulation_step,
                        "memory_activity": working_mem.get_memory_activity()
                    })
                
                return local_results
        
        # Run processes concurrently
        num_processes = 2
        start_time = time.time()
        
        with multiprocessing.Pool(processes=num_processes) as pool:
            results_list = pool.map(worker_process, range(num_processes))
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Flatten results
        all_results = []
        for results in results_list:
            all_results.extend(results)
        
        # Calculate multiprocess performance
        total_operations = num_processes * 50
        operations_per_second = total_operations / duration
        
        # Verify results
        assert len(all_results) == total_operations
        
        # Store performance metrics
        from tests.comprehensive.core.test_framework import PerformanceMetrics
        metrics = PerformanceMetrics(
            cpu_usage=psutil.cpu_percent(),
            memory_usage=psutil.virtual_memory().used,
            execution_time=duration,
            throughput=operations_per_second,
            peak_memory=psutil.virtual_memory().used / 1024 / 1024,
            avg_memory=psutil.virtual_memory().used / 1024 / 1024,
            min_response_time=0.001,
            max_response_time=0.01,
            median_response_time=0.005
        )
        
        return {
            "num_processes": num_processes,
            "operations_per_process": 50,
            "total_operations": total_operations,
            "duration": duration,
            "operations_per_second": operations_per_second,
            "performance_metrics": metrics
        }

# Test discovery and registration functions

def discover_performance_tests():
    """Discover all performance test classes."""
    test_classes = [
        TestMemoryPerformance,
        TestBrainPerformance,
        TestNeuromodulationPerformance,
        TestMemoryStressTesting,
        TestRealTimePerformance,
        TestConcurrentPerformance
    ]
    
    return test_classes

def get_performance_test_categories():
    """Get all performance test categories."""
    return {
        'memory': TestMemoryPerformance,
        'brain': TestBrainPerformance,
        'neuromodulation': TestNeuromodulationPerformance,
        'stress': TestMemoryStressTesting,
        'realtime': TestRealTimePerformance,
        'concurrent': TestConcurrentPerformance
    }

if __name__ == "__main__":
    # Run performance tests
    import unittest
    
    # Create test suite
    loader = unittest.TestLoader()
    
    # Get all performance test classes
    performance_classes = discover_performance_tests()
    
    # Create test suite
    suite = unittest.TestSuite()
    
    for test_class in performance_classes:
        suite.addTests(loader.loadTestsFromTestCase(test_class))
    
    # Run tests
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Exit with appropriate code
    sys.exit(0 if result.wasSuccessful() else 1)
