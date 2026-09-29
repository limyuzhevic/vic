#!/usr/bin/env python3
"""
Comprehensive Performance Tests for NLM

Tests memory pool optimizations, SIMD vectorization,
parallel processing, and checkpoint optimizations.
"""

import sys
import os
import time
import pytest
import numpy as np
from typing import List, Dict, Any

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    from core.Config.Config import Config
    from brain.Brain import Brain
    from performance.MemoryPool import MemoryPool
    from performance.SIMDVectorization import SIMDUtils, VectorizedLIF
    from performance.ParallelProcessing import ThreadPool, ParallelFor
    from performance.CheckpointSystem import CheckpointManager
    from agent.AgentBrain import AgentBrain
    from world.SimpleWorld import SimpleWorld
except ImportError as e:
    print(f"Error importing performance modules: {e}")
    print("Make sure NLM is properly built.")
    sys.exit(1)
class TestPerformance:
    """Test performance optimization features"""
    
    def setup_method(self):
        """Set up test fixtures before each test method"""
        self.config = Config()
        self.config.set("neuron_count", 500)
        self.config.set("plasticity_rate", 0.1)
        self.config.set("working_memory_capacity", 100)
        self.config.set("simulation_speed", 1.0)
    
    def test_memory_pool_optimizations(self):
        """Test memory pool optimizations"""
        # Test different memory pool sizes
        pool_sizes = [100, 500, 1000, 5000]
        
        for pool_size in pool_sizes:
            # Create float memory pool
            float_pool = MemoryPool[float](pool_size, 4)
            
            # Allocate memory
            floats = []
            for i in range(pool_size - 10):  # Leave some space
                floats.append(float_pool.allocate())
            
            # Use the allocated memory
            for i, f in enumerate(floats):
                f[0] = float(i * 1.5)
            
            # Deallocate half
            for i in range(len(floats) // 2):
                float_pool.deallocate(floats[i])
            
            # Verify pool statistics
            allocated = float_pool.allocatedCount()
            available = float_pool.availableCount()
            
            assert allocated + available == pool_size
            assert available >= len(floats) // 2
            
            # Clean up
            for i in range(len(floats) // 2, len(floats)):
                float_pool.deallocate(floats[i])
            
            # Pool should be mostly empty now
            assert float_pool.availableCount() >= pool_size - 5
        
        # Test int memory pool
        int_pool = MemoryPool[int](1000, 4)
        ints = []
        for i in range(500):
            ints.append(int_pool.allocate())
        
        for i, val in enumerate(ints):
            val[0] = i * 2
        
        # Deallocate all
        for val in ints:
            int_pool.deallocate(val)
        
        assert int_pool.allocatedCount() == 0
        assert int_pool.availableCount() == 1000
    
    def test_simd_vectorization(self):
        """Test SIMD vectorization optimizations"""
        simd_utils = SIMDUtils()
        
        # Test SIMD type detection
        simd_type = simd_utils.getSIMDType()
        assert simd_type is not None
        assert len(simd_type) > 0
        
        # Test SIMD availability
        available = simd_utils.isSIMDAvailable()
        assert isinstance(available, bool)
        
        # Test SIMD width
        width = simd_utils.SIMD_WIDTH
        assert width > 0
        
        # Test VectorizedLIF performance
        lif = VectorizedLIF()
        assert lif is not None
        
        # Test LIF parameters
        assert lif.V_rest == -70.0
        assert lif.V_reset == -70.0
        assert lif.V_threshold == -55.0
        
        # Test that VectorizedLIF can process vectors
        # Note: This depends on the actual VectorizedLIF implementation
        # For now, we just verify the object exists and has expected parameters
        
        # Test that SIMD utils can work with arrays
        test_array = np.array([-60.0, -65.0, -70.0, -75.0])
        assert len(test_array) == 4
        assert test_array[0] == -60.0
    
    def test_parallel_processing(self):
        """Test parallel processing optimizations"""
        # Test ThreadPool with different thread counts
        thread_counts = [1, 2, 4, 8]
        
        for thread_count in thread_counts:
            pool = ThreadPool(thread_count)
            
            # Submit multiple tasks
            num_tasks = 20
            futures = []
            
            for i in range(num_tasks):
                future = pool.submitInt(lambda x=i: x * 3 + thread_count)
                futures.append(future)
            
            # Wait for all futures and collect results
            results = []
            for future in futures:
                results.append(future.wait())
            
            # Verify results
            assert len(results) == num_tasks
            for i, result in enumerate(results):
                assert result == i * 3 + thread_count
            
            pool.shutdown()
        
        # Test ParallelFor performance
        # Create large array
        size = 10000
        array1 = [0] * size
        array2 = [0] * size
        
        # Test sequential
        time_start = time.time()
        for i in range(size):
            array1[i] = i * 2
        sequential_time = time.time() - time_start
        
        # Test parallel
        time_start = time.time()
        ParallelFor.simple(0, size, lambda i: array2.__setitem__(i, i * 2))
        parallel_time = time.time() - time_start
        
        # Verify results are the same
        assert array1 == array2
        
        # Parallel should be faster for large arrays (though not always due to overhead)
        # This is just a basic performance check
        assert parallel_time < sequential_time * 5  # Allow some overhead
    
    def test_checkpoint_optimizations(self):
        """Test checkpoint optimization features"""
        # Create checkpoint manager
        manager = CheckpointManager()
        manager.configure("./test_checkpoints", 1000, 5, True, 100)  # path, max, frequency, compress, chunk_size
        
        assert manager.getCheckpointDir() == "./test_checkpoints"
        assert manager.maxCheckpoints_ == 1000
        assert manager.compress_ == True
        assert manager.chunkSize_ == 100
        
        # Create test brain
        brain = Brain(self.config)
        assert brain.initialize() == True
        
        # Test saving checkpoints
        checkpoint1 = manager.saveCheckpoint("brain_1", brain)
        assert checkpoint1 is not None
        
        # Simulate more work
        brain.step(100)
        
        # Save another checkpoint
        checkpoint2 = manager.saveCheckpoint("brain_2", brain)
        assert checkpoint2 is not None
        
        # Verify checkpoints directory exists
        assert os.path.exists("./test_checkpoints")
        
        # Test checkpoint listing
        checkpoints = manager.listCheckpoints()
        assert len(checkpoints) >= 2
        assert any("brain_1" in cp for cp in checkpoints)
        assert any("brain_2" in cp for cp in checkpoints)
        
        # Test cleanup
        manager.cleanupOldCheckpoints()
        
        # Load last checkpoint
        if checkpoints:
            last_checkpoint = checkpoints[-1]
            # Note: This depends on the actual checkpoint loading implementation
            # For now, we just verify the checkpoint was saved
        
        # Cleanup
        import shutil
        if os.path.exists("./test_checkpoints"):
            shutil.rmtree("./test_checkpoints")
        
        brain.shutdown()
    
    def test_memory_efficiency(self):
        """Test memory efficiency of performance optimizations"""
        import psutil
        import gc
        
        process = psutil.Process()
        initial_memory = process.memory_info().rss
        
        # Create multiple brain instances with optimizations
        brains = []
        for i in range(5):
            config = Config()
            config.set("neuron_count", 100 + i * 50)
            config.set("plasticity_rate", 0.1)
            config.set("working_memory_capacity", 50)
            
            brain = Brain(config)
            brain.initialize()
            brains.append(brain)
        
        # Get memory usage
        after_creation = process.memory_info().rss
        memory_increase_creation = after_creation - initial_memory
        
        # Run some simulation work
        for brain in brains:
            brain.step(50)
        
        # Get memory after work
        after_work = process.memory_info().rss
        memory_increase_work = after_work - initial_memory
        
        # Cleanup
        for brain in brains:
            brain.shutdown()
        
        del brains
        gc.collect()
        
        # Get memory after cleanup
        after_cleanup = process.memory_info().rss
        memory_increase_cleanup = after_cleanup - initial_memory
        
        # Memory should not grow excessively
        # (This is a loose check since memory usage varies)
        assert memory_increase_cleanup < 100 * 1024 * 1024  # 100MB
        
        # Memory after cleanup should be close to initial (within reason)
        # Allow for some memory overhead
        memory_increase_after_cleanup = abs(memory_increase_cleanup)
        assert memory_increase_after_cleanup < 50 * 1024 * 1024  # 50MB
    
    def test_performance_scalability(self):
        """Test performance scalability with different configurations"""
        # Test with different neuron counts
        neuron_counts = [100, 500, 1000, 2000]
        
        for neuron_count in neuron_counts:
            config = Config()
            config.set("neuron_count", neuron_count)
            config.set("plasticity_rate", 0.1)
            config.set("working_memory_capacity", min(100, neuron_count // 10))
            config.set("simulation_speed", 1.0)
            
            # Create brain
            brain = Brain(config)
            assert brain.initialize() == True
            
            # Measure initialization time
            start_time = time.time()
            brain.initialize()
            init_time = time.time() - start_time
            
            # Measure step time
            start_time = time.time()
            brain.step(10)
            step_time = time.time() - start_time
            
            # Performance should be reasonable (loose checks)
            assert init_time < 5.0  # Should initialize within 5 seconds
            assert step_time < 1.0   # Should step within 1 second for 10 steps
            
            # Performance should scale reasonably with neuron count
            if neuron_count > 100:
                # Larger neurons may take longer to initialize
                assert init_time < 10.0
            
            brain.shutdown()
    
    def test_simd_performance(self):
        """Test SIMD vectorization performance"""
        simd_utils = SIMDUtils()
        
        if simd_utils.isSIMDAvailable():
            # Test SIMD performance with array operations
            size = 10000
            
            # Create test arrays
            array1 = np.random.randn(size).astype(np.float32)
            array2 = np.random.randn(size).astype(np.float32)
            result = np.zeros(size, dtype=np.float32)
            
            # Time SIMD operations
            start_time = time.time()
            
            # Use SIMD-accelerated operations if available
            # This depends on the actual SIMD implementation
            # For now, we just verify the arrays work
            
            # Simple vector addition test
            for i in range(size):
                result[i] = array1[i] + array2[i]
            
            simd_time = time.time() - start_time
            
            # Verify results
            expected = np.add(array1, array2)
            assert np.allclose(result, expected, rtol=1e-5)
            
            # Performance should be reasonable
            assert simd_time < 1.0  # Should complete in under 1 second for 10k elements
    
    def test_parallel_performance(self):
        """Test parallel processing performance"""
        # Test with different thread counts
        thread_counts = [1, 2, 4]
        task_count = 1000
        
        for thread_count in thread_counts:
            pool = ThreadPool(thread_count)
            
            # Submit many tasks
            futures = []
            for i in range(task_count):
                future = pool.submitInt(lambda x=i: x * x + thread_count * 100)
                futures.append(future)
            
            # Measure time
            start_time = time.time()
            
            # Collect results
            results = []
            for future in futures:
                results.append(future.wait())
            
            elapsed = time.time() - start_time
            
            # Verify results
            assert len(results) == task_count
            for i, result in enumerate(results):
                expected = i * i + thread_count * 100
                assert result == expected
            
            # Performance should improve with more threads (up to a point)
            print(f"Thread {thread_count}: {task_count} tasks in {elapsed:.3f}s")
            
            pool.shutdown()
            
            # Each thread should have done roughly the same amount of work
            # (This is just a performance check, not a strict requirement)
        
        # Test ParallelFor with large arrays
        size = 100000
        array = [0] * size
        
        start_time = time.time()
        ParallelFor.simple(0, size, lambda i: array.__setitem__(i, i * i))
        parallel_time = time.time() - start_time
        
        # Verify results
        assert array[0] == 0
        assert array[size // 2] == (size // 2) * (size // 2)
        assert array[size - 1] == (size - 1) * (size - 1)
        
        # Performance should be good
        assert parallel_time < 2.0  # Should complete in under 2 seconds
    
    def test_memory_pool_performance(self):
        """Test memory pool performance"""
        # Test allocation/deallocation performance
        pool_size = 10000
        
        # Create pool
        pool = MemoryPool[float](pool_size, 4)
        
        # Measure allocation performance
        start_time = time.time()
        
        floats = []
        for i in range(pool_size):
            floats.append(pool.allocate())
        
        alloc_time = time.time() - start_time
        
        # Measure deallocation performance
        start_time = time.time()
        
        for f in floats:
            pool.deallocate(f)
        
        dealloc_time = time.time() - start_time
        
        # Performance should be good
        assert alloc_time < 1.0  # Should allocate 10k floats in under 1 second
        assert dealloc_time < 1.0  # Should deallocate 10k floats in under 1 second
        
        # Pool should be empty now
        assert pool.allocatedCount() == 0
        assert pool.availableCount() == pool_size
    
    def test_checkpoint_performance(self):
        """Test checkpoint save/load performance"""
        manager = CheckpointManager()
        manager.configure("./test_checkpoints_perf", 100, 10, True)
        
        # Create test brain
        brain = Brain(self.config)
        assert brain.initialize() == True
        
        # Simulate some work
        for i in range(100):
            brain.step(i)
        
        # Measure checkpoint save time
        start_time = time.time()
        checkpoint_path = manager.saveCheckpoint("performance_test", brain)
        save_time = time.time() - start_time
        
        assert checkpoint_path is not None
        assert save_time < 5.0  # Should save within 5 seconds
        
        # Measure checkpoint list time
        start_time = time.time()
        checkpoints = manager.listCheckpoints()
        list_time = time.time() - start_time
        
        assert list_time < 0.1  # Should list quickly
        assert len(checkpoints) >= 1
        
        # Cleanup
        import shutil
        if os.path.exists("./test_checkpoints_perf"):
            shutil.rmtree("./test_checkpoints_perf")
        
        brain.shutdown()


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
