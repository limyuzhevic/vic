// Enhanced Error Handling Implementation
// Implementation of the enhanced error handling and logging system

#include "EnhancedErrorHandling.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>

namespace nlm {

// Error handling implementation
void ConsoleErrorHandler::handleError(const EnhancedError& error) {
    if (error.severity < minSeverity_) return;
    
    std::ostringstream oss;
    oss << error.getFullMessage();
    
    switch (error.severity) {
        case ErrorSeverity::Debug:
            NLM_LOG_DEBUG(oss.str());
            break;
        case ErrorSeverity::Info:
            NLM_LOG_INFO(oss.str());
            break;
        case ErrorSeverity::Warning:
            NLM_LOG_WARNING(oss.str());
            break;
        case ErrorSeverity::Error:
            NLM_LOG_ERROR(oss.str());
            break;
        case ErrorSeverity::Critical:
        case ErrorSeverity::Fatal:
            NLM_LOG_CRITICAL(oss.str());
            break;
    }
}

void FileErrorHandler::handleError(const EnhancedError& error) {
    if (error.severity < minSeverity_) return;
    if (!logFile_) return;
    
    std::lock_guard<std::mutex> lock(fileMutex_);
    
    fprintf(logFile_, "%s\n", error.getFullMessage().c_str());
    fflush(logFile_);
}

void ErrorRecoveryHandler::handleError(const EnhancedError& error) {
    if (!canHandleError(error)) return;
    
    NLM_LOG_INFO("Error recovery: " + error.message);
    
    // Check if we can recover from this error
    if (canRecoverFromError(error)) {
        NLM_LOG_INFO("Attempting to recover from error...");
        if (autoRecover_) {
            recoveryResult_ = attemptRecovery(error);
            if (recoveryResult_.success) {
                NLM_LOG_INFO("Recovery successful: " + recoveryResult_.message);
                return;
            } else {
                NLM_LOG_ERROR("Recovery failed: " + recoveryResult_.message);
            }
        }
    }
    
    // If recovery failed, try to retry if applicable
    if (error.severity >= ErrorSeverity::Error && retryCount_ < maxRetries_) {
        retryCount_++;
        NLM_LOG_WARNING("Retrying operation (attempt " + std::to_string(retryCount_) + "/" + std::to_string(maxRetries_) + ")");
        // TODO: Re-execute the operation that caused the error
    }
}

} // namespace nlm
