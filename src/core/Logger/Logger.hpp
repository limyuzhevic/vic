#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <stacktrace>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Performance timing information
    std::chrono::duration<double> duration;
    
    // Memory information
    size_t memoryAllocated = 0;
    
    // Thread information
    std::thread::id threadId;
    
    // Event correlation ID
    std::string correlationId;
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Main Logger class
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Event types for debugging and profiling
enum class DebugEventType {
    TimelineStart,
    TimelineEnd,
    MemoryAllocation,
    MemoryDeallocation,
    FunctionCall,
    FunctionReturn,
    PerformanceSample,
    Custom
};

// Debug event for tracking operations
struct DebugEvent {
    DebugEventType type;
    std::string name;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::chrono::duration<double> duration; // For start/end pairs
    size_t memorySize; // For memory events
    std::string file;
    int line;
    std::string context;
    
    DebugEvent() : type(DebugEventType::Custom), threadId("") {}
};

// Performance profiler for timing critical sections
class PerformanceProfiler {
public:
    PerformanceProfiler(const std::string& name, bool enabled = true);
    ~PerformanceProfiler();
    
    void start();
    void stop();
    double getDuration() const { return duration_; }
    bool isActive() const { return active_; }
    
    // Static methods for convenience
    static void profileSection(const std::string& name, std::function<void()> func);
    static double measureTime(std::function<void()> func);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory tracker for detecting leaks and analyzing patterns
class MemoryTracker {
public:
    MemoryTracker();
    ~MemoryTracker();
    
    void trackAllocation(size_t size, const std::string& file, int line, const std::string& function);
    void trackDeallocation(size_t size, const std::string& file, int line, const std::string& function);
    
    // Get memory statistics
    size_t getTotalAllocated() const;
    size_t getCurrentAllocated() const;
    size_t getAllocationCount() const;
    size_t getDeallocationCount() const;
    
    // Check for leaks
    bool hasLeaks() const;
    std::vector<std::string> getLeakInfo() const;
    
    // Clear statistics
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Timeline profiler for tracking event sequences
class TimelineProfiler {
public:
    TimelineProfiler();
    ~TimelineProfiler();
    
    void startEvent(const std::string& name, const std::string& context = "");
    void endEvent(const std::string& name, const std::string& context = "");
    
    // Get timeline data
    const std::vector<DebugEvent>& getEvents() const { return events_; }
    
    // Export timeline
    std::string exportTimeline() const;
    void saveToFile(const std::string& filepath) const;
    
    // Clear timeline
    void clear();
    
    // Get duration of named event
    double getEventDuration(const std::string& name) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<DebugEvent> events_;
};

// Stack trace capture for debugging crashes
class StackTraceCapture {
public:
    static std::string captureCurrentStackTrace();
    static std::string captureStackTrace(std::thread::id threadId = std::this_thread::get_id());
    
    // Get formatted stack trace
    static std::string formatStackTrace(const std::string& trace);
    
    // Log stack trace with context
    static void logStackTrace(LogLevel level, const std::string& context = "");
    
    // Check if stack trace is available
    static bool isAvailable();
};

// Convenience macro for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
