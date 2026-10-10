#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../environment/Environment.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../experiments/ScalingBenchmark.hpp"
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <set>
#include <functional>
#include <optional>
#include <fstream>
#include <numeric>

namespace nlm {

/**
 * Comprehensive Performance Monitoring System for NLM
 * 
 * Phase 6: Advanced Performance Monitoring
 * 
 * Provides detailed performance metrics throughout simulation execution:
 * - Simulation performance metrics (CPU, memory, timing, throughput)
 * - Neural performance metrics (firing rates, synapse operations, plasticity)
 * - Resource utilization monitoring (memory, CPU, I/O, network, cache)
 * - Bottleneck analysis and optimization opportunities
 * - Real-time monitoring and live dashboards
 * - Comparative performance analysis
 * 
 * Integrates seamlessly with existing NLM architecture and complements
 * DebugAnalysis system for comprehensive system analysis.
 */

class PerformanceMonitor {
public:
    /**
     * Performance monitoring flags configuration
     */
    struct MonitorFlags {
        // Core monitoring domains
        bool cpuMonitoring = false;              // CPU usage tracking
        bool memoryMonitoring = false;           // Memory usage monitoring  
        bool simulationMonitoring = false;       // Simulation performance analysis
        bool neuralMonitoring = false;           // Neural performance metrics
        bool resourceMonitoring = false;         // Resource utilization monitoring
        bool bottleneckMonitoring = false;       // Bottleneck analysis
        bool benchmarkMonitoring = false;        // Benchmark mode integration
        bool liveMonitoring = false;             // Real-time monitoring
        bool trendAnalysis = false;              // Performance trend analysis
        bool comparativeAnalysis = false;        // Comparative performance analysis
        
        // Sub-domain flags
        bool stepTiming = false;                 // Step timing analysis
        bool frameRate = false;                  // Frame rate calculation
        bool firingRateAnalysis = false;         // Neuron firing rate analysis
        bool synapseProfiling = false;           // Synapse operation profiling
        bool plasticityTiming = false;           // Plasticity computation timing
        bool memoryPoolMonitoring = false;       // Memory pool efficiency
        bool cacheEfficiency = false;            // Cache hit rate tracking
        bool iomonitoring = false;               // I/O performance monitoring
        bool networkMonitoring = false;          // Network bandwidth usage
        bool threadUtilization = false;          // CPU core usage distribution
        bool allocationTracking = false;         // Memory allocation tracking
        
        MonitorFlags() = default;
        MonitorFlags(const std::map<std::string, std::string>& args);
    };

    /**
     * Comprehensive performance metrics structure
     */
    struct PerformanceMetrics {
        // Timestamp for this metrics collection
        std::chrono::high_resolution_clock::time_point timestamp;
        double simulationTime;                   // Total simulation time (seconds)
        
        // System performance metrics
        struct SystemPerformance {
            double cpuUsage;                       // Overall CPU usage (%)
            double cpuUserTime;                    // CPU user time (%)
            double cpuSystemTime;                  // CPU system time (%)
            double cpuIdleTime;                    // CPU idle time (%)
            int64_t cpuCores;                      // Number of CPU cores
            double memoryUsageMB;                  // Current memory usage (MB)
            double memoryAvailableMB;              // Available memory (MB)
            double memoryUsagePercent;             // Memory usage (%)
            size_t memoryAllocationCount;          // Number of memory allocations
            size_t memoryDeallocationCount;         // Number of memory deallocations
            double memoryAllocationRate;           // Allocations per second
            double simulationSpeed;                // Steps per second
            double frameRate;                      // Frames per second (if applicable)
            double stepTimeAverageMs;              // Average step time (ms)
            double stepTimeMinMs;                  // Minimum step time (ms)
            double stepTimeMaxMs;                  // Maximum step time (ms)
            double stepTimeStdDevMs;               // Standard deviation of step times (ms)
            double iops;                           // Input/output operations per second
            double networkThroughputMbps;          // Network throughput (Mbps)
        } system;

        // Neural performance metrics
        struct NeuralPerformance {
            double averageFiringRateHz;            // Average firing rate (Hz)
            double maxFiringRateHz;                // Maximum firing rate (Hz)
            double minFiringRateHz;                // Minimum firing rate (Hz)
            double firingRateStdDevHz;             // Firing rate standard deviation
            size_t totalSpikesGenerated;           // Total spikes generated
            size_t totalSpikesReceived;            // Total spikes received
            double spikeFrequency;                 // Spike frequency (Hz)
            size_t totalSynapseOperations;         // Total synaptic operations
            double synapseOperationsPerSecond;     // Synapse operations per second
            double avgSynapticWeightChange;        // Average synaptic weight change
            double plasticityTimeMs;               // Plasticity computation time (ms)
            double stdpTimeMs;                     // STDP computation time (ms)
            double hebbianTimeMs;                  // Hebbian computation time (ms)
            double structuralPlasticityTimeMs;     // Structural plasticity time (ms)
            size_t activeNeurons;                  // Number of active neurons
            size_t firingNeurons;                   // Number of firing neurons
            double networkThroughput;              // Neural network throughput
            double networkDiameter;                // Network diameter (shortest path)
            double clusteringCoefficient;          // Network clustering coefficient
            double efficiency;                     // Network efficiency metric
        } neural;

        // Resource utilization metrics
        struct ResourceUtilization {
            double cpuCoreUsagePercent[64];        // CPU usage per core (max 64 cores)
            double memoryPoolEfficiency;           // Memory pool efficiency (%)
            double cacheHitRate;                   // Cache hit rate (%)
            double cacheMissRate;                  // Cache miss rate (%)
            size_t cacheLineFlushes;               // Number of cache line flushes
            double iowaitPercent;                  // I/O wait percentage (%)
            double diskReadMbps;                   // Disk read throughput (Mbps)
            double diskWriteMbps;                  // Disk write throughput (Mbps)
            size_t fileDescriptorsOpen;            // Number of open file descriptors
            size_t threadsActive;                  // Number of active threads
            double threadCpuTime;                  // Total thread CPU time
            double contextSwitches;                // Number of context switches
        } resources;

        // Bottleneck analysis metrics
        struct BottleneckAnalysis {
            std::vector<std::string> identifiedBottlenecks;  // List of identified bottlenecks
            std::map<std::string, double> bottleneckSeverity;  // Severity of each bottleneck
            std::vector<std::string> optimizationOpportunities;  // List of optimization opportunities
            std::map<std::string, double> optimizationPotential;  // Potential improvement for each optimization
            bool scalingIssues;                           // Indicates scaling problems
            std::vector<double> efficiencyAtScales;       // Efficiency at different scales
            std::map<std::string, double> resourceContention;  // Resource contention metrics
        } bottlenecks;

        // Scaling analysis metrics
        struct ScalingAnalysis {
            bool linearScaling;                          // Whether scaling is linear
            std::vector<double> efficiencyAtScales;      // Efficiency at different scales
            double parallelEfficiency;                   // Parallel efficiency (%)
            std::vector<double> strongScalingEfficiency; // Strong scaling efficiency at different core counts
            std::vector<double> weakScalingEfficiency;   // Weak scaling efficiency at different core counts
        } scaling;

        // Benchmarking metrics
        struct BenchmarkMetrics {
            std::string benchmarkType;                   // Type of benchmark
            double benchmarkScore;                       // Overall benchmark score
            std::map<std, double> componentScores;       // Component benchmark scores
            std::map<std::string, double> targetMetrics; // Target metrics comparison
            bool meetsTargets;                           // Whether targets were met
            std::vector<double> performanceHistory;     // Performance history for this benchmark
        } benchmark;
    };

    /**
     * Real-time monitoring configuration
     */
    struct RealTimeMonitoringConfig {
        double updateIntervalMs;                      // Update interval (milliseconds)
        size_t historySize;                           // Size of metrics history
        bool enableLiveDashboard;                     // Enable live dashboard
        std::string outputFormat;                     // Output format (json, csv, console)
        std::string outputFile;                       // Output file path
        bool consoleOutput;                           // Output to console
        bool fileOutput;                              // Output to file
        bool networkOutput;                           // Output over network
        std::string networkEndpoint;                  // Network endpoint for output
    };

    /**
     * Performance monitor constructor
     */
    PerformanceMonitor(
        const std::shared_ptr<Brain>& brain,
        const std::shared_ptr<Config>& config,
        const MonitorFlags& flags = MonitorFlags()
    );

    /**
     * Destructor
     */
    ~PerformanceMonitor();

    // Disable copying, enable moving
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    PerformanceMonitor(PerformanceMonitor&&) noexcept;
    PerformanceMonitor& operator=(PerformanceMonitor&&) noexcept;

    /**
     * Start performance monitoring
     */
    void start();

    /**
     * Stop performance monitoring
     */
    void stop();

    /**
     * Record a simulation step
     */
    void recordStep(
        double stepTimeMs,
        size_t spikesGenerated,
        size_t activeNeurons,
        double memoryUsageMB,
        double cpuUsage
    );

    /**
     * Record neuron activity metrics
     */
    void recordNeuronMetrics(
        const std::vector<std::shared_ptr<Neuron>>& neurons
    );

    /**
     * Record synapse activity metrics
     */
    void recordSynapseMetrics(
        const std::vector<std::shared_ptr<Synapse>>& synapses
    );

    /**
     * Record resource utilization metrics
     */
    void recordResourceMetrics();

    /**
     * Update bottleneck analysis
     */
    void updateBottleneckAnalysis();

    /**
     * Get current performance metrics
     */
    PerformanceMetrics getCurrentMetrics() const;

    /**
     * Get performance metrics for a time window
     */
    PerformanceMetrics getMetricsForWindow(
        double timeWindowSeconds
    ) const;

    /**
     * Get performance metrics snapshot
     */
    PerformanceMetrics getSnapshot() const;

    /**
     * Get metrics history
     */
    const std::vector<PerformanceMetrics>& getMetricsHistory() const;

    /**
     * Check if monitoring is active
     */
    bool isActive() const { return monitoringActive_.load(); }

    /**
     * Reset performance metrics
     */
    void reset();

    /**
     * Export metrics to JSON format
     */
    std::string exportToJSON() const;

    /**
     * Export metrics to CSV format
     */
    std::string exportToCSV() const;

    /**
     * Generate performance report
     */
    std::string generateReport() const;

    /**
     * Generate performance visualization data
     */
    void generateVisualizationData(const std::string& filepath) const;

    /**
     * Set real-time monitoring configuration
     */
    void setRealTimeConfig(const RealTimeMonitoringConfig& config);

    /**
     * Start live monitoring
     */
    void startLiveMonitoring();

    /**
     * Stop live monitoring
     */
    void stopLiveMonitoring();

    /**
     * Check for performance alerts
     */
    std::vector<std::string> checkPerformanceAlerts() const;

    /**
     * Calculate system efficiency
     */
    double calculateSystemEfficiency() const;

    /**
     * Calculate neural efficiency
     */
    double calculateNeuralEfficiency() const;

    /**
     * Calculate resource efficiency
     */
    double calculateResourceEfficiency() const;

    /**
     * Perform comparative analysis
     */
    void performComparativeAnalysis(const PerformanceMetrics& baselineMetrics);

    /**
     * Get performance recommendations
     */
    std::vector<std::string> getPerformanceRecommendations() const;

private:
    // Internal implementation details
    void initializeMonitoringSystem();
    void updateSystemMetrics();
    void updateNeuralMetrics();
    void updateResourceMetrics();
    void updateBottleneckAnalysis();
    void updateScalingAnalysis();
    void updateBenchmarkMetrics();
    void performRealTimeOutput() const;
    void cleanup();
    
    // Helper functions
    double measureCpuUsage() const;
    double measureMemoryUsage() const;
    size_t measureMemoryAllocationCount() const;
    double measureDiskUsage() const;
    double measureNetworkUsage() const;
    void updateCpuCoreUsage(double* coreUsage) const;
    void updateCacheMetrics() const;
    void updateIoMetrics() const;
    void updateThreadMetrics() const;
    
    // Performance monitoring state
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    MonitorFlags flags_;
    
    // Threading and synchronization
    mutable std::mutex mutex_;
    std::atomic<bool> monitoringActive_;
    std::thread monitoringThread_;
    RealTimeMonitoringConfig realTimeConfig_;
    
    // Metrics collection
    std::vector<PerformanceMetrics> metricsHistory_;
    PerformanceMetrics currentMetrics_;
    std::chrono::high_resolution_clock::time_point startTime_;
    
    // Performance data buffers
    std::vector<double> stepTimes_;
    std::vector<size_t> spikeCounts_;
    std::vector<size_t> activeNeuronCounts_;
    std::vector<double> memoryUsageHistory_;
    std::vector<double> cpuUsageHistory_;
    std::vector<double> speedHistory_;
    
    // Performance monitoring utilities
    class PerformanceDataCollector;
    class PerformanceAnalyzer;
    class PerformanceReporter;
    
    // Performance analysis state
    bool liveMonitoringActive_;
    std::thread liveMonitoringThread_;
    std::atomic<bool> shouldStop_;
    
    // Comparative analysis
    PerformanceMetrics baselineMetrics_;
    bool hasBaseline_;
}; // namespace nlm

/**
 * Convenience function to create performance monitor
 */
inline PerformanceMonitor createPerformanceMonitor(
    const std::shared_ptr<Brain>& brain,
    const std::shared_ptr<Config>& config,
    const PerformanceMonitor::MonitorFlags& flags = PerformanceMonitor::MonitorFlags()
) {
    return PerformanceMonitor(brain, config, flags);
}