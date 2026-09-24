#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure with structured data support
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Structured data for key-value logging
    std::unordered_map<std::string, std::string> structuredData;
    
    // Add key-value pair to structured data
    void addStructuredData(const std::string& key, const std::string& value) {
        structuredData[key] = value;
    }
    
    // Check if structured data exists for key
    bool hasStructuredData(const std::string& key) const {
        return structuredData.find(key) != structuredData.end();
    }
    
    // Get value from structured data
    std::string getStructuredData(const std::string& key) const {
        auto it = structuredData.find(key);
        return (it != structuredData.end()) ? it->second : "";
    }
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
    
    // Enable/disable structured logging output
    void setOutputStructuredData(bool outputStructured);
    bool getOutputStructuredData() const;
    
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
    
    // Convenience methods with structured data
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Structured logging methods
    void debugWithData(const std::string& message, const std::unordered_map<std::string, std::string>& data,
                      const std::string& file = "", int line = 0, const std::string& function = "");
    void infoWithData(const std::string& message, const std::unordered_map<std::string, std::string>& data,
                     const std::string& file = "", int line = 0, const std::string& function = "");
    void warningWithData(const std::string& message, const std::unordered_map<std::string, std::string>& data,
                        const std::string& file = "", int line = 0, const std::string& function = "");
    void errorWithData(const std::string& message, const std::unordered_map<std::string, std::string>& data,
                      const std::string& file = "", int line = 0, const std::string& function = "");
    void criticalWithData(const std::string& message, const std::unordered_map<std::string, std::string>& data,
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

// Convenience macro for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Structured logging macro
#define NLM_LOG_DEBUG_WITH_DATA(msg, data) nlm::Logger::getGlobal().debugWithData(msg, data, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO_WITH_DATA(msg, data) nlm::Logger::getGlobal().infoWithData(msg, data, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING_WITH_DATA(msg, data) nlm::Logger::getGlobal().warningWithData(msg, data, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR_WITH_DATA(msg, data) nlm::Logger::getGlobal().errorWithData(msg, data, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL_WITH_DATA(msg, data) nlm::Logger::getGlobal().criticalWithData(msg, data, __FILE__, __LINE__, __func__)

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
    
    // Add structured data
    LogStream& withData(const std::string& key, const std::string& value);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::unordered_map<std::string, std::string> structuredData_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Helper to create a log stream with structured data
inline nlm::LogStream logWithData(nlm::LogLevel level, const std::string& file = "", int line = 0, const std::string& function = "") {
    return nlm::LogStream(level, file, line, function);
}

} // namespace nlm
