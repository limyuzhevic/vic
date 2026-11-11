#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <map>
#include <set>

namespace nlm {

// Enhanced Log severity levels with additional granularity
enum class LogLevel {
    Off = -1,              // Completely disabled
    Emergency = 0,         // System unusable
    Alert = 1,             // Action must be taken immediately
    Critical = 2,          // Critical conditions
    Error = 3,             // Error conditions
    Warning = 4,           // Warning conditions
    Notice = 5,            // Normal but significant conditions
    Info = 6,              // Informational messages
    Debug = 7,             // Detailed debugging information
    Trace = 8,             // Extremely detailed debugging
    Perf = 9,              // Performance measurements
    Custom = 100           // User-defined levels
};

// Enhanced Log entry structure with additional metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::string context;           // Additional context information
    std::map<std::string, std::string> metadata;  // Arbitrary key-value pairs
    std::vector<std::string> tags; // Categorization tags
    
    LogEntry() : level(LogLevel::Info), line(0), 
                 timestamp(std::chrono::system_clock::now()),
                 threadId(""), context(""), tags() {}
    
    LogEntry(LogLevel lvl, const std::string& msg, 
             const std::string& file = "", int lineNum = 0,
             const std::string& func = "")
        : level(lvl), message(msg), file(file), line(lineNum), 
          function(func), timestamp(std::chrono::system_clock::now()),
          threadId(""), context(""), tags() {}
    
    std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            timestamp.time_since_epoch()) % 1000;
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
};

// Enhanced logger interface with filtering and formatting
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Set log format
    virtual void setFormat(const std::string& format) = 0;
    virtual std::string getFormat() const = 0;
    
    // Enable/disable specific log levels
    virtual void setLevelEnabled(LogLevel level, bool enabled) = 0;
    virtual bool isLevelEnabled(LogLevel level) const = 0;
    
    // Set log tags filter
    virtual void setTagFilter(const std::set<std::string>& tags) = 0;
    virtual std::set<std::string> getTagFilter() const = 0;
    
    // Add handler for specific conditions
    virtual void addFilter(std::function<bool(const LogEntry&)> filter) = 0;
    
    // Log statistics
    virtual void resetStatistics() = 0;
    virtual size_t getLogCount(LogLevel level) const = 0;
    virtual size_t getTotalLogCount() const = 0;
};

// Enhanced Console logger with advanced formatting
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced color and formatting options
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    void setColor(LogLevel level, const std::string& colorCode);
    std::string getColor(LogLevel level) const;
    
    // Enhanced output options
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    void setOutputFormat(const std::string& format);
    std::string getOutputFormat() const;
    
    // Async logging support
    void setAsyncLogging(bool enabled, size_t queueSize = 1000);
    bool isAsyncLogging() const;
    
    // Performance monitoring
    void setPerformanceMonitoring(bool enabled);
    bool isPerformanceMonitoring() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string applyFormat(const LogEntry& entry, const std::string& format) const;
    void logToConsole(const LogEntry& entry);
    void logToFile(const LogEntry& entry);
};

// Enhanced Logger class with comprehensive features
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Enhanced logger management
    void addLogger(std::shared_ptr<ILogger> logger);
    bool removeLogger(ILogger* logger);
    
    void clearLoggers();
    
    // Enhanced level management
    void setLevel(LogLevel level);
    void setLevel(const std::string& levelName);
    LogLevel getLevel() const;
    
    // Global logging functions with enhanced metadata
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::string& context = "");
    
    void logWithMetadata(LogLevel level, const std::string& message,
                        const std::map<std::string, std::string>& metadata,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "");
    
    // Enhanced convenience methods
    void emergency(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void alert(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void notice(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Performance logging
    void perf(const std::string& operation, double duration, int samples = 1);
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    bool isEnabled(const std::string& levelName) const;
    
    // Get statistics
    size_t getLogCount(LogLevel level) const;
    size_t getTotalLogCount() const;
    std::map<LogLevel, size_t> getLogStatistics() const;
    
    // Get global logger instance (singleton)
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Configuration
    void configure(const std::string& configFile);
    bool saveConfig(const std::string& configFile) const;
    void resetToDefaults();
    
    // Context management
    void pushContext(const std::string& context);
    void popContext();
    std::string getCurrentContext() const;
    
    // Thread safety
    void lock();
    void unlock();
    bool tryLock();
    
    // Log level name conversion
    static std::string levelToString(LogLevel level);
    static LogLevel stringToLevel(const std::string& levelName);
    
    // Validation
    static bool isValidLogLevel(const std::string& levelName);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void logInternal(LogLevel level, const std::string& message,
                     const std::string& file, int line,
                     const std::string& function, const std::string& context,
                     const std::map<std::string, std::string>& metadata = {});
};

// Enhanced LogStream with more features
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Special handling for strings with quotes
    LogStream& operator<<(const std::string& value) {
        oss_ << "\"" << value << "\"";
        return *this;
    }
    
    // Special handling for null pointers
    LogStream& operator<<(std::nullptr_t) {
        oss_ << "nullptr";
        return *this;
    }
    
    // Automatic flushing
    LogStream& endl() {
        *this << '\n';
        return *this;
    }
    
    // Set metadata for this log entry
    LogStream& withMetadata(const std::string& key, const std::string& value);
    LogStream& withContext(const std::string& context);
    
    // Add tags
    LogStream& withTag(const std::string& tag);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::map<std::string, std::string> metadata_;
    std::string context_;
    std::vector<std::string> tags_;
};

// Enhanced logging macros with thread safety
#define NLM_LOG_EMERGENCY(msg) nlm::Logger::getGlobal().emergency(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ALERT(msg) nlm::Logger::getGlobal().alert(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_NOTICE(msg) nlm::Logger::getGlobal().notice(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().trace(msg, __FILE__, __LINE__, __func__)

// Performance logging macros
#define NLM_PERF_START(op) nlm::PerfProfiler profiler(__FILE__, __LINE__, __func__, op)
#define NLM_PERF_END(profile) profile.stop()

// Enhanced stream-style logging
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Thread-safe error handling
#define NLM_THROW_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__); throw std::runtime_error(msg)
#define NLM_THROW_LOGGED_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__); throw std::runtime_error("Logged error: " + std::string(msg))

// Performance profiler for logging
class PerfProfiler {
public:
    PerfProfiler(const std::string& file, int line, const std::string& function, const std::string& operation);
    ~PerfProfiler();
    
    void stop();
    double getDuration() const;
    std::string getOperation() const;
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::string file_;
    int line_;
    std::string function_;
    std::string operation_;
    bool stopped_;
};

} // namespace nlm