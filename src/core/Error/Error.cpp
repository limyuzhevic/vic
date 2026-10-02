// Error implementation - Real implementation for the error handling system

#include "Error.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>
#include <cstring>
#include <cctype>
#include <thread>
#include <unordered_map>
#include <filesystem>

namespace nlm {

// ErrorReporter implementation
struct ErrorReporter::Impl {
    std::vector<std::shared_ptr<Error>> errors;
    std::mutex mutex;
    std::function<void(const Error&)> errorHandler;
    ErrorSeverity severityFilter = ErrorSeverity::Debug;
    ErrorCategory categoryFilter = ErrorCategory::None;
    size_t maxErrors = 1000;
    bool errorSuppressionEnabled = false;
    std::vector<std::string> suppressionPatterns;
    std::unordered_map<ErrorCode, std::function<bool(const Error&)>> recoveryHandlers;
    std::chrono::system_clock::time_point lastReportTime;
};

ErrorReporter::ErrorReporter() : pImpl(std::make_unique<Impl>()) {
    pImpl->lastReportTime = std::chrono::system_clock::now();
}

ErrorReporter::~ErrorReporter() = default;

void ErrorReporter::reportError(const Error& error) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Apply severity filter
    if (error.severity() < pImpl->severityFilter) {
        return;
    }
    
    // Apply category filter
    ErrorCategory errorCategory = categorizeError(error.code());
    if (pImpl->categoryFilter != ErrorCategory::None && 
        !(pImpl->categoryFilter & errorCategory)) {
        return;
    }
    
    // Apply error suppression
    if (pImpl->errorSuppressionEnabled) {
        bool suppressed = false;
        for (const auto& pattern : pImpl->suppressionPatterns) {
            if (error.message().find(pattern) != std::string::npos) {
                suppressed = true;
                break;
            }
        }
        if (suppressed) {
            return;
        }
    }
    
    // Add error to collection
    pImpl->errors.push_back(std::make_shared<Error>(error));
    
    // Trim if exceeding maximum
    if (pImpl->errors.size() > pImpl->maxErrors) {
        pImpl->errors.erase(pImpl->errors.begin());
    }
    
    // Invoke error handler if set
    if (pImpl->errorHandler) {
        pImpl->errorHandler(error);
    }
}

void ErrorReporter::setErrorHandler(std::function<void(const Error&)> handler) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorHandler = handler;
}

void ErrorReporter::clearErrorHandler() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorHandler = nullptr;
}

bool ErrorReporter::hasErrorHandler() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->errorHandler != nullptr;
}

void ErrorReporter::setSeverityFilter(ErrorSeverity minSeverity) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->severityFilter = minSeverity;
}

ErrorSeverity ErrorReporter::getSeverityFilter() const {
    return pImpl->severityFilter;
}

void ErrorReporter::setCategoryFilter(ErrorCategory categories) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->categoryFilter = categories;
}

ErrorCategory ErrorReporter::getCategoryFilter() const {
    return pImpl->categoryFilter;
}

void ErrorReporter::setMaxErrors(size_t maxErrors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxErrors = maxErrors;
    if (pImpl->errors.size() > maxErrors) {
        pImpl->errors.erase(pImpl->errors.begin(), 
                          pImpl->errors.begin() + (pImpl->errors.size() - maxErrors));
    }
}

size_t ErrorReporter::getMaxErrors() const {
    return pImpl->maxErrors;
}

size_t ErrorReporter::getCurrentErrorCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->errors.size();
}

void ErrorReporter::clearErrors() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errors.clear();
}

std::vector<std::shared_ptr<Error>> ErrorReporter::getRecentErrors(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (limit >= pImpl->errors.size()) {
        return pImpl->errors;
    }
    
    size_t startIndex = pImpl->errors.size() - limit;
    return std::vector<std::shared_ptr<Error>>(pImpl->errors.begin() + startIndex, 
                                              pImpl->errors.end());
}

std::string ErrorReporter::generateErrorReport() const {
    std::ostringstream oss;
    oss << "=== NLM Error Report ===\n";
    oss << "Generated at: " << formatTimestamp(std::chrono::system_clock::now()) << "\n";
    oss << "Total errors: " << pImpl->errors.size() << "\n";
    oss << "\nError breakdown by severity:\n";
    
    std::unordered_map<ErrorSeverity, size_t> errorsBySeverity;
    for (const auto& error : pImpl->errors) {
        errorsBySeverity[error->severity()]++;
    }
    
    for (const auto& pair : errorsBySeverity) {
        oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
    }
    
    oss << "\nError breakdown by category:\n";
    std::unordered_map<ErrorCategory, size_t> errorsByCategory;
    for (const auto& error : pImpl->errors) {
        ErrorCategory category = categorizeError(error->code());
        errorsByCategory[category]++;
    }
    
    for (const auto& pair : errorsByCategory) {
        oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
    }
    
    oss << "\nRecent errors:\n";
    for (size_t i = 0; i < std::min(size_t(10), pImpl->errors.size()); ++i) {
        const auto& error = pImpl->errors[i];
        oss << "  " << (i + 1) << ". " << error->what() << "\n";
    }
    
    return oss.str();
}

void ErrorReporter::exportErrors(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << generateErrorReport();
        file.close();
    }
}

void ErrorReporter::suppressError(const std::string& pattern) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorSuppressionEnabled = true;
    pImpl->suppressionPatterns.push_back(pattern);
}

void ErrorReporter::enableErrorSuppression(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorSuppressionEnabled = enable;
}

bool ErrorReporter::isErrorSuppressionEnabled() const {
    return pImpl->errorSuppressionEnabled;
}

bool ErrorReporter::attemptRecovery(const Error& error) {
    auto it = pImpl->recoveryHandlers.find(error.code());
    if (it != pImpl->recoveryHandlers.end()) {
        return it->second(error);
    }
    return false;
}

void ErrorReporter::registerRecoveryHandler(ErrorCode code, std::function<bool(const Error&)> handler) {
    pImpl->recoveryHandlers[code] = handler;
}

// ErrorBuilder implementation
ErrorBuilder::ErrorBuilder(const std::string& component, const std::string& operation) {
    context_.component = component;
    context_.operation = operation;
    context_.timestamp = std::chrono::system_clock::now();
    context_.threadId = std::this_thread::get_id();
}

ErrorBuilder& ErrorBuilder::setMessage(const std::string& message) {
    context_.message = message;
    return *this;
}

ErrorBuilder& ErrorBuilder::setErrorCode(ErrorCode code) {
    metadata_["error_code"] = std::to_string(static_cast<int>(code));
    return *this;
}

ErrorBuilder& ErrorBuilder::setSeverity(ErrorSeverity severity) {
    // Note: This would need to be stored in the context for proper error creation
    // For now, we'll store it in metadata
    metadata_["severity"] = std::to_string(static_cast<int>(severity));
    return *this;
}

ErrorBuilder& ErrorBuilder::setFile(const std::string& file, int line, const std::string& function) {
    context_.file = file;
    context_.line = line;
    context_.function = function;
    return *this;
}

ErrorBuilder& ErrorBuilder::addMetadata(const std::string& key, const std::string& value) {
    context_.metadata[key] = value;
    return *this;
}

ErrorBuilder& ErrorBuilder::addMetadata(const std::string& key, double value) {
    std::ostringstream oss;
    oss << value;
    context_.metadata[key] = oss.str();
    return *this;
}

ErrorBuilder& ErrorBuilder::addMetric(const std::string& key, double value) {
    context_.metrics[key] = value;
    return *this;
}

ErrorBuilder& ErrorBuilder::addRelatedError(std::shared_ptr<ErrorContext> error) {
    context_.relatedErrors.push_back(std::move(error));
    return *this;
}

std::shared_ptr<Error> ErrorBuilder::build() const {
    // Determine severity from metadata
    ErrorSeverity severity = ErrorSeverity::Error;
    auto it = context_.metadata.find("severity");
    if (it != context_.metadata.end()) {
        severity = static_cast<ErrorSeverity>(std::stoi(it->second));
    }
    
    return std::make_shared<Error>(context_, severity);
}

std::shared_ptr<ConfigurationError> ErrorBuilder::buildConfigurationError() const {
    // Determine error code from metadata
    ErrorCode code = ErrorCode::UnknownError;
    auto it = context_.metadata.find("error_code");
    if (it != context_.metadata.end()) {
        code = static_cast<ErrorCode>(std::stoi(it->second));
    }
    
    // Determine severity
    ErrorSeverity severity = ErrorSeverity::Error;
    it = context_.metadata.find("severity");
    if (it != context_.metadata.end()) {
        severity = static_cast<ErrorSeverity>(std::stoi(it->second));
    }
    
    return std::make_shared<ConfigurationError>(context_, code, severity);
}

std::shared_ptr<InitializationError> ErrorBuilder::buildInitializationError() const {
    ErrorCode code = ErrorCode::UnknownError;
    auto it = context_.metadata.find("error_code");
    if (it != context_.metadata.end()) {
        code = static_cast<ErrorCode>(std::stoi(it->second));
    }
    
    ErrorSeverity severity = ErrorSeverity::Error;
    it = context_.metadata.find("severity");
    if (it != context_.metadata.end()) {
        severity = static_cast<ErrorSeverity>(std::stoi(it->second));
    }
    
    return std::make_shared<InitializationError>(context_, code, severity);
}

std::shared_ptr<MemoryError> ErrorBuilder::buildMemoryError() const {
    ErrorCode code = ErrorCode::UnknownError;
    auto it = context_.metadata.find("error_code");
    if (it != context_.metadata.end()) {
        code = static_cast<ErrorCode>(std::stoi(it->second));
    }
    
    ErrorSeverity severity = ErrorSeverity::Error;
    it = context_.metadata.find("severity");
    if (it != context_.metadata.end()) {
        severity = static_cast<ErrorSeverity>(std::stoi(it->second));
    }
    
    return std::make_shared<MemoryError>(context_, code, severity);
}

std::shared_ptr<SystemError> ErrorBuilder::buildSystemError() const {
    ErrorCode code = ErrorCode::UnknownError;
    auto it = context_.metadata.find("error_code");
    if (it != context_.metadata.end()) {
        code = static_cast<ErrorCode>(std::stoi(it->second));
    }
    
    ErrorSeverity severity = ErrorSeverity::Error;
    it = context_.metadata.find("severity");
    if (it != context_.metadata.end()) {
        severity = static_cast<ErrorSeverity>(std::stoi(it->second));
    }
    
    return std::make_shared<SystemError>(context_, code, severity);
}

// Global error reporter instance
ErrorReporter& getGlobalErrorReporter() {
    static ErrorReporter globalErrorReporter;
    return globalErrorReporter;
}

// Error categorization helper
ErrorCategory categorizeError(ErrorCode code) {
    if (code >= ErrorCode::ConfigFileNotFound && code <= ErrorCode::ConfigValueInvalid) {
        return ErrorCategory::Configuration;
    } else if (code >= ErrorCode::BrainInitFailed && code <= ErrorCode::ConceptFormationNotInitialized) {
        return ErrorCategory::Initialization;
    } else if (code >= ErrorCode::MemoryAllocationFailed && code <= ErrorCode::MemoryCorruptionDetected) {
        return ErrorCategory::Memory;
    } else if (code >= ErrorCode::FileNotFound && code <= ErrorCode::DirectoryNotFound) {
        return ErrorCategory::FileSystem;
    } else if (code >= ErrorCode::ConvergenceFailed && code <= ErrorCode::InvalidInputData) {
        return ErrorCategory::Algorithm;
    } else if (code >= ErrorCode::ResourceExhausted && code <= ErrorCode::CapacityExceeded) {
        return ErrorCategory::Resource;
    } else if (code >= ErrorCode::ComponentNotAvailable && code <= ErrorCode::ConfigurationMismatch) {
        return ErrorCategory::Integration;
    } else if (code >= ErrorCode::SystemNotReady && code <= ErrorCode::OperationNotSupported) {
        return ErrorCategory::System;
    }
    return ErrorCategory::None;
}

// Helper functions
std::string formatTimestamp(const std::chrono::system_clock::time_point& tp, const std::string& format) {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_buf;
    localtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, format.c_str());
    return oss.str();
}

std::string sanitizeLogMessage(const std::string& message) {
    std::string result = message;
    // Remove potentially problematic characters
    result.erase(std::remove_if(result.begin(), result.end(),
        [](char c) { return c == '\r' || c == '\n'; }), result.end());
    return result;
}

} // namespace nlm