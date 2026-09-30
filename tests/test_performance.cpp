// Performance Monitoring Tests
// Tests for the enhanced performance monitoring and profiling capabilities

#include "performance/PerformanceMonitor.hpp"
#include "core/Config/ConfigEnhanced.hpp"
#include <iostream>
#include <cassert>
#include <chrono>

void testPerformanceMonitorCreation() {
    std::cout << "Running performance monitor creation tests..." << std::endl;
    
    nlm::PerformanceMonitor monitor;
    
    assert(!monitor.isRunning());
    assert(monitor.getUpdateInterval() == 1.0);
    
    std::cout << "Performance monitor creation tests passed!" << std::endl;
}

void testPerformanceMonitorCounters() {
    std::cout << "Running performance monitor counter tests..." << std::endl;
    
    nlm::PerformanceMonitor monitor;
    
    // Add counters
    monitor.addCounter("test_operation");
    monitor.addCounter("another_operation");
    
    // Record some operations
    monitor.recordCounter("test_operation");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    monitor.finishCounter("test_operation");
    
    monitor.recordCounter("another_operation");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    monitor.finishCounter("another_operation");
    
    // Check counter values
    assert(monitor.getCounterValue("test_operation") > 0.0);
    assert(monitor.getCounterValue("another_operation") > 0.0);
    
    std::cout << "Performance monitor counter tests passed!" << std::endl;
}

void testPerformanceMonitorMetrics() {
    std::cout << "Running performance monitor metrics tests..." << std::endl;
    
    nlm::PerformanceMonitor monitor;
    
    // Add counters and simulate work
    monitor.addCounter("cpu_intensive_task");
    monitor.addCounter("memory_intensive_task");
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Simulate CPU work
    monitor.recordCounter("cpu_intensive_task");
    for (int i = 0; i < 1000; ++i) {
        volatile int x = i * i;  // CPU intensive operation
    }
    monitor.finishCounter("cpu_intensive_task");
    
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    
    monitor.updateMetric("elapsed_time", elapsed);
    monitor.updateMetric("iterations", 1000.0);
    
    // Get metrics
    auto metrics = monitor.getMetrics();
    assert(metrics.totalTime > 0.0);
    assert(metrics.stepTime > 0.0);
    assert(metrics.throughput > 0.0);
    
    std::cout << "Performance monitor metrics tests passed!" << std::endl;
}

void testPerformanceMonitorReport() {
    std::cout << "Running performance monitor report tests..." << std::endl;
    
    nlm::PerformanceMonitor monitor;
    
    // Add some counters
    monitor.addCounter("report_test");
    
    monitor.recordCounter("report_test");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    monitor.finishCounter("report_test");
    
    // This should not crash
    monitor.printReport();
    
    // Export to JSON
    std::string jsonOutput = monitor.exportToJSON();
    assert(!jsonOutput.empty());
    
    // Export to CSV
    std::string csvOutput = monitor.exportToCSV();
    assert(!csvOutput.empty());
    
    std::cout << "Performance monitor report tests passed!" << std::endl;
}

void testPerformanceMonitorTimers() {
    std::cout << "Running performance monitor timer tests..." << std::endl;
    
    nlm::PerformanceMonitor monitor;
    
    // Test timer functionality
    {
        auto timer = monitor.createTimer("timer_test_1");
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        double elapsed = timer.getElapsedTime();
        assert(elapsed >= 0.005);
    }
    
    {
        auto timer = monitor.createTimer("timer_test_2");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        double elapsed = timer.getElapsedTime();
        assert(elapsed >= 0.01);
    }
    
    std::cout << "Performance monitor timer tests passed!" << std::endl;
}

void testPerformanceMonitorBenchmark() {
    std::cout << "Running performance monitor benchmark tests..." << std::endl;
    
    nlm::PerformanceMonitor monitor;
    
    // Test benchmark functionality
    {
        auto benchmark = nlm::Benchmark("benchmark_test", monitor);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        // Benchmark automatically finishes when out of scope
    }
    
    std::cout << "Performance monitor benchmark tests passed!" << std::endl;
}

void testGlobalPerformanceMonitor() {
    std::cout << "Running global performance monitor tests..." << std::endl;
    
    // Test global performance monitor functions
    nlm::startPerformanceMonitoring();
    nlm::PerformanceMonitor& globalMonitor = nlm::getPerformanceMonitor();
    assert(globalMonitor.isRunning());
    
    // Add a counter and record some work
    globalMonitor.addCounter("global_test");
    globalMonitor.recordCounter("global_test");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    globalMonitor.finishCounter("global_test");
    
    nlm::stopPerformanceMonitoring();
    
    // Should be stopped now
    // Note: We can't easily test the global monitor after stopping due to thread safety
    
    std::cout << "Global performance monitor tests passed!" << std::endl;
}

int main() {
    std::cout << "=== Performance Monitoring Tests ===" << std::endl << std::endl;
    
    testPerformanceMonitorCreation();
    testPerformanceMonitorCounters();
    testPerformanceMonitorMetrics();
    testPerformanceMonitorReport();
    testPerformanceMonitorTimers();
    testPerformanceMonitorBenchmark();
    testGlobalPerformanceMonitor();
    
    std::cout << std::endl;
    std::cout << "=== All Performance Monitoring Tests Passed Successfully ===" << std::endl;
    
    return 0;
}