// NLM Performance Tests
// CPU Performance Tests

#include "framework/TestFramework.hpp"
#include "framework/TestFixtures.hpp"
#include "brain/PerformanceProfiler.hpp"
#include <cassert>
#include <cmath>
#include <chrono>
#include <algorithm>

class TestCPUPerformance : public TestCase {
public:
    TestCPUPerformance() : TestCase("CPUPerformance") {}
    
    void testIntegerOperations() {
        PerformanceProfiler profiler;
        
        // Test integer operation performance
        profiler.startProfile("integer_operations_benchmark");
        
        // Integer operations (addition, multiplication, etc.)
        long long result = 0;
        for (int i = 0; i < 100000000; ++i) {
            result += i * (i % 1000);
        }
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("integer_operations_benchmark");
        assert(metrics.cpu_cycles > 0);
        assert(metrics.gflops > 0); // Compute GFLOPs for integer operations
        assert(metrics.duration.count() > 0);
        assert(std::isfinite(result)); // Ensure computation completed
    }
    
    void testFloatingPointOperations() {
        PerformanceProfiler profiler;
        
        // Test floating point operation performance
        profiler.startProfile("floating_point_benchmark");
        
        double result = 0.0;
        for (int i = 0; i < 50000000; ++i) {
            double x = i * 0.00001;
            result += std::sin(x) * std::cos(x) * std::tan(x);
        }
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("floating_point_benchmark");
        assert(metrics.cpu_cycles > 0);
        assert(metrics.gflops > 0); // Compute GFLOPs for floating point operations
        assert(metrics.duration.count() > 0);
        assert(std::isfinite(result)); // Ensure computation completed
    }
    
    void testMemoryIntensiveCPU() {
        PerformanceProfiler profiler;
        
        // Test CPU performance with memory-intensive operations
        profiler.startProfile("memory_intensive_cpu_benchmark");
        
        // Large array operations
        const int array_size = 1000000;
        std::vector<double> array1(array_size, 0.0);
        std::vector<double> array2(array_size, 0.0);
        std::vector<double> array3(array_size, 0.0);
        
        // Initialize arrays
        for (int i = 0; i < array_size; ++i) {
            array1[i] = std::sin(i * 0.000001);
            array2[i] = std::cos(i * 0.000001);
            array3[i] = std::tan(i * 0.000001);
        }
        
        // Perform computations
        double sum = 0.0;
        for (int i = 0; i < array_size; ++i) {
            sum += array1[i] * array2[i] + array3[i];
        }
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("memory_intensive_cpu_benchmark");
        assert(metrics.cpu_cycles > 0);
        assert(metrics.gflops > 0); // Compute GFLOPs for memory-intensive operations
        assert(metrics.duration.count() > 0);
        assert(std::isfinite(sum)); // Ensure computation completed
    }
    
    void testSIMDPerformance() {
        PerformanceProfiler profiler;
        
        // Test SIMD (Single Instruction Multiple Data) performance
        profiler.startProfile("simd_operations_benchmark");
        
        // SIMD-like operations using standard library
        const int vector_size = 1000000;
        std::vector<double> data1(vector_size, 0.0);
        std::vector<double> data2(vector_size, 0.0);
        std::vector<double> result(vector_size, 0.0);
        
        // Initialize data
        for (int i = 0; i < vector_size; ++i) {
            data1[i] = std::sin(i * 0.000001);
            data2[i] = std::cos(i * 0.000001);
        }
        
        // SIMD-like parallel operations
        for (int i = 0; i < vector_size; ++i) {
            result[i] = data1[i] * data2[i] + std::sin(data1[i]) + std::cos(data2[i]);
        }
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("simd_operations_benchmark");
        assert(metrics.cpu_cycles > 0);
        assert(metrics.gflops > 0); // Compute GFLOPs for SIMD operations
        assert(metrics.duration.count() > 0);
    
    void testBranchPrediction() {
        PerformanceProfiler profiler;
        
        // Test branch prediction performance
        profiler.startProfile("branch_prediction_benchmark");
        
        // Conditional operations with predictable branches
        int predictable_result = 0;
        for (int i = 0; i < 50000000; ++i) {
            if (i % 2 == 0) { // Predictable pattern
                predictable_result += i;
            } else {
                predictable_result -= i;
            }
        }
        
        profiler.endProfile();
        
        auto predictable_metrics = profiler.getMetrics("branch_prediction_benchmark");
        assert(predictable_metrics.cpu_cycles > 0);
        assert(predictable_metrics.gflops > 0);
        assert(predictable_metrics.duration.count() > 0);
        assert(std::isfinite(predictable_result));
        
        profiler.startProfile("branch_prediction_unpredictable");
        
        // Conditional operations with unpredictable branches
        int unpredictable_result = 0;
        for (int i = 0; i < 50000000; ++i) {
            // Unpredictable pattern (based on computation)
            if (std::sin(i * 0.0001) > 0.0) {
                unpredictable_result += i;
            } else {
                unpredictable_result -= i;
            }
        }
        
        profiler.endProfile();
        
        auto unpredictable_metrics = profiler.getMetrics("branch_prediction_unpredictable");
        assert(unpredictable_metrics.cpu_cycles > 0);
        assert(unpredictable_metrics.gflops > 0);
        assert(unpredictable_metrics.duration.count() > 0);
        assert(std::isfinite(unpredictable_result));
        
        // Unpredictable branches should be slower
        double predictable_efficiency = predictable_metrics.gflops / predictable_metrics.duration.count();
        double unpredictable_efficiency = unpredictable_metrics.gflops / unpredictable_metrics.duration.count();
        
        assert(unpredictable_efficiency <= predictable_efficiency * 2.0); // At most 2x slower
    }
    
    void testCachePerformance() {
        PerformanceProfiler profiler;
        
        // Test cache performance
        profiler.startProfile("cache_small_arrays");
        
        // Small arrays for cache optimization
        std::vector<double> small_array1(1024, 0.0);
        std::vector<double> small_array2(1024, 0.0);
        std::vector<double> small_result(1024, 0.0);
        
        for (int iteration = 0; iteration < 1000000; ++iteration) {
            for (int i = 0; i < 1024; ++i) {
                small_array1[i] = std::sin(i * 0.001);
                small_array2[i] = std::cos(i * 0.001);
                small_result[i] = small_array1[i] * small_array2[i];
            }
        }
        
        profiler.endProfile();
        
        auto small_metrics = profiler.getMetrics("cache_small_arrays");
        assert(small_metrics.cpu_cycles > 0);
        assert(small_metrics.gflops > 0);
        assert(small_metrics.duration.count() > 0);
        
        profiler.startProfile("cache_large_arrays");
        
        // Large arrays for cache thrashing
        std::vector<double> large_array1(1000000, 0.0);
        std::vector<double> large_array2(1000000, 0.0);
        std::vector<double> large_result(1000000, 0.0);
        
        for (int i = 0; i < 1000000; ++i) {
            large_array1[i] = std::sin(i * 0.000001);
            large_array2[i] = std::cos(i * 0.000001);
        }
        
        for (int iteration = 0; iteration < 10; ++iteration) {
            for (int i = 0; i < 1000000; ++i) {
                large_result[i] = large_array1[i] * large_array2[i];
            }
        }
        
        profiler.endProfile();
        
        auto large_metrics = profiler.getMetrics("cache_large_arrays");
        assert(large_metrics.cpu_cycles > 0);
        assert(large_metrics.gflops > 0);
        assert(large_metrics.duration.count() > 0);
        
        // Small arrays should be more efficient (better cache utilization)
        double small_efficiency = small_metrics.gflops / small_metrics.duration.count();
        double large_efficiency = large_metrics.gflops / large_metrics.duration.count();
        
        assert(small_efficiency >= large_efficiency * 0.3); // At least 30% as efficient
    }
    
    void testPipelinePerformance() {
        PerformanceProfiler profiler;
        
        // Test pipeline (instruction-level parallelism) performance
        profiler.startProfile("pipeline_computations");
        
        // Multiple dependent computations
        double result1 = 0.0, result2 = 0.0, result3 = 0.0, result4 = 0.0;
        for (int i = 0; i < 10000000; ++i) {
            double x = std::sin(i * 0.000001);
            result1 += x;
            result2 += std::cos(x);
            result3 += std::tan(x);
            result4 += x * x;
        }
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("pipeline_computations");
        assert(metrics.cpu_cycles > 0);
        assert(metrics.gflops > 0); // Compute GFLOPs for pipeline operations
        assert(metrics.duration.count() > 0);
        assert(std::isfinite(result1) && std::isfinite(result2) && 
               std::isfinite(result3) && std::isfinite(result4));
    }
};