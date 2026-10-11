#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <fstream>
#include <map>

namespace nlm {

// Enhanced log severity levels
enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5
};

// Log entry structure with additional fields for structured logging
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Additional fields for structured logging
    std::map<std::string, std::string> fields;
    std::string traceId;
    std::string spanId;
};

// Log formatter interface
class ILogFormatter {
public:
    virtual ~ILogFormatter() = default;
    virtual std::string format(const LogEntry& entry) = 0;
    virtual std::string formatJson(const LogEntry& entry) = 0;
    virtual std::string formatLogfmt(const LogEntry& entry) = 0;
};

// ILogger interface for logging sinks
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// Console logger implementation with enhanced features
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
    
    // Log file management with rotation
    void setOutputFile(const std::string& filepath);
    void setMaxLogFileSize(size_t maxSize); // in bytes
    void setMaxLogFiles(int maxFiles);
    void closeOutputFile();
    
    // Log format selection
    void setFormat(ILogFormatter* formatter);
    void setEnableStructured(bool enable);
    
    // Log level control
    bool isEnabled(LogLevel level) const;
    
    // Getters
    const std::string& getOutputFile() const;
    size_t getCurrentLogFileSize() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    void rotateLogFile();
};

// JsonLogFormatter - formats logs as JSON
class JsonLogFormatter : public ILogFormatter {
    
    std::string levelToString(LogLevel level) const;
public:
    JsonLogFormatter();
    ~JsonLogFormatter() override;
    
    std::string format(const LogEntry& entry) override;
    std::string formatJson(const LogEntry& entry) override;
    std::string formatLogfmt(const LogEntry& entry) override;
    
    std::string escapeJsonString(const std::string& str);
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
    
    // Enhanced logging with structured fields
    void log(LogLevel level, const std::string& message,
             const std::map<std::string, std::string>& fields,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    
    // Convenience methods
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
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
    
    // Configuration
    void setDefaultFormat(ILogFormatter* formatter);
    void setEnableStructuredLogging(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros
#define NLM_LOG_TRACE(msg) nlm::Logger::getGlobal().trace(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Structured logging macro
#define NLM_LOG_STRUCT(level, msg, ...) \
    do { \
        std::map<std::string, std::string> fields = {}; \
        /* Parse additional fields here */ \
        nlm::Logger::getGlobal().log(level, msg, fields, __FILE__, __LINE__, __func__); \
    } while(0)

} // namespace nlm
