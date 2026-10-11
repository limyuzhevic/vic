"""Test performance of enhanced NLM Python bindings."""
import pytest
from pynlm import Brain, Config
import pynlm.utils as utils
import time

class TestPerformance:
    """Test performance characteristics and benchmarking."""
    
    def test_batch_processing_performance(self, brain):
        """Test batch processing performance."""
        brain.initialize()
        
        # Test different batch sizes
        batch_sizes = [10, 50, 100, 500]
        
        for batch_size in batch_sizes:
            start_time = time.time()
            result = brain.step_batch(batch_size)
            end_time = time.time()
            
            elapsed = end_time - start_time
            assert result == batch_size
            assert brain.spike_count == batch_size
            
            # Performance should be reasonable (less than 1 second for up to 500 steps)
            assert elapsed < 1.0, f"Batch processing of {batch_size} steps took too long: {elapsed}s"
            
            # Log performance metrics
            print(f"Batch {batch_size}: {elapsed:.4f}s, {batch_size/elapsed:.0f} steps/s")
    
    def test_batch_vs_individual_performance(self, brain):
        """Test batch vs individual step performance."""
        brain.initialize()
        
        batch_size = 100
        
        # Time individual steps
        start_time = time.time()
        for _ in range(batch_size):
            brain.step()
        individual_time = time.time() - start_time
        
        # Reset for batch test
        brain.reset()
        brain.initialize()
        
        # Time batch steps
        start_time = time.time()
        result = brain.step_batch(batch_size)
        end_time = time.time()
        batch_time = end_time - start_time
        
        # Batch should be competitive with individual
        # Allow some overhead but batch should not be extremely slower
        if batch_time > 0:
            ratio = individual_time / batch_time
            print(f"Performance ratio (individual/batch): {ratio:.2f}")
            # Batch should be at least 50% as efficient as individual
            assert ratio > 0.5, "Batch processing should be reasonably efficient"
        
        # Reset spike count for clean state
        brain.reset()
        brain.initialize()
    
    def test_benchmark_functionality(self, brain):
        """Test benchmark functionality."""
        brain.initialize()
        
        # Test benchmark with different step counts
        step_counts = [10, 50, 100]
        
        for steps in step_counts:
            benchmark_result = utils.benchmark_brain(brain, steps=steps)
            
            assert isinstance(benchmark_result, dict)
            assert 'avg_step_time_ms' in benchmark_result
            assert 'total_steps' in benchmark_result
            assert 'min_step_time_ms' in benchmark_result
            assert 'max_step_time_ms' in benchmark_result
            assert 'std_step_time_ms' in benchmark_result
            
            assert benchmark_result['total_steps'] == steps
            assert benchmark_result['avg_step_time_ms'] > 0
            assert benchmark_result['min_step_time_ms'] > 0
            assert benchmark_result['max_step_time_ms'] >= benchmark_result['min_step_time_ms']
            assert benchmark_result['std_step_time_ms'] >= 0
            
            # Verify timing consistency
            avg_time_ms = benchmark_result['avg_step_time_ms']
            assert avg_time_ms < 100, f"Average step time too high: {avg_time_ms}ms"
        
        print("✓ Benchmark functionality verified")
    
    def test_memory_usage_monitoring(self):
        """Test memory usage monitoring."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Initial memory usage
        initial_memory = utils.get_memory_usage()
        assert isinstance(initial_memory, dict)
        assert 'rss_mb' in initial_memory
        assert 'vms_mb' in initial_memory
        
        # Process more steps to see if memory grows
        run_steps(brain, 100)
        
        # Check memory after processing
        memory_after = utils.get_memory_usage()
        assert isinstance(memory_after, dict)
        
        # Memory usage should be reasonable
        assert memory_after['rss_mb'] > 0
        assert memory_after['vms_mb'] > 0
        
        # Memory growth should be minimal for stub implementation
        memory_growth = memory_after['rss_mb'] - initial_memory['rss_mb']
        print(f"Memory growth: {memory_growth:.2f} MB")
        
        # For stub implementation, memory growth should be minimal
        assert abs(memory_growth) < 50, "Memory growth too high"
    
    def test_performance_scaling(self, brain):
        """Test performance scaling with increasing complexity."""
        brain.initialize()
        
        # Test different operation counts
        operation_counts = [10, 100, 500]
        
        for count in operation_counts:
            start_time = time.time()
            
            # Run batch operations
            brain.step_batch(count)
            
            end_time = time.time()
            elapsed = end_time - start_time
            
            ops_per_second = count / elapsed if elapsed > 0 else 0
            print(f"Ops/sec for {count} operations: {ops_per_second:.0f}")
            
            # Performance should scale reasonably
            # (for stub implementation, we just check it's not catastrophic)
            assert elapsed < 5.0, f"Processing {count} operations took too long: {elapsed}s"
            assert ops_per_second > 10, f"Performance too low: {ops_per_second} ops/s"
    
    def test_error_handling_performance(self, brain):
        """Test performance impact of error handling."""
        brain.initialize()
        
        # Normal operations (no errors)
        start_time = time.time()
        for _ in range(100):
            brain.step()
        normal_time = time.time() - start_time
        
        # Operations with errors
        brain.reset()
        
        start_time = time.time()
        for _ in range(100):
            try:
                brain.reset()
                brain.step()  # Should raise NotInitializedError
            except pynlm.NotInitializedError:
                pass  # Expected
        error_time = time.time() - start_time
        
        # Error handling should have reasonable overhead
        if normal_time > 0:
            overhead_ratio = error_time / normal_time
            print(f"Error handling overhead ratio: {overhead_ratio:.2f}")
            # Error handling should not be extremely expensive
            assert overhead_ratio < 10, "Error handling too expensive"

class TestPerformanceBenchmarks:
    """Test specific benchmark scenarios."""
    
    def test_large_batch_performance(self, brain):
        """Test performance with large batch sizes."""
        brain.initialize()
        
        # Large batch test
        large_batch = 1000
        
        start_time = time.time()
        result = brain.step_batch(large_batch)
        end_time = time.time()
        
        elapsed = end_time - start_time
        assert result == large_batch
        
        # Large batch should still complete quickly
        assert elapsed < 2.0, f"Large batch ({large_batch}) took too long: {elapsed}s"
        
        ops_per_second = large_batch / elapsed
        print(f"Large batch performance: {ops_per_second:.0f} ops/s")
        
        # Should maintain decent throughput
        assert ops_per_second > 500, "Throughput too low for large batch"
    
    def test_memory_benchmark(self):
        """Test memory benchmarking."""
        config = Config()
        brain = Brain(config)
        brain.initialize()
        
        # Run benchmark multiple times for consistency
        times = []
        for _ in range(3):
            start_time = time.time()
            benchmark_result = utils.benchmark_brain(brain, steps=50)
            end_time = time.time()
            
            elapsed_ms = benchmark_result['avg_step_time_ms']
            times.append(elapsed_ms)
        
        # Results should be consistent
        avg_time = sum(times) / len(times)
        max_time = max(times)
        min_time = min(times)
        
        # Coefficient of variation should be low for consistent performance
        if avg_time > 0:
            cv = (max_time - min_time) / avg_time
            print(f"Benchmark coefficient of variation: {cv:.2f}")
            assert cv < 0.5, "Benchmark results too variable"
        
        print(f"Benchmark average: {avg_time:.2f} ms per step")

class TestEdgeCasePerformance:
    """Test edge cases for performance."""
    
    def test_single_step_performance(self, brain):
        """Test performance with single steps."""
        brain.initialize()
        
        # Single step should be very fast
        start_time = time.time()
        result = brain.step()
        end_time = time.time()
        
        elapsed = end_time - start_time
        assert result is True
        
        # Single step should complete in microseconds
        assert elapsed < 0.001, f"Single step too slow: {elapsed}s"
    
    def test_zero_batch_performance(self, brain):
        """Test performance with zero batch."""
        brain.initialize()
        
        # Zero batch should return quickly
        start_time = time.time()
        result = brain.step_batch(0)
        end_time = time.time()
        
        elapsed = end_time - start_time
        assert result == 0
        assert brain.spike_count == 0  # Should not increment
        
        # Should complete very quickly
        assert elapsed < 0.001, f"Zero batch too slow: {elapsed}s"
    
    def test_extreme_batch_performance(self, brain):
        """Test performance with extreme batch sizes."""
        brain.initialize()
        
        # Large but reasonable batch
        large_batch = 5000
        
        start_time = time.time()
        result = brain.step_batch(large_batch)
        end_time = time.time()
        
        elapsed = end_time - start_time
        assert result == large_batch
        
        # Should still complete in reasonable time
        assert elapsed < 5.0, f"Extreme batch ({large_batch}) too slow: {elapsed}s"
        
        ops_per_second = large_batch / elapsed
        print(f"Extreme batch performance: {ops_per_second:.0f} ops/s")
        
        # Should maintain good throughput
        assert ops_per_second > 1000, "Throughput too low for extreme batch"

# Helper function (used by performance tests)
def run_steps(brain, steps):
    """Helper to run multiple steps."""
    for _ in range(steps):
        brain.step()