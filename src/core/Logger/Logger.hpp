#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <optional>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Advanced log entry with correlation ID and structured data
struct AdvancedLogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string correlationId;
    std::unordered_map<std::string, std::string> structuredData;
    double durationMs = 0.0;  // For timing measurements
    size_t memoryUsage = 0;   // For memory profiling
    int threadId = 0;
};

// Performance timing utilities
class Timer {
public:
    Timer(const std::string& name, LogLevel level = LogLevel::Info, 
          const std::string& correlationId = "");
    ~Timer();
    
    void stop();
    void pause();
    void resume();
    double getDurationMs() const;
    const std::string& getName() const;
    
    // Convert to log entry
    AdvancedLogEntry toLogEntry() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Event tracing for complex workflows
class EventTracer {
public:
    EventTracer(const std::string& name, const std::string& correlationId = "");
    ~EventTracer();
    
    void addEvent(const std::string& eventName, const std::string& details = "");
    void addCheckpoint(const std::string& checkpointName);
    void setContext(const std::string& key, const std::string& value);
    std::string getCorrelationId() const;
    
    // Convert to log entries
    std::vector<AdvancedLogEntry> toLogEntries() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory profiler
class MemoryProfiler {
public:
    MemoryProfiler(const std::string& name, const std::string& correlationId = "");
    ~MemoryProfiler();
    
    void trackAllocation(size_t size, const std::string& allocationSite = "");
    void trackDeallocation(size_t size);
    void trackObjectCreation(const std::string& typeName, size_t count = 1);
    void trackObjectDestruction(const std::string& typeName, size_t count = 1);
    
    // Get current memory usage
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    size_t getTotalAllocations() const;
    size_t getTotalDeallocations() const;
    
    // Convert to log entries
    std::vector<AdvancedLogEntry> toLogEntries() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced logger interface
class IAdvancedLogger {
public:
    virtual ~IAdvancedLogger() = default;
    virtual void log(const AdvancedLogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Format conversion
    virtual std::string format(const AdvancedLogEntry& entry) const = 0;
    virtual std::string formatJson(const AdvancedLogEntry& entry) const = 0;
    virtual std::string formatConsole(const AdvancedLogEntry& entry) const = 0;
    
    // Filtering and routing
    virtual bool shouldLog(const AdvancedLogEntry& entry) const = 0;
    virtual void setFilter(std::function<bool(const AdvancedLogEntry&)> filter) = 0;
};

// Enhanced logger implementation
class AdvancedLogger : public IAdvancedLogger {
public:
    AdvancedLogger();
    ~AdvancedLogger() override;
    
    void log(const AdvancedLogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Formatting
    std::string format(const AdvancedLogEntry& entry) const override;
    std::string formatJson(const AdvancedLogEntry& entry) const override;
    std::string formatConsole(const AdvancedLogEntry& entry) const override;
    
    // Filtering
    bool shouldLog(const AdvancedLogEntry& entry) const override;
    void setFilter(std::function<bool(const AdvancedLogEntry&)> filter) override;
    
    // Log convenience methods
    void logAdvanced(const AdvancedLogEntry& entry);
    void logTimer(const Timer& timer);
    void logEventTrace(const EventTracer& tracer);
    void logMemoryProfile(const MemoryProfiler& profiler);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class with enhanced capabilities
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<IAdvancedLogger> logger);
    void addLogger(std::shared_ptr<ILogger> logger);  // Backward compatibility
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Enhanced logging methods with timing and correlation IDs
    void logAdvanced(LogLevel level, const std::string& message,
                     const std::string& file = "", int line = 0,
                     const std::string& function = "",
                     const std::string& correlationId = "",
                     const std::unordered_map<std::string, std::string>& structuredData = {});
    
    // Performance logging utilities
    void logTimer(const Timer& timer);
    void logEventTrace(const EventTracer& tracer);
    void logMemoryProfile(const MemoryProfiler& profiler);
    
    // Convenience methods for common scenarios
    void logPerformance(const std::string& operation, double durationMs,
                        const std::string& correlationId = "");
    void logMemoryAllocation(size_t size, const std::string& allocationSite = "",
                            const std::string& correlationId = "");
    void logEvent(const std::string& eventName, const std::string& details = "",
                  const std::string& correlationId = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Helper for automatic timing
    template<typename Func>
    auto timeOperation(const std::string& name, LogLevel level, Func&& func) {
        Timer timer(name, level);
        auto result = func();
        return result;
    }
    
    // Helper for event tracing
    EventTracer traceEvent(const std::string& name, const std::string& correlationId = "");
    
    // Helper for memory profiling
    MemoryProfiler profileMemory(const std::string& name, const std::string& correlationId = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for timed operations
#define NLM_TIME_OPERATION(name, func) \
    nlm::Logger::getGlobal().timeOperation(name, nlm::LogLevel::Info, func)

// Convenience macro for event tracing
#define NLM_TRACE_EVENT(name) \
    auto eventTracer = nlm::Logger::getGlobal().traceEvent(name)

// Convenience macro for memory profiling
#define NLM_PROFILE_MEMORY(name) \
    auto memoryProfiler = nlm::Logger::getGlobal().profileMemory(name)

} // namespace nlm
