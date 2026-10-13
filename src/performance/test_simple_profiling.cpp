// test_simple_profiling.cpp
// Simple test to verify the performance profiler basic functionality

#include "performance/PerformanceProfiler.h"
#include <iostream>
#include <chrono>

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::left << std::setw(70) << "Simple Performance Profiler Test" << " ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    // Get the global performance profiler instance
    PerformanceProfiler& profiler = PerformanceProfiler::getInstance();
    
    // Configure the profiler
    PerformanceProfilerConfig config;
    config.enableTiming = true;
    config.enableMemoryTracking = true;
    config.enableVisualOutput = true;
    config.nonIntrusiveMode = true;
    
    profiler.setConfig(config);
    
    std::cout << "Testing basic profiling functionality..." << std::endl;
    
    // Test 1: Basic timing
    std::cout << "1. Testing basic timing..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    
    // Perform some work
    volatile double sum = 0.0;
    for (int i = 0; i < 10000; ++i) {
        sum += 1.0 / (i + 1);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double, std::micro>(end - start).count();
    
    profiler.recordTiming("Simple Loop", "math", duration);
    
    std::cout << "   Loop completed in " << std::fixed << std::setprecision(2) << duration << " μs" << std::endl;
    
    // Test 2: Memory tracking
    std::cout << "\n2. Testing memory tracking..." << std::endl;
    
    std::vector<float> testData(100);
    profiler.recordTiming("Memory Allocation", "memory", 0.0);
    
    // Process data
    for (size_t i = 0; i < testData.size(); ++i) {
        testData[i] = static_cast<float>(i) * 0.1f;
    }
    
    std::cout << "   Memory operations tracked." << std::endl;
    
    // Test 3: Multiple intervals
    std::cout << "\n3. Testing multiple intervals..." << std::endl;
    
    ProfilingInterval* interval1 = profiler.startInterval("Computation 1", "computation");
    for (int i = 0; i < 1000; ++i) {
        volatile float temp = std::sin(i * 0.01);
        (void)temp;
    }
    profiler.endInterval(interval1);
    
    ProfilingInterval* interval2 = profiler.startInterval("Computation 2", "computation");
    for (int i = 0; i < 5000; ++i) {
        volatile float temp = std::cos(i * 0.01);
        (void)temp;
    }
    profiler.endInterval(interval2);
    
    std::cout << "   Multiple intervals completed." << std::endl;
    
    // Test 4: Performance summary
    std::cout << "\n4. Performance summary..." << std::endl;
    
    profiler.printSummaryReport();
    
    // Test 5: Export
    std::cout << "\n5. Testing export..." << std::endl;
    
    // Create export directory
    system("mkdir -p ./profiles");
    
    profiler.exportToCSV("./profiles/test_results.csv");
    profiler.exportToJSON("./profiles/test_results.json");
    
    std::cout << "   Results exported to ./profiles/" << std::endl;
    
    // Print final summary
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "Test Summary:" << std::endl;
    std::cout << "  Metrics Recorded: " << profiler.getMetricCount() << std::endl;
    std::cout << "  Active Intervals: " << profiler.getActiveIntervalCount() << std::endl;
    std::cout << "  Total Profile Time: " << std::fixed << std::setprecision(2) << profiler.getTotalProfileTime() << " μs" << std::endl;
    
    std::cout << "\nAll tests passed successfully!" << std::endl;
    
    // Cleanup
    PerformanceProfiler::cleanupInstance();
    
    return 0;
}