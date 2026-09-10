// Performance and Profiling for NLM Neural Simulation
// Provides performance monitoring and optimization capabilities for the NLM brain simulation

#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

namespace nlm {

// Performance metrics collection
struct PerformanceMetrics {
    double totalSimulationTime;           // Total simulation time in seconds
    double timestep;                     // Simulation timestep
    size_t totalSteps;                   // Number of simulation steps
    size_t totalNeurons;                 // Total number of neurons
    size_t totalSynapses;                // Total number of synapses
    size_t totalSpikes;                  // Total number of spikes generated
    size_t sensoryEvents;                // Number of sensory processing events
    size_t motorCommands;                // Number of motor commands generated
    double averageFiringRate;            // Average firing rate across all neurons
    double energyConsumption;            // Estimated energy consumption
    double memoryUsage;                  // Memory usage in MB
    std::map<std::string, double> componentTimes;  // Time spent in each component
    
    PerformanceMetrics()
        : totalSimulationTime(0.0)
        , timestep(0.0)
        , totalSteps(0)
        , totalNeurons(0)
        , totalSynapses(0)
        , totalSpikes(0)
        , sensoryEvents(0)
        , motorCommands(0)
        , averageFiringRate(0.0)
        , energyConsumption(0.0)
        , memoryUsage(0.0) {}
};

// Time measurement utility
class Timer {
public:
    Timer() : startTime(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return duration.count() / 1000000.0; // Convert to seconds
    }
    
    void reset() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
};

// Performance profiler for NLM components
class PerformanceProfiler {
public:
    PerformanceProfiler() = default;
    
    // Start profiling a component
    void startProfiling(const std::string& componentName) {
        timers[componentName] = Timer();
    }
    
    // Stop profiling and record the time
    double stopProfiling(const std::string& componentName) {
        if (timers.find(componentName) != timers.end()) {
            double elapsed = timers[componentName].elapsed();
            componentTimes[componentName] += elapsed;
            timers.erase(componentName);
            return elapsed;
        }
        return 0.0;
    }
    
    // Get accumulated time for a component
    double getComponentTime(const std::string& componentName) const {
        auto it = componentTimes.find(componentName);
        return (it != componentTimes.end()) ? it->second : 0.0;
    }
    
    // Get all component times
    const std::map<std::string, double>& getComponentTimes() const {
        return componentTimes;
    }
    
    // Reset all timers
    void reset() {
        componentTimes.clear();
        timers.clear();
    }
    
    // Get total profiling time
    double getTotalTime() const {
        double total = 0.0;
        for (const auto& pair : componentTimes) {
            total += pair.second;
        }
        return total;
    }
    
    // Generate performance report
    std::string generateReport() const {
        std::string report = "=== NLM Performance Report ===\n\n";
        report += "Component Performance Times (seconds):\n";
        
        for (const auto& pair : componentTimes) {
            report += "  " + pair.first + ": " + std::to_string(pair.second) + "\n";
        }
        
        report += "\nTotal Time: " + std::to_string(getTotalTime()) + "s\n";
        return report;
    }
    
private:
    std::map<std::string, Timer> timers;
    std::map<std::string, double> componentTimes;
};

// Memory profiler for NLM
class MemoryProfiler {
public:
    MemoryProfiler() = default;
    
    // Get current memory usage in MB
    double getMemoryUsage() const {
        // This is a placeholder - implement platform-specific memory measurement
        return 0.0;
    }
    
    // Get peak memory usage
    double getPeakMemoryUsage() const {
        return peakMemoryUsage;
    }
    
    // Reset memory tracking
    void reset() {
        peakMemoryUsage = 0.0;
    }
    
    // Update memory usage
    void updateMemoryUsage(double currentUsage) {
        if (currentUsage > peakMemoryUsage) {
            peakMemoryUsage = currentUsage;
        }
    }
    
private:
    double peakMemoryUsage = 0.0;
};

// Simulation performance analyzer
class SimulationPerformanceAnalyzer {
public:
    SimulationPerformanceAnalyzer() = default;
    
    // Analyze performance metrics
    void analyzePerformance(const PerformanceMetrics& metrics) {
        currentMetrics = metrics;
        generateInsights();
    }
    
    // Get performance insights
    const std::vector<std::string>& getPerformanceInsights() const {
        return insights;
    }
    
    // Get performance score (0-100)
    int getPerformanceScore() const {
        // Simple scoring based on various metrics
        int score = 100;
        
        // Deduct for high memory usage
        if (currentMetrics.memoryUsage > 1000.0) {
            score -= 20;
        } else if (currentMetrics.memoryUsage > 500.0) {
            score -= 10;
        }
        
        // Deduct for low spike rate (indicates underutilization)
        if (currentMetrics.averageFiringRate < 0.1) {
            score -= 15;
        } else if (currentMetrics.averageFiringRate < 0.5) {
            score -= 5;
        }
        
        // Deduct for high energy consumption
        if (currentMetrics.energyConsumption > 1000.0) {
            score -= 10;
        } else if (currentMetrics.energyConsumption > 500.0) {
            score -= 5;
        }
        
        // Deduct for excessive simulation time
        if (currentMetrics.totalSimulationTime > 3600.0) {
            score -= 10;
        }
        
        return std::max(0, score);
    }
    
    // Generate performance insights
    void generateInsights() {
        insights.clear();
        
        if (currentMetrics.memoryUsage > 500.0) {
            insights.push_back("High memory usage detected - consider memory optimization");
        }
        
        if (currentMetrics.averageFiringRate < 0.1) {
            insights.push_back("Low average firing rate - simulation may be underutilized");
        }
        
        if (currentMetrics.energyConsumption > 100.0) {
            insights.push_back("High energy consumption - consider efficiency improvements");
        }
        
        if (currentMetrics.totalSteps > 10000) {
            insights.push_back("Many simulation steps - consider performance optimization");
        }
        
        if (currentMetrics.sensoryEvents < currentMetrics.totalSteps * 0.1) {
            insights.push_back("Low sensory processing - consider increasing environment interaction");
        }
        
        if (currentMetrics.motorCommands < currentMetrics.totalSteps * 0.1) {
            insights.push_back("Low motor command generation - consider improving motor control");
        }
    }
    
private:
    PerformanceMetrics currentMetrics;
    std::vector<std::string> insights;
};

// Complete NLM performance monitoring
class NLMPerformanceMonitor {
public:
    NLMPerformanceMonitor() = default;
    
    // Start monitoring
    void startMonitoring() {
        profiler.reset();
        memoryProfiler.reset();
        simulationClock = std::chrono::high_resolution_clock::now();
    }
    
    // Stop monitoring and collect metrics
    PerformanceMetrics stopMonitoring() {
        auto endClock = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endClock - simulationClock);
        
        PerformanceMetrics metrics;
        metrics.totalSimulationTime = duration.count() / 1000000.0;
        
        // Collect profiling data
        metrics.componentTimes = profiler.getComponentTimes();
        
        // Get memory usage
        metrics.memoryUsage = memoryProfiler.getMemoryUsage();
        
        // Add simulated metrics (replace with actual data collection)
        metrics.totalSteps = 1000;
        metrics.totalNeurons = 500;
        metrics.totalSynapses = 5000;
        metrics.totalSpikes = 25000;
        metrics.sensoryEvents = 800;
        metrics.motorCommands = 750;
        metrics.averageFiringRate = metrics.totalSpikes / metrics.totalNeurons;
        metrics.energyConsumption = metrics.totalSpikes * 0.01;
        
        return metrics;
    }
    
    // Get performance analyzer
    PerformanceAnalyzer& getAnalyzer() {
        return analyzer;
    }
    
    // Get performance profiler
    PerformanceProfiler& getProfiler() {
        return profiler;
    }
    
    // Get memory profiler
    MemoryProfiler& getMemoryProfiler() {
        return memoryProfiler;
    }
    
    // Generate complete performance report
    std::string generateCompleteReport() const {
        std::string report = "=== NLM Performance Monitoring Report ===\n\n";
        
        // Get latest metrics (this would be stored from last monitoring session)
        report += analyzer.generateReport() + "\n";
        
        // Add performance insights
        report += "Performance Insights:\n";
        for (const auto& insight : analyzer.getPerformanceInsights()) {
            report += "  - " + insight + "\n";
        }
        
        report += "\nPerformance Score: " + std::to_string(analyzer.getPerformanceScore()) + "/100\n";
        
        return report;
    }
    
private:
    PerformanceProfiler profiler;
    MemoryProfiler memoryProfiler;
    SimulationPerformanceAnalyzer analyzer;
    std::chrono::high_resolution_clock::time_point simulationClock;
};

} // namespace nlm
