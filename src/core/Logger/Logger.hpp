#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <iomanip>
#include <fstream>
#include <queue>
#include <thread>
#include <atomic>
#include <map>
#include <set>

namespace nlm {

// Enhanced log severity levels with more granularity
enum class LogLevel {
    Trace = 0,           // Detailed debugging information
    Debug = 1,           // General debugging information
    Info = 2,            // General information messages
    Notice = 3,          // Important events that are not errors
    Warning = 4,         // Warning conditions
    Error = 5,           // Error conditions
    Critical = 6,        // Critical conditions
    Alert = 7,           // Action must be taken immediately
    Emergency = 8,       // System is unusable
    Custom = 99          // User-defined level
};

// Log category for better filtering
enum class LogCategory {
    Core = 0,            // Core system operations
    Brain = 1,           // Brain operations and neural computation
    Memory = 2,          // Memory systems
    Plasticity = 3,      // Plasticity and learning
    Development = 4,     // Development processes
    Sensory = 5,         // Sensory processing
    Motor = 6,           // Motor system
    Cognition = 7,       // Cognitive processes
    Prediction = 8,      // Prediction systems
    Neuromodulation = 9, // Neuromodulation
    MemoryManagement = 10, // Memory management
    Performance = 11,    // Performance metrics
    Configuration = 12,  // Configuration system
    Python = 13,         // Python bindings
    User = 14,           // User application code
    System = 15,         // System-level operations
    Custom = 99          // User-defined category
};

// Enhanced log entry with more metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    LogCategory category;
    std::string tag;
    std::map<std::string, std::string> fields;
    
    LogEntry() : level(LogLevel::Info), line(0), category(LogCategory::Core) {}
    
    std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%f");
        return ss.str();
    }
};

// Enhanced logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setCategoryFilter(const std::set<LogCategory>& categories) = 0;
    virtual std::set<LogCategory> getCategoryFilter() const = 0;
    virtual void setTagFilter(const std::set<std::string>& tags) = 0;
    virtual std::set<std::string> getTagFilter() const = 0;
    virtual void setOutputFormat(const std::string& format) = 0;
    virtual std::string getOutputFormat() const = 0;
};

// Enhanced console logger with better formatting and cross-platform support
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced color support with cross-platform fallback
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setColorScheme(const std::map<LogLevel, std::string>& colors);
    
    // Enhanced file output with rotation and size limits
    void setOutputFile(const std::string& filepath, size_t maxFileSize = 10 * 1024 * 1024, int maxFiles = 5);
    void closeOutputFile();
    bool getOutputFileInfo(std::string& filepath, uint64_t& fileSize, int64_t& lastModified) const;
    
    // Enhanced formatting
    void setPattern(const std::string& pattern);
    void setTimeFormat(const std::string& format);
    void setIncludeThreadId(bool include);
    
    // Async logging support
    void enableAsyncLogging(size_t queueSize = 10000, int workerThreads = 1);
    bool isAsyncLoggingEnabled() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatMessage(const LogEntry& entry) const;
};

// File logger with rotation support
class FileLogger : public ILogger {
public:
    FileLogger();
    explicit FileLogger(const std::string& filepath, LogLevel level = LogLevel::Info);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // File management
    void setFilepath(const std::string& filepath);
    void setMaxFileSize(size_t maxSize);
    void setMaxBackupFiles(int maxFiles);
    void setFlushOnWrite(bool flush);
    
    // File info
    bool getFileInfo(std::string& filepath, uint64_t& fileSize, int64_t& lastModified) const;
    void rotateFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Rotating file logger with automatic rotation
class RotatingFileLogger : public ILogger {
public:
    RotatingFileLogger(const std::string& baseFilename, size_t maxFileSize = 10 * 1024 * 1024, int maxBackupFiles = 5);
    ~RotatingFileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // File management
    void setBaseFilename(const std::string& filename);
    std::string getCurrentFilename() const;
    bool rotateImmediately();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced main Logger class with async logging support
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink with validation
    bool addLogger(std::shared_ptr<ILogger> logger);
    bool removeLogger(ILogger* logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Enhanced logging methods with category and tag support
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "",
             LogCategory category = LogCategory::Core,
             const std::string& tag = "");
    
    // Field-based logging for structured data
    void logWithFields(LogLevel level, const std::string& message,
                      const std::map<std::string, std::string>& fields,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "");
    
    // Convenience methods with category support
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
               LogCategory category = LogCategory::Core, const std::string& tag = "");
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
               LogCategory category = LogCategory::Core, const std::string& tag = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
              LogCategory category = LogCategory::Core, const std::string& tag = "");
    void notice(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
                LogCategory category = LogCategory::Core, const std::string& tag = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
                 LogCategory category = LogCategory::Core, const std::string& tag = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
               LogCategory category = LogCategory::Core, const std::string& tag = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
                  LogCategory category = LogCategory::Core, const std::string& tag = "");
    void alert(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
               LogCategory category = LogCategory::Core, const std::string& tag = "");
    void emergency(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
                   LogCategory category = LogCategory::Core, const std::string& tag = "");
    
    // Check if level is enabled in any logger
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Async logging configuration
    void enableAsyncLogging(size_t queueSize = 10000, int workerThreads = 1);
    bool isAsyncLoggingEnabled() const;
    
    // Configuration
    void setDefaultPattern(const std::string& pattern);
    void setTimeFormat(const std::string& format);
    void setIncludeThreadId(bool include);
    
    // Statistics
    size_t getLoggerCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
              LogCategory category = LogCategory::Core, const std::string& tag = "");
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Field support
    LogStream& operator<<(const std::pair<std::string, std::string>& field);
    
private:
    LogLevel level_;
    std::string file_;
    int line_;
    std::string function_;
    std::ostringstream oss_;
    LogCategory category_;
    std::string tag_;
};

// Convenience macros with enhanced features
#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Trace, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Debug, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Info, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_NOTICE(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Notice, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Warning, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Error, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Critical, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ALERT(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Alert, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_EMERGENCY(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Emergency, msg, __FILE__, __LINE__, __func__)

// Category-specific macros
#define NLM_LOG_CORE(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Info, msg, __FILE__, __LINE__, __func__, nlm::LogCategory::Core)
#define NLM_LOG_BRAIN(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Info, msg, __FILE__, __LINE__, __func__, nlm::LogCategory::Brain)
#define NLM_LOG_MEMORY(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Info, msg, __FILE__, __LINE__, __func__, nlm::LogCategory::Memory)
#define NLM_LOG_PLASTICITY(msg) nlm::Logger::getGlobal().log(nlm::LogLevel::Info, msg, __FILE__, __LINE__, __func__, nlm::LogCategory::Plasticity)

// Enhanced stream macros with category and tag support
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Python bindings friendly macros
#define NLM_LOG_WITH_CATEGORY(level, category, msg) nlm::Logger::getGlobal().log(level, msg, __FILE__, __LINE__, __func__, category)
#define NLM_LOG_WITH_TAG(level, tag, msg) nlm::Logger::getGlobal().log(level, msg, __FILE__, __LINE__, __func__, nlm::LogCategory::Core, tag)

} // namespace nlm

// Version function for backwards compatibility
namespace nlm {
    std::string getLoggerVersion();
}