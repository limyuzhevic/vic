#pragma once

/**
 * GPUProfiler - GPU acceleration profiling for CUDA, OpenCL, and other accelerators
 * 
 * This class provides detailed GPU profiling capabilities for:
 * - CUDA core profiling and occupancy analysis
 * - OpenCL kernel profiling and work-group analysis
 * - Memory bandwidth analysis
 * - Kernel execution time measurement
 * - GPU utilization monitoring
 * - Parallel computation profiling
 * - GPU optimization recommendations
 * 
 * Designed for profiling GPU-accelerated neural network computations
 * and other GPU-accelerated NLM operations
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
#include <set>

namespace nlm {

/**
 * GPU device type
 */
enum class GPUDeviceType {
    NVIDIA_CUDA,
    AMD_OPENCL,
    INTEL_ONEAPI,
    APPLE_METAL,
    ARM_MALI,
    QUALCOMM_ADRENO,
    UNKNOWN
};

/**
 * GPU memory type
 */
enum class GPUMemoryType {
    GLOBAL,
    CONSTANT,
    SHARED,
    LOCAL,
    TEXTURE,
    SURFACE,
    UNKNOWN
};

/**
 * CUDA compute capability
 */
struct CUDACapability {
    int major;
    int minor;
    uint64_t smxCount;          // number of SMX (streaming multiprocessor) units
    uint64_t cudaCoreCount;      // number of CUDA cores
    uint64_t warpSize;           // warp size (typically 32)
    uint64_t maxThreadsPerBlock; // maximum threads per block
    uint64_t maxThreadsPerSMX;   // maximum threads per SMX
    uint64_t sharedMemoryPerSMX; // shared memory per SMX (bytes)
    uint64_t registerPerThread;  // registers per thread
    uint64_t memoryClockRate;    // memory clock rate (Hz)
    uint64_t coreClockRate;      // core clock rate (Hz)
    
    CUDACapability() : major(0), minor(0), smxCount(0), cudaCoreCount(0),
                      warpSize(32), maxThreadsPerBlock(1024), maxThreadsPerSMX(2048),
                      sharedMemoryPerSMX(64 * 1024), registerPerThread(65536),
                      memoryClockRate(0), coreClockRate(0) {}
};

/**
 * GPU kernel execution record
 */
struct GPUTraceRecord {
    uint64_t traceId;
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    std::string kernelName;
    std::string kernelSignature;
    uint32_t gridSizeX;
    uint32_t gridSizeY;
    uint32_t gridSizeZ;
    uint32_t blockSizeX;
    uint32_t blockSizeY;
    uint32_t blockSizeZ;
    uint32_t sharedMemoryBytes;
    std::string compilerOptions;
    std::string codeVersion;
    std::chrono::duration<double> executionTime;      // in seconds
    double occupancy;                                 // percentage (0-100)
    uint64_t registersPerThread;                      // effective registers per thread
    uint64_t memoryTransactions;                      // number of memory transactions
    uint64_t memoryThroughput;                         // memory throughput (bytes/s)
    uint64_t arithmeticInstructions;                  // number of arithmetic instructions
    uint64_t memoryInstructions;                      // number of memory instructions
    double computationalIntensity;                    // operations per memory access
    GPUMemoryType memoryType;
    std::string memoryAccessPattern;
    std::unordered_map<std::string, double> metrics;
    std::string errorMessage;
    bool successful;
    
    GPUTraceRecord() : traceId(0), gridSizeX(1), gridSizeY(1), gridSizeZ(1),
                      blockSizeX(1), blockSizeY(1), blockSizeZ(1),
                      sharedMemoryBytes(0), executionTime(0.0), occupancy(0.0),
                      registersPerThread(0), memoryTransactions(0), memoryThroughput(0),
                      arithmeticInstructions(0), memoryInstructions(0), computationalIntensity(0.0),
                      memoryType(GPUMemoryType::UNKNOWN), successful(false) {}
};

/**
 * GPU memory usage record
 */
struct GPUMemoryRecord {
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    GPUMemoryType memoryType;
    uint64_t allocatedBytes;
    uint64_t usedBytes;
    uint64_t freeBytes;
    uint64_t totalBytes;
    double usagePercentage;
    uint64_t allocationCount;
    uint64_t deallocationCount;
    uint64_t memoryFragmentation;
    std::string allocationPattern;  // "random", "sequential", "bursty"
    std::unordered_map<std::string, uint64_t> allocationBreakdown;  // by kernel
    std::string deviceName;
    uint64_t deviceId;
    
    GPUMemoryRecord() : allocatedBytes(0), usedBytes(0), freeBytes(0), totalBytes(0),
                       usagePercentage(0.0), allocationCount(0), deallocationCount(0),
                       memoryFragmentation(0), deviceId(0) {}
};

/**
 * GPU utilization record
 */
struct GPUUtilizationRecord {
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    double cpuUtilization;           // CPU utilization by GPU driver (%)
    double gpuUtilization;           // GPU core utilization (%)
    double memoryUtilization;       // GPU memory utilization (%)
    double smUtilization;           // SM (streaming multiprocessor) utilization (%)
    double memoryBandwidthUtilization;  // memory bandwidth utilization (%)
    double computeCapability;        // effective compute capability
    std::chrono::duration<double> activeTime;   // time GPU was active
    std::chrono::duration<double> totalTime;   // total monitoring time
    double utilizationEfficiency;    // efficiency metric (0-1)
    std::unordered_map<std::string, double> performanceCounters;
    
    GPUUtilizationRecord() : cpuUtilization(0.0), gpuUtilization(0.0), memoryUtilization(0.0),
                            smUtilization(0.0), memoryBandwidthUtilization(0.0),
                            computeCapability(0.0), activeTime(0.0), totalTime(0.0),
                            utilizationEfficiency(0.0) {}
};

/**
 * GPU device information
 */
struct GPUDeviceInfo {
    uint64_t deviceId;
    std::string name;
    GPUDeviceType deviceType;
    CUDACapability cudaCapability;
    uint64_t globalMemory;            // global memory size (bytes)
    uint64_t sharedMemory;           // shared memory per block (bytes)
    uint64_t maxThreadsPerBlock;     // maximum threads per block
    uint64_t maxTextureSize1D;       // maximum 1D texture size
    uint64_t maxTextureSize2D;       // maximum 2D texture size
    uint64_t maxTextureSize3D;       // maximum 3D texture size
    uint32_t multiProcessorCount;    // number of multiprocessors
    uint32_t asyncEngineCount;       // number of async engines
    uint32_t unifiedAddressing;      // unified virtual addressing
    uint32_t concurrentKernels;      // concurrent kernels support
    uint32_t l2CacheSize;            // L2 cache size (bytes)
    uint32_t eccEnabled;            // ECC memory support
    std::string computeCapability;   // string representation
    std::string deviceArch;          // device architecture
    std::string pciBusId;            // PCI bus ID
    
    GPUDeviceInfo() : deviceId(0), globalMemory(0), sharedMemory(0),
                     maxThreadsPerBlock(0), maxTextureSize1D(0), maxTextureSize2D(0),
                     maxTextureSize3D(0), multiProcessorCount(0), asyncEngineCount(0),
                     unifiedAddressing(0), concurrentKernels(0), l2CacheSize(0),
                     eccEnabled(0) {}
};

/**
 * GPU optimization recommendation
 */
struct GPUOptimization {
    std::string recommendation;
    std::string optimizationType;    // "THREAD_BLOCK_SIZE", "SHARED_MEMORY", "MEMCPY", "KERNEL_LAUNCH"
    double potentialSpeedup;         // expected speedup factor
    double implementationCost;       // implementation complexity (0-10)
    double priority;                 // 0-100 priority score
    std::string targetKernel;        // target kernel for optimization
    std::vector<std::string> affectedComponents;
    
    GPUOptimization() : potentialSpeedup(1.0), implementationCost(0.0), priority(0.0) {}
};

/**
 * GPU profiler configuration
 */
struct GPUProfilerConfig {
    bool enableKernelProfiling;       // Enable kernel profiling
    bool enableMemoryProfiling;      // Enable memory profiling
    bool enableUtilizationMonitoring; // Enable GPU utilization monitoring
    bool enablePerformanceCounters;  // Enable performance counters
    bool enableOccupancyAnalysis;    // Enable occupancy analysis
    bool enableMemoryBandwidthAnalysis; // Enable memory bandwidth analysis
    double samplingInterval;         // monitoring interval (seconds)
    uint64_t maxTraceRecords;        // maximum trace records to store
    uint64_t maxMemoryRecords;       // maximum memory records to store
    std::string gpuVendorFilter;     // filter by GPU vendor
    std::string kernelNameFilter;    // filter by kernel name pattern
    std::string outputDirectory;    // output directory for results
    bool exportToJSON;               // export results to JSON
    bool exportToCSV;                // export results to CSV
    
    GPUProfilerConfig() : enableKernelProfiling(true), enableMemoryProfiling(true),
                         enableUtilizationMonitoring(true), enablePerformanceCounters(true),
                         enableOccupancyAnalysis(true), enableMemoryBandwidthAnalysis(true),
                         samplingInterval(1.0), maxTraceRecords(100000), maxMemoryRecords(10000),
                         gpuVendorFilter(""), kernelNameFilter(""), outputDirectory("./gpu_profiling"),
                         exportToJSON(true), exportToCSV(true) {}
};

/**
 * GPU profiler for comprehensive GPU acceleration tracking and analysis
 */
class GPUProfiler {
public:
    GPUProfiler();
    ~GPUProfiler();
    
    // Disable copying
    GPUProfiler(const GPUProfiler&) = delete;
    GPUProfiler& operator=(const GPUProfiler&) = delete;
    
    /**
     * Initialize the GPU profiler
     */
    bool initialize();
    
    /**
     * Shutdown the GPU profiler
     */
    void shutdown();
    
    /**
     * Record GPU kernel execution trace
     */
    void recordKernelTrace(const GPUTraceRecord& trace);
    
    /**
     * Record GPU memory usage
     */
    void recordMemoryUsage(const GPUMemoryRecord& memory);
    
    /**
     * Record GPU utilization
     */
    void recordUtilization(const GPUUtilizationRecord& utilization);
    
    /**
     * Get all kernel traces
     */
    std::vector<GPUTraceRecord> getKernelTraces() const;
    
    /**
     * Get memory usage records
     */
    std::vector<GPUMemoryRecord> getMemoryRecords() const;
    
    /**
     * Get utilization records
     */
    std::vector<GPUUtilizationRecord> getUtilizationRecords() const;
    
    /**
     * Get GPU device information
     */
    std::vector<GPUDeviceInfo> getDeviceInfo() const;
    
    /**
     * Analyze kernel performance
     */
    std::unordered_map<std::string, double> analyzeKernelPerformance() const;
    
    /**
     * Analyze memory usage patterns
     */
    std::unordered_map<std::string, double> analyzeMemoryUsage() const;
    
    /**
     * Analyze GPU utilization
     */
    std::unordered_map<std::string, double> analyzeUtilization() const;
    
    /**
     * Generate kernel performance report
     */
    std::string generateKernelReport() const;
    
    /**
     * Generate memory usage report
     */
    std::string generateMemoryReport() const;
    
    /**
     * Generate utilization report
     */
    std::string generateUtilizationReport() const;
    
    /**
     * Generate device information report
     */
    std::string generateDeviceReport() const;
    
    /**
     * Export GPU data to JSON
     */
    std::string exportToJSON() const;
    
    /**
     * Export GPU data to CSV
     */
    std::string exportToCSV() const;
    
    /**
     * Generate comprehensive GPU report
     */
    std::string generateReport() const;
    
    /**
     * Reset all GPU profiling data
     */
    void reset();
    
    /**
     * Get profiler instance (singleton)
     */
    static GPUProfiler& getInstance();
    
    /**
     * Configure GPU profiler
     */
    void configure(const GPUProfilerConfig& config) {
        config_ = config;
    }
    
    /**
     * Get current configuration
     */
    GPUProfilerConfig getConfig() const { return config_; }
    
    /**
     * Get top kernels by execution time
     */
    std::vector<std::pair<std::string, double>> getTopKernelsByTime(size_t limit = 10) const;
    
    /**
     * Get GPU utilization statistics
     */
    std::unordered_map<std::string, double> getUtilizationStatistics() const;
    
    /**
     * Get memory usage statistics
     */
    std::unordered_map<std::string, uint64_t> getMemoryStatistics() const;
    
    /**
     * Get performance counter values
     */
    std::unordered_map<std::string, double> getPerformanceCounters() const;
    
    /**
     * Calculate average occupancy
     */
    double getAverageOccupancy() const;
    
    /**
     * Get total GPU execution time
     */
    double getTotalExecutionTime() const;
    
    /**
     * Check for GPU bottlenecks
     */
    std::vector<std::string> detectBottlenecks() const;
    
    /**
     * Get optimization recommendations
     */
    std::vector<GPUOptimization> getOptimizationRecommendations() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    GPUProfilerConfig config_;
    
    /**
     * Clean up old records
     */
    void cleanupOldRecords();
    
    /**
     * Calculate kernel occupancy
     */
    double calculateOccupancy(const GPUTraceRecord& trace, const CUDACapability& capability) const;
    
    /**
     * Calculate computational intensity
     */
    double calculateComputationalIntensity(const GPUTraceRecord& trace) const;
    
    /**
     * Detect memory bandwidth bottlenecks
     */
    bool detectMemoryBandwidthBottleneck(const GPUTraceRecord& trace) const;
    
    /**
     * Generate device information
     */
    void gatherDeviceInfo();
    
    /**
     * Calculate utilization efficiency
     */
    double calculateUtilizationEfficiency(const GPUUtilizationRecord& utilization) const;
    
    /**
     * Analyze performance counter trends
     */
    void analyzePerformanceCounterTrends();
};

} // namespace nlm

#endif // NLM_GPU_PROFILER_HPP