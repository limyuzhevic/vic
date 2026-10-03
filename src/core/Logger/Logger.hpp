#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <queue>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <json/json.h> // For structured logging

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Enhanced log entry with structured data support
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Structured data for advanced logging
    std::string category;
    std::string source;
    std::unordered_map<std::string, std::string> metadata;
    
    // Performance tracking
    std::chrono::steady_clock::time_point processingStartTime;
};

// Logger interface with enhanced capabilities
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enhanced logging methods
    virtual void logStructured(LogLevel level, const std::string& category, 
                             const std::string& message, 
                             const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Performance logging
    virtual void logPerformance(const std::string& operation, 
                              const std::chrono::steady_clock::duration& duration,
                              const std::string& metadata = "");
};

// Async logger implementation
class AsyncLogger : public ILogger {
public:
    AsyncLogger(size_t maxQueueSize = 10000, LogLevel level = LogLevel::Info);
    ~AsyncLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Async operations
    void start();
    void stop();
    bool isRunning() const;
    
    // Configuration
    void setQueueSize(size_t size);
    void setFlushInterval(const std::chrono::milliseconds& interval);
    
    // Statistics
    size_t getProcessedCount() const;
    size_t getQueueSize() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced console logger with advanced formatting
class EnhancedConsoleLogger : public ILogger {
public:
    EnhancedConsoleLogger(LogLevel level = LogLevel::Info);
    ~EnhancedConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Formatting options
    void setFormat(const std::string& format);
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setColorByLevel(bool useLevelColors);
    
    // Output destinations
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Buffering
    void setBufferSize(size_t size);
    void flush();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string formatEntry(const LogEntry& entry) const;
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Main enhanced Logger class
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
             const std::string& function = "", const std::string& category = "");
    
    // Enhanced structured logging
    void logStructured(LogLevel level, const std::string& category, 
                      const std::string& message,
                      const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Performance logging
    void logPerformance(const std::string& operation, 
                       const std::chrono::steady_clock::duration& duration,
                       const std::string& metadata = "");
    
    // Convenience methods with enhanced signatures
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
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Statistics and monitoring
    size_t getTotalLogCount() const;
    size_t getErrorCount() const;
    size_t getPerformanceLogCount() const;
    std::string getLogStatistics() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced convenience macros for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced macros with category
#define NLM_LOG_DEBUG_CAT(msg, cat) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, cat)
#define NLM_LOG_INFO_CAT(msg, cat) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, cat)
#define NLM_LOG_WARNING_CAT(msg, cat) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__, cat)

// Performance logging macros
#define NLM_PERF_LOG(operation, duration) nlm::Logger::getGlobal().logPerformance(operation, duration)

// Enhanced stream logging helper with enhanced capabilities
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function, 
              const std::string& category = "");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Structured data streaming
    LogStream& operator<<(const std::pair<std::string, std::string>& kv);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string category_;
};

// Enhanced stream logging macro
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Color codes for console output
namespace Color {
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string BOLD = "\033[1m";
}

} // namespace nlm
