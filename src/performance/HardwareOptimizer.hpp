#pragma once

/**
 * HardwareOptimizer - Hardware-specific optimizations for NLM
 * 
 * Provides comprehensive hardware-specific optimizations including:
 * - CPU architecture detection and optimization
 * - SIMD instruction set utilization
 * - Cache optimization
 * - Memory bandwidth optimization
 * - Thread affinity and scheduling
 * - Hardware performance counters
 * - NUMA awareness
 * - Power and thermal management
 * - GPU acceleration (if available)
 * - I/O optimization
 * - Hardware-specific tuning parameters
 * 
 * Features:
 * - Automatic hardware detection and optimization
 * - Hardware-specific code generation
 * - Performance tuning based on hardware capabilities
 * - Power-efficient execution
 * - Thermal management
 * - Resource-aware scheduling
 * - Hardware counter profiling
 * - NUMA-aware memory allocation
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
#include <cstring>
#include <cpuid.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <smmintrin.h>
#include <nmmintrin.h>
#include <immintrin.h>

namespace nlm {

// Hardware capability flags
struct HardwareCapabilities {
    bool sse1;                           // SSE1 (MMX)
    bool sse2;                           // SSE2
    bool sse3;                           // SSE3
    bool ssse3;                          // Supplemental SSE3
    bool sse41;                          // SSE4.1
    bool sse42;                          // SSE4.2
    bool avx;                            // AVX
    bool avx2;                           // AVX2
    bool avx512f;                        // AVX512 Foundation
    bool avx512dq;                       // AVX512 Double/Quadword
    bool avx512bw;                       // AVX512 Byte/Word
    bool avx512vl;                       // AVX512 Vector Length
    bool avx512vbmi;                     // AVX512 VBMI
    bool avx512ifma;                     // AVX512 IFMA
    bool avx512vnni;                     // AVX512 VNNI
    bool fma3;                           // FMA3
    bool fma4;                           // FMA4
    bool xsave;                          // XSAVE/XRSTOR
    bool fxsr;                           // FXSAVE/FXRESTOR
    bool osxsave;                        // OSXSAVE
    bool sse4a;                          // SSE4a
    bool aesni;                          // AES-NI
    bool rdrand;                         // RDRAND
    bool rdseed;                         // RDSEED
    bool bmi1;                           // BMI1
    bool bmi2;                           // BMI2
    bool tbm;                            // TBM
    bool lzcnt;                          // LZCNT
    bool prefetchwt1;                    // PREFETCHWT1
    bool prefetchwt2;                    // PREFETCHWT2
    bool clflushopt;                     // CLFLUSHOPT
    bool clwb;                           // CLWB
    bool avx512vnniint16;                // AVX512 VNNI INT16
    bool avx512bf16;                     // AVX512 BF16
    bool avx512fp16;                     // AVX512 FP16
    bool amxbf16;                        // AMX BF16
    bool amxint8;                        // AMX INT8
    bool amxtile;                        // AMX Tile
    bool avxvnni;                        // AVX VNNI
    bool avxvnniint8;                    // AVX VNNI INT8
    bool avx512vpopcntdq;                // AVX512 VPOPCNTDQ
    bool avx512bitalg;                   // AVX512 BitAlg
    bool avx512vpclmulqdq;               // AVX512 VPCMULQDQ
    bool avx512vaes;                     // AVX512 VAES
    bool avx512vngmix;                   // AVX512 VNGMIX
    bool avx512vpermd;                   // AVX512 VPERMD
    bool avx512vpermq;                   // AVX512 VPERMQ
    bool avx512vpsrdq;                   // AVX512 VPSRDQ
    bool avx512vpsraq;                   // AVX512 VPSRAQ
    bool avx512vpsrlq;                   // AVX512 VPSRLQ
    bool avx512vpsraq;                   // AVX512 VPSRAQ
    bool avx512vpsrlq;                   // AVX512 VPSRLQ
    bool avx512vpsraq;                   // AVX512 VPSRAQ
    bool avx512vpsrlq;                   // AVX512 VPSRLQ
    bool avx512vpsraq;                   // AVX512 VPSRAQ
    bool avx512vpsrlq;                   // AVX512 VPSRLQ
    
    // Hardware information
    uint32_t physicalCores;              // Number of physical cores
    uint32_t logicalCores;               // Number of logical cores
    uint32_t threadsPerCore;             // Threads per core
    uint64_t l1CacheSize;                // L1 cache size (bytes)
    uint64_t l2CacheSize;                // L2 cache size (bytes)
    uint64_t l3CacheSize;                // L3 cache size (bytes)
    uint64_t l4CacheSize;                // L4 cache size (bytes)
    uint64_t ramSize;                    // RAM size (bytes)
    uint64_t frequency;                  // CPU frequency (Hz)
    std::string brandString;             // CPU brand string
    std::string modelName;               // CPU model name
    
    // NUMA information
    bool numaEnabled;                    // NUMA enabled
    uint32_t numaNodes;                  // Number of NUMA nodes
    std::vector<uint64_t> nodeMemory;    // Memory per node (bytes)
    std::vector<std::vector<uint32_t>> cpuToNode; // CPU to node mapping
};

// Cache line information
struct CacheInfo {
    uint64_t lineSize;                   // Cache line size (bytes)
    uint64_t l1Size;                     // L1 cache size (bytes)
    uint32_t l1Associativity;            // L1 associativity
    uint64_t l2Size;                     // L2 cache size (bytes)
    uint32_t l2Associativity;            // L2 associativity
    uint64_t l3Size;                     // L3 cache size (bytes)
    uint32_t l3Associativity;            // L3 associativity
    bool inclusive;                      // Whether cache is inclusive
    bool writeBack;                      // Write back policy
};

// Memory information
struct MemoryInfo {
    uint64_t totalBytes;                 // Total memory (bytes)
    uint64_t availableBytes;             // Available memory (bytes)
    uint64_t swapTotal;                  // Swap total (bytes)
    uint64_t swapUsed;                   // Swap used (bytes)
    std::vector<uint64_t> numaNodes;    // Memory per NUMA node (bytes)
    double pageSize;                     // Page size (bytes)
    bool largePages;                     // Large pages supported
    bool transparentHugePages;          // Transparent huge pages
    bool hugepagesSupported;             // Huge pages supported
    uint64_t hugePageSize;               // Huge page size (bytes)
};

// System information
struct SystemInfo {
    std::string os;                      // Operating system
    std::string osVersion;               // OS version
    std::string architecture;            // Architecture (x86_64, arm64, etc.)
    std::string cpuVendor;               // CPU vendor
    std::string cpuModel;                // CPU model
    std::string compiler;                // Compiler version
    std::string buildInfo;               // Build information
    std::string cpuMicroarchitecture;    // CPU microarchitecture
    uint32_t cpuStepping;                // CPU stepping
    uint32_t cpuModelNumber;             // CPU model number
    uint32_t cpuFamily;                  // CPU family
    uint32_t cpuMaxCpuidLevel;           // Maximum CPUID level
    bool virtualization;                // Virtualization support
    std::string virtualizationType;     // Virtualization type
};

/**
 * Hardware information detector
 */
class HardwareDetector {
public:
    HardwareDetector() = default;
    ~HardwareDetector() = default;
    
    /**
     * Detect hardware capabilities
     */
    HardwareCapabilities detectCapabilities() const;
    
    /**
     * Detect cache information
     */
    CacheInfo detectCacheInfo() const;
    
    /**
     * Detect memory information
     */
    MemoryInfo detectMemoryInfo() const;
    
    /**
     * Detect system information
     */
    SystemInfo detectSystemInfo() const;
    
    /**
     * Get CPU brand string
     */
    static std::string getCPUBrandString();
    
    /**
     * Get CPU frequency
     */
    static uint64_t getCPUFrequency();
    
    /**
     * Get number of logical cores
     */
    static uint32_t getLogicalCores();
    
    /**
     * Get number of physical cores
     */
    static uint32_t getPhysicalCores();
    
    /**
     * Check if CPUID is available
     */
    static bool isCPUIDAvailable();
    
    /**
     * Run CPUID instruction
     */
    static void cpuid(uint32_t eax, uint32_t ecx, uint32_t& eaxOut, uint32_t& ebxOut, uint32_t& ecxOut, uint32_t& edxOut);
    
    /**
     * Check if specific instruction is available
     */
    static bool hasInstruction(const std::string& instruction) { return checkInstructionAvailability(instruction); }

private:
    /**
     * Check instruction availability using CPUID
     */
    static bool checkInstructionAvailability(const std::string& instruction);
};

/**
 * Cache optimizer
 */
class CacheOptimizer {
public:
    struct CacheLine {
        uint64_t address;                  // Memory address (aligned)
        uint32_t size;                     // Size (bytes)
        uint32_t accessCount;              // Access count
        std::chrono::nanoseconds lastAccess; // Last access time
        uint32_t coreId;                    // Core ID (for per-core tracking)
    };
    
    CacheOptimizer() = default;
    ~CacheOptimizer() = default;
    
    /**
     * Optimize data layout for cache efficiency
     */
    template<typename T>
    static void optimizeForCache(T* data, size_t count, size_t stride = 1) {
        // Ensure data is cache-aligned
        alignas(CACHE_ALIGNMENT) T* alignedData = reinterpret_cast<T*>(
            (reinterpret_cast<uintptr_t>(data) + CACHE_ALIGNMENT - 1) & ~(CACHE_ALIGNMENT - 1)
        );
        std::memcpy(alignedData, data, count * sizeof(T));
    }
    
    /**
     * Prefetch data for cache
     */
    template<typename T>
    static void prefetch(const T* data, size_t count, int locality = 3) {
        #if defined(__x86_64__) || defined(_M_X64)
        for (size_t i = 0; i < count; ++i) {
            switch (locality) {
                case 0: _mm_prefetch((const char*)&data[i], _MM_HINT_T0); break;
                case 1: _mm_prefetch((const char*)&data[i], _MM_HINT_T1); break;
                case 2: _mm_prefetch((const char*)&data[i], _MM_HINT_T2); break;
                case 3: _mm_prefetch((const char*)&data[i], _MM_HINT_NTA); break;
            }
        }
        #endif
    }
    
    /**
     * Get optimal cache line size
     */
    static constexpr uint64_t getCacheLineSize() { return CACHE_LINE_SIZE; }
    
    /**
     * Get recommended cache alignment
     */
    static constexpr uint64_t getAlignment() { return CACHE_ALIGNMENT; }
    
    /**
     * Calculate cache set index
     */
    static uint64_t calculateCacheSet(uint64_t address) {
        return (address / CACHE_LINE_SIZE) & CACHE_SET_MASK;
    }
    
    /**
     * Generate cache-friendly indices
     */
    template<typename T>
    static std::vector<size_t> generateCacheFriendlyIndices(size_t size, size_t cacheSize) {
        std::vector<size_t> indices;
        indices.reserve(size);
        
        // Generate indices that avoid cache conflicts
        for (size_t i = 0; i < size; ++i) {
            // Use stride to avoid cache conflicts
            indices.push_back((i * CACHE_STRIDE) % size);
        }
        
        return indices;
    }

private:
    static constexpr uint64_t CACHE_LINE_SIZE = 64;
    static constexpr uint64_t CACHE_ALIGNMENT = 64;
    static constexpr uint64_t CACHE_SET_MASK = (1ULL << 20) - 1;  // For 2^20 cache sets
    static constexpr uint64_t CACHE_STRIDE = 8;  // Stride to avoid conflicts
};

/**
 * Memory bandwidth optimizer
 */
class MemoryBandwidthOptimizer {
public:
    struct MemoryAccessPattern {
        uint32_t readCount;                // Number of reads
        uint32_t writeCount;               // Number of writes
        uint64_t readBytes;                // Read bytes
        uint64_t writeBytes;               // Write bytes
        std::chrono::nanoseconds duration; // Duration
        double bandwidth;                  // Bandwidth (GB/s)
    };
    
    MemoryBandwidthOptimizer() = default;
    ~MemoryBandwidthOptimizer() = default;
    
    /**
     * Measure memory bandwidth
     */
    static double measureBandwidth(const void* data, size_t size, 
                                   bool read = true, bool write = false,
                                   uint32_t iterations = 100);
    
    /**
     * Optimize for streaming access pattern
     */
    template<typename T>
    static void optimizeStreamingAccess(T* data, size_t size) {
        // Use aligned memory allocation
        alignas(CACHE_ALIGNMENT) T* aligned = reinterpret_cast<T*>(
            (reinterpret_cast<uintptr_t>(data) + CACHE_ALIGNMENT - 1) & ~(CACHE_ALIGNMENT - 1)
        );
        std::memcpy(aligned, data, size * sizeof(T));
        
        // Ensure data is accessed sequentially
        for (size_t i = 0; i < size; ++i) {
            // Compiler should optimize this to sequential access
            T temp = aligned[i];
            // Use temp to prevent optimization removal
            volatile T unused = temp;
        }
    }
    
    /**
     * Optimize for random access pattern
     */
    template<typename T>
    static void optimizeRandomAccess(T* data, size_t size) {
        // Use smaller working set to fit in cache
        size_t workingSetSize = std::min<size_t>(CACHE_L1_SIZE, size);
        for (size_t i = 0; i < size; ++i) {
            // Access data with pattern that may fit in cache
            size_t index = i % workingSetSize;
            T temp = data[index];
            volatile T unused = temp;
        }
    }
    
    /**
     * Optimize for strided access
     */
    template<typename T>
    static void optimizeStridedAccess(T* data, size_t size, size_t stride) {
        for (size_t i = 0; i < size; i += stride) {
            T temp = data[i];
            volatile T unused = temp;
        }
    }
    
    /**
     * Get L1 cache size
     */
    static uint64_t getL1CacheSize() { return L1_CACHE_SIZE; }
    
    /**
     * Get L2 cache size
     */
    static uint64_t getL2CacheSize() { return L2_CACHE_SIZE; }
    
    /**
     * Get L3 cache size
     */
    static uint64_t getL3CacheSize() { return L3_CACHE_SIZE; }

private:
    static constexpr uint64_t L1_CACHE_SIZE = 32 * 1024;  // 32KB L1
    static constexpr uint64_t L2_CACHE_SIZE = 256 * 1024; // 256KB L2
    static constexpr uint64_t L3_CACHE_SIZE = 8 * 1024 * 1024; // 8MB L3
    static constexpr uint64_t CACHE_ALIGNMENT = 64;
};

/**
 * Thread scheduling optimizer
 */
class ThreadSchedulerOptimizer {
public:
    struct ThreadInfo {
        uint32_t threadId;                   // Thread ID
        uint32_t cpuId;                      // CPU ID (for binding)
        std::chrono::nanoseconds cpuTime;    // CPU time
        std::chrono::nanoseconds waitTime;   // Wait time
        double utilization;                  // CPU utilization
        std::string affinity;                // CPU affinity string
        bool isPinned;                       // Whether thread is CPU-pinned
    };
    
    ThreadSchedulerOptimizer() = default;
    ~ThreadSchedulerOptimizer() = default;
    
    /**
     * Get number of available logical CPUs
     */
    static uint32_t getLogicalCPUs();
    
    /**
     * Get number of physical cores
     */
    static uint32_t getPhysicalCores();
    
    /**
     * Get CPU affinity mask for thread
     */
    static uint64_t getThreadAffinityMask(uint32_t threadId);
    
    /**
     * Set thread affinity
     */
    static bool setThreadAffinity(uint32_t threadId, uint32_t cpuId);
    
    /**
     * Pin thread to specific CPU
     */
    static bool pinThreadToCPU(uint32_t threadId, uint32_t cpuId);
    
    /**
     * Get NUMA node for CPU
     */
    static uint32_t getNUMANodeForCPU(uint32_t cpuId);
    
    /**
     * Get NUMA node for thread
     */
    static uint32_t getNUMANodeForThread(uint32_t threadId);
    
    /**
     * Optimize thread placement for cache locality
     */
    static void optimizeThreadPlacement(std::vector<std::function<void()>>& threads,
                                         uint32_t numThreads = 0);
    
    /**
     * Get CPU load information
     */
    static std::vector<double> getCPULoad();
    
    /**
     * Get thread information
     */
    static std::vector<ThreadInfo> getThreadInfo();

private:
    static constexpr uint32_t MAX_CPU_IDS = 1024;
};

/**
 * Hardware performance counters
 */
class HardwarePerformanceCounters {
public:
    struct PerformanceCounter {
        std::string name;                   // Counter name
        uint64_t value;                     // Counter value
        double rate;                        // Rate (per second)
        std::chrono::nanoseconds timestamp; // Timestamp
        uint32_t coreId;                    // Core ID (if applicable)
        uint32_t threadId;                  // Thread ID (if applicable)
        std::unordered_map<std::string, uint64_t> rawValues; // Raw counter values
    };
    
    HardwarePerformanceCounters() = default;
    ~HardwarePerformanceCounters() = default;
    
    /**
     * Start performance counter monitoring
     */
    void startMonitoring(const std::vector<std::string>& counters);
    
    /**
     * Stop performance counter monitoring
     */n     void stopMonitoring();
    
    /**
     * Get current counter values
     */
    std::vector<PerformanceCounter> getCounters() const;
    
    /**
     * Get counter delta since last measurement
     */
    std::vector<PerformanceCounter> getCounterDeltas() const;
    
    /**
     * Reset counters
     */
    void resetCounters();
    
    /**
     * Is monitoring active
     */
    bool isMonitoring() const { return monitoring_; }
    
    /**
     * Get available counters
     */
    static std::vector<std::string> getAvailableCounters();
    
    /**
     * Get counter types
     */
    enum class CounterType {
        Cycles,
        Instructions,
        BranchInstructions,
        BranchMisses,
        L1CacheReferences,
        L1CacheMisses,
        L2CacheReferences,
        L2CacheMisses,
        L3CacheReferences,
        L3CacheMisses,
        MemoryLoads,
        MemoryStores,
        StoreForwardStalls,
        LoadStoreFwdStall,
        IntegerArithmetic,
        FloatingPoint,
        SIMDInstructions,
        CacheReferences,
        CacheMisses,
        LoadMicroOps,
        StoreMicroOps,
        RetireMicroOps,
        TotalCycles,
        TotalInstructions,
        CoreCycles,
        RefCpuCycles,
        CoreInstructions,
        RefCpuInstructions,
        Custom
    };

private:
    mutable std::mutex countersMutex_;
    std::vector<PerformanceCounter> counters_;
    std::vector<PerformanceCounter> previousCounters_;
    bool monitoring_;
    std::chrono::high_resolution_clock::time_point lastUpdate_;
    std::thread monitoringThread_;
};

/**
 * NUMA optimizer
 */
class NUMAOptimizer {
public:
    struct NUMANode {
        uint32_t nodeId;                    // Node ID
        std::vector<uint32_t> cpuIds;      // CPU IDs in this node
        uint64_t memory;                    // Memory available (bytes)
        double cpuUsage;                    // CPU usage (%)
        std::vector<uint64_t> localMemories; // Local memory for each CPU
    };
    
    NUMAOptimizer() = default;
    ~NUMAOptimizer() = default;
    
    /**
     * Get NUMA node information
     */
    static std::vector<NUMANode> getNUMANodes();
    
    /**
     * Get NUMA node for CPU ID
     */
    static uint32_t getNUMANodeForCPU(uint32_t cpuId);
    
    /**
     * Get NUMA node for memory address
     */
    static uint32_t getNUMANodeForAddress(const void* ptr);
    
    /**
     * Allocate memory on specific NUMA node
     */
    static void* allocateMemoryOnNode(size_t size, uint32_t nodeId);
    
    /**
     * Free NUMA-allocated memory
     */
    static void freeMemoryOnNode(void* ptr);
    
    /**
     * Check if memory is local to CPU
     */
    static bool isMemoryLocal(const void* ptr, uint32_t cpuId);
    
    /**
     * Optimize memory access pattern for NUMA
     */
    template<typename T>
    static void optimizeForNUMA(std::vector<T>& data, uint32_t cpuId) {
        uint32_t numaNode = getNUMANodeForCPU(cpuId);
        void* ptr = data.data();
        // Ensure data is allocated on local NUMA node
        if (!isMemoryLocal(ptr, cpuId)) {
            // Reallocate on local node
            void* localPtr = allocateMemoryOnNode(data.size() * sizeof(T), numaNode);
            std::memcpy(localPtr, data.data(), data.size() * sizeof(T));
            freeMemoryOnNode(data.data());
            // Note: This is a conceptual implementation
            // Actual implementation would need to handle data pointer changes
        }
    }
    
    /**
     * Get preferred CPU for NUMA optimization
     */
    static uint32_t getPreferredCPU();
    
    /**
     * Is NUMA enabled
     */
    static bool isNUMAEnabled();

private:
    static constexpr uint32_t MAX_NUMA_NODES = 64;
};

/**
 * Power and thermal optimizer
 */
class PowerThermalOptimizer {
public:
    struct PowerStats {
        double cpuPower;                     // CPU power (Watts)
        double gpuPower;                     // GPU power (Watts)
        double totalPower;                   // Total power (Watts)
        double temperature;                  // Current temperature (°C)
        double maxTemperature;              // Maximum allowed temperature (°C)
        double thermalThrottling;           // Thermal throttling (%)
        uint32_t powerLimit;                 // Power limit (Watts)
        uint32_t sustainedPowerLimit;       // Sustained power limit (Watts)
        uint32_t transientPowerLimit;       // Transient power limit (Watts)
    };
    
    PowerThermalOptimizer() = default;
    ~PowerThermalOptimizer() = default;
    
    /**
     * Get power statistics
     */
    PowerStats getPowerStats() const;
    
    /**
     * Set power limit
     */
    static bool setPowerLimit(uint32_t powerLimitWatts);
    
    /**
     * Set sustained power limit
     */
    static bool setSustainedPowerLimit(uint32_t powerLimitWatts);
    
    /**
     * Set transient power limit
     */
    static bool setTransientPowerLimit(uint32_t powerLimitWatts);
    
    /**
     * Get current temperature
     */
    static double getTemperature();
    
    /**
     * Check for thermal throttling
     */
    static bool isThermalThrottling();
    
    /**
     * Optimize for power efficiency
     */
    static void optimizeForPowerEfficiency();
    
    /**
     * Optimize for performance
     */
    static void optimizeForPerformance();
    
    /**
     * Monitor power usage
     */
    void startPowerMonitoring(uint32_t intervalMs = 1000);
    
    /**
     * Stop power monitoring
     */
    void stopPowerMonitoring();
    
    /**
     * Get power monitoring history
     */
    std::vector<PowerStats> getPowerHistory() const;

private:
    mutable std::mutex statsMutex_;
    std::vector<PowerStats> powerHistory_;
    bool monitoring_;
    std::thread monitoringThread_;
};

/**
 * I/O optimizer
 */
class IOOptimizer {
public:
    struct IOStats {
        std::string device;                  // Device name
        uint64_t readOperations;             // Read operations
        uint64_t writeOperations;            // Write operations
        uint64_t readBytes;                  // Read bytes
        uint64_t writeBytes;                 // Write bytes
        double readThroughput;              // Read throughput (MB/s)
        double writeThroughput;             // Write throughput (MB/s)
        double readLatency;                  // Read latency (ms)
        double writeLatency;                 // Write latency (ms)
        uint32_t readErrors;                // Read errors
        uint32_t writeErrors;               // Write errors
        uint32_t queueDepth;                 // Queue depth
        double queueUtilization;             // Queue utilization (%)
    };
    
    IOOptimizer() = default;
    ~IOOptimizer() = default;
    
    /**
     * Get I/O statistics for device
     */
    IOStats getIOStats(const std::string& device) const;
    
    /**
     * Get I/O statistics for all devices
     */
    std::vector<IOStats> getAllIOStats() const;
    
    /**
     * Optimize for sequential I/O
     */
    static void optimizeSequentialIO(const std::string& filepath);
    
    /**
     * Optimize for random I/O
     */
    static void optimizeRandomIO(const std::string& filepath);
    
    /**
     * Set I/O priority
     */
    static bool setIOPriority(const std::string& processName, int priority);
    
    /**
     * Get I/O scheduler
     */
    static std::string getIOScheduler(const std::string& device);
    
    /**
     * Set I/O scheduler
     */
    static bool setIOScheduler(const std::string& device, const std::string& scheduler);

private:
    mutable std::mutex statsMutex_;
};

/**
 * Hardware-specific optimizer - Main class
 */
class HardwareOptimizer {
public:
    struct Config {
        bool enableCacheOptimization;         // Enable cache optimization
        bool enableMemoryOptimization;       // Enable memory optimization
        bool enableThreadOptimization;       // Enable thread optimization
        bool enableNUMAOptimization;         // Enable NUMA optimization
        bool enablePowerOptimization;        // Enable power optimization
        bool enableThermalOptimization;      // Enable thermal optimization
        bool enableIOMOptimization;          // Enable I/O optimization
        bool enableHardwareCounters;         // Enable hardware counters
        std::string optimizationLevel;       // Optimization level (none, basic, advanced, extreme)
        uint32_t threadCount;                // Number of threads to use
        std::string cpuAffinity;             // CPU affinity string
        std::string numaPolicy;              // NUMA policy (interleave, prefer, bind)
        uint32_t powerLimit;                 // Power limit (Watts)
        double thermalLimit;                 // Thermal limit (°C)
        std::unordered_map<std::string, std::string> customOptimizations; // Custom optimizations
    };
    
    HardwareOptimizer(const Config& config);
    ~HardwareOptimizer() = default;
    
    /**
     * Configure optimizer
     */
    void configure(const Config& config);
    
    /**
     * Optimize memory allocation
     */
    template<typename T>
    T* optimizeMemoryAllocation(T* ptr, size_t size) {
        // Optimize for cache line alignment
        if (config_.enableCacheOptimization) {
            alignas(CACHE_ALIGNMENT) uint8_t* aligned = reinterpret_cast<uint8_t*>(
                (reinterpret_cast<uintptr_t>(ptr) + CACHE_ALIGNMENT - 1) & ~(CACHE_ALIGNMENT - 1)
            );
            return reinterpret_cast<T*>(aligned);
        }
        return ptr;
    }
    
    /**
     * Optimize data layout
     */
    template<typename T>
    void optimizeDataLayout(std::vector<T>& data, size_t workingSetSize = 0) {
        if (config_.enableCacheOptimization) {
            // Ensure data is aligned and cache-friendly
            size_t alignedSize = (data.size() + CACHE_LINE_SIZE - 1) / CACHE_LINE_SIZE * CACHE_LINE_SIZE;
            std::vector<T> alignedData;
            alignedData.reserve(alignedSize);
            alignedData.insert(alignedData.end(), data.begin(), data.end());
            alignedData.resize(alignedSize, T{});
            data = std::move(alignedData);
        }
    }
    
    /**
     * Optimize thread placement
     */
    void optimizeThreadPlacement(std::vector<std::thread>& threads);
    
    /**
     * Get hardware-specific tuning parameters
     */
    std::unordered_map<std::string, double> getTuningParameters() const;
    
    /**
     * Apply hardware-specific optimizations
     */
    void applyOptimizations();
    
    /**
     * Get hardware information
     */
    HardwareCapabilities getHardwareCapabilities() const { return hardwareDetector_.detectCapabilities(); }
    
    /**
     * Get system information
     */
    SystemInfo getSystemInfo() const { return hardwareDetector_.detectSystemInfo(); }
    
    /**
     * Is optimization enabled
     */
    bool isOptimizationEnabled(const std::string& optimization) const {
        return hardwareOptimizations_.find(optimization) != hardwareOptimizations_.end();
    }
    
    /**
     * Enable optimization
     */
    void enableOptimization(const std::string& optimization) {
        hardwareOptimizations_.insert(optimization);
    }
    
    /**
     * Disable optimization
     */
    void disableOptimization(const std::string& optimization) {
        hardwareOptimizations_.erase(optimization);
    }
    
    /**
     * Get optimization status
     */
    std::unordered_set<std::string> getOptimizationStatus() const { return hardwareOptimizations_; }

private:
    Config config_;
    HardwareDetector hardwareDetector_;
    CacheOptimizer cacheOptimizer_;
    MemoryBandwidthOptimizer memoryOptimizer_;
    ThreadSchedulerOptimizer threadSchedulerOptimizer_;
    NUMAOptimizer numaOptimizer_;
    PowerThermalOptimizer powerThermalOptimizer_;
    IOOptimizer ioOptimizer_;
    HardwarePerformanceCounters performanceCounters_;
    
    std::unordered_set<std::string> hardwareOptimizations_;
    mutable std::mutex optimizationsMutex_;
    
    /**
     * Apply cache optimization
     */
    void applyCacheOptimization();
    
    /**
     * Apply memory optimization
     */
    void applyMemoryOptimization();
    
    /**
     * Apply thread optimization
     */
    void applyThreadOptimization();
    
    /**
     * Apply NUMA optimization
     */
    void applyNUMAOptimization();
    
    /**
     * Apply power optimization
     */
    void applyPowerOptimization();
    
    /**
     * Apply thermal optimization
     */
    void applyThermalOptimization();
    
    /**
     * Apply I/O optimization
     */
    void applyVIOOptimization();
    
    /**
     * Get cache optimization level
     */
    static double getCacheOptimizationLevel();
    
    /**
     * Get memory optimization level
     */
    static double getMemoryOptimizationLevel();
    
    /**
     * Get thread optimization level
     */
    static double getThreadOptimizationLevel();
    
    /**
     * Get NUMA optimization level
     */
    static double getNUMAOptimizationLevel();
    
    /**
     * Get power optimization level
     */
    static double getPowerOptimizationLevel();
    
    /**
     * Get thermal optimization level
     */
    static double getThermalOptimizationLevel();
    
    /**
     * Get I/O optimization level
     */
    static double getVIOOptimizationLevel();
    
    static constexpr uint64_t CACHE_ALIGNMENT = 64;
};

} // namespace nlm