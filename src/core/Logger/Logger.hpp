#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

namespace nlm {

// Log severity levels with enhanced categorization
enum class LogLevel {
    Debug = 0,                    // Detailed debugging information
    Info = 1,                     // General information messages
    Warning = 2,                  // Warning conditions that may cause issues
    Error = 3,                    // Error conditions that prevented some functionality
    Critical = 4,                 // Critical errors that may cause system failure
    Performance = 5,              // Performance metrics and timing
    Memory = 6,                   // Memory usage and management
    Neural = 7,                   // Neural network specific events
    Plasticity = 8,               // Plasticity and learning events
    Development = 9,              // Development and maturation events
    Integration = 10              // System integration events
};

// Log entry structure with enhanced fields
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadName;       // Thread identification
    uint64_t neuronId;            // Optional neuron ID for neural logging
    std::string category;         // Message category for filtering
    double performanceTime;       // Optional performance timing data
    uint64_t memoryUsage;         // Optional memory usage data
};

// Log categories for filtering
enum class LogCategory {
    System = 0,
    Neural = 1,
    Plasticity = 2,
    Memory = 3,
    Performance = 98,
    Development = 99
};

// Log statistics for monitoring
struct LogStatistics {
    uint64_t totalMessages;
    uint64_t messagesByLevel[10];  // Indexed by LogLevel
    std::chrono::duration<double> totalTimeSpent;
    uint64_t errorCount;
    std::chrono::system_clock::time_point firstMessage;
    std::chrono::system_clock::time_point lastMessage;
};

// Log statistics for monitoring
struct LogStatistics {
    uint64_t totalMessages;
    uint64_t messagesByLevel[10];  // Indexed by LogLevel
    std::chrono::duration<double> totalTimeSpent;
    uint64_t errorCount;
    std::chrono::system_clock::time_point firstMessage;
    std::chrono::system_clock::time_point lastMessage;
};

// Logger interface with enhanced features
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Optional enhanced features
    virtual void setCategoryFilter(LogCategory category, bool enabled) {}
    virtual bool isCategoryEnabled(LogCategory category) const { return true; }
    virtual void setPerformanceLogging(bool enabled) {}
    virtual bool isPerformanceLoggingEnabled() const { return false; }
    virtual void setMemoryLogging(bool enabled) {}
    virtual bool isMemoryLoggingEnabled() const { return false; }
    virtual void setThreadName(const std::string& name) {}
    virtual std::string getThreadName() const { return "Main"; }
};

// Console logger implementation with enhanced features
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Basic features
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Enhanced features
    void setLogTimestampFormat(bool usePrecise);  // More precise timestamps
    void setLogThreadInfo(bool enabled);           // Include thread information
    void setLogFileInfo(bool enabled);             // Include file/line info
    void setLogFunctionInfo(bool enabled);         // Include function name
    void setLogCategory(bool enabled);             // Include category
    void setPerformanceLogging(bool enabled);
    void setMemoryLogging(bool enabled);
    void setNeuralLogging(bool enabled);
    void setPlasticityLogging(bool enabled);
    void setDevelopmentLogging(bool enabled);
    
    // Statistics
    LogStatistics getStatistics() const;
    void resetStatistics();
    
    // Filtering
    void enableLevel(LogLevel level);
    void disableLevel(LogLevel level);
    bool isLevelEnabled(LogLevel level) const;
    
    // Export
    bool exportLogs(const std::string& filepath, LogLevel minLevel = LogLevel::Debug);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
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
    
    // Global logging functions with enhanced parameters
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "", const std::string& category = "",
             uint64_t neuronId = 0, bool includeTimestamps = true,
             bool includeFileInfo = true, bool includeFunctionInfo = true);
    
    // Enhanced convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& category = "");
    
    // Performance logging methods
    void logPerformance(const std::string& operation, double duration, const std::string& details = "");
    void logMemoryUsage(uint64_t bytes, const std::string& description = "");
    void logNeuralEvent(uint64_t neuronId, const std::string& event, const std::string& details = "");
    void logPlasticityEvent(const std::string& rule, double weightChange, const std::string& details = "");
    void logDevelopmentEvent(const std::string& stage, double progress, const std::string& details = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Statistics and monitoring
    LogStatistics getGlobalStatistics() const;
    void resetGlobalStatistics();
    void exportGlobalLogs(const std::string& filepath, LogLevel minLevel = LogLevel::Debug);
    
    // Thread management
    static void setCurrentThreadName(const std::string& name);
    static std::string getCurrentThreadName();
    
    // Configuration
    void configureLogFile(const std::string& filepath, bool overwrite = false);
    void configureLogRotation(size_t maxSizeMB, size_t maxFiles = 5);
    
    // Performance monitoring
    void enablePerformanceLogging(bool enabled);
    void enableMemoryLogging(bool enabled);
    void enableNeuralLogging(bool enabled);
    void enablePlasticityLogging(bool enabled);
    void enableDevelopmentLogging(bool enabled);
    
    // Category filtering
    void enableCategory(LogCategory category);
    void disableCategory(LogCategory category);
    bool isCategoryEnabled(LogCategory category) const;
    
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

// Enhanced stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
              const std::string& category = "", uint64_t neuronId = 0);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Performance timing
    template<typename Func>
    LogStream& timeOperation(Func&& func, const std::string& operationName = "operation") {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start).count();
        oss_ << " [" << operationName << " timed: " << duration << " ms]";
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string category_;
    uint64_t neuronId_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
