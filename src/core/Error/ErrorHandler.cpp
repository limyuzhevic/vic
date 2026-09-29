#include "ErrorHandler.hpp"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <random>
#include <algorithm>

namespace nlm {

ErrorHandler& ErrorHandler::getInstance() {
    static ErrorHandler instance;
    return instance;
}

ErrorHandler::ErrorHandler() {
    // Initialize recovery strategies for common error types
    recoveryStrategies_[ErrorCode::ConnectionError] = RecoveryStrategy::Retry;
    recoveryStrategies_[ErrorCode::Timeout] = RecoveryStrategy::Retry;
    recoveryStrategies_[ErrorCode::FileReadError] = RecoveryStrategy::Retry;
    recoveryStrategies_[ErrorCode::FileWriteError] = RecoveryStrategy::Fallback;
    recoveryStrategies_[ErrorCode::ValidationError] = RecoveryStrategy::GracefulDegradation;
    recoveryStrategies_[ErrorCode::MemoryAllocation] = RecoveryStrategy::GracefulDegradation;
    
    // Initialize error stats
    for (int i = 0; i < 10000; ++i) {
        ErrorCode code = static_cast<ErrorCode>(i);
        errorStats_[code] = {
            code,
            "",
            ErrorCategory::System,
            ErrorSeverity::Error,
            0,
            std::chrono::system_clock::now(),
            std::chrono::system_clock::now(),
            0.0,
            {}
        };
    }
}

ErrorHandler::~ErrorHandler() = default;

void ErrorHandler::handleError(const NLMBaseException& error) {
    ErrorEntry entry;
    entry.code = error.getCode();
    entry.message = error.what();
    entry.category = getErrorCategoryFromCode(entry.code);
    entry.severity = getErrorSeverityFromCode(entry.code);
    entry.file = error.getFile();
    entry.line = error.getLine();
    entry.function = error.getFunction();
    entry.timestamp = std::chrono::system_clock::now();
    entry.context = error.getContext();
    entry.stackTrace = generateStackTrace();
    entry.correlationId = generateCorrelationId();
    
    logError(entry);
    updateErrorStats(entry);
    checkAlertThreshold();
}

void ErrorHandler::handleError(ErrorCode code, const std::string& message,
                               const std::string& file, int line,
                               const std::string& function) {
    NLMBaseException error(message, code, file, line, function);
    handleError(error);
}

void ErrorHandler::handleError(const std::exception& error) {
    ErrorEntry entry;
    entry.code = ErrorCode::UnknownError;
    entry.message = error.what();
    entry.category = ErrorCategory::System;
    entry.severity = ErrorSeverity::Error;
    entry.file = "";
    entry.line = 0;
    entry.function = "";
    entry.timestamp = std::chrono::system_clock::now();
    entry.stackTrace = generateStackTrace();
    entry.correlationId = generateCorrelationId();
    
    logError(entry);
    updateErrorStats(entry);
}

RecoveryResult ErrorHandler::recover(const NLMBaseException& error, const std::string& operation) {
    std::lock_guard<std::mutex> lock(mutex_);
    return executeRecoveryStrategy(error, operation);
}

RecoveryResult ErrorHandler::retry(const std::function<void()>& operation, 
                                  const RetryConfig& config) {
    if (!operation) {
        return RecoveryResult::Failed;
    }
    
    int maxRetries = config.maxRetries > 0 ? config.maxRetries : retryConfig_.maxRetries;
    std::chrono::milliseconds initialDelay = config.initialDelay.count() > 0 ? 
        config.initialDelay : retryConfig_.initialDelay;
    double backoffMultiplier = config.backoffMultiplier > 0.0 ? 
        config.backoffMultiplier : retryConfig_.backoffMultiplier;
    auto maxDelay = config.maxDelay.count() > 0 ? config.maxDelay : retryConfig_.maxDelay;
    
    std::vector<ErrorCode> retryableErrors = config.retryableErrors.empty() ? 
        retryConfig_.retryableErrors : config.retryableErrors;
    
    for (int attempt = 0; attempt < maxRetries; ++attempt) {
        try {
            operation();
            return RecoveryResult::Success;
        } catch (const NLMBaseException& e) {
            if (attempt == maxRetries - 1) {
                handleError(e);
                return RecoveryResult::Failed;
            }
            
            if (!isRetryable(e.getCode()) || 
                std::find(retryableErrors.begin(), retryableErrors.end(), e.getCode()) == retryableErrors.end()) {
                handleError(e);
                return RecoveryResult::Failed;
            }
            
            std::this_thread::sleep_for(initialDelay * std::pow(backoffMultiplier, attempt));
            initialDelay = std::min(initialDelay * backoffMultiplier, maxDelay);
        } catch (const std::exception& e) {
            if (attempt == maxRetries - 1) {
                handleError(e);
                return RecoveryResult::Failed;
            }
            
            std::this_thread::sleep_for(initialDelay * std::pow(backoffMultiplier, attempt));
            initialDelay = std::min(initialDelay * backoffMultiplier, maxDelay);
        }
    }
    
    return RecoveryResult::Failed;
}

void ErrorHandler::setRecoveryStrategy(ErrorCode code, RecoveryStrategy strategy) {
    std::lock_guard<std::mutex> lock(mutex_);
    recoveryStrategies_[code] = strategy;
}

void ErrorHandler::setRetryConfig(const RetryConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    retryConfig_ = config;
}

void ErrorHandler::setMaxRetries(int maxRetries) {
    std::lock_guard<std::mutex> lock(mutex_);
    retryConfig_.maxRetries = maxRetries;
}

void ErrorHandler::setEnableCheckpointRecovery(bool enable) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_.enableCheckpointRecovery = enable;
}

bool ErrorHandler::saveCheckpoint(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!config_.enableCheckpointRecovery) {
        return false;
    }
    
    json checkpoint;
    checkpoint["name"] = name;
    checkpoint["timestamp"] = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    checkpoint["errorHistory"] = json::array();
    
    for (const auto& error : errorHistory_) {
        checkpoint["errorHistory"].push_back(error.toJson());
    }
    
    checkpoints_[name] = checkpoint;
    return true;
}

bool ErrorHandler::restoreCheckpoint(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = checkpoints_.find(name);
    if (it == checkpoints_.end()) {
        return false;
    }
    
    const json& checkpoint = it->second;
    errorHistory_.clear();
    
    for (const auto& errorJson : checkpoint["errorHistory"]) {
        errorHistory_.push_back(ErrorEntry::fromJson(errorJson));
    }
    
    return true;
}

void ErrorHandler::clearCheckpoints() {
    std::lock_guard<std::mutex> lock(mutex_);
    checkpoints_.clear();
}

const std::vector<ErrorEntry>& ErrorHandler::getRecentErrors() const {
    return errorHistory_;
}

const ErrorStats& ErrorHandler::getErrorStats(ErrorCode code) const {
    return errorStats_.at(code);
}

std::map<ErrorCode, ErrorStats> ErrorHandler::getAllErrorStats() const {
    return errorStats_;
}

void ErrorHandler::setEnableStructuredLogging(bool enable) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_.enableStructuredLogging = enable;
}

void ErrorHandler::setStructuredLogFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_.structuredLogFile = filepath;
}

void ErrorHandler::setEnableErrorCorrelation(bool enable) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_.enableErrorCorrelation = enable;
}

void ErrorHandler::setAlertThreshold(double threshold) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_.alertThreshold = threshold;
}

std::vector<ErrorPattern> ErrorHandler::detectErrorPatterns(const std::chrono::system_clock::time_point& since) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ErrorPattern> patterns;
    
    // Group errors by code
    std::map<ErrorCode, std::vector<ErrorEntry>> errorsByCode;
    for (const auto& error : errorHistory_) {
        if (error.timestamp >= since) {
            errorsByCode[error.code].push_back(error);
        }
    }
    
    // Detect patterns
    for (const auto& [code, errors] : errorsByCode) {
        if (errors.size() < 3) continue; // Need at least 3 occurrences for pattern detection
        
        ErrorPattern pattern;
        pattern.code = code;
        pattern.patternType = "recurring_error";
        pattern.firstOccurrence = errors.front().timestamp;
        pattern.lastOccurrence = errors.back().timestamp;
        pattern.occurrenceCount = errors.size();
        
        // Analyze error context to identify pattern
        for (const auto& error : errors) {
            for (const auto& [key, value] : error.context) {
                pattern.patternData[key] += value + "; ";
            }
        }
        
        pattern.suggestedAction = "Investigate recurring error pattern for code: " + 
                                 getErrorCodeString(code);
        
        patterns.push_back(pattern);
    }
    
    return patterns;
}

std::string ErrorHandler::analyzeRootCause(const ErrorEntry& error) const {
    std::stringstream ss;
    ss << "Root Cause Analysis for Error Code: " << static_cast<int>(error.code) << "\n";
    ss << "Message: " << error.message << "\n";
    ss << "Category: " << getErrorCategoryString(error.category) << "\n";
    ss << "Severity: " << getErrorSeverityString(error.severity) << "\n";
    ss << "Context: ";
    
    bool first = true;
    for (const auto& [key, value] : error.context) {
        if (!first) ss << ", ";
        ss << key << "=" << value;
        first = false;
    }
    
    ss << "\n\nAnalysis:\n";
    
    // Common root cause patterns
    if (error.code == ErrorCode::MemoryAllocation || error.code == ErrorCode::OutOfMemory) {
        ss << "Possible cause: Memory allocation failure.\n";
        ss << "Suggest: Check memory usage and increase available memory.\n";
    } else if (error.code == ErrorCode::FileNotFound || error.code == ErrorCode::PermissionDenied) {
        ss << "Possible cause: File system access issue.\n";
        ss << "Suggest: Verify file paths and permissions.\n";
    } else if (error.code == ErrorCode::ConnectionError || error.code == ErrorCode::Timeout) {
        ss << "Possible cause: Network connectivity issue.\n";
        ss << "Suggest: Check network connectivity and retry after delay.\n";
    } else if (error.code == ErrorCode::InvalidParameter || error.code == ErrorCode::ValidationError) {
        ss << "Possible cause: Invalid input parameters.\n";
        ss << "Suggest: Validate input data and configuration.\n";
    } else {
        ss << "Possible cause: Unknown system error.\n";
        ss << "Suggest: Review system logs and configuration.\n";
    }
    
    return ss.str();
}

std::string ErrorHandler::getErrorCodeString(ErrorCode code) const {
    switch (code) {
        case ErrorCode::Success: return "SUCCESS";
        case ErrorCode::ValidationError: return "VALIDATION_ERROR";
        case ErrorCode::InvalidParameter: return "INVALID_PARAMETER";
        case ErrorCode::MissingConfiguration: return "MISSING_CONFIGURATION";
        case ErrorCode::FileNotFound: return "FILE_NOT_FOUND";
        case ErrorCode::PermissionDenied: return "PERMISSION_DENIED";
        case ErrorCode::MemoryAllocation: return "MEMORY_ALLOCATION";
        case ErrorCode::MemoryCorruption: return "MEMORY_CORRUPTION";
        case ErrorCode::OutOfMemory: return "OUT_OF_MEMORY";
        case ErrorCode::FileReadError: return "FILE_READ_ERROR";
        case ErrorCode::FileWriteError: return "FILE_WRITE_ERROR";
        case ErrorCode::DirectoryNotFound: return "DIRECTORY_NOT_FOUND";
        case ErrorCode::ConnectionError: return "CONNECTION_ERROR";
        case ErrorCode::Timeout: return "TIMEOUT";
        case ErrorCode::ProtocolError: return "PROTOCOL_ERROR";
        case ErrorCode::NeuralNetworkError: return "NEURAL_NETWORK_ERROR";
        case ErrorCode::InvalidArchitecture: return "INVALID_ARCHITECTURE";
        case ErrorCode::TrainingError: return "TRAINING_ERROR";
        case ErrorCode::InferenceError: return "INFERENCE_ERROR";
        case ErrorCode::SimulationError: return "SIMULATION_ERROR";
        case ErrorCode::IntegrationError: return "INTEGRATION_ERROR";
        case ErrorCode::TimeStepError: return "TIME_STEP_ERROR";
        case ErrorCode::SystemError: return "SYSTEM_ERROR";
        case ErrorCode::NotImplemented: return "NOT_IMPLEMENTED";
        case ErrorCode::UnsupportedFeature: return "UNSUPPORTED_FEATURE";
        case ErrorCode::UnknownError: return "UNKNOWN_ERROR";
        case ErrorCode::FatalError: return "FATAL_ERROR";
        default: return "UNKNOWN_CODE";
    }
}

std::string ErrorHandler::getErrorCategoryString(ErrorCategory category) const {
    switch (category) {
        case ErrorCategory::Validation: return "Validation";
        case ErrorCategory::Resource: return "Resource";
        case ErrorCategory::Network: return "Network";
        case ErrorCategory::NeuralNetwork: return "NeuralNetwork";
        case ErrorCategory::Simulation: return "Simulation";
        case ErrorCategory::System: return "System";
        case ErrorCategory::User: return "User";
        case ErrorCategory::External: return "External";
        default: return "Unknown";
    }
}

std::string ErrorHandler::getErrorSeverityString(ErrorSeverity severity) const {
    switch (severity) {
        case ErrorSeverity::Debug: return "Debug";
        case ErrorSeverity::Info: return "Info";
        case ErrorSeverity::Warning: return "Warning";
        case ErrorSeverity::Error: return "Error";
        case ErrorSeverity::Critical: return "Critical";
        case ErrorSeverity::Fatal: return "Fatal";
        default: return "Unknown";
    }
}

ErrorCategory ErrorHandler::getErrorCategoryFromCode(ErrorCode code) const {
    if (code >= ErrorCode::ValidationError && code <= ErrorCode::PermissionDenied) {
        return ErrorCategory::Validation;
    } else if (code >= ErrorCode::MemoryAllocation && code <= ErrorCode::OutOfMemory) {
        return ErrorCategory::Resource;
    } else if (code >= ErrorCode::FileReadError && code <= ErrorCode::DirectoryNotFound) {
        return ErrorCategory::Resource;
    } else if (code >= ErrorCode::ConnectionError && code <= ErrorCode::ProtocolError) {
        return ErrorCategory::Network;
    } else if (code >= ErrorCode::NeuralNetworkError && code <= ErrorCode::InferenceError) {
        return ErrorCategory::NeuralNetwork;
    } else if (code >= ErrorCode::SimulationError && code <= ErrorCode::TimeStepError) {
        return ErrorCategory::Simulation;
    } else if (code >= ErrorCode::SystemError && code <= ErrorCode::UnsupportedFeature) {
        return ErrorCategory::System;
    } else {
        return ErrorCategory::System;
    }
}

ErrorSeverity ErrorHandler::getErrorSeverityFromCode(ErrorCode code) const {
    if (code == ErrorCode::Success) return ErrorSeverity::Info;
    if (code >= ErrorCode::ValidationError && code <= ErrorCode::PermissionDenied) return ErrorSeverity::Warning;
    if (code >= ErrorCode::MemoryAllocation && code <= ErrorCode::OutOfMemory) return ErrorSeverity::Critical;
    if (code >= ErrorCode::FileReadError && code <= ErrorCode::DirectoryNotFound) return ErrorSeverity::Error;
    if (code >= ErrorCode::ConnectionError && code <= ErrorCode::ProtocolError) return ErrorSeverity::Error;
    if (code >= ErrorCode::NeuralNetworkError && code <= ErrorCode::InferenceError) return ErrorSeverity::Critical;
    if (code >= ErrorCode::SimulationError && code <= ErrorCode::TimeStepError) return ErrorSeverity::Error;
    if (code >= ErrorCode::SystemError && code <= ErrorCode::UnsupportedFeature) return ErrorSeverity::Fatal;
    if (code == ErrorCode::FatalError) return ErrorSeverity::Fatal;
    return ErrorSeverity::Error;
}

bool ErrorHandler::isRetryable(ErrorCode code) const {
    return std::find(retryConfig_.retryableErrors.begin(), retryConfig_.retryableErrors.end(), code) != retryConfig_.retryableErrors.end();
}

RecoveryResult ErrorHandler::executeRecoveryStrategy(const NLMBaseException& error, const std::string& operation) {
    ErrorCode code = error.getCode();
    
    auto it = recoveryStrategies_.find(code);
    if (it == recoveryStrategies_.end()) {
        return RecoveryResult::NeedsManualIntervention;
    }
    
    RecoveryStrategy strategy = it->second;
    
    switch (strategy) {
        case RecoveryStrategy::None:
            handleError(error);
            return RecoveryResult::Skipped;
            
        case RecoveryStrategy::Retry:
            return retry([error]() { throw error; });
            
        case RecoveryStrategy::Fallback:
            // In a real implementation, this would use fallback configurations
            handleError(error);
            return RecoveryResult::Success;
            
        case RecoveryStrategy::GracefulDegradation:
            // In a real implementation, this would degrade functionality
            handleError(error);
            return RecoveryResult::Success;
            
        case RecoveryStrategy::CheckpointRestore:
            if (saveCheckpoint("before_error")) {
                return RecoveryResult::Success;
            }
            return RecoveryResult::Failed;
            
        case RecoveryStrategy::AutoRecovery:
            if (config_.enableAutoRecovery) {
                handleError(error);
                return RecoveryResult::Success;
            }
            return RecoveryResult::Failed;
            
        default:
            handleError(error);
            return RecoveryResult::Failed;
    }
}

void ErrorHandler::logError(const ErrorEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (config_.enableStructuredLogging) {
        logStructuredError(entry);
    } else {
        // Simple logging
        std::string message = "[" + getErrorSeverityString(entry.severity) + "] " + entry.message;
        if (!entry.file.empty()) {
            message += " (" + entry.file + ":" + std::to_string(entry.line) + ")";
        }
        
        if (entry.severity == ErrorSeverity::Critical || entry.severity == ErrorSeverity::Fatal) {
            std::cerr << "CRITICAL: " << message << std::endl;
        } else if (entry.severity == ErrorSeverity::Error) {
            std::cerr << "ERROR: " << message << std::endl;
        } else {
            std::cout << message << std::endl;
        }
    }
    
    addErrorEntry(entry);
}

void ErrorHandler::logStructuredError(const ErrorEntry& entry) {
    json errorJson;
    errorJson["timestamp"] = std::chrono::system_clock::to_time_t(entry.timestamp);
    errorJson["level"] = static_cast<int>(entry.severity);
    errorJson["code"] = static_cast<int>(entry.code);
    errorJson["message"] = entry.message;
    errorJson["category"] = static_cast<int>(entry.category);
    errorJson["file"] = entry.file;
    errorJson["line"] = entry.line;
    errorJson["function"] = entry.function;
    errorJson["correlationId"] = entry.correlationId;
    errorJson["context"] = entry.context;
    
    if (config_.enableErrorCorrelation && !entry.correlationId.empty()) {
        errorJson["correlationId"] = entry.correlationId;
    }
    
    // In a real implementation, this would write to a structured log file
    // For now, we'll output to stdout
    std::cout << errorJson.dump() << std::endl;
}

void ErrorHandler::updateErrorStats(const ErrorEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ErrorStats& stats = errorStats_[entry.code];
    stats.code = entry.code;
    stats.message = entry.message;
    stats.category = entry.category;
    stats.severity = entry.severity;
    stats.count++;
    stats.lastOccurrence = entry.timestamp;
    
    if (stats.firstOccurrence > entry.timestamp) {
        stats.firstOccurrence = entry.timestamp;
    }
    
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stats.lastOccurrence - stats.firstOccurrence);
    stats.rate = duration.count() > 0 ? static_cast<double>(stats.count) / duration.count() : 0.0;
    
    stats.context = entry.context;
}

void ErrorHandler::checkAlertThreshold() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    double errorRate = getErrorRatePerMinute();
    
    if (errorRate >= config_.alertThreshold) {
        auto now = std::chrono::system_clock::now();
        if (now - lastAlertTime_ > std::chrono::minutes(5)) { // Throttle alerts
            std::cerr << "ALERT: Error rate threshold exceeded! Current rate: " << errorRate << std::endl;
            lastAlertTime_ = now;
        }
    }
}

void ErrorHandler::addErrorEntry(const ErrorEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    errorHistory_.push_back(entry);
    
    // Trim error history if it exceeds configured size
    if (errorHistory_.size() > config_.maxErrorsToStore) {
        errorHistory_.erase(errorHistory_.begin(), errorHistory_.begin() + 
                           (errorHistory_.size() - config_.maxErrorsToStore));
    }
}

std::string ErrorHandler::generateStackTrace() const {
    return "Stack trace not available in simplified version";
}

std::string ErrorHandler::generateCorrelationId() const {
    static const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, chars.size() - 1);
    
    std::string correlationId = "NLM-";
    for (int i = 0; i < 8; ++i) {
        correlationId += chars[dist(gen)];
    }
    correlationId += "-";
    for (int i = 0; i < 4; ++i) {
        correlationId += chars[dist(gen)];
    }
    
    return correlationId;
}

void ErrorHandler::setConfig(const Config& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
}

const ErrorHandler::Config& ErrorHandler::getConfig() const {
    return config_;
}

std::string ErrorHandler::analyzeRootCause(const ErrorEntry& error) const {
    return analyzeRootCause(error);
}

double ErrorHandler::calculateErrorRate(const std::chrono::system_clock::time_point& since) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    int errorCount = 0;
    for (const auto& error : errorHistory_) {
        if (error.timestamp >= since) {
            errorCount++;
        }
    }
    
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - since);
    return duration.count() > 0 ? static_cast<double>(errorCount) / duration.count() : 0.0;
}

size_t ErrorHandler::getTotalErrorCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return errorHistory_.size();
}

double ErrorHandler::getErrorRatePerMinute() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (errorHistory_.empty()) {
        return 0.0;
    }
    
    auto lastErrorTime = errorHistory_.back().timestamp;
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - lastErrorTime);
    
    return duration.count() > 0 ? static_cast<double>(errorHistory_.size()) / duration.count() : 0.0;
}

std::chrono::system_clock::time_point ErrorHandler::getLastErrorTime() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (errorHistory_.empty()) {
        return std::chrono::system_clock::time_point::min();
    }
    
    return errorHistory_.back().timestamp;
}

} // namespace nlm
