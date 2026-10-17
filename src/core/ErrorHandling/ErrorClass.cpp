#include "ErrorClass.hpp"
#include "Logger/Logger.hpp"
#include <thread>
#include <algorithm>
#include <map>
#include <set>

namespace nlm {

// ErrorClassifier implementation
std::map<int, ErrorCategory> ErrorClassifier::errorCodeToCategoryMap;
std::map<int, ErrorSeverity> ErrorClassifier::errorCodeToSeverityMap;
std::map<int, std::string> ErrorClassifier::errorCodeToDescriptionMap;
std::map<int, std::string> ErrorClassifier::errorCodeToMessageMap;

ErrorCategory ErrorClassifier::classifyError(int code) {
    initializeErrorMaps();
    auto it = errorCodeToCategoryMap.find(code);
    return it != errorCodeToCategoryMap.end() ? it->second : ErrorCategory::System;
}

ErrorSeverity ErrorClassifier::classifySeverity(int code, const std::string& message) {
    initializeErrorMaps();
    auto it = errorCodeToSeverityMap.find(code);
    if (it != errorCodeToSeverityMap.end()) {
        return it->second;
    }
    
    // Determine severity based on message content if code not found
    std::string lowerMessage = message;
    std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    if (lowerMessage.find("critical") != std::string::npos ||
        lowerMessage.find("fatal") != std::string::npos) {
        return ErrorSeverity::Fatal;
    } else if (lowerMessage.find("error") != std::string::npos) {
        return ErrorSeverity::Error;
    } else if (lowerMessage.find("warning") != std::string::npos) {
        return ErrorSeverity::Warning;
    } else if (lowerMessage.find("debug") != std::string::npos) {
        return ErrorSeverity::Debug;
    } else if (lowerMessage.find("info") != std::string::npos) {
        return ErrorSeverity::Info;
    }
    
    return ErrorSeverity::Error;
}

std::string ErrorClassifier::getErrorCategoryString(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::Neural: return "NEURAL";
        case ErrorCategory::Configuration: return "CONFIGURATION";
        case ErrorCategory::I_O: return "I/O";
        case ErrorCategory::System: return "SYSTEM";
        case ErrorCategory::Memory: return "MEMORY";
        case ErrorCategory::Network: return "NETWORK";
        case ErrorCategory::User: return "USER";
        default: return "UNKNOWN";
    }
}

std::string ErrorClassifier::getErrorSeverityString(ErrorSeverity severity) {
    switch (severity) {
        case ErrorSeverity::Trace: return "TRACE";
        case ErrorSeverity::Debug: return "DEBUG";
        case ErrorSeverity::Info: return "INFO";
        case ErrorSeverity::Warning: return "WARNING";
        case ErrorSeverity::Error: return "ERROR";
        case ErrorSeverity::Critical: return "CRITICAL";
        case ErrorSeverity::Fatal: return "FATAL";
        default: return "UNKNOWN";
    }
}

std::string ErrorClassifier::getErrorCodeString(int code) {
    initializeErrorMaps();
    auto it = errorCodeToMessageMap.find(code);
    return it != errorCodeToMessageMap.end() ? it->second : "UNKNOWN_ERROR_" + std::to_string(code);
}

void ErrorClassifier::initializeErrorMaps() {
    if (errorCodeToCategoryMap.empty()) {
        loadNeuralErrorMappings();
        loadConfigErrorMappings();
        loadIOErrorMappings();
        loadSystemErrorMappings();
        loadMemoryErrorMappings();
        loadNetworkErrorMappings();
        loadUserErrorMappings();
    }
}

void ErrorClassifier::loadNeuralErrorMappings() {
    errorCodeToCategoryMap[ErrorCodes::NEURAL_NETWORK_INIT_FAILED] = ErrorCategory::Neural;
    errorCodeToCategoryMap[ErrorCodes::NEURAL_INVALID_INPUT] = ErrorCategory::Neural;
    errorCodeToCategoryMap[ErrorCodes::NEURAL_INVALID_OUTPUT] = ErrorCategory::Neural;
    errorCodeToCategoryMap[ErrorCodes::NEURAL_CONVERGENCE_FAILED] = ErrorCategory::Neural;
    
    errorCodeToSeverityMap[ErrorCodes::NEURAL_NETWORK_INIT_FAILED] = ErrorSeverity::Critical;
    errorCodeToSeverityMap[ErrorCodes::NEURAL_INVALID_INPUT] = ErrorSeverity::Warning;
    errorCodeToSeverityMap[ErrorCodes::NEURAL_INVALID_OUTPUT] = ErrorSeverity::Warning;
    errorCodeToSeverityMap[ErrorCodes::NEURAL_CONVERGENCE_FAILED] = ErrorSeverity::Critical;
    
    errorCodeToMessageMap[ErrorCodes::NEURAL_NETWORK_INIT_FAILED] = "Neural network initialization failed";
    errorCodeToMessageMap[ErrorCodes::NEURAL_INVALID_INPUT] = "Invalid neural network input";
    errorCodeToMessageMap[ErrorCodes::NEURAL_INVALID_OUTPUT] = "Invalid neural network output";
    errorCodeToMessageMap[ErrorCodes::NEURAL_CONVERGENCE_FAILED] = "Neural network convergence failed";
    
    errorCodeToDescriptionMap[ErrorCodes::NEURAL_NETWORK_INIT_FAILED] = "Failed to initialize neural network architecture";
    errorCodeToDescriptionMap[ErrorCodes::NEURAL_INVALID_INPUT] = "Input data format or values are invalid for neural network";
    errorCodeToDescriptionMap[ErrorCodes::NEURAL_INVALID_OUTPUT] = "Output data format or values are invalid for neural network";
    errorCodeToDescriptionMap[ErrorCodes::NEURAL_CONVERGENCE_FAILED] = "Neural network failed to converge during training";
}

void ErrorClassifier::loadConfigErrorMappings() {
    errorCodeToCategoryMap[ErrorCodes::CONFIG_FILE_NOT_FOUND] = ErrorCategory::Configuration;
    errorCodeToCategoryMap[ErrorCodes::CONFIG_INVALID_FORMAT] = ErrorCategory::Configuration;
    errorCodeToCategoryMap[ErrorCodes::CONFIG_VALUE_INVALID] = ErrorCategory::Configuration;
    errorCodeToCategoryMap[ErrorCodes::CONFIG_SCHEMA_MISMATCH] = ErrorCategory::Configuration;
    
    errorCodeToSeverityMap[ErrorCodes::CONFIG_FILE_NOT_FOUND] = ErrorSeverity::Warning;
    errorCodeToSeverityMap[ErrorCodes::CONFIG_INVALID_FORMAT] = ErrorSeverity::Error;
    errorCodeToSeverityMap[ErrorCodes::CONFIG_VALUE_INVALID] = ErrorSeverity::Warning;
    errorCodeToSeverityMap[ErrorCodes::CONFIG_SCHEMA_MISMATCH] = ErrorSeverity::Error;
    
    errorCodeToMessageMap[ErrorCodes::CONFIG_FILE_NOT_FOUND] = "Configuration file not found";
    errorCodeToMessageMap[ErrorCodes::CONFIG_INVALID_FORMAT] = "Configuration file format is invalid";
    errorCodeToMessageMap[ErrorCodes::CONFIG_VALUE_INVALID] = "Configuration value is invalid";
    errorCodeToMessageMap[ErrorCodes::CONFIG_SCHEMA_MISMATCH] = "Configuration schema mismatch";
    
    errorCodeToDescriptionMap[ErrorCodes::CONFIG_FILE_NOT_FOUND] = "Configuration file cannot be found or accessed";
    errorCodeToDescriptionMap[ErrorCodes::CONFIG_INVALID_FORMAT] = "Configuration file format does not match expected schema";
    errorCodeToDescriptionMap[ErrorCodes::CONFIG_VALUE_INVALID] = "Configuration value is out of range or has invalid type";
    errorCodeToDescriptionMap[ErrorCodes::CONFIG_SCHEMA_MISMATCH] = "Configuration file structure does not match expected schema";
}

void ErrorClassifier::loadIOErrorMappings() {
    errorCodeToCategoryMap[ErrorCodes::IO_FILE_OPEN_FAILED] = ErrorCategory::I_O;
    errorCodeToCategoryMap[ErrorCodes::IO_FILE_READ_FAILED] = ErrorCategory::I_O;
    errorCodeToCategoryMap[ErrorCodes::IO_FILE_WRITE_FAILED] = ErrorCategory::I_O;
    errorCodeToCategoryMap[ErrorCodes::IO_PATH_NOT_FOUND] = ErrorCategory::I_O;
    
    errorCodeToSeverityMap[ErrorCodes::IO_FILE_OPEN_FAILED] = ErrorSeverity::Warning;
    errorCodeToSeverityMap[ErrorCodes::IO_FILE_READ_FAILED] = ErrorSeverity::Error;
    errorCodeToSeverityMap[ErrorCodes::IO_FILE_WRITE_FAILED] = ErrorSeverity::Error;
    errorCodeToSeverityMap[ErrorCodes::IO_PATH_NOT_FOUND] = ErrorSeverity::Warning;
    
    errorCodeToMessageMap[ErrorCodes::IO_FILE_OPEN_FAILED] = "Failed to open file";
    errorCodeToMessageMap[ErrorCodes::IO_FILE_READ_FAILED] = "Failed to read file";
    errorCodeToMessageMap[ErrorCodes::IO_FILE_WRITE_FAILED] = "Failed to write file";
    errorCodeToMessageMap[ErrorCodes::IO_PATH_NOT_FOUND] = "File path not found";
    
    errorCodeToDescriptionMap[ErrorCodes::IO_FILE_OPEN_FAILED] = "Cannot open file for reading or writing";
    errorCodeToDescriptionMap[ErrorCodes::IO_FILE_READ_FAILED] = "Cannot read from file";
    errorCodeToDescriptionMap[ErrorCodes::IO_FILE_WRITE_FAILED] = "Cannot write to file";
    errorCodeToDescriptionMap[ErrorCodes::IO_PATH_NOT_FOUND] = "File path does not exist";
}

void ErrorClassifier::loadSystemErrorMappings() {
    errorCodeToCategoryMap[ErrorCodes::SYSTEM_MEMORY_EXHAUSTED] = ErrorCategory::System;
    errorCodeToCategoryMap[ErrorCodes::SYSTEM_CPU_LIMIT_EXCEEDED] = ErrorCategory::System;
    errorCodeToCategoryMap[ErrorCodes::SYSTEM_TIMEOUT] = ErrorCategory::System;
    errorCodeToCategoryMap[ErrorCodes::SYSTEM_PERMISSION_DENIED] = ErrorCategory::System;
    
    errorCodeToSeverityMap[ErrorCodes::SYSTEM_MEMORY_EXHAUSTED] = ErrorSeverity::Critical;
    errorCodeToSeverityMap[ErrorCodes::SYSTEM_CPU_LIMIT_EXCEEDED] = ErrorSeverity::Critical;
    errorCodeToSeverityMap[ErrorCodes::SYSTEM_TIMEOUT] = ErrorSeverity::Error;
    errorCodeToSeverityMap[ErrorCodes::SYSTEM_PERMISSION_DENIED] = ErrorSeverity::Warning;
    
    errorCodeToMessageMap[ErrorCodes::SYSTEM_MEMORY_EXHAUSTED] = "System memory exhausted";
    errorCodeToMessageMap[ErrorCodes::SYSTEM_CPU_LIMIT_EXCEEDED] = "CPU usage limit exceeded";
    errorCodeToMessageMap[ErrorCodes::SYSTEM_TIMEOUT] = "System operation timed out";
    errorCodeToMessageMap[ErrorCodes::SYSTEM_PERMISSION_DENIED] = "Permission denied";
    
    errorCodeToDescriptionMap[ErrorCodes::SYSTEM_MEMORY_EXHAUSTED] = "System memory usage exceeded available resources";
    errorCodeToDescriptionMap[ErrorCodes::SYSTEM_CPU_LIMIT_EXCEEDED] = "CPU usage exceeded configured limits";
    errorCodeToDescriptionMap[ErrorCodes::SYSTEM_TIMEOUT] = "System operation exceeded time limit";
    errorCodeToDescriptionMap[ErrorCodes::SYSTEM_PERMISSION_DENIED] = "Insufficient permissions to perform operation";
}

void ErrorClassifier::loadMemoryErrorMappings() {
    errorCodeToCategoryMap[ErrorCodes::MEMORY_ALLOCATION_FAILED] = ErrorCategory::Memory;
    errorCodeToCategoryMap[ErrorCodes::MEMORY_ACCESS_VIOLATION] = ErrorCategory::Memory;
    errorCodeToCategoryMap[ErrorCodes::MEMORY_OVERFLOW] = ErrorCategory::Memory;
    errorCodeToCategoryMap[ErrorCodes::MEMORY_UNDERFLOW] = ErrorCategory::Memory;
    
    errorCodeToSeverityMap[ErrorCodes::MEMORY_ALLOCATION_FAILED] = ErrorSeverity::Critical;
    errorCodeToSeverityMap[ErrorCodes::MEMORY_ACCESS_VIOLATION] = ErrorSeverity::Fatal;
    errorCodeToSeverityMap[ErrorCodes::MEMORY_OVERFLOW] = ErrorSeverity::Fatal;
    errorCodeToSeverityMap[ErrorCodes::MEMORY_UNDERFLOW] = ErrorSeverity::Fatal;
    
    errorCodeToMessageMap[ErrorCodes::MEMORY_ALLOCATION_FAILED] = "Memory allocation failed";
    errorCodeToMessageMap[ErrorCodes::MEMORY_ACCESS_VIOLATION] = "Memory access violation";
    errorCodeToMessageMap[ErrorCodes::MEMORY_OVERFLOW] = "Memory overflow detected";
    errorCodeToMessageMap[ErrorCodes::MEMORY_UNDERFLOW] = "Memory underflow detected";
    
    errorCodeToDescriptionMap[ErrorCodes::MEMORY_ALLOCATION_FAILED] = "Failed to allocate required memory";
    errorCodeToDescriptionMap[ErrorCodes::MEMORY_ACCESS_VIOLATION] = "Invalid memory access detected";
    errorCodeToDescriptionMap[ErrorCodes::MEMORY_OVERFLOW] = "Memory buffer overflow";
    errorCodeToDescriptionMap[ErrorCodes::MEMORY_UNDERFLOW] = "Memory buffer underflow";
}

void ErrorClassifier::loadNetworkErrorMappings() {
    errorCodeToCategoryMap[ErrorCodes::NETWORK_CONNECTION_FAILED] = ErrorCategory::Network;
    errorCodeToCategoryMap[ErrorCodes::NETWORK_TIMEOUT] = ErrorCategory::Network;
    errorCodeToCategoryMap[ErrorCodes::NETWORK_PROTOCOL_ERROR] = ErrorCategory::Network;
    errorCodeToCategoryMap[ErrorCodes::NETWORK_PROTOCOL_UNSUPPORTED] = ErrorCategory::Network;
    
    errorCodeToSeverityMap[ErrorCodes::NETWORK_CONNECTION_FAILED] = ErrorSeverity::Error;
    errorCodeToSeverityMap[ErrorCodes::NETWORK_TIMEOUT] = ErrorSeverity::Warning;
    errorCodeToSeverityMap[ErrorCodes::NETWORK_PROTOCOL_ERROR] = ErrorSeverity::Error;
    errorCodeToSeverityMap[ErrorCodes::NETWORK_PROTOCOL_UNSUPPORTED] = ErrorSeverity::Error;
    
    errorCodeToMessageMap[ErrorCodes::NETWORK_CONNECTION_FAILED] = "Network connection failed";
    errorCodeToMessageMap[ErrorCodes::NETWORK_TIMEOUT] = "Network operation timed out";
    errorCodeToMessageMap[ErrorCodes::NETWORK_PROTOCOL_ERROR] = "Network protocol error";
    errorCodeToMessageMap[ErrorCodes::NETWORK_PROTOCOL_UNSUPPORTED] = "Network protocol not supported";
    
    errorCodeToDescriptionMap[ErrorCodes::NETWORK_CONNECTION_FAILED] = "Cannot establish network connection";
    errorCodeToDescriptionMap[ErrorCodes::NETWORK_TIMEOUT] = "Network operation timed out";
    errorCodeToDescriptionMap[ErrorCodes::NETWORK_PROTOCOL_ERROR] = "Network protocol error occurred";
    errorCodeToDescriptionMap[ErrorCodes::NETWORK_PROTOCOL_UNSUPPORTED] = "Network protocol not supported";
}

void ErrorClassifier::loadUserErrorMappings() {
    errorCodeToCategoryMap[ErrorCodes::USER_INPUT_INVALID] = ErrorCategory::User;
    errorCodeToCategoryMap[ErrorCodes::USER_CANCELLED] = ErrorCategory::User;
    errorCodeToCategoryMap[ErrorCodes::USER_ABORTED] = ErrorCategory::User;
    
    errorCodeToSeverityMap[ErrorCodes::USER_INPUT_INVALID] = ErrorSeverity::Warning;
    errorCodeToSeverityMap[ErrorCodes::USER_CANCELLED] = ErrorSeverity::Info;
    errorCodeToSeverityMap[ErrorCodes::USER_ABORTED] = ErrorSeverity::Info;
    
    errorCodeToMessageMap[ErrorCodes::USER_INPUT_INVALID] = "Invalid user input";
    errorCodeToMessageMap[ErrorCodes::USER_CANCELLED] = "User cancelled operation";
    errorCodeToMessageMap[ErrorCodes::USER_ABORTED] = "User aborted operation";
    
    errorCodeToDescriptionMap[ErrorCodes::USER_INPUT_INVALID] = "User provided invalid input";
    errorCodeToDescriptionMap[ErrorCodes::USER_CANCELLED] = "User cancelled the operation";
    errorCodeToDescriptionMap[ErrorCodes::USER_ABORTED] = "User aborted the operation";
}

// NLMException implementation
NLMException::NLMException(const ErrorInfo& errorInfo) : errorInfo_(errorInfo), whatMessage_() {
    std::ostringstream oss;
    oss << "[" << static_cast<int>(errorInfo_.severity) << "] "
        << "[" << static_cast<int>(errorInfo_.category) << "] "
        << "[" << errorInfo_.code << "] "
        << errorInfo_.message;
    whatMessage_ = oss.str();
}

const char* NLMException::what() const noexcept {
    return whatMessage_.c_str();
}

// ErrorClass implementation
struct ErrorClass::Impl {
    std::vector<ErrorInfo> errors;
    std::vector<std::shared_ptr<IErrorReporter>> reporters;
    std::mutex mutex;
    ErrorSeverity minimumSeverity = ErrorSeverity::Warning;
    size_t maxStoredErrors = 1000;
    std::chrono::system_clock::time_point firstErrorTime;
    std::chrono::system_clock::time_point lastErrorTime;
    bool initialized = false;
};

ErrorClass::ErrorClass() : pImpl(std::make_unique<Impl>()) {
    pImpl->initialized = true;
}

ErrorClass::~ErrorClass() = default;

ErrorClass::ErrorClass(ErrorClass&&) noexcept = default;
ErrorClass& ErrorClass::operator=(ErrorClass&&) noexcept = default;

ErrorClass& ErrorClass::getInstance() {
    static ErrorClass instance;
    return instance;
}

void ErrorClass::reportError(const ErrorInfo& error) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update timestamps
    if (!pImpl->initialized || pImpl->firstErrorTime.time_since_epoch().count() == 0) {
        pImpl->firstErrorTime = std::chrono::system_clock::now();
    }
    pImpl->lastErrorTime = std::chrono::system_clock::now();
    
    // Check severity filter
    if (!shouldReportError(error)) {
        return;
    }
    
    // Add error to storage
    addError(error);
    
    // Report to all error reporters
    for (auto& reporter : pImpl->reporters) {
        reporter->reportError(error);
    }
    
    // Log error using NLM logger
    std::string logMessage = "[" + ErrorClassifier::getErrorSeverityString(error.severity) + "] " +
                             "[" + ErrorClassifier::getErrorCategoryString(error.category) + "] " +
                             "[" + std::to_string(error.code) + "] " + error.message;
    
    if (error.severity >= ErrorSeverity::Critical) {
        NLM_LOG_CRITICAL(logMessage);
    } else if (error.severity >= ErrorSeverity::Error) {
        NLM_LOG_ERROR(logMessage);
    } else if (error.severity >= ErrorSeverity::Warning) {
        NLM_LOG_WARNING(logMessage);
    } else if (error.severity >= ErrorSeverity::Info) {
        NLM_LOG_INFO(logMessage);
    } else if (error.severity >= ErrorSeverity::Debug) {
        NLM_LOG_DEBUG(logMessage);
    }
}

void ErrorClass::reportException(const std::exception& ex, const ErrorContext& context) {
    std::string message = ex.what();
    
    // Try to extract error code from exception message
    int errorCode = 0;
    ErrorCategory category = ErrorCategory::System;
    ErrorSeverity severity = ErrorSeverity::Error;
    
    // Simple error code extraction (could be enhanced)
    if (message.find("code") != std::string::npos) {
        size_t codePos = message.find("code") + 4;
        size_t endPos = message.find(" ", codePos);
        if (endPos != std::string::npos) {
            errorCode = std::stoi(message.substr(codePos, endPos - codePos));
        }
    }
    
    ErrorInfo errorInfo(errorCode, category, severity, message, "", "", context);
    reportError(errorInfo);
}

std::vector<ErrorInfo> ErrorClass::getRecentErrors(size_t maxCount) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<ErrorInfo> result;
    size_t count = std::min(maxCount, pImpl->errors.size());
    
    if (count > 0) {
        auto start = pImpl->errors.end() - count;
        result.insert(result.end(), start, pImpl->errors.end());
    }
    
    return result;
}

std::vector<ErrorInfo> ErrorClass::getErrorsByCategory(ErrorCategory category, size_t maxCount) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<ErrorInfo> result;
    
    for (auto it = pImpl->errors.rbegin(); it != pImpl->errors.rend() && result.size() < maxCount; ++it) {
        if (it->category == category) {
            result.push_back(*it);
        }
    }
    
    return result;
}

std::vector<ErrorInfo> ErrorClass::getErrorsBySeverity(ErrorSeverity severity, size_t maxCount) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<ErrorInfo> result;
    
    for (auto it = pImpl->errors.rbegin(); it != pImpl->errors.rend() && result.size() < maxCount; ++it) {
        if (it->severity == severity) {
            result.push_back(*it);
        }
    }
    
    return result;
}

void ErrorClass::clearErrors() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errors.clear();
}

ErrorClass::ErrorStats ErrorClass::getErrorStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    ErrorStats stats;
    
    stats.totalErrors = pImpl->errors.size();
    stats.firstErrorTime = pImpl->firstErrorTime;
    stats.lastErrorTime = pImpl->lastErrorTime;
    
    for (const auto& error : pImpl->errors) {
        stats.errorsByCategory[error.category]++;
        stats.errorsBySeverity[error.severity]++;
        stats.errorsByCode[error.code]++;
        
        if (error.severity >= ErrorSeverity::Critical) {
            stats.criticalErrors++;
        }
        if (error.severity >= ErrorSeverity::Fatal) {
            stats.fatalErrors++;
        }
    }
    
    return stats;
}

void ErrorClass::addErrorReporter(std::shared_ptr<IErrorReporter> reporter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->reporters.push_back(reporter);
}

void ErrorClass::setMinimumSeverity(ErrorSeverity severity) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->minimumSeverity = severity;
}

void ErrorClass::setMaxStoredErrors(size_t maxCount) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->maxStoredErrors = maxCount;
}

void ErrorClass::addError(const ErrorInfo& error) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Add error to storage
    pImpl->errors.push_back(error);
    
    // Limit storage size
    while (pImpl->errors.size() > pImpl->maxStoredErrors) {
        pImpl->errors.erase(pImpl->errors.begin());
    }
}

bool ErrorClass::shouldReportError(const ErrorInfo& error) const {
    return error.severity >= pImpl->minimumSeverity;
}

} // namespace nlm