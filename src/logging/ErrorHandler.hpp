#pragma once

#include "LogEntry.hpp"
#include <string>
#include <memory>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>

namespace nlm {

class ErrorHandlerImpl {
public:
    ErrorHandlerImpl() : enabled_(true), maxRetries_(3), retryDelay_(100) {}  // 100ms default
    
    void handleError(const std::exception& e, const LogEntry& context) {
        if (!enabled_) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Categorize error
        ErrorCategory category = categorizeError(e);
        
        // Create error log entry
        LogEntry errorEntry;
        errorEntry.level = LogLevel::ERROR;
        errorEntry.message = std::string("ERROR: ") + e.what();
        errorEntry.file = context.file;
        errorEntry.line = context.line;
        errorEntry.function = context.function;
        errorEntry.timestamp = std::chrono::system_clock::now();
        errorEntry.metadata["category"] = errorCategoryToString(category);
        errorEntry.metadata["thread_id"] = context.threadId;
        
        // Log the error
        logError(errorEntry);
        
        // Attempt recovery
        attemptRecovery(e, context, category);
    }
    
    void enable(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    void setMaxRetries(int retries) { maxRetries_ = retries; }
    void setRetryDelay(int delay) { retryDelay_ = delay; }
    
    std::vector<LogEntry> getRecentErrors(size_t limit = 100) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (recentErrors_.size() > limit) {
            recentErrors_.erase(recentErrors_.begin(), 
                               recentErrors_.end() - limit);
        }
        return recentErrors_;
    }
    
private:
    enum class ErrorCategory {
        CONFIGURATION,
        FILE_SYSTEM,
        NETWORK,
        MEMORY,
        VALIDATION,
        UNKNOWN
    };
    
    mutable std::mutex mutex_;
    bool enabled_;
    int maxRetries_;
    int retryDelay_;
    std::vector<LogEntry> recentErrors_;
    
    ErrorCategory categorizeError(const std::exception& e) {
        std::string msg = e.what();
        
        if (msg.find("config") != std::string::npos || 
            msg.find("Config") != std::string::npos) {
            return ErrorCategory::CONFIGURATION;
        }
        
        if (msg.find("file") != std::string::npos ||
            msg.find("File") != std::string::npos ||
            msg.find("io") != std::string::npos) {
            return ErrorCategory::FILE_SYSTEM;
        }
        
        if (msg.find("network") != std::string::npos ||
            msg.find("socket") != std::string::npos ||
            msg.find("connect") != std::string::npos) {
            return ErrorCategory::NETWORK;
        }
        
        if (msg.find("memory") != std::string::npos ||
            msg.find("alloc") != std::string::npos ||
            msg.find("nullptr") != std::string::npos) {
            return ErrorCategory::MEMORY;
        }
        
        if (msg.find("validation") != std::string::npos ||
            msg.find("invalid") != std::string::npos) {
            return ErrorCategory::VALIDATION;
        }
        
        return ErrorCategory::UNKNOWN;
    }
    
    std::string errorCategoryToString(ErrorCategory category) {
        switch (category) {
            case ErrorCategory::CONFIGURATION: return "CONFIGURATION";
            case ErrorCategory::FILE_SYSTEM: return "FILE_SYSTEM";
            case ErrorCategory::NETWORK: return "NETWORK";
            case ErrorCategory::MEMORY: return "MEMORY";
            case ErrorCategory::VALIDATION: return "VALIDATION";
            default: return "UNKNOWN";
        }
    }
    
    void logError(const LogEntry& entry) {
        // Add to recent errors
        recentErrors_.push_back(entry);
        
        // Output to stderr
        std::cerr << "[ERROR] ";
        auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
        std::cerr << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        std::cerr << " " << entry.message << " [" << entry.metadata.at("category") << "]" << std::endl;
    }
    
    void attemptRecovery(const std::exception& e, const LogEntry& context, ErrorCategory category) {
        // Only retry for certain error types
        if (category == ErrorCategory::NETWORK || category == ErrorCategory::FILE_SYSTEM) {
            for (int attempt = 0; attempt < maxRetries_; ++attempt) {
                std::this_thread::sleep_for(std::chrono::milliseconds(retryDelay_));
                
                // Check if recovery is possible (simplified)
                try {
                    // In real implementation, this would attempt actual recovery
                    LogEntry recoveryEntry;
                    recoveryEntry.level = LogLevel::INFO;
                    recoveryEntry.message = "Recovery attempt " + std::to_string(attempt + 1) + " successful";
                    recoveryEntry.timestamp = std::chrono::system_clock::now();
                    
                    logError(recoveryEntry);
                    return;
                } catch (...) {
                    continue;
                }
            }
        }
    }
};

class ErrorHandler {
public:
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }
    
    void handle(const std::exception& e, const LogEntry& context = LogEntry()) {
        pImpl_->handleError(e, context);
    }
    
    void enable(bool enabled) { pImpl_->enable(enabled); }
    bool isEnabled() const { return pImpl_->isEnabled(); }
    
    void setMaxRetries(int retries) { pImpl_->setMaxRetries(retries); }
    void setRetryDelay(int delay) { pImpl_->setRetryDelay(delay); }
    
    std::vector<LogEntry> getRecentErrors(size_t limit = 100) {
        return pImpl_->getRecentErrors(limit);
    }
    
private:
    ErrorHandler() : pImpl_(std::make_unique<ErrorHandlerImpl>()) {}
    ~ErrorHandler() = default;
    
    std::unique_ptr<ErrorHandlerImpl> pImpl_;
};

} // namespace nlm