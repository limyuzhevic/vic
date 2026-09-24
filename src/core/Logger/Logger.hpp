#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include "../Error/Error.hpp"

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure with error context support
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    ErrorContext errorContext;  // Enhanced with error context
    
    LogEntry() : level(LogLevel::Info), line(0), timestamp(std::chrono::system_clock::now()) {}
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", 
             int lineNum = 0, const std::string& function = "")
        : level(lvl), message(msg), file(file), line(lineNum), function(function), 
          timestamp(std::chrono::system_clock::now()) {}
};

// Logger interface with error handling support
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Error handling method
    virtual void logError(const NLMException& error) {
        // Default implementation converts error to log entry
        LogEntry entry;
        entry.level = errorToLogLevel(error.getSeverity());
        entry.message = error.what();
        entry.errorContext = error.getContext();
        log(entry);
    }
    
protected:
    LogLevel errorToLogLevel(ErrorSeverity severity) const {
        switch (severity) {
            case ErrorSeverity::Debug: return LogLevel::Debug;
            case ErrorSeverity::Info: return LogLevel::Info;
            case ErrorSeverity::Warning: return LogLevel::Warning;
            case ErrorSeverity::Error: return LogLevel::Error;
            case ErrorSeverity::Critical: return LogLevel::Critical;
            case ErrorSeverity::Fatal: return LogLevel::Critical;
            default: return LogLevel::Error;
        }
    }
};

// Global error logging function
inline void logError(const NLMException& error) {
    Logger::getGlobal().error(error.what());
}

// Convenience macros for error logging with error handling
#define NLM_ERROR_THROW(code, message, category) \
    nlm::ErrorHandling::handleException(nlm::NLMException(category, code, nlm::ErrorSeverity::Error, message, nlm::ErrorContext(__FILE__, __func__, __LINE__)))

} // namespace nlm
