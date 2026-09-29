#!/usr/bin/env python3
"""
Memory Pool Tests for NLM

Tests memory pool optimizations specifically.
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
except ImportError as e:
    print(f"Error importing memory pool modules: {e}")
    print("Make sure NLM is properly built.")
    sys.exit(1)
class TestMemoryPools:
    """Test memory pool specific optimizations"""
    
    def setup_method(self):
        """Set up test fixtures before each test method"""
        self.config = Config()
        self.config.set("neuron_count", 100)
        self.config.set("plasticity_rate", 0.1)
    
    def test_memory_pool_creation_and_basic_operations(self):
        """Test memory pool creation and basic operations"""
        # Test float memory pool
        float_pool = MemoryPool[float](1000, 4)
        
        # Verify initial state
        assert float_pool.allocatedCount() == 0
        assert float_pool.availableCount() == 1000
        
        # Allocate memory
        floats = []
        for i in range(500):
            floats.append(float_pool.allocate())
        
        # Verify allocation
        assert float_pool.allocatedCount() == 500
        assert float_pool.availableCount() == 500
        
        # Verify we can write to allocated memory
        for i, f in enumerate(floats):
            f[0] = float(i * 1.5)
            f[1] = float(i * 2.0)
        
        # Deallocate all
        for f in floats:
            float_pool.deallocate(f)
        
        # Verify deallocation
        assert float_pool.allocatedCount() == 0
        assert float_pool.availableCount() == 1000
    
    def test_memory_pool_different_sizes(self):
        """Test memory pools with different sizes"""
        pool_sizes = [10, 50, 100, 500, 1000, 5000]
        
        for pool_size in pool_sizes:
            pool = MemoryPool[float](pool_size, 4)
            
            # Allocate all but 1
            floats = []
            for i in range(pool_size - 1):
                floats.append(pool.allocate())
            
            assert pool.allocatedCount() == pool_size - 1
            assert pool.availableCount() == 1
            
            # Allocate one more to fill pool
            last_float = pool.allocate()
            assert pool.allocatedCount() == pool_size
            assert pool.availableCount() == 0
            
            # Use all allocated memory
            for i, f in enumerate(floats):
                f[0] = float(i)
            last_float[0] = float(pool_size - 1)
            
            # Deallocate all
            for f in floats:
                pool.deallocate(f)
            pool.deallocate(last_float)
            
            assert pool.allocatedCount() == 0
            assert pool.availableCount() == pool_size
    
    def test_memory_pool_concurrent_access(self):
        """Test memory pool with concurrent access (simulated)"""
        import threading
        import queue
        
        pool = MemoryPool[float](1000, 4)
        results = queue.Queue()
        
        def worker(worker_id, num_allocations):
            allocations = []
            for i in range(num_allocations):
                f = pool.allocate()
                f[0] = float(worker_id * 1000 + i)
                allocations.append(f)
            
            # Deallocate after using
            for f in allocations:
                pool.deallocate(f)
            
            results.put((worker_id, len(allocations)))
        
        # Create multiple worker threads
        threads = []
        num_workers = 5
        allocations_per_worker = 100
        
        for i in range(num_workers):
            thread = threading.Thread(target=worker, args=(i, allocations_per_worker))
            threads.append(thread)
            thread.start()
        
        # Wait for all threads to complete
        for thread in threads:
            thread.join()
        
        # Verify results
        total_allocations = 0
        while not results.empty():
            worker_id, allocations = results.get()
            total_allocations += allocations
        
        assert total_allocations == num_workers * allocations_per_worker
        assert pool.allocatedCount() == 0
        assert pool.availableCount() == 1000
    
    def test_memory_pool_memory_usage(self):
        """Test memory pool memory usage efficiency"""
        import psutil
        import gc
        
        process = psutil.Process()
        initial_memory = process.memory_info().rss
        
        # Create large memory pool
        pool = MemoryPool[float](50000, 4)
        
        # Allocate and use memory
        floats = []
        for i in range(25000):
            f = pool.allocate()
            f[0] = float(i)
            f[1] = float(i * 2)
            floats.append(f)
        
        after_allocation = process.memory_info().rss
        allocation_memory_increase = after_allocation - initial_memory
        
        # Deallocate all
        for f in floats:
            pool.deallocate(f)
        
        gc.collect()
        after_deallocation = process.memory_info().rss
        deallocation_memory_increase = after_deallocation - initial_memory
        
        # Memory should be freed after deallocation
        # (Note: Python memory management may not immediately free memory)
        memory_after_cleanup = abs(deallocation_memory_increase)
        
        print(f"Memory after allocation: {allocation_memory_increase / 1024 / 1024:.2f} MB")
        print(f"Memory after deallocation: {deallocation_memory_increase / 1024 / 1024:.2f} MB")
        
        # Memory increase should be reasonable (less than 50MB for 25k floats)
        # Each float is 8 bytes (double), so 25k floats = 200KB, plus overhead
        assert allocation_memory_increase < 50 * 1024 * 1024  # 50MB
        
        # Cleanup memory pool
        del pool
        gc.collect()
        
        final_memory = process.memory_info().rss
        final_memory_increase = final_memory - initial_memory
        
        print(f"Memory after cleanup: {final_memory_increase / 1024 / 1024:.2f} MB")
        
        # Final memory should be close to initial
        assert abs(final_memory_increase) < 10 * 1024 * 1024  # 10MB
    
    def test_memory_pool_stress_test(self):
        """Stress test memory pool with rapid allocation/deallocation"""
        pool = MemoryPool[float](10000, 4)
        
        # Rapid allocation and deallocation
        iterations = 100
        allocations_per_iteration = 50
        
        for iteration in range(iterations):
            floats = []
            
            # Allocate
            for i in range(allocations_per_iteration):
                f = pool.allocate()
                f[0] = float(iteration * 1000 + i)
                floats.append(f)
            
            # Use memory
            for f in floats:
                _ = f[0] * 2.0
            
            # Deallocate
            for f in floats:
                pool.deallocate(f)
            
            # Verify pool is in expected state
            expected_allocated = 0
            assert pool.allocatedCount() == expected_allocated
            assert pool.availableCount() == 10000
        
        print(f"Completed {iterations} stress test iterations")
        assert True  # If we get here, stress test passed
    
    def test_memory_pool_different_data_types(self):
        """Test memory pools with different data types"""
        # Test int memory pool
        int_pool = MemoryPool[int](1000, 4)
        ints = []
        for i in range(500):
            ints.append(int_pool.allocate())
        
        for i, val in enumerate(ints):
            val[0] = i * 2
            val[1] = i + 100
        
        # Deallocate
        for val in ints:
            int_pool.deallocate(val)
        
        assert int_pool.allocatedCount() == 0
        assert int_pool.availableCount() == 1000
        
        # Test double memory pool
        double_pool = MemoryPool[float](2000, 8)
        doubles = []
        for i in range(1000):
            doubles.append(double_pool.allocate())
        
        for i, val in enumerate(doubles):
            val[0] = float(i) * 3.14
            val[1] = float(i) * 2.71
        
        # Deallocate
        for val in doubles:
            double_pool.deallocate(val)
        
        assert double_pool.allocatedCount() == 0
        assert double_pool.availableCount() == 2000
        
        # Test with different alignment values
        for alignment in [1, 2, 4, 8, 16]:
            pool = MemoryPool[float](1000, alignment)
            f = pool.allocate()
            f[0] = 1.0
            pool.deallocate(f)
            
            # Memory should be aligned
            assert pool.allocatedCount() == 0
    
    def test_memory_pool_integration_with_brain(self):
        """Test memory pool integration with brain simulation"""
        # Create brain
        brain = Brain(self.config)
        assert brain.initialize() == True
        
        # Create memory pool for brain operations
        memory_pool = MemoryPool[float](5000, 4)
        
        # Run brain simulation
        for episode in range(5):
            for step in range(50):
                brain.step(step)
                
                # Simulate brain using memory pool for some operations
                if step % 10 == 0:
                    # Allocate memory for temporary calculations
                    temp_values = []
                    for i in range(100):
                        temp_values.append(memory_pool.allocate())
                    
                    # Use the memory for calculations
                    for i, val in enumerate(temp_values):
                        val[0] = float(i) * brain.getAverageFiringRate()
                        val[1] = float(i) * 0.01
                    
                    # Deallocate
                    for val in temp_values:
                        memory_pool.deallocate(val)
        
        # Verify memory pool is empty
        assert memory_pool.allocatedCount() == 0
        assert memory_pool.availableCount() == 5000
        
        brain.shutdown()


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
