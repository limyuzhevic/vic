// PerformanceProfiler.hpp - Core Profiling Infrastructure
// Provides the foundation for comprehensive performance profiling

#pragma once

/**
 * PerformanceProfiler - Core infrastructure for NLM performance profiling
 * 
 * This module provides the foundation for comprehensive performance profiling
 * of the NLM brain simulation project. It supports:
 * 
 * 1. Multi-level profiling (fine-grained function, coarse-grained system)
 * 2. Real-time and post-mortem profiling modes
 * 3. Thread-safe operation
 * 4. Modular, pluggable architecture
 * 5. Integration with existing NLM performance optimizations
 * 
 * Designed to be non-intrusive and lightweight, with minimal performance overhead.
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

namespace nlm {

/**
 * Profile configuration options
 */
struct ProfileConfig {
    std::string name;
    bool enabled = true;
    bool realtime = false;  // Real-time mode vs post-mortem
    double samplingIntervalMs = 10.0;  // For realtime profiling
    size_t maxHistory = 10000;  // Maximum data points to store
    std::string outputFormat = "json";  // json, csv, yaml, xml
    std::string outputPath = ".";
    bool includeMemoryProfiling = true;
    bool includeCPUProfiling = true;
    bool includeThreadProfiling = true;
    bool includeIOProfiling = false;
    bool detectBottlenecks = true;
    bool generateReports = true;
    std::unordered_map<std::string, std::string> customOptions;
};

/**
 * Profile event types
 */
enum class ProfileEventType {
    // CPU-related events
    CPU_CYCLE = 0,
    CPU_INSTRUCTION = 1,
    CPU_CACHE_MISS = 2,
    CPU_BRANCH_MISPREDICTION = 3,
    CPU_FLOPS = 4,
    
    // Memory-related events
    MEMORY_ALLOC = 5,
    MEMORY_DEALLOC = 6,
    MEMORY_ACCESS = 7,
    MEMORY_CACHE_HIT = 8,
    MEMORY_PAGE_FAULT = 9,
    
    // Thread-related events
    THREAD_CREATE = 10,
    THREAD_DESTROY = 11,
    THREAD_SWITCH = 12,
    THREAD_LOCK_ACQUIRE = 13,
    THREAD_LOCK_RELEASE = 14,
    
    // I/O-related events
    IO_READ = 15,
    IO_WRITE = 16,
    IO_SEEK = 17,
    IO_FSYNC = 18,
    
    // Simulation-specific events
    TIMESTEP_START = 19,
    TIMESTEP_END = 20,
    NEURON_UPDATE = 21,
    SYNAPSE_UPDATE = 22,
    SPIKE_GENERATION = 23,
    SPIKE_PROPAGATION = 24,
    
    // Error events
    EXCEPTION = 25,
    ASSERTION = 26,
    MEMORY_LEAK = 27,
    RACE_CONDITION = 28
};

/**
 * Profile data point
 */
struct ProfileDataPoint {
    uint64_t timestamp;  // Nanoseconds since epoch
    ProfileEventType eventType;
    std::string sourceFile;
    size_t sourceLine;
    std::string functionName;
    std::string threadId;
    
    // Event-specific data
    std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>> eventData;
    
    // Performance metrics
    double durationNs = 0.0;  // Duration of the event
    double cpuTimeNs = 0.0;   // CPU time consumed
    double memoryDeltaBytes = 0.0;  // Memory delta
    
    // Context information
    std::string context;  // Additional context (e.g., timestep, region, population)
    std::unordered_map<std::string, std::string> tags;  // Key-value tags
    
    ProfileDataPoint() = default;
    
    ProfileDataPoint(uint64_t ts, ProfileEventType type, const std::string& file, 
                    size_t line, const std::string& func, const std::string& thread)
        : timestamp(ts), eventType(type), sourceFile(file), sourceLine(line), 
          functionName(func), threadId(thread) {}
};

/**
 * Performance metrics for analysis
 */
struct PerformanceMetrics {
    // CPU metrics
    double cpuUsagePercent = 0.0;
    double cpuUserTimePercent = 0.0;
    double cpuSystemTimePercent = 0.0;
    double cpuFrequencyMHz = 0.0;
    uint64_t totalCycles = 0;
    uint64_t totalInstructions = 0;
    double cacheMissRate = 0.0;
    double branchMispredictionRate = 0.0;
    double flopsPerSecond = 0.0;
    
    // Memory metrics
    size_t totalMemoryBytes = 0;
    size_t peakMemoryBytes = 0;
    size_t availableMemoryBytes = 0;
    double memoryUsagePercent = 0.0;
    uint64_t allocations = 0;
    uint64_t deallocations = 0;
    size_t allocationSizeBytes = 0;
    double memoryAllocationRate = 0.0;
    double memoryAccessRate = 0.0;
    
    // Thread metrics
    size_t activeThreads = 0;
    size_t totalThreads = 0;
    double threadUtilizationPercent = 0.0;
    size_t lockContentionEvents = 0;
    double averageLockWaitTimeNs = 0.0;
    
    // I/O metrics
    uint64_t bytesRead = 0;
    uint64_t bytesWritten = 0;
    size_t readOperations = 0;
    size_t writeOperations = 0;
    double readThroughputMBps = 0.0;
    double writeThroughputMBps = 0.0;
    
    // Simulation metrics
    double timestepsPerSecond = 0.0;
    double avgTimestepTimeMs = 0.0;
    double minTimestepTimeMs = 0.0;
    double maxTimestepTimeMs = 0.0;
    double timestepTimeStdDev = 0.0;
    uint64_t totalSpikesGenerated = 0;
    uint64_t totalSpikesPropagated = 0;
    size_t activeNeurons = 0;
    double avgFiringRateHz = 0.0;
    
    // Error metrics
    uint64_t exceptionsThrown = 0;
    uint64_t assertionsFailed = 0;
    uint64_t memoryLeaksDetected = 0;
    uint64_t raceConditionsDetected = 0;
    
    // System metrics
    size_t cpuCores = 0;
    size_t logicalCPUCores = 0;
    double memoryBandwidthMBps = 0.0;
    double cpuTemperatureCelsius = 0.0;
    
    PerformanceMetrics() = default;
    
    void reset() {
        *this = PerformanceMetrics();
    }
    
    std::string toJSON() const;
    std::string toCSV() const;
    double calculateEfficiencyScore() const;
};

/**
 * Profile session information
 */
struct ProfileSession {
    std::string id;
    std::string description;
    ProfileConfig config;
    uint64_t startTimestamp;  // Nanoseconds since epoch
    uint64_t endTimestamp;    // Nanoseconds since epoch
    std::vector<ProfileDataPoint> dataPoints;
    PerformanceMetrics metrics;
    
    double getDurationSeconds() const {
        return (endTimestamp - startTimestamp) / 1e9;
    }
    
    size_t getDataPointCount() const { return dataPoints.size(); }
    
    std::string toJSON() const;
    std::string toYAML() const;
    std::string toXML() const;
    std::string toCSV() const;
};

/**
 * Base profiler class
 */
class PerformanceProfiler {
public:
    virtual ~PerformanceProfiler() = default;
    
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    
    virtual bool isRunning() const = 0;
    virtual bool isPaused() const = 0;
    
    virtual void recordEvent(ProfileEventType eventType, const std::string& sourceFile = "",
                           size_t sourceLine = 0, const std::string& functionName = "",
                           const std::string& threadId = "", 
                           const std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>>& eventData = {}) = 0;
    
    virtual std::vector<ProfileDataPoint> getEvents(ProfileEventType eventType = ProfileEventType::TIMESTEP_END,
                                                   uint64_t startTime = 0, uint64_t endTime = 0) const = 0;
    
    virtual std::shared_ptr<ProfileSession> getSession() = 0;
    virtual void setSession(std::shared_ptr<ProfileSession> session) = 0;
    
    virtual std::string getProfilerName() const = 0;
    virtual std::string getVersion() const = 0;
    
    // Hook for integration with NLM modules
    virtual void onBrainUpdate(size_t timestep, size_t activeNeurons, double timestepTimeMs) = 0;
    virtual void onNeuronEvent(const std::string& event, const std::string& neuronId, 
                             double value, double timestamp) = 0;
    virtual void onSynapseEvent(const std::string& event, size_t sourceId, size_t targetId,
                              double weight, double timestamp) = 0;
};

/**
 * Thread-local storage for performance data
 */
class ThreadLocalProfiler {
private:
    std::string threadId;
    std::chrono::high_resolution_clock::time_point threadStartTime;
    uint64_t threadEventsCaptured = 0;
    double threadCPUTimeNs = 0.0;
    size_t threadMemoryUsage = 0;
    
public:
    ThreadLocalProfiler(const std::string& id) : threadId(id) {
        threadStartTime = std::chrono::high_resolution_clock::now();
    }
    
    const std::string& getThreadId() const { return threadId; }
    uint64_t getEventsCaptured() const { return threadEventsCaptured; }
    double getCPUTimeNs() const { return threadCPUTimeNs; }
    size_t getMemoryUsage() const { return threadMemoryUsage; }
    
    void incrementEvents() { threadEventsCaptured++; }
    void addCPUTime(double ns) { threadCPUTimeNs += ns; }
    void setMemoryUsage(size_t bytes) { threadMemoryUsage = bytes; }
    
    double getUptimeSeconds() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(now - threadStartTime);
        return duration.count() / 1e9;
    }
};

/**
 * Global profiler registry for singleton management
 */
class ProfilerRegistry {
private:
    std::unordered_map<std::string, std::shared_ptr<PerformanceProfiler>> profilers_;
    std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<ThreadLocalProfiler>> threadLocalProfilers_;
    std::atomic<bool> globalEnabled_{false};
    
public:
    static ProfilerRegistry& getInstance() {
        static ProfilerRegistry instance;
        return instance;
    }
    
    void registerProfiler(const std::string& name, std::shared_ptr<PerformanceProfiler> profiler) {
        std::lock_guard<std::mutex> lock(mutex_);
        profilers_[name] = profiler;
    }
    
    std::shared_ptr<PerformanceProfiler> getProfiler(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = profilers_.find(name);
        return (it != profilers_.end()) ? it->second : nullptr;
    }
    
    std::vector<std::string> getProfilerNames() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> names;
        names.reserve(profilers_.size());
        for (const auto& pair : profilers_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    void registerThreadLocalProfiler(const std::string& threadId, std::shared_ptr<ThreadLocalProfiler> profiler) {
        std::lock_guard<std::mutex> lock(mutex_);
        threadLocalProfilers_[threadId] = profiler;
    }
    
    std::shared_ptr<ThreadLocalProfiler> getThreadLocalProfiler(const std::string& threadId) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = threadLocalProfilers_.find(threadId);
        return (it != threadLocalProfilers_.end()) ? it->second : nullptr;
    }
    
    void setGlobalEnabled(bool enabled) { globalEnabled_.store(enabled); }
    bool isGlobalEnabled() const { return globalEnabled_.load(); }
    
    void cleanupThreadLocalProfiler(const std::string& threadId) {
        std::lock_guard<std::mutex> lock(mutex_);
        threadLocalProfilers_.erase(threadId);
    }
};

/**
 * Profiler factory for creating different types of profilers
 */
class ProfilerFactory {
public:
    static std::shared_ptr<PerformanceProfiler> createProfiler(const std::string& name, 
                                                               const ProfileConfig& config);
    
    static std::shared_ptr<ThreadLocalProfiler> createThreadLocalProfiler(const std::string& threadId);
    
    static std::shared_ptr<ProfileSession> createSession(const ProfileConfig& config);
    
    static std::vector<ProfileConfig> getAvailableProfilerConfigs();
};

/**
 * Performance profiler for NLM integration
 */
class NLPerformanceProfiler : public PerformanceProfiler {
private:
    std::string name_;
    ProfileConfig config_;
    std::shared_ptr<ProfileSession> session_;
    std::atomic<bool> running_{false};
    std::atomic<bool> paused_{false};
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point pauseTime_;
    double totalPausedTimeNs_ = 0.0;
    
    mutable std::mutex dataMutex_;
    std::vector<ProfileDataPoint> events_;
    
    // Thread-local storage
    thread_local static ThreadLocalProfiler* threadLocal_;
    
    // Global performance counters
    std::atomic<uint64_t> totalEvents_{0};
    std::atomic<uint64_t> totalTimesteps_{0};
    std::atomic<uint64_t> totalSpikes_{0};
    
public:
    NLPerformanceProfiler(const std::string& name, const ProfileConfig& config);
    ~NLPerformanceProfiler() override;
    
    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    
    bool isRunning() const override { return running_.load(); }
    bool isPaused() const override { return paused_.load(); }
    
    void recordEvent(ProfileEventType eventType, const std::string& sourceFile = "",
                    size_t sourceLine = 0, const std::string& functionName = "",
                    const std::string& threadId = "", 
                    const std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>>& eventData = {}) override;
    
    std::vector<ProfileDataPoint> getEvents(ProfileEventType eventType = ProfileEventType::TIMESTEP_END,
                                           uint64_t startTime = 0, uint64_t endTime = 0) const override;
    
    std::shared_ptr<ProfileSession> getSession() override { return session_; }
    void setSession(std::shared_ptr<ProfileSession> session) override { session_ = session; }
    
    std::string getProfilerName() const override { return name_; }
    std::string getVersion() const override { return "1.0.0"; }
    
    void onBrainUpdate(size_t timestep, size_t activeNeurons, double timestepTimeMs) override;
    void onNeuronEvent(const std::string& event, const std::string& neuronId, 
                     double value, double timestamp) override;
    void onSynapseEvent(const std::string& event, size_t sourceId, size_t targetId,
                      double weight, double timestamp) override;
    
    // Utility methods
    static std::string getCurrentThreadId();
    static ThreadLocalProfiler* getCurrentThreadLocalProfiler();
    
    // Global counters
    void incrementTimestepCount() { totalTimesteps_.fetch_add(1); }
    void incrementSpikeCount() { totalSpikes_.fetch_add(1); }
    
    uint64_t getTotalTimesteps() const { return totalTimesteps_.load(); }
    uint64_t getTotalSpikes() const { return totalSpikes_.load(); }
    
    // Export methods
    std::string exportToJSON() const;
    std::string exportToCSV() const;
    std::string exportToYAML() const;
    std::string exportToXML() const;
};

// Thread-local static member initialization
inline thread_local ThreadLocalProfiler* NLPerformanceProfiler::threadLocal_ = nullptr;

// Helper functions for easy profiling
namespace Profile {
    inline void recordEvent(ProfileEventType eventType, const std::string& sourceFile = "",
                           size_t sourceLine = 0, const std::string& functionName = "",
                           const std::unordered_map<std::string, std::variant<int64_t, double, std::string, bool>>& eventData = {}) {
        auto profiler = ProfilerRegistry::getInstance().getProfiler("nlm_profiler");
        if (profiler && profiler->isRunning()) {
            profiler->recordEvent(eventType, sourceFile, sourceLine, functionName, 
                                NLPerformanceProfiler::getCurrentThreadId(), eventData);
        }
    }
    
    inline void startProfiling(const ProfileConfig& config = ProfileConfig()) {
        auto profiler = ProfilerRegistry::getInstance().getProfiler("nlm_profiler");
        if (profiler) {
            profiler->start();
        }
    }
    
    inline void stopProfiling() {
        auto profiler = ProfilerRegistry::getInstance().getProfiler("nlm_profiler");
        if (profiler && profiler->isRunning()) {
            profiler->stop();
        }
    }
    
    inline bool isProfiling() {
        auto profiler = ProfilerRegistry::getInstance().getProfiler("nlm_profiler");
        return profiler && profiler->isRunning();
    }
}

} // namespace nlm