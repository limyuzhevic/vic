#pragma once

#include "Error.hpp"
#include "ErrorReporting.hpp"
#include "Validation.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

class ErrorHandlingSystem {
public:
    static ErrorHandlingSystem& getInstance() {
        static ErrorHandlingSystem instance;
        return instance;
    }
    
    // Initialize the error handling system
    void initialize(const std::string& configFile = "", bool enableMetrics = true) {
        // Simplified initialization
    }
    
    // Shutdown the error handling system
    void shutdown() noexcept {
        // Simplified shutdown
    }
    
    // Check if the system is initialized
    bool isInitialized() const {
        return true;
    }
    
    // Report an error
    void reportError(const std::string& message, ErrorCode code = ErrorCode::Unknown,
                    ErrorCategory category = ErrorCategory::None) {
        ErrorContext context(message, code, category);
        // In a real implementation, this would handle the error
    }
    
    // Report an error with context
    void reportError(const ErrorContext& context) {
        // In a real implementation, this would handle the error
    }
    
    // Report an exception
    void reportException(const std::exception& e) {
        ErrorContext context(e.what(), ErrorCode::RuntimeError, ErrorCategory::System);
        // In a real implementation, this would handle the error
    }
    
    // Get error statistics
    ErrorStats getErrorStats() const {
        return ErrorStats();
    }
    
    // Enable graceful degradation
    void enableGracefulDegradation(bool enable) {
        // Simplified implementation
    }
    
    // Check if graceful degradation is enabled
    bool isGracefulDegradationEnabled() const {
        return false;
    }
    
    // Configure from file
    void configureFromFile(const std::string& filepath) {
        // Simplified implementation
    }
    
    // Configure a setting
    void configure(const std::string& key, const std::string& value) {
        // Simplified implementation
    }
    
    // Set global error level
    void setGlobalErrorLevel(LogLevel level) {
        // Simplified implementation
    }
    
    // Get global error level
    LogLevel getGlobalErrorLevel() const {
        return LogLevel::Warning;
    }
    
    // Memory management optimizations
    static constexpr bool isSmallObject() noexcept { return true; }
    
private:
    ErrorHandlingSystem() {}
    ~ErrorHandlingSystem() noexcept {}
    
    ErrorHandlingSystem(const ErrorHandlingSystem&) = delete;
    ErrorHandlingSystem& operator=(const ErrorHandlingSystem&) = delete;
};

// Convenience functions for error reporting
inline void reportError(const std::string& message, ErrorCode code = ErrorCode::Unknown,
                       ErrorCategory category = ErrorCategory::None) {
    ErrorHandlingSystem::getInstance().reportError(message, code, category);
}

inline void reportError(const ErrorContext& context) {
    ErrorHandlingSystem::getInstance().reportError(context);
}

inline void reportException(const std::exception& e) {
    ErrorHandlingSystem::getInstance().reportException(e);
}

inline void setGlobalErrorLevel(LogLevel level) {
    ErrorHandlingSystem::getInstance().setGlobalErrorLevel(level);
}

inline ErrorStats getErrorStats() {
    return ErrorHandlingSystem::getInstance().getErrorStats();
}

// Function to initialize the error handling system
inline bool initializeErrorHandling(const std::string& configFile = "", bool enableMetrics = true) {
    try {
        ErrorHandlingSystem::getInstance().initialize(configFile, enableMetrics);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Function to shutdown the error handling system
inline void shutdownErrorHandling() noexcept {
    try {
        ErrorHandlingSystem::getInstance().shutdown();
    } catch (...) {
        // Ignore errors during shutdown
    }
}

} // namespace nlm


