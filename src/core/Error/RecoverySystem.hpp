// Error handling and recovery system for NLM
// Provides comprehensive error reporting and recovery mechanisms

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <stdexcept>
#include <sstream>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

// Error categories for better organization
enum class ErrorCategory {
    NeuralDynamics,
    Synaptic,
    Plasticity,
    Memory,
    Prediction,
    Neuromodulation,
    Development,
    Configuration,
    InputOutput,
    ResourceManagement,
    Simulation,
    Integration,
    Unknown
};

// Base exception class for NLM errors
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, ErrorSeverity severity = ErrorSeverity::Error,
             ErrorCategory category = ErrorCategory::Unknown,
             const std::string& component = "")
        : std::runtime_error(message), severity(severity), category(category), component(component) {}

    ErrorSeverity getSeverity() const { return severity; }
    ErrorCategory getCategory() const { return category; }
    const std::string& getComponent() const { return component; }

private:
    ErrorSeverity severity;
    ErrorCategory category;
    std::string component;
};

// Error with context information
class NLMErrorWithContext : public NLMError {
public:
    NLMErrorWithContext(const std::string& message, ErrorSeverity severity,
                       ErrorCategory category, const std::string& component,
                       const std::string& context)
        : NLMError(message, severity, category, component), context(context) {}

    const std::string& getContext() const { return context; }

private:
    std::string context;
};

// Error recovery action
class ErrorRecoveryAction {
public:
    virtual ~ErrorRecoveryAction() = default;
    virtual bool execute() = 0;
    virtual std::string getDescription() const = 0;
    virtual bool canRecover() const = 0;
};

// Specific recovery actions
class ResetNeuronStateAction : public ErrorRecoveryAction {
public:
    ResetNeuronStateAction(int neuronId) : neuronId(neuronId) {}
    
    bool execute() override {
        // Reset neuron to stable state
        // Implementation depends on neuron management system
        return true;
    }
    
    std::string getDescription() const override {
        return "Reset neuron state to default values";
    }
    
    bool canRecover() const override {
        return true; // Can always recover by reset
    }
    
private:
    int neuronId;
};

class FallbackComputationAction : public ErrorRecoveryAction {
public:
    FallbackComputationAction(const std::string& operation) : operation(operation) {}
    
    bool execute() override {
        // Use simplified/less accurate computation
        // Implementation depends on specific computation
        return true;
    }
    
    std::string getDescription() const override {
        return "Use fallback computation for " + operation"";
    }
    
    bool canRecover() const override {
        return true; // Can always fallback
    }
    
private:
    std::string operation;
};

class ReinitializeComponentAction : public ErrorRecoveryAction {
public:
    ReinitializeComponentAction(const std::string& componentName) : componentName(componentName) {}
    
    bool execute() override {
        // Reinitialize the component
        // Implementation depends on component system
        return true;
    }
    
    std::string getDescription() const override {
        return "Reinitialize component: " + componentName"";
    }
    
    bool canRecover() const override {
        // Check if component can be reinitialized
        return true;
    }
    
private:
    std::string componentName;
};

// Error recovery strategy
class ErrorRecoveryStrategy {
public:
    virtual ~ErrorRecoveryStrategy() = default;
    virtual bool attemptRecovery(const NLMError& error) = 0;
    virtual std::string getStrategyName() const = 0;
};

// Automatic error recovery system
class ErrorRecoverySystem {
private:
    std::unordered_map<ErrorCategory, std::vector<std::unique_ptr<ErrorRecoveryStrategy>>> strategies;
    std::vector<std::unique_ptr<ErrorRecoveryAction>> pendingActions;
    std::mutex recoveryMutex;
    bool recoveryEnabled = true;
    int maxRecoveryAttempts = 3;
    std::unordered_map<std::string, int> recoveryAttempts;
    
public:
    ErrorRecoverySystem() = default;
    
    // Add a recovery strategy for an error category
    void addRecoveryStrategy(ErrorCategory category, std::unique_ptr<ErrorRecoveryStrategy> strategy) {
        strategies[category].push_back(std::move(strategy));
    }
    
    // Register a recovery action
    void registerRecoveryAction(std::unique_ptr<ErrorRecoveryAction> action) {
        pendingActions.push_back(std::move(action));
    }
    
    // Attempt to recover from an error
    bool recoverFromError(const NLMError& error) {
        if (!recoveryEnabled) return false;
        
        std::lock_guard<std::mutex> lock(recoveryMutex);
        
        std::string key = getErrorKey(error);
        if (recoveryAttempts.find(key) == recoveryAttempts.end()) {
            recoveryAttempts[key] = 0;
        }
        
        if (recoveryAttempts[key] >= maxRecoveryAttempts) {
            return false; // Too many recovery attempts
        }
        
        recoveryAttempts[key]++;
        
        // Try category-specific strategies
        auto categoryIt = strategies.find(error.getCategory());
        if (categoryIt != strategies.end()) {
            for (auto& strategy : categoryIt->second) {
                if (strategy->attemptRecovery(error)) {
                    return true;
                }
            }
        }
        
        // Try general recovery actions
        for (auto& action : pendingActions) {
            if (action->canRecover() && action->execute()) {
                return true;
            }
        }
        
        return false;
    }
    
    // Enable/disable error recovery
    void setRecoveryEnabled(bool enabled) {
        recoveryEnabled = enabled;
    }
    
    // Set maximum recovery attempts per error type
    void setMaxRecoveryAttempts(int attempts) {
        maxRecoveryAttempts = attempts;
    }
    
    // Get recovery statistics
    std::string getRecoveryStats() const {
        std::stringstream ss;
        ss << "Error Recovery Statistics:\n";
        ss << "Recovery enabled: " << (recoveryEnabled ? "yes" : "no") << "\n";
        ss << "Max attempts per error: " << maxRecoveryAttempts << "\n";
        ss << "Pending actions: " << pendingActions.size() << "\n";
        ss << "Categories with strategies: " << strategies.size() << "\n";
        return ss.str();
    }
    
private:
    std::string getErrorKey(const NLMError& error) const {
        return error.getComponent() + "_" + std::to_string(static_cast<int>(error.getCategory()));
    }
};

// Global error recovery system instance
inline ErrorRecoverySystem& getErrorRecoverySystem() {
    static ErrorRecoverySystem system;
    return system;
}

// Error reporting and logging interface
class ErrorReporter {
private:
    std::vector<NLMError> recentErrors;
    std::mutex reportMutex;
    
public:
    void reportError(const NLMError& error) {
        std::lock_guard<std::mutex> lock(reportMutex);
        recentErrors.push_back(error);
        
        // Log the error
        std::stringstream ss;
        ss << "[" << error.getComponent() << "] " << error.what();
        
        switch (error.getSeverity()) {
            case ErrorSeverity::Debug:
                // Debug logging
                break;
            case ErrorSeverity::Info:
                // Info logging
                break;
            case ErrorSeverity::Warning:
                // Warning logging
                break;
            case ErrorSeverity::Error:
                // Error logging
                break;
            case ErrorSeverity::Critical:
                // Critical logging
                break;
        }
        
        // Attempt recovery
        getErrorRecoverySystem().recoverFromError(error);
    }
    
    std::vector<NLMError> getRecentErrors() const {
        std::lock_guard<std::mutex> lock(reportMutex);
        return recentErrors;
    }
    
    void clearRecentErrors() {
        std::lock_guard<std::mutex> lock(reportMutex);
        recentErrors.clear();
    }
    
    std::string getErrorSummary() const {
        std::stringstream ss;
        ss << "Error Summary: " << recentErrors.size() << " recent errors\n";
        
        int errorCount = 0, warningCount = 0, criticalCount = 0;
        for (const auto& error : recentErrors) {
            switch (error.getSeverity()) {
                case ErrorSeverity::Error: errorCount++; break;
                case ErrorSeverity::Warning: warningCount++; break;
                case ErrorSeverity::Critical: criticalCount++; break;
                default: break;
            }
        }
        
        ss << "  Errors: " << errorCount << "\n";
        ss << "  Warnings: " << warningCount << "\n";
        ss << "  Critical: " << criticalCount << "\n";
        
        return ss.str();
    }
};

// Global error reporter instance
inline ErrorReporter& getErrorReporter() {
    static ErrorReporter reporter;
    return reporter;
}

// Convenience error reporting macros
#define REPORT_ERROR(severity, category, component, message) \
    do { \
        nlm::NLMError error(message, severity, category, component); \
        nlm::getErrorReporter().reportError(error); \
    } while(0)

#define REPORT_ERROR_WITH_CONTEXT(severity, category, component, message, context) \
    do { \
        nlm::NLMErrorWithContext error(message, severity, category, component, context); \
        nlm::getErrorReporter().reportError(error); \
    } while(0)

// Error recovery macros
#define ATTEMPT_RECOVERY(error) \
    nlm::getErrorRecoverySystem().recoverFromError(error)

} // namespace nlm