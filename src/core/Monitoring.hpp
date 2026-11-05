#pragma once

#include "../core/Logger/Logger.hpp"
#include "../core/Constants.hpp"
#include "../core/Types/Types.hpp"
#include <chrono>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#include <mutex>

namespace nlm {

/**
 * Performance metrics collection for neural simulation
 */
class PerformanceMetrics {
public:
    struct MetricsSnapshot {
        uint64_t step;                    // Current simulation step
        double simulationTime;             // Real elapsed time
        double wallClockTime;              // Wall clock time
        uint64_t neuronCount;             // Number of neurons
        uint64_t synapseCount;            // Number of synapses
        uint64_t spikeCount;              // Total spikes this step
        uint64_t spikeCountTotal;          // Total spikes overall
        float averageFiringRate;          // Average firing rate across network
        float memoryUsageMB;              // Current memory usage
        float cpuUsagePercent;            // Current CPU usage
        uint64_t checkpointCount;         // Number of checkpoints saved
        double lastCheckpointDuration;    // Duration of last checkpoint
        std::chrono::system_clock::time_point timestamp; // Measurement time
    };
    
    PerformanceMetrics();
    ~PerformanceMetrics();
    
    // Update performance metrics for current step
    void updateMetrics(uint64_t step, double simTime, 
                       const std::vector<Neuron*>& neurons,
                       const std::vector<Synapse*>& synapses,
                       uint64_t spikeCount, uint64_t spikeCountTotal);
    
    // Update memory usage
    void updateMemoryUsage(double memoryMB);
    
    // Update CPU usage
    void updateCPUUsage(double cpuPercent);
    
    // Record checkpoint operation
    void recordCheckpoint(bool success, double duration);
    
    // Get latest metrics snapshot
    MetricsSnapshot getLatestSnapshot() const;
    
    // Get metrics history (last N samples)
    std::vector<MetricsSnapshot> getMetricsHistory(size_t maxSamples = 1000) const;
    
    // Calculate performance statistics
    struct PerformanceStats {
        double averageSpikeRate;        // Average spikes per second
        double averageMemoryUsage;     // Average memory usage
        double averageCPUUsage;        // Average CPU usage
        double checkpointSuccessRate;  // Percentage of successful checkpoints
        double avgCheckpointDuration;  // Average checkpoint time
        uint64_t maxSpikesPerStep;     // Maximum spikes in any step
        uint64_t totalSpikes;          // Total spikes throughout simulation
    };
    
    PerformanceStats calculatePerformanceStats() const;
    
    // Reset metrics
    void reset();
    
    // Get singleton instance
    static PerformanceMetrics& getInstance() {
        static PerformanceMetrics instance;
        return instance;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Advanced monitoring infrastructure for neural simulation
 */
class Monitoring {
public:
    struct SystemResources {
        double memoryUsageMB;              // Memory usage in MB
        double memoryUsagePercent;         // Memory usage percentage
        double cpuUsagePercent;            // CPU usage percentage
        size_t activeThreads;              // Number of active threads
        uint64_t fileDescriptors;          // Open file descriptors
        std::chrono::system_clock::time_point uptime; // System uptime
    };
    
    struct SimulationStats {
        uint64_t currentStep;              // Current simulation step
        uint64_t totalSteps;               // Total steps simulated
        uint64_t neuronCount;              // Number of neurons
        uint64_t synapseCount;             // Number of synapses
        uint64_t spikeCount;               // Total spikes
        double simulationTime;             // Simulation time
        double realTimeElapsed;            // Real time elapsed
        float averageFiringRate;          // Average firing rate
        size_t memoryEfficientNeurons;     // Number of memory-efficient neurons
        size_t memoryEfficientSynapses;    // Number of memory-efficient synapses
    };
    
    struct ActivityMonitor {
        struct TimeWindow {
            std::chrono::system_clock::time_point start;
            std::chrono::system_clock::time_point end;
            uint64_t stepCount;              // Steps in window
            uint64_t spikeCount;             // Spikes in window
            double avgFiringRate;            // Average firing rate in window
        };
        
        // Get activity for the last N seconds
        TimeWindow getActivityWindow(uint64_t seconds) const;
        
        // Get spike rate for specific neurons
        std::unordered_map<uint64_t, float> getNeuronSpikeRates(uint64_t windowSeconds = 60) const;
        
        // Get synaptic activity summary
        struct SynapticActivity {
            uint64_t totalConnections;       // Total synaptic connections
            float averageWeight;             // Average synaptic weight
            float weightVariance;            // Synaptic weight variance
            uint64_t activeConnections;      // Number of active connections
        };
        
        SynapticActivity getSynapticActivity() const;
    };
    
    Monitoring();
    ~Monitoring();
    
    // Update system resources
    void updateSystemResources();
    
    // Update simulation statistics
    void updateSimulationStats(uint64_t step, double simTime,
                               const std::vector<Neuron*>& neurons,
                               const std::vector<Synapse*>& synapses,
                               uint64_t spikeCount);
    
    // Get current system resources
    SystemResources getSystemResources() const;
    
    // Get current simulation statistics
    SimulationStats getSimulationStats() const;
    
    // Get activity monitor
    ActivityMonitor getActivityMonitor() const;
    
    // Check for performance anomalies
    struct PerformanceAnomaly {
        enum class Type {
            HighCPUUsage,
            HighMemoryUsage,
            SpikeRateAnomaly,
            ConnectivityAnomaly,
            MemoryLeak,
            NetworkIssue
        };
        
        Type type;                          // Type of anomaly
        std::string description;           // Human-readable description
        double severity;                    // Severity (0.0 - 1.0)
        std::chrono::system_clock::time_point timestamp; // When detected
        std::string recommendation;        // Recommended action
    };
    
    std::vector<PerformanceAnomaly> detectAnomalies() const;
    
    // Get performance alerts
    struct PerformanceAlert {
        enum class Level {
            Info,
            Warning,
            Critical,
            Fatal
        };
        
        Level level;                        // Alert level
        std::string message;                // Alert message
        std::chrono::system_clock::time_point timestamp; // When triggered
        std::string source;                 // Source component
        std::unordered_map<std::string, std::string> metadata; // Additional info
    };
    
    std::vector<PerformanceAlert> getAlerts() const;
    
    // Clear old alerts (older than specified duration)
    void clearOldAlerts(std::chrono::seconds maxAge);
    
    // Get monitoring summary
    std::string getSummary() const;
    
    // Get singleton instance
    static Monitoring& getInstance() {
        static Monitoring instance;
        return instance;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Checkpoint monitoring and performance tracking
 */
class CheckpointMonitor {
public:
    struct CheckpointMetrics {
        std::string name;                   // Checkpoint name
        uint64_t step;                      // Simulation step at checkpoint
        double simulationTime;              // Simulation time
        double realTimeDuration;            // Real time taken
        uint64_t dataSizeMB;                // Data size in MB
        bool success;                       // Whether checkpoint succeeded
        std::chrono::system_clock::time_point timestamp; // When checkpoint was taken
        std::string errorMessage;           // Error message if failed
    };
    
    CheckpointMonitor();
    ~CheckpointMonitor();
    
    // Record checkpoint attempt
    void recordCheckpoint(const std::string& name, uint64_t step, double simTime,
                         double realDuration, uint64_t dataSize, bool success,
                         const std::string& errorMessage = "");
    
    // Get recent checkpoint metrics
    std::vector<CheckpointMetrics> getRecentCheckpoints(size_t maxCount = 100) const;
    
    // Get checkpoint statistics
    struct CheckpointStats {
        uint64_t totalCheckpoints;          // Total checkpoints recorded
        uint64_t successfulCheckpoints;     // Successful checkpoints
        uint64_t failedCheckpoints;         // Failed checkpoints
        double successRate;                 // Success rate (0.0 - 1.0)
        double averageDuration;             // Average checkpoint duration
        double averageDataSize;            // Average data size
        uint64_t maxDataSize;               // Maximum data size
        double minDuration;                 // Minimum duration
        double maxDuration;                 // Maximum duration
    };
    
    CheckpointStats calculateStats() const;
    
    // Get slowest checkpoints
    std::vector<CheckpointMetrics> getSlowestCheckpoints(size_t count = 10) const;
    
    // Get largest checkpoints
    std::vector<CheckpointMetrics> getLargestCheckpoints(size_t count = 10) const;
    
    // Get failed checkpoints
    std::vector<CheckpointMetrics> getFailedCheckpoints(size_t maxCount = 50) const;
    
    // Analyze checkpoint performance trends
    struct CheckpointTrend {
        double durationTrend;               // Trend in checkpoint duration (slope)
        double sizeTrend;                   // Trend in data size (slope)
        double successTrend;                // Trend in success rate (slope)
        std::string trendDirection;         // Overall trend direction
    };
    
    CheckpointTrend analyzeTrends(size_t analysisWindow = 50) const;
    
    // Clear old metrics
    void clearOldMetrics(std::chrono::hours maxAge = std::chrono::hours(24));
    
    // Get singleton instance
    static CheckpointMonitor& getInstance() {
        static CheckpointMonitor instance;
        return instance;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Real-time monitoring dashboard
 */
class MonitoringDashboard {
public:
    MonitoringDashboard();
    ~MonitoringDashboard();
    
    // Update all monitoring data
    void updateAll();
    
    // Get monitoring data as JSON string
    std::string getJSONReport() const;
    
    // Get monitoring data as CSV string
    std::string getCSVReport() const;
    
    // Get monitoring data as text summary
    std::string getTextSummary() const;
    
    // Generate health report
    std::string generateHealthReport() const;
    
    // Check system health
    struct SystemHealth {
        bool isHealthy;                     // Overall health status
        double healthScore;                 // Health score (0.0 - 1.0)
        std::vector<std::string> issues;   // List of issues (empty if healthy)
        std::vector<std::string> warnings; // List of warnings
        std::chrono::system_clock::time_point lastCheck; // When last checked
    };
    
    SystemHealth checkSystemHealth() const;
    
    // Get singleton instance
    static MonitoringDashboard& getInstance() {
        static MonitoringDashboard instance;
        return instance;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Monitoring utilities
 */
class MonitoringUtils {
public:
    // Format bytes to human-readable string
    static std::string formatBytes(uint64_t bytes);
    
    // Format duration to human-readable string
    static std::string formatDuration(std::chrono::duration<double> duration);
    
    // Format timestamp to string
    static std::string formatTimestamp(std::chrono::system_clock::time_point timestamp);
    
    // Calculate percentage
    static double calculatePercentage(double value, double total);
    
    // Calculate trend slope
    static double calculateTrend(const std::vector<double>& values);
    
    // Smooth values with moving average
    static std::vector<double> movingAverage(const std::vector<double>& values, size_t window);
    
    // Detect anomalies using statistical methods
    static std::vector<size_t> detectOutliers(const std::vector<double>& values, double threshold = 2.0);
    
    // Get system information
    static std::string getSystemInfo();
    
    // Get current memory usage
    static double getCurrentMemoryUsage();
    
    // Get current CPU usage
    static double getCurrentCPUUsage();
    
private:
    MonitoringUtils() = delete;
    ~MonitoringUtils() = delete;
    MonitoringUtils(const MonitoringUtils&) = delete;
    MonitoringUtils& operator=(const MonitoringUtils&) = delete;
};

} // namespace nlm