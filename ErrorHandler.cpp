#include "NLMError.hpp"

namespace nlm {

class ErrorHandler {
public:
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }
    
    // Set global error logging callback
    using ErrorCallback = std::function<void(const NLMError&)>;
    void setErrorCallback(ErrorCallback callback) {
        errorCallback_ = callback;
    }
    
    // Handle error (log and/or call callback)
    void handleError(const NLMError& error) {
        // Log error using NLM logger
        Logger& logger = Logger::getInstance();
        logger.error(ErrorUtils::formatForLogging(error));
        
        // Call user callback if set
        if (errorCallback_) {
            errorCallback_(error);
        }
    }
    
    // Handle warning (log only)
    void handleWarning(const NLMError& error) {
        Logger& logger = Logger::getInstance();
        logger.warning(ErrorUtils::formatForLogging(error));
    }
    
    // Handle critical error (log and exit)
    void handleCritical(const NLMError& error) {
        Logger& logger = Logger::getInstance();
        logger.critical(ErrorUtils::formatForLogging(error));
        // In production, might exit or call system shutdown
    }
    
    // Enable/disable error recovery mode
    void setRecoveryMode(bool enabled) {
        recoveryMode_ = enabled;
    }
    
    // Check if error is recoverable
    static bool isRecoverable(const NLMError& error) {
        return ErrorUtils::isRecoverable(error.getCode());
    }
    
    // Get last error
    const NLMError* getLastError() const {
        return lastError_.get();
    }
    
    // Clear last error
    void clearLastError() {
        lastError_ = nullptr;
    }
    
    // Create error from existing exception
    template<typename T>
    static std::unique_ptr<NLMError> createFromException(const T& exc) {
        std::string message = exc.what();
        // Try to extract error code from message or use default
        ErrorCode code = ErrorCode::ERROR_UNKNOWN;
        if (message.find("null") != std::string::npos) {
            code = ErrorCode::ERROR_NULL_POINTER;
        } else if (message.find("invalid") != std::string::npos) {
            code = ErrorCode::ERROR_INVALID_PARAMETER;
        } else if (message.find("out of bounds") != std::string::npos) {
            code = ErrorCode::ERROR_OUT_OF_BOUNDS;
        }
        
        auto error = std::make_unique<NLMError>(message, code);
        error->setCause(exc);
        return error;
    }
    
    // Convert existing error codes to NLMError (for backward compatibility)
    static std::unique_ptr<NLMError> convertFromLegacy(int legacyCode, const std::string& message = "") {
        // Map legacy error codes to NLM error codes
        ErrorCode code;
        switch (legacyCode) {
            case 0: code = ErrorCode::ERROR_OK; break;
            case 1: code = ErrorCode::ERROR_NULL_POINTER; break;
            case 2: code = ErrorCode::ERROR_INVALID_PARAMETER; break;
            case 3: code = ErrorCode::ERROR_OUT_OF_BOUNDS; break;
            case 4: code = ErrorCode::ERROR_OUT_OF_MEMORY; break;
            default: code = ErrorCode::ERROR_UNKNOWN; break;
        }
        
        std::string msg = message.empty() ? ErrorUtils::getErrorCodeString(code) : message;
        auto error = std::make_unique<NLMError>(msg, code);
        return error;
    }
    
    // Check if error is OK (no error)
    static bool isOK(const NLMError& error) {
        return error.getCode() == ErrorCode::ERROR_OK;
    }
    
    // Get error severity string
    static std::string getSeverityString(ErrorSeverity severity) {
        return ErrorUtils::getSeverityString(severity);
    }
    
    // Get error code string
    static std::string getErrorCodeString(ErrorCode code) {
        return ErrorUtils::getErrorCodeString(code);
    }
    
private:
    ErrorHandler() : recoveryMode_(true) {}
    
    ErrorCallback errorCallback_;
    bool recoveryMode_;
    std::unique_ptr<const NLMError> lastError_;
};

} // namespace nlm
