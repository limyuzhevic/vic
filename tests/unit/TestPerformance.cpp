// NLM Unit Testing Framework
// Performance Tests

#include "framework/TestFramework.hpp"
#include "brain/PerformanceProfiler.hpp"
#include "brain/OptimizationEngine.hpp"
#include <cassert>
#include <chrono>
#include <cmath>

class TestPerformance : public TestCase {
public:
    TestPerformance() : TestCase("Performance") {}
    
    void testProfilerAccuracy() {
        PerformanceProfiler profiler;
        
        // Start profiling a computationally intensive task
        profiler.startProfile("computational_task");
        
        // Perform computation
        double result = 0.0;
        for (int i = 0; i < 100000; ++i) {
            result += std::sin(i * 0.1) * std::cos(i * 0.2);
        }
        
        profiler.endProfile();
        
        // Get performance metrics
        auto metrics = profiler.getMetrics("computational_task");
        assert(metrics.duration.count() > 0);
        assert(metrics.cpu_cycles > 0);
        assert(metrics.memory_usage > 0);
        assert(std::isfinite(metrics.gflops));
    }
    
    void testOptimizationEngine() {
        OptimizationEngine optimizer;
        
        // Test parameter optimization
        std::vector<double> initial_params = {0.5, 0.5, 0.5, 0.5, 0.5};
        std::vector<double> gradient = {0.1, -0.2, 0.3, -0.4, 0.5};
        
        double learning_rate = 0.01;
        std::vector<double> optimized_params = optimizer.optimize(
            initial_params, gradient, learning_rate);
        
        assert(optimized_params.size() == initial_params.size());
        assert(std::abs(optimized_params[0] - (initial_params[0] + learning_rate * gradient[0])) < 0.001);
    }
    
    void testMemoryUsageOptimization() {
        PerformanceProfiler profiler;
        
        // Profile memory usage
        profiler.startProfile("memory_intensive_task");
        
        std::vector<std::vector<double>> large_matrix(1000, std::vector<double>(1000, 0.0));
        
        // Fill matrix with data
        for (int i = 0; i < 1000; ++i) {
            for (int j = 0; j < 1000; ++j) {
                large_matrix[i][j] = std::sin(i * 0.01) * std::cos(j * 0.01);
            }
        }
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("memory_intensive_task");
        assert(metrics.memory_usage > 0); // Should track memory usage
        assert(metrics.duration.count() > 0);
    }
    
    void testCPUBenchmark() {
        PerformanceProfiler profiler;
        
        // Benchmark CPU performance
        profiler.startProfile("cpu_benchmark");
        
        // Compute Pi using Monte Carlo method
        double sum = 0.0;
        for (int i = 0; i < 1000000; ++i) {
            double x = (rand() % 1000) / 1000.0;
            double y = (rand() % 1000) / 1000.0;
            if (x*x + y*y <= 1.0) {
                sum += 1.0;
            }
        }
        double pi_estimate = 4.0 * sum / 1000000.0;
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("cpu_benchmark");
        assert(std::abs(pi_estimate - 3.14159) < 0.01); // Reasonable accuracy
        assert(metrics.cpu_cycles > 0);
        assert(metrics.gflops > 0); // Should compute GFLOPs
    }
    
    void testProfileAggregation() {
        PerformanceProfiler profiler;
        
        // Run multiple profiled tasks
        for (int i = 0; i < 5; ++i) {
            profiler.startProfile("test_task");
            
            // Simple computation
            double result = 0.0;
            for (int j = 0; j < 1000; ++j) {
                result += std::sqrt(j);
            }
            
            profiler.endProfile();
        }
        
        // Get aggregated statistics
        auto stats = profiler.getStatistics("test_task");
        assert(stats.mean_time > 0);
        assert(stats.std_deviation >= 0);
        assert(stats.min_time > 0);
        assert(stats.max_time >= stats.min_time);
        assert(stats.count == 5);
    }
};