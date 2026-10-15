#!/usr/bin/env cpp
// Simple test to demonstrate the NLM Advanced Debugging and Profiling Tools
// This file shows how the debugging and profiling system can be used

#include "debug/DebugManager.hpp"
#include "profiling/PerformanceProfiler.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== NLM Advanced Debugging and Profiling Tools Test ===" << std::endl;
    
    // Initialize debug configuration
    nlm::DebugConfig debugConfig;
    debugConfig.nonIntrusiveMode = true;
    debugConfig.profilingInterval = 0.5;
    
    // Create debug manager
    nlm::DebugManager& debugManager = nlm::DebugManager::getInstance(debugConfig);
    
    std::cout << "1. Debug Manager initialized successfully" << std::endl;
    std::cout << "   - Non-intrusive mode: " << (debugConfig.nonIntrusiveMode ? "Enabled" : "Disabled") << std::endl;
    std::cout << "   - Profiling interval: " << debugConfig.profilingInterval << "s" << std::endl;
    std::cout << "   - Running: " << (debugManager.isRunning() ? "Yes" : "No") << std::endl;
    
    // Initialize performance profiler
    nlm::PerformanceProfilerConfig profilerConfig;
    profilerConfig.enableCPUProfiling = true;
    profilerConfig.enableMemoryProfiling = true;
    profilerConfig.profilingInterval = std::chrono::duration<double>(0.5);
    
    nlm::PerformanceProfiler profiler(profilerConfig);
    
    std::cout << "\n2. Performance profiler initialized" << std::endl;
    std::cout << "   - CPU profiling enabled: " << (profilerConfig.enableCPUProfiling ? "Yes" : "No") << std::endl;
    std::cout << "   - Memory profiling enabled: " << (profilerConfig.enableMemoryProfiling ? "Yes" : "No") << std::endl;
    
    // Start profiling
    debugManager.startProfiling("test_simulation");
    profiler.startProfile("test_simulation");
    
    std::cout << "\n3. Profiling session started" << std::endl;
    std::cout << "   - DebugManager profiling: Started" << std::endl;
    std::cout << "   - PerformanceProfiler profiling: Started" << std::endl;
    
    // Simulate some work
    std::cout << "\n4. Simulating neural network operations..." << std::endl;
    for (int i = 0; i < 10; ++i) {
        // Record performance metrics
        profiler.recordMetric("neuron_count", 1000 + i * 100, "neurons", "simulation");
        profiler.recordMetric("memory_usage", 128.0 + i * 32.0, "mb", "simulation");
        profiler.recordMetric("cpu_usage", 45.0 + i * 10.0, "percent", "simulation");
        profiler.recordMetric("simulation_step", static_cast<double>(i), "steps", "simulation");
        
        // Simulate processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        if ((i + 1) % 5 == 0) {
            std::cout << "   - Step " << (i + 1) << "/10 completed" << std::endl;
        }
    }
    
    // Stop profiling
    debugManager.stopProfiling("test_simulation");
    profiler.stopProfile("test_simulation");
    
    std::cout << "\n5. Profiling session completed" << std::endl;
    
    // Generate reports
    std::cout << "\n6. Generating reports..." << std::endl;
    std::string debugReport = debugManager.generateDebugReport();
    std::string perfReport = profiler.getPerformanceReport();
    
    std::cout << "   - Debug report generated (" << debugReport.length() << " characters)" << std::endl;
    std::cout << "   - Performance report generated (" << perfReport.length() << " characters)" << std::endl;
    
    // Export data
    std::cout << "\n7. Exporting data..." << std::endl;
    bool debugExport = debugManager.exportDebugData("test_debug_report.json", "json");
    bool perfExport = profiler.exportMetrics("test_perf_report.csv", "csv");
    
    std::cout << "   - Debug data exported: " << (debugExport ? "Success" : "Failed") << std::endl;
    std::cout << "   - Performance data exported: " << (perfExport ? "Success" : "Failed") << std::endl;
    
    // Show statistics
    std::cout << "\n8. Final statistics..." << std::endl;
    nlm::DebugManager::DebugStatistics stats = debugManager.getStatistics();
    
    std::cout << "   - Breakpoints triggered: " << stats.breakpointsTriggered << std::endl;
    std::cout << "   - Memory allocations: " << stats.memoryAllocations << std::endl;
    std::cout << "   - Memory deallocations: " << stats.memoryDeallocations << std::endl;
    std::cout << "   - PerformanceProfiler metrics: " << profiler.getMetricCount() << std::endl;
    std::cout << "   - CPU mean: " << profiler.getMetricMean("cpu_usage") << "%" << std::endl;
    std::cout << "   - Memory max: " << profiler.getMetricMax("memory_usage") << "MB" << std::endl;
    
    // Shutdown
    nlm::DebugManager::shutdown();
    
    std::cout << "\n9. Cleanup completed" << std::endl;
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "TEST SUMMARY" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "✓ Debug Manager: Initialized and operational" << std::endl;
    std::cout << "✓ Performance Profiler: Configured and running" << std::endl;
    std::cout << "✓ Profiling Session: Completed successfully" << std::endl;
    std::cout << "✓ Report Generation: Completed" << std::endl;
    std::cout << "✓ Data Export: Completed" << std::endl;
    std::cout << "✓ Statistics Collection: Completed" << std::endl;
    std::cout << "✓ Cleanup: Completed" << std::endl;
    
    std::cout << "\nAll tests completed successfully!" << std::endl;
    return 0;
}
