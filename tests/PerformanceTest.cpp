// PerformanceTest.cpp - Simple test to validate performance infrastructure
#include "performance/Performance.hpp"
#include "performance/SIMDEngine.hpp"
#include "performance/AdaptiveProcessor.hpp"
#include "performance/PerformanceBudget.hpp"
#include "performance/PerformanceProfiler.hpp"
#include <iostream>

int main() {
    std::cout << "=== Performance Infrastructure Test ===" << std::endl;
    
    // Test PerformanceBudget
    std::cout << "\n1. Testing PerformanceBudget...";
    nlm::PerformanceBudget budget(1.0, 1024.0, 10000);
    
    budget.updateBudget(500.0, 5000, 512 * 1024); // 500ms, 5000 events, 512KB
    bool withinBudget = budget.isStepWithinBudget(500.0, 5000, 512 * 1024);
    std::cout << " Budget check: " << (withinBudget ? "PASS" : "FAIL") << std::endl;
    
    // Test SIMDEngine
    std::cout << "\n2. Testing SIMDEngine...";
    nlm::SIMDEngine& simdEngine = nlm::SIMDEngine::getInstance();
    simdEngine.detectSIMDCapabilities();
    
    const char* simdType = simdEngine.getSIMDType();
    std::cout << " SIMD type: " << simdType << std::endl;
    
    // Test AdaptiveProcessor
    std::cout << "\n3. Testing AdaptiveProcessor...";
    nlm::AdaptiveProcessor& adaptiveProcessor = nlm::AdaptiveProcessor::getInstance();
    
    nlm::PerformanceMetrics metrics;
    metrics.cpuUsagePercent = 75.0;
    metrics.memoryUsageBytes = 512 * 1024 * 1024;
    metrics.spikesGenerated = 5000;
    metrics.stepTimeMs = 500.0;
    metrics.synapticEfficiency = 0.8;
    
    nlm::RuntimePerformanceConfig config = 
        adaptiveProcessor.getOptimalConfiguration(metrics);
    
    std::cout << " Adaptive config - Multithreading: " << (config.useMultithreading ? "Enabled" : "Disabled") << std::endl;
    std::cout << " Adaptive config - SIMD: " << (config.useSIMD ? "Enabled" : "Disabled") << std::endl;
    
    // Test PerformanceProfiler
    std::cout << "\n4. Testing PerformanceProfiler...";
    nlm::PerformanceProfiler& profiler = nlm::PerformanceProfiler::getInstance();
    profiler.enable(true);
    
    profiler.startProfiling("test_operation");
    // Simulate some work
    double sum = 0.0;
    for (int i = 0; i < 1000; ++i) {
        sum += static_cast<double>(i);
    }
    profiler.stopProfiling("test_operation");
    
    double avgTime = profiler.getAverageTime("test_operation");
    size_t callCount = profiler.getCallCount("test_operation");
    
    std::cout << " Operation average time: " << avgTime << "ms" << std::endl;
    std::cout << " Operation call count: " << callCount << std::endl;
    
    // Test global performance functions
    std::cout << "\n5. Testing Global Performance Functions...";
    nlm::setRuntimePerformanceConfig(config);
    
    nlm::RuntimePerformanceConfig globalConfig = nlm::getRuntimePerformanceConfig();
    std::cout << " Global config retrieved - Thread count: " << globalConfig.threadCount << std::endl;
    
    nlm::PerformanceMetrics globalMetrics = nlm::getCurrentPerformanceMetrics();
    std::cout << " Global metrics retrieved - Step time: " << globalMetrics.stepTimeMs << "ms" << std::endl;
    
    std::cout << "\n=== All Performance Infrastructure Tests Completed ===" << std::endl;
    return 0;
}