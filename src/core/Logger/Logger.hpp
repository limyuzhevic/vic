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
    
    // Enable/disable colored output
    virtual void setUseColors(bool useColors) {}
    virtual bool getUseColors() const { return false; }
    
    // Set output file for log rotation
    virtual void setOutputFile(const std::string& filepath) {}
    virtual void closeOutputFile() {}
    
    // Configure log rotation
    virtual void enableLogRotation(bool enable) {}
    virtual void setMaxFileSize(size_t maxSize) {}
    virtual void setTimestampFormat(const std::string& format) {}
    
    // Configure log filtering
    virtual void setLevelFilter(LogLevel level, bool enabled) {}
    virtual bool getLevelFilter(LogLevel level) const { return true; }
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
    
    // Configure log rotation
    void enableLogRotation(bool enable);
    void setMaxFileSize(size_t maxSize);
    void setTimestampFormat(const std::string& format);
    
    // Configure log filtering
    void setLevelFilter(LogLevel level, bool enabled) override;
    bool getLevelFilter(LogLevel level) const override;
    
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
    
    // Configure global logger settings
    static void configureGlobalLogger(LogLevel level, bool useColors = true);
    static void addGlobalLogFile(const std::string& filepath, bool enableRotation = false);
    
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
    
    template<typename T>
    LogStream& operator<<(const std::vector<T>& values) {
        oss_ << "[";
        for (size_t i = 0; i < values.size(); ++i) {
            if (i > 0) oss_ << ", ";
            oss_ << values[i];
        }
        oss_ << "]";
        return *this;
    }
    
    template<typename T>
    LogStream& operator<<(const std::unordered_map<std::string, T>& map) {
        oss_ << "{";
        for (auto it = map.begin(); it != map.end(); ++it) {
            if (it != map.begin()) oss_ << ", ";
            oss_ << it->first << ": " << it->second;
        }
        oss_ << "}";
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

// Structured logging macros
#define NLM_LOG_OBJECT(obj) NLM_LOG_STREAM(nlm::LogLevel::Info) << obj
#define NLM_LOG_VECTOR(vec) NLM_LOG_STREAM(nlm::LogLevel::Info) << vec
#define NLM_LOG_MAP(map) NLM_LOG_STREAM(nlm::LogLevel::Info) << map
