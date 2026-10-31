#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <iomanip>
#include <optional>

namespace nlm {

// Enhanced Log Severity Levels with additional context
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    // Enhanced levels for better debugging
    Trace = -1,           // Very detailed tracing
    System = 10,         // System-level events (startup/shutdown)
    Network = 11,        // Network operations
    Memory = 12,         // Memory management
    Performance = 13,    // Performance metrics
    Security = 14,       // Security events
    User = 15,           // User actions
    Development = 16     // Development/debug info
};

// Log entry with enhanced metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Enhanced metadata
    std::string threadId;
    std::string module;
    std::unordered_map<std::string, std::string> context;
    std::string operation;
    uint64_t sequenceId;
    std::optional<std::string> errorCode;
    std::optional<std::string> stackTrace;
    
    LogEntry()
        : level(LogLevel::Info), line(0), sequenceId(0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    LogEntry(LogLevel lvl, const std::string& msg)
        : level(lvl), message(msg), line(0), sequenceId(0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Log output format options
enum class LogFormat {
    Text,
    JSON,
    CSV,
    XML,
    Structured
};

// Logger statistics
struct LoggerStats {
    uint64_t totalMessages{0};
    uint64_t messagesByLevel[5]{0}; // Debug, Info, Warning, Error, Critical
    std::chrono::steady_clock::duration totalProcessingTime{0};
    uint64_t droppedMessages{0};
    uint64_t overflowCount{0};
    
    void reset() {
        totalMessages = 0;
        for (int i = 0; i < 5; ++i) messagesByLevel[i] = 0;
        totalProcessingTime = std::chrono::steady_clock::duration(0);
        droppedMessages = 0;
        overflowCount = 0;
    }
};

// Logger configuration
struct LoggerConfig {
    LogLevel minLevel{LogLevel::Info};
    bool useColors{true};
    bool includeTimestamps{true};
    bool includeThreadIds{true};
    bool includeFileInfo{true};
    bool enableStatistics{false};
    LogFormat outputFormat{LogFormat::Text};
    std::string outputFile;
    size_t maxQueueSize{10000};
    std::chrono::milliseconds flushInterval{100};
    bool enableAsync{true};
    
    // Performance tuning
    bool enableStructuredLogging{false};
    bool enableLogRotation{false};
    size_t maxFileSize{10 * 1024 * 1024}; // 10MB
    int maxFiles{10};
    std::string rotationPattern;
};

// Logger interface with enhanced features
class ILogger {
public:
    virtual ~ILogger() = default;
    
    // Basic logging
    virtual void log(const LogEntry& entry) = 0;
    
    // Configuration
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void configure(const LoggerConfig& config) = 0;
    virtual LoggerConfig getConfig() const = 0;
    
    // Enhanced logging methods
    virtual void logDebug(const std::string& message, const std::string& context = "");
    virtual void logInfo(const std::string& message, const std::string& context = "");
    virtual void logWarning(const std::string& message, const std::string& context = "");
    virtual void logError(const std::string& message, const std::string& context = "");
    virtual void logCritical(const std::string& message, const std::string& context = "");
    
    // Performance monitoring
    virtual LoggerStats getStats() const = 0;
    virtual void resetStats() = 0;
    
    // Enhanced output
    virtual void setOutputFile(const std::string& filepath);
    virtual void closeOutputFile();
    virtual bool isOutputFileOpen() const;
    
    // Log filtering
    virtual void addFilter(std::function<bool(const LogEntry&)> filter);
    virtual void removeFilter(std::function<bool(const LogEntry&)>
    virtual bool shouldLog(const LogEntry& entry) const;
    
    // Performance profiling
    virtual void startProfile(const std::string& name);
    virtual void endProfile(const std::string& name);
    virtual std::unordered_map<std::string, std::chrono::duration<double>> getProfiles() const;
    
    // Error context
    virtual void setErrorContext(const std::string& key, const std::string& value);
    virtual std::string getErrorContext(const std::string& key) const;
};

// Enhanced Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(const LoggerConfig& config);
    ~ConsoleLogger() override;
    
    // ILogger implementation
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void configure(const LoggerConfig& config) override;
    LoggerConfig getConfig() const override;
    
    // Enhanced methods
    void logDebug(const std::string& message, const std::string& context = "") override;
    void logInfo(const std::string& message, const std::string& context = "") override;
    void logWarning(const std::string& message, const std::string& context = "") override;
    void logError(const std::string& message, const std::string& context = "") override;
    void logCritical(const std::string& message, const std::string& context = "") override;
    
    // ILogger interface
    LoggerStats getStats() const override;
    void resetStats() override;
    void setOutputFile(const std::string& filepath) override;
    void closeOutputFile() override;
    bool isOutputFileOpen() const override;
    void addFilter(std::function<bool(const LogEntry&)> filter) override;
    void removeFilter(std::function<bool(const LogEntry&)>
    bool shouldLog(const LogEntry& entry) const override;
    void startProfile(const std::string& name) override;
    void endProfile(const std::string& name) override;
    std::unordered_map<std::string, std::chrono::duration<double>> getProfiles() const override;
    void setErrorContext(const std::string& key, const std::string& value) override;
    std::string getErrorContext(const std::string& key) const override;
    
    // Performance methods
    void flush();
    bool isHealthy() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string formatEntry(const LogEntry& entry) const;
    std::string formatEntryJSON(const LogEntry& entry) const;
    std::string formatEntryStructured(const LogEntry& entry) const;
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string getFormattedTimestamp(const LogEntry& entry) const;
    std::string getFormattedThreadId() const;
    std::string getFormattedLocation(const LogEntry& entry) const;
};

// Main Logger class with enhanced features
class Logger {
public:
    Logger();
    explicit Logger(const LoggerConfig& config);
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Global logger management
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);
    static void reconfigureGlobal(const LoggerConfig& config);
    
    // Logger management
    void addLogger(std::shared_ptr<ILogger> logger);
    void removeLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    
    // Configuration
    void configure(const LoggerConfig& config);
    LoggerConfig getGlobalConfig();
    void setLevel(LogLevel level);
    
    // Global logging with enhanced features
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "", 
             const std::string& module = "",
             const std::unordered_map<std::string, std::string>& context = {});
    
    // Enhanced logging methods
    void logDebug(const std::string& message, const std::string& context = "");
    void logInfo(const std::string& message, const std::string& context = "");
    void logWarning(const std::string& message, const std::string& context = "");
    void logError(const std::string& message, const std::string& context = "");
    void logCritical(const std::string& message, const std::string& context = "");
    
    // Convenience macros with enhanced context
    void debug(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& module = "");
    void info(const std::string& message, const std::string& file = "", int line = 0,
              const std::string& function = "", const std::string& module = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0,
                 const std::string& function = "", const std::string& module = "");
    void error(const std::string& message, const std::string& file = "", int line = 0,
               const std::string& function = "", const std::string& module = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0,
                  const std::string& function = "", const std::string& module = "");
    
    // Global logging functions
    static void log(LogLevel level, const std::string& message,
                   const std::string& file = "", int line = 0,
                   const std::string& function = "",
                   const std::string& module = "",
                   const std::unordered_map<std::string, std::string>& context = {});
    
    // Global convenience methods
    static void debug(const std::string& message, const std::string& file = "", int line = 0,
                      const std::string& function = "", const std::string& module = "");
    static void info(const std::string& message, const std::string& file = "", int line = 0,
                     const std::string& function = "", const std::string& module = "");
    static void warning(const std::string& message, const std::string& file = "", int line = 0,
                        const std::string& function = "", const std::string& module = "");
    static void error(const std::string& message, const std::string& file = "", int line = 0,
                      const std::string& function = "", const std::string& module = "");
    static void critical(const std::string& message, const std::string& file = "", int line = 0,
                         const std::string& function = "", const std::string& module = "");
    
    // Statistics and monitoring
    LoggerStats getGlobalStats();
    void resetGlobalStats();
    std::unordered_map<std::string, std::chrono::duration<double>> getGlobalProfiles() const;
    
    // Error context management
    static void setGlobalErrorContext(const std::string& key, const std::string& value);
    static std::string getGlobalErrorContext(const std::string& key);
    
    // Performance monitoring
    static void startGlobalProfile(const std::string& name);
    static void endGlobalProfile(const std::string& name);
    
    // System health check
    static bool isSystemHealthy();
    static std::string getSystemHealthReport();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    std::shared_ptr<ILogger> createDefaultLogger();
    void applyGlobalConfig(std::shared_ptr<ILogger> logger);
};

// Convenience macros with enhanced context
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced stream-style logging with context
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__, "main")

// Module-specific logging macros
#define NLM_LOG_MODULE_DEBUG(module, msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, module)
#define NLM_LOG_MODULE_INFO(module, msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, module)
#define NLM_LOG_MODULE_WARNING(module, msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__, module)
#define NLM_LOG_MODULE_ERROR(module, msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__, module)

// Performance profiling macros
#define NLM_PROFILE(name) nlm::ProfilerScope _profile(name)

// Error context macros
#define NLM_ERROR_CONTEXT(key, value) nlm::Logger::setGlobalErrorContext(key, value)

} // namespace nlm

// Additional helper classes and functions
namespace nlm {
    
    class ProfilerScope {
    public:
        ProfilerScope(const std::string& name);
        ~ProfilerScope();
        
    private:
        std::string name_;
        std::chrono::steady_clock::time_point startTime_;
    };
    
    // Global configuration management
    void setLoggerConfig(const LoggerConfig& config);
    LoggerConfig getLoggerConfig();
    
    // Log categorization
    enum class LogCategory {
        System,
        Network,
        Memory,
        Performance,
        Security,
        User,
        Development,
        Error,
        Warning,
        Info,
        Debug
    };
    
    LogCategory categorizeLog(const LogEntry& entry);
    
    // Log level utilities
    LogLevel stringToLogLevel(const std::string& levelStr);
    std::string logLevelToString(LogLevel level);
    bool isLevelEnabled(LogLevel level, LogLevel minLevel);
    
    // Log formatting utilities
    std::string formatTimestamp(const std::chrono::system_clock::time_point& tp);
    std::string formatDuration(const std::chrono::duration<double>& dur);
    
    // Performance monitoring utilities
    double calculateThroughput(const LoggerStats& stats, const std::chrono::duration<double>& period);
    double calculateErrorRate(const LoggerStats& stats);
    
    // Health check utilities
    bool checkLoggerHealth(const LoggerStats& stats);
    std::string generateHealthReport(const LoggerStats& stats, const LoggerConfig& config);
    
} // namespace nlm
