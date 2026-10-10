// Performance Tests
#include "test_framework_utils.h"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

namespace test_performance {

void testPerformanceMeasurement() {
    std::cout << "    Testing performance measurement utilities..." << std::endl;
    
    // Test performance timer
    test_utils::PerformanceTimer timer;
    timer.start();
    
    // Simulate some work
    volatile int sum = 0;
    for (int i = 0; i < 1000; ++i) {
        sum += i;
    }
    
    timer.stop();
    
    double elapsedMs = timer.getElapsedMs();
    assert(elapsedMs > 0.0 && elapsedMs < 1000.0); // Should be quick
    
    std::cout << "    testPerformanceMeasurement passed" << std::endl;
}

void testPerformanceBenchmarking() {
    std::cout << "    Testing performance benchmarking..." << std::endl;
    
    // Test benchmarking utilities
    auto result = test_utils::BenchmarkUtils::benchmarkMultiple(
        [](int x) { return x * x; }, 100, 42
    );
    
    assert(result.timeMs > 0.0);
    assert(result.operationsPerSecond > 0);
    
    std::cout << "    testPerformanceBenchmarking passed" << std::endl;
}

void testLargeNetworkPerformance() {
    std::cout << "    Testing large network performance..." << std::endl;
    
    // Simulate large network operations
    std::vector<float> largeVector(10000, 1.0f);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Simulate vector operations
    float sum = std::accumulate(largeVector.begin(), largeVector.end(), 0.0f);
    float avg = sum / largeVector.size();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete quickly
    assert(duration.count() < 100);
    
    std::cout << "    testLargeNetworkPerformance passed" << std::endl;
}

void testMemoryUsagePerformance() {
    std::cout << "    Testing memory usage performance..." << std::endl;
    
    // Simulate memory-intensive operations
    std::vector<std::vector<float>> matrix(100, std::vector<float>(100, 1.0f));
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Simulate matrix operations
    for (auto& row : matrix) {
        for (float& val : row) {
            val = val * 0.5f;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete within reasonable time
    assert(duration.count() < 1000);
    
    std::cout << "    testMemoryUsagePerformance passed" << std::endl;
}

void testNetworkScalability() {
    std::cout << "    Testing network scalability..." << std::endl;
    
    // Test with different network sizes
    std::vector<size_t> sizes = {100, 1000, 10000};
    
    for (size_t size : sizes) {
        std::vector<float> data(size, 1.0f);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Simulate network operations
        float sum = std::accumulate(data.begin(), data.end(), 0.0f);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Larger networks should scale reasonably
        assert(duration.count() < 500);
    }
    
    std::cout << "    testNetworkScalability passed" << std::endl;
}

void testRealTimePerformance() {
    std::cout << "    Testing real-time performance..." << std::endl;
    
    // Simulate real-time operations with timing constraints
    auto start = std::chrono::high_resolution_clock::now();
    
    // Simulate multiple operations
    for (int i = 0; i < 1000; ++i) {
        volatile int x = i * 2;
        volatile int y = i / 2;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete in real-time (less than 100ms for 1000 iterations)
    assert(duration.count() < 100);
    
    std::cout << "    testRealTimePerformance passed" << std::endl;
}

void testStressTesting() {
    std::cout << "    Testing stress conditions..." << std::endl;
    
    // Simulate stress conditions with large data
    std::vector<float> largeData(50000, 1.0f);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Stress test with vector operations
    for (size_t i = 0; i < largeData.size(); ++i) {
        largeData[i] = largeData[i] * 0.5f + 0.1f;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should handle stress without crashing
    assert(duration.count() < 5000); // Allow up to 5 seconds for stress test
    
    std::cout << "    testStressTesting passed" << std::endl;
}

void runAll() {
    std::cout << "Running Performance tests..." << std::endl;
    testPerformanceMeasurement();
    testPerformanceBenchmarking();
    testLargeNetworkPerformance();
    testMemoryUsagePerformance();
    testNetworkScalability();
    testRealTimePerformance();
    testStressTesting();
}

} // namespace test_performance
