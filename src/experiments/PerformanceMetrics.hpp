#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <thread>
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../experiments/ExperimentConfig.hpp"

namespace nlm {

// Performance counter for timing measurements
struct PerformanceCounter {
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    double duration; // in seconds
    std::string name;
    std::string description;
};

// Real-time performance monitoring data
struct RealTimeMetrics {
    Timestamp timestamp;              // Absolute timestamp
    SimulationStep currentStep;      // Current simulation step
    double cpuUsage;                  // CPU usage percentage (0-100)
    double memoryUsage;               // Memory usage in MB
    double memoryAvailable;           // Available memory in MB
    size_t neuronCount;               // Current neuron count
    size_t synapseCount;              // Current synapse count
    size_t spikeCount;                // Current spike count
    double firingRate;                // Average firing rate (Hz)
    double plasticityRate;            // Plasticity application rate
    double learningRate;              // Learning rate
    double avgSynapticWeight;         // Average synaptic weight
    double weightVariance;            // Synaptic weight variance
    double connectivityDensity;       // Synapse density (0-1)
    size_t activeNeuronCount;         // Number of firing neurons
    double maxMembranePotential;      // Maximum membrane potential
    double minMembranePotential;      // Minimum membrane potential
    double avgMembranePotential;      // Average membrane potential
};

// Performance benchmark results
struct BenchmarkResult {
    std::string benchmarkName;
    std::string benchmarkDescription;
    double duration;                  // Total duration in seconds
    size_t neuronCount;               // Neuron count for benchmark
    size_t synapseCount;              // Synapse count for benchmark
    double spikesPerSecond;           // Spikes processed per second
    double neuronsPerSecond;          // Neurons processed per second
    double synapsesPerSecond;         // Synapses processed per second
    double memoryUsage;               // Peak memory usage in MB
    double cpuUtilization;            // Average CPU utilization
    std::vector<double> metricValues; // Individual metric measurements
    std::string summary;
};

// Performance analysis results
struct PerformanceAnalysis {
    std::string analysisName;
    std::string analysisDescription;
    
    // Bottleneck identification
    std::vector<std::string> identifiedBottlenecks;
    std::vector<std::string> optimizationOpportunities;
    
    // Performance metrics summary
    double totalExecutionTime;        // Total execution time
    double cpuTimePercentage;         // CPU time percentage
    double memoryTimePercentage;      // Memory time percentage
    
    // Performance scaling analysis
    double scalingEfficiency;         // How well performance scales with neuron count
    double memoryEfficiency;          // Memory usage efficiency
    double computationalEfficiency;   // Computational efficiency
    
    // Recommendation list
    std::vector<std::string> recommendations;
    std::vector<std::pair<std::string, double>> suggestedImprovements;
    
    // Confidence scores
    double confidenceScore;           // Overall confidence in analysis (0-1)
    std::map<std::string, double> componentScores; // Confidence per component
    
    // Analysis timestamp
    Timestamp analysisTime;           // When analysis was performed
    
    std::string summary() const {
        std::stringstream ss;
        ss << "PerformanceAnalysis: " << analysisName << "\n";
        ss << "  Bottlenecks: " << identifiedBottlenecks.size() << " identified\n";
        ss << "  Optimization opportunities: " << optimizationOpportunities.size() << " found\n";
        ss << "  Confidence score: " << confidenceScore << "\n";
        ss << "  Recommendations: " << recommendations.size() << " provided\n";
        return ss.str();
    }
};

// PerformanceMetrics class for comprehensive performance monitoring
class PerformanceMetrics {
public:
    PerformanceMetrics();
    ~PerformanceMetrics();
    
    // Real-time monitoring
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Record real-time metrics
    void recordRealTimeMetrics(const RealTimeMetrics& metrics);
    RealTimeMetrics getCurrentRealTimeMetrics() const;
    
    // Performance benchmarking
    void startBenchmark(const std::string& name, const std::string& description);
    void endBenchmark();
    BenchmarkResult getCurrentBenchmark() const;
    std::vector<BenchmarkResult> getAllBenchmarks() const;
    
    // Metrics collection and analysis
    void recordMetric(const std::string& name, double value, const std::string& unit = "");
    double getMetric(const std::string& name) const;
    std::vector<double> getMetricHistory(const std::string& name) const;
    std::vector<std::string> getMetricNames() const;
    
    // Performance comparison
    double compareBenchmarks(const std::string& benchmark1, const std::string& benchmark2) const;
    PerformanceAnalysis analyzePerformance(const std::string& benchmarkName) const;
    PerformanceAnalysis analyzeScaling(const std::vector<size_t>& neuronCounts) const;
    
    // Performance profiling
    void recordExecutionTime(const std::string& operation, double duration);
    double getAverageExecutionTime(const std::string& operation) const;
    double getMinExecutionTime(const std::string& operation) const;
    double getMaxExecutionTime(const std::string& operation) const;
    
    // Metrics export and import
    bool exportMetrics(const std::string& filepath) const;
    bool importMetrics(const std::string& filepath);
    bool exportBenchmarks(const std::string& filepath) const;
    bool importBenchmarks(const std::string& filepath);
    
    // Report generation
    std::string generateReport() const;
    std::string generateBenchmarkReport() const;
    std::string generateRealTimeReport() const;
    
    // Statistics and analysis
    size_t getTotalRecordedMetrics() const;
    size_t getTotalBenchmarks() const;
    size_t getTotalRealTimeSamples() const;
    Timestamp getFirstMeasurementTime() const;
    Timestamp getLastMeasurementTime() const;
    double getMeasurementFrequency() const; // measurements per second
    
    // System-specific metrics
    void updateSystemMetrics();
    double getCpuUsage() const;
    double getMemoryUsage() const;
    double getMemoryAvailable() const;
    
    // Integration with experiment config
    void configureFromExperiment(const ExperimentConfig& config);
    bool validateConfiguration() const;
    
    // Cleanup
    void clearAllData();
    void cleanupOldData(Timestamp cutoffTime);
    
    // Get from brain
    static RealTimeMetrics extractMetricsFromBrain(const std::shared_ptr<Brain>& brain,
                                                   const ExperimentConfig& config,
                                                   const PerformanceMetrics& currentMetrics);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    void updateRealTimeMetrics(const ExperimentConfig& config, const std::shared_ptr<Brain>& brain);
    void generatePerformanceSummary();
    std::string formatMetricsReport(const std::string& reportType) const;
};

} // namespace nlm
