#pragma once

#include "ErrorHandling.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <algorithm>

namespace nlm {

struct ErrorLogger::Impl {
    std::vector<NLMException> errors;
    std::vector<std::pair<std::chrono::system_clock::time_point, NLMException>> errorHistory;
    ErrorStats stats;
    std::mutex mutex;
    ErrorCallback errorCallback;
    size_t maxHistorySize = 1000;
    std::ofstream logFile;
    bool logToFile = false;
    bool throwOnError = false;
    bool continueOnError = false;
};

ErrorLogger::ErrorLogger() : pImpl(std::make_unique<Impl>()) {
    // Set up file logging if NLM_LOG_FILE environment variable is set
    const char* logFileEnv = std::getenv("NLM_LOG_FILE");
    if (logFileEnv) {
        pImpl->logFile.open(logFileEnv, std::ios::app);
        pImpl->logToFile = true;
        if (!pImpl->logFile.is_open()) {
            std::cerr << "Warning: Could not open log file: " << logFileEnv << std::endl;
        }
    }
}

ErrorLogger::~ErrorLogger() {
    if (pImpl->logFile.is_open()) {
        pImpl->logFile.close();
    }
}

void ErrorLogger::logException(const NLMException& ex, bool shouldThrow) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Record the error
    pImpl->errors.push_back(ex);
    pImpl->errorHistory.push_back(std::make_pair(std::chrono::system_clock::now(), ex));
    
    // Update statistics
    pImpl->stats.totalErrors++;
    pImpl->stats.errorsByCategory[static_cast<size_t>(ex.getCategory())]++;
    
    auto now = std::chrono::system_clock::now();
    if (pImpl->stats.firstError.time_since_epoch().count() == 0) {
        pImpl->stats.firstError = now;
    }
    pImpl->stats.lastError = now;
    
    // Log to console
    std::cout << "[NLM ERROR] " << ex.getFullMessage() << std::endl;
    
    // Log to file if enabled
    if (pImpl->logToFile) {
        pImpl->logFile << "[NLM ERROR] " << ex.getFullMessage() << std::endl;
        pImpl->logFile.flush();
    }
    
    // Call error callback if set
    if (pImpl->errorCallback) {
        pImpl->errorCallback(ex);
    }
    
    // Trim history if too large
    if (pImpl->errorHistory.size() > pImpl->maxHistorySize) {
        auto start = pImpl->errorHistory.begin() + (pImpl->errorHistory.size() - pImpl->maxHistorySize / 2);
        pImpl->errorHistory.erase(pImpl->errorHistory.begin(), start);
    }
    
    if (shouldThrow) {
        throw ex;
    }
}

void ErrorLogger::logError(const std::string& message,
                          ErrorCategory category,
                          const std::string& context,
                          const std::string& file,
                          int line,
                          const std::string& function) {
    std::string fullMessage = message;
    if (!file.empty() && line > 0) {
        fullMessage += " (" + file + ":" + std::to_string(line) + ")";
    }
    if (!function.empty()) {
        fullMessage += " in " + function;
    }
    
    NLMException ex(fullMessage, category, context);
    logException(ex, pImpl->throwOnError);
}

ErrorLogger::ErrorStats ErrorLogger::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

void ErrorLogger::clearStats() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats = ErrorStats();
    pImpl->errors.clear();
    pImpl->errorHistory.clear();
}

std::vector<NLMException> ErrorLogger::getRecentErrors(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<NLMException> recentErrors;
    size_t count = std::min(limit, pImpl->errorHistory.size());
    
    for (auto it = pImpl->errorHistory.rbegin(); 
         it != pImpl->errorHistory.rend() && recentErrors.size() < count; 
         ++it) {
        recentErrors.push_back(it->second);
    }
    
    return recentErrors;
}

bool ErrorLogger::hasRecentErrors(std::chrono::seconds threshold) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto now = std::chrono::system_clock::now();
    auto thresholdTime = now - threshold;
    
    for (const auto& pair : pImpl->errorHistory) {
        if (pair.first > thresholdTime) {
            return true;
        }
    }
    
    return false;
}

void ErrorLogger::setErrorCallback(ErrorCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorCallback = std::move(callback);
}

// Global error logger implementation
namespace {
    std::unique_ptr<ErrorLogger> globalErrorLogger;
    std::mutex globalErrorLoggerMutex;
}

ErrorLogger& getGlobalErrorLogger() {
    std::lock_guard<std::mutex> lock(globalErrorLoggerMutex);
    if (!globalErrorLogger) {
        globalErrorLogger = std::make_unique<ErrorLogger>();
    }
    return *globalErrorLogger;
}

} // namespace nlm
