#pragma once

#include "Errors.hpp"
#include <chrono>

namespace nlm {

class ErrorHandling {
public:
    // Initialize error handling system
    static void initialize() {
        NLM_LOG_INFO("Initializing error handling system");
        getGlobalErrorHandler().setMode(GlobalErrorHandler::Mode::THROW);
    }

    // Configure error handler mode
    static void setErrorHandlerMode(GlobalErrorHandler::Mode mode) {
        getGlobalErrorHandler().setMode(mode);
        NLM_LOG_INFO("Error handler mode set to: " << static_cast<int>(mode));
    }

    // Add custom error handler
    static void addErrorHandler(const GlobalErrorHandler::ErrorHandlerFunc& handler) {
        getGlobalErrorHandler().addCustomHandler(handler);
    }

    // Set up circuit breaker for specific operations
    static void setupCircuitBreaker(const std::string& operationName,
                                   const ErrorRecovery::RecoveryConfig& config) {
        // In practice, you'd store circuit breakers in a map keyed by operation name
        NLM_LOG_INFO("Circuit breaker setup for operation: " << operationName);
    }

    // Perform safe operation with recovery
    template<typename Func, typename... Args>
    static auto executeWithErrorHandling(Func&& func, const std::string& operationName,
                                         Args&&... args) {
        try {
            NLM_LOG_DEBUG("Executing operation: " << operationName);
            auto result = func(std::forward<Args>(args)...);
            NLM_LOG_DEBUG("Operation completed successfully: " << operationName);
            return result;
        } catch (const NLMException& e) {
            NLM_LOG_ERROR("Error in operation " << operationName << ": " << e.what());
            ErrorReporter::reportError(e);
            throw;
        }
    }

    // Graceful degradation wrapper
    template<typename Func, typename... Args>
    static auto executeWithGracefulDegradation(Func&& func, const std::string& operationName,
                                              const ErrorRecovery::RecoveryConfig& config,
                                              Args&&... args) {
        try {
            return ErrorRecovery::withGracefulDegradation(func, config, std::forward<Args>(args)...);
        } catch (const NLMException& e) {
            NLM_LOG_WARNING("Graceful degradation for operation " << operationName << ": " << e.what());
            throw;
        }
    }

    // Retry mechanism wrapper
    template<typename Func, typename... Args>
    static auto executeWithRetry(Func&& func, const std::string& operationName,
                                const ErrorRecovery::RecoveryConfig& config,
                                Args&&... args) {
        try {
            return ErrorRecovery::withRetry(func, config, std::forward<Args>(args)...);
        } catch (const NLMException& e) {
            NLM_LOG_WARNING("Retry exhausted for operation " << operationName << ": " << e.what());
            throw;
        }
    }

    // Validation wrapper
    template<typename Func, typename... Args>
    static auto executeWithValidation(Func&& func, const std::string& operationName,
                                      Args&&... args) {
        try {
            NLM_LOG_DEBUG("Validating operation: " << operationName);
            auto result = func(std::forward<Args>(args)...);
            NLM_LOG_DEBUG("Validation passed for operation: " << operationName);
            return result;
        } catch (const NLMException& e) {
            NLM_LOG_ERROR("Validation failed for operation " << operationName << ": " << e.what());
            throw;
        }
    }

    // Monitor operation performance
    struct OperationMonitor {
        std::string operationName;
        std::chrono::system_clock::time_point startTime;
        bool completed = false;
        bool success = false;
        std::string errorMessage;

        OperationMonitor(const std::string& name) : operationName(name) {
            startTime = std::chrono::system_clock::now();
        }

        void complete(bool success = true, const std::string& error = "") {
            completed = true;
            this->success = success;
            errorMessage = error;
            auto endTime = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            if (success) {
                NLM_LOG_INFO("Operation " << operationName << " completed in " << duration.count() << "ms");
            } else {
                NLM_LOG_ERROR("Operation " << operationName << " failed: " << error << " after " << duration.count() << "ms");
            }
        }

        double getDurationMs() const {
            if (!completed) {
                auto now = std::chrono::system_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
                return duration.count();
            }
            return 0.0; // Simplified
        }
    };

    static OperationMonitor monitorOperation(const std::string& name) {
        return OperationMonitor(name);
    }

    // Configuration validation
    static void validateSystemConfiguration(const Config& config) {
        NLM_LOG_DEBUG("Validating system configuration");
        std::vector<std::string> errors;
        if (!config.validate(errors)) {
            for (const auto& error : errors) {
                NLM_LOG_ERROR("Config validation error: " << error);
                NLM_THROW(ErrorCode::CONFIG_VALIDATION_FAILED, error);
            }
        }
        NLM_LOG_INFO("Configuration validation passed");
    }

    // Health check
    static bool performHealthCheck() {
        NLM_LOG_DEBUG("Performing health check");
        bool healthy = true;
        
        // Check critical components
        try {
            auto& logger = Logger::getGlobal();
            logger.info("Health check: Logger is working");
        } catch (...) {
            NLM_LOG_ERROR("Health check: Logger failed");
            healthy = false;
        }
        
        // Additional health checks would go here
        // For example: check memory pools, file descriptors, etc.
        
        if (healthy) {
            NLM_LOG_INFO("Health check passed");
        } else {
            NLM_LOG_ERROR("Health check failed");
        }
        
        return healthy;
    }
};

} // namespace nlm