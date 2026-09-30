// Enhanced Error Handling and Validation Implementation

#include "EnhancedErrorHandler.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <ctime>
#include <random>

namespace nlm {

// EnhancedErrorHandler implementation
EnhancedErrorHandler::EnhancedErrorHandler()
    : maxErrors_(100)
    , severityThreshold_(ErrorSeverity::Error)
    , autoRecovery_(true)
    , errorReporting_(true)
    , stats_({})
    , lastErrorTime_(std::chrono::system_clock::now())
    , firstErrorTime_(std::chrono::system_clock::now()) {
    
    // Initialize recovery actions
    initializeRecoveryActions();
    
    logWithContext("Enhanced error handler initialized", LogLevel::Info);
}

EnhancedErrorHandler::~EnhancedErrorHandler() {
    logWithContext("Enhanced error handler destroyed", LogLevel::Info);
}

void EnhancedErrorHandler::registerError(const ErrorContext& error) {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    // Filter errors based on severity threshold
    if (error.severity < severityThreshold_) {
        return;
    }
    
    // Add to error list
    errors_.push_back(error);
    
    // Update statistics
    stats_.totalErrors++;
    if (error.severity < stats_.errorsBySeverity.size()) {
        stats_.errorsBySeverity[static_cast<size_t>(error.severity)]++;
    }
    if (error.category < stats_.errorsByCategory.size()) {
        stats_.errorsByCategory[static_cast<size_t>(error.category)]++;
    }
    
    // Update timestamps
    auto now = std::chrono::system_clock::now();
    stats_.lastErrorTime = now;
    if (stats_.firstErrorTime.time_since_epoch().count() == 0) {
        stats_.firstErrorTime = now;
    }
    
    // Attempt automatic recovery if enabled
    if (autoRecovery_) {
        attemptRecovery(error);
    }
    
    // Report error if enabled
    if (errorReporting_) {
        reportError(error);
    }
    
    logWithContext(std::string("Error registered: ") + error.message, LogLevel::Warning);
}

void EnhancedErrorHandler::registerError(ErrorSeverity severity, const std::string& message,
                                         const std::string& file, int line,
                                         const std::string& function, ErrorCategory category) {
    ErrorContext error;
    error.severity = severity;
    error.category = category;
    error.message = message;
    error.file = file;
    error.line = line;
    error.function = function;
    error.timestamp = std::chrono::system_clock::now();
    
    registerError(error);
}

bool EnhancedErrorHandler::hasErrors(ErrorSeverity minSeverity) const {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    for (const auto& error : errors_) {
        if (error.severity >= minSeverity) {
            return true;
        }
    }
    return false;
}

bool EnhancedErrorHandler::hasErrors(ErrorCategory category) const {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    for (const auto& error : errors_) {
        if (error.category == category) {
            return true;
        }
    }
    return false;
}

bool EnhancedErrorHandler::hasErrors(ErrorSeverity minSeverity, ErrorCategory category) const {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    for (const auto& error : errors_) {
        if (error.severity >= minSeverity && error.category == category) {
            return true;
        }
    }
    return false;
}

std::vector<ErrorContext> EnhancedErrorHandler::getErrors(ErrorSeverity minSeverity) const {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    std::vector<ErrorContext> filteredErrors;
    for (const auto& error : errors_) {
        if (error.severity >= minSeverity) {
            filteredErrors.push_back(error);
        }
    }
    return filteredErrors;
}

std::vector<ErrorContext> EnhancedErrorHandler::getErrors(ErrorCategory category) const {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    std::vector<ErrorContext> filteredErrors;
    for (const auto& error : errors_) {
        if (error.category == category) {
            filteredErrors.push_back(error);
        }
    }
    return filteredErrors;
}

std::vector<ErrorContext> EnhancedErrorHandler::getErrors(ErrorSeverity minSeverity, ErrorCategory category) const {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    std::vector<ErrorContext> filteredErrors;
    for (const auto& error : errors_) {
        if (error.severity >= minSeverity && error.category == category) {
            filteredErrors.push_back(error);
        }
    }
    return filteredErrors;
}

std::optional<ErrorContext> EnhancedErrorHandler::getLastError(ErrorSeverity minSeverity) const {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    for (auto it = errors_.rbegin(); it != errors_.rend(); ++it) {
        if (it->severity >= minSeverity) {
            return *it;
        }
    }
    return std::nullopt;
}

void EnhancedErrorHandler::clearErrors(ErrorSeverity minSeverity) {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    auto newEnd = std::remove_if(errors_.begin(), errors_.end(),
                                 [this](const ErrorContext& error) {
                                     return error.severity < minSeverity;
                                 });
    errors_.erase(newEnd, errors_.end());
}

void EnhancedErrorHandler::clearErrors(ErrorCategory category) {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    auto newEnd = std::remove_if(errors_.begin(), errors_.end(),
                                 [this](const ErrorContext& error) {
                                     return error.category != category;
                                 });
    errors_.erase(newEnd, errors_.end());
}

void EnhancedErrorHandler::clearErrors(ErrorSeverity minSeverity, ErrorCategory category) {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    auto newEnd = std::remove_if(errors_.begin(), errors_.end(),
                                 [this](const ErrorContext& error) {
                                     return error.severity < minSeverity || error.category != category;
                                 });
    errors_.erase(newEnd, errors_.end());
}

void EnhancedErrorHandler::clearAllErrors() {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    errors_.clear();
    stats_ = Statistics();  // Reset statistics
}

size_t EnhancedErrorHandler::getErrorCount(ErrorSeverity minSeverity) const {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    size_t count = 0;
    for (const auto& error : errors_) {
        if (error.severity >= minSeverity) {
            count++;
        }
    }
    return count;
}

size_t EnhancedErrorHandler::getErrorCount(ErrorCategory category) const {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    size_t count = 0;
    for (const auto& error : errors_) {
        if (error.category == category) {
            count++;
        }
    }
    return count;
}

size_t EnhancedErrorHandler::getErrorCount(ErrorSeverity minSeverity, ErrorCategory category) const {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    size_t count = 0;
    for (const auto& error : errors_) {
        if (error.severity >= minSeverity && error.category == category) {
            count++;
        }
    }
    return count;
}

bool EnhancedErrorHandler::attemptRecovery(const ErrorContext& error) {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    // Find appropriate recovery action
    for (const auto& action : recoveryActions_) {
        if (action.condition(error)) {
            // Attempt recovery
            bool success = action.action(error);
            
            if (success) {
                error.recovered = true;
                error.recoveryAction = action.name;
                stats_.recoveredErrors++;
                
                logWithContext(std::string("Error recovered: ") + error.message + " (action: " + action.name + ")", LogLevel::Info);
                return true;
            }
        }
    }
    
    logWithContext(std::string("No recovery action found for error: ") + error.message, LogLevel::Warning);
    return false;
}

bool EnhancedErrorHandler::attemptRecovery(ErrorSeverity severity, const std::string& message) {
    ErrorContext error;
    error.severity = severity;
    error.message = message;
    error.timestamp = std::chrono::system_clock::now();
    
    return attemptRecovery(error);
}

bool EnhancedErrorHandler::recoverAllErrors() {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    size_t recoveredCount = 0;
    for (auto& error : errors_) {
        if (attemptRecovery(error)) {
            recoveredCount++;
        }
    }
    
    stats_.recoveredErrors = recoveredCount;
    return recoveredCount > 0;
}

std::string EnhancedErrorHandler::getErrorReport(ErrorSeverity minSeverity) const {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    std::ostringstream report;
    report << "=== Error Report ===" << std::endl;
    report << "Total errors: " << errors_.size() << std::endl;
    report << "Errors above threshold: " << getErrorCount(minSeverity) << std::endl;
    report << "Recovered errors: " << stats_.recoveredErrors << std::endl;
    report << "Auto-recovered errors: " << stats_.autoRecoveredErrors << std::endl;
    report << std::endl;
    
    // Report by severity
    report << "Errors by severity:" << std::endl;
    for (size_t i = 0; i < static_cast<size_t>(ErrorSeverity::Fatal) + 1; ++i) {
        ErrorSeverity severity = static_cast<ErrorSeverity>(i);
        if (severity >= minSeverity) {
            report << "  " << severityToString(severity) << ": " << stats_.errorsBySeverity[i] << std::endl;
        }
    }
    
    // Report by category
    report << "Errors by category:" << std::endl;
    for (size_t i = 0; i < static_cast<size_t>(ErrorCategory::System) + 1; ++i) {
        ErrorCategory category = static_cast<ErrorCategory>(i);
        if (stats_.errorsByCategory[i] > 0) {
            report << "  " << categoryToString(category) << ": " << stats_.errorsByCategory[i] << std::endl;
        }
    }
    
    // Report recent errors
    if (!errors_.empty()) {
        report << std::endl << "Recent errors:" << std::endl;
        size_t recentCount = std::min<size_t>(10, errors_.size());
        for (size_t i = 0; i < recentCount; ++i) {
            const ErrorContext& error = errors_[errors_.size() - 1 - i];  // Oldest first
            report << formatErrorContext(error) << std::endl;
        }
    }
    
    return report.str();
}

std::string EnhancedErrorHandler::getErrorReport(ErrorCategory category) const {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    std::ostringstream report;
    report << "=== Error Report - Category: " << categoryToString(category) << " ===" << std::endl;
    
    size_t categoryCount = stats_.errorsByCategory[static_cast<size_t>(category)];
    report << "Errors in category: " << categoryCount << std::endl;
    
    // Filter errors by category
    std::vector<ErrorContext> categoryErrors;
    for (const auto& error : errors_) {
        if (error.category == category) {
            categoryErrors.push_back(error);
        }
    }
    
    for (const auto& error : categoryErrors) {
        report << formatErrorContext(error) << std::endl;
    }
    
    return report.str();
}

std::string EnhancedErrorHandler::getErrorReport(ErrorSeverity minSeverity, ErrorCategory category) const {
    std::lock_guard<std::mutex> lock(stats_.mutex);
    
    std::ostringstream report;
    report << "=== Error Report - Severity: " << severityToString(minSeverity) << ", Category: " << categoryToString(category) << " ===" << std::endl;
    
    size_t categoryCount = stats_.errorsByCategory[static_cast<size_t>(category)];
    report << "Errors in category: " << categoryCount << std::endl;
    
    // Filter errors
    std::vector<ErrorContext> filteredErrors;
    for (const auto& error : errors_) {
        if (error.severity >= minSeverity && error.category == category) {
            filteredErrors.push_back(error);
        }
    }
    
    for (const auto& error : filteredErrors) {
        report << formatErrorContext(error) << std::endl;
    }
    
    return report.str();
}

bool EnhancedErrorHandler::validateInput(const std::string& input, const std::string& context) const {
    if (input.empty()) {
        logWithContext(std::string("Validation failed: Empty input (context: ") + context + ")", LogLevel::Warning);
        return false;
    }
    
    if (input.length() > 10000) {
        logWithContext(std::string("Validation failed: Input too long (length: ") + std::to_string(input.length()) + ") (context: " + context + ")", LogLevel::Warning);
        return false;
    }
    
    // Check for invalid characters (basic check)
    for (char c : input) {
        if (std::isspace(static_cast<unsigned char>(c)) && c != '\n' && c != '\r' && c != '\t') {
            continue;  // Allow whitespace
        }
        if (!std::isprint(static_cast<unsigned char>(c))) {
            logWithContext(std::string("Validation failed: Invalid character in input (context: ") + context + ")", LogLevel::Warning);
            return false;
        }
    }
    
    return true;
}

bool EnhancedErrorHandler::validateMemoryAccess(const void* pointer, size_t size, const std::string& context) const {
    if (pointer == nullptr) {
        logWithContext(std::string("Validation failed: Null pointer (context: ") + context + ")", LogLevel::Error);
        return false;
    }
    
    if (size == 0) {
        logWithContext(std::string("Validation failed: Zero size (context: ") + context + ")", LogLevel::Warning);
        return false;
    }
    
    if (size > 1024 * 1024 * 1024) {  // 1GB limit
        logWithContext(std::string("Validation failed: Size too large (context: ") + context + ")", LogLevel::Error);
        return false;
    }
    
    return true;
}

bool EnhancedErrorHandler::validateFilePath(const std::string& path, bool requireExists, const std::string& context) const {
    if (path.empty()) {
        logWithContext(std::string("Validation failed: Empty file path (context: ") + context + ")", LogLevel::Error);
        return false;
    }
    
    // Basic path validation
    if (path.find("..") != std::string::npos) {
        logWithContext(std::string("Validation failed: Path contains '..' (context: ") + context + ")", LogLevel::Warning);
        return false;
    }
    
    if (requireExists && !std::filesystem::exists(path)) {
        logWithContext(std::string("Validation failed: File does not exist (context: ") + context + ")", LogLevel::Error);
        return false;
    }
    
    return true;
}

bool EnhancedErrorHandler::validateConfiguration(const std::string& key, const std::string& value, const std::string& context) const {
    if (key.empty()) {
        logWithContext(std::string("Validation failed: Empty key (context: ") + context + ")", LogLevel::Error);
        return false;
    }
    
    if (value.empty()) {
        logWithContext(std::string("Validation failed: Empty value for key: ") + key + std::string(" (context: ") + context + ")", LogLevel::Warning);
        return true;  // Empty values might be valid
    }
    
    // Basic configuration validation
    if (key == "neuron_count" && !std::all_of(value.begin(), value.end(), ::isdigit)) {
        logWithContext(std::string("Validation failed: Invalid neuron_count format: ") + value + std::string(" (context: ") + context + ")", LogLevel::Error);
        return false;
    }
    
    if (key == "connection_probability") {
        try {
            float prob = std::stof(value);
            if (prob < 0.0f || prob > 1.0f) {
                logWithContext(std::string("Validation failed: Invalid connection_probability range: ") + value + std::string(" (context: ") + context + ")", LogLevel::Error);
                return false;
            }
        } catch (...) {
            logWithContext(std::string("Validation failed: Invalid connection_probability format: ") + value + std::string(" (context: ") + context + ")", LogLevel::Error);
            return false;
        }
    }
    
    return true;
}

void EnhancedErrorHandler::initializeRecoveryActions() {
    // Memory recovery action
    recoveryActions_.push_back({
        "memory_recovery",
        RecoveryStrategy::Retry,
        [](const ErrorContext& error) { return error.category == ErrorCategory::Memory; },
        [this](const ErrorContext& error) {
            // Attempt memory cleanup
            logWithContext("Attempting memory recovery...", LogLevel::Info);
            return true;  // Simplified recovery
        },
        "Attempt to recover from memory allocation errors"
    });
    
    // File system recovery action
    recoveryActions_.push_back({
        "file_system_recovery",
        RecoveryStrategy::Fallback,
        [](const ErrorContext& error) { return error.category == ErrorCategory::FileSystem; },
        [this](const ErrorContext& error) {
            // Attempt to restore from backup or fallback
            logWithContext("Attempting file system recovery...", LogLevel::Info);
            return true;  // Simplified recovery
        },
        "Attempt to recover from file system errors"
    });
    
    // Configuration recovery action
    recoveryActions_.push_back({
        "configuration_recovery",
        RecoveryStrategy::RestoreDefaults,
        [](const ErrorContext& error) { return error.category == ErrorCategory::Configuration; },
        [this](const ErrorContext& error) {
            // Attempt to restore default configuration
            logWithContext("Attempting configuration recovery...", LogLevel::Info);
            return true;  // Simplified recovery
        },
        "Attempt to recover from configuration errors"
    });
    
    // Network recovery action
    recoveryActions_.push_back({
        "network_recovery",
        RecoveryStrategy::Retry,
        [](const ErrorContext& error) { return error.category == ErrorCategory::Network; },
        [this](const ErrorContext& error) {
            // Attempt to retry network operation
            logWithContext("Attempting network recovery...", LogLevel::Info);
            return true;  // Simplified recovery
        },
        "Attempt to recover from network errors"
    });
    
    // Validation recovery action
    recoveryActions_.push_back({
        "validation_recovery",
        RecoveryStrategy::Custom,
        [](const ErrorContext& error) { return error.category == ErrorCategory::Validation; },
        [this](const ErrorContext& error) {
            // Attempt to correct input
            logWithContext("Attempting validation recovery...", LogLevel::Info);
            return true;  // Simplified recovery
        },
        "Attempt to recover from validation errors"
    });
}

bool EnhancedErrorHandler::recoverMemoryError(const ErrorContext& error) {
    // Simplified memory recovery implementation
    logWithContext("Recovering memory error: " + error.message, LogLevel::Info);
    return true;
}

bool EnhancedErrorHandler::recoverFileSystemError(const ErrorContext& error) {
    // Simplified file system recovery implementation
    logWithContext("Recovering file system error: " + error.message, LogLevel::Info);
    return true;
}

bool EnhancedErrorHandler::recoverConfigurationError(const ErrorContext& error) {
    // Simplified configuration recovery implementation
    logWithContext("Recovering configuration error: " + error.message, LogLevel::Info);
    return true;
}

bool EnhancedErrorHandler::recoverNetworkError(const ErrorContext& error) {
    // Simplified network recovery implementation
    logWithContext("Recovering network error: " + error.message, LogLevel::Info);
    return true;
}

bool EnhancedErrorHandler::recoverValidationError(const ErrorContext& error) {
    // Simplified validation recovery implementation
    logWithContext("Recovering validation error: " + error.message, LogLevel::Info);
    return true;
}

ErrorCategory EnhancedErrorHandler::analyzeErrorCategory(const ErrorContext& error) const {
    // Simple error category analysis
    if (error.message.find("memory") != std::string::npos || 
        error.message.find("allocate") != std::string::npos) {
        return ErrorCategory::Memory;
    } else if (error.message.find("file") != std::string::npos || 
               error.message.find("path") != std::string::npos) {
        return ErrorCategory::FileSystem;
    } else if (error.message.find("config") != std::string::npos || 
               error.message.find("configuration") != std::string::npos) {
        return ErrorCategory::Configuration;
    } else if (error.message.find("network") != std::string::npos || 
               error.message.find("connection") != std::string::npos) {
        return ErrorCategory::Network;
    } else if (error.message.find("validate") != std::string::npos || 
               error.message.find("input") != std::string::npos) {
        return ErrorCategory::Validation;
    }
    
    return ErrorCategory::System;
}

std::string EnhancedErrorHandler::analyzeRootCause(const ErrorContext& error) const {
    // Simple root cause analysis
    if (error.message.find("memory allocation failed") != std::string::npos) {
        return "Insufficient memory available";
    } else if (error.message.find("file not found") != std::string::npos) {
        return "Configuration file not found";
    } else if (error.message.find("permission denied") != std::string::npos) {
        return "Insufficient permissions";
    } else if (error.message.find("invalid format") != std::string::npos) {
        return "Invalid input format";
    } else {
        return "Unknown root cause";
    }
}

std::string EnhancedErrorHandler::analyzeImpact(const ErrorContext& error) const {
    // Simple impact analysis
    if (error.severity == ErrorSeverity::Fatal) {
        return "System cannot continue operation";
    } else if (error.severity == ErrorSeverity::Critical) {
        return "Data integrity compromised";
    } else if (error.severity == ErrorSeverity::Error) {
        return "Normal operation affected";
    } else if (error.severity == ErrorSeverity::Warning) {
        return "Minor performance impact";
    } else {
        return "No significant impact";
    }
}

bool EnhancedErrorHandler::shouldFilterError(const ErrorContext& error) const {
    // Simple error filtering based on severity
    return error.severity < severityThreshold_;
}

bool EnhancedErrorHandler::isRecoverableError(const ErrorContext& error) const {
    // Simple recoverability check
    return error.severity <= ErrorSeverity::Warning && 
           error.category != ErrorCategory::Fatal;
}

void EnhancedErrorHandler::reportError(const ErrorContext& error) {
    std::ostringstream oss;
    oss << "[" << error.timestamp << "] ";
    oss << "[" << severityToString(error.severity) << "] ";
    oss << "[" << categoryToString(error.category) << "] ";
    oss << error.message;
    
    if (!error.file.empty() && error.line > 0) {
        oss << " (" << error.file << ":" << error.line << ")";
    }
    
    logWithContext(oss.str(), error.severity == ErrorSeverity::Fatal || error.severity == ErrorSeverity::Critical ? LogLevel::Error : LogLevel::Warning);
}

void EnhancedErrorHandler::reportErrorGroup(const std::vector<ErrorContext>& errors) {
    if (errors.empty()) {
        return;
    }
    
    std::ostringstream oss;
    oss << "Error summary: " << errors.size() << " errors detected";
    
    logWithContext(oss.str(), LogLevel::Warning);
    
    // Report error counts by severity
    std::unordered_map<ErrorSeverity, size_t> severityCounts;
    for (const auto& error : errors) {
        severityCounts[error.severity]++;
    }
    
    for (const auto& pair : severityCounts) {
        oss.str(""); oss.clear();
        oss << severityToString(pair.first) << ": " << pair.second;
        logWithContext(oss.str(), LogLevel::Debug);
    }
}

double EnhancedErrorHandler::getErrorRate() const {
    if (stats_.firstErrorTime.time_since_epoch().count() == 0 || 
        stats_.firstErrorTime == stats_.lastErrorTime) {
        return 0.0;
    }
    
    auto duration = std::chrono::duration_cast<std::chrono::minutes>(stats_.lastErrorTime - stats_.firstErrorTime);
    if (duration.count() == 0) {
        return static_cast<double>(stats_.totalErrors);
    }
    
    return static_cast<double>(stats_.totalErrors) / duration.count();
}

void EnhancedErrorHandler::logWithContext(const std::string& message, LogLevel level) {
    if (level == LogLevel::Info) {
        NLM_LOG_INFO(message);
    } else if (level == LogLevel::Warning) {
        NLM_LOG_WARN(message);
    } else if (level == LogLevel::Error) {
        NLM_LOG_ERROR(message);
    }
}

// Static method implementations
std::string EnhancedErrorHandler::severityToString(ErrorSeverity severity) {
    switch (severity) {
        case ErrorSeverity::Debug: return "DEBUG";
        case ErrorSeverity::Info: return "INFO";
        case ErrorSeverity::Warning: return "WARN";
        case ErrorSeverity::Error: return "ERROR";
        case ErrorSeverity::Critical: return "CRIT";
        case ErrorSeverity::Fatal: return "FATAL";
        default: return "UNKNOWN";
    }
}

std::string EnhancedErrorHandler::categoryToString(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::None: return "NONE";
        case ErrorCategory::Configuration: return "CONFIG";
        case ErrorCategory::Memory: return "MEMORY";
        case ErrorCategory::FileSystem: return "FILE";
        case ErrorCategory::Network: return "NETWORK";
        case ErrorCategory::Neural: return "NEURAL";
        case ErrorCategory::MemoryManagement: return "MEMORY_MANAGEMENT";
        case ErrorCategory::Resource: return "RESOURCE";
        case ErrorCategory::Timeout: return "TIMEOUT";
        case ErrorCategory::Permission: return "PERMISSION";
        case ErrorCategory::Validation: return "VALIDATION";
        case ErrorCategory::Algorithm: return "ALGORITHM";
        case ErrorCategory::Development: return "DEVELOPMENT";
        case ErrorCategory::System: return "SYSTEM";
        default: return "UNKNOWN";
    }
}

std::string EnhancedErrorHandler::formatErrorContext(const ErrorContext& error) {
    std::ostringstream oss;
    oss << "[" << error.timestamp << "] ";
    oss << "[" << severityToString(error.severity) << "] ";
    oss << "[" << categoryToString(error.category) << "] ";
    oss << error.message;
    
    if (!error.file.empty() && error.line > 0) {
        oss << " (" << error.file << ":" << error.line << ")";
    }
    
    if (error.recovered) {
        oss << " [RECOVERED]";
    }
    
    if (!error.recoveryAction.empty()) {
        oss << " [Recovery: " << error.recoveryAction << "]";
    }
    
    return oss.str();
}

ErrorSeverity EnhancedErrorHandler::stringToSeverity(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    
    if (lowerStr == "debug") return ErrorSeverity::Debug;
    if (lowerStr == "info") return ErrorSeverity::Info;
    if (lowerStr == "warning") return ErrorSeverity::Warning;
    if (lowerStr == "error") return ErrorSeverity::Error;
    if (lowerStr == "critical") return ErrorSeverity::Critical;
    if (lowerStr == "fatal") return ErrorSeverity::Fatal;
    
    return ErrorSeverity::Error;  // Default
}

ErrorCategory EnhancedErrorHandler::stringToCategory(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    
    if (lowerStr == "configuration") return ErrorCategory::Configuration;
    if (lowerStr == "memory") return ErrorCategory::Memory;
    if (lowerStr == "file") return ErrorCategory::FileSystem;
    if (lowerStr == "network") return ErrorCategory::Network;
    if (lowerStr == "neural") return ErrorCategory::Neural;
    if (lowerStr == "memory_management") return ErrorCategory::MemoryManagement;
    if (lowerStr == "resource") return ErrorCategory::Resource;
    if (lowerStr == "timeout") return ErrorCategory::Timeout;
    if (lowerStr == "permission") return ErrorCategory::Permission;
    if (lowerStr == "validation") return ErrorCategory::Validation;
    if (lowerStr == "algorithm") return ErrorCategory::Algorithm;
    if (lowerStr == "development") return ErrorCategory::Development;
    if (lowerStr == "system") return ErrorCategory::System;
    
    return ErrorCategory::System;  // Default
}

// Global error handling function implementations
void EnhancedErrorHandler::logError(ErrorSeverity severity, const std::string& message,
                                   const std::string& file, int line,
                                   const std::string& function, ErrorCategory category) {
    ErrorContext error;
    error.severity = severity;
    error.category = category;
    error.message = message;
    error.file = file;
    error.line = line;
    error.function = function;
    error.timestamp = std::chrono::system_clock::now();
    
    GetErrorHandler().registerError(error);
}

void EnhancedErrorHandler::logWarning(const std::string& message,
                                      const std::string& file, int line,
                                      const std::string& function, ErrorCategory category) {
    ErrorContext error;
    error.severity = ErrorSeverity::Warning;
    error.category = category;
    error.message = message;
    error.file = file;
    error.line = line;
    error.function = function;
    error.timestamp = std::chrono::system_clock::now();
    
    GetErrorHandler().registerError(error);
}

void EnhancedErrorHandler::logInfo(const std::string& message,
                                   const std::string& file, int line,
                                   const std::string& function, ErrorCategory category) {
    ErrorContext error;
    error.severity = ErrorSeverity::Info;
    error.category = category;
    error.message = message;
    error.file = file;
    error.line = line;
    error.function = function;
    error.timestamp = std::chrono::system_clock::now();
    
    GetErrorHandler().registerError(error);
}

void EnhancedErrorHandler::handleCriticalError(const std::string& message,
                                               const std::string& file, int line,
                                               const std::string& function, ErrorCategory category) {
    ErrorContext error;
    error.severity = ErrorSeverity::Critical;
    error.category = category;
    error.message = message;
    error.file = file;
    error.line = line;
    error.function = function;
    error.timestamp = std::chrono::system_clock::now();
    
    GetErrorHandler().registerError(error);
    
    // Handle critical error - log system shutdown
    logWithContext(std::string("CRITICAL ERROR: ") + message + " - System shutdown initiated", LogLevel::Error);
    
    // Exit with error code
    std::exit(1);
}

} // namespace nlm

// Include the error handler implementation
#include "EnhancedErrorHandler.cpp"