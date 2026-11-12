#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <map>
#include <mutex>
#include <atomic>
#include <iomanip>

namespace nlm {

// Enhanced log severity levels with more granularity
enum class LogLevel {
    // Debug levels
    Trace = -1,      // Very detailed tracing
    Debug = 0,       // General debugging information
    Verbose = 1,     // More verbose than Debug
    
    // Info levels  
    Info = 2,        // General information
    Notice = 3,      // Notable events that are not errors
    
    // Warning levels
    Warning = 4,     // Warning conditions
    Minor = 5,       // Minor issues that don't affect functionality
    
    // Error levels
    Error = 6,       // Error conditions
    Critical = 7,    // Critical errors
    Fatal = 8,       // Fatal errors that cause program termination
    
    // Special levels
    Audit = 9,       // Audit trail entries
    Performance = 10 // Performance measurements
};

// Log entry structure with enhanced metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::map<std::string, std::string> metadata;  // Structured logging metadata
    std::string category;                          // Log category (e.g., "neural", "memory", "plasticity")
    
    LogEntry() : level(LogLevel::Info), line(0), timestamp(std::chrono::system_clock::now()) {}
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", int line = 0, const std::string& function = "")
        : level(lvl), message(msg), file(file), line(line), function(function), timestamp(std::chrono::system_clock::now()) {
        // Extract thread ID for better debugging
        threadId = "unknown";
    }
};

// Log filter function type
using LogFilter = std::function<bool(const LogEntry&)>;

// Log output formatter function type
using LogFormatter = std::function<std::string(const LogEntry&)>;

// Log statistics structure
struct LogStatistics {
    std::atomic<size_t> totalLogs{0};
    std::atomic<size_t> logsByLevel[(size_t)LogLevel::Performance + 1]{0};
    std::chrono::system_clock::time_point startTime;
    std::mutex mutex;
    
    LogStatistics() : startTime(std::chrono::system_clock::now()) {}
    
    void increment(const LogEntry& entry) {
        std::lock_guard<std::mutex> lock(mutex);
        totalLogs++;
        if (static_cast<size_t>(entry.level) < sizeof(logsByLevel) / sizeof(logsByLevel[0])) {
            logsByLevel[static_cast<size_t>(entry.level)]++;
        }
    }
    
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "Log Statistics:\n";
        oss << "  Total logs: " << totalLogs << "\n";
        oss << "  Levels:\n";
        for (int i = (int)LogLevel::Trace; i <= (int)LogLevel::Performance; ++i) {
            if (logsByLevel[i] > 0) {
                oss << "    " << logLevelToString((LogLevel)i) << ": " << logsByLevel[i] << "\n";
            }
        }
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        oss << "  Duration: " << duration << " seconds\n";
        oss << "  Rate: " << (duration > 0 ? totalLogs / duration : 0) << " logs/second\n";
        return oss.str();
    }
};

// Enhanced logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setFilter(const LogFilter& filter) = 0;
    virtual LogFilter getFilter() const = 0;
    virtual void setFormatter(const LogFormatter& formatter) = 0;
    virtual LogFormatter getFormatter() const = 0;
    virtual void setCategoryFilter(const std::set<std::string>& categories) = 0;
    virtual std::set<std::string> getCategoryFilter() const = 0;
};

// Enhanced Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFilter(const LogFilter& filter) override;
    LogFilter getFilter() const override;
    void setFormatter(const LogFormatter& formatter) override;
    LogFormatter getFormatter() const override;
    void setCategoryFilter(const std::set<std::string>& categories) override;
    std::set<std::string> getCategoryFilter() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    bool isOutputFileOpen() const;
    
    // Configure console output
    void setColorForLevel(LogLevel level, const std::string& colorCode);
    std::string getColorForLevel(LogLevel level) const;
    
    // Performance logging
    void logPerformance(const std::string& operation, double durationMs, const std::map<std::string, std::string>& metadata = {});
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatEntry(const LogEntry& entry) const;
};

// Enhanced Main Logger class
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
    
    // Remove specific logger
    bool removeLogger(std::shared_ptr<ILogger> logger);
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Set global filter
    void setFilter(const LogFilter& filter);
    
    // Set global formatter
    void setFormatter(const LogFormatter& formatter);
    
    // Set category filter
    void setCategoryFilter(const std::set<std::string>& categories);
    
    // Add category to filter (include)
    void includeCategory(const std::string& category);
    
    // Remove category from filter (exclude)
    void excludeCategory(const std::string& category);
    
    // Get global statistics
    LogStatistics getGlobalStatistics() const;
    
    // Reset statistics
    void resetStatistics();
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::string& category = "");
    
    void logWithMetadata(LogLevel level, const std::string& message, 
                         const std::map<std::string, std::string>& metadata,
                         const std::string& file = "", int line = 0, 
                         const std::string& function = "", const std::string& category = "");
    
    // Convenience methods
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    void verbose(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    void notice(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    void minor(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    void fatal(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Log helper functions
    static std::string getCurrentThreadId();
    static std::string formatTimestamp(const std::chrono::system_clock::time_point& tp);
    static std::string logLevelToString(LogLevel level);
    
    // Configuration
    static void configureDefaultLogger(const std::string& name = "default");
    static std::shared_ptr<Logger> getConfiguredLogger(const std::string& name = "default");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for different log levels
#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().trace(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_VERBOSE(msg) nlm::Logger::getGlobal().verbose(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_NOTICE(msg) nlm::Logger::getGlobal().notice(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_MINOR(msg) nlm::Logger::getGlobal().minor(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_FATAL(msg) nlm::Logger::getGlobal().fatal(msg, __FILE__, __LINE__, __func__)

// Enhanced stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function, const std::string& category = "");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add metadata to the log entry
    LogStream& addMetadata(const std::string& key, const std::string& value);
    
    // Add category to the log entry
    LogStream& withCategory(const std::string& category);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string category_;
    std::map<std::string, std::string> metadata_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Performance timing helper
class Timer {
public:
    Timer(const std::string& operation, const std::string& category = "performance", LogLevel level = LogLevel::Performance);
    ~Timer();
    
    void setMetadata(const std::string& key, const std::string& value);
    
private:
    std::string operation_;
    std::string category_;
    LogLevel level_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::map<std::string, std::string> metadata_;
};

#define NLM_PERFORMANCE_SCOPE(operation, category) nlm::Timer timer(operation, category)

} // namespace nlm
