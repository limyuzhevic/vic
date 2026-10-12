#!/usr/bin/env python3
"""
Performance test suite for NLM Python bindings.
Tests performance characteristics and benchmarking capabilities.
"""

import pytest
import sys
import os
import time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm as nlm

class TestPerformanceCharacteristics:
    """Test basic performance characteristics."""
    
    def test_config_creation_performance(self):
        """Test performance of configuration creation."""
        import time
        
        # Test creating many configs
        start_time = time.time()
        for i in range(100):
            config = nlm.Config()
            config.update(key_i=i)
        elapsed = time.time() - start_time
        
        # Should be fast (< 1 second for 100 configs)
        assert elapsed < 1.0
    
    def test_config_update_performance(self):
        """Test performance of config update operations."""
        config = nlm.Config()
        
        start_time = time.time()
        # Update with various types
        for i in range(100):
            config.update(
                string_val=f"value_{i}",
                int_val=i,
                double_val=float(i) * 1.5,
                bool_val=i % 2 == 0
            )
        elapsed = time.time() - start_time
        
        # Should handle 100 updates efficiently
        assert elapsed < 0.5
        assert config.getOr("string_val_99", "") == "value_99"
    
    def test_brain_creation_performance(self):
        """Test performance of brain creation."""
        config = nlm.create_simulation_config()
        
        start_time = time.time()
        for i in range(10):
            brain = nlm.Brain(config)
            brain.initialize()
        elapsed = time.time() - start_time
        
        # Should create and initialize 10 brains reasonably quickly
        assert elapsed < 5.0

class TestBenchmarkingPerformance:
    """Test benchmarking and measurement capabilities."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.config = nlm.create_simulation_config()
        self.brain = nlm.Brain(self.config)
        assert self.brain.initialize()
    
    def test_brain_benchmark_basic(self):
        """Test basic brain benchmarking."""
        start_time = time.time()
        
        # Run benchmark
        results = self.brain.benchmark(steps=10, warmup=5)
        
        elapsed = time.time() - start_time
        
        # Should complete quickly
        assert elapsed < 2.0
        
        # Verify results
        assert isinstance(results, dict)
        assert "total_time_seconds" in results
        assert "average_time_per_step_seconds" in results
        assert "steps_per_second" in results
        assert "steps" in results
        
        assert results["steps"] == 10
        assert results["total_time_seconds"] >= 0
        assert results["average_time_per_step_seconds"] >= 0
        assert results["steps_per_second"] > 0
    
    def test_brain_benchmark_warmup(self):
        """Test brain benchmark warmup functionality."""
        # Test with different warmup values
        results1 = self.brain.benchmark(steps=5, warmup=2)
        results2 = self.brain.benchmark(steps=5, warmup=5)
        
        # Both should complete successfully
        assert results1["steps"] == 5
        assert results2["steps"] == 5
        
        # More warmup might result in better performance (not guaranteed)
        assert isinstance(results1, dict)
        assert isinstance(results2, dict)
    
    def test_brain_benchmark_steps(self):
        """Test brain benchmark with different step counts."""
        # Test with various step counts
        for steps in [1, 5, 10, 20]:
            results = self.brain.benchmark(steps=steps, warmup=2)
            
            assert results["steps"] == steps
            assert results["total_time_seconds"] >= 0
            assert results["steps_per_second"] > 0
    
    def test_brain_benchmark_consistency(self):
        """Test consistency of benchmark results."""
        # Run benchmark multiple times
        results1 = self.brain.benchmark(steps=10, warmup=5)
        results2 = self.brain.benchmark(steps=10, warmup=5)
        
        # Results should be consistent in structure
        assert isinstance(results1, dict)
        assert isinstance(results2, dict)
        
        # Keys should be the same
        assert set(results1.keys()) == set(results2.keys())

class TestLargeScalePerformance:
    """Test performance with large-scale operations."""
    
    def test_large_config_performance(self):
        """Test performance with large configurations."""
        config = nlm.Config()
        
        start_time = time.time()
        
        # Add many configuration entries
        for i in range(1000):
            config.update(**{
                f"param_{i}": i,
                f"float_param_{i}": float(i) * 0.001,
                f"bool_param_{i}": i % 2 == 0,
                f"string_param_{i}": f"value_{i}"
            })
        
        elapsed = time.time() - start_time
        
        # Should handle 1000*4 = 4000 parameters efficiently
        assert elapsed < 2.0
        assert len(config.getKeys()) >= 1000
        
        # Test retrieval performance
        start_time = time.time()
        for i in range(1000):
            value = config.getOr(f"param_{i}", 0)
            assert value == i
        elapsed = time.time() - start_time
        
        # Should retrieve 1000 keys quickly
        assert elapsed < 0.5
    
    def test_large_brain_performance(self):
        """Test performance with large brain configurations."""
        config = nlm.create_simulation_config()
        
        # Test with different brain sizes
        for region_count in [1, 2, 3]:
            config.update(region_count=region_count)
            
            brain = nlm.Brain(config)
            assert brain.initialize()
            
            # Test simple operations
            start_time = time.time()
            brain.step(0)
            elapsed = time.time() - start_time
            
            # Should step quickly
            assert elapsed < 0.1
            
            # Test statistics
            start_time = time.time()
            neuron_count = brain.getTotalNeuronCount()
            synapse_count = brain.getTotalSynapseCount()
            firing_count = brain.getFiringNeuronCount()
            elapsed = time.time() - start_time
            
            # Should get statistics quickly
            assert elapsed < 0.1
            assert neuron_count > 0
            assert synapse_count >= 0
            assert firing_count >= 0
    
    def test_memory_usage_simulation(self):
        """Test memory usage with many objects."""
        world = nlm.create_test_environment()
        
        # Add many objects to world
        start_time = time.time()
        for i in range(1000):
            x = (i % 20) * 5.0
            y = (i // 20) * 5.0
            world.add_object_at(x, y, nlm.WorldObjectType.Resource, float(i))
        
        elapsed = time.time() - start_time
        
        # Should add 1000 objects reasonably quickly
        assert elapsed < 2.0
        
        # Test spatial queries on large world
        start_time = time.time()
        nearby = world.get_nearby_objects(50.0, 50.0, 25.0)
        elapsed = time.time() - start_time
        
        # Should query spatial data quickly
        assert elapsed < 0.5
        assert isinstance(nearby, list)

class TestOperationLatency:
    """Test operation latency measurements."""
    
    def setup_method(self):
        """Setup for each test method."""
        self.config = nlm.create_simulation_config()
        self.brain = nlm.Brain(self.config)
        assert self.brain.initialize()
        
        self.world = nlm.create_test_environment()
        self.agent_brain = nlm.AgentBrain(self.brain)
        assert self.agent_brain.initialize(self.world)
    
    def test_config_operation_latency(self):
        """Test latency of configuration operations."""
        config = nlm.Config()
        
        # Measure update latency
        times = []
        for i in range(100):
            start_time = time.time()
            config.update(value_i=i)
            elapsed = time.time() - start_time
            times.append(elapsed)
        
        avg_time = sum(times) / len(times)
        max_time = max(times)
        
        # Should be fast (< 1ms per update on average)
        assert avg_time < 0.001  # 1ms
        assert max_time < 0.01    # 10ms
    
    def test_brain_operation_latency(self):
        """Test latency of brain operations."""
        # Measure step latency
        times = []
        for i in range(50):
            start_time = time.time()
            self.brain.step(i)
            elapsed = time.time() - start_time
            times.append(elapsed)
        
        avg_time = sum(times) / len(times)
        max_time = max(times)
        
        # Should be fast (< 10ms per step)
        assert avg_time < 0.01   # 10ms
        assert max_time < 0.05   # 50ms
    
    def test_world_operation_latency(self):
        """Test latency of world operations."""
        # Measure update latency
        times = []
        for i in range(100):
            start_time = time.time()
            self.world.update(0.01)
            elapsed = time.time() - start_time
            times.append(elapsed)
        
        avg_time = sum(times) / len(times)
        max_time = max(times)
        
        # Should be fast (< 1ms per update)
        assert avg_time < 0.001  # 1ms
        assert max_time < 0.005   # 5ms

class TestScalingPerformance:
    """Test performance scaling."""
    
    def test_config_scaling(self):
        """Test how config performance scales with size."""
        import time
        
        # Test different config sizes
        for size in [10, 100, 500, 1000]:
            config = nlm.Config()
            
            start_time = time.time()
            for i in range(size):
                config.update(**{f"key_{i}": i})
            elapsed = time.time() - start_time
            
            # Should scale reasonably (O(n) or better)
            # 1000 items should complete quickly
            assert elapsed < 2.0
    
    def test_brain_scaling(self):
        """Test how brain performance scales with complexity."""
        # Test with different region counts
        for region_count in [1, 2, 3, 5]:
            config = nlm.create_simulation_config()
            config.update(region_count=region_count)
            
            brain = nlm.Brain(config)
            assert brain.initialize()
            
            # Measure step performance
            start_time = time.time()
            brain.step(0)
            elapsed = time.time() - start_time
            
            # Should handle increased complexity reasonably
            assert elapsed < 0.1
            
            # Measure statistics performance
            start_time = time.time()
            neuron_count = brain.getTotalNeuronCount()
            synapse_count = brain.getTotalSynapseCount()
            elapsed = time.time() - start_time
            
            assert elapsed < 0.01
            assert neuron_count > 0
    
    def test_world_scaling(self):
        """Test how world performance scales with object count."""
        # Test with different object counts
        for obj_count in [10, 50, 100, 200]:
            world = nlm.SimpleWorld()
            world.configure_simulation(100.0, 100.0, 32, 32)
            
            # Add objects
            for i in range(obj_count):
                x = (i % 20) * 5.0
                y = (i // 20) * 5.0
                world.add_object_at(x, y, nlm.WorldObjectType.Resource, float(i))
            
            # Measure spatial query performance
            start_time = time.time()
            nearby = world.get_nearby_objects(50.0, 50.0, 25.0)
            elapsed = time.time() - start_time
            
            # Should handle many objects and queries efficiently
            assert elapsed < 0.5
            assert isinstance(nearby, list)

if __name__ == "__main__":
    pytest.main([__file__, "-v"])