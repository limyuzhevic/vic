#pragma once

#include "Logger.hpp"
#include <unordered_map>
#include <chrono>
#include <string>
#include <memory>

namespace nlm {

// LoggerConfiguration: Configuration for enhanced logging
struct LoggerConfiguration {
    // Global settings
    LogLevel globalLevel = LogLevel::Info;
    bool enableStructuredLogging = false;
    bool enablePerformanceMonitoring = false;
    bool enableContextualLogging = true;
    bool enableThreadLocalContext = true;
    std::string timestampFormat = "%Y-%m-%d %H:%M:%S";
    
    // Component-specific settings
    std::unordered_map<std::string, LogLevel> componentLevels;
    std::unordered_map<std::string, std::string> componentFormats;
    
    // Performance monitoring
    bool trackExecutionTime = true;
    double performanceThresholdMs = 100.0;
    
    // Error handling
    bool enableErrorReporting = true;
    bool includeStackTrace = true;
    std::unordered_map<std::string, std::string> errorCategories;
    
    // Sanitization
    bool enableSanitization = true;
    std::vector<std::string> sensitivePatterns;
    std::vector<std::string> redactFields;
    
    // Output formatting
    bool enableColors = true;
    bool includeLocation = true;
    bool includeFunction = true;
    bool includeThreadId = false;
    
    // File output
    bool enableFileOutput = false;
    std::string logFile;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    size_t maxFiles = 5;
    bool rotateDaily = true;
};

// ContextualLogger: Enhanced logger with contextual and performance monitoring
class ContextualLogger {
public:
    ContextualLogger();
    ~ContextualLogger();
    
    // Configuration
    void configure(const LoggerConfiguration& config);
    LoggerConfiguration getConfiguration() const;
    
    // Global context management
    void setGlobalContext(const std::string& key, const std::string& value);
    std::string getGlobalContext(const std::string& key) const;
    void clearGlobalContext();
    void clearGlobalContext(const std::string& key);
    
    // Thread-local context management
    void setThreadLocalContext(const std::string& key, const std::string& value);
    std::string getThreadLocalContext(const std::string& key) const;
    void clearThreadLocalContext();
    void clearThreadLocalContext(const std::string& key);
    
    // Component-specific logging
    void setComponentLevel(const std::string& component, LogLevel level);
    LogLevel getComponentLevel(const std::string& component) const;
    
    // Enhanced logging methods
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "",
             const std::string& component = "");
    
    void logWithDuration(LogLevel level, const std::string& message,
                        double durationMs,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "",
                        const std::string& component = "");
    
    void logError(const std::string& errorCode, const std::string& errorMessage,
                  const std::string& errorCategory = "",
                  const std::string& stackTrace = "",
                  const std::string& component = "");
    
    // Performance monitoring
    void startPerformanceTimer(const std::string& operationId, const std::string& category = "");
    void endPerformanceTimer(const std::string& operationId);
    void logPerformance(const std::string& operationId, double durationMs, const std::string& category = "");
    
    // Specialized logging methods
    void neuralDebug(const std::string& message, const std::string& component = "brain");
    void plasticityDebug(const std::string& message, const std::string& component = "plasticity");
    void memoryDebug(const std::string& message, const std::string& component = "memory");
    void performance(const std::string& message, double durationMs, const std::string& component = "performance");
    void system(const std::string& message, const std::string& component = "system");
    void userAction(const std::string& message, const std::string& userId = "", const std::string& component = "user");
    void validation(const std::string& message, const std::string& component = "validation");
    void security(const std::string& message, const std::string& component = "security");
    void config(const std::string& message, const std::string& component = "config");
    void experiment(const std::string& message, const std::string& experimentId = "", const std::string& component = "experiment");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& component = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& component = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& component = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& component = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& component = "");
    
    // Global logger instance
    static ContextualLogger& getGlobal();
    static void setGlobal(std::shared_ptr<ContextualLogger> logger);
    
    // Integration with existing Logger
    void integrateWithExistingLogger(std::shared_ptr<Logger> existingLogger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string formatMessage(const LogEntry& entry, const LoggerConfiguration& config) const;
    std::string applySanitization(const std::string& message, const LoggerConfiguration& config) const;
    bool shouldLog(const LogEntry& entry, const LoggerConfiguration& config) const;
};

// Global configuration function
void configureGlobalLogger(const LoggerConfiguration& config);

// Convenience macros for enhanced logging
#define NLM_LOG_PERFORMANCE(operation, duration) nlm::ContextualLogger::getGlobal().logPerformance(operation, duration)
#define NLM_LOG_NEURAL_DEBUG(msg) nlm::ContextualLogger::getGlobal().neuralDebug(msg)
#define NLM_LOG_PLASTICITY_DEBUG(msg) nlm::ContextualLogger::getGlobal().plasticityDebug(msg)
#define NLM_LOG_MEMORY_DEBUG(msg) nlm::ContextualLogger::getGlobal().memoryDebug(msg)
#define NLM_LOG_ERROR_CODE(code, msg, category) nlm::ContextualLogger::getGlobal().logError(code, msg, category)
#define NLM_LOG_WITH_CONTEXT(level, msg, key, value) do { \
    nlm::ContextualLogger::getGlobal().setGlobalContext(key, value); \
    nlm::ContextualLogger::getGlobal().log(level, msg); \
} while(0)

// Performance timer helper
class PerformanceTimer {
public:
    PerformanceTimer(const std::string& operationId, const std::string& category = "", ContextualLogger* logger = nullptr);
    ~PerformanceTimer();
    
    void stop();
    double getDuration() const;
    std::string getOperationId() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Macro for easy performance timing
#define NLM_PERFORMANCE_TIMER(operation) nlm::PerformanceTimer performanceTimer(operation, "", &nlm::ContextualLogger::getGlobal())

} // namespace nlm
