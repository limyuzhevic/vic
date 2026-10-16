#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
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
    
    // Optional features
    virtual void setUseColors(bool useColors) {}
    virtual bool getUseColors() const { return false; }
    
    virtual void setUseTimestamps(bool useTimestamps) {}
    virtual bool getUseTimestamps() const { return false; }
    
    virtual void setIncludeThreadId(bool includeThreadId) {}
    virtual bool getIncludeThreadId() const { return false; }
    
    virtual void setLogFormat(const std::string& format) {}
    virtual std::string getLogFormat() const { return "[%LEVEL%] %TIMESTAMP% %MESSAGE%"; }
    
    virtual void setOutputFile(const std::string& filepath) {}
    virtual void closeOutputFile() {}
    
    virtual void setFileRotation(size_t maxSizeMB, size_t rotationCount) {}
    virtual void setEnablePerformanceTiming(bool enable) {}
    
    virtual void startTimer(const std::string& name) {}
    virtual void stopTimer(const std::string& name) {}
    virtual void logPerformanceData() {}
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
    void setUseColors(bool useColors) override;
    bool getUseColors() const override;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath) override;
    void closeOutputFile() override;
    
    // Advanced features
    void setUseTimestamps(bool useTimestamps) override;
    bool getUseTimestamps() const override;
    
    void setIncludeThreadId(bool includeThreadId) override;
    bool getIncludeThreadId() const override;
    
    void setLogFormat(const std::string& format) override;
    std::string getLogFormat() const override;
    
    void setFileRotation(size_t maxSizeMB, size_t rotationCount) override;
    void setEnablePerformanceTiming(bool enable) override;
    
    void startTimer(const std::string& name) override;
    void stopTimer(const std::string& name) override;
    void logPerformanceData() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    
    std::string formatMessage(const LogEntry& entry) const;
    void rotateLogFile();
    std::string extractFileIndex(const std::string& filepath) const;
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
    
    // Console logger convenience methods
    void setConsoleLevel(LogLevel level);
    void enableConsoleColors(bool enable);
    void enableConsoleTimestamps(bool enable);
    void enableThreadIds(bool enable);
    void setConsoleLogFormat(const std::string& format);
    void setConsoleFileOutput(const std::string& filepath);
    void setConsoleFileRotation(size_t maxSizeMB, size_t rotationCount);
    void enableConsolePerformanceTiming(bool enable);
    void startTimer(const std::string& name);
    void stopTimer(const std::string& name);
    void logConsolePerformanceData();
    
    // Log format constants
    static const std::string DEFAULT_LOG_FORMAT;
    static const std::string SIMPLE_LOG_FORMAT;
    static const std::string DETAILED_LOG_FORMAT;
    static const std::string COLOR_LOG_FORMAT;

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
    
    // Add newline to complete the log entry
    LogStream& endl() {
        oss_ << std::endl;
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