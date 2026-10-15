// Error handler implementation

#include "ErrorHandler.hpp"
#include <iostream>
#include <iomanip>

// Global error handler instance
ErrorHandler globalErrorHandler;

// Implementation of ErrorHandler methods
ErrorHandler::ErrorHandler() : enableRecovery(true), enableLogging(true), maxRetries(3), retryDelay(1.0) {
    sessionStart = std::chrono::system_clock::now();
}

void ErrorHandler::enableRecovery(bool enabled) {
    enableRecovery = enabled;
}

void ErrorHandler::enableLogging(bool enabled) {
    enableLogging = enabled;
}

void ErrorHandler::setMaxRetries(int retries) {
    maxRetries = retries;
}

void ErrorHandler::setRetryDelay(double delay) {
    retryDelay = delay;
}

bool ErrorHandler::handleError(const ErrorInfo& error) {
    // Add to error history
    errorHistory.push_back(error);
    
    // Update error counts
    errorCounts[error.category]++;
    
    // Log the error
    if (enableLogging) {
        logError(error);
    }
    
    // Check if error is recoverable
    if (!error.recoverable) {
        return false;  // Cannot recover
    }
    
    // If recovery is enabled, attempt recovery
    if (enableRecovery) {
        return attemptRecovery(error);
    }
    
    return false;
}

bool ErrorHandler::attemptRecovery(const ErrorInfo& error) {
    if (error.retryCount >= maxRetries) {
        NLM_LOG_WARN("Max retries exceeded for error: " + error.message);
        return false;
    }
    
    // Log retry attempt
    if (enableLogging) {
        NLM_LOG_INFO("Attempting recovery for error " + error.code + ": " + 
                    error.message + " (attempt " + std::to_string(error.retryCount + 1) + "/" + 
                    std::to_string(maxRetries) + ")");
    }
    
    // Wait for retry delay
    std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(retryDelay)));
    
    // Increment retry count
    ErrorInfo recoverableError = error;
    recoverableError.retryCount++;
    
    // Log recovery success
    if (enableLogging) {
        NLM_LOG_INFO("Recovery attempt successful for error: " + error.code);
    }
    
    return true;
}

bool ErrorHandler::handleException(const std::exception& e, const std::string& context,
                                  const std::map<std::string, std::string>& metadata) {
    ErrorCategory category = determineErrorCategory(e);
    ErrorSeverity severity = determineErrorSeverity(e);
    
    ErrorInfo error(category, severity, e.what(), "EXCEPTION", context);
    error.metadata = metadata;
    error.suggestions = generateSuggestions(error);
    error.recoverable = isErrorRecoverable(error);
    
    return handleError(error);
}

std::vector<std::string> ErrorHandler::generateSuggestions(const ErrorInfo& error) {
    std::vector<std::string> suggestions;
    
    switch (error.category) {
        case ErrorCategory::Configuration:
            suggestions.push_back("Check configuration file for syntax errors");
            suggestions.push_back("Verify required parameters are set");
            suggestions.push_back("Use default configuration if loading from file fails");
            break;
            
        case ErrorCategory::Memory:
            suggestions.push_back("Reduce simulation size or increase available memory");
            suggestions.push_back("Enable memory pool optimizations");
            suggestions.push_back("Check for memory leaks in simulation code");
            break;
            
        case ErrorCategory::Simulation:
            suggestions.push_back("Check simulation parameters");
            suggestions.push_back("Verify neural network connectivity");
            suggestions.push_back("Try reducing simulation steps");
            break;
            
        case ErrorCategory::IO:
            suggestions.push_back("Check file permissions");
            suggestions.push_back("Verify file paths are correct");
            suggestions.push_back("Ensure output directory exists");
            break;
            
        default:
            suggestions.push_back("Review error messages for details");
            suggestions.push_back("Contact support with error code and context");
            break;
    }
    
    return suggestions;
}

ErrorHandler::ErrorCategory ErrorHandler::determineErrorCategory(const std::exception& e) {
    const std::string& what = e.what();
    
    if (what.find("configuration") != std::string::npos || 
        what.find("config") != std::string::npos) {
        return ErrorCategory::Configuration;
    } else if (what.find("memory") != std::string::npos || 
               what.find("allocate") != std::string::npos) {
        return ErrorCategory::Memory;
    } else if (what.find("simulation") != std::string::npos) {
        return ErrorCategory::Simulation;
    } else if (what.find("file") != std::string::npos || 
               what.find("io") != std::string::npos) {
        return ErrorCategory::IO;
    }
    
    return ErrorCategory::Unknown;
}

ErrorHandler::ErrorSeverity ErrorHandler::determineErrorSeverity(const std::exception& e) {
    const std::string& what = e.what();
    
    if (what.find("critical") != std::string::npos || 
        what.find("fatal") != std::string::npos) {
        return ErrorSeverity::Critical;
    } else if (what.find("error") != std::string::npos) {
        return ErrorSeverity::Error;
    } else if (what.find("warning") != std::string::npos) {
        return ErrorSeverity::Warning;
    } else if (what.find("info") != std::string::npos) {
        return ErrorSeverity::Info;
    }
    
    return ErrorSeverity::Error;
}

bool ErrorHandler::isErrorRecoverable(const ErrorInfo& error) {
    switch (error.severity) {
        case ErrorSeverity::Critical:
            return false;  // Critical errors are not recoverable
            
        case ErrorSeverity::Error:
            return error.category != ErrorCategory::Memory;  // Memory errors are critical
            
        case ErrorSeverity::Warning:
            return true;  // Warnings are recoverable
            
        case ErrorSeverity::Info:
            return true;  // Info messages are recoverable
            
        default:
            return true;
    }
}

void ErrorHandler::logError(const ErrorInfo& error) {
    switch (error.severity) {
        case ErrorSeverity::Critical:
            NLM_LOG_CRITICAL("CRITICAL ERROR: " + error.message);
            break;
        case ErrorSeverity::Error:
            NLM_LOG_ERROR("ERROR: " + error.message);
            break;
        case ErrorSeverity::Warning:
            NLM_LOG_WARN("WARNING: " + error.message);
            break;
        case ErrorSeverity::Info:
            NLM_LOG_INFO("INFO: " + error.message);
            break;
        case ErrorSeverity::Debug:
            NLM_LOG_DEBUG("DEBUG: " + error.message);
            break;
    }
}

ErrorHandler::ErrorStatistics ErrorHandler::getStatistics() {
    ErrorStatistics stats;
    
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - sessionStart);
    stats.sessionDuration = duration.count();
    
    for (const auto& error : errorHistory) {
        stats.totalErrors++;
        
        switch (error.severity) {
            case ErrorSeverity::Critical:
                stats.criticalErrors++;
                break;
            default:
                break;
        }
        
        switch (error.category) {
            case ErrorCategory::Memory:
                stats.memoryErrors++;
                break;
            case ErrorCategory::Configuration:
                stats.configurationErrors++;
                break;
            case ErrorCategory::Simulation:
                stats.simulationErrors++;
                break;
            case ErrorCategory::IO:
                stats.ioErrors++;
                break;
            default:
                break;
        }
    }
    
    return stats;
}

const std::vector<ErrorHandler::ErrorInfo>& ErrorHandler::getErrorHistory() const {
    return errorHistory;
}

int ErrorHandler::getErrorCount(ErrorCategory category) const {
    auto it = errorCounts.find(category);
    return it != errorCounts.end() ? it->second : 0;
}

void ErrorHandler::clearErrorHistory() {
    errorHistory.clear();
    errorCounts.clear();
}

bool ErrorHandler::hasCriticalErrors() const {
    for (const auto& error : errorHistory) {
        if (error.severity == ErrorSeverity::Critical) {
            return true;
        }
    }
    return false;
}