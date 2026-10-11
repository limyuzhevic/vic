// ThreadProfiler.hpp - Thread-level profiling
#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace nlm {

/**
 * ThreadProfiler - Thread-level profiling for NLM
 * 
 * Provides comprehensive thread profiling including:
 * - Thread-level profiling and synchronization analysis
 * - Thread creation and destruction tracking
 * - Thread execution time measurements
 * - Thread synchronization analysis
 * - Thread contention detection
 * - Thread affinity and CPU affinity tracking
 * 
 * Features:
 * - Real-time thread monitoring during execution
 * - Post-execution thread analysis
 * - Thread performance analysis
 * - Thread contention detection and reporting
 * - Thread usage statistics
 * - Integration with performance profiler
 * 
 * Integration with existing systems:
 * - Existing logging system for thread profile output
 * - Configuration system for thread profiling settings
 * 
 * Output formats:
 * - Console output with formatted thread reports
 * - File output in various formats (JSON, CSV, HTML)
 * 
 * @see PerformanceProfiler
 * @see Benchmark
 */
class ThreadProfiler {
public:
    /**
     * Thread execution information
     */
    struct ThreadInfo {
        std::thread::id id;
        std::string name;
        int cpuAffinity;
        size_t stackSize;
        std::chrono::steady_clock::time_point creationTime;
        std::chrono::steady_clock::time_point lastActivity;
        int64_t totalExecutionTime; // nanoseconds
        int64_t cpuTime;           // nanoseconds
        int64_t waitTime;          // nanoseconds
        int64_t sleepTime;         // nanoseconds
        size_t contextSwitches;
        size_t pageFaults;
        bool isDetached;
        bool isJoined;
        std::unordered_map<std::string, double> customMetrics;
    };

    /**
     * Thread synchronization event
     */
    struct SynchronizationEvent {
        int64_t timestamp;
        std::thread::id threadId;
        std::string eventType; // "lock", "unlock", "wait", "notify"
        std::string lockName;
        int64_t duration; // nanoseconds
        bool successful;
        std::string threadState;
    };

    /**
     * Thread profiling configuration
     */
    struct ThreadProfilerConfig {
        bool enableThreadTracking = true;
        bool enableSynchronizationTracking = true;
        bool enableContentionDetection = true;
        bool enableAffinityTracking = false;
        bool enableRealTimeMonitoring = true;
        double samplingIntervalMs = 100.0;
        size_t maxThreadDataPoints = 10000;
        size_t maxSynchronizationEvents = 100000;
        std::string outputFormat = "json"; // json, csv, html
        std::string outputFile;
        std::string sessionName = "thread_default";
        bool trackCustomMetrics = true;
        bool trackSystemMetrics = true;
    };

    /**
     * Thread statistics
     */
    struct ThreadStats {
        size_t totalThreads;
        size_t activeThreads;
        size_t detachedThreads;
        size_t joinedThreads;
        double totalExecutionTime; // milliseconds
        double totalCpuTime;       // milliseconds
        double totalWaitTime;      // milliseconds
        double totalSleepTime;     // milliseconds
        size_t totalContextSwitches;
        size_t totalPageFaults;
        std::unordered_map<std::string, size_t> threadTypes;
        std::unordered_map<std::string, double> performanceMetrics;
    };

    /**
     * ThreadProfiler constructor
     */
    ThreadProfiler();
    
    /**
     * ThreadProfiler constructor with configuration
     */
    ThreadProfiler(const ThreadProfilerConfig& config);
    
    /**
     * ThreadProfiler destructor
     */
    ~ThreadProfiler();
    
    /**
     * Start thread profiling
     */
    void startProfiling(const std::string& sessionName = "thread_default");
    
    /**
     * Stop thread profiling
     */
    void stopProfiling();
    
    /**
     * Check if thread profiling is active
     */
    bool isProfilingActive() const { return isActive; }
    
    /**
     * Configure thread profiler
     */
    void configure(const ThreadProfilerConfig& config) { threadConfig = config; }
    
    /**
     * Register current thread
     */
    void registerThread(const std::string& name = "",
                       int cpuAffinity = -1,
                       size_t stackSize = 0);
    
    /**
     * Unregister thread
     */
    void unregisterThread();
    
    /**
     * Record thread execution time
     */
    void recordThreadExecution(int64_t durationNs);
    
    /**
     * Record thread CPU time
     */
    void recordThreadCPUTime(int64_t durationNs);
    
    /**
     * Record thread wait time
     */
    void recordThreadWaitTime(int64_t durationNs);
    
    /**
     * Record thread sleep time
     */
    void recordThreadSleepTime(int64_t durationNs);
    
    /**
     * Record context switch
     */
    void recordContextSwitch();
    
    /**
     * Record page fault
     */
    void recordPageFault();
    
    /**
     * Record synchronization event
     */
    void recordSynchronizationEvent(const SynchronizationEvent& event);
    
    /**
     * Get thread information
     */
    ThreadInfo getThreadInfo(std::thread::id threadId) const;
    
    /**
     * Get current thread information
     */
    ThreadInfo getCurrentThreadInfo() const;
    
    /**
     * Get thread statistics
     */
    ThreadStats getThreadStats() const;
    
    /**
     * Get synchronization events
     */
    std::vector<SynchronizationEvent> getSynchronizationEvents() const;
    
    /**
     * Get active threads
     */
    std::vector<ThreadInfo> getActiveThreads() const;
    
    /**
     * Detect thread contention
     */
    std::vector<std::string> detectContention() const;
    
    /**
     * Generate thread report
     */
    std::string generateReport(const std::string& format = "auto") const;
    
    /**
     * Export thread data to file
     */
    bool exportData(const std::string& filename, const std::string& format = "auto") const;
    
    /**
     * Generate thread performance analysis
     */
    std::string analyzePerformance() const;
    
    /**
     * Get session name
     */
    const std::string& getSessionName() const { return sessionName; }
    
    /**
     * Get profiling duration
     */
    int64_t getDuration() const;

private:
    bool isActive;
    std::string sessionName;
    ThreadProfilerConfig threadConfig;
    
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    
    mutable std::mutex dataMutex;
    
    std::unordered_map<std::thread::id, ThreadInfo> threadInfos;
    std::vector<SynchronizationEvent> synchronizationEvents;
    
    std::atomic<size_t> totalContextSwitches;
    std::atomic<size_t> totalPageFaults;
    
    void cleanupOldData();
    void updateThreadStats();
    
    // Suppress warnings about unused private members
    ThreadProfiler(const ThreadProfiler&) = delete;
    ThreadProfiler& operator=(const ThreadProfiler&) = delete;
};

} // namespace nlm