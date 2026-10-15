#pragma once

/**
 * Performance Monitoring System for NLM Benchmarking
 * 
 * Real-time performance monitoring system for collecting and analyzing
 * simulation metrics, system resource usage, and brain state statistics.
 */

#include "BenchmarkSuite.hpp"
#include "brain/Brain.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include <thread>
#include <mutex>
#include <sstream>
#include <iomanip>

namespace nlm {

/**
 * System resource monitor (simplified cross-platform implementation)
 * 
 * Provides basic system resource monitoring capabilities for benchmarking.
 * In a production system, this would use platform-specific APIs (Linux proc,
 * Windows Performance Counters, macOS sysctl, etc.) for accurate measurements.
 */
class SystemResourceMonitor {
public:
    SystemResourceMonitor();
    ~SystemResourceMonitor();
    
    // System state update
    void update();
    
    // Resource measurements
    double getCpuUsage() const { return cpuUsage_; }
    double getMemoryUsage() const { return memoryUsage_; }
    double getMemoryUsageMB() const { return memoryUsage_ / (1024 * 1024); }
    size_t getProcessMemoryKB() const { return processMemoryKB_; }
    size_t getPeakMemoryKB() const { return peakMemoryKB_; }
    
    // Time measurements
    double getUptime() const { return uptime_; }
    double getSystemTime() const { return systemTime_; }
    
    // Threading
    int getCpuCount() const { return cpuCount_; }
    int getProcessThreadCount() const { return processThreadCount_; }
    
    // File descriptors
    size_t getOpenFileDescriptors() const { return openFds_; }
    
    // Network (simplified)
    uint64_t getNetworkBytesReceived() const { return networkBytesReceived_; }
    uint64_t getNetworkBytesSent() const { return networkBytesSent_; }
    
    // Get formatted resource report
    std::string getResourceReport() const;
    
private:
    mutable std::mutex monitorMutex_;
    
    // CPU metrics
    double cpuUsage_;
    int cpuCount_;
    double cpuFrequencyMHz_;
    std::vector<double> cpuCoreUsages_;
    
    // Memory metrics
    double memoryUsage_;          // Total system memory (bytes)
    double memoryAvailable_;      // Available system memory (bytes)
    size_t processMemoryKB_;      // Current process memory (KB)
    size_t peakMemoryKB_;         // Peak process memory (KB)
    size_t virtualMemorySize_;    // Virtual memory size (bytes)
    
    // Time metrics
    double uptime_;               // System uptime (seconds)
    double systemTime_;           // Current system time (seconds)
    std::chrono::high_resolution_clock::time_point lastUpdateTime_;
    
    // Threading metrics
    int processThreadCount_;
    
    // I/O metrics
    size_t openFds_;              // Open file descriptors
    uint64_t networkBytesReceived_;
    uint64_t networkBytesSent_;
    
    // Update methods (platform-specific)
    void updateCpuMetrics();
    void updateMemoryMetrics();
    void updateProcessMetrics();
    void updateTimeMetrics();
    void updateNetworkMetrics();
    void updateSystemSpecificMetrics();
};

/**
 * Brain state monitor for neural activity tracking
 */
class BrainStateMonitor {
public:
    BrainStateMonitor();
    ~BrainStateMonitor();
    
    // Configuration
    void setUpdateInterval(double interval) { updateInterval_ = interval; }
    void setBrain(std::shared_ptr<Brain> brain) { brain_ = brain; }
    
    // Monitoring operations
    void update();
    bool isMonitoring() const { return monitoring_; }
    void startMonitoring();
    void stopMonitoring();
    
    // Brain metrics collection
    void recordNeuronMetrics();
    void recordSynapseMetrics();
    void recordSpikeMetrics();
    void recordPlasticityMetrics();
    void recordMemoryMetrics();
    void recordDevelopmentMetrics();
    
    // Get metrics
    double getAverageFiringRate() const { return averageFiringRate_; }
    size_t getActiveNeuronCount() const { return activeNeuronCount_; }
    size_t getTotalSpikeCount() const { return totalSpikeCount_; }
    size_t getPeakActiveNeurons() const { return peakActiveNeurons_; }
    double getExcitatoryInhibitionRatio() const { return eiRatio_; }
    
    // Temporal metrics
    double getMinStepTime() const { return minStepTime_; }
    double getMaxStepTime() const { return maxStepTime_; }
    double getMedianStepTime() const { return medianStepTime_; }
    double getStepTimeCoefficientOfVariation() const { return stepTimeCV_; }
    
    // Memory metrics
    size_t getMemoryPoolAllocated() const { return memoryPoolAllocated_; }
    size_t getMemoryPoolAvailable() const { return memoryPoolAvailable_; }
    size_t getEventQueueSize() const { return eventQueueSize_; }
    double getMemoryFragmentation() const { return memoryFragmentation_; }
    
    // Get monitoring data
    std::unordered_map<std::string, double> getBrainMetrics() const;
    std::string getDetailedBrainReport() const;
    std::string getTemporalMetricsReport() const;
    
private:
    bool monitoring_;
    double updateInterval_;
    double lastUpdateTime_;
    std::shared_ptr<Brain> brain_;
    mutable std::mutex monitorMutex_;
    
    // Neural activity metrics
    double averageFiringRate_;
    size_t activeNeuronCount_;
    size_t peakActiveNeurons_;
    size_t totalSpikeCount_;
    double eiRatio_;
    
    // Temporal metrics
    double minStepTime_;
    double maxStepTime_;
    double medianStepTime_;
    double stepTimeCV_;
    std::vector<double> stepTimes_;
    
    // Memory metrics
    size_t memoryPoolAllocated_;
    size_t memoryPoolAvailable_;
    size_t eventQueueSize_;
    double memoryFragmentation_;
    
    // Update methods
    void updateNeuronMetrics();
    void updateSynapseMetrics();
    void updateSpikeMetrics();
    void updateTemporalMetrics();
    void updateMemoryMetrics();
    void updateNeuromodulationMetrics();
};

/**
 * Performance Monitor - Main monitoring interface
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Configuration
    void configure(bool monitorMemory = true, bool monitorCpu = true, 
                   bool monitorEvents = true, bool monitorFragmentation = true);
    void setBrain(std::shared_ptr<Brain> brain) { brainMonitor_.setBrain(brain); }
    
    // Monitoring control
    void start();
    void stop();
    bool isRunning() const { return running_; }
    bool isSystemMonitoring() const { return systemMonitor_.isRunning(); }
    
    // Data collection
    void update();
    
    // Record specific metrics
    void recordMetric(const std::string& name, double value);
    void recordTime(const std::string& operation, double duration);
    void recordBrainMetrics();
    void recordMemoryMetrics();
    void recordSystemMetrics();
    
    // Get metrics
    double getMetric(const std::string& name) const;
    std::vector<std::pair<std::string, double>> getAllMetrics() const;
    std::unordered_map<std::string, double> getSystemMetrics() const;
    std::unordered_map<std::string, double> getBrainMetrics() const;
    std::unordered_map<std::string, double> getTemporalMetrics() const;
    
    // Reports
    std::string getSystemResourceReport() const;
    std::string getBrainStateReport() const;
    std::string getTemporalPerformanceReport() const;
    std::string getDetailedReport() const;
    std::string getSummaryReport() const;
    
    // Memory tracking
    void recordMemoryPoolStats(const MemoryPool<NeuronId>* pool, const std::string& name);
    void recordMemoryPoolStats(const MemoryPool<SynapseId>* pool, const std::string& name);
    void recordEventQueueStats(const SpikeRingBuffer& queue);
    
    // Time measurements
    class Timer {
    public:
        Timer(PerformanceMonitor& monitor, const std::string& operation);
        ~Timer();
        
        void stop();
        double getDuration() const { return duration_; }
    
    private:
        PerformanceMonitor& monitor_;
        std::string operation_;
        std::chrono::high_resolution_clock::time_point startTime_;
        double duration_;
        bool stopped_;
    };
    
    Timer measure(const std::string& operation);
    
private:
    bool running_;
    std::unique_ptr<SystemResourceMonitor> systemMonitor_;
    std::unique_ptr<BrainStateMonitor> brainMonitor_;
    std::unique_ptr<SystemResourceMonitor> resourceMonitor_;
    
    mutable std::mutex monitorMutex_;
    double startTime_;
    double lastUpdateTime_;
    
    std::unordered_map<std::string, double> metrics_;
    std::unordered_map<std::string, std::vector<double>> timeMetrics_;
    
    // Update methods
    void updateSystemMetrics();
    void updateBrainMetrics();
    void updateTemporalMetrics();
    void aggregateMetrics();
    
    // Resource monitoring
    void monitorCpuUsage();
    void monitorMemoryUsage();
    void monitorSystemEvents();
}; // class PerformanceMonitor

} // namespace nlm