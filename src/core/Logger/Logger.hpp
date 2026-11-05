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
    
    // Set log level for this stream
    void setLevel(LogLevel level) { level_ = level; }
    
    // Get log level
    LogLevel getLevel() const { return level_; }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

// Advanced logging utilities
class LoggingUtils {
public:
    // Format log entry with JSON support
    static std::string formatLogEntry(const LogEntry& entry);
    
    // Validate log level
    static bool isValidLevel(LogLevel level);
    
    // Get level name as string
    static std::string levelToString(LogLevel level);
    
    // Get level from string
    static LogLevel stringToLevel(const std::string& levelName);
    
    // Performance monitoring
    static void updatePerformanceStats(const std::string& operation, 
                                        double duration, 
                                        LogLevel level = LogLevel::Info);
    
    // Log stack trace (if available)
    static void logStackTrace(LogLevel level = LogLevel::Error);
    
    // Validate log message
    static bool isValidMessage(const std::string& message);
    
    // Sanitize log message
    static std::string sanitizeMessage(const std::string& message);
};

// Async logging interface
class IAsyncLogger {
public:
    virtual ~IAsyncLogger() = default;
    virtual void asyncLog(const LogEntry& entry) = 0;
    virtual void setQueueSize(size_t size) = 0;
    virtual size_t getQueueSize() const = 0;
};

// Structured logging interface
class IStructuredLogger {
public:
    virtual ~IStructuredLogger() = default;
    virtual void logStructured(const std::string& eventType, 
                              const std::map<std::string, std::string>& fields,
                              LogLevel level = LogLevel::Info) = 0;
};

// Component-based logging
struct LogComponent {
    enum class Component {
        Core,
        Brain,
        Memory,
        Neuromodulation,
        Cognition,
        Development,
        Sensory,
        Motor,
        Environment,
        World,
        Agent,
        Experiment,
        Performance,
        System,
        Custom
    };
    
    std::string name;
    Component component;
    LogLevel level;
    bool enabled;
    
    LogComponent(const std::string& n, Component c = Component::System, 
                 LogLevel l = LogLevel::Info, bool e = true)
        : name(n), component(c), level(l), enabled(e) {}
};

// Log category for filtering
enum class LogCategory {
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Performance,
    Validation,
    Memory,
    Neuromodulation,
    Cognition,
    Development,
    Sensory,
    Motor,
    Environment,
    System,
    UserAction,
    Configuration,
    Experiment
};

// Enhanced Logger with advanced features
class AdvancedLogger {
public:
    AdvancedLogger();
    ~AdvancedLogger();
    
    // Component management
    void addComponent(const LogComponent& component);
    void removeComponent(const std::string& name);
    bool hasComponent(const std::string& name) const;
    
    // Log category filtering
    void enableCategory(LogCategory category, bool enable = true);
    bool isCategoryEnabled(LogCategory category) const;
    
    // Structured logging
    void logStructured(const std::string& eventType,
                      const std::map<std::string, std::string>& fields,
                      LogLevel level = LogLevel::Info,
                      LogCategory category = LogCategory::System);
    
    // Performance logging
    void logPerformance(const std::string& operation,
                       double duration,
                       LogLevel level = LogLevel::Info);
    
    // Validation logging
    void logValidation(const std::string& validationId,
                      const std::string& message,
                      bool success,
                      LogLevel level = LogLevel::Warning);
    
    // Memory logging
    void logMemoryUsage(const std::string& context = "");
    void logMemoryGrowth(const std::string& context = "");
    
    // Neuromodulation logging
    void logNeuromodulation(const std::string& signalType,
                           double level,
                           double change);
    
    // Cognition logging
    void logCognition(const std::string& cognitiveProcess,
                     const std::map<std::string, double>& metrics);
    
    // Development logging
    void logDevelopment(DevelopmentalStage stage,
                       const std::string& event);
    
    // Async logging support
    void addAsyncLogger(std::shared_ptr<IAsyncLogger> logger);
    
    // Performance statistics
    struct PerformanceStats {
        uint64_t totalLogs = 0;
        uint64_t logsByLevel[5] = {0}; // Debug, Info, Warning, Error, Critical
        double avgLogTime = 0.0;
        uint64_t activeComponents = 0;
        uint64_t memoryUsage = 0;
    };
    
    PerformanceStats getPerformanceStats() const;
    
    // Configuration validation
    void validateConfiguration();
    
    // Export logs
    void exportLogs(const std::string& filepath, 
                   const std::map<std::string, bool>& categories = {});
    
    // Clear all logs
    void clearAllLogs();
    
    // Get global advanced logger instance
    static AdvancedLogger& getGlobal();
    
    // Set global advanced logger
    static void setGlobal(std::shared_ptr<AdvancedLogger> logger);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Macro for structured logging
#define NLM_LOG_STRUCTURED(eventType, fields) \
    nlm::AdvancedLogger::getGlobal().logStructured(eventType, fields)

// Macro for performance logging
#define NLM_LOG_PERFORMANCE(operation, duration) \
    nlm::AdvancedLogger::getGlobal().logPerformance(operation, duration)

// Macro for validation logging
#define NLM_LOG_VALIDATION(id, message, success) \
    nlm::AdvancedLogger::getGlobal().logValidation(id, message, success)

// Macro for component-specific logging
#define NLM_LOG_COMPONENT(component, level, message) \
    if (nlm::AdvancedLogger::getGlobal().hasComponent(component)) \
        nlm::Logger::getGlobal().log(level, message, __FILE__, __LINE__, __func__)

#define NLM_LOG_PERFORMANCE_DEBUG(operation, duration) \
    NLM_LOG_PERFORMANCE(operation, duration)

#define NLM_LOG_VALIDATION_DEBUG(id, message, success) \
    NLM_LOG_VALIDATION(id, message, success)

} // namespace nlm
