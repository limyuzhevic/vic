#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "../Logger/Logger.hpp"

namespace nlm {

// Performance metrics for simulation timing
struct PerformanceMetrics {
    std::string simulationName;
    uint64_t totalSteps;
    double totalWallTime;
    double totalCpuTime;
    double avgStepTime;
    double minStepTime;
    double maxStepTime;
    double stepsPerSecond;
    size_t peakMemoryUsage;
    size_t currentMemoryUsage;
    uint64_t totalSpikes;
    double totalFiringRate;
    
    PerformanceMetrics() : totalSteps(0), totalWallTime(0.0), totalCpuTime(0.0),
                          avgStepTime(0.0), minStepTime(0.0), maxStepTime(0.0),
                          stepsPerSecond(0.0), peakMemoryUsage(0), currentMemoryUsage(0),
                          totalSpikes(0), totalFiringRate(0.0) {}
};

// Timer for measuring intervals
class Timer {
public:
    Timer() : startTime(std::chrono::high_resolution_clock::now()) {}
    
    void reset() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    double elapsed() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return duration.count() / 1000000.0; // Convert to seconds
    }
    
    std::chrono::high_resolution_clock::time_point getStart() const { return startTime; }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
};

// Performance monitor for simulation profiling
class PerformanceMonitor {
public:
    PerformanceMonitor() = default;
    ~PerformanceMonitor() = default;
    
    // Start monitoring a simulation
    void startSimulation(const std::string& name) {
        currentTimer.reset();
        simulationName = name;
        stepTimes.clear();
        
        NLM_LOG_INFO("Performance monitoring started for simulation: " + name);
    }
    
    // Record a simulation step
    void recordStep(uint64_t step, size_t spikeCount, double firingRate) {
        double stepTime = currentTimer.elapsed();
        stepTimes.push_back(stepTime);
        
        // Update metrics
        currentMetrics.totalSteps = step + 1;
        currentMetrics.totalSpikes += spikeCount;
        currentMetrics.totalFiringRate += firingRate;
        currentMetrics.totalWallTime += stepTime;
        
        // Update min/max step times
        if (stepTimes.size() == 1) {
            currentMetrics.minStepTime = stepTime;
            currentMetrics.maxStepTime = stepTime;
        } else {
            currentMetrics.minStepTime = std::min(currentMetrics.minStepTime, stepTime);
            currentMetrics.maxStepTime = std::max(currentMetrics.maxStepTime, stepTime);
        }
    }
    
    // Stop monitoring and calculate final metrics
    PerformanceMetrics stopSimulation() {
        double totalWallTime = currentTimer.elapsed();
        
        currentMetrics.totalWallTime = totalWallTime;
        currentMetrics.totalCpuTime = totalWallTime; // Simplified CPU time estimation
        
        if (!stepTimes.empty()) {
            currentMetrics.avgStepTime = currentMetrics.totalWallTime / stepTimes.size();
            currentMetrics.stepsPerSecond = stepTimes.size() / currentMetrics.totalWallTime;
            currentMetrics.totalFiringRate /= stepTimes.size();
        }
        
        // Get memory usage
        currentMetrics.currentMemoryUsage = getMemoryUsage();
        currentMetrics.peakMemoryUsage = currentMetrics.currentMemoryUsage; // Simplified
        
        NLM_LOG_INFO("Performance monitoring stopped for simulation: " + simulationName);
        NLM_LOG_INFO("Total steps: " + std::to_string(currentMetrics.totalSteps));
        NLM_LOG_INFO("Total wall time: " + std::to_string(currentMetrics.totalWallTime) + "s");
        NLM_LOG_INFO("Average step time: " + std::to_string(currentMetrics.avgStepTime) + "s");
        NLM_LOG_INFO("Steps per second: " + std::to_string(currentMetrics.stepsPerSecond));
        NLM_LOG_INFO("Total spikes: " + std::to_string(currentMetrics.totalSpikes));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(currentMetrics.totalFiringRate));
        
        // Generate performance report
        generatePerformanceReport();
        
        return currentMetrics;
    }
    
    // Get current metrics
    const PerformanceMetrics& getCurrentMetrics() const { return currentMetrics; }
    
    // Get simulation name
    const std::string& getSimulationName() const { return simulationName; }
    
    // Check if monitoring is active
    bool isMonitoring() const { return !simulationName.empty(); }
    
    // Generate detailed performance report
    std::string generatePerformanceReport() {
        std::ostringstream report;
        report << "=== Performance Report ===\n";
        report << "Simulation: " << simulationName << "\n";
        report << "Total Steps: " << currentMetrics.totalSteps << "\n";
        report << "Total Wall Time: " << currentMetrics.totalWallTime << "s\n";
        report << "Average Step Time: " << currentMetrics.avgStepTime << "s\n";
        report << "Min Step Time: " << currentMetrics.minStepTime << "s\n";
        report << "Max Step Time: " << currentMetrics.maxStepTime << "s\n";
        report << "Steps Per Second: " << currentMetrics.stepsPerSecond << "\n";
        report << "Total Spikes: " << currentMetrics.totalSpikes << "\n";
        report << "Average Firing Rate: " << currentMetrics.totalFiringRate << "\n";
        report << "Peak Memory Usage: " << (currentMetrics.peakMemoryUsage / 1024 / 1024) << " MB\n";
        report << "Current Memory Usage: " << (currentMetrics.currentMemoryUsage / 1024 / 1024) << " MB\n";
        report << "\n=== Step Time Distribution ===\n";
        
        if (!stepTimes.empty()) {
            std::vector<double> sortedTimes = stepTimes;
            std::sort(sortedTimes.begin(), sortedTimes.end());
            
            // Calculate percentiles
            auto p50 = sortedTimes.size() * 0.5;
            auto p90 = sortedTimes.size() * 0.9;
            auto p95 = sortedTimes.size() * 0.95;
            auto p99 = sortedTimes.size() * 0.99;
            
            report << "P50 (median): " << sortedTimes[static_cast<size_t>(p50)] << "s\n";
            report << "P90: " << sortedTimes[static_cast<size_t>(p90)] << "s\n";
            report << "P95: " << sortedTimes[static_cast<size_t>(p95)] << "s\n";
            report << "P99: " << sortedTimes[static_cast<size_t>(p99)] << "s\n";
        }
        
        // Write report to file
        std::string filename = "performance_report_" + simulationName + ".txt";
        std::ofstream file(filename);
        if (file.is_open()) {
            file << report.str();
            file.close();
            NLM_LOG_INFO("Performance report written to: " + filename);
        }
        
        return report.str();
    }
    
    // Get memory usage in bytes
    static size_t getMemoryUsage() {
        // Implementation varies by platform
        // For now, return 0 (platform-specific implementation needed)
        return 0;
    }
    
private:
    std::string simulationName;
    PerformanceMetrics currentMetrics;
    Timer currentTimer;
    std::vector<double> stepTimes;
};

// Global performance monitor instance
class GlobalPerformanceMonitor {
public:
    static GlobalPerformanceMonitor& getInstance() {
        static GlobalPerformanceMonitor instance;
        return instance;
    }
    
    void startSimulation(const std::string& name) {
        monitor.startSimulation(name);
        NLM_LOG_INFO("Global performance monitoring activated for simulation: " + name);
    }
    
    void recordStep(uint64_t step, size_t spikeCount, double firingRate) {
        if (monitor.isMonitoring()) {
            monitor.recordStep(step, spikeCount, firingRate);
        }
    }
    
    void stopSimulation() {
        if (monitor.isMonitoring()) {
            PerformanceMetrics metrics = monitor.stopSimulation();
            NLM_LOG_INFO("Global performance monitoring completed");
            NLM_LOG_INFO("Performance summary: " + std::to_string(metrics.totalSteps) + 
                        " steps, " + std::to_string(metrics.totalWallTime) + "s total");
        }
    }
    
    const PerformanceMetrics& getMetrics() const { return monitor.getCurrentMetrics(); }
    
    bool isMonitoring() const { return monitor.isMonitoring(); }
    
    std::string generateReport() {
        return monitor.generatePerformanceReport();
    }
    
private:
    GlobalPerformanceMonitor() = default;
    ~GlobalPerformanceMonitor() = default;
    GlobalPerformanceMonitor(const GlobalPerformanceMonitor&) = delete;
    GlobalPerformanceMonitor& operator=(const GlobalPerformanceMonitor&) = delete;
    
    PerformanceMonitor monitor;
};

// Macro for easy performance monitoring
#define NLM_PERF_START(name) GlobalPerformanceMonitor::getInstance().startSimulation(name)
#define NLM_PERF_RECORD(step, spikes, firing_rate) GlobalPerformanceMonitor::getInstance().recordStep(step, spikes, firing_rate)
#define NLM_PERF_STOP() GlobalPerformanceMonitor::getInstance().stopSimulation()

} // namespace nlm
