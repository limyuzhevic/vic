#include <iostream>
#include <chrono>
#include <thread>

// Simple test to compile and run
int main() {
    std::cout << "Performance Infrastructure Implementation Test" << std::endl;
    
    // Test 1: Basic PerformanceBudget
    {
        std::cout << "\n1. Testing PerformanceBudget:" << std::endl;
        nlm::PerformanceBudget budget(1.0, 1024.0, 10000);
        
        double stepTime = 500.0; // ms
        size_t events = 5000;
        size_t memoryBytes = 512 * 1024; // 512KB
        
        bool withinBudget = budget.isStepWithinBudget(stepTime, events, memoryBytes);
        std::cout << "   Budget check: " << (withinBudget ? "PASS" : "FAIL") << std::endl;
        
        budget.updateBudget(stepTime, events, memoryBytes);
        
        std::cout << "   Remaining time budget: " << budget.getRemainingTimeBudget() << "ms" << std::endl;
        std::cout << "   Remaining event budget: " << budget.getRemainingEventBudget() << std::endl;
        std::cout << "   Remaining memory budget: " << budget.getRemainingMemoryBudget() << " bytes" << std::endl;
    }
    
    // Test 2: Basic SIMDEngine
    {
        std::cout << "\n2. Testing SIMDEngine:" << std::endl;
        nlm::SIMDEngine& simdEngine = nlm::SIMDEngine::getInstance();
        simdEngine.detectSIMDCapabilities();
        
        const char* simdType = simdEngine.getSIMDType();
        std::cout << "   SIMD type: " << simdType << std::endl;
        std::cout << "   Has AVX512: " << (simdEngine.hasAVX512() ? "Yes" : "No") << std::endl;
        std::cout << "   Has AVX2: " << (simdEngine.hasAVX2() ? "Yes" : "No") << std::endl;
        std::cout << "   Has SSE: " << (simdEngine.hasSSE() ? "Yes" : "No") << std::endl;
    }
    
    // Test 3: Basic AdaptiveProcessor
    {
        std::cout << "\n3. Testing AdaptiveProcessor:" << std::endl;
        nlm::AdaptiveProcessor& adaptiveProcessor = nlm::AdaptiveProcessor::getInstance();
        
        nlm::PerformanceMetrics metrics;
        metrics.cpuUsagePercent = 75.0;
        metrics.memoryUsageBytes = 512 * 1024 * 1024;
        metrics.spikesGenerated = 5000;
        metrics.stepTimeMs = 500.0;
        metrics.synapticEfficiency = 0.8;
        metrics.totalThreads = 4;
        
        nlm::RuntimePerformanceConfig config = 
            adaptiveProcessor.getOptimalConfiguration(metrics);
        
        std::cout << "   Multithreading enabled: " << (config.useMultithreading ? "Yes" : "No") << std::endl;
        std::cout << "   SIMD enabled: " << (config.useSIMD ? "Yes" : "No") << std::endl;
        std::cout << "   Thread count: " << config.threadCount << std::endl;
        std::cout << "   Event queue capacity: " << config.eventQueueCapacity << std::endl;
    }
    
    // Test 4: Basic PerformanceProfiler
    {
        std::cout << "\n4. Testing PerformanceProfiler:" << std::endl;
        nlm::PerformanceProfiler& profiler = nlm::PerformanceProfiler::getInstance();
        profiler.enable(true);
        
        profiler.startProfiling("test_operation_1");
        // Simulate work
        double result = 0.0;
        for (int i = 0; i < 1000; ++i) {
            result += static_cast<double>(i) * 1.5;
        }
        profiler.stopProfiling("test_operation_1");
        
        profiler.startProfiling("test_operation_2");
        // Simulate more work
        for (int i = 0; i < 2000; ++i) {
            result += static_cast<double>(i) / 2.0;
        }
        profiler.stopProfiling("test_operation_2");
        
        double avgTime1 = profiler.getAverageTime("test_operation_1");
        double avgTime2 = profiler.getAverageTime("test_operation_2");
        size_t count1 = profiler.getCallCount("test_operation_1");
        size_t count2 = profiler.getCallCount("test_operation_2");
        
        std::cout << "   Operation 1 - Average time: " << avgTime1 << "ms, Calls: " << count1 << std::endl;
        std::cout << "   Operation 2 - Average time: " << avgTime2 << "ms, Calls: " << count2 << std::endl;
    }
    
    // Test 5: Global Performance Functions
    {
        std::cout << "\n5. Testing Global Performance Functions:" << std::endl;
        
        nlm::RuntimePerformanceConfig config;
        config.useMemoryPool = true;
        config.useMultithreading = true;
        config.useSIMD = true;
        config.threadCount = 4;
        
        nlm::setRuntimePerformanceConfig(config);
        
        nlm::RuntimePerformanceConfig globalConfig = nlm::getRuntimePerformanceConfig();
        std::cout << "   Global config - Thread count: " << globalConfig.threadCount << std::endl;
        std::cout << "   Global config - Memory pool: " << (globalConfig.useMemoryPool ? "Enabled" : "Disabled") << std::endl;
        
        nlm::PerformanceMetrics metrics;
        metrics.stepTimeMs = 450.0;
        metrics.spikesProcessed = 1000;
        metrics.memoryUsageBytes = 256 * 1024 * 1024;
        
        nlm::setCurrentPerformanceMetrics(metrics);
        
        nlm::PerformanceMetrics retrievedMetrics = nlm::getCurrentPerformanceMetrics();
        std::cout << "   Retrieved metrics - Step time: " << retrievedMetrics.stepTimeMs << "ms" << std::endl;
    }
    
    std::cout << "\n=== All Performance Tests Completed ===" << std::endl;
    return 0;
}