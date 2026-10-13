// simple_performance_demo.cpp
// Simple demonstration of the NLM performance profiler

#include "performance/PerformanceProfiler.h"
#include <iostream>
#include <chrono>
#include <vector>

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║ " << std::left << std::setw(70) << "NLM Performance Profiler Demo" << " ║" << std::endl;
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
    config.enableExport = true;
    config.exportPath = "./profiles";
    
    profiler.setConfig(config);
    
    std::cout << "Starting performance profiling demo..." << std::endl;
    
    // Demo 1: Basic timing measurements
    std::cout << "\n1. Basic Timing Measurements:" << std::endl;
    
    // Time a simple loop
    auto start = std::chrono::high_resolution_clock::now();
    volatile double sum = 0.0;
    for (int i = 0; i < 10000; ++i) {
        sum += 1.0 / (i + 1);
    }
    auto end = std::chrono::high_resolution_clock::now();
    double loopDuration = std::chrono::duration<double, std::micro>(end - start).count();
    profiler.recordTiming("Simple Loop", "math", loopDuration);
    
    std::cout << "   Loop completed in " << std::fixed << std::setprecision(2) << loopDuration << " μs" << std::endl;
    
    // Demo 2: Vector operations
    std::cout << "\n2. Vector Operations:" << std::endl;
    
    start = std::chrono::high_resolution_clock::now();
    std::vector<double> numbers(1000);
    for (size_t i = 0; i < numbers.size(); ++i) {
        numbers[i] = i * 0.1;
    }
    
    // Process the vector
    double result = 0.0;
    for (const auto& num : numbers) {
        result += num * num;
    }
    
    end = std::chrono::high_resolution_clock::now();
    double vectorDuration = std::chrono::duration<double, std::micro>(end - start).count();
    profiler.recordTiming("Vector Processing", "memory", vectorDuration);
    
    std::cout << "   Vector processed in " << std::fixed << std::setprecision(2) << vectorDuration << " μs" << std::endl;
    
    // Demo 3: Memory tracking
    std::cout << "\n3. Memory Tracking:" << std::endl;
    
    std::vector<int> memoryTest(100);
    profiler.recordTiming("Memory Allocation", "memory", 0.0);  // Trigger memory tracking
    
    std::cout << "   Memory allocations tracked." << std::endl;
    
    // Demo 4: Multiple intervals
    std::cout << "\n4. Multiple Profiling Intervals:" << std::endl;
    
    ProfilingInterval* interval1 = profiler.startInterval("Interval 1", "computation");
    for (int i = 0; i < 1000; ++i) {
        volatile float temp = std::sin(i * 0.01);
        (void)temp;
    }
    profiler.endInterval(interval1);
    
    ProfilingInterval* interval2 = profiler.startInterval("Interval 2", "computation");
    for (int i = 0; i < 5000; ++i) {
        volatile float temp = std::cos(i * 0.01);
        (void)temp;
    }
    profiler.endInterval(interval2);
    
    std::cout << "   Multiple intervals profiled." << std::endl;
    
    // Print performance summary
    std::cout << "\nPerformance Summary:" << std::endl;
    std::cout << "  Total Metrics Recorded: " << profiler.getMetricCount() << std::endl;
    std::cout << "  Active Intervals: " << profiler.getActiveIntervalCount() << std::endl;
    std::cout << "  Total Profile Time: " << std::fixed << std::setprecision(2) << profiler.getTotalProfileTime() << " μs" << std::endl;
    
    // Generate reports
    std::cout << "\nGenerating reports..." << std::endl;
    
    profiler.printSummaryReport();
    
    // Export results
    profiler.exportToCSV("./profiles/demo_results.csv");
    profiler.exportToJSON("./profiles/demo_results.json");
    
    std::cout << "\nDemo completed successfully!" << std::endl;
    std::cout << "Reports generated in ./profiles/" << std::endl;
    
    // Cleanup
    PerformanceProfiler::cleanupInstance();
    
    return 0;
}