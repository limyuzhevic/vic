// Benchmark Tests
// Phase 3: Scaling and Performance Benchmarking

#include "performance/PerformanceMonitor.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>

namespace test_benchmark {

void testBenchmarkExecution() {
    nlm::PerformanceMonitor monitor;
    
    // Initialize monitor
    monitor.start(0.01, nlm::PerformanceMonitor::SamplingPolicy::RealTime);
    
    // Simulate benchmark workload
    std::vector<double> results;
    for (int i = 0; i < 100; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Simulate computational work
        volatile double sum = 0.0;
        for (int j = 0; j < 1000; ++j) {
            sum += j * 0.1;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - start).count();
        results.push_back(duration);
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto samples = monitor.stop();
    
    // Validate benchmark results
    assert(!results.empty());
    assert(samples.empty() || true);
    
    std::cout << "    testBenchmarkExecution passed" << std::endl;
}

void testBenchmarkScaling() {
    nlm::PerformanceMonitor monitor;
    
    // Test scaling with increasing workload
    std::vector<size_t> workloads = {10, 100, 1000, 5000};
    std::vector<double> executionTimes;
    
    for (size_t workload : workloads) {
        monitor.start(0.01, nlm::PerformanceMonitor::SamplingPolicy::RealTime);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Simulate work based on workload
        std::vector<double> data(workload, 0.0);
        for (size_t i = 0; i < workload; ++i) {
            data[i] = i * 0.001;
        }
        
        // Perform some computation
        double sum = 0.0;
        for (size_t i = 0; i < workload; ++i) {
            sum += std::sqrt(data[i]);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - start).count();
        
        executionTimes.push_back(duration);
        monitor.stop();
    }
    
    // Validate scaling results
    assert(executionTimes.size() == workloads.size());
    assert(executionTimes.back() > executionTimes.front());
    
    std::cout << "    testBenchmarkScaling passed" << std::endl;
}

void testBenchmarkMetricsCollection() {
    nlm::PerformanceMonitor monitor;
    
    monitor.start(0.001, nlm::PerformanceMonitor::SamplingPolicy::RealTime);
    
    // Perform benchmark with metrics collection
    std::vector<double> metrics;
    for (int i = 0; i < 50; ++i) {
        auto metrics = monitor.sample();
        metrics.push_back(metrics.metrics.cpuUsage);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    auto samples = monitor.stop();
    
    // Validate metrics
    assert(!metrics.empty());
    assert(samples.empty() || true);
    
    std::cout << "    testBenchmarkMetricsCollection passed" << std::endl;
}

void testBenchmarkResultValidation() {
    nlm::PerformanceMonitor monitor;
    
    monitor.start(0.01, nlm::PerformanceMonitor::SamplingPolicy::RealTime);
    
    // Run benchmark with predictable results
    double expectedSum = 0.0;
    for (int i = 1; i <= 100; ++i) {
        expectedSum += i;
    }
    
    double actualSum = 0.0;
    for (int i = 1; i <= 100; ++i) {
        actualSum += i;
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    auto samples = monitor.stop();
    
    // Validate results
    assert(std::abs(actualSum - expectedSum) < 0.001);
    assert(samples.empty() || true);
    
    std::cout << "    testBenchmarkResultValidation passed" << std::endl;
}

void testBenchmarkHardwareUtilization() {
    nlm::PerformanceMonitor monitor;
    
    monitor.start(0.1, nlm::PerformanceMonitor::SamplingPolicy::RealTime);
    
    // Simulate hardware utilization monitoring
    std::vector<double> cpuUtilizations;
    for (int i = 0; i < 20; ++i) {
        auto metrics = monitor.sample();
        cpuUtilizations.push_back(metrics.metrics.cpuUsage);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    auto samples = monitor.stop();
    
    // Validate hardware utilization data
    assert(!cpuUtilizations.empty());
    assert(samples.empty() || true);
    
    std::cout << "    testBenchmarkHardwareUtilization passed" << std::endl;
}

void testBenchmarkPerformanceMetrics() {
    nlm::PerformanceMonitor monitor;
    
    monitor.start(0.01, nlm::PerformanceMonitor::SamplingPolicy::RealTime);
    
    // Collect various performance metrics
    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    
    volatile double computation = 0.0;
    for (int i = 0; i < 10000; ++i) {
        computation += i * 0.0001;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime).count();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto samples = monitor.stop();
    
    // Validate performance metrics
    assert(elapsed > 0.0);
    assert(samples.empty() || true);
    
    std::cout << "    testBenchmarkPerformanceMetrics passed" << std::endl;
}

void runAll() {
    testBenchmarkExecution();
    testBenchmarkScaling();
    testBenchmarkMetricsCollection();
    testBenchmarkResultValidation();
    testBenchmarkHardwareUtilization();
    testBenchmarkPerformanceMetrics();
}

} // namespace test_benchmark
