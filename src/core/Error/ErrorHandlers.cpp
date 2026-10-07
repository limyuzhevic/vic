#pragma once

#include "ErrorSystem.hpp"
#include "core/Logger/Logger.hpp"
#include <stdexcept>

namespace nlm {

// Default error handler that logs to the logger
class DefaultErrorHandler {
public:
    static void handleError(const NLMError& error) {
        std::string errorMsg = error.what();
        std::string details = error.getErrorDetails();
        
        // Log the error with appropriate level
        ErrorSeverity severity = getErrorSeverity(error);
        
        switch (severity) {
            case ErrorSeverity::Debug:
                NLM_LOG_DEBUG(details);
                break;
            case ErrorSeverity::Info:
                NLM_LOG_INFO(details);
                break;
            case ErrorSeverity::Warning:
                NLM_LOG_WARNING(details);
                break;
            case ErrorSeverity::Error:
                NLM_LOG_ERROR(details);
                break;
            case ErrorSeverity::Critical:
                NLM_LOG_CRITICAL(details);
                break;
        }
        
        // Also output to stderr for critical errors
        if (error.isCritical()) {
            std::cerr << "CRITICAL ERROR: " << errorMsg << std::endl;
        }
    }
    
private:
    enum class ErrorSeverity {
        Debug = 0,
        Info = 1,
        Warning = 2,
        Error = 3,
        Critical = 4
    };
    
    static ErrorSeverity getErrorSeverity(const NLMError& error) {
        if (error.isCritical()) {
            return ErrorSeverity::Critical;
        }
        
        switch (error.getType()) {
            case NLMError::ErrorType::ConfigurationError:
            case NLMError::ErrorType::InputValidationError:
                return ErrorSeverity::Warning;
            case NLMError::ErrorType::MemoryError:
            case NLMError::ErrorType::ResourceExhaustionError:
                return ErrorSeverity::Error;
            case NLMError::ErrorType::SimulationError:
            case NLMError::ErrorType::PlasticityError:
                return ErrorSeverity::Error;
            case NLMError::ErrorType::CheckpointError:
            case NLMError::ErrorType::PlatformError:
                return ErrorSeverity::Critical;
            default:
                return ErrorSeverity::Error;
        }
    }
};

// Error recovery handler implementations
namespace ErrorRecoveryHandlers {
    
    bool attemptConfigurationRecovery(const NLMError& error) {
        if (error.getType() == NLMError::ErrorType::ConfigurationError) {
            NLM_LOG_WARNING("Attempting configuration recovery...");
            // Attempt to reload default configuration
            // Implementation depends on your config system
            return true;
        }
        return false;
    }
    
    bool attemptMemoryRecovery(const NLMError& error) {
        if (error.getType() == NLMError::ErrorType::MemoryError) {
            NLM_LOG_WARNING("Attempting memory recovery (garbage collection)...");
            // Attempt garbage collection or resource cleanup
            // Implementation depends on your memory management
            return true;
        }
        return false;
    }
    
    bool attemptRestartRecovery(const NLMError& error) {
        if (error.getType() == NLMError::ErrorType::SimulationError) {
            NLM_LOG_WARNING("Attempting simulation restart...");
            // Attempt to reset neural state and restart simulation
            return true;
        }
        return false;
    }
}

} // namespace nlm