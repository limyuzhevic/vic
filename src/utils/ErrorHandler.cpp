#include "TimingProfiler.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>
#include <sys/resource.h>
#include <unistd.h>
#include <cmath>
#include <fstream>
#include <numeric>
#include <ctime>
#include <errno.h>

namespace nlm {

struct ErrorHandler::Impl {
    std::mutex mutex;
    ErrorCallback globalHandler;
    std::vector<ErrorInfo> errorHistory;
    Statistics stats;
    
    // Recovery functions by category
    std::unordered_map<ErrorCategory, std::function<bool()>> recoveryFunctions;
    
    // Error message registry
    std::unordered_map<int, std::string> errorMessages;
    
    // Context information
    std::unordered_map<std::string, std::string> context;
    
    Impl() {
        // Initialize default error messages
        errorMessages[0] = "Success";
        errorMessages[1] = "General error";
        errorMessages[2] = "File not found";
        errorMessages[3] = "Permission denied";
        errorMessages[4] = "Invalid argument";
        errorMessages[5] = "Out of memory";
        errorMessages[6] = "Not implemented";
        errorMessages[7] = "Network error";
        errorMessages[8] = "Timeout";
        errorMessages[9] = "Resource busy";
        errorMessages[10] = "Invalid format";
    }
};

ErrorHandler::ErrorHandler() : pImpl(std::make_unique<Impl>()) {}

ErrorHandler::~ErrorHandler() = default;

ErrorHandler::ErrorHandler(ErrorHandler&&) noexcept = default;

ErrorHandler& ErrorHandler::operator=(ErrorHandler&&) noexcept = default;

void ErrorHandler::setGlobalErrorHandler(const ErrorCallback& handler) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalHandler = handler;
}

ErrorHandler::ErrorCallback ErrorHandler::getGlobalErrorHandler() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->globalHandler;
}

void ErrorHandler::reportError(const ErrorInfo& info) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->errorHistory.push_back(info);
    pImpl->stats.totalErrors++;
    
    // Update category statistics
    if (info.category != ErrorCategory::None) {
        size_t categoryIndex = static_cast<size_t>(info.category);
        if (categoryIndex < 12) {
            pImpl->stats.errorsByCategory[categoryIndex]++;
        }
    }
    
    // Update severity statistics
    size_t severityIndex = static_cast<size_t>(info.severity);
    if (severityIndex < 5) {
        pImpl->stats.errorsBySeverity[severityIndex]++;
    }
    
    // Update timing statistics
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - info.timestamp);
    
    if (pImpl->stats.totalErrors > 0) {
        pImpl->stats.errorFrequency = 1.0 / duration.count();
    }
    
    pImpl->stats.lastErrorTime = duration.count();
    
    // Log the error
    std::string severityStr = "ERROR";
    switch (info.severity) {
        case Severity::Debug: severityStr = "DEBUG"; break;
        case Severity::Info: severityStr = "INFO"; break;
        case Severity::Warning: severityStr = "WARNING"; break;
        case Severity::Error: severityStr = "ERROR"; break;
        case Severity::Critical: severityStr = "CRITICAL"; break;
    }
    
    NLM_LOG_##severityStr("Error " << info.code << ": " << info.message << " (" << static_cast<int>(info.category) << ")");
    
    // Call global handler if set
    if (pImpl->globalHandler) {
        try {
            pImpl->globalHandler(info);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception in error handler: ") + e.what());
        }
    }
}

void ErrorHandler::reportException(const std::exception& e) {
    ErrorInfo info;
    info.code = 1;
    info.message = std::string("Exception: ") + e.what();
    info.category = ErrorCategory::System;
    info.severity = Severity::Error;
    info.timestamp = std::chrono::system_clock::now();
    
    reportError(info);
}

void ErrorHandler::reportSystemError(int code, const std::string& message,
                                      ErrorCategory category, Severity severity) {
    ErrorInfo info = createError(code, message, category, severity);
    reportError(info);
}

ErrorHandler::ErrorInfo ErrorHandler::createError(int code, const std::string& message,
                                                   ErrorCategory category, Severity severity) {
    ErrorInfo info;
    info.code = code;
    info.message = message;
    info.category = category;
    info.severity = severity;
    info.timestamp = std::chrono::system_clock::now();
    
    // Add stack trace if available
    info.stackTrace = getStackTrace();
    
    return info;
}

void ErrorHandler::addContext(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->context[key] = value;
}

ErrorHandler::Statistics ErrorHandler::getStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

std::vector<ErrorHandler::ErrorInfo> ErrorHandler::getRecentErrors(size_t maxErrors) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    size_t startIndex = 0;
    if (pImpl->errorHistory.size() > maxErrors) {
        startIndex = pImpl->errorHistory.size() - maxErrors;
    }
    
    return std::vector<ErrorInfo>(pImpl->errorHistory.begin() + startIndex, pImpl->errorHistory.end());
}

void ErrorHandler::clearHistory() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorHistory.clear();
    pImpl->stats.totalErrors = 0;
    std::fill(std::begin(pImpl->stats.errorsByCategory), std::end(pImpl->stats.errorsByCategory), 0);
    std::fill(std::begin(pImpl->stats.errorsBySeverity), std::end(pImpl->stats.errorsBySeverity), 0);
}

ErrorHandler::RecoveryInfo ErrorHandler::getRecoveryInfo(const ErrorInfo& info) {
    RecoveryInfo recoveryInfo;
    
    // Check if we have a recovery function for this category
    auto it = pImpl->recoveryFunctions.find(info.category);
    if (it != pImpl->recoveryFunctions.end() && it->second) {
        recoveryInfo.recoverable = true;
        recoveryInfo.recoveryFunction = it->second;
        recoveryInfo.action = "Recover using " + std::to_string(static_cast<int>(info.category)) + " handler";
        recoveryInfo.timeout = 30.0; // 30 seconds timeout
    } else {
        recoveryInfo.recoverable = false;
        recoveryInfo.action = "Manual intervention required";
        recoveryInfo.timeout = 0.0;
    }
    
    return recoveryInfo;
}

bool ErrorHandler::attemptRecovery(const ErrorInfo& info) {
    RecoveryInfo recoveryInfo = getRecoveryInfo(info);
    
    if (!recoveryInfo.recoverable || !recoveryInfo.recoveryFunction) {
        return false;
    }
    
    // Execute recovery function with timeout
    std::future<bool> future = std::async(std::launch::async, recoveryInfo.recoveryFunction);
    
    // Wait for completion with timeout
    auto status = future.wait_for(std::chrono::duration<double>(recoveryInfo.timeout));
    
    if (status == std::future_status::ready) {
        bool result = future.get();
        if (result) {
            NLM_LOG_INFO(std::string("Successfully recovered from error ") + std::to_string(info.code));
        } else {
            NLM_LOG_WARNING(std::string("Recovery from error ") + std::to_string(info.code) + " failed");
        }
        return result;
    } else {
        NLM_LOG_WARNING(std::string("Recovery from error ") + std::to_string(info.code) + " timed out");
        return false;
    }
}

void ErrorHandler::setRecoveryFunction(ErrorCategory category, 
                                       const std::function<bool()>& recoveryFunction) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->recoveryFunctions[category] = recoveryFunction;
}

std::string ErrorHandler::getErrorMessage(int code) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->errorMessages.find(code);
    if (it != pImpl->errorMessages.end()) {
        return it->second;
    }
    
    return "Unknown error " + std::to_string(code);
}

void ErrorHandler::setErrorMessage(int code, const std::string& message) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorMessages[code] = message;
}

std::string ErrorHandler::formatError(const ErrorInfo& info) {
    std::stringstream ss;
    ss << "Error " << info.code << ": " << info.message << "\n";
    ss << "Category: " << static_cast<int>(info.category) << "\n";
    ss << "Severity: " << static_cast<int>(info.severity) << "\n";
    ss << "Timestamp: " << std::chrono::system_clock::to_time_t(info.timestamp) << "\n";
    
    if (!info.file.empty()) {
        ss << "File: " << info.file << ":" << info.line << "\n";
    }
    
    if (!info.function.empty()) {
        ss << "Function: " << info.function << "\n";
    }
    
    if (!info.stackTrace.empty()) {
        ss << "Stack trace: " << std::endl << info.stackTrace << "\n";
    }
    
    if (!info.context.empty()) {
        ss << "Context:" << std::endl;
        for (const auto& [key, value] : info.context) {
            ss << "  " << key << ": " << value << std::endl;
        }
    }
    
    return ss.str();
}

// Helper functions for stack trace and system information

std::string getStackTrace() {
    #ifdef __GNUC__
        void* callstack[128];
        int frames = backtrace(callstack, 128);
        char** symbols = backtrace_symbols(callstack, frames);
        
        std::stringstream ss;
        if (symbols) {
            for (int i = 0; i < frames; ++i) {
                ss << i << ": " << symbols[i] << "\n";
            }
            std::free(symbols);
        }
        return ss.str();
    #else
        return "Stack trace not available on this platform";
    #endif
}

} // namespace nlm