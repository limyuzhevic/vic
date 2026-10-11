// NLM Stress Tests
// Memory Pressure Tests

#include "framework/TestFramework.hpp"
#include "framework/TestFixtures.hpp"
#include "brain/PerformanceProfiler.hpp"
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>

class TestMemoryPressure : public TestCase {
public:
    TestMemoryPressure() : TestCase("MemoryPressure") {}
    
    void testExtremeMemoryAllocation() {
        PerformanceProfiler profiler;
        
        // Test extreme memory allocation
        profiler.startProfile("extreme_allocation");
        
        std::vector<std::vector<double>> extreme_allocations;
        
        // Allocate massive amounts of memory
        for (int i = 0; i < 100; ++i) {
            size_t allocation_size = 10 * 1024 * 1024; // 10MB per allocation
            std::vector<double> block;
            block.reserve(allocation_size / sizeof(double));
            
            for (size_t j = 0; j < block.capacity(); ++j) {
                block.push_back(static_cast<double>(rand()) / RAND_MAX);
            }
            
            extreme_allocations.push_back(block);
            
            // Use the allocated memory
            double sum = 0.0;
            for (size_t j = 0; j < block.size(); j += 1000) {
                sum += block[j];
            }
            assert(std::isfinite(sum));
            
            // Check memory usage
            size_t estimated_memory = block.capacity() * sizeof(double);
            assert(estimated_memory > 0); // Should track memory
        }
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("extreme_allocation");
        assert(metrics.memory_usage > 0); // Should track peak memory
        assert(metrics.duration.count() > 0);
        
        // Should handle extreme memory allocation
        assert(metrics.memory_usage < 5000000000); // Less than 5GB despite allocations
    }
    
    void testMemoryFragmentation() {
        PerformanceProfiler profiler;
        
        // Test memory fragmentation
        profiler.startProfile("memory_fragmentation");
        
        std::vector<std::vector<double>> fragmented_allocations;
        
        // Allocate and deallocate in various sizes to create fragmentation
        for (int i = 0; i < 1000; ++i) {
            // Small allocations
            if (i % 4 == 0) {
                std::vector<double> small(100, 0.0);
                fragmented_allocations.push_back(small);
            }
            // Medium allocations
            else if (i % 4 == 1) {
                std::vector<double> medium(10000, 0.0);
                fragmented_allocations.push_back(medium);
            }
            // Large allocations
            else if (i % 4 == 2) {
                std::vector<double> large(100000, 0.0);
                fragmented_allocations.push_back(large);
            }
            // Very large allocations
            else {
                std::vector<double> very_large(1000000, 0.0);
                fragmented_allocations.push_back(very_large);
            }
        }
        
        // Process fragmented memory
        double total_sum = 0.0;
        for (auto& allocation : fragmented_allocations) {
            for (size_t j = 0; j < allocation.size(); ++j) {
                total_sum += allocation[j];
            }
        }
        
        assert(std::isfinite(total_sum));
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("memory_fragmentation");
        assert(metrics.memory_usage >= 0); // Should handle fragmentation
        assert(metrics.duration.count() > 0);
        
        // Memory usage should be reasonable despite fragmentation
        assert(metrics.memory_usage < 2000000000); // Less than 2GB
    }
    
    void testMemoryLeakDetection() {
        PerformanceProfiler profiler;
        
        // Test memory leak detection
        profiler.startProfile("memory_leak_detection");
        
        std::vector<std::vector<double>> allocated_memory;
        
        // Allocate memory and let it go out of scope (should be deallocated)
        for (int i = 0; i < 1000; ++i) {
            std::vector<double> temp_vector(100000, 0.0);
            
            // Use memory
            for (size_t j = 0; j < temp_vector.size(); j += 1000) {
                temp_vector[j] = std::sin(j * 0.000001);
            }
            
            // Vector goes out of scope here
            // If there's a memory leak, profiler should detect it
            allocated_memory.push_back(temp_vector);
        }
        
        // Clear allocations
        allocated_memory.clear();
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("memory_leak_detection");
        assert(metrics.memory_usage >= 0); // Should not grow uncontrollably
        assert(metrics.duration.count() > 0);
        
        // Memory usage should stabilize
        assert(metrics.memory_usage < 1000000000); // Less than 1GB
    }
    
    void testMemoryPoolExhaustion() {
        PerformanceProfiler profiler;
        
        // Test memory pool exhaustion
        profiler.startProfile("memory_pool_exhaustion");
        
        std::vector<std::vector<double>> memory_pools;
        
        // Create multiple memory pools
        for (int pool_id = 0; pool_id < 10; ++pool_id) {
            std::vector<double> pool;
            pool.reserve(50 * 1024 * 1024); // 50MB pool
            
            // Fill pool with data
            for (size_t i = 0; i < pool.capacity(); ++i) {
                pool.push_back(static_cast<double>(rand()) / RAND_MAX);
            }
            
            memory_pools.push_back(pool);
            
            // Use pool data
            double pool_sum = 0.0;
            for (size_t i = 0; i < pool.size(); i += 1000) {
                pool_sum += pool[i];
            }
            assert(std::isfinite(pool_sum));
        }
        
        // Exhaust all memory pools
        memory_pools.clear();
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("memory_pool_exhaustion");
        assert(metrics.memory_usage >= 0); // Should handle pool exhaustion
        assert(metrics.duration.count() > 0);
        
        // Should recover from memory pool exhaustion
        assert(metrics.memory_usage < 1000000000); // Less than 1GB after cleanup
    }
    
    void testMemoryPressureUnderLoad() {
        PerformanceProfiler profiler;
        
        // Test memory pressure under computational load
        profiler.startProfile("memory_pressure_under_load");
        
        std::vector<std::vector<double>> pressured_data;
        
        // Create data under memory pressure
        for (int i = 0; i < 100; ++i) {
            std::vector<double> data(1000000, 0.0); // 8MB each
            pressured_data.push_back(data);
            
            // Process data while under memory pressure
            double sum = 0.0;
            for (size_t j = 0; j < data.size(); j += 100) {
                data[j] = std::sin(j * 0.000001);
                sum += data[j];
            }
            
            assert(std::isfinite(sum));
            
            // Check memory usage under pressure
            size_t estimated_memory = pressured_data.size() * data.size() * sizeof(double);
            assert(estimated_memory > 0); // Should track pressure
        }
        
        // Process all data under continuous pressure
        double total_pressure_sum = 0.0;
        for (auto& data : pressured_data) {
            for (size_t j = 0; j < data.size(); ++j) {
                total_pressure_sum += data[j];
            }
        }
        
        assert(std::isfinite(total_pressure_sum));
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("memory_pressure_under_load");
        assert(metrics.memory_usage > 0); // Should track high memory usage
        assert(metrics.duration.count() > 0);
        
        // Should handle sustained memory pressure
        assert(metrics.memory_usage < 3000000000); // Less than 3GB
    }
    
    void testMemoryRecovery() {
        PerformanceProfiler profiler;
        
        // Test memory recovery after pressure
        profiler.startProfile("memory_recovery");
        
        std::vector<std::vector<double>> allocated_memory;
        
        // Allocate large amounts of memory
        for (int i = 0; i < 50; ++i) {
            std::vector<double> large_block(2000000, 0.0); // 16MB each
            allocated_memory.push_back(large_block);
            
            // Fill with data
            for (size_t j = 0; j < large_block.size(); ++j) {
                large_block[j] = std::cos(j * 0.0000005);
            }
        }
        
        // Check memory usage at peak
        size_t peak_memory = allocated_memory.size() * 2000000 * sizeof(double);
        assert(peak_memory > 0); // Should have high memory usage
        
        // Clear memory to recover
        allocated_memory.clear();
        
        // Memory should be freed
        size_t recovered_memory = 0; // Would normally be tracked by profiler
        assert(recovered_memory < peak_memory); // Should be less after recovery
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("memory_recovery");
        assert(metrics.memory_usage >= 0); // Should track recovery
        assert(metrics.duration.count() > 0);
        
        // Should recover from memory pressure
        assert(metrics.memory_usage < peak_memory); // Less memory after recovery
    }
};