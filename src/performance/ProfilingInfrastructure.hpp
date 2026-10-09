// Performance Profiling Infrastructure
// Comprehensive profiling framework for NLM brain emulation

#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <memory>
#include <sstream>
#include <iomanip>
#include <mutex>

namespace nlm {

/**
 * Thread profiling implementation
 */
class ThreadProfiler {
public:
    ThreadProfiler();
    ~ThreadProfiler();
    
    // Start/stop profiling
    void startProfiling(const std::string& threadName = "");
    void stopProfiling();
    bool isProfiling() const { return isProfiling_; }
    
    // Record function entry/exit
    void recordFunctionEntry(const std::string& functionName,
                            const std::string& filePath,
                            uint32_t lineNumber,
                            uint64_t instructionPointer);
    
    void recordFunctionExit(uint64_t instructionPointer);
    
    // Get profiling data
    std::vector<ProfileFrame> getCurrentProfile() const;
    std::vector<ProfileFrame> getCompleteProfile() const { return completeProfile_; }
    
    // Performance analysis
    double calculateHotness(const std::string& functionName) const;
    std::vector<std::string> getTopHotFunctions(size_t count = 10) const;
    
    // Flame graph generation
    std::string generateFlameGraph(size_t maxDepth = 50) const;
    
private:
    struct CallStackEntry {
        uint64_t timestamp;
        uint64_t instructionPointer;
        std::string functionName;
        std::string filePath;
        uint32_t lineNumber;
        bool isEntry;
        std::vector<uint64_t> children;  // Child instruction pointers
        
        CallStackEntry(uint64_t ts, uint64_t ip, const std::string& func,
                      const std::string& file, uint32_t line, bool entry)
            : timestamp(ts), instructionPointer(ip), functionName(func),
              filePath(file), lineNumber(line), isEntry(entry) {}
    };
    
    bool isProfiling_;
    std::string threadName_;
    uint64_t threadId_;
    std::vector<CallStackEntry> callStack_;
    std::vector<ProfileFrame> completeProfile_;
    mutable std::mutex mutex_;
    
    // Internal methods
    void buildCompleteProfile();
    ProfileFrame createProfileFrame(const CallStackEntry& entry) const;
    
    // Sampling thread
    std::thread samplingThread_;
    std::atomic<bool> stopSampling_;
    double samplingInterval_;
    
    void samplingWorker();
};

/**
 * Memory profiling with leak detection
 */
class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Start/stop profiling
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const { return isProfiling_; }
    
    // Allocation tracking
    void recordAllocation(size_t size, const char* file, size_t line,
                         const std::string& type = "default");
    void recordDeallocation(void* address, size_t size);
    
    // Leak detection
    struct LeakInfo {
        void* address;
        size_t size;
        const char* file;
        size_t line;
        std::string allocationType;
        uint64_t timestamp;
        std::vector<MemoryRecord> stackTrace;
    };
    
    std::vector<LeakInfo> detectLeaks() const;
    std::vector<LeakInfo> detectLeaks(std::chrono::milliseconds ageThreshold) const;
    
    // Memory usage tracking
    struct MemoryUsage {
        uint64_t timestamp;
        size_t totalAllocated;
        size_t peakAllocated;
        size_t currentAllocated;
        std::unordered_map<std::string, size_t> allocationsByType;
        std::vector<size_t> allocationHistory;
    };
    
    MemoryUsage getCurrentMemoryUsage() const;
    std::vector<MemoryUsage> getMemoryUsageHistory(size_t count = 100) const;
    
    // Heap visualization
    std::string generateHeapMap() const;
    std::string generateAllocationHeatmap() const;
    
private:
    struct AllocationRecord {
        void* address;
        size_t size;
        const char* file;
        size_t line;
        std::string type;
        uint64_t timestamp;
        std::vector<MemoryRecord> stackTrace;
    };
    
    bool isProfiling_;
    std::vector<AllocationRecord> allocations_;
    mutable std::mutex mutex_;
    
    // Memory tracking thread
    std::thread trackingThread_;
    std::atomic<bool> stopTracking_;
    double trackingInterval_;
    
    void trackingWorker();
    
    // System memory info
    size_t getCurrentRSS() const;
    size_t getPeakRSS() const;
    std::unordered_map<std::string, size_t> getAllocationsByType() const;
};

/**
 * CPU profiler with call stack sampling
 */
class CPUProfiler {
public:
    CPUProfiler();
    ~CPUProfiler();
    
    // Start/stop profiling
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const { return isProfiling_; }
    
    // Sampling configuration
    void setSamplingInterval(double interval) { samplingInterval_ = interval; }
    void setSampleDepth(size_t depth) { sampleDepth_ = depth; }
    void enableSystemMetrics(bool enable) { systemMetricsEnabled_ = enable; }
    
    // Data access
    std::vector<CPUSnapshot> getCPUHistory(size_t count = 1000) const;
    CPUSnapshot getLatestSnapshot() const;
    
    // Analysis
    std::vector<ProfileFrame> getMostActiveFunctions(size_t count = 20) const;
    double getCPUUsage() const;
    double getInstructionPerSecond() const;
    double getCacheMissRate() const;
    
    // Performance metrics
    double calculateIPC() const;  // Instructions per cycle
    double calculateCPI() const;  // Cycles per instruction
    std::string generatePerformanceReport() const;
    
private:
    struct SampleRecord {
        uint64_t timestamp;
        uint64_t stepId;
        std::vector<ProfileFrame> callStack;
        double cpuTime;
        double instructionCount;
        double cacheMisses;
        std::string threadName;
    };
    
    bool isProfiling_;
    double samplingInterval_;
    size_t sampleDepth_;
    bool systemMetricsEnabled_;
    
    std::vector<SampleRecord> samples_;
    mutable std::mutex mutex_;
    
    // Sampling thread
    std::thread samplingThread_;
    std::atomic<bool> stopSampling_;
    
    // System sampling
    void samplingWorker();
    ProfileFrame sampleCurrentStack() const;
    uint64_t getCurrentInstructionPointer() const;
    
    // System metrics
    double getCPUUtilization() const;
    double getInstructionRate() const;
    double getCacheMissRate() const;
    std::string getThreadName() const;
};

/**
 * I/O profiler for disk and network operations
 */
class IOProfiler {
public:
    IOProfiler();
    ~IOProfiler();
    
    // Start/stop profiling
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const { return isProfiling_; }
    
    // Disk I/O tracking
    struct DiskOperation {
        uint64_t timestamp;
        std::string operation;  // Read, Write, ReadWrite
        size_t bytes;           // Number of bytes transferred
        double duration;        // Seconds
        std::string filePath;  // File being accessed
        uint64_t processId;     // Process ID
    };
    
    void recordDiskOperation(const std::string& operation, size_t bytes,
                            double duration, const std::string& filePath);
    std::vector<DiskOperation> getDiskOperations(size_t count = 1000) const;
    
    // Network I/O tracking
    struct NetworkOperation {
        uint64_t timestamp;
        std::string protocol;  // TCP, UDP, HTTP, etc.
        size_t bytesSent;      // Bytes sent
        size_t bytesReceived;  // Bytes received
        double duration;       // Seconds
        std::string remoteAddress;
        uint16_t remotePort;
    };
    
    void recordNetworkOperation(const std::string& protocol,
                               size_t bytesSent, size_t bytesReceived,
                               double duration, const std::string& remoteAddress,
                               uint16_t remotePort);
    std::vector<NetworkOperation> getNetworkOperations(size_t count = 1000) const;
    
    // Performance metrics
    struct IOStats {
        double diskReadThroughput;  // MB/s
        double diskWriteThroughput; // MB/s
        double networkUpload;       // Mbps
        double networkDownload;     // Mbps
        size_t diskIOPS;            // Operations per second
        size_t networkIOPS;         // Operations per second
        double averageReadLatency; // Milliseconds
        double averageWriteLatency;// Milliseconds
        double averageNetworkLatency; // Milliseconds
    };
    
    IOStats calculateIOStats(size_t historySeconds = 60) const;
    std::string generateIOReport() const;
    
private:
    bool isProfiling_;
    
    std::vector<DiskOperation> diskOperations_;
    std::vector<NetworkOperation> networkOperations_;
    mutable std::mutex mutex_;
    
    // File system monitoring
    void monitorFileSystem();
    void monitorNetwork();
    
    // System call interception (platform-specific)
    void setupSystemCallInterception();
    void cleanupSystemCallInterception();
};

/**
 * GPU profiler for accelerator-based computations
 */
class GPUProfiler {
public:
    GPUProfiler();
    ~GPUProfiler();
    
    // Start/stop profiling
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const { return isProfiling_; }
    
    // Kernel execution tracking
    struct KernelLaunch {
        uint64_t timestamp;
        std::string kernelName;
        size_t gridSize;          // Number of thread blocks
        size_t blockSize;         // Threads per block
        size_t sharedMemory;      // Shared memory per block (bytes)
        size_t registersPerThread; // Number of registers per thread
        double executionTime;     // Kernel execution time (seconds)
        size_t memoryTransferred; // Total memory transferred (bytes)
        std::string streamName;   // CUDA stream name
    };
    
    void recordKernelLaunch(const std::string& kernelName,
                           size_t gridSize, size_t blockSize,
                           size_t sharedMemory, size_t registersPerThread,
                           double executionTime, size_t memoryTransferred,
                           const std::string& streamName = "default");
    std::vector<KernelLaunch> getKernelLaunches(size_t count = 1000) const;
    
    // Memory operations tracking
    struct GPUMemoryOp {
        uint64_t timestamp;
        std::string operation;   // Allocate, Free, Copy, Memset
        size_t size;             // Number of bytes
        void* deviceAddress;     // CUDA device pointer
        void* hostAddress;       // Host pointer (for copy ops)
        double duration;         // Operation duration
        std::string streamName; // CUDA stream name
    };
    
    void recordMemoryOperation(const std::string& operation, size_t size,
                               void* deviceAddress, void* hostAddress,
                               double duration, const std::string& streamName);
    std::vector<GPUMemoryOp> getMemoryOperations(size_t count = 1000) const;
    
    // Performance metrics
    struct GPUStats {
        double utilization;           // GPU utilization percentage (0-100)
        double memoryBandwidthUsed;  // GB/s
        double computeCapacity;      // TFLOPS used / available
        size_t activeWarps;          // Number of active warps
        size_t schedulerBlocks;      // Number of blocks in scheduler
        double averageMemoryLatency; // ms
        double averageKernelLatency; // ms
        size_t totalKernelLaunches;  // Total kernel launches
    };
    
    GPUStats getGPUStats() const;
    std::string generateGPUReport() const;
    
    // Accelerator detection
    bool isGPUAvailable() const;
    std::string getGPUName() const;
    size_t getGPUComputeCapability() const;
    size_t getGPUGlobalMemory() const;
    
private:
    bool isProfiling_;
    
    std::vector<KernelLaunch> kernelLaunches_;
    std::vector<GPUMemoryOp> memoryOperations_;
    mutable std::mutex mutex_;
    
    // CUDA/NVIDIA API interception (platform-specific)
    void setupCUDAInterception();
    void cleanupCUDAInterception();
    
    // OpenCL interception (platform-specific)
    void setupOpenCLInterception();
    void cleanupOpenCLInterception();
};

/**
 * Real-time dashboard for performance monitoring
 */
class PerformanceDashboard {
public:
    PerformanceDashboard();
    ~PerformanceDashboard();
    
    // Update dashboard data
    void update(const PerformanceMetrics& metrics);
    void update(const CPUSnapshot& snapshot);
    void update(const MemoryRecord& record);
    void update(const ThreadRecord& record);
    
    // Dashboard generation
    std::string generateHTMLDashboard() const;
    std::string generateJSONMetrics() const;
    std::string generateRealtimeUpdates() const;
    
    // Visualization generation
    std::string generateFlameGraph() const;
    std::string generateMemoryHeatmap() const;
    std::string generateSystemOverview() const;
    
    // Alert management
    void addAlert(const std::string& metric, double value,
                  const std::string& severity, const std::string& message);
    std::vector<std::string> getAlerts() const { return alerts_; }
    void clearAlerts() { alerts_.clear(); }
    
private:
    PerformanceMetrics latestMetrics_;
    std::vector<CPUSnapshot> cpuSnapshots_;
    std::vector<MemoryRecord> memoryRecords_;
    std::vector<ThreadRecord> threadRecords_;
    
    std::vector<std::string> alerts_;
    mutable std::mutex mutex_;
    
    // HTML template helpers
    std::string generateMetricCard(const std::string& name,
                                   double value,
                                   const std::string& unit,
                                   const std::string& color = "blue") const;
    std::string generateChart(const std::vector<double>& data,
                              const std::vector<uint64_t>& timestamps,
                              const std::string& title) const;
};

/**
 * Performance regression testing
 */
class PerformanceRegressionTester {
public:
    PerformanceRegressionTester();
    ~PerformanceRegressionTester();
    
    // Benchmark definitions
    struct Benchmark {
        std::string name;
        size_t neurons;
        size_t synapses;
        size_t steps;
        double targetStepsPerSecond;
        double targetMemoryEfficiency;
        double targetAccuracy;
        std::function<bool(const PerformanceMetrics&)> validator;
    };
    
    // Benchmark management
    void addBenchmark(const Benchmark& benchmark);
    void removeBenchmark(const std::string& name);
    std::vector<Benchmark> getBenchmarks() const { return benchmarks_; }
    
    // Regression testing
    struct TestResult {
        std::string benchmarkName;
        uint64_t timestamp;
        PerformanceMetrics baseline;
        PerformanceMetrics current;
        bool passed;
        std::string details;
        double regressionRatio; // > 1.0 indicates regression
    };
    
    TestResult runBenchmark(const std::string& benchmarkName,
                           const PerformanceMetrics& currentMetrics);
    std::vector<TestResult> runAllBenchmarks(const PerformanceMetrics& currentMetrics);
    
    // Baseline management
    void setBaseline(const std::string& benchmarkName,
                     const PerformanceMetrics& baseline);
    PerformanceMetrics getBaseline(const std::string& benchmarkName) const;
    void clearBaseline(const std::string& benchmarkName);
    void clearAllBaselines() { baselines_.clear(); }
    
    // Report generation
    std::string generateRegressionReport(const std::vector<TestResult>& results) const;
    std::string generateTrendAnalysis(const std::string& benchmarkName,
                                      size_t historyPoints = 100) const;
    
private:
    std::vector<Benchmark> benchmarks_;
    std::unordered_map<std::string, PerformanceMetrics> baselines_;
    mutable std::mutex mutex_;
};

/**
 * Main performance profiling orchestrator
 */
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Component access
    ThreadProfiler& getThreadProfiler() { return threadProfiler_; }
    MemoryProfiler& getMemoryProfiler() { return memoryProfiler_; }
    CPUProfiler& getCPUProfiler() { return cpuProfiler_; }
    IOProfiler& getIOProfiler() { return ioProfiler_; }
    GPUProfiler& getGPUProfiler() { return gpuProfiler_; }
    PerformanceDashboard& getDashboard() { return dashboard_; }
    PerformanceRegressionTester& getRegressionTester() { return regressionTester_; }
    
    // Global control
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const { return isProfiling_; }
    
    // Integration with neural simulation
    void attachToSimulation(const std::string& simulationId);
    void detachFromSimulation();
    std::string getAttachedSimulationId() const { return attachedSimulationId_; }
    
    // Configuration
    void setSamplingInterval(double interval);
    void setMetricsLevel(size_t level);
    void enableComponent(const std::string& component, bool enable);
    bool isComponentEnabled(const std::string& component) const;
    
    // Reporting and analysis
    std::string generateComprehensiveReport() const;
    void exportProfileData(const std::string& filename) const;
    void importProfileData(const std::string& filename);
    
    // Real-time monitoring
    void startRealTimeDashboard(size_t port = 8080);
    void stopRealTimeDashboard();
    
private:
    bool isProfiling_;
    std::string attachedSimulationId_;
    
    ThreadProfiler threadProfiler_;
    MemoryProfiler memoryProfiler_;
    CPUProfiler cpuProfiler_;
    IOProfiler ioProfiler_;
    GPUProfiler gpuProfiler_;
    PerformanceDashboard dashboard_;
    PerformanceRegressionTester regressionTester_;
    
    std::unordered_map<std::string, bool> componentEnabled_;
    mutable std::mutex mutex_;
    
    // Data management
    void saveProfileData(std::ostream& output) const;
    void loadProfileData(std::istream& input);
};