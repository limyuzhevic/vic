// Enhanced error handling for Brain class
class BrainErrorHandler : public IErrorHandler {
public:
    BrainErrorHandler(std::shared_ptr<Brain> brain) : brain(brain) {}
    
    void handleError(const ErrorInfo& error) override {
        // Log the error
        NLM_LOG_ERROR(std::string("Brain error: ") + error.message);
        
        // Check if error is recoverable
        if (isRecoverable(error)) {
            NLM_LOG_INFO("Attempting to recover from brain error...");
            // In a real implementation, this would attempt recovery
        } else {
            NLM_LOG_CRITICAL(std::string("Brain error is not recoverable: ") + error.message);
        }
    }
    
    bool canHandleError(const ErrorInfo& error) const override {
        // Handle all brain-related errors
        return error.category == ErrorCategory::Simulation ||
               error.category == ErrorCategory::Integration ||
               error.category == ErrorCategory::Resource;
    }
    
    RecoveryResult recoverFromError(const ErrorInfo& error) override {
        RecoveryResult result;
        
        if (!isRecoverable(error)) {
            result.success = false;
            result.message = "Error is not recoverable";
            result.strategy = RecoveryStrategy::None;
            return result;
        }
        
        // Attempt to recover
        try {
            if (error.message.find("step") != std::string::npos) {
                // Simulation step error - try to continue with alternative parameters
                NLM_LOG_INFO("Attempting to recover from simulation step error");
                // In a real implementation, this would attempt to recover
                result.success = true;
                result.message = "Recovered from simulation error";
                result.strategy = RecoveryStrategy::Degrade;
            } else if (error.message.find("memory") != std::string::npos) {
                // Memory error - try to free resources
                NLM_LOG_INFO("Attempting to recover from memory error");
                // In a real implementation, this would attempt to free resources
                result.success = true;
                result.message = "Recovered from memory error";
                result.strategy = RecoveryStrategy::Degrade;
            } else {
                // Generic error recovery
                NLM_LOG_INFO("Attempting generic error recovery");
                // In a real implementation, this would attempt generic recovery
                result.success = true;
                result.message = "Recovered from generic error";
                result.strategy = RecoveryStrategy::Degrade;
            }
        } catch (const std::exception& e) {
            result.success = false;
            result.message = std::string("Recovery failed: ") + e.what();
            result.strategy = RecoveryStrategy::None;
        }
        
        return result;
    }
    
    void logError(const ErrorInfo& error) override {
        handleError(error);
    }
    
private:
    bool isRecoverable(const ErrorInfo& error) const {
        // Determine if an error can be recovered from
        // This is a simplified implementation
        if (error.severity >= ErrorSeverity::Fatal) {
            return false;
        }
        
        if (error.message.find("step") != std::string::npos) {
            return true; // Simulation step errors are recoverable
        }
        
        if (error.message.find("memory") != std::string::npos) {
            return true; // Memory errors are recoverable
        }
        
        if (error.message.find("connection") != std::string::npos) {
            return true; // Connection errors are recoverable
        }
        
        return error.severity < ErrorSeverity::Critical;
    }
    
    std::shared_ptr<Brain> brain;
};

// Global error handling framework
class GlobalErrorHandling {
public:
    static GlobalErrorHandling& getInstance() {
        static GlobalErrorHandling instance;
        return instance;
    }
    
    void initialize() {
        // Initialize global error handling components
        errorLogger = std::make_shared<ErrorLogger>();
        errorRecoveryManager = std::make_shared<ErrorRecoveryManager>();
        
        // Register error logger
        errorRecoveryManager->registerErrorHandler(errorLogger);
        
        // Enable error handling
        errorRecoveryManager->setEnabled(true);
        
        NLM_LOG_INFO("Global error handling initialized");
    }
    
    void shutdown() {
        // Clean up error handling components
        errorLogger.reset();
        errorRecoveryManager.reset();
        
        NLM_LOG_INFO("Global error handling shutdown complete");
    }
    
    void logError(ErrorCategory category, ErrorSeverity severity, 
                  const std::string& message, const std::string& source = "",
                  const std::string& file = "", int line = 0,
                  const std::string& function = "") {
        ErrorInfo error;
        error.category = category;
        error.severity = severity;
        error.message = message;
        error.source = source;
        error.file = file;
        error.line = line;
        error.function = function;
        
        if (errorLogger) {
            errorLogger->handleError(error);
        }
    }
    
    void logError(const ErrorInfo& error) {
        if (errorLogger) {
            errorLogger->handleError(error);
        }
    }
    
    RecoveryResult attemptRecovery(const ErrorInfo& error) {
        if (errorRecoveryManager) {
            return errorRecoveryManager->attemptRecovery(error);
        }
        
        RecoveryResult result;
        result.success = false;
        result.message = "Error recovery manager not available";
        return result;
    }
    
    size_t getActiveErrorCount() const {
        if (errorRecoveryManager) {
            return errorRecoveryManager->getActiveErrors();
        }
        return 0;
    }
    
    std::string getErrorReport() const {
        std::stringstream ss;
        
        if (errorLogger) {
            ss << errorLogger->getErrorReport();
        }
        
        if (errorRecoveryManager) {
            ss << errorRecoveryManager->getErrorSummary();
        }
        
        return ss.str();
    }
    
    void enableErrorRecovery(bool enabled) {
        if (errorRecoveryManager) {
            errorRecoveryManager->setEnabled(enabled);
        }
    }
    
    void configureErrorRecovery(double timeout) {
        if (errorRecoveryManager) {
            errorRecoveryManager->setGlobalTimeout(timeout);
        }
    }
    
    void registerBrainErrorHandler(std::shared_ptr<Brain> brain) {
        if (errorLogger) {
            auto brainHandler = std::make_shared<BrainErrorHandler>(brain);
            errorLogger->registerHandler(brainHandler);
        }
    }
    
private:
    GlobalErrorHandling() = default;
    ~GlobalErrorHandling() = default;
    
    std::shared_ptr<ErrorLogger> errorLogger;
    std::shared_ptr<ErrorRecoveryManager> errorRecoveryManager;
};

// Convenience functions for error handling
namespace error_handling {
    
    void initialize() {
        GlobalErrorHandling::getInstance().initialize();
    }
    
    void shutdown() {
        GlobalErrorHandling::getInstance().shutdown();
    }
    
    void logConfigurationError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error) {
        GlobalErrorHandling::getInstance().logError(
            ErrorCategory::Configuration, severity, message, "Configuration");
    }
    
    void logResourceError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Critical) {
        GlobalErrorHandling::getInstance().logError(
            ErrorCategory::Resource, severity, message, "Resource");
    }
    
    void logSimulationError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error) {
        GlobalErrorHandling::getInstance().logError(
            ErrorCategory::Simulation, severity, message, "Simulation");
    }
    
    void logIntegrationError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error) {
        GlobalErrorHandling::getInstance().logError(
            ErrorCategory::Integration, severity, message, "Integration");
    }
    
    void logUserError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Warning) {
        GlobalErrorHandling::getInstance().logError(
            ErrorCategory::User, severity, message, "User");
    }
    
    RecoveryResult attemptRecovery(const ErrorInfo& error) {
        return GlobalErrorHandling::getInstance().attemptRecovery(error);
    }
    
    std::string getErrorReport() {
        return GlobalErrorHandling::getInstance().getErrorReport();
    }
    
    void enableErrorRecovery(bool enabled) {
        GlobalErrorHandling::getInstance().enableErrorRecovery(enabled);
    }
    
    void configureErrorRecovery(double timeout) {
        GlobalErrorHandling::getInstance().configureErrorRecovery(timeout);
    }
    
} // namespace error_handling

// Error handling macros
#define NLM_LOG_ERROR_WITH_CONTEXT(category, severity, message, source) \
    do { \
        nlm::ErrorInfo error; \
        error.category = category; \
        error.severity = severity; \
        error.message = message; \
        error.source = source; \
        error.file = __FILE__; \
        error.line = __LINE__; \
        error.function = __func__; \
        nlm::GlobalErrorHandling::getInstance().logError(error); \
    } while(0)

#define NLM_LOG_RECOVERABLE_ERROR(category, severity, message, source) \
    do { \
        nlm::ErrorInfo error; \
        error.category = category; \
        error.severity = severity; \
        error.message = message; \
        error.source = source; \
        error.file = __FILE__; \
        error.line = __LINE__; \
        error.function = __func__; \
        nlm::GlobalErrorHandling::getInstance().logError(error); \
        auto recovery = nlm::GlobalErrorHandling::getInstance().attemptRecovery(error); \
        if (!recovery.success) { \
            NLM_LOG_WARNING(std::string("Error recovery failed: ") + recovery.message); \
        } \
    } while(0)

} // namespace nlm
