#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <regex>

namespace nlm {

// Extended log severity levels
enum class LogLevel {
    Trace = 0,          // Very detailed debugging
    Debug = 1,          // General debugging information
    Info = 2,           // General information
    Notice = 3,         // Important events that are not errors
    Warning = 4,        // Warning conditions
    Error = 5,          // Error conditions
    Critical = 6,       // Critical conditions
    Alert = 7,          // Action must be taken immediately
    Emergency = 8       // System is unusable
};

// Log entry with extended metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> context;
    
    LogEntry() : level(LogLevel::Info), line(0) {}
};

// Log filter interface
class ILogFilter {
public:
    virtual ~ILogFilter() = default;
    virtual bool shouldLog(const LogEntry& entry) const = 0;
};

// Simple pattern-based filter
class PatternFilter : public ILogFilter {
public:
    PatternFilter(const std::vector<std::string>& patterns, bool exclude = false);
    
    bool shouldLog(const LogEntry& entry) const override;
    
    void addPattern(const std::string& pattern, bool exclude = false);
    void clearPatterns();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Log sink interface with context
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Set custom formatting
    virtual void setFormatter(std::function<std::string(const LogEntry&)> formatter) = 0;
    
    // Add filter
    virtual void addFilter(std::shared_ptr<ILogFilter> filter) = 0;
    
    // Remove all filters
    virtual void clearFilters() = 0;
};

// Enhanced console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced console features
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Output file management
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Formatting options
    void setFormatter(std::function<std::string(const LogEntry&)> formatter);
    void addFilter(std::shared_ptr<ILogFilter> filter) override;
    void clearFilters() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Main Logger class with enhanced features
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
    
    // Global logging functions with context
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", 
             const std::unordered_map<std::string, std::string>& context = {});
    
    // Convenience methods with context
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::unordered_map<std::string, std::string>& context = {});
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::unordered_map<std::string, std::string>& context = {});
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::unordered_map<std::string, std::string>& context = {});
    void notice(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::unordered_map<std::string, std::string>& context = {});
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::unordered_map<std::string, std::string>& context = {});
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::unordered_map<std::string, std::string>& context = {});
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::unordered_map<std::string, std::string>& context = {});
    void alert(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::unordered_map<std::string, std::string>& context = {});
    void emergency(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::unordered_map<std::string, std::string>& context = {});
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Performance timing helper
    class Timer {
    public:
        Timer(const std::string& name, LogLevel level = LogLevel::Debug);
        ~Timer();
        
        // Add context to timer
        Timer& setContext(const std::string& key, const std::string& value);
        
        // Add elapsed time to context
        void recordElapsed();
        
    private:
        std::string name_;
        LogLevel level_;
        std::unordered_map<std::string, std::string> context_;
        std::chrono::system_clock::time_point start_;
    };
    
    // Convenience macro for timed operations
    #define NLM_TIME_OPERATION(operation_name) \
        nlm::Logger::Timer timer_##operation_name(#operation_name); \
        try { \
            operation_name; \
            timer_##operation_name.recordElapsed(); \
        } catch (...) { \
            timer_##operation_name.setContext("status", "failed"); \
            throw; \
        }
    
    // Convenience macros with context
    #define NLM_LOG_INFO_WITH_CTX(msg, ...) \
        nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, buildContext(__VA_ARGS__))
    
    #define NLM_LOG_ERROR_WITH_CTX(msg, ...) \
        nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__, buildContext(__VA_ARGS__))
    
    // Helper for building context maps
    static std::unordered_map<std::string, std::string> buildContext(std::initializer_list<std::pair<std::string, std::string>> pairs);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Stream-style logging helper with context
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add context to the log entry
    LogStream& addContext(const std::string& key, const std::string& value);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::unordered_map<std::string, std::string> context_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
