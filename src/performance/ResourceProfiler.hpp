#pragma once

/**
 * ResourceProfiler - Real-time resource usage tracking and monitoring
 * 
 * Provides comprehensive resource monitoring including:
 * - CPU usage and performance counters
 * - Memory allocation and usage tracking
 * - I/O operations monitoring
 * - Thread performance tracking
 * - Network performance metrics
 * - GPU resources (if available)
 * - System-wide resource aggregation
 * - Resource leak detection
 * - Performance bottleneck identification
 * 
 * Features:
 * - Real-time resource monitoring
 * - Historical resource tracking
 * - Resource leak detection
 * - Performance bottleneck identification
 * - Resource usage visualization
 * - Automated resource optimization suggestions
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <thread>
#include <queue>
#include <deque>
#include <map>
#include <set>
#include <array>
#include <variant>
#include <optional>

namespace nlm {

// Resource types
enum class ResourceType {
    CPU,
    Memory,
    Disk,
    Network,
    Threads,
    FileDescriptors,
    GPU,
    OpenCL,
    CUDA,
    DirectX,
    Vulkan,
    Custom
};

// Resource measurement units
enum class ResourceUnit {
    Count,
    Bytes,
    Bits,
    Percent,
    Milliseconds,
    Nanoseconds,
    Operations,
    Requests,
    BytesPerSecond,
    OperationsPerSecond,
    Hertz
};

// Resource measurement
struct ResourceMeasurement {
    ResourceType type;                      // Resource type
    ResourceUnit unit;                      // Measurement unit
    double value;                          // Measured value
    uint64_t timestamp;                    // Measurement timestamp (ns)
    std::string label;                     // Resource label (e.g., "neuron-123")
    std::string metadata;                  // Additional metadata
    std::unordered_map<std::string, double> customMetrics;  // Custom metrics
};

// Resource usage statistics
struct ResourceStats {
    ResourceType type;                      // Resource type
    ResourceUnit unit;                      // Measurement unit
    double count;                          // Number of measurements
    double sum;                           // Sum of values
    double min;                           // Minimum value
    double max;                           // Maximum value
    double mean;                          // Mean value
    double median;                        // Median value
    double stddev;                        // Standard deviation
    double variance;                      // Variance
    double p95;                           // 95th percentile
    double p99;                           // 99th percentile
    double p999;                          // 99.9th percentile
    double throughput;                    // Values per second
    double efficiency;                    // Efficiency metric
};

// Resource allocation info
struct ResourceAllocation {
    std::string label;                     // Resource label
    uint64_t size;                         // Allocation size
    uint64_t timestamp;                    // Allocation timestamp
    std::string source;                    // Allocation source (function, file, etc.)
    int threadId;                          // Thread that allocated
    bool isFreed;                          // Whether resource was freed
    uint64_t freeTimestamp;                // Free timestamp (if freed)
    std::chrono::nanoseconds lifetime;     // Resource lifetime
    std::string allocationStack;           // Allocation stack trace
    std::string freeStack;                 // Free stack trace
};

// System-wide resource summary
struct SystemResourceSummary {
    uint64_t timestamp;                    // Timestamp
    double cpuUsage;                       // CPU usage (%)
    double memoryUsage;                    // Memory usage (MB)
    double memoryAvailable;                // Available memory (MB)
    double diskUsage;                      // Disk usage (MB)
    double networkUpload;                  // Network upload (MB/s)
    double networkDownload;                // Network download (MB/s)
    uint32_t threadCount;                  // Number of threads
    uint32_t fileDescriptorCount;          // Number of file descriptors
    std::vector<ResourceStats> resourceStats; // Per-resource statistics
    std::vector<ResourceAllocation> allocations; // Resource allocations
};

/**
 * System information
 */
struct SystemInfo {
    std::string hostname;                  // Host name
    std::string os;                        // Operating system
    std::string architecture;              // CPU architecture
    uint32_t cpuCores;                     // Number of CPU cores
    uint64_t totalMemory;                  // Total memory (bytes)
    std::string cpuBrand;                  // CPU brand string
    std::string compiler;                  // Compiler information
    std::string buildInfo;                 // Build information
    std::vector<std::string> gpuDevices;   // GPU device names
    std::string kernelVersion;             // Kernel version (Linux)
    std::string driverVersion;             // Driver version
};

/**
 * Resource counter for high-performance measurement
 */
class ResourceCounter {
public:
    ResourceCounter() = default;
    ~ResourceCounter() = default;
    
    /**
     * Start counting
     */
    void start() {
        startTime_ = std::chrono::high_resolution_clock::now();
        running_ = true;
    }
    
    /**
     * Stop counting
     */
    void stop() {
        if (running_) {
            auto endTime = std::chrono::high_resolution_clock::now();
            duration_ += std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime_);
            running_ = false;
        }
    }
    
    /**
     * Get elapsed time
     */
    std::chrono::nanoseconds getElapsed() const {
        if (running_) {
            auto now = std::chrono::high_resolution_clock::now();
            return duration_ + (now - startTime_);
        }
        return duration_;
    }
    
    /**
     * Reset counter
     */
    void reset() {
        duration_ = std::chrono::nanoseconds(0);
        running_ = false;
    }

private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::nanoseconds duration_;
    bool running_ = false;
};

/**
 * CPU performance counters
 */
class CPUMonitor {
public:
    struct CPUStats {
        double usagePercent;                 // CPU usage (%)
        uint64_t userTime;                   // User time (ns)
        uint64_t systemTime;                 // System time (ns)
        uint64_t idleTime;                   // Idle time (ns)
        uint32_t frequency;                  // CPU frequency (MHz)
        uint32_t cores;                      // Number of cores
        double temperature;                  // CPU temperature (°C)
        double power;                        // CPU power (Watts)
        uint64_t instructionsRetired;       // Instructions retired
        uint64_t cycles;                     // CPU cycles
        double instructionsPerCycle;         // Instructions per cycle
        std::vector<double> coreUsages;      // Per-core usage (%)
    };
    
    CPUMonitor() = default;
    ~CPUMonitor() = default;
    
    /**
     * Get current CPU stats
     */
    CPUStats getStats() const;
    
    /**
     * Start CPU monitoring
     */
    void startMonitoring(uint32_t intervalMs = 100);
    
    /**
     * Stop CPU monitoring
     */
    void stopMonitoring();
    
    /**
     * Get monitoring history
     */
    std::vector<CPUStats> getHistory() const;
    
    /**
     * Set monitoring interval
     */
    void setInterval(uint32_t intervalMs) { monitoringIntervalMs_ = intervalMs; }

private:
    mutable std::mutex statsMutex_;
    std::vector<CPUStats> history_;
    uint32_t monitoringIntervalMs_;
    bool monitoring_;
    std::thread monitoringThread_;
};

/**
 * Memory monitor
 */
class MemoryMonitor {
public:
    struct MemoryStats {
        uint64_t totalBytes;                 // Total memory (bytes)
        uint64_t usedBytes;                  // Used memory (bytes)
        uint64_t freeBytes;                  // Free memory (bytes)
        uint64_t cachedBytes;                // Cached memory (bytes)
        uint64_t buffersBytes;               // Buffer memory (bytes)
        double usagePercent;                 // Usage (%)
        uint64_t availableBytes;             // Available memory (bytes)
        std::vector<uint64_t> processMemory; // Per-process memory (bytes)
        std::vector<std::string> processNames;// Process names
        
        // Allocation tracking
        uint64_t mallocCount;                // malloc calls
        uint64_t mallocBytes;                // malloc bytes allocated
        uint64_t freeCount;                  // free calls
        uint64_t freeBytes;                  // free bytes
        uint64_t reallocCount;               // realloc calls
        uint64_t reallocBytes;               // realloc bytes
    };
    
    MemoryMonitor() = default;
    ~MemoryMonitor() = default;
    
    /**
     * Get current memory stats
     */
    MemoryStats getStats() const;
    
    /**
     * Start memory monitoring
     */
    void startMonitoring(uint32_t intervalMs = 100);
    
    /**
     * Stop memory monitoring
     */
    void stopMonitoring();
    
    /**
     * Get monitoring history
     */
    std::vector<MemoryStats> getHistory() const;
    
    /**
     * Record allocation
     */
    void recordAllocation(uint64_t size, const std::string& label = "", 
                         const std::string& source = "");
    
    /**
     * Record deallocation
     */
    void recordDeallocation(uint64_t size, const std::string& label = "");
    
    /**
     * Get allocation summary
     */
    std::vector<ResourceAllocation> getAllocations() const;
    
    /**
     * Set monitoring interval
     */
    void setInterval(uint32_t intervalMs) { monitoringIntervalMs_ = intervalMs; }

private:
    mutable std::mutex statsMutex_;
    std::vector<MemoryStats> history_;
    std::vector<ResourceAllocation> allocations_;
    uint32_t monitoringIntervalMs_;
    bool monitoring_;
    std::thread monitoringThread_;
};

/**
 * Network monitor
 */
class NetworkMonitor {
public:
    struct NetworkStats {
        double uploadSpeed;                  // Upload speed (MB/s)
        double downloadSpeed;                // Download speed (MB/s)
        uint64_t uploadBytes;                // Upload bytes
        uint64_t downloadBytes;              // Download bytes
        uint64_t uploadPackets;              // Upload packets
        uint64_t downloadPackets;            // Download packets
        uint32_t uploadErrors;              // Upload errors
        uint32_t downloadErrors;            // Download errors
        std::vector<std::string> activeConnections; // Active connections
        uint32_t connectionCount;            // Number of connections
        double latency;                      // Network latency (ms)
        double jitter;                       // Network jitter (ms)
    };
    
    NetworkMonitor() = default;
    ~NetworkMonitor() = default;
    
    /**
     * Get current network stats
     */
    NetworkStats getStats() const;
    
    /**
     * Start network monitoring
     */
    void startMonitoring(uint32_t intervalMs = 1000);
    
    /**
     * Stop network monitoring
     */
    void stopMonitoring();
    
    /**
     * Get monitoring history
     */
    std::vector<NetworkStats> getHistory() const;
    
    /**
     * Set monitoring interval
     */
    void setInterval(uint32_t intervalMs) { monitoringIntervalMs_ = intervalMs; }

private:
    mutable std::mutex statsMutex_;
    std::vector<NetworkStats> history_;
    uint32_t monitoringIntervalMs_;
    bool monitoring_;
    std::thread monitoringThread_;
};

/**
 * Thread monitor
 */
class ThreadMonitor {
public:
    struct ThreadStats {
        uint32_t threadId;                   // Thread ID
        std::string name;                    // Thread name
        std::chrono::nanoseconds cpuTime;    // CPU time (ns)
        std::chrono::nanoseconds wallTime;   // Wall time (ns)
        double cpuUsage;                     // CPU usage (%)
        uint64_t memoryUsage;                // Memory usage (bytes)
        std::string state;                   // Thread state (running, sleeping, etc.)
        uint64_t waitTime;                   // Wait time (ns)
        uint64_t contextSwitches;           // Context switches
        uint64_t pageFaults;                 // Page faults
        std::vector<std::string> stackTrace; // Stack trace (sampled)
    };
    
    ThreadMonitor() = default;
    ~ThreadMonitor() = default;
    
    /**
     * Get current thread stats
     */
    std::vector<ThreadStats> getStats() const;
    
    /**
     * Start thread monitoring
     */
    void startMonitoring(uint32_t intervalMs = 100);
    
    /**
     * Stop thread monitoring
     */
    void stopMonitoring();
    
    /**
     * Get monitoring history
     */
    std::vector<std::vector<ThreadStats>> getHistory() const;
    
    /**
     * Set monitoring interval
     */
    void setInterval(uint32_t intervalMs) { monitoringIntervalMs_ = intervalMs; }

private:
    mutable std::mutex statsMutex_;
    std::vector<std::vector<ThreadStats>> history_;
    uint32_t monitoringIntervalMs_;
    bool monitoring_;
    std::thread monitoringThread_;
};

/**
 * GPU monitor (if available)
 */
class GPUMonitor {
public:
    struct GPUStats {
        uint32_t deviceId;                   // GPU device ID
        std::string name;                    // GPU name
        double memoryUsage;                  // Memory usage (MB)
        double memoryAvailable;              // Available memory (MB)
        double memoryUsagePercent;           // Memory usage (%)
        double utilization;                  // GPU utilization (%)
        double temperature;                  // Temperature (°C)
        double power;                        // Power consumption (Watts)
        double clockSpeed;                   // Clock speed (MHz)
        uint64_t memoryBandwidth;            // Memory bandwidth (GB/s)
        uint64_t computeUnits;               // Number of compute units
        uint64_t streamingProcessors;        // Number of streaming processors
        std::vector<uint64_t> memoryUsageHistory; // Memory usage history
        std::vector<double> utilizationHistory;   // Utilization history
    };
    
    GPUMonitor() = default;
    ~GPUMonitor() = default;
    
    /**
     * Get current GPU stats
     */
    std::vector<GPUStats> getStats() const;
    
    /**
     * Start GPU monitoring
     */
    void startMonitoring(uint32_t intervalMs = 100);
    
    /**
     * Stop GPU monitoring
     */
    void stopMonitoring();
    
    /**
     * Get monitoring history
     */
    std::vector<std::vector<GPUStats>> getHistory() const;
    
    /**
     * Set monitoring interval
     */
    void setInterval(uint32_t intervalMs) { monitoringIntervalMs_ = intervalMs; }

private:
    mutable std::mutex statsMutex_;
    std::vector<std::vector<GPUStats>> history_;
    uint32_t monitoringIntervalMs_;
    bool monitoring_;
    std::thread monitoringThread_;
};

/**
 * I/O monitor
 */
class IOMonitor {
public:
    struct IOStats {
        std::string device;                  // Device name (e.g., "sda", "/dev/sda")
        uint64_t reads;                      // Number of reads
        uint64_t writes;                     // Number of writes
        uint64_t readBytes;                  // Read bytes
        uint64_t writeBytes;                 // Write bytes
        uint64_t readTime;                   // Read time (ns)
        uint64_t writeTime;                  // Write time (ns)
        double readRate;                     // Read rate (MB/s)
        double writeRate;                    // Write rate (MB/s)
        double readLatency;                  // Read latency (ms)
        double writeLatency;                 // Write latency (ms)
        uint32_t readErrors;                // Read errors
        uint32_t writeErrors;               // Write errors
    };
    
    IOMonitor() = default;
    ~IOMonitor() = default;
    
    /**
     * Get current I/O stats
     */
    std::vector<IOStats> getStats() const;
    
    /**
     * Start I/O monitoring
     */
    void startMonitoring(uint32_t intervalMs = 1000);
    
    /**
     * Stop I/O monitoring
     */
    void stopMonitoring();
    
    /**
     * Get monitoring history
     */
    std::vector<std::vector<IOStats>> getHistory() const;
    
    /**
     * Set monitoring interval
     */
    void setInterval(uint32_t intervalMs) { monitoringIntervalMs_ = intervalMs; }

private:
    mutable std::mutex statsMutex_;
    std::vector<std::vector<IOStats>> history_;
    uint32_t monitoringIntervalMs_;
    bool monitoring_;
    std::thread monitoringThread_;
};

/**
 * Main ResourceProfiler class
 */
class ResourceProfiler {
public:
    struct Config {
        bool enableCPUMonitoring;              // Enable CPU monitoring
        bool enableMemoryMonitoring;          // Enable memory monitoring
        bool enableNetworkMonitoring;         // Enable network monitoring
        bool enableThreadMonitoring;          // Enable thread monitoring
        bool enableGPUMonitoring;             // Enable GPU monitoring
        bool enableIOMonitoring;              // Enable I/O monitoring
        uint32_t monitoringIntervalMs;        // Monitoring interval (ms)
        uint64_t maxHistorySize;              // Maximum history size
        bool detectLeaks;                     // Enable leak detection
        bool enableBottleneckDetection;       // Enable bottleneck detection
        double leakThresholdPercent;          // Leak detection threshold (%)
        std::string outputFile;              // Output file for resource data
        bool enableRealTimeReporting;         // Enable real-time reporting
        uint32_t reportingIntervalMs;         // Reporting interval (ms)
    };
    
    ResourceProfiler(const Config& config);
    ~ResourceProfiler();
    
    /**
     * Configure monitoring components
     */
    void configure(const Config& config);
    
    /**
     * Start resource monitoring
     */
    void startMonitoring();
    
    /**
     * Stop resource monitoring
     */
    void stopMonitoring();
    
    /**
     * Take a snapshot
     */
    ResourceMeasurement takeSnapshot(ResourceType type, const std::string& label = "",
                                     const std::string& metadata = "");
    
    /**
     * Get resource statistics
     */
    ResourceStats getStats(ResourceType type, const std::string& label = "") const;
    
    /**
     * Get time-series data
     */
    std::vector<ResourceMeasurement> getTimeSeries(ResourceType type, 
                                                   const std::string& label = "",
                                                   uint64_t startTime = 0,
                                                   uint64_t endTime = 0) const;
    
    /**
     * Get system resource summary
     */
    SystemResourceSummary getSystemSummary() const;
    
    /**
     * Get system information
     */
    SystemInfo getSystemInfo() const;
    
    /**
     * Get allocation information
     */
    std::vector<ResourceAllocation> getAllocations(const std::string& label = "") const;
    
    /**
     * Detect resource leaks
     */
    std::vector<ResourceAllocation> detectLeaks(double thresholdPercent = 5.0) const;
    
    /**
     * Detect performance bottlenecks
     */
    std::vector<std::string> detectBottlenecks() const;
    
    /**
     * Generate resource report
     */
    std::string generateReport() const;
    
    /**
     * Save resource data to file
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * Load resource data from file
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * Clear history
     */
    void clearHistory();
    
    /**
     * Set custom metric
     */
    void setCustomMetric(const std::string& name, double value);
    
    /**
     * Get custom metric
     */
    double getCustomMetric(const std::string& name) const;
    
    /**
     * Is monitoring active
     */
    bool isMonitoring() const { return monitoring_; }
    
    /**
     * Get monitoring status
     */
    std::string getMonitoringStatus() const;

private:
    Config config_;
    bool monitoring_;
    mutable std::mutex dataMutex_;
    
    // Component monitors
    CPUMonitor cpuMonitor_;
    MemoryMonitor memoryMonitor_;
    NetworkMonitor networkMonitor_;
    ThreadMonitor threadMonitor_;
    GPUMonitor gpuMonitor_;
    IOMonitor ioMonitor_;
    
    // Resource data
    std::deque<ResourceMeasurement> measurements_;
    std::deque<SystemResourceSummary> systemSummaries_;
    uint64_t maxHistorySize_;
    
    // Custom metrics
    std::unordered_map<std::string, double> customMetrics_;
    
    // Background monitoring thread
    std::thread monitoringThread_;
    
    /**
     * Background monitoring loop
     */
    void monitoringLoop();
    
    /**
     * Update system summary
     */
    void updateSystemSummary();
    
    /**
     * Calculate resource statistics
     */
    static ResourceStats calculateStats(const std::vector<ResourceMeasurement>& measurements);
    
    /**
     * Filter measurements by time range
     */
    static std::vector<ResourceMeasurement> filterByTime(const std::vector<ResourceMeasurement>& measurements,
                                                        uint64_t startTime,
                                                        uint64_t endTime);
};

} // namespace nlm