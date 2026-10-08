// Error handling and logging implementation

#include "error_handling.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <thread>

namespace nlm {

// NLMException implementation
std::string NLMException::getFullMessage() const {
    std::ostringstream oss;
    oss << "[" << errorCodeToString(code_) << "] ";
    oss << "[" << errorCategoryToString(category_) << "] ";
    oss << "[" << errorSeverityToString(severity_) << "] ";
    oss << what();
    
    if (!file_.empty() || line_ > 0) {
        oss << " (" << getLocationInfo() << ")";
    }
    
    return oss.str();
}

std::string NLMException::getLocationInfo() const {
    if (file_.empty() && line_ == 0) {
        return "unknown location";
    }
    
    std::string lineStr = (line_ > 0) ? ":" + std::to_string(line_) : "";
    return file_ + lineStr + " in " + function_;
}

std::string NLMException::getSeverityString() const {
    return errorSeverityToString(severity_);
}

std::string NLMException::getCategoryString() const {
    return errorCategoryToString(category_);
}

// ErrorHandler implementation
void ErrorHandler::reportError(const NLMException& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    stats_.totalErrors++;
    
    if (reportingEnabled_) {
        Logger::getGlobal().log(error.getLogLevel(), error.getFullMessage(),
                              error.getFile(), error.getLine(), error.what());
    }
    
    if (errorCallback_) {
        errorCallback_(error);
    }
}

bool ErrorHandler::handleError(const NLMException& error) {
    return ErrorRecovery::handleError(error, defaultConfig_);
}

int ErrorHandler::ErrorStats::errorsByCategory(int category) const {
    // Implementation would track category-specific errors
    return 0;
}

int ErrorHandler::ErrorStats::errorsBySeverity(int severity) const {
    // Implementation would track severity-specific errors
    return 0;
}

double ErrorHandler::ErrorStats::errorRate() const {
    // Implementation would calculate error rate
    return 0.0;
}

std::string ErrorHandler::getErrorMessage(ErrorCode code) {
    // Return human-readable message for error code
    switch (code) {
        case ErrorCode::Success: return "Operation completed successfully";
        case ErrorCode::SystemNotInitialized: return "System not initialized";
        case ErrorCode::InvalidParameter: return "Invalid parameter provided";
        case ErrorCode::OutOfMemory: return "Out of memory";
        case ErrorCode::FileNotFound: return "File not found";
        case ErrorCode::PermissionDenied: return "Permission denied";
        case ErrorCode::ConfigFileNotFound: return "Configuration file not found";
        case ErrorCode::InvalidConfigValue: return "Invalid configuration value";
        case ErrorCode::ConfigValidationFailed: return "Configuration validation failed";
        case ErrorCode::MissingRequiredConfig: return "Missing required configuration";
        case ErrorCode::MemoryAllocationFailed: return "Memory allocation failed";
        case ErrorCode::MemoryCorruption: return "Memory corruption detected";
        case ErrorCode::MemoryLimitExceeded: return "Memory limit exceeded";
        case ErrorCode::IntegrationFailed: return "Integration failed";
        case ErrorCode::SystemNotConnected: return "System not connected";
        case ErrorCode::CommunicationError: return "Communication error";
        case ErrorCode::Timeout: return "Operation timed out";
        case ErrorCode::PlasticityRuleNotFound: return "Plasticity rule not found";
        case ErrorCode::PlasticityParameterInvalid: return "Plasticity parameter invalid";
        case ErrorCode::STDPTimingError: return "STDP timing error";
        case ErrorCode::LearningNotConverged: return "Learning did not converge";
        case ErrorCode::LearningRateTooHigh: return "Learning rate too high";
        case ErrorCode::LearningRateTooLow: return "Learning rate too low";
        case ErrorCode::DevelopmentStageError: return "Development stage error";
        case ErrorCode::CriticalPeriodMissed: return "Critical period missed";
        case ErrorCode::MaturationError: return "Maturation error";
        case ErrorCode::UnknownError: return "Unknown error";
        case ErrorCode::NotImplemented: return "Not implemented";
        case ErrorCode::OperationCancelled: return "Operation cancelled";
        case ErrorCode::ResourceUnavailable: return "Resource unavailable";
        default: return "Unknown error code: " + std::to_string(static_cast<int>(code));
    }
}

NLMException ErrorHandler::createException(ErrorCode code, const std::string& message,
                                          const std::string& file, int line,
                                          const std::string& function) {
    ErrorSeverity severity = ErrorUtils::mapToSeverity(code);
    ErrorCategory category = ErrorCategory::Unknown;
    
    // Determine category based on error code
    if (code >= ErrorCode::ConfigFileNotFound && code <= ErrorCode::MissingRequiredConfig) {
        category = ErrorCategory::Configuration;
    } else if (code >= ErrorCode::MemoryAllocationFailed && code <= ErrorCode::MemoryLimitExceeded) {
        category = ErrorCategory::Memory;
    } else if (code >= ErrorCode::IntegrationFailed && code <= ErrorCode::Timeout) {
        category = ErrorCategory::Integration;
    } else if (code >= ErrorCode::PlasticityRuleNotFound && code <= ErrorCode::STDPTimingError) {
        category = ErrorCategory::Plasticity;
    } else if (code >= ErrorCode::LearningNotConverged && code <= ErrorCode::LearningRateTooLow) {
        category = ErrorCategory::Learning;
    } else if (code >= ErrorCode::DevelopmentStageError && code <= ErrorCode::MaturationError) {
        category = ErrorCategory::Development;
    }
    
    return NLMException(code, message + " (" + getErrorMessage(code) + ")",
                        category, severity, file, line, function);
}

// EnhancedLogger implementation
std::vector<EnhancedLogger::LogContext> EnhancedLogger::operationLogs_;
bool EnhancedLogger::performanceLoggingEnabled_ = true;
std::mutex EnhancedLogger::logsMutex_;

void EnhancedLogger::logWithContext(LogLevel level, const std::string& message,
                                  const std::string& operation,
                                  const std::string& component,
                                  const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(logsMutex_);
    
    LogContext context;
    context.operation = operation;
    context.component = component;
    context.startTime = std::chrono::system_clock::now();
    context.endTime = std::chrono::system_clock::now();
    context.severityLevel = static_cast<int>(level);
    context.details = message;
    
    operationLogs_.push_back(context);
    
    // Also log using the standard logger
    Logger::getGlobal().log(level, message, file, line, "");
}

void EnhancedLogger::logOperationStart(const std::string& operation,
                                      const std::string& component,
                                      const std::string& file, int line) {
    logWithContext(LogLevel::Info, "Starting operation: " + operation, operation, component, file, line);
}

void EnhancedLogger::logOperationEnd(const std::string& operation,
                                   const std::string& result,
                                   const std::string& details,
                                   const std::string& file, int line) {
    std::ostringstream oss;
    oss << "Operation completed: " << operation << " - " << result;
    if (!details.empty()) {
        oss << " - " << details;
    }
    
    logWithContext(LogLevel::Info, oss.str(), operation, "", file, line);
}

void EnhancedLogger::logPerformance(const std::string& operation,
                                   double duration,
                                   size_t memoryUsed,
                                   const std::string& component) {
    if (!performanceLoggingEnabled_) {
        return;
    }
    
    std::ostringstream oss;
    oss << "Performance: " << operation << " took " << duration << "ms";
    if (memoryUsed > 0) {
        oss << " (memory: " << memoryUsed << " bytes)";
    }
    if (!component.empty()) {
        oss << " [" << component << "]";
    }
    
    logWithContext(LogLevel::Info, oss.str(), operation, "performance");
}

std::vector<EnhancedLogger::LogContext> EnhancedLogger::getOperationLogs(const std::string& operation,
                                                                       const std::string& component) {
    std::lock_guard<std::mutex> lock(logsMutex_);
    
    std::vector<LogContext> result;
    for (const auto& log : operationLogs_) {
        if ((operation.empty() || log.operation == operation) &&
            (component.empty() || log.component == component)) {
            result.push_back(log);
        }
    }
    
    return result;
}

void EnhancedLogger::clearOperationLogs() {
    std::lock_guard<std::mutex> lock(logsMutex_);
    operationLogs_.clear();
}

// ErrorRecovery implementation
ErrorRecovery::RecoveryConfig ErrorRecovery::defaultConfig_ = {
    RecoveryStrategy::Retry,
    3, // maxRetries
    std::chrono::milliseconds(100), // retryDelay
    "", // fallbackValue
    true // enableLogging
};

std::mutex ErrorRecovery::mutex_;

bool ErrorRecovery::handleError(const NLMException& error, const RecoveryConfig& config) {
    switch (config.strategy) {
        case RecoveryStrategy::Retry:
            // Already handled in the template function
            return false;
            
        case RecoveryStrategy::Fallback:
            // Log that fallback was used
            EnhancedLogger::logWithContext(LogLevel::Warning, 
                                         "Using fallback value: " + config.fallbackValue,
                                         "error_recovery", "fallback");
            return true;
            
        case RecoveryStrategy::DisableFeature:
            EnhancedLogger::logWithContext(LogLevel::Warning,
                                         "Disabling feature due to error: " + std::string(error.what()),
                                         "error_recovery", "disable_feature");
            return true;
            
        case RecoveryStrategy::RestoreDefault:
            EnhancedLogger::logWithContext(LogLevel::Warning,
                                         "Restoring default configuration due to error",
                                         "error_recovery", "restore_default");
            return true;
            
        case RecoveryStrategy::Abort:
            EnhancedLogger::logWithContext(LogLevel::Error,
                                         "Aborting operation due to error: " + std::string(error.what()),
                                         "error_recovery", "abort");
            return false;
            
        default:
            return false;
    }
}

// ErrorUtils namespace implementation
namespace ErrorUtils {
    std::string formatError(const NLMException& error) {
        std::ostringstream oss;
        oss << "[" << error.getCode() << "] " << error.getSeverityString() << ": ";
        oss << error.what();
        if (!error.getFile().empty()) {
            oss << " at " << error.getFile();
            if (error.getLine() > 0) {
                oss << ":" << error.getLine();
            }
        }
        return oss.str();
    }
    
    std::string errorCategoryToString(ErrorCategory category) {
        switch (category) {
            case ErrorCategory::System: return "System";
            case ErrorCategory::Configuration: return "Configuration";
            case ErrorCategory::Memory: return "Memory";
            case ErrorCategory::Network: return "Network";
            case ErrorCategory::File: return "File";
            case ErrorCategory::Validation: return "Validation";
            case ErrorCategory::Integration: return "Integration";
            case ErrorCategory::Plasticity: return "Plasticity";
            case ErrorCategory::Learning: return "Learning";
            case ErrorCategory::Development: return "Development";
            case ErrorCategory::Unknown: return "Unknown";
            default: return "Unknown";
        }
    }
    
    std::string errorSeverityToString(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::Debug: return "DEBUG";
            case ErrorSeverity::Info: return "INFO";
            case ErrorSeverity::Warning: return "WARNING";
            case ErrorSeverity::Error: return "ERROR";
            case ErrorSeverity::Critical: return "CRITICAL";
            case ErrorSeverity::Fatal: return "FATAL";
            default: return "UNKNOWN";
        }
    }
    
    std::string errorCodeToString(ErrorCode code) {
        return std::to_string(static_cast<int>(code));
    }
    
    bool isRecoverable(ErrorCode code) {
        // Some error codes are recoverable, others are not
        switch (code) {
            case ErrorCode::Success: return true;
            case ErrorCode::InvalidParameter: return true;
            case ErrorCode::FileNotFound: return false;
            case ErrorCode::PermissionDenied: return false;
            case ErrorCode::ConfigFileNotFound: return true;
            case ErrorCode::InvalidConfigValue: return true;
            case ErrorCode::ConfigValidationFailed: return true;
            case ErrorCode::MissingRequiredConfig: return true;
            case ErrorCode::MemoryAllocationFailed: return false;
            case ErrorCode::MemoryCorruption: return false;
            case ErrorCode::MemoryLimitExceeded: return false;
            case ErrorCode::IntegrationFailed: return true;
            case ErrorCode::SystemNotConnected: return true;
            case ErrorCode::CommunicationError: return true;
            case ErrorCode::Timeout: return true;
            case ErrorCode::PlasticityRuleNotFound: return false;
            case ErrorCode::PlasticityParameterInvalid: return true;
            case ErrorCode::STDPTimingError: return true;
            case ErrorCode::LearningNotConverged: return true;
            case ErrorCode::LearningRateTooHigh: return true;
            case ErrorCode::LearningRateTooLow: return true;
            case ErrorCode::DevelopmentStageError: return false;
            case ErrorCode::CriticalPeriodMissed: return false;
            case ErrorCode::MaturationError: return false;
            case ErrorCode::UnknownError: return true;
            case ErrorCode::NotImplemented: return false;
            case ErrorCode::OperationCancelled: return true;
            case ErrorCode::ResourceUnavailable: return false;
            default: return true;
        }
    }
    
    ErrorSeverity mapToSeverity(ErrorCode code) {
        bool recoverable = isRecoverable(code);
        
        if (code == ErrorCode::Success || code == ErrorCode::NotImplemented) {
            return ErrorSeverity::Info;
        } else if (!recoverable) {
            return ErrorSeverity::Fatal;
        } else if (code >= ErrorCode::MemoryAllocationFailed && code <= ErrorCode::MemoryLimitExceeded) {
            return ErrorSeverity::Critical;
        } else if (code >= ErrorCode::IntegrationFailed && code <= ErrorCode::Timeout) {
            return ErrorSeverity::Error;
        } else if (code >= ErrorCode::PlasticityRuleNotFound && code <= ErrorCode::STDPTimingError) {
            return ErrorSeverity::Warning;
        } else if (code >= ErrorCode::LearningNotConverged && code <= ErrorCode::LearningRateTooLow) {
            return ErrorSeverity::Warning;
        } else {
            return ErrorSeverity::Error;
        }
    }
}

// ExternalErrorReporter implementation
bool ExternalErrorReporter::reportingEnabled_ = false;
std::map<std::string, std::string> ExternalErrorReporter::metadata_;
std::mutex ExternalErrorReporter::mutex_;

bool ExternalErrorReporter::reportError(const NLMException& error, const ReportConfig& config) {
    if (!reportingEnabled_ || !config.enableReporting) {
        return false;
    }
    
    // In a real implementation, this would send the error to an external endpoint
    // For now, we'll just log it locally
    EnhancedLogger::logWithContext(LogLevel::Warning,
                                 "Error report to external system: " + error.getFullMessage(),
                                 "external_reporter", "error_report");
    
    return true;
}

} // namespace nlm
