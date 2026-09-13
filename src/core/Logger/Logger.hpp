#pragma once

#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <optional>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <thread>
#include <algorithm>
#include <map>
#include <cstdint>
#include <limits>
#include <numeric>

namespace nlm {

// Enhanced Log severity levels
enum class LogLevel {
    Silent = -1,
    Error = 0,
    Warning = 1,
    Info = 2,
    Debug = 3,
    Trace = 4,
    Performance = 5,
    System = 6
};

// Log entry with additional metadata for debugging
struct EnhancedLogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    uint64_t threadId;
    std::string category;
    double elapsedTime;
    bool isSystemEvent;
    bool isError;
    int errorCode;
    
    EnhancedLogEntry()
        : level(LogLevel::Info), line(0), threadId(0), category("General"),
          elapsedTime(0.0), isSystemEvent(false), isError(false), errorCode(0) {}
};

// Advanced Logger interface for enhanced debugging
class IAdvancedLogger {
public:
    virtual ~IAdvancedLogger() = default;
    virtual void log(const EnhancedLogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enhanced configuration options
    virtual void setUseColors(bool useColors) = 0;
    virtual bool getUseColors() const = 0;
    
    virtual void setOutputFile(const std::string& filepath) = 0;
    virtual void closeOutputFile() = 0;
    
    virtual void setUseTimestamps(bool useTimestamps) = 0;
    virtual bool getUseTimestamps() const = 0;
    
    virtual void setUseThreadIds(bool useThreadIds) = 0;
    virtual bool getUseThreadIds() const = 0;
    
    virtual void setUseCategories(bool useCategories) = 0;
    virtual bool getUseCategories() const = 0;
    
    virtual void setUseErrorCodes(bool useErrorCodes) = 0;
    virtual bool getUseErrorCodes() const = 0;
    
    virtual void setUseElapsedTime(bool useElapsedTime) = 0;
    virtual bool getUseElapsedTime() const = 0;
    
    virtual void setHighlightSystemEvents(bool highlight) = 0;
    virtual bool getHighlightSystemEvents() const = 0;
    
    virtual void setOutputStream(std::ostream& stream) = 0;
};

// Console logger implementation with enhanced features
class ConsoleAdvancedLogger : public IAdvancedLogger {
public:
    ConsoleAdvancedLogger();
    explicit ConsoleAdvancedLogger(LogLevel level);
    ~ConsoleAdvancedLogger() override;
    
    void log(const EnhancedLogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void setUseColors(bool useColors) override;
    bool getUseColors() const override;
    
    void setOutputFile(const std::string& filepath) override;
    void closeOutputFile() override;
    
    void setUseTimestamps(bool useTimestamps) override;
    bool getUseTimestamps() const override;
    
    void setUseThreadIds(bool useThreadIds) override;
    bool getUseThreadIds() const override;
    
    void setUseCategories(bool useCategories) override;
    bool getUseCategories() const override;
    
    void setUseErrorCodes(bool useErrorCodes) override;
    bool getUseErrorCodes() const override;
    
    void setUseElapsedTime(bool useElapsedTime) override;
    bool getUseElapsedTime() const override;
    
    void setHighlightSystemEvents(bool highlight) override;
    bool getHighlightSystemEvents() const override;
    
    void setOutputStream(std::ostream& stream) override;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class with comprehensive debugging capabilities
class Logger {
public:
    Logger();
    ~Logger();
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Logger management
    void addLogger(std::shared_ptr<IAdvancedLogger> logger);
    void clearLoggers();
    void setLevel(LogLevel level);
    
    // Global logging configuration
    static LogLevel getGlobalLevel();
    static void setGlobalLevel(LogLevel level);
    static bool isDebugModeEnabled();
    static void setDebugMode(bool enable);
    static bool isPerformanceMonitoringEnabled();
    static void setPerformanceMonitoring(bool enable);
    static bool areSystemHealthChecksEnabled();
    static void setSystemHealthChecks(bool enable);
    static bool areErrorCodesEnabled();
    static void setErrorCodes(bool enable);
    static bool isEventLoggingEnabled();
    static void setEventLogging(bool enable);
    static bool isConnectionTrackingEnabled();
    static void setConnectionTracking(bool enable);
    
    // Enhanced logging methods
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "", const std::string& category = "General",
             bool isSystemEvent = false, bool isError = false, int errorCode = 0);
    
    // Convenience methods with enhanced features
    void logError(const std::string& message, const std::string& file = "", int line = 0,
                  const std::string& function = "", const std::string& category = "Error",
                  int errorCode = 0);
    
    void logWarning(const std::string& message, const std::string& file = "", int line = 0,
                    const std::string& function = "", const std::string& category = "Warning");
    
    void logInfo(const std::string& message, const std::string& file = "", int line = 0,
                 const std::string& function = "", const std::string& category = "Info");
    
    void logDebug(const std::string& message, const std::string& file = "", int line = 0,
                  const std::string& function = "", const std::string& category = "Debug");
    
    void logTrace(const std::string& message, const std::string& file = "", int line = 0,
                  const std::string& function = "", const std::string& category = "Trace");
    
    void logPerformance(const std::string& message, double elapsedTime,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "", const std::string& category = "Performance");
    
    void logSystemEvent(const std::string& message, const std::string& file = "", int line = 0,
                        const std::string& function = "", const std::string& category = "System");
    
    // Stream-style logging
    class LogStream {
    public:
        LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
                  const std::string& category = "General", bool isSystemEvent = false,
                  bool isError = false, int errorCode = 0);
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
        std::string category_;
        bool isSystemEvent_;
        bool isError_;
        int errorCode_;
    };
    
    LogStream logStream(LogLevel level, const std::string& file, int line, const std::string& function,
                        const std::string& category = "General", bool isSystemEvent = false,
                        bool isError = false, int errorCode = 0);
    
    // Log level checking
    bool isEnabled(LogLevel level) const;
    
    // Global logger management
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Log statistics
    struct LogStatistics {
        uint64_t totalLogs;
        uint64_t debugLogs;
        uint64_t infoLogs;
        uint64_t warningLogs;
        uint64_t errorLogs;
        uint64_t criticalLogs;
        uint64_t performanceLogs;
        uint64_t systemLogs;
        std::chrono::steady_clock::time_point startTime;
        double uptime;
        
        LogStatistics() : totalLogs(0), debugLogs(0), infoLogs(0), warningLogs(0),
                         errorLogs(0), criticalLogs(0), performanceLogs(0), systemLogs(0),
                         startTime(std::chrono::steady_clock::now()), uptime(0.0) {}
    };
    
    static LogStatistics getStatistics();
    static void resetStatistics();
    
    // Configuration validation
    static bool validateConfig(const std::string& key, const std::string& value,
                               const std::string& category = "ConfigValidation");
    static bool validateConnection(const std::string& source, const std::string& target,
                                   const std::string& category = "ConnectionValidation");
    
    // Event logging
    static void logSpikeEvent(NeuronId neuron, Timestamp timestamp, SimulationStep step,
                              const std::string& category = "Spikes");
    static void logLearningEvent(const std::string& type, double strength,
                                 const std::string& category = "Learning");
    static void logMemoryUpdate(const std::string& memoryType, size_t size,
                                const std::string& category = "Memory");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for enhanced logging
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().logDebug(msg, __FILE__, __LINE__, __func__, "Debug")
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().logInfo(msg, __FILE__, __LINE__, __func__, "Info")
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().logWarning(msg, __FILE__, __LINE__, __func__, "Warning")
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().logError(msg, __FILE__, __LINE__, __func__, "Error")
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().logError(msg, __FILE__, __LINE__, __func__, "Critical", -1)
#define NLM_LOG_PERF(msg, elapsed) nlm::Logger::getGlobal().logPerformance(msg, elapsed, __FILE__, __LINE__, __func__, "Performance")
#define NLM_LOG_SYSTEM(msg) nlm::Logger::getGlobal().logSystemEvent(msg, __FILE__, __LINE__, __func__, "System")

} // namespace nlm
