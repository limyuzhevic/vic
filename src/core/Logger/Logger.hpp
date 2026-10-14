#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include "../DebugAdvanced.hpp"

namespace nlm {

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
    virtual void logStructured(const StructuredLogEntry& entry) = 0;
    virtual void setCategoryLevel(const std::string& category, DebugLevel level) = 0;
    virtual void addCategory(const std::string& category) = 0;
    virtual std::vector<std::string> getCategories() const = 0;
    virtual void enableJSONOutput(bool enable) = 0;
    virtual void setCorrelationID(const std::string& id) = 0;
    virtual std::string getCurrentCorrelationID() const = 0;
};

// Console logger implementation
class ConsoleLogger : public ILogger, public IAdvancedLogger {
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
    
    // IAdvancedLogger interface implementation
    void logStructured(const StructuredLogEntry& entry) override;
    void setCategoryLevel(const std::string& category, DebugLevel level) override;
    void addCategory(const std::string& category) override;
    std::vector<std::string> getCategories() const override;
    void enableJSONOutput(bool enable) override;
    void setCorrelationID(const std::string& id) override;
    std::string getCurrentCorrelationID() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    
    DebugLevel debugLevelToLogLevel(DebugLevel level) const;
    std::string debugLevelToString(DebugLevel level) const;
};

// Main Logger class
class Logger : public IAdvancedLogger {
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
    
    // IAdvancedLogger interface implementation
    void logStructured(const StructuredLogEntry& entry) override;
    void setCategoryLevel(const std::string& category, DebugLevel level) override;
    void addCategory(const std::string& category) override;
    std::vector<std::string> getCategories() const override;
    void enableJSONOutput(bool enable) override;
    void setCorrelationID(const std::string& id) override;
    std::string getCurrentCorrelationID() const override;
    
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
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
