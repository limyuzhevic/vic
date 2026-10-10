#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stacktrace>
#include <execinfo.h>
#include <cxxabi.h>
#include <memory>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <cmath>

namespace nlm {

// Advanced logging enhancements with structured data
class StructuredLogger {
public:
    StructuredLogger();
    ~StructuredLogger();
    
    // Log structured data as JSON
    void logStructured(const std::string& category, const std::string& event,
                      const std::unordered_map<std::string, std::string>& data,
                      int severity = 0);
    
    // Log structured data with performance metrics
    void logPerformance(const std::string& operation,
                       const std::unordered_map<std::string, double>& metrics,
                       const std::string& context = "");
    
    // Log error with full context
    void logErrorWithContext(const std::string& errorMessage,
                           const std::string& file = "",
                           int line = 0,
                           const std::string& function = "",
                           const std::unordered_map<std::string, std::string>& context = {});
    
    // Log stack trace
    void logStackTrace(const std::string& message, const std::string& level = "ERROR");
    
    // Get performance log history
    const std::vector<std::string>& getPerformanceLog() const { return performanceLog_; }
    
    // Export structured logs
    std::string exportStructuredLogs() const;
    
    // Export performance logs
    std::string exportPerformanceLogs() const;
    
private:
    struct LogEntry {
        std::chrono::system_clock::time_point timestamp;
        std::string category;
        std::string event;
        std::unordered_map<std::string, std::string> data;
        int severity;
        
        LogEntry(const std::string& cat, const std::string& evt,
                 const std::unordered_map<std::string, std::string>& d, int sev)
            : category(cat), event(evt), data(d), severity(sev),
              timestamp(std::chrono::system_clock::now()) {}
    };
    
    struct PerformanceEntry {
        std::chrono::system_clock::time_point timestamp;
        std::string operation;
        std::unordered_map<std::string, double> metrics;
        std::string context;
        
        PerformanceEntry(const std::string& op,
                        const std::unordered_map<std::string, double>& met,
                        const std::string& ctx)
            : operation(op), metrics(met), context(ctx),
              timestamp(std::chrono::system_clock::now()) {}
    };
    
    std::vector<LogEntry> structuredLogs_;
    std::vector<PerformanceEntry> performanceLogs_;
    mutable std::mutex mutex_;
};

// Debug visualization output
class DebugVisualizer {
public:
    DebugVisualizer();
    ~DebugVisualizer();
    
    // Generate HTML debug dashboard
    std::string generateHTMLDashboard(const std::string& title = "NLM Debug Dashboard") const;
    
    // Generate text-based visualization
    std::string generateTextVisualization() const;
    
    // Generate performance chart data
    std::string generatePerformanceChartData() const;
    
    // Export debug data to file
    bool exportDebugData(const std::string& filename) const;
    
    // Add debug data points
    void addDataPoint(const std::string& category, const std::string& name,
                     double value, const std::string& unit = "");
    
    // Add debug snapshot
    void addDebugSnapshot(const std::string& name, const std::string& data);
    
private:
    struct DataPoint {
        std::string category;
        std::string name;
        double value;
        std::string unit;
        std::chrono::system_clock::time_point timestamp;
        
        DataPoint(const std::string& cat, const std::string& n, double val,
                 const std::string& unit = "")
            : category(cat), name(n), value(val), unit(unit),
              timestamp(std::chrono::system_clock::now()) {}
    };
    
    struct DebugSnapshot {
        std::string name;
        std::string data;
        std::chrono::system_clock::time_point timestamp;
        
        DebugSnapshot(const std::string& n, const std::string& d)
            : name(n), data(d), timestamp(std::chrono::system_clock::now()) {}
    };
    
    std::vector<DataPoint> dataPoints_;
    std::vector<DebugSnapshot> debugSnapshots_;
    mutable std::mutex mutex_;
};

// Performance monitoring with advanced metrics
class AdvancedPerformanceMonitor {
public:
    AdvancedPerformanceMonitor();
    
    // Record operation start
    void recordOperationStart(const std::string& operationName);
    
    // Record operation end
    void recordOperationEnd(const std::string& operationName);
    
    // Record memory allocation
    void recordMemoryAllocation(size_t size, const std::string& context);
    
    // Record memory deallocation
    void recordMemoryDeallocation(size_t size, const std::string& context);
    
    // Record CPU-intensive operation
    void recordCPUOperation(const std::string& operation, double cpuTime);
    
    // Record I/O operation
    void recordIOOperation(const std::string& operation, size_t bytes, double time);
    
    // Get performance statistics
    struct PerformanceStats {
        size_t totalOperations;
        double totalTime;
        size_t totalMemoryAllocated;
        size_t currentMemoryUsage;
        double totalCPUTime;
        uint64_t totalBytesTransferred;
        double averageOperationTime;
        double throughput;
    };
    
    PerformanceStats getPerformanceStats(const std::string& operation = "") const;
    
    // Export performance analysis
    std::string exportPerformanceAnalysis(const std::string& operation = "") const;
    
    // Generate performance report
    std::string generatePerformanceReport() const;
    
private:
    struct OperationStats {
        std::string name;
        std::chrono::system_clock::time_point startTime;
        double totalTime;
        size_t callCount;
        double avgTime;
        double minTime;
        double maxTime;
        
        OperationStats(const std::string& name) 
            : name(name), totalTime(0.0), callCount(0), avgTime(0.0),
              minTime(std::numeric_limits<double>::max()),
              maxTime(0.0) {}
    };
    
    struct MemoryStats {
        size_t totalAllocated;
        size_t currentUsage;
        std::unordered_map<std::string, size_t> allocationCounts;
        
        MemoryStats() : totalAllocated(0), currentUsage(0) {}
    };
    
    mutable std::mutex mutex_;
    std::unordered_map<std::string, OperationStats> operationStats_;
    MemoryStats memoryStats_;
    double totalCPUTime_;
    uint64_t totalBytesTransferred_;
    std::chrono::system_clock::time_point sessionStartTime_;
};

// Memory pool profiling for tracking allocation patterns
class MemoryPoolProfiler {
public:
    MemoryPoolProfiler();
    
    // Track memory pool allocation
    void trackAllocation(const std::string& poolName, size_t size, const std::string& context);
    
    // Track memory pool deallocation
    void trackDeallocation(const std::string& poolName, size_t size, const std::string& context);
    
    // Get memory pool statistics
    struct PoolStats {
        std::string name;
        size_t totalAllocated;
        size_t currentUsage;
        size_t allocationCount;
        size_t deallocationCount;
        double allocationRate;
        size_t maxUsage;
        
        PoolStats(const std::string& n) : name(n), totalAllocated(0), currentUsage(0),
                                         allocationCount(0), deallocationCount(0),
                                         allocationRate(0.0), maxUsage(0) {}
    };
    
    std::vector<PoolStats> getPoolStats() const;
    
    // Export memory pool analysis
    std::string exportMemoryPoolAnalysis() const;
    
    // Generate memory pool report
    std::string generateMemoryPoolReport() const;
    
private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, PoolStats> poolStats_;
    std::chrono::system_clock::time_point sessionStartTime_;
};

// Thread profiling for analyzing concurrency patterns
class ThreadProfiler {
public:
    ThreadProfiler();
    
    // Record thread start
    void recordThreadStart(const std::string& threadName, const std::string& threadId);
    
    // Record thread end
    void recordThreadEnd(const std::string& threadName, const std::string& threadId);
    
    // Record thread activity
    void recordThreadActivity(const std::string& threadName, const std::string& activity,
                             double duration, const std::string& context = "");
    
    // Get thread statistics
    struct ThreadStats {
        std::string name;
        std::string id;
        double totalActiveTime;
        size_t activityCount;
        std::unordered_map<std::string, double> activityTimes;
        
        ThreadStats(const std::string& n, const std::string& id) 
            : name(n), id(id), totalActiveTime(0.0), activityCount(0) {}
    };
    
    std::vector<ThreadStats> getThreadStats() const;
    
    // Export thread profiling data
    std::string exportThreadProfilingData() const;
    
    // Generate thread analysis report
    std::string generateThreadAnalysisReport() const;
    
private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, ThreadStats> threadStats_;
    std::chrono::system_clock::time_point sessionStartTime_;
};

// Output format converters for profiling data
class ProfilingOutputFormatter {
public:
    // Format data as JSON
    static std::string toJSON(const std::string& data);
    
    // Format data as CSV
    static std::string toCSV(const std::string& data);
    
    // Format data as text
    static std::string toText(const std::string& data);
    
    // Format data as HTML
    static std::string toHTML(const std::string& data);
    
    // Format performance data as text report
    static std::string formatPerformanceReport(const std::string& title, const std::string& data);
    
    // Format debug data as HTML dashboard
    static std::string formatDebugDashboard(const std::string& title, const std::string& data);
    
    // Format memory leak report
    static std::string formatMemoryLeakReport(const std::string& title, const std::string& data);
    
    // Format system health report
    static std::string formatSystemHealthReport(const std::string& title, const std::string& data);
    
private:
    // Helper functions for formatting
    static std::string escapeJSON(const std::string& input);
    static std::string escapeHTML(const std::string& input);
};

} // namespace nlm