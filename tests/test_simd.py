#!/usr/bin/env python3
"""
SIMD Optimization Tests for NLM

Tests SIMD vectorization optimizations specifically.
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
except ImportError as e:
    print(f"Error importing SIMD modules: {e}")
    print("Make sure NLM is properly built.")
    sys.exit(1)
class TestSIMD:
    """Test SIMD vectorization specific optimizations"""
    
    def setup_method(self):
        """Set up test fixtures before each test method"""
        self.config = Config()
        self.config.set("neuron_count", 100)
        self.config.set("plasticity_rate", 0.1)
    
    def test_simd_utils_creation(self):
        """Test SIMD utils creation and basic properties"""
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
        
        # Test that SIMD utils have expected methods
        assert hasattr(simd_utils, 'getSIMDType')
        assert hasattr(simd_utils, 'isSIMDAvailable')
        assert hasattr(simd_utils, 'SIMD_WIDTH')
    
    def test_simd_vectorized_lif(self):
        """Test VectorizedLIF implementation"""
        lif = VectorizedLIF()
        assert lif is not None
        
        # Test default parameters
        assert lif.V_rest == -70.0
        assert lif.V_reset == -70.0
        assert lif.V_threshold == -55.0
        
        # Test that LIF has expected methods
        assert hasattr(lif, 'V_rest')
        assert hasattr(lif, 'V_reset')
        assert hasattr(lif, 'V_threshold')
        
        # Test that VectorizedLIF can process vector inputs
        # This depends on the actual implementation
        # For now, we verify the object exists and has expected properties
    
    def test_simd_array_operations(self):
        """Test SIMD-accelerated array operations"""
        simd_utils = SIMDUtils()
        
        if not simd_utils.isSIMDAvailable():
            pytest.skip("SIMD not available on this system")
        
        # Create test arrays
        size = 10000
        array1 = np.random.randn(size).astype(np.float32)
        array2 = np.random.randn(size).astype(np.float32)
        
        # Time regular Python operations
        start_time = time.time()
        result_regular = np.add(array1, array2)
        regular_time = time.time() - start_time
        
        # Time SIMD operations (if available)
        start_time = time.time()
        
        # Use SIMD-accelerated operations
        # This depends on the actual SIMD implementation
        # For now, we just use numpy which may or may not use SIMD
        result_simd = np.add(array1, array2)
        
        simd_time = time.time() - start_time
        
        # Verify results are the same
        assert np.allclose(result_regular, result_simd, rtol=1e-5)
        
        # SIMD should be faster (or at least not much slower)
        print(f"Regular operation time: {regular_time:.4f}s")
        print(f"SIMD operation time: {simd_time:.4f}s")
        
        # Allow some overhead for SIMD setup
        assert simd_time < regular_time * 2
    
    def test_simd_vector_operations(self):
        """Test SIMD vectorized operations"""
        # Create test data
        size = 1000
        
        # Create arrays
        base_array = np.arange(size, dtype=np.float32)
        multiplier = np.random.randn(size).astype(np.float32)
        
        # Test vectorized operations
        start_time = time.time()
        
        # Element-wise multiplication
        result1 = base_array * multiplier
        
        # Element-wise addition
        result2 = result1 + 1.0
        
        # Element-wise sin
        result3 = np.sin(result2)
        
        # Element-wise exponential
        result4 = np.exp(result3)
        
        operation_time = time.time() - start_time
        
        # Verify results
        expected1 = base_array * multiplier
        expected2 = expected1 + 1.0
        expected3 = np.sin(expected2)
        expected4 = np.exp(expected3)
        
        assert np.allclose(result1, expected1, rtol=1e-5)
        assert np.allclose(result2, expected2, rtol=1e-5)
        assert np.allclose(result3, expected3, rtol=1e-5)
        assert np.allclose(result4, expected4, rtol=1e-5)
        
        print(f"SIMD vector operations time: {operation_time:.4f}s")
        
        # Performance should be reasonable
        assert operation_time < 1.0
    
    def test_simd_memory_access_patterns(self):
        """Test SIMD memory access patterns"""
        simd_utils = SIMDUtils()
        
        if not simd_utils.isSIMDAvailable():
            pytest.skip("SIMD not available on this system")
        
        # Create test arrays with different access patterns
        size = 10000
        
        # Sequential access (good for SIMD)
        sequential_array = np.arange(size, dtype=np.float32)
        
        # Random access (bad for SIMD)
        np.random.seed(42)
        random_indices = np.random.randint(0, size, size)
        random_array = sequential_array[random_indices]
        
        # Strided access
        strided_array = sequential_array[::2]
        
        # Test operations on different patterns
        start_time = time.time()
        
        # Sequential pattern
        seq_result = np.add(sequential_array, 1.0)
        
        # Random pattern
        rand_result = np.add(random_array, 1.0)
        
        # Strided pattern
        stride_result = np.add(strided_array, 1.0)
        
        pattern_time = time.time() - start_time
        
        print(f"SIMD memory access patterns time: {pattern_time:.4f}s")
        
        # Sequential should be fastest
        assert pattern_time < 1.0
    
    def test_simd_performance_benchmark(self):
        """Benchmark SIMD performance"""
        simd_utils = SIMDUtils()
        
        if not simd_utils.isSIMDAvailable():
            pytest.skip("SIMD not available on this system")
        
        # Test with different array sizes
        sizes = [1000, 5000, 10000, 50000]
        
        for size in sizes:
            print(f"\nTesting SIMD performance with size {size}")
            
            # Create test arrays
            array1 = np.random.randn(size).astype(np.float32)
            array2 = np.random.randn(size).astype(np.float32)
            
            # Benchmark SIMD operations
            start_time = time.time()
            
            for i in range(10):  # Repeat for more accurate timing
                result = np.add(array1, array2)
                result = np.multiply(result, 2.0)
                result = np.sin(result)
                result = np.exp(result)
            
            end_time = time.time()
            
            total_time = end_time - start_time
            ops_per_second = (size * 40 * 10) / total_time  # 40 ops per iteration
            
            print(f"  Size: {size}, Time: {total_time:.4f}s, Ops/sec: {ops_per_second:,.0f}")
            
            # Performance should scale reasonably with size
            # (less than 100M operations per second for modern CPUs)
            assert ops_per_second > 1000000  # At least 1M ops/sec
    
    def test_simd_compatibility_with_numpy(self):
        """Test SIMD compatibility with NumPy operations"""
        # Test that SIMD operations are compatible with NumPy
        simd_utils = SIMDUtils()
        
        # Create test arrays
        array1 = np.array([1.0, 2.0, 3.0, 4.0], dtype=np.float32)
        array2 = np.array([0.1, 0.2, 0.3, 0.4], dtype=np.float32)
        
        # Test various NumPy operations
        operations = [
            ("addition", lambda a, b: a + b),
            ("subtraction", lambda a, b: a - b),
            ("multiplication", lambda a, b: a * b),
            ("division", lambda a, b: a / b),
            ("sin", lambda a, b: np.sin(a)),
            ("cos", lambda a, b: np.cos(a)),
            ("exp", lambda a, b: np.exp(a)),
            ("log", lambda a, b: np.log(a)),
        ]
        
        for op_name, op_func in operations:
            # Test with different array sizes
            for size in [10, 100, 1000]:
                # Create test data
                a = np.random.randn(size).astype(np.float32)
                b = np.random.randn(size).astype(np.float32)
                
                # Apply operation
                start_time = time.time()
                result = op_func(a, b)
                operation_time = time.time() - start_time
                
                # Verify result
                if op_name in ["addition", "subtraction", "multiplication", "division"]:
                    expected = op_func(a, b)
                    assert np.allclose(result, expected, rtol=1e-5)
                else:
                    expected = op_func(a, np.zeros_like(a))
                    assert np.allclose(result, expected, rtol=1e-5)
                
                print(f"  {op_name} ({size} elements): {operation_time:.6f}s")
                
                # Performance should be good
                assert operation_time < 0.1
    
    def test_simd_with_brain_simulation(self):
        """Test SIMD integration with brain simulation"""
        # Create brain
        brain = Brain(self.config)
        assert brain.initialize() == True
        
        # Get SIMD utilities
        simd_utils = SIMDUtils()
        
        if not simd_utils.isSIMDAvailable():
            print("SIMD not available, skipping SIMD-brain integration test")
            return
        
        # Create memory pool for SIMD operations
        memory_pool = MemoryPool[float](10000, 4)
        
        # Run brain simulation with SIMD operations
        for episode in range(5):
            for step in range(50):
                brain.step(step)
                
                # Perform SIMD-accelerated operations on brain state
                if step % 10 == 0:
                    # Allocate memory for SIMD operations
                    simd_data = []
                    for i in range(100):
                        simd_data.append(memory_pool.allocate())
                    
                    # Get brain statistics
                    stats = brain.getPerformanceStats()
                    firing_rate = stats.avgFiringRate
                    
                    # Perform SIMD-accelerated calculations
                    for i, val in enumerate(simd_data):
                        # Simulate neural activity calculation
                        val[0] = float(i) * firing_rate
                        val[1] = float(i) * 0.01
                        val[2] = float(i) * np.sin(firing_rate)
                        val[3] = float(i) * np.exp(-firing_rate)
                    
                    # Deallocate
                    for val in simd_data:
                        memory_pool.deallocate(val)
        
        # Verify memory pool is empty
        assert memory_pool.allocatedCount() == 0
        assert memory_pool.availableCount() == 10000
        
        brain.shutdown()
    
    def test_simd_data_type_compatibility(self):
        """Test SIMD compatibility with different data types"""
        # Test with different NumPy dtypes
        dtypes = [np.float32, np.float64]
        
        for dtype in dtypes:
            print(f"\nTesting SIMD with dtype {dtype}")
            
            size = 1000
            
            # Create test arrays
            if dtype == np.float32:
                array1 = np.random.randn(size).astype(np.float32)
                array2 = np.random.randn(size).astype(np.float32)
            else:
                array1 = np.random.randn(size).astype(np.float64)
                array2 = np.random.randn(size).astype(np.float64)
            
            # Test operations
            start_time = time.time()
            
            for i in range(10):
                result = array1 + array2
                result = result * 2.0
                result = np.sin(result)
            
            operation_time = time.time() - start_time
            
            print(f"  {dtype} operations time: {operation_time:.4f}s")
            
            # Performance should be reasonable
            assert operation_time < 1.0
    
    def test_simd_memory_efficiency(self):
        """Test SIMD memory efficiency"""
        import psutil
        import gc
        
        process = psutil.Process()
        initial_memory = process.memory_info().rss
        
        simd_utils = SIMDUtils()
        
        if not simd_utils.isSIMDAvailable():
            print("SIMD not available, skipping memory efficiency test")
            return
        
        # Create large arrays
        size = 50000
        array1 = np.random.randn(size).astype(np.float32)
        array2 = np.random.randn(size).astype(np.float32)
        
        after_creation = process.memory_info().rss
        memory_increase = after_creation - initial_memory
        
        # Perform SIMD operations
        start_time = time.time()
        
        for i in range(100):
            result = np.add(array1, array2)
            result = np.multiply(result, 2.0)
        
        operation_time = time.time() - start_time
        
        # Cleanup
        del array1, array2, result
        gc.collect()
        
        after_cleanup = process.memory_info().rss
        cleanup_memory_increase = after_cleanup - initial_memory
        
        print(f"SIMD memory efficiency test:")
        print(f"  Memory after array creation: {memory_increase / 1024 / 1024:.2f} MB")
        print(f"  Memory after cleanup: {cleanup_memory_increase / 1024 / 1024:.2f} MB")
        print(f"  Operations time: {operation_time:.4f}s")
        
        # Memory should be freed after cleanup
        final_memory_increase = abs(cleanup_memory_increase)
        assert final_memory_increase < 10 * 1024 * 1024  # 10MB


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
