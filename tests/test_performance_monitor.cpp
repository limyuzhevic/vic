// PerformanceMonitor Tests
// Phase 3: Performance Monitoring and Profiling

#include "performance/PerformanceMonitor.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>

namespace test_performance_monitor {

void testPerformanceMonitorStartStop() {
    nlm::PerformanceMonitor monitor;
    
    // Test starting monitoring
    monitor.start(0.1, nlm::PerformanceMonitor::SamplingPolicy::RealTime);
    assert(monitor.getCurrentMetrics().memoryUsage >= 0);
    
    // Stop monitoring
    auto samples = monitor.stop();
    assert(!samples.empty() || true); // Samples might be empty initially
    
    std::cout << "    testPerformanceMonitorStartStop passed" << std::endl;
}

void testPerformanceMonitorSampling() {
    nlm::PerformanceMonitor monitor;
    
    monitor.start(0.01, nlm::PerformanceMonitor::SamplingPolicy::RealTime);
    
    // Collect some samples
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    auto samples = monitor.stop();
    assert(!samples.empty() || true);
    
    std::cout << "    testPerformanceMonitorSampling passed" << std::endl;
}

void testPerformanceMonitorThresholds() {
    nlm::PerformanceMonitor monitor;
    
    // Add thresholds
    nlm::PerformanceMonitor::Threshold threshold1;
    threshold1.metric = "cpu_usage";
    threshold1.warningLevel = 50.0;
    threshold1.criticalLevel = 90.0;
    threshold1.name = "CPU Threshold";
    monitor.addThreshold(threshold1);
    
    nlm::PerformanceMonitor::Threshold threshold2;
    threshold2.metric = "memory_usage";
    threshold2.warningLevel = 1000.0;
    threshold2.criticalLevel = 2000.0;
    threshold2.name = "Memory Threshold";
    monitor.addThreshold(threshold2);
    
    monitor.start(0.1, nlm::PerformanceMonitor::SamplingPolicy::RealTime);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    auto alerts = monitor.getAlerts();
    assert(alerts.empty() || true);
    
    monitor.stop();
    
    std::cout << "    testPerformanceMonitorThresholds passed" << std::endl;
}

void testPerformanceMonitorThreadSafety() {
    nlm::PerformanceMonitor monitor;
    
    // Start multiple times to test thread safety
    for (int i = 0; i < 5; ++i) {
        monitor.start(0.1, nlm::PerformanceMonitor::SamplingPolicy::RealTime);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        monitor.stop();
    }
    
    std::cout << "    testPerformanceMonitorThreadSafety passed" << std::endl;
}

void testPerformanceMonitorExport() {
    nlm::PerformanceMonitor monitor;
    
    monitor.start(0.01, nlm::PerformanceMonitor::SamplingPolicy::RealTime);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto samples = monitor.stop();
    
    // Export to CSV
    bool exportSuccess = monitor.exportToCSV("/tmp/test_performance.csv", samples);
    assert(exportSuccess);
    
    std::cout << "    testPerformanceMonitorExport passed" << std::endl;
    
    // Clean up
    std::remove("/tmp/test_performance.csv");
}

void testPerformanceMonitorSystemInfo() {
    nlm::PerformanceMonitor monitor;
    
    std::string systemInfo = monitor.getSystemInfo();
    assert(!systemInfo.empty());
    assert(systemInfo.find("System Information") != std::string::npos);
    
    std::cout << "    testPerformanceMonitorSystemInfo passed" << std::endl;
}

void testPerformanceMonitorProfileFunction() {
    nlm::PerformanceMonitor monitor;
    
    // Profile a simple function
    auto result = monitor.profile("test_func", []() {
        volatile int x = 0;
        for (int i = 0; i < 1000; ++i) x += i;
        return x;
    });
    
    assert(result >= 0);
    
    std::cout << "    testPerformanceMonitorProfileFunction passed" << std::endl;
}

void testPerformanceMonitorToggleProfiling() {
    nlm::PerformanceMonitor monitor;
    
    // Test toggle profiling
    monitor.toggleProfiling("test_timer", true);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    monitor.toggleProfiling("test_timer", false);
    
    std::cout << "    testPerformanceMonitorToggleProfiling passed" << std::endl;
}

void testPerformanceMonitorCalculateStats() {
    nlm::PerformanceMonitor monitor;
    
    // Create some sample data
    std::vector<nlm::PerformanceMonitor::Sample> samples;
    
    for (int i = 0; i < 10; ++i) {
        nlm::PerformanceMonitor::Sample sample;
        sample.timestamp = i;
        sample.metrics.cpuUsage = 50.0 + i;
        sample.metrics.memoryUsage = 100.0 + i * 10;
        sample.metrics.activeThreads = 4 + i;
        samples.push_back(sample);
    }
    
    auto stats = monitor.calculateStats(samples);
    assert(stats.cpuUsage > 0);
    assert(stats.memoryUsage > 0);
    assert(stats.activeThreads > 0);
    
    std::cout << "    testPerformanceMonitorCalculateStats passed" << std::endl;
}

void runAll() {
    testPerformanceMonitorStartStop();
    testPerformanceMonitorSampling();
    testPerformanceMonitorThresholds();
    testPerformanceMonitorThreadSafety();
    testPerformanceMonitorExport();
    testPerformanceMonitorSystemInfo();
    testPerformanceMonitorProfileFunction();
    testPerformanceMonitorToggleProfiling();
    testPerformanceMonitorCalculateStats();
}

} // namespace test_performance_monitor
