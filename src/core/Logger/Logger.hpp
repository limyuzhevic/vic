#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <fstream>
#include <iomanip>

namespace nlm {

// Enhanced log severity levels with more granular control
enum class LogLevel {
    Silent = -1,    // Nothing logged
    Fatal = 0,       // System is unusable
    Error = 1,       // Error conditions
    Warning = 2,     // Warning conditions
    Notice = 3,      // Normal but significant conditions
    Info = 4,        // General information
    Debug = 5,       // Detailed debugging information
    Trace = 6        // Fine-grained tracing
};

// Log entry structure with enhanced metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    uint64_t threadId;
    std::string threadName;
    
    // Additional metadata
    std::string category;        // Log category (e.g., "PLASTICITY", "MEMORY")
    std::string module;           // Module name
    std::unordered_map<std::string, std::string> context; // Key-value context
};

// Log sink interface with enhanced capabilities
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Optional enhanced features
    virtual void flush() {}
    virtual void setFormat(const std::string& format) {}
    virtual std::string getFormat() const { return "standard"; }
};

// Enhanced console logger implementation
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
    void setColorScheme(const std::string& scheme);
    
    // File output with rotation support
    void setOutputFile(const std::string& filepath, size_t maxSize = 10 * 1024 * 1024, int maxFiles = 5);
    void closeOutputFile();
    void rotateLogFile();
    
    // Enhanced formatting
    void setIncludeTimestamp(bool include);
    void setIncludeThreadInfo(bool include);
    void setIncludeLocationInfo(bool include);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level, const std::string& scheme = "default") const;
    std::string formatMessage(const LogEntry& entry) const;
    void writeToFile(const std::string& message);
};

// Rotating file logger for persistent logs
class FileLogger : public ILogger {
public:
    FileLogger(const std::string& filepath, size_t maxSize = 10 * 1024 * 1024, int maxFiles = 5);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void flush() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class with enhanced features
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove specific logger
    void removeLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Get level of first logger (or global default)
    LogLevel getLevel() const;
    
    // Global logging functions with enhanced parameters
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "",
             const std::string& category = "", const std::string& module = "");
    
    // Convenience methods with enhanced parameters
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
               const std::string& category = "", const std::string& module = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
              const std::string& category = "", const std::string& module = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
                 const std::string& category = "", const std::string& module = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
               const std::string& category = "", const std::string& module = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
                  const std::string& category = "", const std::string& module = "");
    
    // Context-aware logging with key-value pairs
    void logWithContext(LogLevel level, const std::string& message,
                        const std::unordered_map<std::string, std::string>& context,
                        const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Get all active loggers
    std::vector<std::shared_ptr<ILogger>> getLoggers() const;
    
    // Log statistics
    struct LogStats {
        uint64_t totalLogs = 0;
        std::unordered_map<LogLevel, uint64_t> logsByLevel;
        std::unordered_map<std::string, uint64_t> logsByCategory;
        std::chrono::system_clock::time_point firstLog;
        std::chrono::system_clock::time_point lastLog;
    };
    
    LogStats getStats() const;
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    double getAverageLogTime() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced convenience macros with file/line/function info
#define NLM_LOG_FATAL(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Fatal, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_NOTICE(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Notice, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Trace, msg, __FILE__, __LINE__, __func__)

// Context-aware logging macros
#define NLM_LOG_CONTEXT(level, msg, ...) nlm::Logger::getGlobal().logWithContext(level, msg, std::unordered_map<std::string, std::string>{__VA_ARGS__}, __FILE__, __LINE__, __func__)

// Enhanced stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
              const std::string& category = "", const std::string& module = "");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Support for chaining with context
    LogStream& withContext(const std::string& key, const std::string& value);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string category_;
    std::string module_;
    std::unordered_map<std::string, std::string> context_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Logger category enum for better organization
enum class LogCategory {
    GENERAL,
    PLASTICITY,
    DEVELOPMENT,
    NEUROMODULATION,
    MEMORY,
    COGNITION,
    SENSORY,
    MOTOR,
    ENVIRONMENT,
    EXPERIMENT,
    VISUALIZATION,
    CONFIG,
    PERFORMANCE
};

// Helper function to get category from string
LogCategory parseLogCategory(const std::string& categoryStr);

} // namespace nlm
