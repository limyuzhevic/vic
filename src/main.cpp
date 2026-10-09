#include "PerformanceProfiler.hpp"
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    std::cout << "NLM Performance Profiler Demo" << std::endl;
    std::cout << "=================================" << std::endl;
    
    // Create performance profiler
    nlm::PerformanceProfiler profiler;
    
    // Configure profiler for active monitoring
    profiler.configure(nlm::PerformanceProfiler::Mode::Active);
    
    // Start monitoring
    profiler.startMonitoring();
    
    // Simulate some work
    std::cout << "Starting simulation..." << std::endl;
    
    for (int i = 0; i < 10; ++i) {
        // Simulate spike processing
        profiler.startProfile("spike_processing");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        profiler.endProfile("spike_processing");
        
        // Simulate plasticity computation
        profiler.startProfile("plasticity_computation");
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        profiler.endProfile("plasticity_computation");
        
        // Simulate memory operations
        profiler.recordResourceMetric("memory_usage", 1024.0 * (i + 1), "KB");
        
        // Simulate CPU usage
        profiler.recordResourceMetric("cpu_usage", 50.0 + (i * 5), "percent");
        
        // Simulate count metrics
        profiler.recordCountMetric("spike_count", i * 100, "spikes");
        profiler.recordCountMetric("plasticity_events", i * 50, "events");
        
        std::cout << "Step " << i + 1 << "/10 completed" << std::endl;
    }
    
    // Generate performance report
    std::cout << "\nGenerating performance report..." << std::endl;
    auto report = profiler.generateReport();
    
    std::cout << "\nPerformance Report Summary:" << std::endl;
    std::cout << "==========================" << std::endl;
    std::cout << "Timestamp: " << report.timestamp << std::endl;
    std::cout << "Step: " << report.step << std::endl;
    std::cout << "Total CPU Usage: " << (report.totalCpuUsage * 100) << "%" << std::endl;
    std::cout << "Total Memory Usage: " << (report.totalMemoryUsage / 1024) << " KB" << std::endl;
    std::cout << "Total Simulation Time: " << report.totalSimulationTime << "s" << std::endl;
    
    std::cout << "\nPerformance Analysis:" << std::endl;
    std::cout << "==================" << std::endl;
    
    // Analyze bottlenecks
    std::vector<std::string> bottlenecks;
    profiler.analyzeBottlenecks(bottlenecks);
    
    if (!bottlenecks.empty()) {
        std::cout << "Bottlenecks identified:" << std::endl;
        for (const auto& bottleneck : bottlenecks) {
            std::cout << "  - " << bottleneck << std::endl;
        }
    } else {
        std::cout << "No bottlenecks detected." << std::endl;
    }
    
    // Generate recommendations
    std::cout << "\nOptimization Recommendations:" << std::endl;
    std::cout << "===========================" << std::endl;
    
    std::vector<std::string> recommendations;
    profiler.generateOptimizationRecommendations(recommendations);
    
    if (!recommendations.empty()) {
        for (const auto& recommendation : recommendations) {
            std::cout << "  - " << recommendation << std::endl;
        }
    } else {
        std::cout << "No recommendations available." << std::endl;
    }
    
    // Take performance snapshot
    auto snapshot = profiler.takeSnapshot();
    std::cout << "\nPerformance Snapshot:" << std::endl;
    std::cout << "===================" << std::endl;
    std::cout << "CPU Usage: " << (snapshot.cpuUsage * 100) << "%" << std::endl;
    std::cout << "Memory Usage: " << (snapshot.memoryUsage / 1024) << " KB" << std::endl;
    std::cout << "Active Monitors: " << snapshot.activeMonitors << std::endl;
    
    // Stop monitoring
    profiler.stopMonitoring();
    
    std::cout << "\nPerformance profiler completed successfully!" << std::endl;
    return 0;
}
