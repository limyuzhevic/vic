// Performance Monitor Implementation for NLM
// This file implements the performance monitoring and profiling capabilities

#include "PerformanceMonitor.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <fstream>
#include <algorithm>
#include <numeric>

namespace nlm {

PerformanceMonitor::PerformanceMonitor() : running(false), updateInterval(1.0), startTime(std::chrono::steady_clock::now()) {
    // Initialize standard performance counters
    counters["brain_initialization"] = std::make_unique<PerformanceCounter>();
    counters["simulation_step"] = std::make_unique<PerformanceCounter>();
    counters["spike_processing"] = std::make_unique<PerformanceCounter>();
    counters["plasticity_update"] = std::make_unique<PerformanceCounter>();
    counters["memory_operation"] = std::make_unique<PerformanceCounter>();
    counters["neuron_update"] = std::make_unique<PerformanceCounter>();
}

void PerformanceMonitor::monitorLoop() {
    auto lastUpdate = std::chrono::steady_clock::now();
    
    while (running) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - lastUpdate).count();
        
        if (elapsed >= updateInterval) {
            // Update system metrics
            updateSystemMetrics();
            lastUpdate = now;
        }
        
        // Sleep briefly to avoid busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void PerformanceMonitor::updateSystemMetrics() {
    // Update memory usage (simplified)
    updateMetric("memory_usage", getMemoryUsage());
    
    // Update CPU usage (simplified)
    updateMetric("cpu_usage", getCPUUsage());
    
    // Update thread count
    updateMetric("thread_count", getThreadCount());
    
    // Update spike statistics
    updateMetric("spike_count", getSpikeCount());
}

void PerformanceMonitor::printReport() const {
    PerformanceMetrics metrics = getMetrics();
    
    std::cout << "=== NLM Performance Report ===" << std::endl;
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Total execution time: " << metrics.totalTime << " seconds" << std::endl;
    std::cout << "Initialization time: " << metrics.initializationTime << " seconds" << std::endl;
    std::cout << "Average step time: " << metrics.stepTime << " seconds" << std::endl;
    std::cout << "Total spikes generated: " << metrics.totalSpikes << std::endl;
    std::cout << "Spike rate: " << metrics.spikeRate << " spikes/second" << std::endl;
    std::cout << "Firing rate: " << metrics.firingRate << " Hz" << std::endl;
    std::cout << "Total plasticity events: " << metrics.totalPlastictyEvents << std::endl;
    std::cout << "Throughput: " << metrics.throughput << " operations/second" << std::endl;
    std::cout << "CPU usage: " << metrics.cpuUsage << "%" << std::endl;
    std::cout << "Thread count: " << metrics.threadCount << std::endl;
    
    // Print counter details
    std::cout << std::endl << "Detailed Counter Statistics:" << std::endl;
    for (const auto& pair : counters) {
        const auto& counter = pair.second;
        if (counter->getCount() > 0) {
            std::cout << "  " << std::left << std::setw(25) << pair.first 
                      << ": count=" << counter->getCount()
                      << ", avg_time=" << counter->getAverageTime()
                      << ", min_time=" << counter->getMinTime()
                      << ", max_time=" << counter->getMaxTime()
                      << std::endl;
        }
    }
}

// Global performance monitor instance
PerformanceMonitor& getGlobalPerformanceMonitor() {
    static PerformanceMonitor monitor;
    return monitor;
}

} // namespace nlm

// Implementation file ends here
// This provides the Performance Monitor implementation for NLM
// The PerformanceMonitor.hpp header should be included in main.cpp to use these features