#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <mutex>
#include "../Error.hpp"

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
    
    LogEntry(LogLevel lvl, const std::string& msg, 
             const std::string& file = "", int line = 0,
             const std::string& function = "")
        : level(lvl), message(msg), file(file), line(line), 
          function(function), timestamp(std::chrono::system_clock::now()) {}
};

// Logger exception types
class LoggerError : public NLMError {
public:
    LoggerError(const std::string& message, const std::string& context = "", int errorCode = 0)
        : NLMError(message, NLMErrorCategory::System, context, errorCode) {}
};

class LoggerConfigurationError : public NLMError {
public:
    LoggerConfigurationError(const std::string& message, const std::string& context = "", int errorCode = 0)
        : NLMError(message, NLMErrorCategory::Configuration, context, errorCode) {}
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    
    // Log entry - may throw LoggerError
    virtual void log(const LogEntry& entry) = 0;
    
    // Set minimum level - may throw LoggerConfigurationError
    virtual void setLevel(LogLevel level) = 0;
    
    // Get current level
    virtual LogLevel getLevel() const = 0;
    
    // Enable/disable logger - may throw LoggerError
    virtual void enable(bool enabled) = 0;
    virtual bool isEnabled() const = 0;
};

// Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    // Prevent copying
    ConsoleLogger(const ConsoleLogger&) = delete;
    ConsoleLogger& operator=(const ConsoleLogger&) = delete;
    
    // Enable moving
    ConsoleLogger(ConsoleLogger&&) noexcept;
    ConsoleLogger& operator=(ConsoleLogger&&) noexcept;
    
    // ILogger interface
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void enable(bool enabled) override;
    bool isEnabled() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Flush output
    void flush();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    void logToConsole(const LogEntry& entry);
};

// File logger implementation
class FileLogger : public ILogger {
public:
    FileLogger(const std::string& filepath, bool append = true);
    ~FileLogger() override;
    
    // Prevent copying
    FileLogger(const FileLogger&) = delete;
    FileLogger& operator=(const FileLogger&) = delete;
    
    // Enable moving
    FileLogger(FileLogger&&) noexcept;
    FileLogger& operator=(FileLogger&&) noexcept;
    
    // ILogger interface
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void enable(bool enabled) override;
    bool isEnabled() const override;
    
    // Close file
    void close();
    
    // Get file path
    std::string getFilePath() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class with exception safety
class Logger {
public:
    Logger();
    ~Logger();
    
    // Prevent copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Enable moving
    Logger(Logger&&) noexcept;
    Logger& operator=(Logger&&) noexcept;
    
    // Add logger sink with exception safety
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove specific logger
    bool removeLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions with exception safety
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods with exception safety
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance (exception safe)
    static Logger& getGlobal();
    
    // Set global logger (exception safe)
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Get all active loggers
    std::vector<std::shared_ptr<ILogger>> getActiveLoggers() const;
    
    // Check if logger is healthy (all sinks available)
    bool isHealthy() const;
    
    // Get logger statistics
    struct Statistics {
        size_t totalLogEntries{0};
        size_t errorEntries{0};
        size_t warningEntries{0};
        size_t debugEntries{0};
        size_t infoEntries{0};
        size_t criticalEntries{0};
        size_t failedLogAttempts{0};
    };
    
    Statistics getStatistics() const;
    
    // Reset statistics
    void resetStatistics();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void logToAllSinks(const LogEntry& entry);
    void addLoggerImpl(std::shared_ptr<ILogger> logger);
    void removeLoggerImpl(std::shared_ptr<ILogger> logger);
};

// Global logging macros with enhanced error handling
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced logging macros with error checking
#define NLM_SAFE_LOG_DEBUG(msg) do { try { nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__); } catch (const nlm::NLMError&) { /* Silently fail for debug logs */ } } while(0)

#define NLM_SAFE_LOG_ERROR(msg) do { try { nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__); } catch (const nlm::NLMError&) { /* Ensure error logging is safe */ } } while(0)

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
    
    // Exception-safe stream with error handling
    LogStream& safeWrite(const std::string& message);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    bool shouldLog_;
    
    void tryLog();
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Custom log handler for error recovery
class ErrorLogHandler {
public:
    virtual ~ErrorLogHandler() = default;
    virtual void onLogError(const LogEntry& entry) = 0;
    virtual void onLogFailure(const std::exception& e) = 0;
};

// Global error handler management
void registerErrorHandler(std::shared_ptr<ErrorLogHandler> handler);
void unregisterErrorHandler(std::shared_ptr<ErrorLogHandler> handler);

// Safe logging wrapper with error recovery
void safeLog(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0,
             const std::string& function = "");

} // namespace nlm

