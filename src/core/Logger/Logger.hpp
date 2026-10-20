#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <thread>
#include <nlohmann/json.hpp>

namespace nlm {

// Enhanced log severity levels
enum class LogLevel {
    Off = -1,
    Trace = 0,
    Debug = 1,
    Info = 2,
    Notice = 3,
    Warning = 4,
    Error = 5,
    Critical = 6,
    Fatal = 7,
    All = 100
};

// Log entry with enhanced metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::thread::id threadId;
    uint64_t sequenceNumber;
    
    // Performance timing
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    
    // Structured logging fields
    std::unordered_map<std::string, std::string> fields;
    nlohmann::json structuredData;
    
    LogEntry()
        : level(LogLevel::Info),
          line(0),
          sequenceNumber(0),
          threadId(std::this_thread::get_id()) {
        timestamp = std::chrono::system_clock::now();
        startTime = endTime = std::chrono::high_resolution_clock::now();
    }
};

// Log formatter interface
class ILogFormatter {
public:
    virtual ~ILogFormatter() = default;
    virtual std::string format(const LogEntry& entry) = 0;
    virtual std::string formatStructured(const LogEntry& entry) = 0;
};

// Console formatter with colors and formatting
class ConsoleFormatter : public ILogFormatter {
public:
    ConsoleFormatter();
    explicit ConsoleFormatter(bool useColors);
    ~ConsoleFormatter() override;
    
    std::string format(const LogEntry& entry) override;
    std::string formatStructured(const LogEntry& entry) override;
    
    // Configure formatting options
    void setMaxMessageLength(size_t maxLength);
    void setIncludeThreadId(bool include);
    void setIncludeTimestamp(bool include);
    void setTimestampFormat(const std::string& format);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// JSON formatter for structured logging
class JsonFormatter : public ILogFormatter {
public:
    JsonFormatter();
    ~JsonFormatter() override;
    
    std::string format(const LogEntry& entry) override;
    std::string formatStructured(const LogEntry& entry) override;
    
    // Configure JSON formatting
    void setPrettyPrint(bool pretty);
    void setIncludeTimestamps(bool include);
    void setIncludeThreadInfo(bool include);
};

// Logger interface with enhanced features
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void logWithTiming(const std::string& message, 
                             const std::string& file = "",
                             int line = 0,
                             const std::string& function = "");
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Structured logging
    virtual void logStructured(LogLevel level, const nlohmann::json& data,
                              const std::string& message = "",
                              const std::string& file = "",
                              int line = 0,
                              const std::string& function = "");
    
    // Performance logging
    virtual void logPerformance(const std::string& operation,
                               std::chrono::high_resolution_clock::duration duration,
                               const std::string& context = "");
};

// Enhanced Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    explicit ConsoleLogger(LogLevel level, std::shared_ptr<ILogFormatter> formatter);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void logWithTiming(const std::string& message,
                      const std::string& file = "",
                      int line = 0,
                      const std::string& function = "") override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Set custom formatter
    void setFormatter(std::shared_ptr<ILogFormatter> formatter);
    std::shared_ptr<ILogFormatter> getFormatter() const;
    
    // Configure output format
    void setIncludeThreadId(bool include);
    void setIncludeTimestamp(bool include);
    void setTimestampFormat(const std::string& format);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main enhanced Logger class
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
    
    // Global logging functions with timing support
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "",
             int line = 0, 
             const std::string& function = "");
    
    // Enhanced logging functions
    void trace(const std::string& message, const std::string& file = "", 
               int line = 0, const std::string& function = "");
    void debug(const std::string& message, const std::string& file = "", 
               int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", 
              int line = 0, const std::string& function = "");
    void notice(const std::string& message, const std::string& file = "", 
                int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", 
                 int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", 
               int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", 
                  int line = 0, const std::string& function = "");
    void fatal(const std::string& message, const std::string& file = "", 
               int line = 0, const std::string& function = "");
    
    // Stream-style logging helper with timing
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
        ~LogStream();
        
        template<typename T>
        LogStream& operator<<(const T& value) {
            oss_ << value;
            return *this;
        }
        
        // Performance measurement helper
        LogStream& duration(std::chrono::high_resolution_clock::duration dur);
        
    private:
        LogLevel level_;
        std::ostringstream oss_;
        std::string file_;
        int line_;
        std::string function_;
        std::chrono::high_resolution_clock::time_point startTime_;
    };
    
    #define NLM_LOG_STREAM(level) nlm::Logger::getGlobal().getStream(level, __FILE__, __LINE__, __func__)
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Get formatter for global logger
    static std::shared_ptr<ILogFormatter> getGlobalFormatter();
    
    // Configure global logging
    static void configureGlobalLog(LogLevel level, bool useColors = true);
    
    // Performance measurement utilities
    static auto measurePerformance(const std::string& operation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros with timing support
#define NLM_LOG_TIMED(level, message) \
    auto _nlm_log_ ## __LINE__ = nlm::Logger::getGlobal().measurePerformance(std::string("## ") + std::string(message) + "##"); \
    nlm::Logger::getGlobal().level(message, __FILE__, __LINE__, __func__); \
    auto _nlm_log_ ## __LINE__ ## _duration = std::chrono::high_resolution_clock::now() - _nlm_log_ ## __LINE__ ## _start;

// Async logging setup (for future implementation)
class AsyncLogger {
public:
    static void initialize(size_t queueSize = 10000);
    static void shutdown();
    static bool log(const std::string& message, LogLevel level = LogLevel::Info);
};

} // namespace nlm
