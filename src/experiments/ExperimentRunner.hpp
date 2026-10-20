#pragma once

#include "Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <vector>
#include <chrono>
#include <map>
#include <string>
#include <unordered_map>

namespace nlm {

// Performance metrics collection
struct PerformanceMetrics {
    // Timing measurements
    double totalSimulationTime = 0.0;           // Total simulation time in seconds
    double simulationSetupTime = 0.0;          // Time to setup simulation
    double brainInitializationTime = 0.0;     // Time to initialize brain
    double experimentRunTime = 0.0;            // Time to run experiments
    double cleanupTime = 0.0;                  // Time to cleanup
    
    // Performance statistics
    double stepsPerSecond = 0.0;               // Steps per second
    double neuronsPerSecond = 0.0;             // Neurons processed per second
    double synapsesPerSecond = 0.0;            // Synapses processed per second
    double averageSpikeRate = 0.0;             // Average spike rate per neuron
    double memoryUsageMB = 0.0;                // Memory usage in MB
    
    // Event timing
    std::unordered_map<std::string, double> eventTimes;  // Specific event timings
    std::unordered_map<std::string, size_t> eventCounts;  // Event counts
    
    // Performance snapshots
    std::vector<std::pair<double, double>> performanceSnapshots; // (timestamp, value)
    
    // Reset metrics
    void reset() {
        totalSimulationTime = 0.0;
        simulationSetupTime = 0.0;
        brainInitializationTime = 0.0;
        experimentRunTime = 0.0;
        cleanupTime = 0.0;
        stepsPerSecond = 0.0;
        neuronsPerSecond = 0.0;
        synapsesPerSecond = 0.0;
        averageSpikeRate = 0.0;
        memoryUsageMB = 0.0;
        eventTimes.clear();
        eventCounts.clear();
        performanceSnapshots.clear();
    }
};

// Performance monitoring system
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start monitoring
    void startMonitoring();
    
    // Stop monitoring
    void stopMonitoring();
    
    // Record a performance snapshot
    void recordSnapshot(const std::string& label);
    
    // Record an event timing
    void recordEvent(const std::string& eventName, double duration);
    
    // Record simulation metrics
    void recordSimulationMetrics(std::shared_ptr<Brain> brain, 
                                 SimulationStep totalSteps,
                                 double simulationTime);
    
    // Get current performance metrics
    PerformanceMetrics getMetrics() const;
    
    // Get performance summary
    std::string getPerformanceSummary() const;
    
    // Save performance data to file
    bool savePerformanceData(const std::string& filepath) const;
    
    // Load performance data from file
    bool loadPerformanceData(const std::string& filepath);
    
    // Enable/disable specific metrics collection
    void setMetricCollectionEnabled(const std::string& metric, bool enabled);
    bool isMetricCollectionEnabled(const std::string& metric) const;
    
    // Set monitoring interval
    void setMonitoringInterval(double interval);
    
    // Get performance statistics
    double getAverageStepsPerSecond() const;
    double getAverageMemoryUsageMB() const;
    double getAverageSpikeRate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced ExperimentRunner with performance monitoring
class ExperimentRunner {
public:
    ExperimentRunner();
    ~ExperimentRunner();
    
    // Enable performance monitoring
    void enablePerformanceMonitoring(bool enabled = true);
    
    // Set performance monitor
    void setPerformanceMonitor(std::shared_ptr<PerformanceMonitor> monitor);
    
    // Create new experiment
    std::shared_ptr<Experiment> createExperiment(const std::string& name);
    
    // Run single experiment with performance monitoring
    bool runExperiment(std::shared_ptr<Experiment> experiment,
                      std::shared_ptr<Brain> brain,
                      std::shared_ptr<Environment> environment,
                      SimulationStep maxSteps,
                      bool enableMonitoring = true);
    
    // Run batch of experiments with performance monitoring
    std::vector<std::shared_ptr<Experiment>> runBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps,
        bool enableMonitoring = true);
    
    // Get performance metrics
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Get performance summary
    std::string getPerformanceSummary() const;
    
    // Save experiment results with performance data
    bool saveResults(const std::string& filepath, bool includePerformance = true) const;
    
    // Get all experiments
    const std::vector<std::shared_ptr<Experiment>>& getExperiments() const;
    
    // Get experiment by name
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;
    
    // Clear all experiments
    void clearExperiments();
    
    // Set random seed for reproducibility
    void setGlobalSeed(uint64_t seed);
    
    // Run benchmark tests
    bool runBenchmarks(std::shared_ptr<Brain> brain,
                       std::shared_ptr<Environment> environment,
                       const std::vector<SimulationStep>& stepCounts);
    
    // Generate performance report
    std::string generatePerformanceReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
