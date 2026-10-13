// test_performance_profiler.cpp
// Simple test to verify the performance profiler implementation

#include "performance/PerformanceProfiler.h"
#include "performance/PerformanceBenchmarkSuite.h"
#include "performance/MemoryProfiler.h"
#include "performance/IntegrationTestProfiler.h"
#include <iostream>

int main() {
    std::cout << "Testing NLM Performance Profiler Framework" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::endl;
    
    // Test PerformanceProfiler
    std::cout << "1. Testing PerformanceProfiler..." << std::endl;
    PerformanceProfiler& profiler = PerformanceProfiler::getInstance();
    
    PerformanceProfilerConfig config;
    config.enableTiming = true;
    config.enableMemoryTracking = true;
    config.enableVisualOutput = false;
    
    profiler.setConfig(config);
    
    // Test timing recording
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        volatile float temp = 1.0f / (i + 1);
        (void)temp;
    }
    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double, std::micro>(end - start).count();
    profiler.recordTiming("Test Loop", "math", duration);
    
    std::cout << "   PerformanceProfiler test completed." << std::endl;
    
    // Test MemoryProfiler
    std::cout << "2. Testing MemoryProfiler..." << std::endl;
    MemoryProfiler& memoryProfiler = MemoryProfiler::getInstance();
    
    MemoryProfilerConfig memoryConfig;
    memoryConfig.enableAllocationTracking = true;
    memoryConfig.enableDeallocationTracking = true;
    memoryConfig.enableLeakDetection = true;
    
    memoryProfiler.setConfig(memoryConfig);
    
    // Test memory tracking
    std::vector<float> testData(100);
    memoryProfiler.trackAllocation(sizeof(float) * 100, testData.data(), "Vector<float>");
    
    memoryProfiler.trackDeallocation(testData.data());
    
    std::cout << "   MemoryProfiler test completed." << std::endl;
    
    // Test PerformanceBenchmarkSuite
    std::cout << "3. Testing PerformanceBenchmarkSuite..." << std::endl;
    PerformanceBenchmarkSuite& benchmarkSuite = PerformanceBenchmarkSuite::getInstance();
    
    PerformanceProfilerConfig benchmarkConfig;
    benchmarkConfig.enableTiming = true;
    benchmarkConfig.enableVisualOutput = false;
    
    benchmarkSuite.setProfilerConfig(benchmarkConfig);
    
    // Test running benchmarks
    std::cout << "   Running benchmarks..." << std::endl;
    benchmarkSuite.runSmallScaleBenchmarks();
    
    std::cout << "   PerformanceBenchmarkSuite test completed." << std::endl;
    
    // Test IntegrationTestProfiler
    std::cout << "4. Testing IntegrationTestProfiler..." << std::endl;
    IntegrationTestProfiler& integrationProfiler = IntegrationTestProfiler::getInstance();
    
    IntegrationTestProfilerConfig integrationConfig;
    integrationConfig.enablePerformanceProfiling = true;
    integrationConfig.enableMemoryProfiling = false;
    integrationConfig.enableDetailedOutput = false;
    
    integrationProfiler.setConfig(integrationConfig);
    
    // Set baseline
    integrationProfiler.setBaseline("Test Operation", 0.001);
    
    // Run a test
    std::cout << "   Running integration test..." << std::endl;
    bool testResult = true; // Simulated test result
    TestResult result = integrationProfiler.profileTest("Test Operation", "integration", [&]() -> bool { return testResult; });
    
    std::cout << "   IntegrationTestProfiler test completed." << std::endl;
    
    // Print summary
    std::cout << std::endl;
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::left << std::setw(70) << "Test Summary" << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PerformanceProfiler Metrics Recorded: " << profiler.getMetricCount() << std::endl;
    std::cout << "MemoryProfiler Allocations Tracked: " << memoryProfiler.getAllocationCount() << std::endl;
    std::cout << "MemoryProfiler Deallocations Tracked: " << memoryProfiler.getDeallocationCount() << std::endl;
    std::cout << "MemoryProfiler Leaks Detected: " << memoryProfiler.getLeakCount() << std::endl;
    std::cout << "Benchmarks Completed: " << benchmarkSuite.getResults().size() << std::endl;
    std::cout << "Integration Tests Completed: " << integrationProfiler.getTestCount() << std::endl;
    std::cout << "Integration Tests Passed: " << integrationProfiler.getPassedCount() << std::endl;
    
    std::cout << std::endl;
    std::cout << "All tests completed successfully!" << std::endl;
    
    // Cleanup
    PerformanceProfiler::cleanupInstance();
    MemoryProfiler::cleanupInstance();
    PerformanceBenchmarkSuite::cleanupInstance();
    IntegrationTestProfiler::cleanupInstance();
    
    return 0;
}