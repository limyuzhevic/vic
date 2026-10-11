#include "Error.hpp"
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <iomanip>
#include <ctime>
#include <unordered_map>
#include <sstream>

namespace nlm {

struct ConsoleErrorReporter::Impl {
    ErrorSeverity threshold = ErrorSeverity::Warning;
    std::mutex mutex;
    std::unordered_map<ErrorCategory, std::string> categoryNames;
    std::unordered_map<ErrorSeverity, std::string> severityNames;
    std::unordered_map<ErrorCategory, std::string> errorCodes;
    
    Impl() {
        // Initialize category names
        categoryNames[ErrorCategory::Configuration] = "CONFIG";
        categoryNames[ErrorCategory::FileIO] = "FILEIO";
        categoryNames[ErrorCategory::Validation] = "VALID";
        categoryNames[ErrorCategory::Network] = "NETWORK";
        categoryNames[ErrorCategory::Memory] = "MEMORY";
        categoryNames[ErrorCategory::Simulation] = "SIM";
        categoryNames[ErrorCategory::Logger] = "LOGGER";
        categoryNames[ErrorCategory::Random] = "RANDOM";
        categoryNames[ErrorCategory::System] = "SYSTEM";
        
        // Initialize severity names
        severityNames[ErrorSeverity::Debug] = "DEBUG";
        severityNames[ErrorSeverity::Info] = "INFO";
        severityNames[ErrorSeverity::Warning] = "WARN";
        severityNames[ErrorSeverity::Error] = "ERROR";
        severityNames[ErrorSeverity::Critical] = "CRIT";
        
        // Initialize error codes
        errorCodes[ErrorCategory::Configuration] = "CFG-";
        errorCodes[ErrorCategory::FileIO] = "IO-";
        errorCodes[ErrorCategory::Validation] = "VAL-";
        errorCodes[ErrorCategory::Network] = "NET-";
        errorCodes[ErrorCategory::Memory] = "MEM-";
        errorCodes[ErrorCategory::Simulation] = "SIM-";
        errorCodes[ErrorCategory::Logger] = "LOG-";
        errorCodes[ErrorCategory::Random] = "RNG-";
        errorCodes[ErrorCategory::System] = "SYS-";
    }
};

ConsoleErrorReporter::ConsoleErrorReporter() : pImpl(std::make_unique<Impl>()) {}
ConsoleErrorReporter::~ConsoleErrorReporter() = default;

void ConsoleErrorReporter::reportError(const NLMException& error) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string timestamp = getTimestampString(error.getTimestamp());
    std::string category = pImpl->categoryNames[error.getCategory()];
    std::string severity = pImpl->severityNames[error.getSeverity()];
    std::string code = pImpl->errorCodes[error.getCategory()];
    
    std::cerr << "[" << timestamp << "] [" << severity << "] [" << code << ""
              << error.what() << std::endl;
}

void ConsoleErrorReporter::reportError(const std::string& message,
                                      ErrorCategory category,
                                      ErrorSeverity severity) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (severity < pImpl->threshold && severity != ErrorSeverity::Critical) {
        return;
    }
    
    std::string timestamp = getCurrentTimestampString();
    std::string categoryStr = pImpl->categoryNames[category];
    std::string severityStr = pImpl->severityNames[severity];
    std::string code = pImpl->errorCodes[category];
    
    std::cerr << "[" << timestamp << "] [" << severityStr << "] [" << code << "] "
              << message << std::endl;
}

void ConsoleErrorReporter::setSeverityThreshold(ErrorSeverity threshold) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->threshold = threshold;
}

ErrorSeverity ConsoleErrorReporter::getSeverityThreshold() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->threshold;
}

std::string ConsoleErrorReporter::categoryToString(ErrorCategory category) const {
    auto it = pImpl->categoryNames.find(category);
    return it != pImpl->categoryNames.end() ? it->second : "UNKNOWN";
}

std::string ConsoleErrorReporter::severityToString(ErrorSeverity severity) const {
    auto it = pImpl->severityNames.find(severity);
    return it != pImpl->severityNames.end() ? it->second : "UNKNOWN";
}

std::string ConsoleErrorReporter::errorCodeToString(ErrorCategory category) const {
    auto it = pImpl->errorCodes.find(category);
    return it != pImpl->errorCodes.end() ? it->second : "UNK-";
}

// Global error handling functions
namespace ErrorHandling {
    static std::shared_ptr<IErrorReporter> globalReporter;
    static std::mutex reporterMutex;
    
    void setGlobalErrorReporter(std::shared_ptr<IErrorReporter> reporter) {
        std::lock_guard<std::mutex> lock(reporterMutex);
        globalReporter = reporter;
    }
    
    std::shared_ptr<IErrorReporter> getGlobalErrorReporter() {
        std::lock_guard<std::mutex> lock(reporterMutex);
        return globalReporter;
    }
    
    void reportError(const std::string& message,
                    const ErrorContext& context,
                    ErrorCategory category,
                    ErrorSeverity severity) {
        std::shared_ptr<IErrorReporter> reporter;
        {
            std::lock_guard<std::mutex> lock(reporterMutex);
            reporter = globalReporter;
        }
        
        if (reporter && ErrorHandling::shouldReport(severity)) {
            reporter->reportError(message, category, severity);
        }
    }
    
    void reportError(const NLMException& error,
                    const ErrorContext& context) {
        std::shared_ptr<IErrorReporter> reporter;
        {
            std::lock_guard<std::mutex> lock(reporterMutex);
            reporter = globalReporter;
        }
        
        if (reporter) {
            reporter->reportError(error);
        }
    }
    
    bool shouldReport(ErrorSeverity severity) {
        std::shared_ptr<IErrorReporter> reporter;
        {
            std::lock_guard<std::mutex> lock(reporterMutex);
            reporter = globalReporter;
        }
        
        if (!reporter) return false;
        
        ErrorSeverity threshold = reporter->getSeverityThreshold();
        return severity >= threshold;
    }
    
    ErrorContext createContext(const std::string& component) {
        ErrorContext context;
        context.component = component;
        return context;
    }
} // namespace ErrorHandling

// Error recovery manager implementation
struct ErrorRecoveryManager::Impl {
    std::unordered_map<ErrorCategory, ErrorRecoveryCallback> recoveryCallbacks;
    std::mutex mutex;
};

ErrorRecoveryManager::ErrorRecoveryManager() : pImpl(std::make_unique<Impl>()) {}
ErrorRecoveryManager::~ErrorRecoveryManager() = default;

void ErrorRecoveryManager::registerRecoveryCallback(ErrorCategory category,
                                                   ErrorRecoveryCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->recoveryCallbacks[category] = std::move(callback);
}

bool ErrorRecoveryManager::attemptRecovery(const NLMException& error) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->recoveryCallbacks.find(error.getCategory());
    if (it != pImpl->recoveryCallbacks.end() && it->second) {
        return it->second(error);
    }
    return false;
}

void ErrorRecoveryManager::clearRecoveryCallbacks(ErrorCategory category) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->recoveryCallbacks.erase(category);
}

bool ErrorRecoveryManager::hasRecovery(ErrorCategory category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->recoveryCallbacks.find(category) != pImpl->recoveryCallbacks.end();
}

// Global error recovery manager instance
ErrorRecoveryManager globalErrorRecoveryManager;

// Helper functions for timestamp formatting
std::string getCurrentTimestampString() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string getTimestampString(std::chrono::system_clock::time_point timestamp) {
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

} // namespace nlm
