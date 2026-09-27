// Performance Monitoring and Profiling System
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <atomic>
#include <algorithm>

namespace nlm {

// Performance monitoring utilities
class PerformanceMonitor {
public:
    enum class MetricType {
        CPU_USAGE,
        MEMORY_USAGE,
        GPU_USAGE,
        NETWORK_TRAFFIC,
        DISK_IO,
        CONTEXT_SWITCHES,
        SYSTEM_CALLS,
        THREAD_COUNT,
        PROCESS_COUNT,
        CUSTOM
    };
    
    struct Metric {
        MetricType type;
        std::string name;
        double value;
        std::string unit;
        Timestamp timestamp;
        std::map<std::string, std::string> metadata;
        
        Metric(MetricType t, const std::string& n, double v, const std::string& u = "")
            : type(t), name(n), value(v), unit(u), timestamp(std::chrono::duration_cast<Timestamp>(
                std::chrono::high_resolution_clock::now().time_since_epoch())) {}
    };
    
    struct PerformanceEvent {
        std::string operation;
        Timestamp startTime;
        Timestamp endTime;
        double duration; // in seconds
        uint64_t memoryUsage; // in bytes
        uint64_t memoryPeak; // peak memory usage
        uint32_t threadCount;
        std::string threadId;
        std::map<std::string, std::string> metrics;
        std::vector<Metric> customMetrics;
        
        PerformanceEvent(const std::string& op) 
            : operation(op), startTime(std::chrono::duration_cast<Timestamp>(
                std::chrono::high_resolution_clock::now().time_since_epoch())),
              memoryUsage(0), memoryPeak(0), threadCount(0) {}
    };
    
    struct PerformanceReport {
        Timestamp startTime;
        Timestamp endTime;
        double totalDuration;
        uint64_t totalMemoryUsage;
        uint64_t peakMemoryUsage;
        uint32_t peakThreadCount;
        uint32_t totalThreadCount;
        size_t operationCount;
        std::map<std::string, size_t> operationFrequency;
        std::map<std::string, double> operationDurations;
        std::map<std::string, uint64_t> operationMemoryUsage;
        std::vector<Metric> systemMetrics;
        std::map<std::string, std::string> metadata;
        
        void calculateSummary() {
            totalDuration = (endTime - startTime).count() / 1000000.0; // Convert to seconds
            // Calculate averages and statistics
        }
        
        std::string generateReport() const {
            std::ostringstream oss;
            oss << "Performance Report" << std::endl;
            oss << "================" << std::endl;
            oss << "Start Time: " << startTime.count() << std::endl;
            oss << "End Time: " << endTime.count() << std::endl;
            oss << "Total Duration: " << totalDuration << " seconds" << std::endl;
            oss << "Total Memory Usage: " << (totalMemoryUsage / 1024 / 1024) << " MB" << std::endl;
            oss << "Peak Memory Usage: " << (peakMemoryUsage / 1024 / 1024) << " MB" << std::endl;
            oss << "Peak Thread Count: " << peakThreadCount << std::endl;
            oss << "Total Operations: " << operationCount << std::endl;
            oss << std::endl;
            
            oss << "Operation Statistics:" << std::endl;
            for (const auto& pair : operationFrequency) {
                oss << "  " << pair.first << ": " << pair.second << " calls, "
                    << operationDurations.at(pair.first) / pair.second << " avg time" << std::endl;
            }
            
            return oss.str();
        }
    };
    
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    // Event recording
    void startEvent(const std::string& operation);
    void endEvent(const std::string& operation, uint64_t memoryUsage = 0);
    void addEvent(const PerformanceEvent& event);
    
    // Metric collection
    void recordMetric(MetricType type, const std::string& name, double value, const std::string& unit = "");
    void recordSystemMetrics();
    
    // Thread-specific monitoring
    void setCurrentThreadId(const std::string& threadId);
    std::string getCurrentThreadId() const;
    
    // Configuration
    void enableMonitoring(bool enable) { enabled_ = enable; }
    bool isMonitoringEnabled() const { return enabled_; }
    
    void setSamplingInterval(double interval) { samplingInterval_ = interval; }
    double getSamplingInterval() const { return samplingInterval_; }
    
    // Performance analysis
    PerformanceReport generateReport() const;
    std::vector<PerformanceEvent> getEvents(const std::string& operation) const;
    std::vector<PerformanceEvent> getAllEvents() const;
    
    // Reporting
    bool saveReport(const std::string& filename) const;
    bool saveMetrics(const std::string& filename) const;
    bool exportData(const std::string& filename) const;
    
    // Memory profiling
    void startMemoryTracking();
    void stopMemoryTracking();
    uint64_t getCurrentMemoryUsage() const;
    uint64_t getPeakMemoryUsage() const;
    
    // Thread profiling
    void startThreadTracking();
    void stopThreadTracking();
    uint32_t getCurrentThreadCount() const;
    
    // Performance alerts
    void addPerformanceAlert(const std::string& condition, const std::string& message);
    std::vector<std::pair<std::string, std::string>> getPerformanceAlerts() const;
    
    // Cleanup
    void clear();
    
private:
    PerformanceMonitor() : enabled_(false), samplingInterval_(0.1), 
                          currentMemoryUsage_(0), peakMemoryUsage_(0),
                          currentThreadCount_(0), peakThreadCount_(0) {}
    
    bool enabled_;
    double samplingInterval_;
    Timestamp lastSampleTime_;
    
    // Event storage
    std::vector<PerformanceEvent> events_;
    std::map<std::string, std::vector<PerformanceEvent>> eventsByOperation_;
    
    // Metric storage
    std::vector<Metric> metrics_;
    std::map<Timestamp, std::vector<Metric>> metricsByTime_;
    
    // Thread tracking
    std::string currentThreadId_;
    std::map<std::string, uint32_t> threadCounts_;
    
    // Memory tracking
    std::atomic<uint64_t> currentMemoryUsage_;
    std::atomic<uint64_t> peakMemoryUsage_;
    bool memoryTrackingEnabled_;
    
    // Thread tracking
    std::atomic<uint32_t> currentThreadCount_;
    std::atomic<uint32_t> peakThreadCount_;
    bool threadTrackingEnabled_;
    
    // Alerts
    std::vector<std::pair<std::string, std::string>> performanceAlerts_;
    
    // System monitoring
    void sampleSystemMetrics();
    void updateMemoryUsage();
    void updateThreadCount();
    
    // Helper functions
    Timestamp getCurrentTimestamp() const;
    double getSystemCPUUsage() const;
    uint64_t getSystemMemoryUsage() const;
    uint32_t getSystemThreadCount() const;
    std::string getSystemThreadId();
};

// Profiling decorators for performance measurement
class ProfileScope {
public:
    ProfileScope(const std::string& operation, PerformanceMonitor* monitor = nullptr)
        : operation_(operation), monitor_(monitor ? monitor : &PerformanceMonitor::getInstance()),
          startTime_(std::chrono::high_resolution_clock::now()) {}
    
    ~ProfileScope() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
        
        PerformanceEvent event(operation_);
        event.duration = duration.count() / 1000000.0; // Convert to seconds
        event.endTime = std::chrono::duration_cast<Timestamp>(endTime.time_since_epoch());
        
        if (monitor_) {
            monitor_->addEvent(event);
        }
    }
    
private:
    std::string operation_;
    PerformanceMonitor* monitor_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

// Performance analysis utilities
class PerformanceAnalyzer {
public:
    static PerformanceAnalyzer& getInstance() {
        static PerformanceAnalyzer instance;
        return instance;
    }
    
    // Bottleneck detection
    std::vector<std::string> findBottlenecks(const PerformanceMonitor& monitor) const;
    std::vector<std::string> findMemoryIssues(const PerformanceMonitor& monitor) const;
    std::vector<std::string> findThreadIssues(const PerformanceMonitor& monitor) const;
    
    // Performance optimization recommendations
    std::vector<std::string> getOptimizationRecommendations(const PerformanceMonitor& monitor) const;
    
    // Performance comparison
    double comparePerformance(const PerformanceMonitor& monitor1, const PerformanceMonitor& monitor2) const;
    
    // Performance profiling
    std::map<std::string, double> profileOperationFrequency(const PerformanceMonitor& monitor) const;
    std::map<std::string, double> profileMemoryUsage(const PerformanceMonitor& monitor) const;
    std::map<std::string, double> profileCPUUsage(const PerformanceMonitor& monitor) const;
    
    // Performance validation
    bool validatePerformanceThresholds(const PerformanceMonitor& monitor,
                                       const std::map<std::string, double>& thresholds) const;
    
    // Performance reporting
    std::string generatePerformanceSummary(const PerformanceMonitor& monitor) const;
    std::string generatePerformanceDetails(const PerformanceMonitor& monitor) const;
    
private:
    PerformanceAnalyzer() = default;
}; 

// Memory profiling utilities
class MemoryProfiler {
public:
    static MemoryProfiler& getInstance() {
        static MemoryProfiler instance;
        return instance;
    }
    
    // Memory tracking
    void trackAllocation(size_t size, const std::string& location = "");
    void trackDeallocation(size_t size, const std::string& location = "");
    void trackMemoryUsage();
    
    // Memory analysis
    std::map<std::string, size_t> getMemoryUsageByLocation() const;
    std::vector<std::pair<size_t, std::string>> getMemoryLeakCandidates() const;
    double getMemoryUsageRatio() const;
    
    // Memory visualization
    std::string generateMemoryUsageReport() const;
    std::string generateMemoryHeatmap() const;
    
    // Configuration
    void enableAllocationTracking(bool enable) { allocationTrackingEnabled_ = enable; }
    bool isAllocationTrackingEnabled() const { return allocationTrackingEnabled_; }
    
    void setLeakDetectionThreshold(double threshold) { leakDetectionThreshold_ = threshold; }
    double getLeakDetectionThreshold() const { return leakDetectionThreshold_; }
    
    // Cleanup
    void clear();
    
private:
    MemoryProfiler() : allocationTrackingEnabled_(false), leakDetectionThreshold_(0.1) {}
    
    bool allocationTrackingEnabled_;
    double leakDetectionThreshold_;
    
    // Allocation tracking
    struct AllocationRecord {
        size_t size;
        std::string location;
        Timestamp timestamp;
        std::string threadId;
    };
    
    std::vector<AllocationRecord> allocationRecords_;
    std::map<std::string, size_t> memoryUsageByLocation_;
    
    // Memory analysis
    std::vector<std::pair<size_t, std::string>> findMemoryLeaks() const;
    size_t calculateTotalMemoryUsage() const;
};

// Thread profiler
class ThreadProfiler {
public:
    static ThreadProfiler& getInstance() {
        static ThreadProfiler instance;
        return instance;
    }
    
    // Thread tracking
    void trackThreadCreation(const std::string& threadId);
    void trackThreadDestruction(const std::string& threadId);
    void trackThreadActivity(const std::string& threadId, const std::string& activity);
    
    // Thread analysis
    std::map<std::string, size_t> getThreadActivityCount() const;
    std::vector<std::string> getLongRunningThreads() const;
    std::vector<std::string> getIdleThreads() const;
    
    // Thread visualization
    std::string generateThreadReport() const;
    std::string generateThreadTimeline() const;
    
    // Configuration
    void enableThreadTracking(bool enable) { threadTrackingEnabled_ = enable; }
    bool isThreadTrackingEnabled() const { return threadTrackingEnabled_; }
    
    void setThreadTimeout(double timeout) { threadTimeout_ = timeout; }
    double getThreadTimeout() const { return threadTimeout_; }
    
    // Cleanup
    void clear();
    
private:
    ThreadProfiler() : threadTrackingEnabled_(false), threadTimeout_(1.0) {}
    
    bool threadTrackingEnabled_;
    double threadTimeout_;
    
    // Thread tracking
    std::map<std::string, size_t> threadActivities_;
    std::map<std::string, Timestamp> threadCreationTimes_;
    std::map<std::string, std::string> threadActivitiesMap_;
    
    // Thread analysis
    std::vector<std::string> findLongRunningThreads() const;
    std::vector<std::string> findIdleThreads() const;
};

// System performance monitor
class SystemPerformanceMonitor {
public:
    static SystemPerformanceMonitor& getInstance() {
        static SystemPerformanceMonitor instance;
        return instance;
    }
    
    // System metrics
    double getCPUUsage() const;
    uint64_t getMemoryUsage() const;
    uint64_t getSwapUsage() const;
    uint32_t getThreadCount() const;
    uint32_t getProcessCount() const;
    double getDiskReadSpeed() const;
    double getDiskWriteSpeed() const;
    double getNetworkUploadSpeed() const;
    double getNetworkDownloadSpeed() const;
    
    // System monitoring
    void startSystemMonitoring();
    void stopSystemMonitoring();
    bool isMonitoring() const { return monitoringEnabled_; }
    
    // Configuration
    void setMonitoringInterval(double interval) { monitoringInterval_ = interval; }
    double getMonitoringInterval() const { return monitoringInterval_; }
    
    // Metrics access
    std::vector<PerformanceMonitor::Metric> getSystemMetrics() const;
    
private:
    SystemPerformanceMonitor() : monitoringEnabled_(false), monitoringInterval_(1.0) {}
    
    bool monitoringEnabled_;
    double monitoringInterval_;
    std::vector<PerformanceMonitor::Metric> systemMetrics_;
    Timestamp lastUpdateTime_;
    
    void updateSystemMetrics();
    
    // Platform-specific implementations
#ifdef _WIN32
    double getWindowsCPUUsage() const;
    uint64_t getWindowsMemoryUsage() const;
    uint32_t getWindowsThreadCount() const;
    uint32_t getWindowsProcessCount() const;
#endif
#ifdef __linux__
    double getLinuxCPUUsage() const;
    uint64_t getLinuxMemoryUsage() const;
    uint32_t getLinuxThreadCount() const;
    uint32_t getLinuxProcessCount() const;
#endif
}; 

// Performance monitoring macros
#define NLM_PERF_START(operation) ProfileScope profileScope_##__LINE__(operation)
#define NLM_PERF_START_WITH_MONITOR(operation, monitor) ProfileScope profileScope_##__LINE__(operation, monitor)

#define NLM_RECORD_METRIC(type, name, value, unit) PerformanceMonitor::getInstance().recordMetric(type, name, value, unit)
#define NLM_START_EVENT(operation) PerformanceMonitor::getInstance().startEvent(operation)
#define NLM_END_EVENT(operation) PerformanceMonitor::getInstance().endEvent(operation)

// Performance analysis macros
#define NLM_PROFILE_FUNCTION() ProfileScope profileScope_##__LINE__(__func__)
#define NLM_PROFILE_FUNCTION_WITH_MONITOR(monitor) ProfileScope profileScope_##__LINE__(__func__, monitor)

} // namespace nlm