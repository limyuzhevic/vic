#include "ErrorCodes.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <thread>
#include <algorithm>

namespace nlm {

struct ErrorReporter::Impl {
    ErrorCode level;
    bool enabled;
    std::mutex mutex;
    std::ofstream logFile;
    std::string logFilePath;
    
    // Error statistics
    std::unordered_map<ErrorCode, size_t> errorCounts;
    std::chrono::system_clock::time_point lastErrorTime;
    size_t totalErrorCount;
    
    // Error callbacks
    ErrorCallback errorCallback;
    
    // Previous errors for rate calculation
    std::vector<ErrorDetails> recentErrors;
    
    // Severity levels for error filtering
    enum class SeverityLevel {
        None = 0,
        Warning = 1,
        Error = 2,
        Critical = 3
    };
    
    SeverityLevel getSeverityFromCode(ErrorCode code) const {
        if (code >= ErrorCode::Success && code <= ErrorCode::OutOfRange) {
            return SeverityLevel::Warning;
        } else if (code >= ErrorCode::NotFound && code <= ErrorCode::PermissionDenied) {
            return SeverityLevel::Warning;
        } else if (code >= ErrorCode::ConfigFileNotFound && code <= ErrorCode::ConfigValidationFailed) {
            return SeverityLevel::Error;
        } else if (code >= ErrorCode::MemoryAllocationFailed && code <= ErrorCode::OutOfMemory) {
            return SeverityLevel::Critical;
        } else if (code >= ErrorCode::NeuronNotFound && code <= ErrorCode::NetworkSynchronizationFailed) {
            return SeverityLevel::Critical;
        } else if (code >= ErrorCode::LoggerNotInitialized && code <= ErrorCode::LoggerAlreadyInitialized) {
            return SeverityLevel::Error;
        } else if (code >= ErrorCode::ProtocolError && code <= ErrorCode::NetworkTimeout) {
            return SeverityLevel::Critical;
        } else if (code >= ErrorCode::OperationAborted && code <= ErrorCode::RecoveryFailed) {
            return SeverityLevel::Critical;
        }
        return SeverityLevel::Error;
    }
};

ErrorReporter::ErrorReporter() : pImpl(std::make_unique<Impl>()) {
    pImpl->level = ErrorCode::Warning;
    pImpl->enabled = true;
    pImpl->totalErrorCount = 0;
    pImpl->lastErrorTime = std::chrono::system_clock::now();
    
    // Try to open default error log file
    std::string home = std::getenv("HOME") ? std::getenv("HOME") : ".";
    pImpl->logFilePath = home + "/nlm_errors.log";
    pImpl->logFile.open(pImpl->logFilePath, std::ios::app);
}

ErrorReporter::~ErrorReporter() {
    if (pImpl->logFile.is_open()) {
        pImpl->logFile.close();
    }
}

void ErrorReporter::report(const ErrorDetails& error) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->enabled) return;
    
    // Check if we should report this error code
    if (!shouldReport(error.code)) return;
    
    // Update statistics
    pImpl->errorCounts[error.code]++;
    pImpl->totalErrorCount++;
    pImpl->lastErrorTime = std::chrono::system_clock::now();
    
    // Add to recent errors for rate calculation
    pImpl->recentErrors.push_back(error);
    if (pImpl->recentErrors.size() > 1000) {
        pImpl->recentErrors.erase(pImpl->recentErrors.begin());
    }
    
    // Log to file
    if (pImpl->logFile.is_open()) {
        pImpl->logFile << error.toString() << "\n";
        pImpl->logFile.flush();
    }
    
    // Also output to stderr for immediate visibility
    std::cerr << "ERROR: " << error.toString() << std::endl;
    
    // Call callback if registered
    if (pImpl->errorCallback) {
        try {
            pImpl->errorCallback(error);
        } catch (const std::exception& e) {
            std::cerr << "Error in error callback: " << e.what() << std::endl;
        }
    }
}

void ErrorReporter::setErrorLevel(ErrorCode level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->level = level;
}

bool ErrorReporter::shouldReport(ErrorCode code) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->enabled) return false;
    
    // Convert error code to severity
    Impl::SeverityLevel errorSeverity = pImpl->getSeverityFromCode(code);
    Impl::SeverityLevel levelSeverity = pImpl->getSeverityFromCode(pImpl->level);
    
    return static_cast<int>(errorSeverity) >= static_cast<int>(levelSeverity);
}

size_t ErrorReporter::getErrorCount(ErrorCode code) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->errorCounts.find(code);
    return it ? it->second : 0;
}

size_t ErrorReporter::getTotalErrorCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->totalErrorCount;
}

double ErrorReporter::getErrorRate(ErrorCode code) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->recentErrors.empty()) return 0.0;
    
    size_t count = getErrorCount(code);
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
        now - pImpl->lastErrorTime).count();
    
    if (duration <= 0) return static_cast<double>(count);
    
    return static_cast<double>(count) / static_cast<double>(duration);
}

void ErrorReporter::clearStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorCounts.clear();
    pImpl->totalErrorCount = 0;
    pImpl->recentErrors.clear();
}

void ErrorReporter::setErrorCallback(const ErrorCallback& callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorCallback = callback;
}

void ErrorReporter::setEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->enabled = enabled;
}

bool ErrorReporter::isEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->enabled;
}

void ErrorReporter::lock() const {
    pImpl->mutex.lock();
}

void ErrorReporter::unlock() const {
    pImpl->mutex.unlock();
}

ErrorReporter& ErrorReporter::getInstance() {
    static ErrorReporter instance;
    return instance;
}

NLMException::NLMException(const ErrorDetails& details) 
    : std::runtime_error(details.toString()), details_(details) {
}

ErrorDetails NLMException::fromException(const std::exception& e) {
    ErrorDetails details(ErrorCode::UnknownError, e.what());
    return details;
}

} // namespace nlm
