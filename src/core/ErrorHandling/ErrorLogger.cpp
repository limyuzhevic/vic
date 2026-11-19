// ErrorLogger implementation

#include "ErrorLogger.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <fstream>

namespace nlm {

struct ErrorLogger::Impl {
    ErrorSeverity minimumSeverity = ErrorSeverity::Info;
    std::vector<NLMError> errorHistory;
    mutable std::mutex mutex;
    std::function<bool(const NLMError&)> recoveryCallback;
    size_t maxErrorHistory = 1000;
    ErrorStats stats;
    std::chrono::system_clock::time_point startTime;
};

ErrorLogger::ErrorLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->startTime = std::chrono::system_clock::now();
}

ErrorLogger::~ErrorLogger() = default;

void ErrorLogger::logError(const NLMError& error) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check if error meets minimum severity requirement
    if (static_cast<int>(error.severity) < static_cast<int>(pImpl->minimumSeverity)) {
        return;
    }
    
    // Add to error history
    pImpl->errorHistory.push_back(error);
    
    // Trim error history if it gets too large
    if (pImpl->errorHistory.size() > pImpl->maxErrorHistory) {
        pImpl->errorHistory.erase(
            pImpl->errorHistory.begin(),
            pImpl->errorHistory.begin() + 
            (pImpl->errorHistory.size() - pImpl->maxErrorHistory)
        );
    }
    
    // Update statistics
    pImpl->stats.totalErrors++;
    if (error.severity >= ErrorSeverity::Error) {
        pImpl->stats.criticalErrors++;
    }
    
    auto now = std::chrono::system_clock::now();
    if (pImpl->stats.totalErrors == 1) {
        pImpl->stats.firstError = now;
    }
    pImpl->stats.lastError = now;
    
    // Output error information
    std::ostringstream oss;
    oss << "[" << static_cast<int>(error.severity) << "] ";
    
    // Add timestamp
    auto time = std::chrono::system_clock::to_time_t(error.timestamp);
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    oss << " " << error.message;
    
    if (!error.context.empty()) {
        oss << " (Context: " << error.context << ")";
    }
    
    std::cout << oss.str() << std::endl;
    
    // Attempt recovery if callback is set
    if (pImpl->recoveryCallback) {
        try {
            if (pImpl->recoveryCallback(error)) {
                pImpl->stats.recoveredErrors++;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error during error recovery: " << e.what() << std::endl;
        }
    }
}

void ErrorLogger::logError(NLMErrorType type, ErrorSeverity severity,
                          const std::string& message,
                          const std::string& file, int line,
                          const std::string& function,
                          const std::string& context) {
    NLMError error(type, severity, message, file, line, function);
    error.context = context;
    logError(error);
}

void ErrorLogger::setMinimumSeverity(ErrorSeverity severity) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->minimumSeverity = severity;
}

ErrorSeverity ErrorLogger::getMinimumSeverity() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->minimumSeverity;
}

std::vector<NLMError> ErrorLogger::getRecentErrors(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    size_t startIndex = 0;
    if (pImpl->errorHistory.size() > limit) {
        startIndex = pImpl->errorHistory.size() - limit;
    }
    
    std::vector<NLMError> result;
    result.reserve(limit);
    
    for (size_t i = startIndex; i < pImpl->errorHistory.size(); ++i) {
        result.push_back(pImpl->errorHistory[i]);
    }
    
    return result;
}

void ErrorLogger::clearErrors() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorHistory.clear();
    pImpl->stats = ErrorStats{};
    pImpl->startTime = std::chrono::system_clock::now();
}

bool ErrorLogger::canRecover(const NLMError& error) const {
    return error_recovery::isRecoverable(error.type, error.context);
}

bool ErrorLogger::recover(const NLMError& error) {
    if (!canRecover(error)) {
        return false;
    }
    
    return error_recovery::attemptRecovery(error);
}

void ErrorLogger::setRecoveryCallback(const std::function<bool(const NLMError&)>& callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->recoveryCallback = callback;
}

ErrorLogger::ErrorStats ErrorLogger::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

std::string ErrorLogger::generateErrorReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== NLM Error Report ===\n";
    oss << "Generated: " << std::put_time(
        std::localtime(&std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now())), "%Y-%m-%d %H:%M:%S")
    << "\n";
    
    oss << "Total Errors: " << pImpl->stats.totalErrors << "\n";
    oss << "Critical Errors: " << pImpl->stats.criticalErrors << "\n";
    oss << "Recovered Errors: " << pImpl->stats.recoveredErrors << "\n";
    
    if (pImpl->stats.totalErrors > 0) {
        oss << "\nError Distribution:\n";
        std::map<ErrorSeverity, size_t> severityCounts;
        for (const auto& err : pImpl->errorHistory) {
            severityCounts[err.severity]++;
        }
        
        for (const auto& pair : severityCounts) {
            oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
        }
    }
    
    return oss.str();
}

std::string ErrorLogger::generateHealthReport() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "=== NLM System Health Report ===\n";
    oss << "Generated: " << std::put_time(
        std::localtime(&std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now())), "%Y-%m-%d %H:%M:%S")
    << "\n";
    
    // Calculate uptime
    auto now = std::chrono::system_clock::now();
    auto uptime = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->startTime).count();
    
    oss << "Uptime: " << uptime << " seconds\n";
    oss << "Total Errors: " << pImpl->stats.totalErrors << "\n";
    
    if (pImpl->stats.totalErrors > 0) {
        oss << "Health Status: UNHEALTHY\n";
        oss << "Issues:\n";
        
        // Add error-related issues
        for (const auto& err : pImpl->errorHistory) {
            if (err.severity >= ErrorSeverity::Error) {
                oss << "  - " << err.message << " (" << static_cast<int>(err.type) << ")\n";
            }
        }
    } else {
        oss << "Health Status: HEALTHY\n";
    }
    
    return oss.str();
}

} // namespace nlm
