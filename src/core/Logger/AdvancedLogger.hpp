#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace nlm {

// Enhanced Log severity levels including profiling
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Profile = 5,
    Timeline = 6
};

// Enhanced Log entry structure with timing information
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    double elapsedTime;           // For profiling measurements
    std::string operationId;      // For timeline operations
    std::string category;         // For categorizing log messages
    std::unordered_map<std::string, std::string> metadata; // Key-value pairs for additional context
};

// Performance metrics structure
struct PerformanceMetrics {
    std::string operationName;
    double elapsedTime;           // in milliseconds
    size_t memoryUsed;            // in bytes
    int callsCount;              // Number of times this operation was called
    double averageTime;          // Calculated
    double minTime;              // Calculated
    double maxTime;              // Calculated
    std::string category;         // Operation category
    std::chrono::system_clock::time_point startTime; // For timing measurements
    std::chrono::system_clock::time_point endTime;   // For timing measurements
};

// Timeline event for sequence analysis
struct TimelineEvent {
    std::string eventId;
    std::string eventName;
    std::chrono::system_clock::time_point timestamp;
    double duration;              // in milliseconds
    std::string category;
    std::string parentEventId;    // For hierarchical events
    std::vector<std::string> childEventIds;
    std::string threadId;
    std::unordered_map<std::string, std::string> metadata;
};

// Advanced ILogger interface with profiling capabilities
class IAdvancedLogger : public ILogger {
public:
    virtual ~IAdvancedLogger() = default;
    
    // Standard ILogger interface
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Performance monitoring
    virtual void startPerformanceMeasurement(const std::string& operationName, 
                                             const std::string& category = "",
                                             const std::unordered_map<std::string, std::string>& metadata = {});
    virtual PerformanceMetrics stopPerformanceMeasurement(const std::string& operationName);
    virtual void recordPerformanceMetric(const PerformanceMetrics& metric);
    virtual std::vector<PerformanceMetrics> getPerformanceMetrics(const std::string& category = "", 
                                                                 const std::string& operationName = "") const;
    virtual void clearPerformanceMetrics();
    
    // Timeline profiling
    virtual std::string startTimelineEvent(const std::string& eventName,
                                          const std::string& category = "",
                                          const std::string& parentEventId = "",
                                          const std::unordered_map<std::string, std::string>& metadata = {});
    virtual void stopTimelineEvent(const std::string& eventId);
    virtual TimelineEvent getTimelineEvent(const std::string& eventId) const;
    virtual std::vector<TimelineEvent> getTimelineEvents(const std::string& category = "", 
                                                         const std::string& parentEventId = "") const;
    virtual std::vector<TimelineEvent> getTimelinePath(const std::string& eventId) const;
    virtual void clearTimelineEvents();
    
    // Memory tracking
    virtual void recordMemoryUsage(size_t memoryBytes, const std::string& context = "");
    virtual std::vector<std::pair<size_t, std::string>> getMemoryUsageHistory() const;
    
    // Statistics and analytics
    virtual double getAverageExecutionTime(const std::string& operationName) const;
    virtual size_t getCallCount(const std::string& operationName) const;
    virtual std::string getMostUsedOperation() const;
}; 

// Console logger with profiling capabilities
class AdvancedConsoleLogger : public IAdvancedLogger {
public:
    AdvancedConsoleLogger();
    explicit AdvancedConsoleLogger(LogLevel level);
    ~AdvancedConsoleLogger() override;
    
    // ILogger interface
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // IAdvancedLogger interface
    void startPerformanceMeasurement(const std::string& operationName,
                                     const std::string& category = "",
                                     const std::unordered_map<std::string, std::string>& metadata = {}) override;
    PerformanceMetrics stopPerformanceMeasurement(const std::string& operationName) override;
    void recordPerformanceMetric(const PerformanceMetrics& metric) override;
    std::vector<PerformanceMetrics> getPerformanceMetrics(const std::string& category = "",
                                                         const std::string& operationName = "") const override;
    void clearPerformanceMetrics() override;
    
    std::string startTimelineEvent(const std::string& eventName,
                                   const std::string& category = "",
                                   const std::string& parentEventId = "",
                                   const std::unordered_map<std::string, std::string>& metadata = {}) override;
    void stopTimelineEvent(const std::string& eventId) override;
    TimelineEvent getTimelineEvent(const std::string& eventId) const override;
    std::vector<TimelineEvent> getTimelineEvents(const std::string& category = "",
                                                const std::string& parentEventId = "") const override;
    std::vector<TimelineEvent> getTimelinePath(const std::string& eventId) const override;
    void clearTimelineEvents() override;
    
    void recordMemoryUsage(size_t memoryBytes, const std::string& context = "") override;
    std::vector<std::pair<size_t, std::string>> getMemoryUsageHistory() const override;
    
    double getAverageExecutionTime(const std::string& operationName) const override;
    size_t getCallCount(const std::string& operationName) const override;
    std::string getMostUsedOperation() const override;
    
    // Generate comprehensive profiling report
    std::string generateProfilingReport() const;
    
    // Export profiling data to file
    bool exportProfilingData(const std::string& filepath) const;
    
    // Clear all data
    void clearAllData();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatTimelineEvent(const TimelineEvent& event) const;
    std::string formatPerformanceMetric(const PerformanceMetrics& metric) const;
};

// Main advanced Logger class
class AdvancedLogger {
public:
    AdvancedLogger();
    ~AdvancedLogger();
    
    // Disable copying
    AdvancedLogger(const AdvancedLogger&) = delete;
    AdvancedLogger& operator=(const AdvancedLogger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<IAdvancedLogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions with timing
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "", const std::string& category = "");
    
    // Convenience methods with timing
    void debug(const std::string& message, const std::string& file = "", int line = 0,
              const std::string& function = "", const std::string& category = "");
    void info(const std::string& message, const std::string& file = "", int line = 0,
             const std::string& function = "", const std::string& category = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0,
                const std::string& function = "", const std::string& category = "");
    void error(const std::string& message, const std::string& file = "", int line = 0,
              const std::string& function = "", const std::string& category = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0,
                 const std::string& function = "", const std::string& category = "");
    
    // Profile measurements with automatic timing
    void profile(const std::string& operationName, std::function<void()> func,
                 const std::string& category = "", const std::string& description = "");
    void profileWithReturn(const std::string& operationName, std::function<void*()> func,
                          const std::string& category = "", const std::string& description = "");
    
    // Timeline events
    std::string timelineStart(const std::string& eventName,
                             const std::string& category = "",
                             const std::string& parentEventId = "");
    void timelineEnd(const std::string& eventId);
    
    // Get global logger instance
    static AdvancedLogger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<AdvancedLogger> logger);
    
    // Get profiling statistics
    std::string getProfilingSummary() const;
    std::vector<PerformanceMetrics> getTopPerformanceMetrics(size_t limit = 10) const;
    std::vector<TimelineEvent> getTimelineReport() const;
    
    // Export functionality
    bool exportProfilingReport(const std::string& filepath) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for advanced logging
#define NLM_PROFILE(operationName, func) nlm::AdvancedLogger::getGlobal().profile(operationName, func, "performance")
#define NLM_TIMELINE_START(eventName) nlm::AdvancedLogger::getGlobal().timelineStart(eventName, "timeline")
#define NLM_TIMELINE_END(eventId) nlm::AdvancedLogger::getGlobal().timelineEnd(eventId)
#define NLM_PERF_MEASURE(operationName, func, category) nlm::AdvancedLogger::getGlobal().profile(operationName, func, category)

} // namespace nlm