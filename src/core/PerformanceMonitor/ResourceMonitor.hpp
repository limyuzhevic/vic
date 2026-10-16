#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <atomic>
#include <mutex>
#include <functional>
#include <unordered_map>
#include <deque>
#include <optional>
#include <thread>
#include <condition_variable>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <set>
#include <random>
#include <iostream>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// System resource monitoring data structures

enum class ResourceType {
    CPU,
    Memory,
    Disk,
    Network,
    GPU,
    Process,
    System
};

enum class ResourceMetric {
    Usage,
    Temperature,
    Frequency,
    Load,
    Throughput,
    Latency,
    Capacity,
    Available
};

// System resource measurement
struct ResourceMeasurement {
    ResourceType resourceType;
    ResourceMetric metric;
    double value;
    std::string unit;
    std::chrono::system_clock::time_point timestamp;
    size_t processId;
    std::string processName;
    std::map<std::string, std::string> metadata;
    
    ResourceMeasurement() : value(0.0), processId(0) {}
    
    ResourceMeasurement(ResourceType type, ResourceMetric metric, double val,
                       const std::string& unit = "",
                       size_t pid = 0, const std::string& pname = "")
        : resourceType(type), metric(metric), value(val), unit(unit),
          timestamp(std::chrono::system_clock::now()),
          processId(pid), processName(pname) {}
};

// Memory allocation tracking
struct MemoryAllocationInfo {
    uint64_t address;
    size_t size;
    std::string allocationType;
    std::chrono::system_clock::time_point allocatedTime;
    std::chrono::system_clock::time_point freedTime;
    bool isAllocated;
    size_t threadId;
    std::string allocationStack;
    
    MemoryAllocationInfo() : address(0), size(0), isAllocated(true), threadId(0) {}
};

// I/O performance metrics
struct IOMetrics {
    double readThroughput;
    double writeThroughput;
    double readLatency;
    double writeLatency;
    size_t readOperations;
    size_t writeOperations;
    std::chrono::system_clock::time_point timestamp;
    
    IOMetrics() : readThroughput(0.0), writeThroughput(0.0), 
                  readLatency(0.0), writeLatency(0.0),
                  readOperations(0), writeOperations(0) {}
};

// Resource monitoring statistics
struct ResourceStatistics {
    double avgUsage;
    double maxUsage;
    double minUsage;
    double stdDev;
    std::chrono::system_clock::time_point firstMeasurement;
    std::chrono::system_clock::time_point lastMeasurement;
    size_t measurementCount;
    
    ResourceStatistics() : avgUsage(0.0), maxUsage(0.0), minUsage(0.0), 
                          stdDev(0.0), measurementCount(0) {}
};

// Core resource monitor class
class ResourceMonitor {
public:
    using ResourceCallback = std::function<void(const ResourceMeasurement&)>
    using AlertCallback = std::function<void(const std::string&)>
    
    ResourceMonitor();
    ~ResourceMonitor();
    
    // Disable copying
    ResourceMonitor(const ResourceMonitor&) = delete;
    ResourceMonitor& operator=(const ResourceMonitor&) = delete;
    
    // Singleton pattern
    static ResourceMonitor& getInstance() {
        static ResourceMonitor instance;
        return instance;
        static_cast<void>(instance.initialize()); // Initialize singleton
        return instance;
    }
    
    // Initialization and configuration
    bool initialize();
    void shutdown();
    bool isInitialized() const { return initialized_; }
    
    // Resource measurement
    void recordMeasurement(const ResourceMeasurement& measurement);
    void recordMeasurement(ResourceType type, ResourceMetric metric, double value,
                          const std::string& unit = "",
                          size_t processId = 0, const std::string& processName = "");
    
    // CPU monitoring
    void recordCPUUsage(double usage);
    void recordCPUFrequency(double frequency);
    void recordCPULoad(double load);
    void recordCPUThreads(size_t threadCount);
    
    // Memory monitoring
    void recordMemoryUsage(uint64_t totalBytes, uint64_t usedBytes,
                          uint64_t availableBytes, uint64_t swapUsed);
    void recordMemoryAllocation(uint64_t address, size_t size,
                               const std::string& allocationType);
    void recordMemoryDeallocation(uint64_t address);
    
    // Disk I/O monitoring
    void recordDiskRead(double throughput, double latency);
    void recordDiskWrite(double throughput, double latency);
    void recordDiskUsage(double usage);
    
    // Network monitoring
    void recordNetworkUpload(double throughput, size_t packets);
    void recordNetworkDownload(double throughput, size_t packets);
    void recordNetworkLatency(double latency);
    
    // Process monitoring
    void recordProcessCPU(const std::string& processName, double usage, size_t pid);
    void recordProcessMemory(const std::string& processName, uint64_t memoryBytes, size_t pid);
    void recordProcessIO(const std::string& processName, const IOMetrics& ioMetrics, size_t pid);
    
    // GPU monitoring
    void recordGPUUsage(double usage);
    void recordGPUMemory(double usedBytes, double totalBytes);
    void recordGPUPower(double powerWatts);
    
    // Callbacks
    void setResourceCallback(ResourceCallback callback) { resourceCallback_ = callback; }
    void setAlertCallback(AlertCallback callback) { alertCallback_ = callback; }
    
    // Resource queries
    double getResourceUsage(ResourceType type, ResourceMetric metric,
                           size_t processId = 0) const;
    ResourceMeasurement getLatestMeasurement(ResourceType type, ResourceMetric metric,
                                            size_t processId = 0) const;
    std::vector<ResourceMeasurement> getMeasurements(ResourceType type,
                                                     ResourceMetric metric,
                                                     size_t count = 100) const;
    
    // Resource statistics
    ResourceStatistics getResourceStatistics(ResourceType type, ResourceMetric metric,
                                            size_t processId = 0) const;
    
    // Process information
    std::vector<std::pair<size_t, std::string>> getActiveProcesses() const;
    std::optional<std::string> getProcessName(size_t processId) const;
    
    // Memory tracking
    void startMemoryTracking();
    void stopMemoryTracking();
    bool isMemoryTracking() const { return memoryTracking_; }
    
    void allocateMemory(uint64_t address, size_t size, const std::string& type);
    void deallocateMemory(uint64_t address);
    std::optional<MemoryAllocationInfo> getMemoryAllocation(uint64_t address) const;
    std::vector<MemoryAllocationInfo> getActiveAllocations() const;
    
    // Performance analysis
    std::string analyzeMemoryUsage() const;
    std::string analyzeCPUUsage() const;
    std::string analyzeIOUsage() const;
    
    // Resource alerts
    void addResourceThreshold(ResourceType type, ResourceMetric metric,
                             double warningLevel, double criticalLevel);
    void removeResourceThreshold(ResourceType type, ResourceMetric metric);
    
    // Resource monitoring control
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const { return monitoring_; }
    
    // Integration with existing systems
    void integrateWithLogger(std::shared_ptr<Logger> logger);
    void integrateWithStatisticsCollector(std::shared_ptr<StatisticsCollector> collector);
    void integrateWithRealTimeMonitor(std::shared_ptr<RealTimeMonitor> monitor);
    
    // Performance hooks for simulation modes
    void setupBasicConnectivityModeMonitoring();
    void setupPlasticityExperimentMonitoring();
    void setupStdpVerificationMonitoring();
    void setupBenchmarkMonitoring();
    void setupContinuousModeMonitoring();
    void setupDemoModeMonitoring();
    void setupPythonInterfaceMonitoring();
    void setupScalingBenchmarkMonitoring();
    void setupResultsSaveMonitoring(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool initialized_ = false;
    bool monitoring_ = false;
    bool memoryTracking_ = false;
    
    mutable std::mutex dataMutex_;
    
    // Resource measurements
    std::deque<ResourceMeasurement> cpuMeasurements_;
    std::deque<ResourceMeasurement> memoryMeasurements_;
    std::deque<ResourceMeasurement> diskMeasurements_;
    std::deque<ResourceMeasurement> networkMeasurements_;
    std::deque<ResourceMeasurement> gpuMeasurements_;
    
    // Memory tracking
    std::unordered_map<uint64_t, MemoryAllocationInfo> memoryAllocations_;
    std::mutex memoryMutex_;
    
    // Resource thresholds
    std::vector<std::tuple<ResourceType, ResourceMetric, double, double>> resourceThresholds_;
    
    // Callbacks
    ResourceCallback resourceCallback_;
    AlertCallback alertCallback_;
    
    // Integration objects
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<StatisticsCollector> statisticsCollector_;
    std::shared_ptr<RealTimeMonitor> realTimeMonitor_;
    
    // Monitoring threads
    std::thread monitoringThread_;
    std::thread memoryTrackingThread_;
    
    // Monitoring configuration
    double monitoringInterval_ = 0.1; // seconds
    size_t maxMeasurementHistory_ = 10000;
    bool enableRealTimeAlerts_ = true;
    bool trackMemoryAllocations_ = true;
    size_t maxMemoryAllocations_ = 100000;
    
    // Resource monitoring implementation
    void updateCPUMonitoring();
    void updateMemoryMonitoring();
    void updateDiskMonitoring();
    void updateNetworkMonitoring();
    void updateGPUMonitoring();
    void updateProcessMonitoring();
    
    void checkResourceThresholds();
    void generateResourceAlerts();
    void notifyResourceListeners(const ResourceMeasurement& measurement);
    void notifyAlertListeners(const std::string& alert);
    
    // Memory tracking implementation
    void trackMemoryAllocations();
    void cleanupMemoryAllocations();
    void recordMemoryAllocationInternal(const MemoryAllocationInfo& allocation);
    void recordMemoryDeallocationInternal(uint64_t address);
    
    // System-specific measurement functions
    void measureCPUUsage();
    void measureMemoryUsage();
    void measureDiskUsage();
    void measureNetworkUsage();
    void measureGPUUsage();
    void measureProcessUsage();
    
    // Statistics calculation
    double calculateAverage(const std::deque<ResourceMeasurement>& measurements) const;
    double calculateMax(const std::deque<ResourceMeasurement>& measurements) const;
    double calculateMin(const std::deque<ResourceMeasurement>& measurements) const;
    double calculateStdDev(const std::deque<ResourceMeasurement>& measurements) const;
};

// Memory profiler for detailed memory allocation analysis
class MemoryProfiler {
public:
    using AllocationCallback = std::function<void(const MemoryAllocationInfo&)>
    using DeallocationCallback = std::function<void(uint64_t)>
    using LeakCallback = std::function<void(const std::vector<MemoryAllocationInfo>&)>
    
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Disable copying
    MemoryProfiler(const MemoryProfiler&) = delete;
    MemoryProfiler& operator=(const MemoryProfiler&) = delete;
    
    // Singleton pattern
    static MemoryProfiler& getInstance() {
        static MemoryProfiler instance;
        return instance;
    }
    
    // Initialization and configuration
    void initialize();
    void shutdown();
    bool isInitialized() const { return initialized_; }
    
    // Memory allocation tracking
    void trackAllocation(uint64_t address, size_t size, const std::string& type,
                        const std::string& stackTrace = "");
    void trackDeallocation(uint64_t address);
    void trackAllocationWithCallback(uint64_t address, size_t size,
                                   const std::string& type,
                                   const std::string& stackTrace = "");
    
    // Callbacks
    void setAllocationCallback(AllocationCallback callback) { allocationCallback_ = callback; }
    void setDeallocationCallback(DeallocationCallback callback) { deallocationCallback_ = callback; }
    void setLeakCallback(LeakCallback callback) { leakCallback_ = callback; }
    
    // Memory analysis
    size_t getTotalAllocatedMemory() const;
    size_t getTotalDeallocatedMemory() const;
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    
    std::vector<MemoryAllocationInfo> getActiveAllocations() const;
    std::vector<MemoryAllocationInfo> getAllocationHistory() const;
    
    // Memory leak detection
    std::vector<MemoryAllocationInfo> detectMemoryLeaks() const;
    size_t calculateMemoryLeakSize() const;
    bool hasMemoryLeaks() const;
    
    // Memory allocation patterns
    std::map<std::string, size_t> getAllocationCountsByType() const;
    std::map<std::string, size_t> getDeallocationCountsByType() const;
    std::map<std::string, size_t> getSizeDistributionByType() const;
    
    // Stack trace analysis
    std::map<std::string, size_t> analyzeAllocationPatterns() const;
    std::string getTopAllocatingFunctions(size_t limit = 10) const;
    std::string getTopDeallocatingFunctions(size_t limit = 10) const;
    
    // Memory optimization suggestions
    std::vector<std::string> generateMemoryOptimizationSuggestions() const;
    std::string getMemoryUsageSummary() const;
    std::string getMemoryLeakReport() const;
    
    // Memory profiling for simulation modes
    void setupMemoryProfilingForMode(const std::string& mode);
    void setupMemoryProfilingForMode(const std::string& mode, size_t simulationSize);
    
    // Memory profiling for specific operations
    void profileMemoryAllocation(const std::string& operation, size_t size);
    void profileMemoryDeallocation(const std::string& operation, size_t size);
    void profileMemoryUsage(const std::string& operation, uint64_t memoryUsage);
    
    // Integration with existing systems
    void integrateWithLogger(std::shared_ptr<Logger> logger);
    void integrateWithStatisticsCollector(std::shared_ptr<StatisticsCollector> collector);
    void integrateWithRealTimeMonitor(std::shared_ptr<RealTimeMonitor> monitor);
    
    // Memory monitoring control
    void startTracking();
    void stopTracking();
    bool isTracking() const { return tracking_; }
    
    // Memory analysis
    void analyzeMemoryUsage();
    void generateMemoryReport();
    void exportMemoryData(const std::string& filepath) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool initialized_ = false;
    bool tracking_ = false;
    
    mutable std::mutex dataMutex_;
    
    // Memory allocation tracking
    std::unordered_map<uint64_t, MemoryAllocationInfo> currentAllocations_;
    std::deque<MemoryAllocationInfo> allocationHistory_;
    
    // Callbacks
    AllocationCallback allocationCallback_;
    DeallocationCallback deallocationCallback_;
    LeakCallback leakCallback_;
    
    // Integration objects
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<StatisticsCollector> statisticsCollector_;
    std::shared_ptr<RealTimeMonitor> realTimeMonitor_;
    
    // Memory tracking thread
    std::thread memoryTrackingThread_;
    
    // Memory profiling configuration
    bool trackStackTraces_ = false;
    size_t maxStackTraceLength_ = 50;
    size_t allocationHistoryLimit_ = 100000;
    bool detectLeaks_ = true;
    double leakDetectionThreshold_ = 0.01; // 1% of allocated memory
    
    // Memory tracking implementation
    void trackMemoryAllocations();
    void cleanupMemoryHistory();
    void checkForMemoryLeaks();
    void generateMemoryAnalysis();
    
    // Memory allocation analysis
    void analyzeAllocationPatterns(std::map<std::string, size_t>& countsByType) const;
    void analyzeDeallocationPatterns(std::map<std::string, size_t>& countsByType) const;
    void analyzeSizeDistribution(std::map<std::string, size_t>& sizeByType) const;
    void analyzeStackTraces(std::map<std::string, size_t>& functionsByCount) const;
    
    // Memory leak detection
    bool isMemoryLeak(const MemoryAllocationInfo& allocation) const;
    size_t calculateLeakSize(const std::vector<MemoryAllocationInfo>& leaks) const;
    void generateLeakReport(const std::vector<MemoryAllocationInfo>& leaks) const;
    
    // Memory optimization suggestions
    void suggestPoolAllocators(std::vector<std::string>& suggestions) const;
    void suggestMemoryReuse(std::vector<std::string>& suggestions) const;
    void suggestAllocationSizeOptimization(std::vector<std::string>& suggestions) const;
    void suggestLifetimeOptimization(std::vector<std::string>& suggestions) const;
};

// Network profiler for neural network performance analysis
class NetworkProfiler {
public:
    using NetworkCallback = std::function<void(const std::string&, double)>
    
    NetworkProfiler();
    ~NetworkProfiler();
    
    // Disable copying
    NetworkProfiler(const NetworkProfiler&) = delete;
    NetworkProfiler& operator=(const NetworkProfiler&) = delete;
    
    // Singleton pattern
    static NetworkProfiler& getInstance() {
        static NetworkProfiler instance;
        return instance;
    }
    
    // Initialization and configuration
    void initialize();
    void shutdown();
    bool isInitialized() const { return initialized_; }
    
    // Neural network performance tracking
    void recordSpikePropagation(size_t neuronId, double time, double delay);
    void recordSynapticOperation(const std::string& operation, size_t synapseCount,
                                double time, double weightChange);
    void recordPlasticityOperation(const std::string& plasticityType, size_t neuronCount,
                                 double learningRate, double time);
    void recordNetworkActivity(size_t activeNeurons, size_t totalNeurons,
                              double averageFiringRate, double synchrony);
    void recordConnectivityChange(size_t oldConnections, size_t newConnections,
                                 double rewiringRate);
    
    // Performance metrics
    double getAverageSpikePropagationDelay() const;
    double getTotalSpikeCount() const;
    double getAverageFiringRate() const;
    double getNetworkSynchronousity() const;
    double getAveragePlasticityChange() const;
    double getConnectivityChangeRate() const;
    
    // Performance analysis
    std::string analyzeSpikePropagationPerformance() const;
    std::string analyzeSynapticPerformance() const;
    std::string analyzePlasticityPerformance() const;
    std::string analyzeNetworkDynamics() const;
    
    // Network performance optimization
    std::vector<std::string> getPerformanceOptimizationSuggestions() const;
    std::string getNetworkPerformanceSummary() const;
    std::string getDetailedPerformanceReport() const;
    
    // Network profiling for simulation modes
    void setupNetworkProfilingForMode(const std::string& mode);
    void setupNetworkProfilingForMode(const std::string& mode, size_t neuronCount);
    
    // Integration with existing systems
    void integrateWithLogger(std::shared_ptr<Logger> logger);
    void integrateWithStatisticsCollector(std::shared_ptr<StatisticsCollector> collector);
    void integrateWithRealTimeMonitor(std::shared_ptr<RealTimeMonitor> monitor);
    
    // Network profiling control
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const { return profiling_; }
    
    // Performance analysis
    void analyzeNetworkPerformance();
    void generateNetworkReport();
    void exportNetworkData(const std::string& filepath) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool initialized_ = false;
    bool profiling_ = false;
    
    mutable std::mutex dataMutex_;
    
    // Network performance tracking
    std::deque<std::tuple<size_t, double, double>> spikePropagationEvents_;
    std::deque<std::tuple<std::string, size_t, double, double>> synapticOperations_;
    std::deque<std::tuple<std::string, size_t, double, double>> plasticityOperations_;
    std::deque<std::tuple<size_t, size_t, double, double>> networkActivity_;
    std::deque<std::tuple<size_t, size_t, double>> connectivityChanges_;
    
    // Callbacks
    NetworkCallback networkCallback_;
    
    // Integration objects
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<StatisticsCollector> statisticsCollector_;
    std::shared_ptr<RealTimeMonitor> realTimeMonitor_;
    
    // Network profiling configuration
    bool enableSpikeTimingAnalysis_ = true;
    bool enableSynapticAnalysis_ = true;
    bool enablePlasticityAnalysis_ = true;
    bool enableNetworkDynamicsAnalysis_ = true;
    size_t spikePropagationHistoryLimit_ = 10000;
    size_t operationHistoryLimit_ = 10000;
    
    // Network performance analysis
    void analyzeSpikePropagation() const;
    void analyzeSynapticOperations() const;
    void analyzePlasticityOperations() const;
    void analyzeNetworkDynamics() const;
    
    // Performance metrics calculation
    double calculateAverageSpikePropagationDelay() const;
    double calculateAverageFiringRate() const;
    double calculateNetworkSynchronousity() const;
    double calculateAveragePlasticityChange() const;
    double calculateConnectivityChangeRate() const;
    
    // Performance optimization analysis
    void suggestNetworkOptimization(std::vector<std::string>& suggestions) const;
    void suggestSynapticOptimization(std::vector<std::string>& suggestions) const;
    void suggestPlasticityOptimization(std::vector<std::string>& suggestions) const;
    void suggestConnectivityOptimization(std::vector<std::string>& suggestions) const;
};

// Performance integration manager for coordinating all performance systems
class PerformanceIntegrationManager {
public:
    PerformanceIntegrationManager();
    ~PerformanceIntegrationManager();
    
    // Disable copying
    PerformanceIntegrationManager(const PerformanceIntegrationManager&) = delete;
    PerformanceIntegrationManager& operator=(const PerformanceIntegrationManager&) = delete;
    
    // Singleton pattern
    static PerformanceIntegrationManager& getInstance() {
        static PerformanceIntegrationManager instance;
        return instance;
    }
    
    // Initialization and configuration
    void initialize();
    void shutdown();
    bool isInitialized() const { return initialized_; }
    
    // Component management
    void addPerformanceMonitor(std::shared_ptr<PerformanceMonitor> monitor);
    void removePerformanceMonitor(const std::string& name);
    std::shared_ptr<PerformanceMonitor> getPerformanceMonitor(const std::string& name);
    std::shared_ptr<PerformanceMonitor> getDefaultPerformanceMonitor();
    
    void addProfiler(std::shared_ptr<Profiler> profiler);
    void removeProfiler(const std::string& name);
    std::shared_ptr<Profiler> getProfiler(const std::string& name);
    
    void addResourceMonitor(std::shared_ptr<ResourceMonitor> monitor);
    void removeResourceMonitor(const std::string& name);
    std::shared_ptr<ResourceMonitor> getResourceMonitor(const std::string& name);
    
    void addMemoryProfiler(std::shared_ptr<MemoryProfiler> profiler);
    void removeMemoryProfiler(const std::string& name);
    std::shared_ptr<MemoryProfiler> getMemoryProfiler(const std::string& name);
    
    void addNetworkProfiler(std::shared_ptr<NetworkProfiler> profiler);
    void removeNetworkProfiler(const std::string& name);
    std::shared_ptr<NetworkProfiler> getNetworkProfiler(const std::string& name);
    
    // Global integration
    void integrateWithLogger(std::shared_ptr<Logger> logger);
    void integrateWithStatisticsCollector(std::shared_ptr<StatisticsCollector> collector);
    void integrateWithRealTimeMonitor(std::shared_ptr<RealTimeMonitor> monitor);
    
    // Performance monitoring control
    void startAllMonitoring();
    void stopAllMonitoring();
    bool isAnyMonitoringActive();
    
    // Profile management
    void saveAllProfiles(const std::string& directory = "profiles");
    void loadAllProfiles(const std::string& directory = "profiles");
    
    // Performance analysis
    std::string generateComprehensivePerformanceReport() const;
    std::string identifySystemBottlenecks() const;
    std::string analyzePerformanceTrends() const;
    
    // Performance hooks for main.cpp
    void setupMainCppPerformanceHooks();
    void setupPerformanceMonitoringIntegration();
    
    // Configuration
    void configureFromFile(const std::string& configFile);
    void configureFromString(const std::string& configString);
    
    // Performance monitoring status
    std::string getPerformanceMonitoringStatus() const;
    std::string getActiveComponents() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool initialized_ = false;
    
    // Performance monitoring components
    std::map<std::string, std::shared_ptr<PerformanceMonitor>> performanceMonitors_;
    std::map<std::string, std::shared_ptr<Profiler>> profilers_;
    std::map<std::string, std::shared_ptr<ResourceMonitor>> resourceMonitors_;
    std::map<std::string, std::shared_ptr<MemoryProfiler>> memoryProfilers_;
    std::map<std::string, std::shared_ptr<NetworkProfiler>> networkProfilers_;
    
    // Integration objects
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<StatisticsCollector> statisticsCollector_;
    std::shared_ptr<RealTimeMonitor> realTimeMonitor_;
    
    // Default components
    std::shared_ptr<PerformanceMonitor> defaultPerformanceMonitor_;
    std::shared_ptr<Profiler> defaultProfiler_;
    std::shared_ptr<ResourceMonitor> defaultResourceMonitor_;
    std::shared_ptr<MemoryProfiler> defaultMemoryProfiler_;
    std::shared_ptr<NetworkProfiler> defaultNetworkProfiler_;
    
    // Integration implementation
    void setupLoggerIntegration();
    void setupStatisticsCollectorIntegration();
    void setupRealTimeMonitorIntegration();
    void setupPerformanceMonitorIntegration();
    void setupProfilerIntegration();
    void setupResourceMonitorIntegration();
    void setupMemoryProfilerIntegration();
    void setupNetworkProfilerIntegration();
    
    void generateIntegrationReport() const;
    void validateIntegration() const;
};

// Global convenience functions
inline std::shared_ptr<PerformanceMonitor> getPerformanceMonitor(const std::string& name = "default") {
    return PerformanceIntegrationManager::getInstance().getPerformanceMonitor(name);
}

inline std::shared_ptr<Profiler> getProfiler(const std::string& name = "default") {
    return PerformanceIntegrationManager::getInstance().getProfiler(name);
}

inline std::shared_ptr<ResourceMonitor> getResourceMonitor(const std::string& name = "default") {
    return PerformanceIntegrationManager::getInstance().getResourceMonitor(name);
}

inline std::shared_ptr<MemoryProfiler> getMemoryProfiler(const std::string& name = "default") {
    return PerformanceIntegrationManager::getInstance().getMemoryProfiler(name);
}

inline std::shared_ptr<NetworkProfiler> getNetworkProfiler(const std::string& name = "default") {
    return PerformanceIntegrationManager::getInstance().getNetworkProfiler(name);
}

inline void initializePerformanceSystems() {
    PerformanceIntegrationManager::getInstance().initialize();
}

inline void shutdownPerformanceSystems() {
    PerformanceIntegrationManager::getInstance().shutdown();
}

inline void startAllPerformanceMonitoring() {
    PerformanceIntegrationManager::getInstance().startAllMonitoring();
}

inline void stopAllPerformanceMonitoring() {
    PerformanceIntegrationManager::getInstance().stopAllMonitoring();
}

inline void generatePerformanceReport() {
    PerformanceIntegrationManager::getInstance().generateComprehensivePerformanceReport();
}

} // namespace nlm
