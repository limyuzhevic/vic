// Performance Monitoring System for NLM Brain Simulation
// This file provides comprehensive performance monitoring and benchmarking
// capabilities for the NLM brain simulation, including metrics collection,
// performance analysis, and profiling tools.

#ifndef NLM_PERFORMANCE_MONITOR_H
#define NLM_PERFORMANCE_MONITOR_H

#include <chrono>
#include <vector>
#include <string>
#include <memory>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <thread>
#include <atomic>

namespace nlm {

// Performance Metrics Structure
struct PerformanceMetrics {
    // Timing metrics
    double simulationTime;           // Total simulation time (seconds)
    double wallClockTime;            // Wall clock time (seconds)
    double cpuTime;                  // CPU time (seconds)
    
    // Neural performance metrics
    size_t totalSpikeCount;          // Total spikes generated
    size_t activeNeuronCount;        // Number of active neurons
    float averageFiringRate;         // Average firing rate (Hz)
    float maxFiringRate;             // Maximum firing rate (Hz)
    
    // Memory system metrics
    size_t workingMemoryActiveTraces; // Active working memory traces
    size_t episodicMemoryEpisodeCount; // Number of episodic memory episodes
    float memoryUtilization;         // Memory system utilization (0-1)
    
    // Plasticity metrics
    float totalPlasticityWeightChange; // Total synaptic weight change
    size_t plasticityEventsCount;     // Number of plasticity events
    float plasticityEfficiency;      // Plasticity learning efficiency
    
    // System resource metrics
    size_t memoryUsageMB;            // Memory usage in MB
    double cpuUsagePercent;          // CPU usage percentage
    
    // Integration metrics
    float integrationScore;          // Overall system integration score
    double timeStep;                 // Time step size
    
    // Performance profiling
    std::vector<double> stepTimes;   // Time per simulation step
    std::vector<double> spikeTimes;  // Times of spike events
    std::vector<size_t> neuronActivity; // Neuron activity counts
    
    PerformanceMetrics()
        : simulationTime(0.0)
        , wallClockTime(0.0)
        , cpuTime(0.0)
        , totalSpikeCount(0)
        , activeNeuronCount(0)
        , averageFiringRate(0.0f)
        , maxFiringRate(0.0f)
        , workingMemoryActiveTraces(0)
        , episodicMemoryEpisodeCount(0)
        , memoryUtilization(0.0f)
        , totalPlasticityWeightChange(0.0f)
        , plasticityEventsCount(0)
        , plasticityEfficiency(0.0f)
        , memoryUsageMB(0)
        , cpuUsagePercent(0.0)
        , integrationScore(0.0f)
        , timeStep(0.0) {}
};

// Performance Sample Structure
struct PerformanceSample {
    SimulationStep step;            // Simulation step number
    Timestamp time;                  // Simulation time
    double wallClockTime;            // Wall clock time at this step
    size_t totalSpikes;             // Total spikes up to this step
    size_t firingNeurons;            // Number of firing neurons
    float firingRate;                // Current firing rate
    float memoryUsage;               // Memory usage
    double cpuUsage;                 // CPU usage percentage
    
    PerformanceSample(SimulationStep s, Timestamp t, double wct, 
                     size_t ts, size_t fn, float fr, float mu, double cu)
        : step(s), time(t), wallClockTime(wct), totalSpikes(ts), 
          firingNeurons(fn), firingRate(fr), memoryUsage(mu), cpuUsage(cu) {}
};

// Performance Benchmark Configuration
struct PerformanceBenchmarkConfig {
    size_t maxSteps;                 // Maximum simulation steps
    size_t samplingInterval;         // Sampling interval (steps)
    bool enableRealTimeMonitoring;   // Enable real-time monitoring
    bool enableDetailedProfiling;    // Enable detailed profiling
    size_t performanceHistorySize;   // Size of performance history buffer
    double cpuUsageSampleInterval;   // CPU usage sampling interval
    std::string outputFile;          // Output file for performance data
    
    PerformanceBenchmarkConfig()
        : maxSteps(10000)
        , samplingInterval(100)
        , enableRealTimeMonitoring(true)
        , enableDetailedProfiling(false)
        , performanceHistorySize(1000)
        , cpuUsageSampleInterval(1.0)
        , outputFile("performance_data.csv") {}
};

// Performance Analysis Results
struct PerformanceAnalysis {
    double meanStepTime;             // Mean simulation step time
    double stdStepTime;              // Standard deviation of step time
    double minStepTime;              // Minimum step time
    double maxStepTime;              // Maximum step time
    float meanFiringRate;            // Mean firing rate
    float stdFiringRate;             // Standard deviation of firing rate
    float coefficientOfVariation;    // Coefficient of variation (CV = std/mean)
    double totalSimulationTime;      // Total simulation time
    size_t totalSpikes;             // Total number of spikes
    size_t peakSpikes;               // Peak spike count in any step
    float memoryEfficiency;         // Memory usage efficiency
    double cpuEfficiency;           // CPU efficiency (spikes per CPU cycle)
    
    PerformanceAnalysis()
        : meanStepTime(0.0)
        , stdStepTime(0.0)
        , minStepTime(0.0)
        , maxStepTime(0.0)
        , meanFiringRate(0.0f)
        , stdFiringRate(0.0f)
        , coefficientOfVariation(0.0f)
        , totalSimulationTime(0.0)
        , totalSpikes(0)
        , peakSpikes(0)
        , memoryEfficiency(0.0f)
        , cpuEfficiency(0.0) {}
};

class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Initialize performance monitor
    bool initialize(Brain* brain, const PerformanceBenchmarkConfig& config);
    
    // Update performance metrics
    void update(SimulationStep step, Timestamp time);
    
    // Record performance sample
    void recordSample(SimulationStep step, Timestamp time);
    
    // Get current performance metrics
    const PerformanceMetrics& getCurrentMetrics() const { return currentMetrics; }
    
    // Get performance history
    const std::vector<PerformanceSample>& getPerformanceHistory() const { return performanceHistory; }
    
    // Get performance analysis
    PerformanceAnalysis analyzePerformance() const;
    
    // Get performance summary
    std::string getPerformanceSummary() const;
    
    // Get detailed performance report
    std::string getDetailedReport() const;
    
    // Save performance data to file
    bool savePerformanceData(const std::string& filepath) const;
    
    // Load performance data from file
    bool loadPerformanceData(const std::string& filepath);
    
    // Export performance data as CSV
    bool exportToCSV(const std::string& filepath) const;
    
    // Clear performance history
    void clearHistory();
    
    // Set sampling interval
    void setSamplingInterval(size_t interval) { samplingInterval = interval; }
    
    // Enable/disable real-time monitoring
    void setRealTimeMonitoring(bool enabled) { enableRealTimeMonitoring = enabled; }
    
    // Get performance status
    std::string getStatus() const;
    
    // Reset performance metrics
    void resetMetrics();
    
    // Check if performance monitor is initialized
    bool isInitialized() const { return brain != nullptr; }
    
private:
    Brain* brain;                                    // Pointer to brain being monitored
    PerformanceBenchmarkConfig config;               // Benchmark configuration
    PerformanceMetrics currentMetrics;               // Current performance metrics
    std::vector<PerformanceSample> performanceHistory; // Performance history
    std::chrono::high_resolution_clock::time_point startTime; // Start time
    std::chrono::high_resolution_clock::time_point lastSampleTime; // Last sample time
    size_t samplingInterval;                         // Sampling interval
    bool enableRealTimeMonitoring;                   // Real-time monitoring flag
    size_t stepsSinceLastSample;                      // Steps since last sample
    double totalCpuTime;                             // Total CPU time
    size_t totalMemorySamples;                       // Total memory samples
    
    // Helper methods
    void updateSystemMetrics();
    void updateNeuralMetrics();
    void updateMemoryMetrics();
    void updatePlasticityMetrics();
    void updateIntegrationMetrics();
    void updateResourceMetrics();
    
    double getCurrentCpuUsage() const;
    size_t getCurrentMemoryUsage() const;
    
    double calculateMean(const std::vector<double>& values) const;
    double calculateStd(const std::vector<double>& values, double mean) const;
    double calculateMin(const std::vector<double>& values) const;
    double calculateMax(const std::vector<double>& values) const;
    
    std::string formatTime(double seconds) const;
    std::string formatValue(double value, int precision = 6) const;
};

} // namespace nlm

#endif // NLM_PERFORMANCE_MONITOR_H