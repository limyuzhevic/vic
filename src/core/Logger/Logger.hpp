#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

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
    std::string component;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string context;  // Additional context for errors/warnings
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
    
    // Global logging functions with component identification
    void log(LogLevel level, const std::string& component, const std::string& message,
             const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::string& context = "");
    
    // Convenience methods
    void debug(const std::string& component, const std::string& message, 
               const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& component, const std::string& message, 
              const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& component, const std::string& message,
                 const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& component, const std::string& message,
               const std::string& file = "", int line = 0, const std::string& function = "", const std::string& context = "");
    void critical(const std::string& component, const std::string& message,
                  const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Standard logging macros with component identification
#define NLM_LOG_DEBUG(component, msg) nlm::Logger::getGlobal().debug(component, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(component, msg) nlm::Logger::getGlobal().info(component, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(component, msg) nlm::Logger::getGlobal().warning(component, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(component, msg, ctx) nlm::Logger::getGlobal().error(component, msg, __FILE__, __LINE__, __func__, ctx)
#define NLM_LOG_ERROR_SIMPLE(component, msg) nlm::Logger::getGlobal().error(component, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(component, msg) nlm::Logger::getGlobal().critical(component, msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper with component support
class LogStream {
public:
    LogStream(LogLevel level, const std::string& component, const std::string& file, 
              int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
private:
    LogLevel level_;
    std::string component_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_STREAM(component, level) nlm::LogStream(level, component, __FILE__, __LINE__, __func__)
