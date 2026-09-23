#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <filesystem>
#include <optional>
#include <atomic>
#include <shared_mutex>

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
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Virtual destructor for proper cleanup
    virtual void close() {}
};

// Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger() = default;
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    // Disable copying, enable moving
    ConsoleLogger(const ConsoleLogger&) = delete;
    ConsoleLogger& operator=(const ConsoleLogger&) = delete;
    ConsoleLogger(ConsoleLogger&&) noexcept;
    ConsoleLogger& operator=(ConsoleLogger&&) noexcept;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Get current log count
    size_t getLogCount() const;
    
    // Close all resources
    void close() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatLogEntry(const LogEntry& entry) const;
    static std::string extractFilename(const std::string& filepath);
};

// Main Logger class
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Enable moving
    Logger(Logger&&) noexcept;
    Logger& operator=(Logger&&) noexcept;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Log with source location information
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
    
    // Get total log count
    size_t getTotalLogCount() const;
    
    // Get session ID
    std::string getSessionId() const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger (advanced usage)
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Close all resources
    void closeAll();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for easy logging
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
    
    // Allow chaining for better logging
    LogStream& operator<<(const std::string& value) {
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

// Helper functions for common operations
namespace LogHelpers {
    // Convert log level to string
    std::string levelToString(LogLevel level);
    
    // Convert log level to ANSI color code
    std::string levelToColor(LogLevel level);
    
    // Extract filename from full path
    std::string extractFilename(const std::string& filepath);
    
    // Check if file exists and is writable
    bool isFileWritable(const std::string& filepath);
    
    // Generate unique session ID
    std::string generateSessionId();
    
    // Get current thread ID as string
    std::string getThreadId();
}

} // namespace nlm
