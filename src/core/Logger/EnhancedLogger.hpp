#pragma once

#include "Logger.hpp"
#include <string>
#include <functional>
#include <map>
#include <atomic>

namespace nlm {

// Enhanced logging interface with error handling and recovery
class IEnhancedLogger {
public:
    virtual ~IEnhancedLogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Error handling capabilities
    virtual void setErrorHandler(std::function<bool(const std::string&)> handler) = 0;
    virtual void setRecoveryMode(bool enabled) = 0;
    virtual bool isInRecoveryMode() const = 0;
    
    // Session management
    virtual void startSession(const std::string& sessionName) = 0;
    virtual void endSession(const std::string& sessionName) = 0;
    
    // Statistics and monitoring
    struct LogStatistics {
        size_t debugCount = 0;
        size_t infoCount = 0;
        size_t warningCount = 0;
        size_t errorCount = 0;
        size_t criticalCount = 0;
        time_t startTime = 0;
        time_t endTime = 0;
    };
    
    virtual LogStatistics getLogStatistics() const = 0;
};

// Enhanced logger with recovery and error handling capabilities
class EnhancedLogger : public Logger, public IEnhancedLogger {
public:
    EnhancedLogger();
    explicit EnhancedLogger(const std::string& logDirectory);
    ~EnhancedLogger() override;
    
    // Disable copying
    EnhancedLogger(const EnhancedLogger&) = delete;
    EnhancedLogger& operator=(const EnhancedLogger&) = delete;
    
    // IEnhancedLogger interface
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    void setErrorHandler(std::function<bool(const std::string&)> handler) override;
    void setRecoveryMode(bool enabled) override;
    bool isInRecoveryMode() const override;
    
    void startSession(const std::string& sessionName) override;
    void endSession(const std::string& sessionName) override;
    
    LogStatistics getLogStatistics() const override;
    
    // Error handling functions
    static bool HandleError(const std::string& errorMessage, LogLevel severity = LogLevel::Error);
    
    // Log directory management
    static void InitializeEnhancedLogging(const std::string& logDirectory = "");
    
    // Get global enhanced logger instance
    static EnhancedLogger& getGlobal();
    
    // Set global enhanced logger instance
    static void setGlobal(std::shared_ptr<EnhancedLogger> logger);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::string logDirectory;
    std::function<bool(const std::string&)> errorHandler;
    std::atomic<bool> recoveryMode;
};

// Convenience macros for enhanced logging
#define NLM_LOG_DEBUG_ENHANCED(msg) EnhancedLogger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO_ENHANCED(msg) EnhancedLogger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING_ENHANCED(msg) EnhancedLogger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR_ENHANCED(msg) EnhancedLogger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL_ENHANCED(msg) EnhancedLogger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Global enhanced logger instance (convenience)
extern EnhancedLogger& gEnhancedLogger;

} // namespace nlm