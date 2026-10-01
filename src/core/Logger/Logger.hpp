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
    
    // Thread-safe logging interface
    // Log entries may be processed concurrently from multiple threads
    // All implementations must be thread-safe
    virtual void log(const LogEntry& entry) = 0;
    
    // Thread-safe level setting
    virtual void setLevel(LogLevel level) = 0;
    
    // Thread-safe level retrieval
    virtual LogLevel getLevel() const = 0;
};

// Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    // Thread-safe logging implementation
    // Uses mutex to synchronize access to shared resources
    void log(const LogEntry& entry) override;
    
    // Thread-safe level setting
    void setLevel(LogLevel level) override;
    
    // Thread-safe level retrieval
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
private:
    // Thread safety: All operations are synchronized with mutex
    // Single mutex protects all shared state (level, output streams, file handles)
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
    
    // Thread safety: All operations are synchronized with mutex
    // Multiple threads can safely add/remove loggers, log messages, set/get levels
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Thread-safe: Operation is synchronized with mutex
    // addLogger may throw if memory allocation fails
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Thread-safe: Operation is synchronized with mutex
    void clearLoggers();
    
    // Thread-safe: Operation is synchronized with mutex
    // Sets level on all loggers
    void setLevel(LogLevel level);
    
    // Thread-safe: Operation is synchronized with mutex
    // Log entry with file/line/function info for debugging
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods with thread-safe implementations
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Thread-safe: Operation is synchronized with mutex
    // Checks if any logger is enabled for this level
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    // Thread-safe: Static initialization is synchronized by the system
    static Logger& getGlobal();
    
    // Set global logger
    // Thread-safe: All operations are synchronized with mutex
    static void setGlobal(std::shared_ptr<Logger> logger);
    
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
