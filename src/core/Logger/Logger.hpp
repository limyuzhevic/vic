#pragma once
#include "../Exceptions/NLMErrors.hpp"

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>

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
    std::unordered_map<std::string, std::string> metadata;
    std::shared_ptr<nlm::NLMException> exception;
    std::string stackTrace;
    bool isStructured;
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
class ConsoleLogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger();
    
    void log(const LogEntry& entry);
    void setLevel(LogLevel level);
    LogLevel getLevel() const;
    
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
    
    // Exception-aware logging methods
    void logException(const std::shared_ptr<nlm::NLMException>& exception, 
                     const std::string& context = "");
    void throwOnError(const std::shared_ptr<nlm::NLMException>& exception, 
                      const std::string& context = "");
    
    // Exception chaining support
    void logExceptionChain(const std::shared_ptr<nlm::NLMException>& exception,
                         bool includeStackTrace = true);
    
    // Formatted logging with parameter substitution
    void formatLog(const std::string& format, const std::vector<std::string>& params,
                  LogLevel level, const std::string& file = "", int line = 0, 
                  const std::string& function = "");
    
    // Structured logging with context
    void logWithContext(const std::string& message, LogLevel level,
                       const std::unordered_map<std::string, std::string>& context,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "");
    
    // Batch logging
    void logBatch(const std::vector<LogEntry>& entries);
    
    // Thread-safe access for LogStream
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
        ~LogStream();
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            oss_ << value;
            return *this;
        }
        
        // Add metadata
        LogStream& addMetadata(const std::string& key, const std::string& value);
        
        // Add exception
        LogStream& addException(std::shared_ptr<nlm::NLMException> exception);
        
        // Add stack trace
        LogStream& addStackTrace(const std::string& stackTrace);
        
        // Enable structured logging
        LogStream& enableStructuredLogging();
        
        // Format with parameters
        std::string formatWithParams(const std::string& format) const;
        
    private:
        LogLevel level_;
        std::ostringstream oss_;
        std::string file_;
        int line_;
        std::string function_;
        std::unordered_map<std::string, std::string> metadata_;
        std::shared_ptr<nlm::NLMException> exception_;
        std::string stackTrace_;
        bool isStructured_;
        
        // Thread-safe access
        mutable std::mutex mutex_;
    };
    
    // Create log stream
    static LogStream createLogStream(LogLevel level, 
                                    const std::string& file = "",
                                    int line = 0,
                                    const std::string& function = "");
    
    // Stack trace capture
    static std::string captureStackTrace();
    
    // Source location capture
    static std::source_location getCurrentSourceLocation();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for logging
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging macro
#define NLM_LOG_STREAM(level) nlm::Logger::createLogStream(level, __FILE__, __LINE__, __func__)

// Exception-aware logging macros
#define NLM_LOG_EXCEPTION(exc, context) nlm::Logger::getGlobal().logException(exc, context)
#define NLM_THROW_ON_ERROR(exc, context) nlm::Logger::getGlobal().throwOnError(exc, context)
#define NLM_LOG_EXCEPTION_CHAIN(exc) nlm::Logger::getGlobal().logExceptionChain(exc)
#define NLM_LOG_WITH_CONTEXT(msg, level, context) nlm::Logger::getGlobal().logWithContext(msg, level, context, __FILE__, __LINE__, __func__)
#define NLM_FORMAT_LOG(format, params, level) nlm::Logger::getGlobal().formatLog(format, params, level, __FILE__, __LINE__, __func__)

// Structured logging macros
#define NLM_LOG_STREAM_STRUCT(level) nlm::Logger::createLogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
