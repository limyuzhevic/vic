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

// Log categories for structured logging
enum class LogCategory {
    Core = 0,
    Neural = 1,
    Memory = 2,
    Plasticity = 3,
    Development = 4,
    Neuromodulation = 5,
    Prediction = 6,
    Cognition = 7,
    Sensory = 8,
    Motor = 9,
    Environment = 10,
    Experiment = 11,
    Performance = 12,
    Debug = 13,
    System = 14,
    User = 15
};

// Enhanced Log entry structure with category and structured data
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    LogCategory category;
    std::string threadName;
    uint64_t threadId;
    
    // Structured data for advanced logging
    std::vector<std::pair<std::string, std::string>> keyValuePairs;
    std::vector<std::pair<std::string, double>> numericPairs;
    std::vector<std::pair<std::string, std::vector<std::string>>> stringArrayPairs;
};

// Logger interface with enhanced features
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enhanced logging with category
    virtual void logWithCategory(LogLevel level, LogCategory category, const std::string& message,
                                const std::string& file = "", int line = 0, 
                                const std::string& function = "");
    
    // Structured logging methods
    virtual void logStructured(LogLevel level, LogCategory category, const std::string& message,
                               const std::vector<std::pair<std::string, std::string>>& kvPairs,
                               const std::string& file = "", int line = 0,
                               const std::string& function = "");
    
    // Category-specific logging
    virtual void debugCategory(LogCategory category, const std::string& message,
                               const std::string& file = "", int line = 0,
                               const std::string& function = "");
    
    virtual void infoCategory(LogCategory category, const std::string& message,
                              const std::string& file = "", int line = 0,
                              const std::string& function = "");
    
    virtual void warningCategory(LogCategory category, const std::string& message,
                                 const std::string& file = "", int line = 0,
                                 const std::string& function = "");
    
    virtual void errorCategory(LogCategory category, const std::string& message,
                               const std::string& file = "", int line = 0,
                               const std::string& function = "");
    
    virtual void criticalCategory(LogCategory category, const std::string& message,
                                  const std::string& file = "", int line = 0,
                                  const std::string& function = "");
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
    
    // Enhanced formatting options
    void setEnableTimestamps(bool enable);
    void setEnableThreadInfo(bool enable);
    void setEnableColors(bool useColors);
    void setMaxMessageLength(size_t maxLength);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatStructuredData(const LogEntry& entry) const;
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
    
    // Set category-specific log levels
    void setCategoryLevel(LogCategory category, LogLevel level);
    LogLevel getCategoryLevel(LogCategory category) const;
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Enhanced logging with category
    void logWithCategory(LogLevel level, LogCategory category, const std::string& message,
                         const std::string& file = "", int line = 0,
                         const std::string& function = "");
    
    // Structured logging methods
    void logStructured(LogLevel level, LogCategory category, const std::string& message,
                       const std::vector<std::pair<std::string, std::string>>& kvPairs,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "");
    
    void logStructured(LogLevel level, LogCategory category, const std::string& message,
                       const std::vector<std::pair<std::string, double>>& numPairs,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Category-specific convenience methods
    void debugCategory(LogCategory category, const std::string& message,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "");
    
    void infoCategory(LogCategory category, const std::string& message,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "");
    
    void warningCategory(LogCategory category, const std::string& message,
                         const std::string& file = "", int line = 0,
                         const std::string& function = "");
    
    void errorCategory(LogCategory category, const std::string& message,
                       const std::string& file = "", int line = 0,
                       const std::string& function = "");
    
    void criticalCategory(LogCategory category, const std::string& message,
                          const std::string& file = "", int line = 0,
                          const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    bool isEnabled(LogLevel level, LogCategory category) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Performance monitoring
    void enablePerformanceLogging(bool enable);
    bool isPerformanceLoggingEnabled() const;
    double getLogPerformanceRate() const;  // logs per second
    
    // Configuration
    void setMaxLogSize(size_t maxBytes);
    void setMaxLogFiles(size_t maxFiles);
    void enableLogRotation(bool enable);
    
    // Export/import log configuration
    std::string exportLogConfig() const;
    bool importLogConfig(const std::string& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced convenience macros for structured logging
#define NLM_LOG_DEBUG_CATEGORY(cat, msg) nlm::Logger::getGlobal().debugCategory(cat, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO_CATEGORY(cat, msg) nlm::Logger::getGlobal().infoCategory(cat, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING_CATEGORY(cat, msg) nlm::Logger::getGlobal().warningCategory(cat, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR_CATEGORY(cat, msg) nlm::Logger::getGlobal().errorCategory(cat, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL_CATEGORY(cat, msg) nlm::Logger::getGlobal().criticalCategory(cat, msg, __FILE__, __LINE__, __func__)

// Structured logging macros
#define NLM_LOG_STRUCT_DEBUG(cat, msg, ...) nlm::Logger::getGlobal().logStructured(nlm::LogLevel::Debug, cat, msg, std::vector<std::pair<std::string, std::string>>{__VA_ARGS__}, __FILE__, __LINE__, __func__)
#define NLM_LOG_STRUCT_INFO(cat, msg, ...) nlm::Logger::getGlobal().logStructured(nlm::LogLevel::Info, cat, msg, std::vector<std::pair<std::string, std::string>>{__VA_ARGS__}, __FILE__, __LINE__, __func__)

// Stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, LogCategory category, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Helper methods for structured data
    LogStream& withKeyValue(const std::string& key, const std::string& value);
    LogStream& withKeyValue(const std::string& key, double value);
    LogStream& withKeyValue(const std::string& key, int value);
    LogStream& withKeyValue(const std::string& key, bool value);
    
private:
    LogLevel level_;
    LogCategory category_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::vector<std::pair<std::string, std::string>> kvPairs_;
    std::vector<std::pair<std::string, double>> numPairs_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, nlm::LogCategory::Debug, __FILE__, __LINE__, __func__)

} // namespace nlm
