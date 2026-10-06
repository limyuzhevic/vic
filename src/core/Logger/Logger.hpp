#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <format>
#include <span>
#include <ranges>
#include <expected>
#include <optional>
#include <source_location>

namespace nlm {

// Enhanced log severity levels with C++20 features
enum class LogLevel : uint8_t {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Trace = 5
};

// Helper functions for log level management
constexpr const char* LogLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        case LogLevel::Trace: return "TRACE";
        default: return "UNKNOWN";
    }
}

constexpr bool isValidLogLevel(LogLevel level) {
    return static_cast<uint8_t>(level) <= static_cast<uint8_t>(LogLevel::Trace);
}

// Enhanced log entry structure with C++20 features
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Enhanced constructor with source location (C++20)
    LogEntry(LogLevel lvl, const std::string& msg, 
             std::source_location location = std::source_location::current())
        : level(lvl), message(msg), 
          file(std::filesystem::path(location.file_name()).filename().string()),
          line(location.line()), 
          function(location.function_name()),
          timestamp(std::chrono::system_clock::now()) {}
    
    // String representation using std::format (C++20)
    std::string toString() const {
        return std::format("[{}] {} ({}{}::{})", 
                         LogLevelToString(level), message,
                         file, line, function);
    }
    
    // Check if log entry is valid
    bool isValid() const {
        return isValidLogLevel(level) && !message.empty();
    }
};

// Enhanced logger interface with C++20 features
class ILogger {
public:
    virtual ~ILogger() = default;
    
    // Enhanced logging method with optional error reporting
    virtual std::expected<void, std::string> log(const LogEntry& entry) = 0;
    
    // Enhanced level setting with validation
    virtual std::expected<void, std::string> setLevel(LogLevel level) = 0;
    
    // Enhanced level getting with error reporting
    virtual std::expected<LogLevel, std::string> getLevel() const = 0;
    
    // Check if level is enabled (new C++20 feature)
    virtual bool isEnabled(LogLevel level) const {
        return level >= getLevel().value_or(LogLevel::Info);
    }
};

// Enhanced console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    std::expected<void, std::string> log(const LogEntry& entry) override;
    std::expected<void, std::string> setLevel(LogLevel level) override;
    std::expected<LogLevel, std::string> getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    std::expected<void, std::string> setOutputFile(const std::string& filepath);
    std::expected<void, std::string> closeOutputFile();
    
    // Enhanced string conversion methods using C++20
    static std::string levelToString(LogLevel level) {
        return LogLevelToString(level);
    }
    
    static std::string levelToColor(LogLevel level);
    
    // Enhanced log entry formatting
    std::string formatLogEntry(const LogEntry& entry) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced main Logger class with C++20 features
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink with validation
    std::expected<void, std::string> addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers with validation
    std::expected<void, std::string> setLevel(LogLevel level);
    
    // Enhanced global logging functions with source location support
    std::expected<void, std::string> log(LogLevel level, const std::string& message, 
                                         std::source_location location = std::source_location::current());
    
    // Convenience methods with source location
    void debug(const std::string& message, std::source_location location = std::source_location::current());
    void info(const std::string& message, std::source_location location = std::source_location::current());
    void warning(const std::string& message, std::source_location location = std::source_location::current());
    void error(const std::string& message, std::source_location location = std::source_location::current());
    void critical(const std::string& message, std::source_location location = std::source_location::current());
    
    // Check if level is enabled with enhanced error reporting
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger with validation
    static std::expected<void, std::string> setGlobal(std::shared_ptr<Logger> logger);
    
    // Enhanced utility functions
    static std::string getTimestamp();
    static std::string getFormattedTimestamp(std::chrono::system_clock::time_point tp = std::chrono::system_clock::now());
    
    // Batch log entries (new C++20 feature)
    std::expected<void, std::string> logBatch(const std::vector<LogEntry>& entries);
    
    // Filter and process log entries using views (C++20)
    auto getLogEntriesByLevel(LogLevel level) const {
        return getAllLogEntries() | std::views::filter([level](const LogEntry& entry) {
            return entry.level == level;
        });
    }
    
    // Get all log entries
    std::vector<LogEntry> getAllLogEntries() const;
    
    // Statistics and analysis
    size_t getLogCount(LogLevel level = LogLevel::Info) const;
    std::string getLogStatistics() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced convenience macros using C++20 source location
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, std::source_location::current())
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, std::source_location::current())
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, std::source_location::current())
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, std::source_location::current())
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, std::source_location::current())

// Enhanced stream-style logging helper with C++20 source location
class LogStream {
public:
    LogStream(LogLevel level, std::source_location location = std::source_location::current());
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Flush and log the accumulated message
    std::expected<void, std::string> log();
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::source_location location_;
};

// Enhanced C++20 logging macros
#define NLM_LOG_STREAM(level) nlm::LogStream(level, std::source_location::current())

// Enhanced logging helper functions
namespace log {
    // Structured logging with context
    template<typename... Args>
    void logStructured(LogLevel level, const std::format_string<Args...> fmt, Args&&... args) {
        auto message = std::format(fmt, std::forward<Args>(args)...);
        nlm::Logger::getGlobal().log(level, message, std::source_location::current());
    }
    
    // Convenience functions for structured logging
    template<typename... Args>
    void debug(const std::format_string<Args...> fmt, Args&&... args) {
        logStructured(LogLevel::Debug, fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void info(const std::format_string<Args...> fmt, Args&&... args) {
        logStructured(LogLevel::Info, fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void warning(const std::format_string<Args...> fmt, Args&&... args) {
        logStructured(LogLevel::Warning, fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void error(const std::format_string<Args...> fmt, Args&&... args) {
        logStructured(LogLevel::Error, fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void critical(const std::format_string<Args...> fmt, Args&&... args) {
        logStructured(LogLevel::Critical, fmt, std::forward<Args>(args)...);
    }
}

} // namespace nlm
