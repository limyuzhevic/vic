// ProfilingDataCollector.hpp - Comprehensive Profiling Data Collection
// Collects real-time and detailed performance data for NLM

#pragma once

/**
 * ProfilingDataCollector - Comprehensive data collection for NLM profiling
 * 
 * This module provides advanced data collection capabilities for the
 * Neural Learning Machine, including:
 * 
 * 1. Real-time Performance Monitoring
 *    - Continuous monitoring with configurable sampling rates
 *    - Zero-copy data collection where possible
 *    - Event-driven data collection based on system state
 * 
 * 2. Detailed Timestep-by-Timestep Profiling
 *    - Per-timestep performance metrics
 *    - Component-level timing breakdown
 *    - Spike event tracking
 *    - Resource utilization per timestep
 * 
 * 3. System-wide Performance Statistics
 *    - Global system statistics aggregation
 *    - Time-series data collection
 *    - Statistical analysis of collected data
 *    - Historical performance tracking
 * 
 * 4. Resource Utilization Tracking
 *    - CPU resource utilization
 *    - Memory allocation/deallocation tracking
 *    - Thread usage and contention monitoring
 *    - I/O performance tracking
 * 
 * 5. Error and Exception Profiling
 *    - Exception tracking and analysis
 *    - Assertion failure monitoring
 *    - Memory leak detection
 *    - Race condition detection
 *    
 * Features:
 * - Thread-safe data collection
 * - Configurable collection strategies
 * - Integration with existing NLM modules
 * - Memory-efficient storage
 * - Real-time and post-mortem modes
 * - Custom data sources and hooks
 */

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <atomic>
#include <functional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <thread>
#include <future>
#include <map>
#include <variant>
#include <optional>
#include <numeric>
#include <cmath>
#include <type_traits>
#include <queue>
#include <deque>
#include <set>
#include <regex>
#include <list>

namespace nlm {

/**
 * Profiling data source types
 */
enum class DataSourceType {
    NONE = 0,
    SYSTEM = 1,
    SIMULATION = 2,
    MEMORY = 3,
    THREAD = 4,
    IO = 5,
    CUSTOM = 6
};

/**
 * Data collection configuration
 */
struct DataCollectionConfig {
    std::string name;
    bool enabled = true;
    double samplingIntervalMs = 10.0;  // Sampling interval in milliseconds
    size_t maxDataPoints = 100000;    // Maximum data points to store
    bool enableRealTime = true;       // Enable real-time collection
    bool enablePostMortem = true;     // Enable post-mortem collection
    bool threadSafe = true;           // Thread-safe operation
    size_t bufferSize = 8192;         // Internal buffer size
    std::string compression = "none"; // none, gzip, lz4
    bool deduplicate = true;          // Remove duplicate data points
    
    // Data collection targets
    std::unordered_set<DataSourceType> enabledSources;
    std::unordered_set<std::string> customSources; // Custom source identifiers
    
    // Filtering
    uint64_t minTimestamp = 0;        // Minimum timestamp to collect
    uint64_t maxTimestamp = UINT64_MAX; // Maximum timestamp to collect
    std::unordered_set<ProfileEventType> excludeEventTypes; // Event types to exclude
    
    // Processing
    size_t dataAggregationWindow = 100; // Window for data aggregation
    bool enableStreaming = false;       // Enable streaming output
    std::string streamEndpoint;         // Stream output endpoint
    
    DataCollectionConfig() {
        enabledSources = {
            DataSourceType::SYSTEM,
            DataSourceType::SIMULATION,
            DataSourceType::MEMORY,
            DataSourceType::THREAD,
            DataSourceType::IO
        };
    }
};

/**
 * Data collection result
 */
struct CollectionResult {
    uint64_t totalDataPoints = 0;
    uint64_t dataPointsCollected = 0;
    uint64_t dataPointsDropped = 0;
    uint64_t memoryUsage = 0;
    double collectionRate = 0.0;  // Data points per second
    std::string status;            // "running", "stopped", "paused"
    std::chrono::high_resolution_clock::time_point lastUpdate;
    
    CollectionResult() {
        lastUpdate = std::chrono::high_resolution_clock::now();
    }
    
    double getUptimeSeconds() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate);
        return static_cast<double>(duration.count());
    }
};

/**
 * Custom data collector interface
 */
class CustomDataCollector {
public:
    virtual ~CustomDataCollector() = default;
    
    virtual std::string getCollectorName() const = 0;
    virtual DataSourceType getCollectorType() const = 0;
    
    virtual std::vector<ProfileDataPoint> collectData(uint64_t startTime, uint64_t endTime) = 0;
    virtual std::vector<ProfileDataPoint> collectData(const std::vector<uint64_t>& timestamps) = 0;
    
    virtual bool isAvailable() const = 0;
    virtual std::string getCollectorStatus() const = 0;
    
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    
    virtual std::vector<ProfileDataPoint> getRecentData(size_t maxCount = 0) = 0;
    virtual size_t getCollectedCount() const = 0;
    virtual size_t getDroppedCount() const = 0;
};

/**
 * System data collector
 */
class SystemDataCollector : public CustomDataCollector {
private:
    struct SystemState {
        uint64_t timestamp;
        double cpuUsagePercent;
        size_t memoryUsageBytes;
        size_t availableMemoryBytes;
        size_t activeThreads;
        size_t totalThreads;
        uint64_t diskReads;
        uint64_t diskWrites;
        double networkSendRate;
        double networkReceiveRate;
        double cpuTemperatureCelsius;
        double cpuFrequencyMHz;
        
        SystemState() : timestamp(0), cpuUsagePercent(0.0), memoryUsageBytes(0),
                       availableMemoryBytes(0), activeThreads(0), totalThreads(0),
                       diskReads(0), diskWrites(0), networkSendRate(0.0),
                       networkReceiveRate(0.0), cpuTemperatureCelsius(0.0),
                       cpuFrequencyMHz(0.0) {}
    };
    
    std::string collectorName_;
    bool running_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<SystemState> systemHistory_;
    std::atomic<uint64_t> collectedCount_{0};
    std::atomic<uint64_t> droppedCount_{0};
    mutable std::mutex mutex_;
    
    // System monitoring state
    std::atomic<uint64_t> lastDiskReads_{0};
    std::atomic<uint64_t> lastDiskWrites_{0};
    std::chrono::steady_clock::time_point lastDiskUpdate_;
    
public:
    SystemDataCollector(const std::string& name);
    
    std::string getCollectorName() const override { return collectorName_; }
    DataSourceType getCollectorType() const override { return DataSourceType::SYSTEM; }
    
    std::vector<ProfileDataPoint> collectData(uint64_t startTime, uint64_t endTime) override;
    std::vector<ProfileDataPoint> collectData(const std::vector<uint64_t>& timestamps) override;
    
    bool isAvailable() const override { return true; }
    std::string getCollectorStatus() const override;
    
    void start() override;
    void stop() override;
    void pause() override {} // No pause needed for system monitoring
    void resume() override {} // No resume needed for system monitoring
    
    std::vector<ProfileDataPoint> getRecentData(size_t maxCount = 0) override;
    size_t getCollectedCount() const override { return collectedCount_.load(); }
    size_t getDroppedCount() const override { return droppedCount_.load(); }
    
    // System-specific methods
    void updateDiskIOCounters();
    double calculateDiskThroughput();
    
private:
    void sampleSystemState();
    SystemState getCurrentSystemState() const;
    double getCurrentCPUUsage() const;
    size_t getCurrentMemoryUsage() const;
    size_t getCurrentAvailableMemory() const;
    uint64_t getCurrentDiskReads() const;
    uint64_t getCurrentDiskWrites() const;
};

/**
 * Simulation data collector
 */
class SimulationDataCollector : public CustomDataCollector {
private:
    std::string collectorName_;
    bool running_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<ProfileDataPoint> simulationEvents_;
    std::atomic<uint64_t> collectedCount_{0};
    std::atomic<uint64_t> droppedCount_{0};
    mutable std::mutex mutex_;
    
    // Simulation state
    uint64_t currentTimestep_{0};
    std::unordered_map<std::string, size_t> eventCounters_;
    
public:
    SimulationDataCollector(const std::string& name);
    
    std::string getCollectorName() const override { return collectorName_; }
    DataSourceType getCollectorType() const override { return DataSourceType::SIMULATION; }
    
    std::vector<ProfileDataPoint> collectData(uint64_t startTime, uint64_t endTime) override;
    std::vector<ProfileDataPoint> collectData(const std::vector<uint64_t>& timestamps) override;
    
    bool isAvailable() const override { return true; }
    std::string getCollectorStatus() const override;
    
    void start() override;
    void stop() override;
    void pause() override {} // No pause needed for simulation monitoring
    void resume() override {} // No resume needed for simulation monitoring
    
    std::vector<ProfileDataPoint> getRecentData(size_t maxCount = 0) override;
    size_t getCollectedCount() const override { return collectedCount_.load(); }
    size_t getDroppedCount() const override { return droppedCount_.load(); }
    
    // Simulation-specific methods
    void recordTimestepEvent(uint64_t timestep, double timestepTimeNs, 
                            size_t spikesGenerated, size_t spikesPropagated,
                            size_t activeNeurons, size_t neuronUpdates,
                            size_t synapseUpdates, double memoryDeltaBytes);
    
    void recordNeuronEvent(const std::string& neuronId, double membranePotential,
                          double synapticInput, bool isFiring, uint64_t spikeTime);
    
    void recordSynapseEvent(const std::string& synapseId, size_t sourceId,
                           size_t targetId, double weight, bool isPlastic);
    
    void recordSpikeEvent(const std::string& neuronId, uint64_t spikeTime,
                         double synapticInput);
};

/**
 * Memory data collector
 */
class MemoryDataCollector : public CustomDataCollector {
private:
    std::string collectorName_;
    bool running_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<MemoryAllocationInfo> memoryEvents_;
    std::vector<ProfileDataPoint> memoryAccessEvents_;
    std::atomic<uint64_t> collectedCount_{0};
    std::atomic<uint64_t> droppedCount_{0};
    mutable std::mutex mutex_;
    
    // Memory tracking state
    std::atomic<size_t> peakMemoryUsage_{0};
    std::atomic<size_t> currentMemoryUsage_{0};
    std::atomic<uint64_t> totalAllocations_{0};
    std::atomic<uint64_t> totalDeallocations_{0};
    std::atomic<uint64_t> memoryLeaksDetected_{0};
    
    // Leak detection
    std::unordered_map<void*, MemoryAllocationInfo> liveAllocations_;
    std::chrono::steady_clock::time_point lastLeakCheck_;
    
public:
    MemoryDataCollector(const std::string& name);
    
    std::string getCollectorName() const override { return collectorName_; }
    DataSourceType getCollectorType() const override { return DataSourceType::MEMORY; }
    
    std::vector<ProfileDataPoint> collectData(uint64_t startTime, uint64_t endTime) override;
    std::vector<ProfileDataPoint> collectData(const std::vector<uint64_t>& timestamps) override;
    
    bool isAvailable() const override { return true; }
    std::string getCollectorStatus() const override;
    
    void start() override;
    void stop() override;
    void pause() override {} // No pause needed for memory monitoring
    void resume() override {} // No pause needed for memory monitoring
    
    std::vector<ProfileDataPoint> getRecentData(size_t maxCount = 0) override;
    size_t getCollectedCount() const override { return collectedCount_.load(); }
    size_t getDroppedCount() const override { return droppedCount_.load(); }
    
    // Memory-specific methods
    void recordAllocation(size_t size, const char* file, size_t line,
                         const char* function, bool isPool, uint32_t poolId);
    
    void recordDeallocation(size_t size, const char* file, size_t line,
                           const char* function, bool isPool, uint32_t poolId);
    
    void recordMemoryAccess(void* address, size_t size, bool isWrite,
                           const char* file, size_t line, const char* function);
    
    void detectMemoryLeaks();
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    uint64_t getTotalAllocations() const { return totalAllocations_.load(); }
    uint64_t getTotalDeallocations() const { return totalDeallocations_.load(); }
    uint64_t getMemoryLeaksDetected() const { return memoryLeaksDetected_.load(); }
};

/**
 * Thread data collector
 */
class ThreadDataCollector : public CustomDataCollector {
private:
    std::string collectorName_;
    bool running_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<ThreadPerformanceMetrics> threadEvents_;
    std::vector<ProfileDataPoint> threadAccessEvents_;
    std::atomic<uint64_t> collectedCount_{0};
    std::atomic<uint64_t> droppedCount_{0};
    mutable std::mutex mutex_;
    
    // Thread tracking state
    std::unordered_map<std::string, ThreadPerformanceMetrics> activeThreads_;
    std::atomic<size_t> totalThreads_{0};
    std::chrono::steady_clock::time_point lastThreadUpdate_;
    
public:
    ThreadDataCollector(const std::string& name);
    
    std::string getCollectorName() const override { return collectorName_; }
    DataSourceType getCollectorType() const override { return DataSourceType::THREAD; }
    
    std::vector<ProfileDataPoint> collectData(uint64_t startTime, uint64_t endTime) override;
    std::vector<ProfileDataPoint> collectData(const std::vector<uint64_t>& timestamps) override;
    
    bool isAvailable() const override { return true; }
    std::string getCollectorStatus() const override;
    
    void start() override;
    void stop() override;
    void pause() override {} // No pause needed for thread monitoring
    void resume() override {} // No resume needed for thread monitoring
    
    std::vector<ProfileDataPoint> getRecentData(size_t maxCount = 0) override;
    size_t getCollectedCount() const override { return collectedCount_.load(); }
    size_t getDroppedCount() const override { return droppedCount_.load(); }
    
    // Thread-specific methods
    void recordThreadCreation(const std::string& threadId, size_t stackSize);
    void recordThreadDestruction(const std::string& threadId);
    void recordThreadSwitch(const std::string& fromThread, const std::string& toThread,
                           double waitTimeNs);
    void recordLockAcquisition(const std::string& threadId, const std::string& lockId,
                             double waitTimeNs);
    void recordLockRelease(const std::string& threadId, const std::string& lockId);
    
    void updateThreadMetrics(const std::string& threadId, const ThreadPerformanceMetrics& metrics);
    void detectRaceConditions();
    
    std::vector<ThreadPerformanceMetrics> getActiveThreads() const;
    size_t getTotalThreadsCount() const { return totalThreads_.load(); }
};

/**
 * I/O data collector
 */
class IODataCollector : public CustomDataCollector {
private:
    std::string collectorName_;
    bool running_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<IOMetrics> ioEvents_;
    std::vector<ProfileDataPoint> ioAccessEvents_;
    std::atomic<uint64_t> collectedCount_{0};
    std::atomic<uint64_t> droppedCount_{0};
    mutable std::mutex mutex_;
    
    // I/O tracking state
    std::unordered_map<std::string, IOMetrics> deviceMetrics_;
    std::chrono::steady_clock::time_point lastIOUpdate_;
    
    // File monitoring
    std::unordered_set<std::string> monitoredFiles_;
    std::unordered_map<std::string, uint64_t> fileSizes_;
    
public:
    IODataCollector(const std::string& name);
    
    std::string getCollectorName() const override { return collectorName_; }
    DataSourceType getCollectorType() const override { return DataSourceType::IO; }
    
    std::vector<ProfileDataPoint> collectData(uint64_t startTime, uint64_t endTime) override;
    std::vector<ProfileDataPoint> collectData(const std::vector<uint64_t>& timestamps) override;
    
    bool isAvailable() const override { return true; }
    std::string getCollectorStatus() const override;
    
    void start() override;
    void stop() override;
    void pause() override {} // No pause needed for I/O monitoring
    void resume() override {} // No resume needed for I/O monitoring
    
    std::vector<ProfileDataPoint> getRecentData(size_t maxCount = 0) override;
    size_t getCollectedCount() const override { return collectedCount_.load(); }
    size_t getDroppedCount() const override { return droppedCount_.load(); }
    
    // I/O-specific methods
    void recordFileRead(const std::string& filePath, size_t bytes, double latencyNs);
    void recordFileWrite(const std::string& filePath, size_t bytes, double latencyNs);
    void recordNetworkActivity(const std::string& interface, double sendRate, double receiveRate);
    
    void updateDeviceMetrics(const std::string& device, const IOMetrics& metrics);
    
    std::vector<IOMetrics> getDeviceMetrics(const std::string& deviceRegex = "") const;
    double getTotalReadThroughput() const;
    double getTotalWriteThroughput() const;
};

/**
 * Profiling data collector
 */
class ProfilingDataCollector {
private:
    std::string name_;
    DataCollectionConfig config_;
    bool running_;
    std::chrono::high_resolution_clock::time_point startTime_;
    
    // Data storage
    std::vector<ProfileDataPoint> allEvents_;
    std::vector<MemoryAllocationInfo> memoryAllocations_;
    std::vector<TimestepPerformance> timestepData_;
    std::vector<ThreadPerformanceMetrics> threadData_;
    std::vector<IOMetrics> ioData_;
    
    // Custom collectors
    std::vector<std::unique_ptr<CustomDataCollector>> customCollectors_;
    
    // Aggregation
    std::unordered_map<ProfileEventType, std::vector<ProfileDataPoint>> eventTypeIndex_;
    std::unordered_map<std::string, std::vector<ProfileDataPoint>> tagIndex_;
    
    // Statistics
    std::atomic<uint64_t> totalEventsCollected_{0};
    std::atomic<uint64_t> totalEventsDropped_{0};
    std::atomic<size_t> memoryUsage_{0};
    mutable std::mutex mutex_;
    
    // Callbacks
    std::vector<std::function<void(const std::vector<ProfileDataPoint>&)>> dataProcessedCallbacks_;
    std::vector<std::function<void(const CollectionResult&)>> collectionStatusCallbacks_;
    
public:
    ProfilingDataCollector(const std::string& name, const DataCollectionConfig& config);
    ~ProfilingDataCollector();
    
    void start();
    void stop();
    void pause();
    void resume();
    
    bool isRunning() const { return running_.load(); }
    bool isPaused() const { return paused_.load(); }
    
    // Collection control
    void setConfig(const DataCollectionConfig& config) { config_ = config; }
    const DataCollectionConfig& getConfig() const { return config_; }
    
    // Event recording
    void recordEvent(const ProfileDataPoint& event);
    void recordTimestepEvent(const TimestepPerformance& ts);
    void recordMemoryAllocation(const MemoryAllocationInfo& alloc);
    void recordThreadMetrics(const ThreadPerformanceMetrics& metrics);
    void recordIOMetrics(const IOMetrics& metrics);
    
    // Data access
    std::vector<ProfileDataPoint> getEvents(ProfileEventType eventType = ProfileEventType::TIMESTEP_END,
                                          uint64_t startTime = 0, uint64_t endTime = 0) const;
    std::vector<TimestepPerformance> getTimestepData(size_t maxCount = 0) const;
    std::vector<MemoryAllocationInfo> getMemoryAllocations(size_t maxCount = 0) const;
    std::vector<ThreadPerformanceMetrics> getThreadData(size_t maxCount = 0) const;
    std::vector<IOMetrics> getIOData(size_t maxCount = 0) const;
    
    // Statistics
    size_t getTotalEventsCount() const { return allEvents_.size(); }
    size_t getMemoryAllocationsCount() const { return memoryAllocations_.size(); }
    size_t getTimestepCount() const { return timestepData_.size(); }
    double getDurationSeconds() const;
    CollectionResult getCollectionResult() const;
    
    // Analysis
    std::string analyzeEventDistribution() const;
    std::string analyzeTemporalPatterns() const;
    std::string analyzeResourceUtilization() const;
    std::string analyzeErrorPatterns() const;
    
    // Custom collectors
    void addCustomCollector(std::unique_ptr<CustomDataCollector> collector);
    std::vector<std::string> getCustomCollectorNames() const;
    bool removeCustomCollector(const std::string& name);
    
    // Callbacks
    void addDataProcessedCallback(std::function<void(const std::vector<ProfileDataPoint>&)> callback);
    void addCollectionStatusCallback(std::function<void(const CollectionResult&)> callback);
    
    // Export
    std::string exportToJSON() const;
    std::string exportToCSV() const;
    std::string exportToYAML() const;
    std::string exportToXML() const;
    
    // Integration
    void integrateWithProfiler(class NLPerformanceProfiler& profiler);
    void integrateWithMonitor(class PerformanceMonitor& monitor);
    
private:
    void processCollectedData();
    void aggregateData();
    void updateIndexes();
    bool shouldDropEvent(const ProfileDataPoint& event) const;
    
    // Memory management
    void cleanupOldData();
    void compressData();
};

/**
 * Data collector factory
 */
class DataCollectorFactory {
public:
    static std::shared_ptr<ProfilingDataCollector> createCollector(const std::string& name, 
                                                                   const DataCollectionConfig& config);
    
    static std::vector<DataCollectionConfig> getAvailableCollectorConfigs();
    
    static std::unique_ptr<CustomDataCollector> createSystemCollector(const std::string& name);
    static std::unique_ptr<CustomDataCollector> createSimulationCollector(const std::string& name);
    static std::unique_ptr<CustomDataCollector> createMemoryCollector(const std::string& name);
    static std::unique_ptr<CustomDataCollector> createThreadCollector(const std::string& name);
    static std::unique_ptr<CustomDataCollector> createIOCollector(const std::string& name);
};

} // namespace nlm