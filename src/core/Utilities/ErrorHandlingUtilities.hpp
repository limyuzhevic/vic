// Error handling utilities and managers for NLM
#pragma once

#include "ErrorSystem.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>

namespace nlm {

// Error context for enhanced error reporting
struct ErrorContext {
    std::string component;
    std::string operation;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> relatedErrors;
    std::string suggestedAction;
    
    ErrorContext() : line(0) {}
    ErrorContext(const std::string& comp, const std::string& op,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "")
        : component(comp), operation(op), file(file), line(line),
          function(function), timestamp(std::chrono::system_clock::now()) {}
};

// Error handling strategy interface
class IErrorHandlingStrategy {
public:
    virtual ~IErrorHandlingStrategy() = default;
    virtual bool canHandle(const std::exception& error) const = 0;
    virtual std::exception handleError(const std::exception& error) = 0;
    virtual std::string getStrategyName() const = 0;
    virtual ErrorSeverity getSeverity() const = 0;
};

// Error recovery strategy interface
class IErrorRecoveryStrategy {
public:
    virtual ~IErrorRecoveryStrategy() = default;
    virtual bool canRecover(const std::exception& error) const = 0;
    virtual bool recover(std::exception& error) = 0;
    virtual std::string getStrategyName() const = 0;
    virtual RecoveryResult getRecoveryResult() const = 0;
};

// Error handler implementation
class ErrorHandler {
private:
    std::vector<std::shared_ptr<IErrorHandlingStrategy>> strategies_;
    std::vector<std::shared_ptr<IErrorRecoveryStrategy>> recoveryStrategies_;
    std::shared_ptr<IErrorReporter> errorReporter_;
    ErrorSeverity defaultSeverity_;
    bool enableAutoRecovery_;
    bool propagateExceptions_;
    size_t maxExceptionStackSize_;
    
public:
    ErrorHandler()
        : defaultSeverity_(ErrorSeverity::Error), enableAutoRecovery_(true),
          propagateExceptions_(true), maxExceptionStackSize_(10) {
        // Add default strategies
        strategies_.push_back(std::make_shared<DefaultErrorHandlingStrategy>());
        strategies_.push_back(std::make_shared<BrainErrorHandlingStrategy>());
        strategies_.push_back(std::make_shared<MemoryErrorHandlingStrategy>());
        strategies_.push_back(std::make_shared<ConfigErrorHandlingStrategy>());
        
        recoveryStrategies_.push_back(std::make_shared<AutomaticRecoveryStrategy>());
        recoveryStrategies_.push_back(std::make_shared<FallbackRecoveryStrategy>());
        recoveryStrategies_.push_back(std::make_shared<ResetRecoveryStrategy>());
        
        errorReporter_ = std::make_shared<ConsoleErrorReporter>(defaultSeverity_);
    }
    
    // Handle exception with context
    std::exception handle(const std::exception& error, const ErrorContext& context = ErrorContext()) {
        // Try each strategy to handle the error
        for (const auto& strategy : strategies_) {
            if (strategy->canHandle(error)) {
                auto handledError = strategy->handleError(error);
                // Add context to the error
                addContext(handledError, context);
                // Report the error
                errorReporter_->reportError(static_cast<const NLMException&>(handledError));
                return handledError;
            }
        }
        
        // No strategy found, return original error
        errorReporter_->reportError(static_cast<const NLMException&>(error));
        return error;
    }
    
    // Attempt recovery
    RecoveryResult attemptRecovery(std::exception& error) {
        if (!enableAutoRecovery_) {
            return RecoveryResult::NotApplicable;
        }
        
        for (const auto& strategy : recoveryStrategies_) {
            if (strategy->canRecover(error)) {
                bool success = strategy->recover(error);
                return success ? RecoveryResult::Success : RecoveryResult::Failed;
            }
        }
        
        return RecoveryResult::NotApplicable;
    }
    
    // Get error statistics
    struct ErrorStats {
        size_t totalHandled_ = 0;
        size_t autoRecovered_ = 0;
        size_t propagated_ = 0;
        std::map<ErrorSeverity, size_t> severityCounts;
    };
    
    ErrorStats getErrorStats() const {
        ErrorStats stats;
        // Implementation would collect statistics from strategies and reporter
        return stats;
    }
    
    // Configuration methods
    void setDefaultSeverity(ErrorSeverity severity) {
        defaultSeverity_ = severity;
        if (errorReporter_) {
            errorReporter_->setErrorThreshold(severity);
        }
    }
    
    void setAutoRecoveryEnabled(bool enabled) {
        enableAutoRecovery_ = enabled;
    }
    
    void setPropagateExceptions(bool propagate) {
        propagateExceptions_ = propagate;
    }
    
    void setMaxExceptionStackSize(size_t size) {
        maxExceptionStackSize_ = size;
    }
    
    // Add custom strategies
    void addHandlingStrategy(std::shared_ptr<IErrorHandlingStrategy> strategy) {
        strategies_.push_back(strategy);
    }
    
    void addRecoveryStrategy(std::shared_ptr<IErrorRecoveryStrategy> strategy) {
        recoveryStrategies_.push_back(strategy);
    }
    
    // Get error reporter
    std::shared_ptr<IErrorReporter> getErrorReporter() const {
        return errorReporter_;
    }
    
    void setErrorReporter(std::shared_ptr<IErrorReporter> reporter) {
        errorReporter_ = reporter;
    }
    
private:
    void addContext(std::exception& error, const ErrorContext& context) {
        // Implementation would add context to error if it's an NLMException
        NLMException* nlmEx = dynamic_cast<NLMException*>(&error);
        if (nlmEx) {
            nlmEx->addContext("Context: " + context.component + " - " + context.operation);
            if (!context.suggestedAction.empty()) {
                nlmEx->addRecoveryStep(context.suggestedAction);
            }
        }
    }
};

// Component-specific error handling strategies
class DefaultErrorHandlingStrategy : public IErrorHandlingStrategy {
public:
    bool canHandle(const std::exception& error) const override {
        return true; // Handle all errors by default
    }
    
    std::exception handleError(const std::exception& error) override {
        return error; // Just return the error as-is
    }
    
    std::string getStrategyName() const override {
        return "Default Error Handler";
    }
    
    ErrorSeverity getSeverity() const override {
        return ErrorSeverity::Error;
    }
};

class BrainErrorHandlingStrategy : public IErrorHandlingStrategy {
public:
    bool canHandle(const std::exception& error) const override {
        // Handle brain-related errors
        const NLMException* nlmEx = dynamic_cast<const NLMException*>(&error);
        return nlmEx && (nlmEx->getErrorCode().find("BRAIN") != std::string::npos ||
                         nlmEx->getErrorCode().find("NEURON") != std::string::npos ||
                         nlmEx->getErrorCode().find("SYNAPS") != std::string::npos);
    }
    
    std::exception handleError(const std::exception& error) override {
        // Enhanced error handling for brain components
        std::ostringstream oss;
        oss << "Brain System Error: " << error.what();
        oss << "\nPossible causes: Network instability, Invalid neuron/synapse configuration, "
            << "Memory allocation failure, Plasticity rule violations";
        oss << "\nSuggested actions: Check brain configuration, Reset brain state, "
            << "Reduce neural population, Review plasticity parameters";
        
        return NLMException(oss.str(), ErrorSeverity::Error, "BRAIN_001", "Brain system error");
    }
    
    std::string getStrategyName() const override {
        return "Brain Error Handler";
    }
    
    ErrorSeverity getSeverity() const override {
        return ErrorSeverity::Error;
    }
};

class MemoryErrorHandlingStrategy : public IErrorHandlingStrategy {
public:
    bool canHandle(const std::exception& error) const override {
        // Handle memory-related errors
        const NLMException* nlmEx = dynamic_cast<const NLMException*>(&error);
        return nlmEx && (nlmEx->getErrorCode().find("MEM") != std::string::npos ||
                         nlmEx->getErrorCode().find("MEMORY") != std::string::npos);
    }
    
    std::exception handleError(const std::exception& error) override {
        std::ostringstream oss;
        oss << "Memory System Error: " << error.what();
        oss << "\nPossible causes: Memory fragmentation, Insufficient system resources, "
            << "Memory pool exhaustion, Invalid memory operations";
        oss << "\nSuggested actions: Clear memory pools, Reduce working memory capacity, "
            << "Optimize memory allocation, Restart simulation";
        
        return NLMException(oss.str(), ErrorSeverity::Critical, "MEM_001", "Memory system error");
    }
    
    std::string getStrategyName() const override {
        return "Memory Error Handler";
    }
    
    ErrorSeverity getSeverity() const override {
        return ErrorSeverity::Critical;
    }
};

class ConfigErrorHandlingStrategy : public IErrorHandlingStrategy {
public:
    bool canHandle(const std::exception& error) const override {
        // Handle configuration-related errors
        const NLMException* nlmEx = dynamic_cast<const NLMException*>(&error);
        return nlmEx && (nlmEx->getErrorCode().find("CFG") != std::string::npos ||
                         nlmEx->getErrorCode().find("CONFIG") != std::string::npos);
    }
    
    std::exception handleError(const std::exception& error) override {
        std::ostringstream oss;
        oss << "Configuration Error: " << error.what();
        oss << "\nPossible causes: Invalid parameter values, Missing required fields, "
            << "Schema validation failures, File format errors";
        oss << "\nSuggested actions: Check configuration file syntax, Use default values, "
            << "Fix parameter ranges, Adjust schema rules";
        
        return NLMException(oss.str(), ErrorSeverity::Warning, "CFG_001", "Configuration error");
    }
    
    std::string getStrategyName() const override {
        return "Configuration Error Handler";
    }
    
    ErrorSeverity getSeverity() const override {
        return ErrorSeverity::Warning;
    }
};

// Component-specific error recovery strategies
class AutomaticRecoveryStrategy : public IErrorRecoveryStrategy {
public:
    bool canRecover(const std::exception& error) const override {
        // Can recover from non-critical errors
        ErrorSeverity severity = ErrorUtils::getSeverity(error);
        return severity < ErrorSeverity::Critical;
    }
    
    bool recover(std::exception& error) override {
        // Attempt automatic recovery
        NLMException* nlmEx = dynamic_cast<NLMException*>(&error);
        if (!nlmEx) return false;
        
        // Try different recovery approaches based on error type
        if (nlmEx->getErrorCode().find("MEM") != std::string::npos) {
            return recoverMemoryError(*nlmEx);
        } else if (nlmEx->getErrorCode().find("BRAIN") != std::string::npos) {
            return recoverBrainError(*nlmEx);
        } else if (nlmEx->getErrorCode().find("CFG") != std::string::npos) {
            return recoverConfigError(*nlmEx);
        }
        
        return false;
    }
    
    std::string getStrategyName() const override {
        return "Automatic Recovery";
    }
    
    RecoveryResult getRecoveryResult() const override {
        return RecoveryResult::Success; // Placeholder
    }
    
private:
    bool recoverMemoryError(NLMException& error) {
        // Implementation for memory recovery
        return true;
    }
    
    bool recoverBrainError(NLMException& error) {
        // Implementation for brain recovery
        return true;
    }
    
    bool recoverConfigError(NLMException& error) {
        // Implementation for config recovery
        return true;
    }
};

class FallbackRecoveryStrategy : public IErrorRecoveryStrategy {
public:
    bool canRecover(const std::exception& error) const override {
        // Can recover from any error by falling back to safe defaults
        return true;
    }
    
    bool recover(std::exception& error) override {
        // Fall back to safe defaults
        return applySafeDefaults(error);
    }
    
    std::string getStrategyName() const override {
        return "Fallback Recovery";
    }
    
    RecoveryResult getRecoveryResult() const override {
        return RecoveryResult::Success; // Placeholder
    }
    
private:
    bool applySafeDefaults(std::exception& error) {
        // Implementation for applying safe defaults
        return true;
    }
};

class ResetRecoveryStrategy : public IErrorRecoveryStrategy {
public:
    bool canRecover(const std::exception& error) const override {
        // Can recover from recoverable errors by reset
        return true;
    }
    
    bool recover(std::exception& error) override {
        // Reset system state
        return resetSystemState(error);
    }
    
    std::string getStrategyName() const override {
        return "Reset Recovery";
    }
    
    RecoveryResult getRecoveryResult() const override {
        return RecoveryResult::Success; // Placeholder
    }
    
private:
    bool resetSystemState(std::exception& error) {
        // Implementation for resetting system state
        return true;
    }
};

// Safe default configuration with error handling
class SafeDefaultConfig {
public:
    struct SafeConfig {
        // Neural network parameters
        uint64_t neuronCount = 1000;
        uint64_t regionCount = 4;
        float connectionProbability = 0.1f;
        float connectionWeightRangeMin = 0.0f;
        float connectionWeightRangeMax = 1.0f;
        
        // Plasticity parameters
        float stdpLearningRate = 0.01f;
        float hebbianLearningRate = 0.01f;
        float structuralPlasticityRate = 0.001f;
        float eligibilityTraceDecay = 0.1f;
        
        // Development parameters
        uint32_t criticalPeriodDuration = 100;
        float maturationRate = 0.01f;
        float pruningRate = 0.05f;
        
        // Neuromodulation parameters
        float dopamineBaseline = 0.1f;
        float curiosityBaseline = 0.1f;
        float noveltyBaseline = 0.1f;
        float predictionErrorBaseline = 0.1f;
        
        // Memory parameters
        uint32_t workingMemoryCapacity = 1000;
        uint32_t episodicMemoryCapacity = 10000;
        uint32_t associativeMemoryCapacity = 10000;
        
        // Simulation parameters
        float timestep = 0.001f;
        float simulationTimeLimit = 1000.0f;
        uint32_t maxSpikeHistory = 100;
        
        // Error handling parameters
        ErrorSeverity errorThreshold = ErrorSeverity::Warning;
        bool enableAutoRecovery = true;
        bool enableGracefulDegradation = true;
        uint32_t recoveryAttempts = 3;
        float recoveryTimeout = 10.0f;
    };
    
    SafeDefaultConfig() {
        // Initialize safe default values
    }
    
    SafeConfig getSafeConfig() const {
        return SafeConfig();
    }
    
    // Apply safe defaults to configuration
    static void applyToConfig(std::shared_ptr<Config> config) {
        SafeConfig safe = SafeConfig();
        
        // Set safe defaults for key parameters
        config->set("neuron_count", static_cast<int64_t>(safe.neuronCount), ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(safe.regionCount), ConfigSource::Runtime);
        config->set("connection_probability", safe.connectionProbability, ConfigSource::Runtime);
        config->set("stdp_learning_rate", safe.stdpLearningRate, ConfigSource::Runtime);
        config->set("hebbian_learning_rate", safe.hebbianLearningRate, ConfigSource::Runtime);
        config->set("working_memory_capacity", static_cast<int64_t>(safe.workingMemoryCapacity), ConfigSource::Runtime);
        config->set("simulation_timestep", safe.timestep, ConfigSource::Runtime);
        config->set("error_threshold", static_cast<int64_t>(safe.errorThreshold), ConfigSource::Runtime);
        config->set("enable_auto_recovery", safe.enableAutoRecovery, ConfigSource::Runtime);
    }
};

// Error callback system
class ErrorCallbackManager {
private:
    std::vector<std::function<void(const std::exception&)>> errorCallbacks_;
    std::vector<std::function<void(const std::exception&, const ErrorContext&)>> errorContextCallbacks_;
    std::mutex mutex_;
    
public:
    // Register error callback
    void addErrorCallback(std::function<void(const std::exception&)> callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        errorCallbacks_.push_back(callback);
    }
    
    // Register error context callback
    void addErrorContextCallback(std::function<void(const std::exception&, const ErrorContext&)> callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        errorContextCallbacks_.push_back(callback);
    }
    
    // Invoke all error callbacks
    void invokeCallbacks(const std::exception& error, const ErrorContext& context = ErrorContext()) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (const auto& callback : errorCallbacks_) {
            callback(error);
        }
        
        for (const auto& callback : errorContextCallbacks_) {
            callback(error, context);
        }
    }
    
    // Remove all callbacks
    void clearCallbacks() {
        std::lock_guard<std::mutex> lock(mutex_);
        errorCallbacks_.clear();
        errorContextCallbacks_.clear();
    }
};

// Global error handler instance
inline ErrorHandler& getGlobalErrorHandler() {
    static ErrorHandler handler;
    return handler;
}

// Global error callback manager instance
inline ErrorCallbackManager& getGlobalErrorCallbackManager() {
    static ErrorCallbackManager manager;
    return manager;
}

// Convenience functions for error handling
inline void registerErrorCallback(std::function<void(const std::exception&)> callback) {
    getGlobalErrorCallbackManager().addErrorCallback(callback);
}

inline void registerErrorContextCallback(std::function<void(const std::exception&, const ErrorContext&)> callback) {
    getGlobalErrorCallbackManager().addErrorContextCallback(callback);
}

inline void logErrorWithContext(const std::string& component, const std::string& operation,
                                const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
                                const std::string& file = "", int line = 0, const std::string& function = "") {
    ErrorContext context(component, operation, file, line, function);
    getGlobalErrorHandler().handle(NLMException(message, severity), context);
}

inline void logErrorWithContext(const std::exception& error, const ErrorContext& context) {
    getGlobalErrorHandler().handle(error, context);
}

} // namespace nlm
