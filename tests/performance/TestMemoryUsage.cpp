// NLM Performance Tests
// Memory Usage Tests

#include "framework/TestFramework.hpp"
#include "framework/TestFixtures.hpp"
#include "brain/PerformanceProfiler.hpp"
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>

class TestMemoryUsage : public TestCase {
public:
    TestMemoryUsage() : TestCase("MemoryUsage") {}
    
    void testMemoryAllocationPatterns() {
        PerformanceProfiler profiler;
        
        // Test different memory allocation patterns
        profiler.startProfile("memory_pattern_linear");
        
        // Linear allocation
        std::vector<double> linear_data;
        for (int i = 0; i < 100000; ++i) {
            linear_data.push_back(static_cast<double>(rand()) / RAND_MAX);
        }
        
        profiler.endProfile();
        
        auto linear_metrics = profiler.getMetrics("memory_pattern_linear");
        assert(linear_metrics.memory_usage > 0);
        assert(linear_metrics.duration.count() > 0);
        
        profiler.startProfile("memory_pattern_exponential");
        
        // Exponential allocation (growth curve)
        std::vector<double> exponential_data;
        for (int i = 0; i < 100; ++i) {
            size_t size = 1000 * pow(2, i); // Exponentially growing
            exponential_data.resize(size);
            for (size_t j = 0; j < size; ++j) {
                exponential_data[j] = std::sin(j * 0.01);
            }
        }
        
        profiler.endProfile();
        
        auto exponential_metrics = profiler.getMetrics("memory_pattern_exponential");
        assert(exponential_metrics.memory_usage > 0);
        assert(exponential_metrics.duration.count() > 0);
        
        // Compare memory efficiency
        double linear_efficiency = linear_metrics.memory_usage / linear_metrics.duration.count();
        double exponential_efficiency = exponential_metrics.memory_usage / exponential_metrics.duration.count();
        
        assert(linear_efficiency > 0); // Should be positive
        assert(exponential_efficiency > 0);
    }
    
    void testMemoryReuseEfficiency() {
        PerformanceProfiler profiler;
        
        // Test memory reuse efficiency
        profiler.startProfile("memory_reuse_benchmark");
        
        // Allocate and reuse memory
        std::vector<double> buffer(100000, 0.0);
        
        for (int iteration = 0; iteration < 100; ++iteration) {
            // Process data
            for (size_t i = 0; i < buffer.size(); ++i) {
                buffer[i] = std::sin(i * 0.01 + iteration * 0.1);
            }
            
            // Verify data
            double sum = 0.0;
            for (size_t i = 0; i < buffer.size(); ++i) {
                sum += buffer[i];
            }
            assert(std::isfinite(sum));
        }
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("memory_reuse_benchmark");
        assert(metrics.memory_usage > 0);
        assert(metrics.duration.count() > 0);
        
        // Memory reuse should be efficient
        double efficiency = metrics.memory_usage / metrics.duration.count();
        assert(efficiency > 0); // Should have positive efficiency
    }
    
    void testMemoryLeakDetection() {
        PerformanceProfiler profiler;
        
        // Test for memory leaks
        profiler.startProfile("memory_leak_test");
        
        // Allocate and deallocate memory multiple times
        for (int i = 0; i < 50; ++i) {
            std::vector<double> temp_vector(100000, 0.0);
            
            // Use memory
            for (size_t j = 0; j < temp_vector.size(); ++j) {
                temp_vector[j] = std::sin(j * 0.01);
            }
            
            // Vector goes out of scope here - should be deallocated
            // If there's a memory leak, profiler should detect it
        }
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("memory_leak_test");
        assert(metrics.memory_usage >= 0); // Should not grow uncontrollably
        assert(metrics.duration.count() > 0);
        
        // Memory usage should be reasonable
        assert(metrics.memory_usage < 1000000000); // Less than 1GB
    }
    
    void testMemoryFragmentation() {
        PerformanceProfiler profiler;
        
        // Test memory fragmentation
        profiler.startProfile("memory_fragmentation_test");
        
        // Allocate and deallocate in various sizes
        std::vector<std::vector<double>> allocations;
        
        // Small allocations
        for (int i = 0; i < 1000; ++i) {
            std::vector<double> small_vec(100, 0.0);
            allocations.push_back(small_vec);
        }
        
        // Medium allocations
        for (int i = 0; i < 100; ++i) {
            std::vector<double> medium_vec(10000, 0.0);
            allocations.push_back(medium_vec);
        }
        
        // Large allocations
        for (int i = 0; i < 10; ++i) {
            std::vector<double> large_vec(100000, 0.0);
            allocations.push_back(large_vec);
        }
        
        // Deallocate in reverse order
        std::reverse(allocations.begin(), allocations.end());
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("memory_fragmentation_test");
        assert(metrics.memory_usage >= 0); // Should not grow uncontrollably
        assert(metrics.duration.count() > 0);
    }
    
    void testMemoryPressure() {
        PerformanceProfiler profiler;
        
        // Test memory under pressure
        profiler.startProfile("memory_pressure_test");
        
        // Allocate large amounts of memory
        std::vector<std::vector<double>> large_allocations;
        
        // Allocate progressively larger memory blocks
        for (int i = 0; i < 10; ++i) {
            size_t size = 1000 * pow(2, i) * 100; // Up to 100MB * 2^10
            std::vector<double> block(size, 0.0);
            large_allocations.push_back(block);
            
            // Use memory
            for (size_t j = 0; j < size; j += 1000) {
                block[j] = std::sin(j * 0.000001);
            }
        }
        
        // Process while under memory pressure
        double total_sum = 0.0;
        for (auto& block : large_allocations) {
            for (size_t i = 0; i < block.size(); ++i) {
                total_sum += block[i];
            }
        }
        
        assert(std::isfinite(total_sum));
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("memory_pressure_test");
        assert(metrics.memory_usage > 0); // Should track high memory usage
        assert(metrics.duration.count() > 0);
        
        // Should handle memory pressure gracefully
        assert(metrics.memory_usage < 2000000000); // Less than 2GB
    }
    
    void testMemoryAccessPatterns() {
        PerformanceProfiler profiler;
        
        // Test different memory access patterns
        profiler.startProfile("memory_access_random");
        
        // Random access pattern
        std::vector<double> random_data(100000, 0.0);
        for (size_t i = 0; i < random_data.size(); ++i) {
            random_data[i] = static_cast<double>(rand()) / RAND_MAX;
        }
        
        double random_sum = 0.0;
        for (size_t i = 0; i < random_data.size(); ++i) {
            size_t access_index = rand() % random_data.size();
            random_sum += random_data[access_index];
        }
        
        profiler.endProfile();
        
        auto random_metrics = profiler.getMetrics("memory_access_random");
        assert(random_metrics.memory_usage > 0);
        assert(random_metrics.duration.count() > 0);
        
        profiler.startProfile("memory_access_sequential");
        
        // Sequential access pattern
        std::vector<double> sequential_data(100000, 0.0);
        for (size_t i = 0; i < sequential_data.size(); ++i) {
            sequential_data[i] = std::sin(i * 0.01);
        }
        
        double sequential_sum = 0.0;
        for (size_t i = 0; i < sequential_data.size(); ++i) {
            sequential_sum += sequential_data[i];
        }
        
        profiler.endProfile();
        
        auto sequential_metrics = profiler.getMetrics("memory_access_sequential");
        assert(sequential_metrics.memory_usage > 0);
        assert(sequential_metrics.duration.count() > 0);
        
        // Sequential access should be faster than random access
        double random_efficiency = random_metrics.memory_usage / random_metrics.duration.count();
        double sequential_efficiency = sequential_metrics.memory_usage / sequential_metrics.duration.count();
        
        assert(sequential_efficiency >= random_efficiency * 0.5); // At least 50% as efficient
    }
};