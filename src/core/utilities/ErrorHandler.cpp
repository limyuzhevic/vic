// Error Handling Implementation

#include "ErrorHandler.hpp"
#include <iomanip>
#include <sstream>
#include <ctime>

namespace nlm {

void NLMException::setContext(ErrorCategory category, ErrorSeverity severity, const std::string& file,
                              size_t line, const std::string& function,
                              const std::map<std::string, std::string>& metadata) {
    category_ = category;
    severity_ = severity;
    sourceFile_ = file;
    lineNumber_ = line;
    functionName_ = function;
    metadata_ = metadata;
}

std::string NLMException::formatWithContext(const std::string& message, const std::string& file, size_t line) {
    std::stringstream ss;
    ss << message;
    if (!file.empty()) {
        ss << " (File: " << file << ", Line: " << line << ")";
    }
    return ss.str();
}

std::string NLMException::getFormattedMessage() const {
    std::stringstream ss;
    ss << "[" << static_cast<int>(severity_) << "] " << what();
    if (!functionName_.empty()) {
        ss << " (Function: " << functionName_ << ")";
    }
    if (!sourceFile_.empty()) {
        ss << " (File: " << sourceFile_ << ":" << lineNumber_ << ")";
    }
    return ss.str();
}

std::string NLMException::getFullReport() const {
    std::stringstream ss;
    ss << "=== NLM Exception Report ===\n";
    ss << "Message: " << what() << "\n";
    ss << "Severity: " << static_cast<int>(severity_) << "\n";
    ss << "Category: " << static_cast<int>(category_) << "\n";
    ss << "Source File: " << sourceFile_ << ":" << lineNumber_ << "\n";
    ss << "Function: " << functionName_ << "\n";
    
    if (!metadata_.empty()) {
        ss << "Metadata:\n";
        for (const auto& pair : metadata_) {
            ss << "  " << pair.first << ": " << pair.second << "\n";
        }
    }
    
    ss << "Stack Trace:\n" << stackTrace << "\n";
    return ss.str();
}

ErrorHandler& ErrorHandler::getInstance() {
    static ErrorHandler instance;
    return instance;
}

void ErrorHandler::configure(bool enableDetailedLogging, size_t maxErrorHistory,
                             ErrorSeverity minSeverityToLog, bool enableErrorAggregation) {
    enableDetailedLogging_ = enableDetailedLogging;
    maxErrorHistory_ = maxErrorHistory;
    minSeverityToLog_ = minSeverityToLog;
    enableErrorAggregation_ = enableErrorAggregation;
}

void ErrorHandler::logError(const std::string& message, ErrorCategory category,
                           ErrorSeverity severity, const std::string& file,
                           size_t line, const std::string& function,
                           const std::map<std::string, std::string>& metadata) {
    ErrorContext context;
    context.category = category;
    context.severity = severity;
    context.sourceFile = file;
    context.lineNumber = line;
    context.functionName = function;
    context.message = message;
    context.metadata = metadata;
    context.timestamp = std::chrono::system_clock::now();
    
    addError(context);
}

void ErrorHandler::logException(const std::exception& e, ErrorCategory category) {
    logError(std::string("Exception: ") + e.what(), category, ErrorSeverity::Error);
}

void ErrorHandler::setErrorCallback(ErrorCallback callback) {
    errorCallback_ = callback;
}

ErrorHandler::ErrorStats ErrorHandler::getErrorStats() const {
    ErrorStats stats;
    stats.totalErrors = errorHistory_.size();
    
    for (const auto& error : errorHistory_) {
        stats.errorsByCategory[static_cast<size_t>(error.category)]++;
        stats.errorsBySeverity[static_cast<size_t>(error.severity)]++;
    }
    
    // Calculate average errors per minute
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - lastCleanupTime_);
    if (duration.count() > 0) {
        stats.averageErrorsPerMinute = static_cast<float>(stats.totalErrors) / duration.count();
    }
    
    return stats;
}

bool ErrorHandler::hasErrorsAboveThreshold(size_t threshold) const {
    return errorHistory_.size() > threshold;
}

bool ErrorHandler::validateSystemState() const {
    return true;
}

void ErrorHandler::addError(const ErrorContext& context) {
    if (errorCallback_) {
        std::string formatted = formatError(context);
        errorCallback_(context, formatted);
    }
    
    errorHistory_.push_back(context);
    
    if (errorHistory_.size() > maxErrorHistory_) {
        cleanupOldErrors();
    }
    
    if (enableErrorAggregation_) {
        aggregateErrors();
    }
}

std::string ErrorHandler::formatError(const ErrorContext& context) const {
    std::stringstream ss;
    ss << "[" << static_cast<int>(context.severity) << "] ";
    ss << "[" << static_cast<int>(context.category) << "] ";
    ss << context.message;
    
    if (!context.sourceFile.empty()) {
        ss << " (File: " << context.sourceFile << ":" << context.lineNumber << ")";
    }
    if (!context.functionName.empty()) {
        ss << " (Function: " << context.functionName << ")";
    }
    
    return ss.str();
}

void ErrorHandler::aggregateErrors() {
}

void ErrorHandler::cleanupOldErrors() {
    auto now = std::chrono::system_clock::now();
    auto cutoff = now - std::chrono::hours(24);
    
    errorHistory_.erase(
        std::remove_if(errorHistory_.begin(), errorHistory_.end(),
                      [cutoff](const ErrorContext& error) {
                          return error.timestamp < cutoff;
                      }),
        errorHistory_.end()
    );
    
    lastCleanupTime_ = now;
}

std::string formatErrorMessage(const std::string& message, const std::string& context) {
    if (context.empty()) {
        return message;
    }
    return context + ": " + message;
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string getStackTrace() {
    return "Stack trace not available (platform-specific)";
}

} // namespace nlm
