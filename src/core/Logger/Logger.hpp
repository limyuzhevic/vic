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

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Modern C++20-style logging macros that include file/line/function info
// These macros maintain backward compatibility while providing better error handling

// Error context information for better debugging
struct ErrorContext {
    std::string file;
    int line;
    std::string function;
    std::string message;
};

// Enhanced logging macro with better error context and modern C++20 support
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging with context
#define NLM_LOG_STREAM_ERROR nlm::LogStream(nlm::LogLevel::Error, __FILE__, __LINE__, __func__)
#define NLM_LOG_STREAM_WARNING nlm::LogStream(nlm::LogLevel::Warning, __FILE__, __LINE__, __func__)
#define NLM_LOG_STREAM_INFO nlm::LogStream(nlm::LogLevel::Info, __FILE__, __LINE__, __func__)

// Enhanced error handling utility for common patterns
class ErrorHandler {
public:
    template<typename T>
    static bool checkAndLog(bool condition, const std::string& errorMsg, 
                           const std::string& file = "", int line = 0, 
                           const std::string& function = "") {
        if (!condition) {
            NLM_LOG_ERROR(errorMsg + " at " + file + ":" + std::to_string(line) + 
                         " in function " + function);
            return false;
        }
        return true;
    }
    
    template<typename T>
    static bool checkAndWarn(bool condition, const std::string& warningMsg,
                            const std::string& file = "", int line = 0,
                            const std::string& function = "") {
        if (!condition) {
            NLM_LOG_WARNING(warningMsg + " at " + file + ":" + std::to_string(line) +
                           " in function " + function);
            return false;
        }
        return true;
    }
    
    // File operation error handling
    static bool checkFileOpen(std::ifstream& file, const std::string& path,
                             const std::string& operation = "read") {
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to " + operation + " file: " + path);
            return false;
        }
        return true;
    }
    
    static bool checkFileWrite(std::ofstream& file, const std::string& path,
                              const std::string& operation = "write") {
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to " + operation + " file: " + path);
            return false;
        }
        return true;
    }
    
    // Resource allocation error handling
    template<typename T>
    static T* checkAllocation(T* ptr, const std::string& resourceType,
                             const std::string& file = "", int line = 0,
                             const std::string& function = "") {
        if (!ptr) {
            NLM_LOG_ERROR("Failed to allocate " + resourceType + " at " + file +
                         ":" + std::to_string(line) + " in function " + function);
        }
        return ptr;
    }
};

} // namespace nlm
