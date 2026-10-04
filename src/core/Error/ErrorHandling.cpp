#include "ErrorHandling.hpp"

namespace nlm {

// Global error handler instance
ErrorHandlerManager& getErrorHandlerManager() {
    return ErrorHandlerManager::getInstance();
}

// Error reporting utility functions
namespace ErrorReporting {
    
    void logError(const Error& error) {
        ErrorReporter::reportError(error);
    }
    
    void logWarning(const std::string& message) {
        ErrorReporter::reportWarning(message);
    }
    
    void logInfo(const std::string& message) {
        ErrorReporter::reportInfo(message);
    }
}

// Error recovery helper functions
namespace ErrorRecovery {
    
    bool isRecoverable(const Error& error) {
        ErrorCode code = error.getCode();
        switch (code) {
            case ErrorCode::InvalidParameter:
            case ErrorCode::OperationCancelled:
            case ErrorCode::Timeout:
            case ErrorCode::ResourceExhausted:
                return true;
            case ErrorCode::MemoryAccessViolation:
            case ErrorCode::MemoryCorruption:
            case ErrorCode::SystemError:
                return false;
            default:
                return true;
        }
    }
    
    void logErrorRecovery(const Error& error, const std::string& recoveryAction) {
        std::cout << "Error recovery: " << error.what() << " -> " << recoveryAction << std::endl;
    }
}

} // namespace nlm
