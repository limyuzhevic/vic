#pragma once

/**
 * PerformanceProfiler - Advanced performance profiling system
 * 
 * Core profiling engine for NLM neural simulation performance analysis.
 * Provides comprehensive profiling capabilities including:
 * - Timeline profiling with detailed metrics
 * - Memory profiling with allocation tracking
 * - CPU profiling with instruction-level analysis
 * - I/O profiling for memory and file operations
 * - Custom event profiling for user-defined metrics
 * - Performance bottleneck identification
 * - Optimization validation and recommendations
 * 
 * Integration with existing infrastructure:
 * - Hooks into EventQueue.hpp for event timing
 * - Integration with MemoryPool.hpp for allocation tracking
 * - SIMD profiling through SIMDVectorization.hpp
 * - Parallel processing metrics through ParallelProcessing.hpp
 * - Network optimization through existing connectivity systems
 */

#include "src/performance/EventQueue.hpp"
#include "src/performance/MemoryPool.hpp"
#include "src/performance/SIMDVectorization.hpp"
#include "src/performance/ParallelProcessing.hpp"
#include <chrono>
#include <map>
#include <unordered_map>
#include <vector>
#include <deque>
#include <string>
#include <thread>
#include <atomic>
#include <memory>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <functional>
#include <cstdint>
#include <type_traits>

namespace nlm {

/**
 * Profile event types
 */
enum class ProfileEventType {
    TIMING = 0,
    MEMORY_ALLOC = 1,
    MEMORY_FREE = 2,
    CPU_CYCLE = 3,
    CACHE_MISS = 4,
    MEMORY_BANDWIDTH = 5,
    THREAD_IDLE = 6,
    EVENT_PROCESSING = 7,
    CUSTOM = 8,
    MAX_TYPE = 9
};

/**
 * Detailed timing information
 */
struct TimingSample {
    std::chrono::high_resolution_clock::time_point timestamp;
    double wallTime;              // Wall clock time in seconds
    uint64_t cpuCycles;           // CPU cycles (if available)
    uint64_t instructions;       // Instructions retired (if available)
    uint32_t threadId;            // ID of thread that generated the sample
    ProfileEventType eventType;   // Type of event
    std::string eventName;        // Human-readable event name
    double duration;              // Duration in seconds
    
    // Performance metrics
    double cpuUsage;              // CPU usage percentage (0-100)
    double memoryUsage;           // Memory usage in MB
    double cacheHitRate;          // Cache hit rate (0-1)
    
    TimingSample() : wallTime(0), cpuCycles(0), instructions(0), threadId(0),
                    eventType(ProfileEventType::TIMING), wallTime(0), duration(0),
                    cpuUsage(0), memoryUsage(0), cacheHitRate(0) {}
    
    TimingSample(ProfileEventType type, const std::string& name)
        : threadId(static_cast<uint32_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()))),
          eventType(type), eventName(name), wallTime(0), duration(0),
          cpuUsage(0), memoryUsage(0), cacheHitRate(0) {}
};

/**
 * Memory allocation tracking
 */
struct MemorySample {
    std::chrono::high_resolution_clock::time_point timestamp;
    uint64_t allocationId;        // Unique ID for allocation
    size_t size;                  // Size in bytes
    const char* allocationSite;   // Source file and line
    const char* deallocationSite; // Source file and line
    bool isAllocation;            // True for alloc, false for free
    uint32_t threadId;            // Thread that performed operation
    ProfileEventType eventType;   // Type of memory operation
    
    // Memory pool information
    bool fromMemoryPool;          // Was allocated from memory pool
    const char* poolName;         // Name of memory pool if applicable
    
    MemorySample() : allocationId(0), size(0), allocationSite(nullptr),
                    deallocationSite(nullptr), threadId(0),
                    isAllocation(true), eventType(ProfileEventType::MEMORY_ALLOC),
                    fromMemoryPool(false), poolName(nullptr) {}
};

/**
 * Performance metrics snapshot
 */
struct PerformanceSnapshot {
    std::chrono::high_resolution_clock::time_point timestamp;
    uint64_t stepNumber;          // Simulation step number
    double wallTime;              // Total wall time elapsed
    double cpuTime;               // Total CPU time used
    
    // Resource usage
    size_t memoryUsage;           // Current memory usage in bytes
    size_t peakMemoryUsage;       // Peak memory usage in bytes
    uint64_t cacheMisses;         // Total cache misses
    uint64_t cacheHits;           // Total cache hits
    
    // Neural processing metrics
    uint64_t neuronsProcessed;    // Number of neurons processed
    uint64_t spikesProcessed;     // Number of spikes processed
    uint64_t synapticEvents;      // Number of synaptic events
    double avgFiringRate;         // Average firing rate
    double realTimeFactor;        // Simulated time / real time ratio
    
    // Parallel processing metrics
    size_t activeThreads;         // Number of active threads
    double parallelEfficiency;   // Parallel efficiency (0-1)
    
    // SIMD metrics
    uint64_t simdOperations;      // Number of SIMD operations
    double simdUtilization;       // SIMD utilization (0-1)
    
    PerformanceSnapshot() : stepNumber(0), wallTime(0), cpuTime(0),
                           memoryUsage(0), peakMemoryUsage(0),
                           cacheMisses(0), cacheHits(0),
                           neuronsProcessed(0), spikesProcessed(0),
                           synapticEvents(0), avgFiringRate(0),
                           realTimeFactor(0), activeThreads(0),
                           parallelEfficiency(0), simdOperations(0),
                           simdUtilization(0) {}
};

/**
 * Timeline segment for visualization
 */
struct TimelineSegment {
    double startTime;             // Start time in seconds
    double duration;              // Duration in seconds
    std::string label;            // Human-readable label
    ProfileEventType type;        // Event type
    double height;                // Height in timeline visualization (0-1)
    uint32_t threadId;            // Thread ID
    
    TimelineSegment() : startTime(0), duration(0), height(0), threadId(0) {}
    
    TimelineSegment(double start, double dur, const std::string& lbl,
                   ProfileEventType t, double h = 1.0, uint32_t tid = 0)
        : startTime(start), duration(dur), label(lbl), type(t), height(h), threadId(tid) {}
};

/**
 * Performance profiler core class
 */
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Disable copying
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    
    /**
     * Start profiling session
     * @param enableMemory Enable memory profiling
     * @param enableCPU Enable CPU profiling
     * @param enableTimeline Enable timeline recording
     */
    void start(bool enableMemory = true, bool enableCPU = true, 
               bool enableTimeline = true, bool enableIOLogging = false);
    
    /**
     * Stop profiling session
     * @param outputFile Output file path (optional, saves to default if not specified)
     */
    void stop(const std::string& outputFile = "");
    
    /**
     * Start timing a specific event
     * @param eventName Name of the event to time
     * @param eventType Type of event
     * @param threadId Thread ID for multi-threaded profiling
     * @return Event ID for later completion
     */
    uint64_t startEvent(const std::string& eventName, 
                       ProfileEventType eventType = ProfileEventType::TIMING,
                       uint32_t threadId = static_cast<uint32_t>(
                           std::hash<std::thread::id>{}(std::this_thread::get_id())));
    
    /**
     * End timing a specific event
     * @param eventId ID of the event to end
     */
    void endEvent(uint64_t eventId);
    
    /**
     * Record a timing sample
     * @param eventType Type of event
     * @param eventName Name of event
     * @param duration Duration in seconds
     */
    void recordTiming(ProfileEventType eventType, const std::string& eventName,
                     double duration, uint32_t threadId = static_cast<uint32_t>(
                         std::hash<std::thread::id>{}(std::this_thread::get_id())));
    
    /**
     * Record memory allocation
     * @param size Size in bytes
     * @param allocationSite Source file and line
     * @param fromPool Whether allocated from memory pool
     * @param poolName Name of memory pool
     */
    void recordMemoryAlloc(size_t size, const char* allocationSite = nullptr,
                          bool fromPool = false, const char* poolName = nullptr);
    
    /**
     * Record memory deallocation
     * @param allocationId ID of allocation being freed
     */
    void recordMemoryFree(uint64_t allocationId);
    
    /**
     * Record CPU performance metrics
     * @param cpuCycles CPU cycles
     * @param instructions Instructions retired
     * @param cacheMisses Cache misses
     */
    void recordCPUMetrics(uint64_t cpuCycles, uint64_t instructions,
                         uint64_t cacheMisses);
    
    /**
     * Record I/O performance
     * @param readBytes Bytes read
     * @param writeBytes Bytes written
     * @param readTime Time spent reading
     * @param writeTime Time spent writing
     */
    void recordIOStats(uint64_t readBytes, uint64_t writeBytes,
                      double readTime, double writeTime);
    
    /**
     * Get current performance snapshot
     */
    PerformanceSnapshot getSnapshot() const;
    
    /**
     * Get timeline segments for visualization
     */
    std::vector<TimelineSegment> getTimelineSegments() const;
    
    /**
     * Export profiling data to JSON format
     * @param output Output stream
     */
    void exportToJSON(std::ostream& output) const;
    
    /**
     * Export profiling data to CSV format
     * @param output Output stream
     */
    void exportToCSV(std::ostream& output) const;
    
    /**
     * Generate performance report
     * @return Formatted report string
     */
    std::string generateReport() const;
    
    /**
     * Identify performance bottlenecks
     * @return Vector of bottleneck descriptions
     */
    std::vector<std::string> identifyBottlenecks() const;
    
    /**
     * Get total number of events recorded
     */
    size_t getEventCount() const { return timingSamples_.size(); }
    
    /**
     * Get total memory allocations
     */
    size_t getMemoryAllocCount() const { return memorySamples_.size(); }
    
    /**
     * Get current profiler state
     */
    bool isRunning() const { return isRunning_; }
    
    /**
     * Get profiling session duration
     */
    double getSessionDuration() const;
    
    /**
     * Clear all collected data
     */
    void clear();
    
    /**
     * Save profiling data to file
     * @param filename File path
     */
    void saveToFile(const std::string& filename) const;
    
    /**
     * Load profiling data from file
     * @param filename File path
     */
    bool loadFromFile(const std::string& filename);
    
    /**
     * Generate visualization data for web-based timeline
     * @return JSON string for web visualization
     */
    std::string generateWebVisualization() const;
    
    /**
     * Register custom profiler for specialized data
     */
    void registerProfiler(std::function<void(std::ostream&)> customExport);
    
    /**
     * Add custom event marker
     * @param markerName Name of marker
     * @param data Associated data (optional)
     */
    void addMarker(const std::string& markerName, const std::string& data = "");
    
    /**
     * Get next allocation ID
     */
    static uint64_t getNextAllocationId() {
        static std::atomic<uint64_t> nextId{1};
        return nextId.fetch_add(1);
    }
    
    /**
     * Get thread-local performance counter
     */
    static uint32_t getCurrentThreadId() {
        static thread_local uint32_t threadId = []() {
            static std::atomic<uint32_t> nextId{0};
            return nextId.fetch_add(1);
        }();
        return threadId;
    }
    
private:
    bool isRunning_;
    std::chrono::high_resolution_clock::time_point sessionStartTime_;
    std::chrono::high_resolution_clock::time_point sessionEndTime_;
    
    // Timing samples
    std::deque<TimingSample> timingSamples_;
    std::map<uint64_t, std::chrono::high_resolution_clock::time_point> activeEvents_;
    
    // Memory samples
    std::deque<MemorySample> memorySamples_;
    std::map<uint64_t, MemorySample> activeAllocations_;
    
    // Performance snapshots
    std::vector<PerformanceSnapshot> snapshots_;
    
    // Timeline data
    std::vector<TimelineSegment> timelineSegments_;
    
    // Custom exporters
    std::vector<std::function<void(std::ostream&)>> customExporters_;
    
    // Marker data
    std::vector<std::pair<double, std::string>> markers_;
    
    // I/O statistics
    struct IOStats {
        uint64_t readBytes;
        uint64_t writeBytes;
        double readTime;
        double writeTime;
        
        IOStats() : readBytes(0), writeBytes(0), readTime(0), writeTime(0) {}
    } ioStats_;
    
    // CPU statistics
    struct CPUStats {
        uint64_t totalCycles;
        uint64_t totalInstructions;
        uint64_t totalCacheMisses;
        double totalCPUTime;
        
        CPUStats() : totalCycles(0), totalInstructions(0), totalCacheMisses(0),
                    totalCPUTime(0) {}
    } cpuStats_;
    
    // Current snapshot state
    mutable PerformanceSnapshot currentSnapshot_;
    
    // Performance thresholds for bottleneck detection
    struct PerformanceThresholds {
        double maxWallTimePerStep;      // Max wall time per simulation step
        size_t maxMemoryUsage;          // Max memory usage in bytes
        double maxCPUTimePerStep;       // Max CPU time per simulation step
        uint64_t maxCacheMisses;        // Max cache misses per step
        double maxMemoryBandwidth;      // Max memory bandwidth in GB/s
        double minSIMDEfficiency;       // Min SIMD utilization
        double minParallelEfficiency;  // Min parallel efficiency
        
        PerformanceThresholds() : maxWallTimePerStep(0.1),  // 100ms per step
                                 maxMemoryUsage(1024 * 1024 * 1024),  // 1GB
                                 maxCPUTimePerStep(0.05),  // 50ms CPU time
                                 maxCacheMisses(1000000),  // 1M cache misses
                                 maxMemoryBandwidth(25.6),  // 25.6 GB/s (DDR4)
                                 minSIMDEfficiency(0.8),   // 80% SIMD utilization
                                 minParallelEfficiency(0.7) {}  // 70% parallel efficiency
    } thresholds_;
    
    // Session configuration
    struct ProfilerConfig {
        bool enableMemoryProfiling;
        bool enableCPUProfiling;
        bool enableTimelineRecording;
        bool enableIOLogging;
        double samplingInterval;       // Sampling interval in seconds
        size_t maxSamples;             // Maximum samples to store
        std::string outputDirectory;   // Output directory for reports
        
        ProfilerConfig() : enableMemoryProfiling(true), enableCPUProfiling(true),
                          enableTimelineRecording(true), enableIOLogging(false),
                          samplingInterval(0.1), maxSamples(1000000),
                          outputDirectory(".") {}
    } config_;
    
    // Internal helper methods
    void updateSnapshot();
    void cleanupOldData();
    void recordTimelineEvent(const TimingSample& sample);
    double calculateRealTimeFactor(double wallTime, double cpuTime) const;
    std::string formatDuration(double seconds) const;
    std::string formatBytes(size_t bytes) const;
    std::string generateHTMLTimeline() const;
    std::string generateJSONData() const;
    std::string generateCSVData() const;
    std::vector<std::string> detectPerformanceIssues() const;
    std::vector<TimelineSegment> generateTimelineFromSamples() const;
    void updatePerformanceThresholds();
    double calculateSIMDEfficiency() const;
    double calculateParallelEfficiency() const;
    void exportToWebFormat(std::ostream& output) const;
};