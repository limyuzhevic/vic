#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <filesystem>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <algorithm>
#include <unordered_map>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure with enhanced error context
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    // Enhanced error context
    std::string errorCode;
    std::string stackTrace;
    std::unordered_map<std::string, std::string> metadata;
    std::string category;
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void logStructured(const LogEntry& entry, const std::string& format) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// Custom formatter implementation
class LogFormatter {
public:
    virtual ~LogFormatter() = default;
    virtual std::string format(const LogEntry& entry) = 0;
};

class SimpleFormatter : public LogFormatter {
public:
    std::string format(const LogEntry& entry) override;
};

class JsonFormatter : public LogFormatter {
public:
    std::string format(const LogEntry& entry) override;
};

// Async logging queue implementation
class AsyncLogger {
public:
    AsyncLogger(size_t queueSize = 10000);
    ~AsyncLogger();
    
    void log(const LogEntry& entry);
    void stop();
    void setLogger(std::shared_ptr<ILogger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// File logger implementation with log rotation
class FileLogger : public ILogger {
public:
    FileLogger(const std::string& filepath, size_t maxFileSize = 10 * 1024 * 1024, // 10MB
               int maxFiles = 5, LogLevel level = LogLevel::Info,
               std::shared_ptr<LogFormatter> formatter = nullptr);
    ~FileLogger() override;
    
    void log(const LogEntry& entry) override;
    void logStructured(const LogEntry& entry, const std::string& format) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Rotate logs immediately
    void rotate();
    
    // Get current log file path
    std::string getCurrentFilePath() const;
    
    // Set formatter
    void setFormatter(std::shared_ptr<LogFormatter> formatter);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced Console logger implementation with better error handling
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void logStructured(const LogEntry& entry, const std::string& format) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Set formatter
    void setFormatter(std::shared_ptr<LogFormatter> formatter);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced Main Logger class with metadata and error context
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
    
    // Enhanced logging with error context
    void error(const std::string& message, const std::string& errorCode,
               const std::string& file = "", int line = 0,
               const std::string& function = "", const std::string& stackTrace = "");
    
    // Log with metadata
    void logWithMetadata(LogLevel level, const std::string& message,
                         const std::unordered_map<std::string, std::string>& metadata,
                         const std::string& file = "", int line = 0,
                         const std::string& function = "");
    
    // Log categorization
    void logWithCategory(LogLevel level, const std::string& message,
                        const std::string& category,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced Stream-style logging helper with metadata support
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add metadata to log entry
    LogStream& metadata(const std::string& key, const std::string& value);
    
    // Add error context
    LogStream& errorCode(const std::string& code);
    LogStream& stackTrace(const std::string& trace);
    
    // Add category
    LogStream& category(const std::string& category);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::unordered_map<std::string, std::string> metadata_;
    std::string errorCode_;
    std::string stackTrace_;
    std::string category_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Log categorization system
class LogCategory {
public:
    enum Category {
        System,
        User,
        Network,
        Database,
        Performance,
        Security,
        Development,
        Custom
    };
    
    LogCategory(Category category, const std::string& name) 
        : category_(category), name_(name) {}
    
    Category getCategory() const { return category_; }
    const std::string& getName() const { return name_; }
    
    static std::string categoryToString(Category category);
    
private:
    Category category_;
    std::string name_;
};

} // namespace nlm