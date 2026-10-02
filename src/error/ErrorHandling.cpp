#include "ErrorHandling.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm::error {

// Internal error handling implementation
bool ErrorHandler::_handleError(const ErrorContext& error, bool attemptRecovery) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Add to error history
    errorHistory_.push_back(error);
    if (errorHistory_.size() > maxHistorySize_) {
        errorHistory_.erase(errorHistory_.begin());
    }

    // Attempt recovery if requested and callback exists
    if (attemptRecovery && recoveryCallback_) {
        RecoveryResult recovery = recoveryCallback_(error);
        if (recovery.success) {
            return true;
        }
    }

    // Log the error
    _logError(error);
    return false;
}

// Internal logging implementation
void ErrorHandler::_logError(const ErrorContext& error) {
    // Log error using Logger class
    std::string formattedMsg = _formatError(error);
    
    // Use appropriate logging based on error type
    switch (error.type) {
        case ErrorType::CONFIG_VALIDATION:
        case ErrorType::MEMORY_ALLOCATION:
        case ErrorType::UNKNOWN:
            NLM_LOG_ERROR(formattedMsg);
            break;
        case ErrorType::NEURAL_DYNAMICS:
        case ErrorType::PLASTICITY_FAILURE:
            NLM_LOG_WARNING(formattedMsg);
            break;
        case ErrorType::SIMULATION_ERROR:
            NLM_LOG_ERROR(formattedMsg);
            break;
        case ErrorType::FILE_OPERATION:
        case ErrorType::NETWORK_ERROR:
            NLM_LOG_ERROR(formattedMsg);
            break;
    }
}

// Get error history
std::vector<ErrorContext> ErrorHandler::_getErrorHistory(size_t count) {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t start = (count < errorHistory_.size()) ? 
                   errorHistory_.size() - count : 0;
    return std::vector<ErrorContext>(errorHistory_.begin() + start, errorHistory_.end());
}

// Clear error history
void ErrorHandler::_clearErrorHistory() {
    std::lock_guard<std::mutex> lock(mutex_);
    errorHistory_.clear();
}

// Format error for display
std::string ErrorHandler::_formatError(const ErrorContext& error) {
    std::ostringstream oss;
    oss << "[Error " << static_cast<int>(error.type) << "] " << error.message;
    if (!error.file.empty()) {
        oss << " in " << error.file << ":" << error.line;
    }
    if (!error.details.empty()) {
        oss << " - Details: " << error.details;
    }
    if (!error.recoverySteps.empty()) {
        oss << " - Recovery steps: ";
        for (size_t i = 0; i < error.recoverySteps.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << error.recoverySteps[i];
        }
    }
    return oss.str();
}

// Set recovery callback
void ErrorHandler::_setRecoveryCallback(std::function<RecoveryResult(const ErrorContext&)> callback) {
    recoveryCallback_ = callback;
}

} // namespace nlm::error