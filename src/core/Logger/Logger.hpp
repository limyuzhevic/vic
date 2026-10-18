#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <stack>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log categories for component-based filtering
enum class LogCategory {
    All = 0,
    Core = 1,
    Brain = 2,
    Dynamics = 3,
    Plasticity = 4,
    Memory = 5,
    Neuromodulation = 6,
    Cognition = 7,
    Sensory = 8,
    Motor = 9,
    Environment = 10,
    Development = 11,
    Prediction = 12,
    Experiment = 13,
    Performance = 14,
    Configuration = 15,
    PythonBindings = 16,
    Visualization = 17
};

// Log entry structure with enhanced metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Enhanced metadata
    std::string correlationId;      // For request tracing
    std::string userId;             // User performing operation
    std::string sessionId;          // Session identifier
    std::string threadId;           // Thread identifier
    std::unordered_map<std::string, std::string> context; // Key-value context data
    
    // Performance tracking
    std::chrono::high_resolution_clock::time_point processingStart;
    std::chrono::high_resolution_clock::time_point processingEnd;
    
    // Error-specific data
    std::string errorCode;
    std::string errorCategory;
    std::vector<std::string> stackTrace;
    
    LogEntry()
        : level(LogLevel::Info), line(0), processingStart(std::chrono::high_resolution_clock::now()) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Log context manager for thread-local correlation data
class LogContext {
private:
    struct ContextData {
        std::string correlationId;
        std::string userId;
        std::string sessionId;
        std::string threadId;
        std::unordered_map<std::string, std::string> customContext;
    };
    
    static thread_local ContextData context_;
    
public:
    static void setCorrelationId(const std::string& id) { context_.correlationId = id; }
    static std::string getCorrelationId() { return context_.correlationId; }
    
    static void setUserId(const std::string& id) { context_.userId = id; }
    static std::string getUserId() { return context_.userId; }
    
    static void setSessionId(const std::string& id) { context_.sessionId = id; }
    static std::string getSessionId() { return context_.sessionId; }
    
    static void setContext(const std::string& key, const std::string& value) {
        context_.customContext[key] = value;
    }
    
    static std::string getContext(const std::string& key) {
        auto it = context_.customContext.find(key);
        return it != context_.customContext.end() ? it->second : "";
    }
    
    static void clear() {
        context_.correlationId.clear();
        context_.userId.clear();
        context_.sessionId.clear();
        context_.customContext.clear();
    }
    
    static std::string toString() {
        std::ostringstream oss;
        oss << "[corr:" << context_.correlationId << "]";
        oss << "[user:" << context_.userId << "]";
        oss << "[session:" << context_.sessionId << "]";
        for (const auto& [key, value] : context_.customContext) {
            oss << "[" << key << ":" << value << "]";
        }
        return oss.str();
    }
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setCategoryFilter(LogCategory category, bool enabled) = 0;
    virtual bool isCategoryEnabled(LogCategory category) const = 0;
    virtual void setContextFilter(const std::string& key, const std::string& value) = 0;
};

// Structured log entry for JSON-like output
struct StructuredLogEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string level;
    std::string message;
    std::unordered_map<std::string, std::string> fields;
    
    std::string toJSON() const;
    std::string toString() const;
};

// Structured logger for machine-readable logs
class StructuredLogger : public ILogger {
public:
    StructuredLogger();
    explicit StructuredLogger(LogLevel level);
    ~StructuredLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setCategoryFilter(LogCategory category, bool enabled) override;
    bool isCategoryEnabled(LogCategory category) const override;
    void setContextFilter(const std::string& key, const std::string& value) override;
    
    // JSON log file output
    void setJSONOutput(const std::string& filepath);
    void closeJSONOutput();
    
    // Log structured events
    void logEvent(const std::string& eventType, const std::unordered_map<std::string, std::string>& data);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
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
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods with enhanced context
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Enhanced error logging with context
    void logErrorWithContext(const std::string& errorCode, const std::string& errorMessage,
                            const std::string& errorCategory = "",
                            const std::vector<std::string>& stackTrace = {});
    
    // Performance logging
    void logPerformance(const std::string& operation, 
                       std::chrono::high_resolution_clock::duration duration,
                       const std::unordered_map<std::string, std::string>& metrics = {});
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Log context management
    static void setCorrelationId(const std::string& id);
    static std::string getCorrelationId();
    static void setUserId(const std::string& id);
    static std::string getUserId();
    static void setSessionId(const std::string& id);
    static std::string getSessionId();
    static void setContext(const std::string& key, const std::string& value);
    static std::string getContext(const std::string& key);
    static void clearContext();
    
    // Error reporting helper
    static void reportError(const std::string& component, const std::string& errorCode,
                           const std::string& errorMessage, const std::string& file = "",
                           int line = 0, const std::string& function = "");
    
    // Performance monitoring
    static void startPerformanceTimer(const std::string& operation);
    static void endPerformanceTimer(const std::string& operation, 
                                   const std::unordered_map<std::string, std::string>& metrics = {});
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info with context
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced error logging macro with error code
#define NLM_LOG_ERROR_WITH_CODE(code, msg) nlm::Logger::reportError("generic", code, msg, __FILE__, __LINE__, __func__)

// Performance logging macro
#define NLM_PERF_START(op) nlm::Logger::startPerformanceTimer(op)
#define NLM_PERF_END(op) nlm::Logger::endPerformanceTimer(op)

// Context macros
#define NLM_LOG_WITH_CONTEXT(key, value) nlm::Logger::setContext(key, value)

// Stream-style logging helper with enhanced context
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Add structured field
    LogStream& field(const std::string& key, const std::string& value);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::unordered_map<std::string, std::string> fields_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Category-specific logging macros
#define NLM_CORE_LOG_INFO(msg) NLM_LOG_INFO("[CORE] " msg)
#define NLM_BRAIN_LOG_INFO(msg) NLM_LOG_INFO("[BRAIN] " msg)
#define NLM_PLASMATICITY_LOG_INFO(msg) NLM_LOG_INFO("[PLASTICITY] " msg)
#define NLM_MEMORY_LOG_INFO(msg) NLM_LOG_INFO("[MEMORY] " msg)

} // namespace nlm