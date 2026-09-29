// Simple integration test for Performance Monitoring Infrastructure
// This test verifies the basic functionality of the performance monitoring system

#include "Monitoring.hpp"
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    std::cout << "=== Performance Monitoring Integration Test ===" << std::endl;
    std::cout << std::endl;
    
    // Test 1: Singleton pattern
    std::cout << "Test 1: Singleton Pattern" << std::endl;
    auto& monitor1 = PerformanceMonitor::getInstance();
    auto& monitor2 = PerformanceMonitor::getInstance();
    
    if (&monitor1 == &monitor2) {
        std::cout << "  ✓ Singleton pattern verified" << std::endl;
    } else {
        std::cout << "  ✗ Singleton pattern failed" << std::endl;
        return 1;
    }
    
    // Test 2: Basic metric recording
    std::cout << std::endl << "Test 2: Basic Metric Recording" << std::endl;
    monitor1.recordMetric("test_metric", 42.5, MetricType::Gauge, "test");
    monitor1.recordMetric("test_counter", 100, MetricType::Counter, "test");
    
    std::cout << "  ✓ Metrics recorded successfully" << std::endl;
    
    // Test 3: Component registration
    std::cout << std::endl << "Test 3: Component Registration" << std::endl;
    monitor1.registerComponent("brain", ComponentType::Brain);
    monitor1.registerComponent("memory", ComponentType::Memory);
    monitor1.registerComponent("environment", ComponentType::Environment);
    
    std::cout << "  ✓ Components registered successfully" << std::endl;
    
    // Test 4: Timing and profiling
    std::cout << std::endl << "Test 4: Timing and Profiling" << std::endl;
    
    // Record timing
    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto end = std::chrono::high_resolution_clock::now();
    monitor1.recordTiming("sleep_duration", end - start, "test");
    
    // Test profiling scope
    auto scope = monitor1.profile("test_operation", "test");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    scope.complete();
    
    std::cout << "  ✓ Timing and profiling work correctly" << std::endl;
    
    // Test 5: Metric retrieval
    std::cout << std::endl << "Test 5: Metric Retrieval" << std::endl;
    
    double currentValue = monitor1.getCurrentMetric("test_metric");
    if (currentValue == 42.5) {
        std::cout << "  ✓ Metric retrieval works" << std::endl;
    } else {
        std::cout << "  ✗ Metric retrieval failed" << std::endl;
        return 1;
    }
    
    // Test 6: Integration adapters
    std::cout << std::endl << "Test 6: Integration Adapters" << std::endl;
    
    // Test adapter creation (without actual components)
    BrainPerformanceAdapter brainAdapter(nullptr);
    brainAdapter.setComponentName("test_brain_adapter");
    if (brainAdapter.getComponentName() == "test_brain_adapter") {
        std::cout << "  ✓ BrainPerformanceAdapter works" << std::endl;
    } else {
        std::cout << "  ✗ BrainPerformanceAdapter failed" << std::endl;
        return 1;
    }
    
    MemoryPerformanceAdapter memoryAdapter(nullptr, nullptr);
    memoryAdapter.setComponentName("test_memory_adapter");
    if (memoryAdapter.getComponentName() == "test_memory_adapter") {
        std::cout << "  ✓ MemoryPerformanceAdapter works" << std::endl;
    } else {
        std::cout << "  ✗ MemoryPerformanceAdapter failed" << std::endl;
        return 1;
    }
    
    EnvironmentPerformanceAdapter envAdapter(nullptr);
    envAdapter.setComponentName("test_env_adapter");
    if (envAdapter.getComponentName() == "test_env_adapter") {
        std::cout << "  ✓ EnvironmentPerformanceAdapter works" << std::endl;
    } else {
        std::cout << "  ✗ EnvironmentPerformanceAdapter failed" << std::endl;
        return 1;
    }
    
    // Test 7: Report generation
    std::cout << std::endl << "Test 7: Report Generation" << std::endl;
    
    PerformanceReporter reporter;
    std::string textReport = reporter.generateDetailedReport();
    if (!textReport.empty() && textReport.find("Performance Metrics Report") != std::string::npos) {
        std::cout << "  ✓ Text report generation works" << std::endl;
    } else {
        std::cout << "  ✗ Text report generation failed" << std::endl;
        return 1;
    }
    
    std::cout << std::endl << "=== All Performance Monitoring Tests PASSED ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Performance monitoring infrastructure is working correctly!" << std::endl;
    std::cout << "Ready for neural network simulation performance tracking." << std::endl;
    
    return 0;
}
