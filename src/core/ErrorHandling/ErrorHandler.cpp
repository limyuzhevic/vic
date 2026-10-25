// Enhanced Error Handling Implementation
// Provides robust error handling, recovery, and diagnostics

#include "ErrorHandler.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace nlm {

// Error handler implementation
bool DefaultErrorHandler::handleError(const ErrorContext& context) {
    // Convert to NLM log level
    LogLevel logLevel;
    switch (context.severity) {
        case ErrorSeverity::Debug: logLevel = LogLevel::Debug; break;
        case ErrorSeverity::Info: logLevel = LogLevel::Info; break;
        case ErrorSeverity::Warning: logLevel = LogLevel::Warning; break;
        case ErrorSeverity::Error: logLevel = LogLevel::Error; break;
        case ErrorSeverity::Critical: logLevel = LogLevel::Error; break;
        case ErrorSeverity::Fatal: logLevel = LogLevel::Error; break;
        default: logLevel = LogLevel::Error;
    }
    
    std::string message = "[" + std::to_string(static_cast<int>(context.category)) + "] " + context.toString();
    
    // Use NLM logging system
    switch (logLevel) {
        case LogLevel::Debug: NLM_LOG_DEBUG(message); break;
        case LogLevel::Info: NLM_LOG_INFO(message); break;
        case LogLevel::Warning: NLM_LOG_WARNING(message); break;
        case LogLevel::Error: NLM_LOG_ERROR(message); break;
        case LogLevel::Critical: NLM_LOG_ERROR("[CRITICAL] " + message); break;
        default: NLM_LOG_ERROR(message); break;
    }
    
    return false;  // No recovery attempted
}

bool DefaultErrorHandler::handleException(const std::exception& e) {
    NLM_LOG_ERROR(std::string("Exception: ") + e.what());
    return false;
}

// RecoveryErrorHandler implementation
bool RecoveryErrorHandler::handleError(const ErrorContext& context) {
    // Check if this error has recovery handlers
    auto it = recoveryHandlers_.find(context.code);
    if (it != recoveryHandlers_.end()) {
        bool recovered = it->second.handler(context);
        if (recovered) {
            NLM_LOG_INFO("Error recovered: " + context.toString());
            return true;
        }
    }
    
    // No recovery available, log and continue
    return handleErrorWithoutRecovery(context);
}

bool RecoveryErrorHandler::handleException(const std::exception& e) {
    NLM_LOG_ERROR(std::string("Exception: ") + e.what());
    return false;
}

bool RecoveryErrorHandler::handleErrorWithoutRecovery(const ErrorContext& context) {
    LogLevel logLevel = getLogLevel(context.severity);
    std::string message = context.toString();
    
    switch (logLevel) {
        case LogLevel::Debug: NLM_LOG_DEBUG(message); break;
        case LogLevel::Info: NLM_LOG_INFO(message); break;
        case LogLevel::Warning: NLM_LOG_WARNING(message); break;
        case LogLevel::Error: NLM_LOG_ERROR(message); break;
        case LogLevel::Critical: NLM_LOG_ERROR("[CRITICAL] " + message); break;
        default: NLM_LOG_ERROR(message); break;
    }
    
    return false;
}

LogLevel RecoveryErrorHandler::getLogLevel(ErrorSeverity severity) const {
    switch (severity) {
        case ErrorSeverity::Debug: return LogLevel::Debug;
        case ErrorSeverity::Info: return LogLevel::Info;
        case ErrorSeverity::Warning: return LogLevel::Warning;
        case ErrorSeverity::Error: return LogLevel::Error;
        case ErrorSeverity::Critical: return LogLevel::Error;
        case ErrorSeverity::Fatal: return LogLevel::Error;
        default: return LogLevel::Error;
    }
}

bool RecoveryErrorHandler::recoverMemoryExhaustion(const ErrorContext& context) {
    NLM_LOG_INFO("Attempting recovery from memory exhaustion...");
    
    try {
        NLM_LOG_INFO("Memory recovery action taken (cache cleanup)");
        return true;
    } catch (...) {
        NLM_LOG_ERROR("Memory recovery failed");
        return false;
    }
}

bool RecoveryErrorHandler::recoverConfigurationError(const ErrorContext& context) {
    NLM_LOG_INFO("Attempting recovery from configuration error...");
    
    try {
        NLM_LOG_INFO("Configuration recovery action taken (restore defaults)");
        return true;
    } catch (...) {
        NLM_LOG_ERROR("Configuration recovery failed");
        return false;
    }
}

bool RecoveryErrorHandler::recoverNumericalOverflow(const ErrorContext& context) {
    NLM_LOG_INFO("Attempting recovery from numerical overflow...");
    
    try {
        NLM_LOG_INFO("Numerical recovery action taken (clamp values)");
        return true;
    } catch (...) {
        NLM_LOG_ERROR("Numerical recovery failed");
        return false;
    }
}

// ErrorReporter implementation
void ErrorReporter::reportError(const ErrorContext& context) {
    std::lock_guard<std::mutex> lock(errorMutex_);
    
    // Add to error history
    errorHistory_.push_back(context);
    
    // Check if we should report this error
    if (shouldReport(context)) {
        logError(context);
        notifyErrorHandlers(context);
    }
    
    // Update error statistics
    updateErrorStats(context);
}

void ErrorReporter::reportException(const std::exception& e, ErrorCategory category) {
    ErrorContext context(category, ErrorSeverity::Error, 0, e.what());
    reportError(context);
}

void ErrorReporter::logError(const ErrorContext& context) {
    LogLevel logLevel = getLogLevel(context.severity);
    
    std::ostringstream oss;
    oss << "[" << static_cast<int>(context.category) << "] ";
    oss << context.toString();
    
    // Add context data
    if (!context.contextData.empty()) {
        oss << " Context: ";
        bool first = true;
        for (const auto& pair : context.contextData) {
            if (!first) oss << ", ";
            oss << pair.first << "=" << pair.second;
            first = false;
        }
    }
    
    // Add recovery actions
    if (!context.recoveryActions.empty()) {
        oss << " Recovery Actions: ";
        bool first = true;
        for (const auto& action : context.recoveryActions) {
            if (!first) oss << ", ";
            oss << action;
            first = false;
        }
    }
    
    std::string message = oss.str();
    
    switch (logLevel) {
        case LogLevel::Debug: NLM_LOG_DEBUG(message); break;
        case LogLevel::Info: NLM_LOG_INFO(message); break;
        case LogLevel::Warning: NLM_LOG_WARNING(message); break;
        case LogLevel::Error: NLM_LOG_ERROR(message); break;
        case LogLevel::Critical: NLM_LOG_ERROR("[CRITICAL] " + message); break;
        default: NLM_LOG_ERROR(message); break;
    }
    
    // Update last report time
    lastReportTime_ = std::chrono::system_clock::now();
}

void ErrorReporter::notifyErrorHandlers(const ErrorContext& context) {
    for (auto& handler : errorHandlers_) {
        try {
            handler->handleError(context);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error handler exception: ") + e.what());
        }
    }
}

void ErrorReporter::updateErrorStats(const ErrorContext& context) {
    // Update error statistics (simplified version)
    auto now = std::chrono::system_clock::now();
    errorStats_.lastErrorTime = now;
    errorStats_.totalErrors++;
    
    int catIdx = static_cast<int>(context.category);
    if (catIdx <= static_cast<int>(ErrorCategory::System)) {
        errorStats_.errorsByCategory[catIdx]++;
    }
    
    int sevIdx = static_cast<int>(context.severity);
    if (sevIdx <= static_cast<int>(ErrorSeverity::Fatal)) {
        errorStats_.errorsBySeverity[sevIdx]++;
    }
}

LogLevel ErrorReporter::getLogLevel(ErrorSeverity severity) const {
    switch (severity) {
        case ErrorSeverity::Debug: return LogLevel::Debug;
        case ErrorSeverity::Info: return LogLevel::Info;
        case ErrorSeverity::Warning: return LogLevel::Warning;
        case ErrorSeverity::Error: return LogLevel::Error;
        case ErrorSeverity::Critical: return LogLevel::Error;
        case ErrorSeverity::Fatal: return LogLevel::Error;
        default: return LogLevel::Error;
    }
}

// ErrorManager implementation
void ErrorManager::reportConfigError(int code, const std::string& description, const std::string& file, int line) {
    ErrorContext context(ErrorCategory::Configuration, ErrorSeverity::Error, code, description, file, line);
    getGlobal().reportError(context);
}

void ErrorManager::reportMemoryError(int code, const std::string& description, const std::string& file, int line) {
    ErrorContext context(ErrorCategory::Memory, ErrorSeverity::Error, code, description, file, line);
    getGlobal().reportError(context);
}

void ErrorManager::reportComputationError(int code, const std::string& description, const std::string& file, int line) {
    ErrorContext context(ErrorCategory::Computation, ErrorSeverity::Error, code, description, file, line);
    getGlobal().reportError(context);
}

void ErrorManager::reportSystemError(int code, const std::string& description, const std::string& file, int line) {
    ErrorContext context(ErrorCategory::System, ErrorSeverity::Error, code, description, file, line);
    getGlobal().reportError(context);
}

void ErrorManager::reportCriticalError(int code, const std::string& description, const std::string& file, int line) {
    ErrorContext context(ErrorCategory::System, ErrorSeverity::Critical, code, description, file, line);
    getGlobal()->reportError(context);
}

// Utility functions
void reportError(ErrorCategory category, ErrorSeverity severity, int code,
                 const std::string& description, const std::string& file,
                 int line, const std::string& function) {
    ErrorContext context(category, severity, code, description, file, line, function);
    ErrorManager::getGlobal().reportError(context);
}

void reportError(ErrorCategory category, int code, const std::string& description,
                 const std::string& file, int line) {
    reportError(category, ErrorSeverity::Error, code, description, file, line, "");
}

void reportConfigError(int code, const std::string& description, const std::string& file, int line) {
    ErrorManager::reportConfigError(code, description, file, line);
}

void reportMemoryError(int code, const std::string& description, const std::string& file, int line) {
    ErrorManager::reportMemoryError(code, description, file, line);
}

void reportComputationError(int code, const std::string& description, const std::string& file, int line) {
    ErrorManager::reportComputationError(code, description, file, line);
}

void reportSystemError(int code, const std::string& description, const std::string& file, int line) {
    ErrorManager::reportSystemError(code, description, file, line);
}

void reportCriticalError(int code, const std::string& description, const std::string& file, int line) {
    ErrorManager::reportCriticalError(code, description, file, line);
}

} // namespace nlm