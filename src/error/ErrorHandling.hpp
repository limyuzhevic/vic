#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <mutex>
#include <map>
#include <functional>
#include <sstream>

namespace nlm::error {

// Error types for classification
enum class ErrorType {
    CONFIG_VALIDATION,    // Configuration validation errors
    MEMORY_ALLOCATION,     // Memory allocation failures
    NEURAL_DYNAMICS,       // Neural network computation errors
    PLASTICITY_FAILURE,    // Synaptic plasticity update errors
    SIMULATION_ERROR,      // General simulation errors
    FILE_OPERATION,        // File read/write errors
    NETWORK_ERROR,         // Network communication errors
    UNKNOWN                // Unknown or uncategorized errors
};

// Detailed error context
struct ErrorContext {
    ErrorType type;
    std::string message;
    std::string file;
    int line;
    std::string details;
    std::vector<std::string> recoverySteps;
    std::chrono::system_clock::time_point timestamp;
}; 

// Recovery result
struct RecoveryResult {
    bool success;
    std::string message;
    std::vector<std::string> warnings;
};

// ErrorHandler class with thread-safe error management
class ErrorHandler {
public:
    // Singleton access
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }

    // Handle error with recovery attempt
    static bool handleError(const ErrorContext& error) {
        return getInstance()._handleError(error);
    }

    // Handle error without recovery attempt
    static bool handleError(const ErrorContext& error, bool attemptRecovery) {
        return getInstance()._handleError(error, attemptRecovery);
    }

    // Log error without recovery
    static void logError(const ErrorContext& error) {
        getInstance()._logError(error);
    }

    // Get recent error history
    static std::vector<ErrorContext> getErrorHistory(size_t count = 100) {
        return getInstance()._getErrorHistory(count);
    }

    // Clear error history
    static void clearErrorHistory() {
        getInstance()._clearErrorHistory();
    }

    // Format error for display
    static std::string formatError(const ErrorContext& error) {
        return getInstance()._formatError(error);
    }

    // Set custom recovery callback
    static void setRecoveryCallback(std::function<RecoveryResult(const ErrorContext&)> callback) {
        getInstance()._setRecoveryCallback(callback);
    }

private:
    ErrorHandler() = default;

    // Internal error handling implementation
    bool _handleError(const ErrorContext& error, bool attemptRecovery = true);

    // Internal logging implementation
    void _logError(const ErrorContext& error);

    // Get error history
    std::vector<ErrorContext> _getErrorHistory(size_t count);

    // Clear error history
    void _clearErrorHistory();

    // Format error for display
    std::string _formatError(const ErrorContext& error);

    // Set recovery callback
    void _setRecoveryCallback(std::function<RecoveryResult(const ErrorContext&)> callback);

    // Thread-safe storage
    std::vector<ErrorContext> errorHistory_;
    std::mutex mutex_;
    size_t maxHistorySize_ = 1000;
    std::function<RecoveryResult(const ErrorContext&)> recoveryCallback_;
};

// Helper function to get current timestamp
inline std::chrono::system_clock::time_point getCurrentTimestamp() {
    return std::chrono::system_clock::now();
}

} // namespace nlm::error

// Error handling macros
#define NLM_THROW_ERROR(type, msg) \
    nlm::error::ErrorHandler::handleError({ \
        type, msg, __FILE__, __LINE__, {}, {}, nlm::error::getCurrentTimestamp() })

#define NLM_LOG_ERROR_WITH_RECOVERY(type, msg, recovery) \
    nlm::error::ErrorHandler::handleError({ \
        type, msg, __FILE__, __LINE__, {}, {recovery}, nlm::error::getCurrentTimestamp() })

#define NLM_LOG_ERROR_WITH_CONTEXT(type, msg) \
    nlm::error::ErrorHandler::logError({ \
        type, msg, __FILE__, __LINE__, {}, {}, nlm::error::getCurrentTimestamp() })