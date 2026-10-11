// NLM Integration Tests
// Performance Integration Tests

#include "framework/TestFramework.hpp"
#include "framework/TestFixtures.hpp"
#include "brain/PerformanceProfiler.hpp"
#include "brain/OptimizationEngine.hpp"
#include <cassert>
#include <cmath>
#include <chrono>

class TestPerformanceIntegration : public TestCase {
public:
    TestPerformanceIntegration() : TestCase("PerformanceIntegration") {}
    
    void testProfilerIntegration() {
        PerformanceProfiler profiler;
        
        // Start performance profiling
        profiler.startProfile("system_performance");
        
        // Perform computational work
        double result = 0.0;
        for (int i = 0; i < 1000000; ++i) {
            result += std::sin(i * 0.01) * std::cos(i * 0.02);
        }
        
        profiler.endProfile();
        
        // Get performance metrics
        auto metrics = profiler.getMetrics("system_performance");
        assert(metrics.duration.count() > 0);
        assert(metrics.cpu_cycles > 0);
        assert(metrics.memory_usage > 0);
        assert(std::isfinite(metrics.gflops));
        
        // Profile multiple tasks
        for (int i = 0; i < 5; ++i) {
            profiler.startProfile(std::string("task_") + std::to_string(i));
            
            // Simulate work
            double work = 0.0;
            for (int j = 0; j < 100000; ++j) {
                work += std::sqrt(j);
            }
            
            profiler.endProfile();
        }
        
        // Get aggregated statistics
        auto stats = profiler.getStatistics("task_0");
        assert(stats.count == 5);
        assert(stats.mean_time > 0);
        assert(stats.std_deviation >= 0);
        assert(stats.min_time > 0);
        assert(stats.max_time >= stats.min_time);
    }
    
    void testOptimizationEngineIntegration() {
        OptimizationEngine optimizer;
        
        // Test parameter optimization with multiple objectives
        std::vector<double> initial_params = {0.5, 0.3, 0.8, 0.2, 0.6};
        std::vector<double> gradient = {0.1, -0.2, 0.3, -0.4, 0.5};
        
        // Run optimization
        std::vector<double> optimized_params = optimizer.optimize(
            initial_params, gradient, 0.01);
        
        assert(optimized_params.size() == initial_params.size());
        
        // Test multiple optimization runs
        std::vector<std::vector<double>> param_sets;
        std::vector<std::vector<double>> gradients;
        
        for (int i = 0; i < 10; ++i) {
            param_sets.push_back(std::vector<double>(5, rand() / (RAND_MAX + 1.0)));
            gradients.push_back(std::vector<double>(5, (rand() / (RAND_MAX + 1.0) * 2.0 - 1.0)));
        }
        
        // Batch optimize
        std::vector<std::vector<double>> optimized_sets = optimizer.batchOptimize(param_sets, gradients, 0.01);
        
        assert(optimized_sets.size() == param_sets.size());
        for (size_t i = 0; i < optimized_sets.size(); ++i) {
            assert(optimized_sets[i].size() == param_sets[i].size());
        }
        
        // Test constraint satisfaction
        optimizer.addConstraint("param_range", [](const std::vector<double>& params) {
            for (double param : params) {
                if (param < 0.0 || param > 1.0) return false;
            }
            return true;
        });
        
        std::vector<double> constrained_params = {0.1, 0.5, 0.9, 0.2, 0.7};
        bool is_feasible = optimizer.checkConstraints(constrained_params);
        assert(is_feasible);
        
        std::vector<double> infeasible_params = {1.2, 0.5, 0.9, 0.2, 0.7};
        is_feasible = optimizer.checkConstraints(infeasible_params);
        assert(!is_feasible);
    }
    
    void testMemoryUsageTracking() {
        PerformanceProfiler profiler;
        
        // Profile memory-intensive operations
        profiler.startProfile("memory_allocation");
        
        // Allocate and use memory
        std::vector<std::vector<double>> matrix(1000, std::vector<double>(1000, 0.0));
        for (int i = 0; i < 1000; ++i) {
            for (int j = 0; j < 1000; ++j) {
                matrix[i][j] = std::sin(i * 0.01) * std::cos(j * 0.01);
            }
        }
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("memory_allocation");
        assert(metrics.memory_usage > 0); // Should track allocated memory
        assert(metrics.duration.count() > 0);
        
        // Profile multiple memory operations
        for (int i = 0; i < 5; ++i) {
            profiler.startProfile(std::string("memory_op_") + std::to_string(i));
            
            std::vector<double> data(100000, 0.0);
            for (int j = 0; j < 100000; ++j) {
                data[j] = std::sqrt(j);
            }
            
            profiler.endProfile();
        }
        
        // Compare memory usage
        auto stats = profiler.getStatistics("memory_op_0");
        assert(stats.mean_time > 0);
        assert(stats.min_time > 0);
        assert(stats.max_time >= stats.min_time);
    }
    
    void testCPUBenchmarkIntegration() {
        PerformanceProfiler profiler;
        
        // Run CPU benchmarks
        profiler.startProfile("monte_carlo_pi");
        
        double sum = 0.0;
        int samples = 1000000;
        for (int i = 0; i < samples; ++i) {
            double x = (rand() % 1000) / 1000.0;
            double y = (rand() % 1000) / 1000.0;
            if (x*x + y*y <= 1.0) {
                sum += 1.0;
            }
        }
        double pi_estimate = 4.0 * sum / samples;
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("monte_carlo_pi");
        assert(std::abs(pi_estimate - 3.14159) < 0.01); // Reasonable accuracy
        assert(metrics.cpu_cycles > 0);
        assert(metrics.gflops > 0);
        
        // Profile other CPU-intensive operations
        profiler.startProfile("matrix_multiplication");
        
        std::vector<std::vector<double>> A(500, std::vector<double>(500, 0.0));
        std::vector<std::vector<double>> B(500, std::vector<double>(500, 0.0));
        std::vector<std::vector<double>> C(500, std::vector<double>(500, 0.0));
        
        for (int i = 0; i < 500; ++i) {
            for (int j = 0; j < 500; ++j) {
                A[i][j] = (rand() % 100) / 100.0;
                B[j][i] = (rand() % 100) / 100.0;
            }
        }
        
        for (int i = 0; i < 500; ++i) {
            for (int j = 0; j < 500; ++j) {
                double sum = 0.0;
                for (int k = 0; k < 500; ++k) {
                    sum += A[i][k] * B[k][j];
                }
                C[i][j] = sum;
            }
        }
        
        profiler.endProfile();
        
        auto matrix_metrics = profiler.getMetrics("matrix_multiplication");
        assert(matrix_metrics.gflops > 0);
        assert(matrix_metrics.cpu_cycles > 0);
        assert(matrix_metrics.duration.count() > 0);
    }
};