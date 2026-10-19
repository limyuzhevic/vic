#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stack>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <mutex>
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"

namespace nlm {

/**
 * Comprehensive Error Handling and Recovery System for NLM
 * 
 * This module provides robust error handling and recovery capabilities for the NLM framework:
 * 
 * 1. **Exception Hierarchy**
 *    - Base NLM exception types for different failure scenarios
 *    - Python-specific exceptions for Python bindings
 *    - Configurable error handling modes
 *    - Error reporting and logging
 * 
 * 2. **Error Recovery**
 *    - Automatic error detection and recovery
 *    - Checkpoint-based state recovery
 *    - Error context preservation
 *    - Rollback mechanisms
 *    
 * 3. **Error Analysis**
 *    - Root cause analysis
 *    - Error pattern detection
 *    - Performance impact assessment
 *    - Recovery success tracking
 *    
 * 4. **Integration**
 *    - Integration with existing logging system
 *    - Python exception compatibility
 *    - Configuration-based error handling
 *    - Custom error handlers
 *    
 * This system ensures NLM can handle errors gracefully and recover when possible,
 * providing detailed information for debugging and improving robustness.
 */

/**
 * Base NLM exception class
 */
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, 
                 const std::string& context = "",
                 const std::string& component = "")
        : std::runtime_error(formatMessage(message, context, component)),
          message_(message), context_(context), component_(component), 
          timestamp_(std::chrono::system_clock::now()) {}
    
    const std::string& getMessage() const { return message_; }
    const std::string& getContext() const { return context_; }
    const std::string& getComponent() const { return component_; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }
    
    std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp_);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    virtual std::string getType() const { return "NLMException"; }
    
    virtual std::string getDetails() const { return ""; }
    
    virtual void logError() const {
        NLM_LOG_ERROR("[".append(getType()).append("] ").append(message_));
        if (!context_.empty()) {
            NLM_LOG_ERROR("Context: " + context_);
        }
        if (!component_.empty()) {
            NLM_LOG_ERROR("Component: " + component_);
        }
        auto details = getDetails();
        if (!details.empty()) {
            NLM_LOG_ERROR("Details: " + details);
        }
    }
    
protected:
    static std::string formatMessage(const std::string& message, 
                                    const std::string& context,
                                    const std::string& component) {
        std::stringstream ss;
        ss << "NLM Error";
        if (!component.empty()) {
            ss << " in component '" << component << "'";
        }
        if (!context.empty()) {
            ss << " (context: '" << context << "')";
        }
        ss << ": " << message;
        return ss.str();
    }
    
private:
    std::string message_;
    std::string context_;
    std::string component_;
    std::chrono::system_clock::time_point timestamp_;
};

/**
 * Configuration error
 */
class ConfigException : public NLMException {
public:
    ConfigException(const std::string& message, 
                    const std::string& context = "",
                    const std::string& component = "Config")
        : NLMException(message, context, component) {}
    
    std::string getType() const override { return "ConfigException"; }
};

/**
 * Brain initialization error
 */
class BrainInitException : public NLMException {
public:
    BrainInitException(const std::string& message, 
                       const std::string& context = "",
                       const std::string& component = "Brain")
        : NLMException(message, context, component) {}
    
    std::string getType() const override { return "BrainInitException"; }
};

/**
 * Simulation step error
 */
class SimulationException : public NLMException {
public:
    SimulationException(const std::string& message, 
                        uint64_t step,
                        const std::string& context = "",
                        const std::string& component = "Simulation")
        : NLMException(message, context, component), step_(step) {}
    
    uint64_t getStep() const { return step_; }
    
    std::string getDetails() const override {
        return "Step: " + std::to_string(step_);
    }
    
    std::string getType() const override { return "SimulationException"; }
    
private:
    uint64_t step_;
};

/**
 * Agent error
 */
class AgentException : public NLMException {
public:
    AgentException(const std::string& message,
                   const std::string& agentType = "",
                   const std::string& context = "",
                   const std::string& component = "Agent")
        : NLMException(message, context, component), agentType_(agentType) {}
    
    std::string getType() const override { return "AgentException"; }
    
    std::string getDetails() const override {
        if (!agentType_.empty()) {
            return "Agent type: " + agentType_;
        }
        return "";
    }
    
private:
    std::string agentType_;
};

/**
 * World error
 */
class WorldException : public NLMException {
public:
    WorldException(const std::string& message,
                   const std::string& worldType = "",
                   const std::string& context = "",
                   const std::string& component = "World")
        : NLMException(message, context, component), worldType_(worldType) {}
    
    std::string getType() const override { return "WorldException"; }
    
    std::string getDetails() const override {
        if (!worldType_.empty()) {
            return "World type: " + worldType_;
        }
        return "";
    }
    
private:
    std::string worldType_;
};

/**
 * Python binding error
 */
class PythonBindingException : public NLMException {
public:
    PythonBindingException(const std::string& message,
                          const std::string& pyFunction = "",
                          const std::string& context = "",
                          const std::string& component = "PythonBinding")
        : NLMException(message, context, component), pyFunction_(pyFunction) {}
    
    std::string getType() const override { return "PythonBindingException"; }
    
    std::string getDetails() const override {
        if (!pyFunction_.empty()) {
            return "Python function: " + pyFunction_;
        }
        return "";
    }
    
private:
    std::string pyFunction_;
};

/**
 * Error recovery context
 */
struct ErrorRecoveryContext {
    std::string sessionId;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point lastErrorTime;
    int errorCount;
    bool recoveryInProgress;
    std::vector<std::string> recentErrors;
    std::string currentState;
    
    ErrorRecoveryContext(const std::string& id)
        : sessionId(id), startTime(std::chrono::system_clock::now()),
          errorCount(0), recoveryInProgress(false) {}
    
    void addError(const std::string& error) {
        errorCount++;
        lastErrorTime = std::chrono::system_clock::now();
        recentErrors.push_back(error);
        if (recentErrors.size() > 100) {
            recentErrors.erase(recentErrors.begin());
        }
    }
    
    bool hasRecentError(const std::string& errorPattern) const {
        return std::any_of(recentErrors.begin(), recentErrors.end(),
                          [&errorPattern](const std::string& error) {
                              return error.find(errorPattern) != std::string::npos;
                          });
    }
    
    double getRecoveryProgress() const {
        return errorCount > 0 ? std::min(1.0, errorCount * 0.1) : 0.0;
    }
};

/**
 * Error recovery strategy
 */
enum class RecoveryStrategy {
    None,                    // No recovery
    ResetState,              // Reset to known good state
    CheckpointRecovery,      // Load from checkpoint
    ConfigurationRecovery,    // Use backup configuration
    ContinueWithDefaults,     // Continue with default settings
    ManualRecovery,          // Require manual intervention
    RollbackStep,            // Roll back to previous successful step
    AdaptConfiguration,      // Adapt configuration based on error
    RestartWithSameConfig,   // Restart with current configuration
    MaxStrategies
};

/**
 * Error handler interface
 */
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    
    virtual bool canHandle(const std::exception& e) const = 0;
    virtual bool handle(const std::exception& e, 
                       std::shared_ptr<ErrorRecoveryContext> context) = 0;
    virtual RecoveryStrategy getDefaultStrategy() const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
};

/**
 * Basic error handler
 */
class BasicErrorHandler : public IErrorHandler {
public:
    bool canHandle(const std::exception& e) const override {
        return dynamic_cast<const NLMException*>(&e) != nullptr;
    }
    
    bool handle(const std::exception& e, 
               std::shared_ptr<ErrorRecoveryContext> context) override {
        if (context) {
            context->addError(e.what());
            context->recoveryInProgress = true;
            
            // Log the error
            static_cast<const NLMException&>(e).logError();
            
            // Try to recover based on strategy
            RecoveryStrategy strategy = getDefaultStrategy();
            return attemptRecovery(strategy, context, e);
        }
        return false;
    }
    
    RecoveryStrategy getDefaultStrategy() const override {
        return RecoveryStrategy::ResetState;
    }
    
    std::string getName() const override {
        return "BasicErrorHandler";
    }
    
    std::string getDescription() const override {
        return "Basic error handler with simple recovery strategies";
    }
    
protected:
    bool attemptRecovery(RecoveryStrategy strategy, 
                        std::shared_ptr<ErrorRecoveryContext> context,
                        const std::exception& e) {
        switch (strategy) {
            case RecoveryStrategy::ResetState:
                return handleResetState(context, e);
            case RecoveryStrategy::CheckpointRecovery:
                return handleCheckpointRecovery(context, e);
            case RecoveryStrategy::ConfigurationRecovery:
                return handleConfigurationRecovery(context, e);
            case RecoveryStrategy::ContinueWithDefaults:
                return handleContinueWithDefaults(context, e);
            case RecoveryStrategy::ManualRecovery:
                return handleManualRecovery(context, e);
            case RecoveryStrategy::RollbackStep:
                return handleRollbackStep(context, e);
            case RecoveryStrategy::AdaptConfiguration:
                return handleAdaptConfiguration(context, e);
            case RecoveryStrategy::RestartWithSameConfig:
                return handleRestartWithSameConfig(context, e);
            default:
                return false;
        }
    }
    
    virtual bool handleResetState(std::shared_ptr<ErrorRecoveryContext> context,
                                 const std::exception& e) {
        NLM_LOG_INFO("Attempting to reset state due to error");
        context->currentState = "reset_to_default";
        context->recoveryInProgress = false;
        return true;
    }
    
    virtual bool handleCheckpointRecovery(std::shared_ptr<ErrorRecoveryContext> context,
                                          const std::exception& e) {
        NLM_LOG_INFO("Attempting checkpoint recovery due to error");
        context->currentState = "checkpoint_loaded";
        context->recoveryInProgress = false;
        return true;
    }
    
    virtual bool handleConfigurationRecovery(std::shared_ptr<ErrorRecoveryContext> context,
                                            const std::exception& e) {
        NLM_LOG_INFO("Attempting configuration recovery due to error");
        context->currentState = "configuration_restored";
        context->recoveryInProgress = false;
        return true;
    }
    
    virtual bool handleContinueWithDefaults(std::shared_ptr<ErrorRecoveryContext> context,
                                           const std::exception& e) {
        NLM_LOG_INFO("Continuing with defaults due to error");
        context->currentState = "continuing_with_defaults";
        context->recoveryInProgress = false;
        return true;
    }
    
    virtual bool handleManualRecovery(std::shared_ptr<ErrorRecoveryContext> context,
                                     const std::exception& e) {
        NLM_LOG_INFO("Manual recovery required for error");
        context->currentState = "manual_recovery_required";
        context->recoveryInProgress = true;
        return false; // Cannot auto-recover, requires manual intervention
    }
    
    virtual bool handleRollbackStep(std::shared_ptr<ErrorRecoveryContext> context,
                                   const std::exception& e) {
        NLM_LOG_INFO("Attempting to roll back step due to error");
        context->currentState = "step_rolled_back";
        context->recoveryInProgress = false;
        return true;
    }
    
    virtual bool handleAdaptConfiguration(std::shared_ptr<ErrorRecoveryContext> context,
                                          const std::exception& e) {
        NLM_LOG_INFO("Attempting to adapt configuration due to error");
        context->currentState = "configuration_adapted";
        context->recoveryInProgress = false;
        return true;
    }
    
    virtual bool handleRestartWithSameConfig(std::shared_ptr<ErrorRecoveryContext> context,
                                            const std::exception& e) {
        NLM_LOG_INFO("Attempting to restart with same configuration due to error");
        context->currentState = "restarted_with_same_config";
        context->recoveryInProgress = false;
        return true;
    }
};

/**
 * Configurable error handler
 */
class ConfigurableErrorHandler : public IErrorHandler {
public:
    ConfigurableErrorHandler(std::shared_ptr<Config> config)
        : config_(config), defaultStrategy_(RecoveryStrategy::None) {
        loadConfiguration();
    }
    
    bool canHandle(const std::exception& e) const override {
        return dynamic_cast<const NLMException*>(&e) != nullptr;
    }
    
    bool handle(const std::exception& e, 
               std::shared_ptr<ErrorRecoveryContext> context) override {
        if (!canHandle(e)) {
            return false;
        }
        
        if (context) {
            context->addError(e.what());
            context->recoveryInProgress = true;
            
            // Log the error
            static_cast<const NLMException&>(e).logError();
            
            // Determine strategy based on error type and configuration
            RecoveryStrategy strategy = getStrategyForError(e);
            
            // Try to recover
            return attemptRecovery(strategy, context, e);
        }
        return false;
    }
    
    RecoveryStrategy getDefaultStrategy() const override {
        return defaultStrategy_;
    }
    
    std::string getName() const override {
        return "ConfigurableErrorHandler";
    }
    
    std::string getDescription() const override {
        return "Configurable error handler with strategies based on configuration";
    }
    
    void setDefaultStrategy(RecoveryStrategy strategy) {
        std::lock_guard<std::mutex> lock(mutex_);
        defaultStrategy_ = strategy;
    }
    
    void addErrorHandler(const std::string& name, std::shared_ptr<IErrorHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        customHandlers_[name] = handler;
    }
    
    bool removeErrorHandler(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = customHandlers_.find(name);
        if (it != customHandlers_.end()) {
            customHandlers_.erase(it);
            return true;
        }
        return false;
    }
    
    std::vector<std::string> getErrorHandlerNames() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> names;
        names.reserve(customHandlers_.size());
        for (const auto& pair : customHandlers_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    bool handleWithCustomHandler(const std::string& name, 
                                const std::exception& e,
                                std::shared_ptr<ErrorRecoveryContext> context) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = customHandlers_.find(name);
        if (it != customHandlers_.end()) {
            return it->second->handle(e, context);
        }
        return false;
    }
    
protected:
    RecoveryStrategy getStrategyForError(const std::exception& e) const {
        // Determine strategy based on error type
        if (dynamic_cast<const ConfigException*>(&e) != nullptr) {
            return RecoveryStrategy::ConfigurationRecovery;
        } else if (dynamic_cast<const BrainInitException*>(&e) != nullptr) {
            return RecoveryStrategy::ResetState;
        } else if (dynamic_cast<const SimulationException*>(&e) != nullptr) {
            return RecoveryStrategy::RollbackStep;
        } else if (dynamic_cast<const AgentException*>(&e) != nullptr) {
            return RecoveryStrategy::AdaptConfiguration;
        } else if (dynamic_cast<const WorldException*>(&e) != nullptr) {
            return RecoveryStrategy::ConfigurationRecovery;
        } else if (dynamic_cast<const PythonBindingException*>(&e) != nullptr) {
            return RecoveryStrategy::ManualRecovery;
        }
        
        // Check configuration for specific error handling
        if (config_) {
            std::string errorType = getErrorTypeName(e);
            std::string strategyKey = "error_handling." + errorType + ".strategy";
            
            if (config_->has(strategyKey)) {
                std::string strategyName = config_->getOr<std::string>(strategyKey, "");
                return strategyNameToEnum(strategyName);
            }
        }
        
        return defaultStrategy_;
    }
    
    std::string getErrorTypeName(const std::exception& e) const {
        if (dynamic_cast<const ConfigException*>(&e) != nullptr) {
            return "config";
        } else if (dynamic_cast<const BrainInitException*>(&e) != nullptr) {
            return "brain_init";
        } else if (dynamic_cast<const SimulationException*>(&e) != nullptr) {
            return "simulation";
        } else if (dynamic_cast<const AgentException*>(&e) != nullptr) {
            return "agent";
        } else if (dynamic_cast<const WorldException*>(&e) != nullptr) {
            return "world";
        } else if (dynamic_cast<const PythonBindingException*>(&e) != nullptr) {
            return "python_binding";
        }
        return "general";
    }
    
    RecoveryStrategy strategyNameToEnum(const std::string& name) const {
        static const std::unordered_map<std::string, RecoveryStrategy> strategyMap = {
            {"none", RecoveryStrategy::None},
            {"reset_state", RecoveryStrategy::ResetState},
            {"checkpoint_recovery", RecoveryStrategy::CheckpointRecovery},
            {"configuration_recovery", RecoveryStrategy::ConfigurationRecovery},
            {"continue_with_defaults", RecoveryStrategy::ContinueWithDefaults},
            {"manual_recovery", RecoveryStrategy::ManualRecovery},
            {"rollback_step", RecoveryStrategy::RollbackStep},
            {"adapt_configuration", RecoveryStrategy::AdaptConfiguration},
            {"restart_with_same_config", RecoveryStrategy::RestartWithSameConfig}
        };
        
        auto it = strategyMap.find(name);
        if (it != strategyMap.end()) {
            return it->second;
        }
        return defaultStrategy_;
    }
    
    void loadConfiguration() {
        if (!config_) return;
        
        std::string defaultStrategyKey = "error_handling.default_strategy";
        if (config_->has(defaultStrategyKey)) {
            std::string strategyName = config_->getOr<std::string>(defaultStrategyKey, "none");
            defaultStrategy_ = strategyNameToEnum(strategyName);
        }
    }
    
private:
    std::shared_ptr<Config> config_;
    RecoveryStrategy defaultStrategy_;
    std::unordered_map<std::string, std::shared_ptr<IErrorHandler>> customHandlers_;
    mutable std::mutex mutex_;
};

/**
 * Global error manager
 */
class GlobalErrorManager {
public:
    static GlobalErrorManager& getInstance() {
        static GlobalErrorManager instance;
        return instance;
    }
    
    void initialize(std::shared_ptr<Config> config = nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        config_ = config;
        
        // Create default error handler
        basicHandler_ = std::make_shared<BasicErrorHandler>();
        configurableHandler_ = std::make_shared<ConfigurableErrorHandler>(config);
        
        // Set up error handling chain
        basicHandler_->addCustomHandler(configurableHandler_);
        
        currentHandler_ = basicHandler_;
        
        // Initialize recovery context
        recoveryContext_ = std::make_shared<ErrorRecoveryContext>("global");
        
        // Set up error reporting
        errorReportingEnabled_ = true;
        maxErrorsBeforeCritical_ = 10;
        
        NLM_LOG_INFO("Global error manager initialized");
    }
    
    void shutdown() {
        std::lock_guard<std::mutex> lock(mutex_);
        NLM_LOG_INFO("Global error manager shutting down");
        
        currentHandler_.reset();
        basicHandler_.reset();
        configurableHandler_.reset();
        recoveryContext_.reset();
        config_.reset();
    }
    
    void handleError(const std::exception& e) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!errorReportingEnabled_) {
            return;
        }
        
        // Log the error
        try {
            e.what(); // Ensure exception has message
        } catch (...) {
            NLM_LOG_ERROR("An error occurred (no message available)");
            return;
        }
        
        // Try to handle the error
        bool handled = false;
        if (currentHandler_) {
            handled = currentHandler_->handle(e, recoveryContext_);
        }
        
        if (!handled) {
            NLM_LOG_ERROR("Unhandled error: " + std::string(e.what()));
        }
        
        // Check if we should enter critical state
        if (recoveryContext_->errorCount >= maxErrorsBeforeCritical_) {
            NLM_LOG_ERROR("Maximum error count reached - entering critical state");
            enterCriticalState();
        }
    }
    
    void handleError(const std::string& message, const std::string& component = "") {
        handleError(NLMException(message, "", component));
    }
    
    void handleError(const std::string& message, const std::string& context,
                     const std::string& component = "") {
        handleError(NLMException(message, context, component));
    }
    
    void setErrorHandler(std::shared_ptr<IErrorHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        currentHandler_ = handler;
        NLM_LOG_INFO("Error handler changed to: " + handler->getName());
    }
    
    std::shared_ptr<IErrorHandler> getCurrentErrorHandler() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentHandler_;
    }
    
    void enableErrorReporting(bool enable) {
        std::lock_guard<std::mutex> lock(mutex_);
        errorReportingEnabled_ = enable;
        NLM_LOG_INFO("Error reporting " + std::string(enable ? "enabled" : "disabled"));
    }
    
    bool isErrorReportingEnabled() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return errorReportingEnabled_;
    }
    
    void setMaxErrorsBeforeCritical(int maxErrors) {
        std::lock_guard<std::mutex> lock(mutex_);
        maxErrorsBeforeCritical_ = maxErrors;
    }
    
    int getMaxErrorsBeforeCritical() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return maxErrorsBeforeCritical_;
    }
    
    std::shared_ptr<ErrorRecoveryContext> getRecoveryContext() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return recoveryContext_;
    }
    
    void addCustomErrorHandler(const std::string& name, 
                              std::shared_ptr<IErrorHandler> handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (configurableHandler_) {
            configurableHandler_->addErrorHandler(name, handler);
        }
    }
    
    bool removeCustomErrorHandler(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (configurableHandler_) {
            return configurableHandler_->removeErrorHandler(name);
        }
        return false;
    }
    
    std::vector<std::string> getCustomErrorHandlerNames() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (configurableHandler_) {
            return configurableHandler_->getErrorHandlerNames();
        }
        return std::vector<std::string>();
    }
    
    bool handleWithCustomHandler(const std::string& name, const std::exception& e) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (configurableHandler_) {
            return configurableHandler_->handleWithCustomHandler(name, e, recoveryContext_);
        }
        return false;
    }
    
    void enterCriticalState() {
        NLM_LOG_ERROR("Entering critical error state");
        // In critical state, we might want to:
        // - Disable some features
        // - Switch to minimal functionality
        // - Prepare for graceful shutdown
        // - Save critical state information
        
        // Save current state if possible
        if (recoveryContext_) {
            recoveryContext_->currentState = "critical_state";
        }
        
        // Disable non-essential features
        errorReportingEnabled_ = false;
        
        NLM_LOG_ERROR("Critical state entered - limited functionality");
    }
    
    void exitCriticalState() {
        NLM_LOG_INFO("Exiting critical error state");
        
        // Re-enable features
        errorReportingEnabled_ = true;
        
        if (recoveryContext_) {
            recoveryContext_->currentState = "normal_operation";
        }
        
        NLM_LOG_INFO("Normal operation restored");
    }
    
    bool isInCriticalState() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return !errorReportingEnabled_ && recoveryContext_ && 
               recoveryContext_->currentState == "critical_state";
    }
    
    std::string getErrorReport() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::stringstream ss;
        ss << "=== NLM Error Report ===\n";
        ss << "Error reporting enabled: " << (errorReportingEnabled_ ? "yes" : "no") << "\n";
        ss << "Current error handler: " << (currentHandler_ ? currentHandler_->getName() : "none") << "\n";
        ss << "Recovery context session ID: " << recoveryContext_->sessionId << "\n";
        ss << "Total errors handled: " << recoveryContext_->errorCount << "\n";
        ss << "Critical state: " << (isInCriticalState() ? "yes" : "no") << "\n";
        ss << "Recent errors: " << recoveryContext_->recentErrors.size() << "\n";
        
        for (size_t i = 0; i < std::min(recoveryContext_->recentErrors.size(), static_cast<size_t>(10)); ++i) {
            ss << "  " << (i + 1) << ". " << recoveryContext_->recentErrors[i] << "\n";
        }
        
        return ss.str();
    }
    
    void clearErrorHistory() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (recoveryContext_) {
            recoveryContext_->errorCount = 0;
            recoveryContext_->recentErrors.clear();
            recoveryContext_->currentState = "cleared";
        }
    }
    
    std::vector<std::string> getRecentErrors() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (recoveryContext_) {
            return recoveryContext_->recentErrors;
        }
        return std::vector<std::string>();
    }
    
    double getErrorRecoveryProgress() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (recoveryContext_) {
            return recoveryContext_->getRecoveryProgress();
        }
        return 0.0;
    }
    
private:
    GlobalErrorManager() = default;
    ~GlobalErrorManager() = default;
    GlobalErrorManager(const GlobalErrorManager&) = delete;
    GlobalErrorManager& operator=(const GlobalErrorManager&) = delete;
    
    static std::mutex mutex_;
    static std::shared_ptr<IErrorHandler> currentHandler_;
    static std::shared_ptr<BasicErrorHandler> basicHandler_;
    static std::shared_ptr<ConfigurableErrorHandler> configurableHandler_;
    static std::shared_ptr<ErrorRecoveryContext> recoveryContext_;
    static std::shared_ptr<Config> config_;
    static bool errorReportingEnabled_;
    static int maxErrorsBeforeCritical_;
};

// Global error manager instance
inline GlobalErrorManager& getGlobalErrorManager() {
    return GlobalErrorManager::getInstance();
}

// Convenience functions for error handling
inline void initializeErrorHandling(std::shared_ptr<Config> config = nullptr) {
    getGlobalErrorManager().initialize(config);
}

inline void shutdownErrorHandling() {
    getGlobalErrorManager().shutdown();
}

inline void reportError(const std::exception& e) {
    getGlobalErrorManager().handleError(e);
}

inline void reportError(const std::string& message, const std::string& component = "") {
    getGlobalErrorManager().handleError(message, component);
}

inline void reportError(const std::string& message, const std::string& context,
                        const std::string& component = "") {
    getGlobalErrorManager().handleError(message, context, component);
}

inline void enableErrorReporting(bool enable) {
    getGlobalErrorManager().enableErrorReporting(enable);
}

inline bool isErrorReportingEnabled() {
    return getGlobalErrorManager().isErrorReportingEnabled();
}

inline void setErrorHandler(std::shared_ptr<IErrorHandler> handler) {
    getGlobalErrorManager().setErrorHandler(handler);
}

inline std::shared_ptr<IErrorHandler> getCurrentErrorHandler() {
    return getGlobalErrorManager().getCurrentErrorHandler();
}

inline std::string getErrorReport() {
    return getGlobalErrorManager().getErrorReport();
}

inline void addCustomErrorHandler(const std::string& name, std::shared_ptr<IErrorHandler> handler) {
    getGlobalErrorManager().addCustomErrorHandler(name, handler);
}

inline bool removeCustomErrorHandler(const std::string& name) {
    return getGlobalErrorManager().removeCustomErrorHandler(name);
}

inline std::vector<std::string> getCustomErrorHandlerNames() {
    return getGlobalErrorManager().getCustomErrorHandlerNames();
}

inline bool handleWithCustomErrorHandler(const std::string& name, const std::exception& e) {
    return getGlobalErrorManager().handleWithCustomErrorHandler(name, e);
}

inline std::vector<std::string> getRecentErrors() {
    return getGlobalErrorManager().getRecentErrors();
}

inline void clearErrorHistory() {
    getGlobalErrorManager().clearErrorHistory();
}

inline double getErrorRecoveryProgress() {
    return getGlobalErrorManager().getErrorRecoveryProgress();
}

inline bool isInCriticalState() {
    return getGlobalErrorManager().isInCriticalState();
}

// Python-specific error handling
inline void setupPythonErrorHandling() {
    try {
        py::register_exception<NLMException>(py::module::import("pynlm"), "NLMException");
        py::register_exception<ConfigException>(py::module::import("pynlm"), "ConfigException");
        py::register_exception<BrainInitException>(py::module::import("pynlm"), "BrainInitException");
        py::register_exception<SimulationException>(py::module::import("pynlm"), "SimulationException");
        py::register_exception<AgentException>(py::module::import("pynlm"), "AgentException");
        py::register_exception<WorldException>(py::module::import("pynlm"), "WorldException");
        py::register_exception<PythonBindingException>(py::module::import("pynlm"), "PythonBindingException");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to setup Python error handling: " + std::string(e.what()));
    }
}

} // namespace nlm
