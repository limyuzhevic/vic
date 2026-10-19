#ifndef NLM_ERROR_HANDLING_INL
#define NLM_ERROR_HANDLING_INL

#include "ErrorHandling.hpp"
#include <fmt/format.h>
#include <fmt/chrono.h>

namespace nlm {

// Additional utility functions
namespace ErrorContextUtils {
    
    inline std::string getCurrentTimeString(const std::string& format) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_time = *std::localtime(&time);
        
        std::ostringstream oss;
        oss << std::put_time(&tm_time, format.c_str());
        return oss.str();
    }
    
    inline std::string categoryToString(ErrorCategory category) {
        switch (category) {
            case ErrorCategory::NONE: return "none";
            case ErrorCategory::CONFIGURATION: return "configuration";
            case ErrorCategory::NEURAL_NETWORK: return "neural_network";
            case ErrorCategory::MEMORY_MANAGEMENT: return "memory_management";
            case ErrorCategory::PERFORMANCE: return "performance";
            case ErrorCategory::IO: return "io";
            case ErrorCategory::INTEGRATION: return "integration";
            case ErrorCategory::SYSTEM: return "system";
            case ErrorCategory::THREADING: return "threading";
            case ErrorCategory::VALIDATION: return "validation";
            case ErrorCategory::RECOVERY: return "recovery";
            default: return "unknown";
        }
    }
}

// Error handler implementations
namespace ErrorHandlers {
    
    class NeuralNetworkErrorHandler : public IErrorHandler {
    public:
        void handleError(const std::shared_ptr<NLMException>& error) override {
            std::cerr << "[NEURAL_NETWORK] " << error->what() << std::endl;
            
            // Trigger recovery actions if needed
            if (error->getSeverity() >= ErrorSeverity::CRITICAL) {
                std::cerr << "CRITICAL neural network error - attempting recovery..." << std::endl;
                // Reinitialize neural network components
            }
        }
        
        bool canHandle(ErrorCategory category, ErrorCode code) const override {
            return category == ErrorCategory::NEURAL_NETWORK;
        }
        
        ErrorAction getDefaultAction(ErrorCategory category) const override {
            return ErrorAction::RECOVER;
        }
    };
    
    class MemoryManagementErrorHandler : public IErrorHandler {
    public:
        void handleError(const std::shared_ptr<NLMException>& error) override {
            std::cerr << "[MEMORY_MANAGEMENT] " << error->what() << std::endl;
        }
        
        bool canHandle(ErrorCategory category, ErrorCode code) const override {
            return category == ErrorCategory::MEMORY_MANAGEMENT;
        }
        
        ErrorAction getDefaultAction(ErrorCategory category) const override {
            return ErrorAction::FAIL_FAST;
        }
    };
    
    class ConfigurationErrorHandler : public IErrorHandler {
    public:
        void handleError(const std::shared_ptr<NLMException>& error) override {
            std::cerr << "[CONFIGURATION] " << error->what() << std::endl;
        }
        
        bool canHandle(ErrorCategory category, ErrorCode code) const override {
            return category == ErrorCategory::CONFIGURATION;
        }
        
        ErrorAction getDefaultAction(ErrorCategory category) const override {
            return ErrorAction::RECOVER;
        }
    };
}

// Initialize error handling system
inline void initializeErrorHandling() {
    GlobalErrorHandler& globalHandler = GlobalErrorHandler::getInstance();
    
    // Set up default error handlers
    globalHandler.setHandler(std::make_shared<ErrorHandlers::NeuralNetworkErrorHandler>());
    globalHandler.setHandler(std::make_shared<ErrorHandlers::MemoryManagementErrorHandler>());
    globalHandler.setHandler(std::make_shared<ErrorHandlers::ConfigurationErrorHandler>());
    
    // Configure recovery strategies
    ErrorManager& errorManager = getGlobalErrorManager();
    
    // Neural network errors
    errorManager.configureError(
        ErrorCategory::NEURAL_NETWORK,
        ErrorCode::NN_INITIALIZATION_FAILED,
        RecoveryStrategies::NeuralNetworkRecovery::create()
    );
    
    // Memory management errors
    errorManager.configureError(
        ErrorCategory::MEMORY_MANAGEMENT,
        ErrorCode::MEMORY_ALLOCATION_FAILED,
        RecoveryStrategies::MemoryManagementRecovery::create()
    );
    
    // Configuration errors
    errorManager.configureError(
        ErrorCategory::CONFIGURATION,
        ErrorCode::CONFIG_LOAD_FAILED,
        RecoveryStrategies::ConfigurationRecovery::create()
    );
    
    // Set up logging integration
    LoggingIntegration::getGlobalIntegration();
}

// Utility for thread-safe error handling
inline void recordThreadError(const std::shared_ptr<NLMException>& error) {
    ThreadSafeErrorHandling::getGlobalThreadSafeManager().recordThreadError(error);
}

inline void clearThreadErrors() {
    ThreadSafeErrorHandling::getGlobalThreadSafeManager().clearThreadErrors();
}

// Error checking macros implementation
inline bool checkPreConditions() {
    std::vector<std::string> failures = ErrorPrevention::getGlobalPreventionSystem().runAllChecks();
    
    if (!failures.empty()) {
        std::ostringstream oss;
        oss << "Pre-condition checks failed:\n";
        for (const auto& failure : failures) {
            oss << "  - " << failure << "\n";
        }
        
        auto error = std::make_shared<NLMException>(
            ErrorSeverity::ERROR,
            ErrorCategory::VALIDATION,
            ErrorCode::PRECONDITION_FAILED,
            oss.str()
        );
        
        getGlobalErrorManager().recordError(error);
        return false;
    }
    
    return true;
}

} // namespace nlm

#endif // NLM_ERROR_HANDLING_INL
