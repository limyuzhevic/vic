// NLM Performance Tests
// Benchmark Tests

#include "framework/TestFramework.hpp"
#include "framework/TestFixtures.hpp"
#include "brain/PerformanceProfiler.hpp"
#include <cassert>
#include <cmath>
#include <chrono>
#include <algorithm>

class TestBenchmarks : public TestCase {
public:
    TestBenchmarks() : TestCase("Benchmarks") {}
    
    void testMemoryBenchmark() {
        PerformanceProfiler profiler;
        
        // Memory benchmark - test memory allocation and access patterns
        profiler.startProfile("memory_allocation_benchmark");
        
        std::vector<std::vector<double>> matrix(1000, std::vector<double>(1000, 0.0));
        
        // Fill matrix with data
        for (int i = 0; i < 1000; ++i) {
            for (int j = 0; j < 1000; ++j) {
                matrix[i][j] = std::sin(i * 0.01) * std::cos(j * 0.01);
            }
        }
        
        // Access matrix elements
        double sum = 0.0;
        for (int i = 0; i < 1000; ++i) {
            for (int j = 0; j < 1000; ++j) {
                sum += matrix[i][j];
            }
        }
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("memory_allocation_benchmark");
        assert(metrics.memory_usage > 0); // Should track memory usage
        assert(metrics.duration.count() > 0);
        assert(std::isfinite(sum)); // Ensure computation completed
    
    void testCPUBenchmark() {
        PerformanceProfiler profiler;
        
        // CPU benchmark - test computational throughput
        profiler.startProfile("cpu_benchmark_matrix_mult");
        
        // Matrix multiplication benchmark
        int matrix_size = 500;
        std::vector<std::vector<double>> A(matrix_size, std::vector<double>(matrix_size));
        std::vector<std::vector<double>> B(matrix_size, std::vector<double>(matrix_size));
        std::vector<std::vector<double>> C(matrix_size, std::vector<double>(matrix_size, 0.0));
        
        // Initialize matrices
        for (int i = 0; i < matrix_size; ++i) {
            for (int j = 0; j < matrix_size; ++j) {
                A[i][j] = (rand() % 100) / 100.0;
                B[j][i] = (rand() % 100) / 100.0;
            }
        }
        
        // Matrix multiplication
        for (int i = 0; i < matrix_size; ++i) {
            for (int j = 0; j < matrix_size; ++j) {
                double sum = 0.0;
                for (int k = 0; k < matrix_size; ++k) {
                    sum += A[i][k] * B[k][j];
                }
                C[i][j] = sum;
            }
        }
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("cpu_benchmark_matrix_mult");
        assert(metrics.gflops > 0); // Should compute GFLOPs
        assert(metrics.cpu_cycles > 0);
        assert(metrics.duration.count() > 0);
        
        // Expected GFLOPs for matrix multiplication: 2 * n^3 operations
        double expected_gflops = 2.0 * matrix_size * matrix_size * matrix_size / (metrics.duration.count() * 1e9);
        assert(expected_gflops > 0); // Should be positive
    }
    
    void testNetworkBenchmark() {
        PerformanceProfiler profiler;
        
        // Network benchmark - test communication performance
        profiler.startProfile("network_communication_benchmark");
        
        // Simulate network communication
        std::vector<double> data(100000, 0.0);
        for (int i = 0; i < 100000; ++i) {
            data[i] = std::sin(i * 0.001);
        }
        
        // Process data (simulating network processing)
        std::vector<double> processed(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            processed[i] = data[i] * 2.0 + 0.5;
        }
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("network_communication_benchmark");
        assert(metrics.duration.count() > 0);
        assert(std::isfinite(metrics.gflops)); // Compute GFLOPs
    }
    
    void testIntegrationBenchmark() {
        PerformanceProfiler profiler;
        
        // Integration benchmark - test end-to-end system performance
        profiler.startProfile("integration_benchmark");
        
        // Simulate complete system integration
        std::vector<double> input_sensory = {0.7, 0.4, 0.8, 0.2, 0.6, 0.3, 0.9, 0.1, 0.5, 0.7};
        std::vector<double> input_motor = {0.3, 0.5, 0.2, 0.8, 0.4, 0.6, 0.1, 0.7, 0.3, 0.5};
        
        // Process through integration pipeline
        std::vector<double> processed_sensory = processSensoryData(input_sensory);
        std::vector<double> cognitive_state = processCognitiveData(processed_sensory);
        std::vector<double> motor_output = processMotorData(cognitive_state);
        
        // Verify integration
        assert(processed_sensory.size() == input_sensory.size());
        assert(motor_output.size() == input_motor.size());
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("integration_benchmark");
        assert(metrics.duration.count() > 0);
        assert(std::isfinite(metrics.gflops));
    }
    
    void testMemoryUsage() {
        PerformanceProfiler profiler;
        
        // Memory usage benchmark - test memory efficiency
        profiler.startProfile("memory_usage_stress");
        
        // Allocate various memory patterns
        std::vector<std::vector<double>> large_matrix(2000, std::vector<double>(2000, 0.0));
        std::vector<double> vector_data(1000000, 0.0);
        std::vector<int> int_array(500000);
        
        // Use memory
        for (int i = 0; i < 2000; ++i) {
            for (int j = 0; j < 2000; ++j) {
                large_matrix[i][j] = std::sin(i * 0.001) * std::cos(j * 0.001);
            }
        }
        
        for (int i = 0; i < 1000000; ++i) {
            vector_data[i] = std::sqrt(i);
        }
        
        for (int i = 0; i < 500000; ++i) {
            int_array[i] = i % 1000;
        }
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("memory_usage_stress");
        assert(metrics.memory_usage > 0); // Should track peak memory usage
        assert(metrics.duration.count() > 0);
    }
    
    void testCPUPerformance() {
        PerformanceProfiler profiler;
        
        // CPU performance benchmark - test computational efficiency
        profiler.startProfile("cpu_performance_math");
        
        // Complex mathematical operations
        double result = 0.0;
        for (int i = 0; i < 1000000; ++i) {
            result += std::sin(i * 0.01) * std::cos(i * 0.02) * std::tan(i * 0.03);
        }
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("cpu_performance_math");
        assert(metrics.cpu_cycles > 0);
        assert(metrics.gflops > 0); // Compute GFLOPs for math operations
        assert(metrics.duration.count() > 0);
        assert(std::isfinite(result)); // Ensure computation completed
    }
    
    void testNetworkPerformance() {
        PerformanceProfiler profiler;
        
        // Network performance benchmark - test communication speed
        profiler.startProfile("network_performance_bandwidth");
        
        // Simulate network data transfer
        const int data_size = 10000000; // 10MB
        std::vector<double> data(data_size, 0.0);
        
        // Generate data
        for (int i = 0; i < data_size; ++i) {
            data[i] = std::sin(i * 0.0001);
        }
        
        // Process data (simulating network transfer)
        std::vector<double> processed(data_size);
        for (size_t i = 0; i < data.size(); ++i) {
            processed[i] = data[i] * 1.1; // Simulate processing
        }
        
        profiler.endProfile();
        
        auto metrics = profiler.getMetrics("network_performance_bandwidth");
        assert(metrics.duration.count() > 0);
        
        // Calculate throughput (GB/s)
        double throughput_gb_s = (data_size * sizeof(double)) / (metrics.duration.count() * 1e6) / (1024 * 1024); // Convert to GB/s
        assert(throughput_gb_s > 0); // Should have positive throughput
    }
};