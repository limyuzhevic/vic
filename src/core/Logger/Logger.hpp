#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <map>
#include <mutex>
#include <filesystem>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Structured log field type
using LogField = std::variant<bool, int64_t, double, std::string>;

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, LogField> fields;
};

// Log rotation policy
enum class LogRotation {
    None,
    SizeBased,
    TimeBased,
    SizeAndTime
};

// Log format
enum class LogFormat {
    Text,
    JSON,
    Structured,
    Compact
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

#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <map>
#include <mutex>
#include <filesystem>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Structured log field type
using LogField = std::variant<bool, int64_t, double, std::string>;

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, LogField> fields;
};

// Log rotation policy
enum class LogRotation {
    None,
    SizeBased,
    TimeBased,
    SizeAndTime
};

// Log format
enum class LogFormat {
    Text,
    JSON,
    Structured,
    Compact
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setFormat(LogFormat format) = 0;
    virtual LogFormat getFormat() const = 0;
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
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Enhanced features
    void setMaxFileSize(size_t maxSize);
    void setRotationPolicy(LogRotation policy, size_t maxSize = 10 * 1024 * 1024, const std::string& timePattern = "%Y%m%d");
    void setFormat(LogFormat format) override;
    LogFormat getFormat() const override;
    bool shouldRotate();
    void rotateIfNeeded();
    std::string getLogFilePath() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatEntry(const LogEntry& entry) const;
    std::string formatEntryJSON(const LogEntry& entry) const;
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
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Structured logging
    void logStructured(LogLevel level, const std::string& message,
                      const std::map<std::string, LogField>& fields,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Structured logging convenience methods
    void debugStructured(const std::string& message, const std::map<std::string, LogField>& fields = {}, const std::string& file = "", int line = 0, const std::string& function = "");
    void infoStructured(const std::string& message, const std::map<std::string, LogField>& fields = {}, const std::string& file = "", int line = 0, const std::string& function = "");
    void warningStructured(const std::string& message, const std::map<std::string, LogField>& fields = {}, const std::string& file = "", int line = 0, const std::string& function = "");
    void errorStructured(const std::string& message, const std::map<std::string, LogField>& fields = {}, const std::string& file = "", int line = 0, const std::string& function = "");
    void criticalStructured(const std::string& message, const std::map<std::string, LogField>& fields = {}, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Log file management
    void setLogFile(const std::string& filepath);
    void closeLogFile();
    std::string getLogFilePath() const;
    
    // Configuration
    void setAutoCreateDirs(bool autoCreate);
    void setDefaultLogFormat(LogFormat format);
    void setMaxFileSize(size_t maxSize);
    void setRotationPolicy(LogRotation policy, size_t maxSize = 10 * 1024 * 1024, const std::string& timePattern = "%Y%m%d");

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Structured logging macros
#define NLM_LOG_DEBUG_STRUCT(msg, fields) nlm::Logger::getGlobal().debugStructured(msg, fields, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO_STRUCT(msg, fields) nlm::Logger::getGlobal().infoStructured(msg, fields, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING_STRUCT(msg, fields) nlm::Logger::getGlobal().warningStructured(msg, fields, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR_STRUCT(msg, fields) nlm::Logger::getGlobal().errorStructured(msg, fields, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL_STRUCT(msg, fields) nlm::Logger::getGlobal().criticalStructured(msg, fields, __FILE__, __LINE__, __func__)

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
