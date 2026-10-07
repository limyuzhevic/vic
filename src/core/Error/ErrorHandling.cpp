// ErrorHandling.cpp - Implementation of comprehensive error handling system

#include "core/Error/ErrorHandling.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <vector>
#include <map>
#include <chrono>
#include <stacktrace>

namespace nlm {

// Default error handler implementation
void DefaultErrorHandler::logMessage(const std::string& message, LogLevel level) {
    std::cout << message << std::endl;
}

void DefaultErrorHandler::logError(const ErrorDetails& error) {
    logMessage(formatError(error), LogLevel::Error);
    if (!error.file.empty()) {
        logMessage("  File: " + error.file, LogLevel::Debug);
    }
    if (error.line > 0) {
        logMessage("  Line: " + std::to_string(error.line), LogLevel::Debug);
    }
    if (!error.function.empty()) {
        logMessage("  Function: " + error.function, LogLevel::Debug);
    }
    if (!error.description.empty()) {
        logMessage("  Description: " + error.description, LogLevel::Debug);
    }
    if (!error.suggestion.empty()) {
        logMessage("  Suggestion: " + error.suggestion, LogLevel::Info);
    }
    if (!error.stackTrace.empty()) {
        logMessage("  Stack Trace:", LogLevel::Debug);
        for (const auto& frame : error.stackTrace) {
            logMessage("    " + frame, LogLevel::Debug);
        }
    }
}

// Error classification implementation
ErrorCategory ErrorClassifier::classifyException(const std::exception& e) {
    std::string message = e.what();
    std::string lowerMessage = toLower(message);
    
    if (lowerMessage.find("memory") != std::string::npos || 
        lowerMessage.find("allocation") != std::string::npos ||
        lowerMessage.find("out of memory") != std::string::npos) {
        return ErrorCategory::Memory;
    } else if (lowerMessage.find("configuration") != std::string::npos ||
               lowerMessage.find("parse") != std::string::npos ||
               lowerMessage.find("invalid") != std::string::npos) {
        return ErrorCategory::Configuration;
    } else if (lowerMessage.find("neural") != std::string::npos ||
               lowerMessage.find("synapse") != std::string::npos ||
               lowerMessage.find("neuron") != std::string::npos) {
        return ErrorCategory::Neural;
    } else if (lowerMessage.find("file") != std::string::npos ||
               lowerMessage.find("io") != std::string::npos ||
               lowerMessage.find("read") != std::string::npos ||
               lowerMessage.find("write") != std::string::npos) {
        return ErrorCategory::IO;
    } else if (lowerMessage.find("plasticit") != std::string::npos) {
        return ErrorCategory::Plasticity;
    } else if (lowerMessage.find("development") != std::string::npos) {
        return ErrorCategory::Development;
    } else if (lowerMessage.find("neuromod") != std::string::npos) {
        return ErrorCategory::Neuromodulation;
    } else if (lowerMessage.find("memory") != std::string::npos && 
               (lowerMessage.find("system") != std::string::npos || lowerMessage.find("episol") != std::string::npos)) {
        return ErrorCategory::MemorySystem;
    } else if (lowerMessage.find("predict") != std::string::npos) {
        return ErrorCategory::Prediction;
    } else if (lowerMessage.find("cognit") != std::string::npos) {
        return ErrorCategory::Cognition;
    } else if (lowerMessage.find("validate") != std::string::npos) {
        return ErrorCategory::Validation;
    } else if (lowerMessage.find("performance") != std::string::npos) {
        return ErrorCategory::Performance;
    } else if (lowerMessage.find("security") != std::string::npos) {
        return ErrorCategory::Security;
    } else if (lowerMessage.find("network") != std::string::npos) {
        return ErrorCategory::Network;
    } else {
        return ErrorCategory::Experimental;
    }
}

ErrorSeverity ErrorClassifier::classifySeverity(const std::exception& e) {
    std::string message = e.what();
    std::string lowerMessage = toLower(message);
    
    if (lowerMessage.find("fatal") != std::string::npos ||
        lowerMessage.find("critical") != std::string::npos ||
        lowerMessage.find("out of memory") != std::string::npos ||
        lowerMessage.find("segmentation fault") != std::string::npos) {
        return ErrorSeverity::Fatal;
    } else if (lowerMessage.find("error") != std::string::npos) {
        return ErrorSeverity::Error;
    } else if (lowerMessage.find("warning") != std::string::npos) {
        return ErrorSeverity::Warning;
    } else {
        return ErrorSeverity::Info;
    }
}

std::string ErrorClassifier::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Error recovery manager implementation
bool ErrorRecoveryManager::isRetryable(const ErrorDetails& error, const std::exception& exception) const {
    std::string message = exception.what();
    std::string lowerMessage = toLower(message);
    
    // Check for transient errors
    if (lowerMessage.find("timeout") != std::string::npos ||
        lowerMessage.find("temporary") != std::string::npos ||
        lowerMessage.find("transient") != std::string::npos ||
        lowerMessage.find("retry") != std::string::npos) {
        return true;
    }
    
    // Check for network-related errors
    if (error.category == ErrorCategory::Network ||
        error.category == ErrorCategory::IO) {
        return true;
    }
    
    return false;
}

bool ErrorRecoveryManager::isSkippable(const ErrorDetails& error, const std::exception& exception) const {
    std::string message = exception.what();
    std::string lowerMessage = toLower(message);
    
    // Check for non-critical errors
    if (lowerMessage.find("not implemented") != std::string::npos ||
        lowerMessage.find("unsupported") != std::string::npos ||
        lowerMessage.find("feature not available") != std::string::npos) {
        return true;
    }
    
    return false;
}

bool ErrorRecoveryManager::isGracefulDegradeError(const ErrorDetails& error, const std::exception& exception) const {
    std::string message = exception.what();
    std::string lowerMessage = toLower(message);
    
    // Check for performance-related errors
    if (lowerMessage.find("performance") != std::string::npos ||
        lowerMessage.find("slow") != std::string::npos ||
        lowerMessage.find("resource") != std::string::npos) {
        return true;
    }
    
    // Check for optional features
    if (error.category == ErrorCategory::None && 
        error.recoveryAction == ErrorRecoveryAction::GracefulDegrade) {
        return true;
    }
    
    return false;
}

bool ErrorRecoveryManager::isRevertable(const ErrorDetails& error, const std::exception& exception) const {
    std::string message = exception.what();
    std::string lowerMessage = toLower(message);
    
    // Check for state-related errors
    if (lowerMessage.find("state") != std::string::npos &&
        (lowerMessage.find("corrupt") != std::string::npos ||
         lowerMessage.find("invalid") != std::string::npos)) {
        return true;
    }
    
    return false;
}

bool ErrorRecoveryManager::isShutdownRequired(const ErrorDetails& error, const std::exception& exception) const {
    std::string message = exception.what();
    std::string lowerMessage = toLower(message);
    
    // Check for fatal system errors
    if (lowerMessage.find("fatal") != std::string::npos ||
        lowerMessage.find("critical") != std::string::npos ||
        lowerMessage.find("system failure") != std::string::npos ||
        lowerMessage.find("panic") != std::string::npos) {
        return true;
    }
    
    return false;
}

std::string ErrorRecoveryManager::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Error reporter implementation
size_t ErrorReporter::countErrorsBySeverity(const std::vector<ErrorDetails>& errors, ErrorSeverity severity) {
    return std::count_if(errors.begin(), errors.end(),
        [severity](const ErrorDetails& error) { return error.severity == severity; });
}

// Global error handling implementation
void GlobalErrorHandling::shutdown() {
    NLM_LOG_FATAL("System shutdown requested due to fatal error");
    std::exit(1);
}

void GlobalErrorHandling::reportAndExit(const ErrorDetails& error) {
    NLM_LOG_FATAL("Fatal error: " + error.message);
    if (defaultErrorHandler) {
        defaultErrorHandler->handleError(error);
    }
    std::exit(1);
}

void GlobalErrorHandling::sendAlert(const ErrorDetails& error) {
    // Implementation for sending alerts (email, webhook, etc.)
    NLM_LOG_WARNING("ALERT: " + error.message);
}

void GlobalErrorHandling::restartComponent(const ErrorDetails& error) {
    // Implementation for restarting affected component
    NLM_LOG_WARNING("RESTART: Restarting component due to error: " + error.message);
}

void GlobalErrorHandling::fallforward(const ErrorDetails& error) {
    // Implementation for falling forward to next available path
    NLM_LOG_WARNING("FALLFORWARD: Continuing with alternate path due to error: " + error.message);
}

// Convenience functions implementation
void logError(const std::string& message, ErrorCategory category, 
              ErrorSeverity severity, ErrorRecoveryAction recovery) {
    ErrorDetails error("ERR_" + std::to_string(static_cast<int>(category)), message, category, severity, recovery);
    GlobalErrorHandling::getInstance().handleError(error);
}

void logWarning(const std::string& message, ErrorCategory category) {
    GlobalErrorHandling::getInstance().handleWarning(message);
}

void logInfo(const std::string& message, ErrorCategory category) {
    GlobalErrorHandling::getInstance().handleInfo(message);
}

void addSystemError(const std::exception& exception, const std::string& context) {
    ErrorDetails error;
    error.message = exception.what();
    error.description = context;
    error.category = ErrorClassifier::classifyException(exception);
    error.severity = ErrorClassifier::classifySeverity(exception);
    error.file = "Unknown";
    error.line = 0;
    error.function = "Unknown";
    
    GlobalErrorHandling::getInstance().handleError(error);
}

void setErrorDebugMode(bool enabled) {
    GlobalErrorHandling::getInstance().setDebugMode(enabled);
}

void setErrorMinLogSeverity(ErrorSeverity severity) {
    GlobalErrorHandling::getInstance().setMinLogSeverity(severity);
}

} // namespace nlm
