#pragma once

/**
 * IOProfiler - I/O performance profiling for large datasets
 * 
 * This class provides detailed I/O performance profiling capabilities for:
 * - File I/O throughput and latency measurement
 * - Disk I/O performance analysis
 * - Network I/O performance monitoring
 * - SSD/HHD performance comparison
 * - I/O pattern analysis
 * - I/O optimization recommendations
 * 
 * Designed for profiling I/O-intensive operations in large-scale
 * neural network simulations and data processing
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <queue>
#include <set>
#include <filesystem>
#include <fstream>
#include <map>

namespace nlm {

/**
 * File operation type
 */
enum class FileOperation {
    READ,
    WRITE,
    READ_WRITE,
    DELETE,
    CREATE,
    OPEN,
    CLOSE,
    SYNC,
    FTRUNCATE,
    FSTAT,
    LSEEK,
    FSYNC,
    UNKNOWN
};

/**
 * File I/O operation record
 */
struct FileIOOperation {
    uint64_t operationId;
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    FileOperation operation;
    std::string filePath;
    uint64_t fileSize;
    uint64_t bytesTransferred;
    double latency;               // milliseconds
    double throughput;            // bytes per second
    uint64_t blockSize;
    std::string accessPattern;    // "sequential", "random", "mixed"
    std::string ioPriority;      // "high", "medium", "low"
    std::string errorMessage;
    bool successful;
    std::string userContext;      // "simulation", "checkpoint", "memory", "cache"
    std::unordered_map<std::string, std::string> metadata;
    
    FileIOOperation() : operationId(0), fileSize(0), bytesTransferred(0), latency(0.0),
                       throughput(0.0), blockSize(0), successful(false) {}
};

/**
 * Disk performance metrics
 */
struct DiskPerformance {
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    std::string deviceName;       // e.g., "/dev/sda", "nvme0n1", "ramdisk"
    std::string deviceType;       // "HDD", "SSD", "NVMe", "RAM_DISK"
    uint64_t totalSpace;          // total disk space (bytes)
    uint64_t freeSpace;           // free disk space (bytes)
    uint64_t usedSpace;           // used disk space (bytes)
    double readSpeed;            // MB/s
    double writeSpeed;           // MB/s
    double readIOPS;             // IOPS (input/output operations per second)
    double writeIOPS;            // IOPS (input/output operations per second)
    double averageReadLatency;   // milliseconds
    double averageWriteLatency;  // milliseconds
    double queueDepth;           // average queue depth
    uint64_t readOperations;      // number of read operations
    uint64_t writeOperations;     // number of write operations
    std::string filesystem;      // filesystem type
    std::string mountPoint;      // mount point
    bool isRemovable;            // removable media
    bool isSSD;                  // solid state drive
    bool isNVMe;                 // NVMe drive
    std::string serialNumber;    // device serial number
    std::string model;           // device model
    
    DiskPerformance() : totalSpace(0), freeSpace(0), usedSpace(0), readSpeed(0.0),
                       writeSpeed(0.0), readIOPS(0.0), writeIOPS(0.0),
                       averageReadLatency(0.0), averageWriteLatency(0.0),
                       queueDepth(0.0), readOperations(0), writeOperations(0),
                       isRemovable(false), isSSD(false), isNVMe(false) {}
};

/**
 * I/O pattern record
 */
struct IOPattern {
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    std::string patternType;     // "burst", "steady", "periodic", "random"
    double averageLatency;       // milliseconds
    double latencyVariation;    // standard deviation
    double throughput;          // bytes per second
    uint64_t totalBytes;         // total bytes transferred
    uint64_t totalOperations;    // total operations
    uint64_t peakOperationsPerSec; // peak IOPS
    double burstFrequency;      // bursts per second
    std::string dataPattern;    // "sequential", "random", "interleaved"
    std::unordered_map<std::string, double> metrics;
    
    IOPattern() : startTime(std::chrono::high_resolution_clock::now()), endTime(startTime),
                 averageLatency(0.0), latencyVariation(0.0), throughput(0.0),
                 totalBytes(0), totalOperations(0), peakOperationsPerSec(0), burstFrequency(0.0) {}
};

/**
 * Network I/O performance metrics
 */
struct NetworkIOPerformance {
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    std::string protocol;        // "TCP", "UDP", "HTTP", "HTTPS"
    std::string endpoint;        // remote endpoint
    double uploadSpeed;          // Mbps
    double downloadSpeed;        // Mbps
    double pingLatency;          // milliseconds
    double jitter;               // milliseconds
    uint64_t packetsSent;         // packets sent
    uint64_t packetsReceived;    // packets received
    double packetLossRate;       // fraction lost
    double connectionTime;       // seconds
    uint64_t concurrentConnections; // number of concurrent connections
    std::string bandwidthProfile; // "high", "medium", "low"
    std::unordered_map<std::string, double> qosMetrics;
    
    NetworkIOPerformance() : uploadSpeed(0.0), downloadSpeed(0.0), pingLatency(0.0),
                            jitter(0.0), packetsSent(0), packetsReceived(0),
                            packetLossRate(0.0), connectionTime(0.0), concurrentConnections(0) {}
};

/**
 * I/O bottleneck detection
 */
struct IOBottleneck {
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    std::string bottleneckType;      // "disk_latency", "disk_throughput", "network_latency", "network_throughput"
    std::string component;           // "neural_computation", "checkpoint", "memory", "cache"
    double measuredValue;            // current value
    double threshold;               // threshold value
    double severity;                 // 0.0 to 1.0 severity level
    bool isActive;                   // whether bottleneck is currently active
    std::string recommendation;      // recommendation to resolve bottleneck
    std::chrono::duration<double> duration; // duration of bottleneck
    
    IOBottleneck() : measuredValue(0.0), threshold(0.0), severity(0.0),
                    isActive(false), duration(0.0) {}
};

/**
 * I/O profiler configuration
 */
struct IOProfilerConfig {
    bool enableFileIOProfiling;           // Enable file I/O profiling
    bool enableDiskPerformanceMonitoring; // Enable disk performance monitoring
    bool enableNetworkIOMonitoring;       // Enable network I/O monitoring
    bool enablePatternAnalysis;          // Enable I/O pattern analysis
    bool enableBottleneckDetection;      // Enable bottleneck detection
    double fileIOInterval;               // file I/O sampling interval (seconds)
    double diskMonitorInterval;          // disk monitoring interval (seconds)
    double networkMonitorInterval;       // network monitoring interval (seconds)
    size_t maxFileOperations;           // maximum file operations to store
    size_t maxDiskSamples;              // maximum disk samples to store
    size_t maxNetworkSamples;           // maximum network samples to store
    std::string defaultFileAccessPattern; // default access pattern
    std::string ioPriorityLevel;        // default I/O priority level
    std::string outputDirectory;        // output directory for results
    bool exportToJSON;                   // export results to JSON
    bool exportToCSV;                    // export results to CSV
    
    IOProfilerConfig() : enableFileIOProfiling(true), enableDiskPerformanceMonitoring(true),
                        enableNetworkIOMonitoring(true), enablePatternAnalysis(true),
                        enableBottleneckDetection(true), fileIOInterval(0.1),
                        diskMonitorInterval(1.0), networkMonitorInterval(1.0),
                        maxFileOperations(100000), maxDiskSamples(10000),
                        maxNetworkSamples(10000), defaultFileAccessPattern("sequential"),
                        ioPriorityLevel("high"), outputDirectory("./io_profiling"),
                        exportToJSON(true), exportToCSV(true) {}
};

/**
 * I/O profiler for comprehensive I/O performance tracking and analysis
 */
class IOProfiler {
public:
    IOProfiler();
    ~IOProfiler();
    
    // Disable copying
    IOProfiler(const IOProfiler&) = delete;
    IOProfiler& operator=(const IOProfiler&) = delete;
    
    /**
     * Initialize the I/O profiler
     */
    bool initialize();
    
    /**
     * Shutdown the I/O profiler
     */
    void shutdown();
    
    /**
     * Record file I/O operation
     */
    void recordFileOperation(const FileIOOperation& operation);
    
    /**
     * Record disk performance metrics
     */
    void recordDiskPerformance(const DiskPerformance& performance);
    
    /**
     * Record network I/O performance
     */
    void recordNetworkIO(const NetworkIOPerformance& networkIO);
    
    /**
     * Start I/O pattern tracking
     */
    void startPatternTracking(const std::string& patternId, const std::string& patternType);
    
    /**
     * Update I/O pattern
     */
    void updatePattern(const std::string& patternId, uint64_t bytesTransferred,
                       double latency, size_t operations);
    
    /**
     * Stop I/O pattern tracking
     */
    void stopPatternTracking(const std::string& patternId);
    
    /**
     * Record I/O bottleneck
     */
    void recordBottleneck(const IOBottleneck& bottleneck);
    
    /**
     * Get all file operations
     */
    std::vector<FileIOOperation> getFileOperations() const;
    
    /**
     * Get disk performance metrics
     */
    std::vector<DiskPerformance> getDiskPerformance() const;
    
    /**
     * Get network I/O performance
     */
    std::vector<NetworkIOPerformance> getNetworkIOPerformance() const;
    
    /**
     * Get I/O patterns
     */
    std::unordered_map<std::string, IOPattern> getIOPatterns() const;
    
    /**
     * Get I/O bottlenecks
     */
    std::vector<IOBottleneck> getIOBottlenecks() const;
    
    /**
     * Analyze I/O performance
     */
    std::unordered_map<std::string, double> analyzeIOPerformance() const;
    
    /**
     * Analyze disk performance
     */
    std::unordered_map<std::string, double> analyzeDiskPerformance() const;
    
    /**
     * Analyze network performance
     */
    std::unordered_map<std::string, double> analyzeNetworkPerformance() const;
    
    /**
     * Generate file I/O report
     */
    std::string generateFileIOReport() const;
    
    /**
     * Generate disk performance report
     */
    std::string generateDiskReport() const;
    
    /**
     * Generate network I/O report
     */
    std::string generateNetworkIOReport() const;
    
    /**
     * Generate I/O pattern report
     */
    std::string generateIOPatternReport() const;
    
    /**
     * Generate bottleneck report
     */
    std::string generateBottleneckReport() const;
    
    /**
     * Export I/O data to JSON
     */
    std::string exportToJSON() const;
    
    /**
     * Export I/O data to CSV
     */
    std::string exportToCSV() const;
    
    /**
     * Generate comprehensive I/O report
     */
    std::string generateReport() const;
    
    /**
     * Reset all I/O profiling data
     */
    void reset();
    
    /**
     * Get profiler instance (singleton)
     */
    static IOProfiler& getInstance();
    
    /**
     * Configure I/O profiler
     */
    void configure(const IOProfilerConfig& config) {
        config_ = config;
    }
    
    /**
     * Get current configuration
     */
    IOProfilerConfig getConfig() const { return config_; }
    
    /**
     * Get top files by I/O operations
     */
    std::vector<std::pair<std::string, uint64_t>> getTopFilesByIO(size_t limit = 10) const;
    
    /**
     * Get I/O performance by operation type
     */
    std::unordered_map<std::string, double> getPerformanceByOperation() const;
    
    /**
     * Get I/O performance by access pattern
     */
    std::unordered_map<std::string, double> getPerformanceByAccessPattern() const;
    
    /**
     * Get disk performance statistics
     */
    std::unordered_map<std::string, double> getDiskStatistics() const;
    
    /**
     * Get network I/O statistics
     */
    std::unordered_map<std::string, double> getNetworkStatistics() const;
    
    /**
     * Check for I/O bottlenecks
     */
    std::vector<std::string> detectBottlenecks() const;
    
    /**
     * Get optimization recommendations
     */
    std::vector<std::string> getOptimizationRecommendations() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    IOProfilerConfig config_;
    
    /**
     * Clean up old data
     */
    void cleanupOldData();
    
    /**
     * Calculate I/O throughput
     */
    double calculateThroughput(const FileIOOperation& operation) const;
    
    /**
     * Detect I/O bottlenecks
     */
    IOBottleneck detectBottleneck(const FileIOOperation& operation) const;
    
    /**
     * Get disk device information
     */
    void gatherDiskInfo();
    
    /**
     * Analyze I/O patterns
     */
    void analyzeIOPatterns();
    
    /**
     * Generate file access report
     */
    std::string generateFileAccessReport() const;
    
    /**
     * Generate disk health report
     */
    std::string generateDiskHealthReport() const;
};

} // namespace nlm

#endif // NLM_IO_PROFILER_HPP