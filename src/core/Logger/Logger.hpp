#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <format>
#include <fstream>
#include <deque>
#include <regex>
#include <filesystem>
#include <stdexcept>

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
    std::unordered_map<std::string, std::string> metadata;  // Structured logging support
    
    LogEntry() : level(LogLevel::Info), line(0) {}
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", 
             int line = 0, const std::string& function = "")
        : level(lvl), message(msg), file(file), line(line), function(function),
          timestamp(std::chrono::system_clock::now()) {}
};

// Structured log entry with metadata
struct StructuredLogEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string level;
    std::string message;
    std::unordered_map<std::string, std::string> metadata;
    std::string file;
    int line;
    std::string function;
};

// Log validation error
struct LogValidationError {
    std::string path;
    std::string message;
    std::string expected;
    std::string actual;
};

// Log configuration validation function type
using LogValidator = std::function<bool(const std::string&, const LogLevel&, std::vector<LogValidationError>&)>;

// Log entry validator for level-based filtering
struct LogEntryValidator {
    LogLevel minLevel;
    std::vector<LogValidator> levelValidators;
    std::vector<std::string> forbiddenPatterns;
    std::vector<std::string> requiredPatterns;
    std::unordered_map<std::string, std::string> requiredMetadata;
    std::string logFile;
    size_t maxFileSize;
    size_t maxFiles;
    bool validateFilePaths;
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setMetadata(const std::string& key, const std::string& value) = 0;
    virtual std::string getMetadata(const std::string& key) const = 0;
    virtual void clearMetadata() = 0;
    virtual size_t getLogCount() const = 0;
    virtual std::vector<LogEntry> getRecentLogs(size_t limit = 100) const = 0;
    
    // Log validation interface
    virtual bool validateEntry(const LogEntry& entry) const = 0;
    virtual std::vector<LogValidationError> getValidationErrors(const LogEntry& entry) const = 0;
};

// Enhanced Console logger implementation with thread safety and log rotation
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
    
    // Thread-safe log rotation
    void enableLogRotation(size_t maxFileSize = 10 * 1024 * 1024,  // 10MB default
                           size_t maxFiles = 5);
    void disableLogRotation();
    bool isLogRotationEnabled() const;
    
    // Get current log file size
    size_t getLogFileSize() const;
    
    // Structured logging support
    void setMetadata(const std::string& key, const std::string& value) override;
    std::string getMetadata(const std::string& key) const override;
    void clearMetadata() override;
    size_t getLogCount() const override;
    std::vector<LogEntry> getRecentLogs(size_t limit = 100) const override;
    
    // Log validation interface
    bool validateEntry(const LogEntry& entry) const override;
    std::vector<LogValidationError> getValidationErrors(const LogEntry& entry) const override;
    
    // Log validation configuration
    void addLogValidator(const LogValidator& validator);
    void setForbiddenPatterns(const std::vector<std::string>& patterns);
    void setRequiredPatterns(const std::vector<std::string>& patterns);
    void setRequiredMetadata(const std::unordered_map<std::string, std::string>& metadata);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    void rotateLogFileIfNeeded();
    bool validateFilePath(const std::string& filepath) const;
};

// Main Logger class with thread-safe logging
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
    
    // Structured logging with metadata
    void logWithMetadata(LogLevel level, const std::string& message,
                        const std::unordered_map<std::string, std::string>& metadata,
                        const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Template-based logging for different types
    template<typename T>
    void logValue(LogLevel level, const std::string& key, const T& value,
                  const std::string& file = "", int line = 0, const std::string& function = "") {
        std::ostringstream oss;
        oss << value;
        std::unordered_map<std::string, std::string> metadata;
        metadata[key] = oss.str();
        logWithMetadata(level, "Metric", metadata, file, line, function);
    }
    
    // Log validation interface
    bool validateEntry(const LogEntry& entry) const;
    std::vector<LogValidationError> getValidationErrors(const LogEntry& entry) const;
    
    // Log validation configuration
    void addLogValidator(const LogValidator& validator);
    void setForbiddenPatterns(const std::vector<std::string>& patterns);
    void setRequiredPatterns(const std::vector<std::string>& patterns);
    void setRequiredMetadata(const std::unordered_map<std::string, std::string>& metadata);
    
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
    
    // Allow chaining of multiple values
    LogStream& operator<<(const std::string& value) {
        oss_ << value;
        return *this;
    }
    
    // Support for structured metadata
    template<typename T>
    LogStream& addMetadata(const std::string& key, const T& value) {
        std::ostringstream oss;
        oss << value;
        metadata_[key] = oss.str();
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::unordered_map<std::string, std::string> metadata_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// File-based logger implementation
class FileLogger : public ILogger {
public:
    FileLogger(const std::string& filepath, LogLevel level = LogLevel::Info);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void setMetadata(const std::string& key, const std::string& value) override;
    std::string getMetadata(const std::string& key) const override;
    void clearMetadata() override;
    size_t getLogCount() const override;
    std::vector<LogEntry> getRecentLogs(size_t limit = 100) const override;
    
    void flush();
    void close();
    
    // Log validation interface
    bool validateEntry(const LogEntry& entry) const override;
    std::vector<LogValidationError> getValidationErrors(const LogEntry& entry) const override;
    
    // Log validation configuration
    void addLogValidator(const LogValidator& validator);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
