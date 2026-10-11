#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <iomanip>
#include <sstream>
#include <nlohmann/json.hpp>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Forward declarations
class Brain;
class DebugConsole;

// Performance metrics structure
struct PerformanceMetrics {
    // Timing metrics
    struct TimingMetrics {
        double totalSimulationTime;      // Total simulation time
        double lastFrameTime;           // Time for last frame
        double averageFrameTime;        // Average frame time
        double minFrameTime;            // Minimum frame time
        double maxFrameTime;            // Maximum frame time
        double fps;                     // Frames per second
        double simulationHz;            // Simulation steps per second
        
        TimingMetrics() : totalSimulationTime(0.0), lastFrameTime(0.0), averageFrameTime(0.0),
                         minFrameTime(1e9), maxFrameTime(0.0), fps(0.0), simulationHz(0.0) {}
    } timing;
    
    // Memory metrics
    struct MemoryMetrics {
        size_t totalMemoryUsage;        // Total memory usage in bytes
        size_t peakMemoryUsage;         // Peak memory usage in bytes
        size_t currentMemoryUsage;      // Current memory usage in bytes
        size_t memoryAllocated;         // Total memory allocated
        size_t memoryFreed;             // Total memory freed
        size_t memoryLeaks;              // Estimated memory leaks
        double memoryGrowthRate;        // Memory growth rate (bytes per second)
        
        // Component-specific memory usage
        std::map<std::string, size_t> componentMemoryUsage;
        std::map<std::string, size_t> peakComponentMemoryUsage;
        
        MemoryMetrics() : totalMemoryUsage(0), peakMemoryUsage(0), currentMemoryUsage(0),
                         memoryAllocated(0), memoryFreed(0), memoryLeaks(0), memoryGrowthRate(0.0) {}
    } memory;
    
    // CPU metrics
    struct CPUMetrics {
        std::vector<double> cpuUsageHistory;        // CPU usage history
        double averageCpuUsage;                     // Average CPU usage (%)
        double peakCpuUsage;                        // Peak CPU usage (%)
        size_t activeThreads;                       // Number of active threads
        std::map<std::string, double> threadCPUTime; // CPU time per thread
        
        CPUMetrics() : averageCpuUsage(0.0), peakCpuUsage(0.0), activeThreads(0) {}
    } cpu;
    
    // Network metrics (for distributed debugging)
    struct NetworkMetrics {
        size_t bytesSent;                // Total bytes sent
        size_t bytesReceived;            // Total bytes received
        size_t packetsSent;               // Total packets sent
        size_t packetsReceived;           // Total packets received
        double networkUtilization;       // Network utilization (%)
        double packetsLost;              // Percentage of packets lost
        
        NetworkMetrics() : bytesSent(0), bytesReceived(0), packetsSent(0), packetsReceived(0),
                          networkUtilization(0.0), packetsLost(0.0) {}
    } network;
    
    // Neural computation metrics
    struct NeuralComputationMetrics {
        size_t neuronsProcessed;          // Number of neurons processed per step
        size_t synapsesProcessed;         // Number of synapses processed per step
        size_t spikesDetected;            // Number of spikes detected per step
        double averageNeuronFiringRate;   // Average neuron firing rate
        double stddevNeuronFiringRate;   // Standard deviation of firing rates
        double connectionUpdateTime;    // Time to update connections
        double plasticityUpdateTime;     // Time to update plasticity
        double memoryAccessTime;         // Time spent on memory access
        
        // Per-component computation times
        std::map<std::string, double> componentComputationTimes;
        
        NeuralComputationMetrics() : neuronsProcessed(0), synapsesProcessed(0), spikesDetected(0),
                                    averageNeuronFiringRate(0.0), stddevNeuronFiringRate(0.0),
                                    connectionUpdateTime(0.0), plasticityUpdateTime(0.0), memoryAccessTime(0.0) {}
    } neural;
    
    // System resource metrics
    struct SystemResourceMetrics {
        double cpuTemperature;            // CPU temperature
        double gpuTemperature;            // GPU temperature (if applicable)
        double systemTemperature;        // Overall system temperature
        size_t diskUsage;                 // Disk usage in bytes
        double diskUsagePercent;         // Disk usage (%)
        size_t swapUsage;                 // Swap usage in bytes
        double swapUsagePercent;         // Swap usage (%)
        std::chrono::system_clock::time_point uptime; // System uptime
        
        SystemResourceMetrics() : cpuTemperature(0.0), gpuTemperature(0.0), systemTemperature(0.0),
                                 diskUsage(0), diskUsagePercent(0.0), swapUsage(0), swapUsagePercent(0.0) {}
    } system;
    
    // Simulation-specific metrics
    struct SimulationMetrics {
        SimulationStep currentStep;       // Current simulation step
        double currentTime;               // Current simulation time
        SimulationStep totalSteps;        // Total steps simulated
        double simulationDuration;        // Total simulation duration
        double stepsPerSecond;            // Simulation speed
        size_t totalNeurons;              // Total number of neurons
        size_t totalSynapses;             // Total number of synapses
        size_t activeNeurons;             // Number of active neurons
        size_t firingNeurons;             // Number of firing neurons
        float averageFiringRate;          // Average firing rate
        float excitationInhibitionRatio; // E/I ratio
        
        // Event counters
        size_t sensoryInputsProcessed;   // Number of sensory inputs processed
        size_t actionsProduced;          // Number of actions produced
        size_t episodesStored;           // Number of episodes stored
        size_t memoryConsolidations;     // Number of memory consolidations
        size_t rewiringEvents;           // Number of structural rewiring events
        
        SimulationMetrics() : currentStep(0), currentTime(0.0), totalSteps(0),
                             simulationDuration(0.0), stepsPerSecond(0.0), totalNeurons(0),
                             totalSynapses(0), activeNeurons(0), firingNeurons(0),
                             averageFiringRate(0.0), excitationInhibitionRatio(0.0),
                             sensoryInputsProcessed(0), actionsProduced(0), episodesStored(0),
                             memoryConsolidations(0), rewiringEvents(0) {}
    } simulation;
    
    // Profiling statistics
    struct ProfilingStats {
        uint64_t totalProfileSamples;     // Total number of profile samples
        uint64_t samplesPerFrame;        // Number of samples per frame
        double profilingOverhead;        // Overhead of profiling itself
        double accuracy;                 // Profiling accuracy (%)
        
        // Hotspot detection
        std::vector<std::pair<std::string, double>> hotspots;  // Hotspot locations and times
        std::vector<std::pair<std::string, double>> bottlenecks;  // Bottleneck locations and times
        
        ProfilingStats() : totalProfileSamples(0), samplesPerFrame(0), profilingOverhead(0.0), accuracy(0.0) {}
    } profiling;
    
    // Timestamp
    std::chrono::system_clock::time_point timestamp;
    
    PerformanceMetrics() : timestamp(std::chrono::system_clock::now()) {}
};

// Performance analysis result
struct PerformanceAnalysis {
    bool valid;                         // Whether analysis is valid
    std::string summary;                 // Summary of analysis
    std::vector<std::string> warnings;   // Performance warnings
    std::vector<std::string> errors;     // Performance errors
    std::vector<std::string> suggestions; // Optimization suggestions
    
    // Bottleneck information
    std::vector<std::pair<std::string, double>> bottlenecks;  // Bottleneck and time contribution
    std::vector<std::pair<std::string, double>> hotspots;     // Hotspot and time contribution
    
    // Performance trend
    double trendOverTime;               // Performance trend (-1 to 1)
    double performanceScore;            // Overall performance score (0 to 100)
    
    // Resource efficiency
    double cpuEfficiency;               // CPU efficiency (%)
    double memoryEfficiency;            // Memory efficiency (%)
    double computationalEfficiency;    // Computational efficiency (%)
    
    PerformanceAnalysis() : valid(false), trendOverTime(0.0), performanceScore(0.0),
                           cpuEfficiency(0.0), memoryEfficiency(0.0), computationalEfficiency(0.0) {}
};

// Performance profiler configuration
struct PerformanceProfilerConfig {
    bool enabled = true;
    bool autoProfile = true;
    double profileInterval = 0.1;       // Profile interval in seconds
    bool enableCPUProfiling = true;
    bool enableMemoryProfiling = true;
    bool enableNeuralProfiling = true;
    bool enableSystemProfiling = true;
    bool enableNetworkProfiling = false;
    size_t profileSampleSize = 100;     // Number of samples to collect
    double maxMemoryUsage = 1024.0 * 1024.0 * 1024.0; // 1GB
    double maxCpuUsage = 100.0;         // 100%
    bool enableAnomalyDetection = true;
    double anomalyThreshold = 2.0;     // Standard deviations
    bool enablePerformanceWarnings = true;
    std::chrono::duration<double> warningThresholds; // Time thresholds for warnings
    std::chrono::duration<double> errorThresholds;   // Time thresholds for errors
    bool enableReportGeneration = true;
    std::string reportDirectory = "./profiles";
    std::string reportFormat = "json";  // "json", "csv", "html"
    bool enableProgressReporting = true;
    std::chrono::duration<double> progressInterval = std::chrono::seconds(1);
};

// Performance alert
struct PerformanceAlert {
    std::chrono::system_clock::time_point timestamp;
    std::string type;                   // "memory", "cpu", "neural", "system"
    std::string severity;               // "info", "warning", "error", "critical"
    std::string message;                 // Alert message
    double currentValue;                 // Current value
    double threshold;                    // Threshold that was exceeded
    std::map<std::string, std::string> metadata; // Additional metadata
    
    PerformanceAlert(const std::string& t, const std::string& s, const std::string& msg,
                     double current, double threshold)
        : timestamp(std::chrono::system_clock::now()), type(t), severity(s),
          message(msg), currentValue(current), threshold(threshold) {}
};

// Performance profiler class
class PerformanceProfiler {
public:
    PerformanceProfiler();
    PerformanceProfiler(const PerformanceProfilerConfig& config);
    ~PerformanceProfiler();
    
    // Disable copying
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    
    // Initialize profiler
    bool initialize();
    
    // Start/stop profiling
    void start();
    void stop();
    bool isActive() const;
    
    // Update profiling data
    void update(const Brain* brain, double deltaTime);
    
    // Get current metrics
    PerformanceMetrics getCurrentMetrics() const;
    
    // Get historical metrics
    std::vector<PerformanceMetrics> getHistoricalMetrics(size_t count = 100) const;
    
    // Get recent metrics (last N seconds)
    std::vector<PerformanceMetrics> getRecentMetrics(double seconds = 60.0) const;
    
    // Performance analysis
    PerformanceAnalysis analyzePerformance(const std::vector<PerformanceMetrics>& metrics) const;
    PerformanceAnalysis getCurrentAnalysis() const;
    
    // Alerts management
    void addAlert(const PerformanceAlert& alert);
    void clearAlerts();
    const std::vector<PerformanceAlert>& getAlerts() const;
    
    // Configuration
    void updateConfig(const PerformanceProfilerConfig& config);
    const PerformanceProfilerConfig& getConfig() const;
    
    // Progress reporting
    void setProgressCallback(std::function<void(double, const std::string&)> callback);
    void clearProgressCallback();
    
    // File operations
    bool saveProfile(const std::string& filepath, const std::string& format = "json");
    bool loadProfile(const std::string& filepath);
    
    // Profile management
    void clear();
    size_t getSampleCount() const;
    double getProfileDuration() const;
    
    // Performance optimization
    std::vector<std::string> getOptimizationRecommendations() const;
    std::vector<std::string> getBottleneckAnalysis() const;
    
    // Integration
    void setDebugConsole(DebugConsole* console);
    
    // Check if profiler is collecting data
    bool isCollecting() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
