# Enhanced error handling system for NLM
#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include <map>
#include <memory>

namespace nlm {

// Error categories for better error classification
enum class ErrorCategory {
    None,
    Configuration,
    Memory,
    Simulation,
    Plasticity,
    Neuromodulation,
    MemorySystem,
    Prediction,
    Cognition,
    Development,
    IO,
    Network,
    Performance,
    Internal
};

// Enhanced error class with detailed context
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, 
             ErrorCategory category = ErrorCategory::Internal,
             const std::string& context = "")
        : std::runtime_error(formatMessage(message, context)),
          category_(category),
          context_(context),
          timestamp_(getCurrentTimestamp()) {}
    
    ErrorCategory getCategory() const { return category_; }
    const std::string& getContext() const { return context_; }
    const std::string& getTimestamp() const { return timestamp_; }
    
    // Add recovery suggestions
    void addRecoverySuggestion(const std::string& suggestion) {
        recoverySuggestions_.push_back(suggestion);
    }
    
    const std::vector<std::string>& getRecoverySuggestions() const { 
        return recoverySuggestions_; 
    }
    
    // Check if error is recoverable
    bool isRecoverable() const {
        return !recoverySuggestions_.empty() && 
               category_ != ErrorCategory::Internal;
    }
    
    // Convert to string with full details
    std::string toString() const {
        std::string result = "NLM Error: " + what();
        if (!context_.empty()) {
            result += "\nContext: " + context_;
        }
        if (!recoverySuggestions_.empty()) {
            result += "\nRecovery Suggestions:";
            for (const auto& suggestion : recoverySuggestions_) {
                result += "\n  - " + suggestion;
            }
        }
        return result;
    }
    
protected:
    static std::string formatMessage(const std::string& message, const std::string& context) {
        std::string result = message;
        if (!context.empty()) {
            result += " (Context: " + context + ")";
        }
        return result;
    }
    
    static std::string getCurrentTimestamp() {
        // Simple timestamp implementation
        return "[" + std::to_string(0) + "]";  // Placeholder
    }
    
private:
    ErrorCategory category_;
    std::string context_;
    std::string timestamp_;
    std::vector<std::string> recoverySuggestions_;
};

// Specialized error classes for common scenarios
class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message, const std::string& context = "")
        : NLMError(message, ErrorCategory::Configuration, context) {
        addRecoverySuggestion("Check configuration file syntax");
        addRecoverySuggestion("Verify required configuration keys exist");
        addRecoverySuggestion("Use default configuration and override specific values");
    }
};

class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message, const std::string& context = "")
        : NLMError(message, ErrorCategory::Memory, context) {
        addRecoverySuggestion("Reduce simulation size or increase available memory");
        addRecoverySuggestion("Enable memory pooling and check memory usage");
        addRecoverySuggestion("Run with memory-efficient settings");
    }
};

class SimulationError : public NLMError {
public:
    SimulationError(const std::string& message, const std::string& context = "")
        : NLMError(message, ErrorCategory::Simulation, context) {
        addRecoverySuggestion("Check simulation parameters");
        addRecoverySuggestion("Verify neuron and synapse initialization");
        addRecoverySuggestion("Try shorter simulation steps");
    }
};

class PlasticityError : public NLMError {
public:
    PlasticityError(const std::string& message, const std::string& context = "")
        : NLMError(message, ErrorCategory::Plasticity, context) {
        addRecoverySuggestion("Check plasticity rule parameters");
        addRecoverySuggestion("Verify STDP and Hebbian learning settings");
        addRecoverySuggestion("Try disabling plasticity to isolate the issue");
    }
};

class MemorySystemError : public NLMError {
public:
    MemorySystemError(const std::string& message, const std::string& context = "")
        : NLMError(message, ErrorCategory::MemorySystem, context) {
        addRecoverySuggestion("Check memory capacity settings");
        addRecoverySuggestion("Verify memory consolidation parameters");
        addRecoverySuggestion("Try reducing memory retention time");
    }
};

class PredictionError : public NLMError {
public:
    PredictionError(const std::string& message, const std::string& context = "")
        : NLMError(message, ErrorCategory::Prediction, context) {
        addRecoverySuggestion("Check prediction system configuration");
        addRecoverySuggestion("Verify prediction error calculation");
        addRecoverySuggestion("Try disabling prediction for testing");
    }
};

class CognitionError : public NLMError {
public:
    CognitionError(const std::string& message, const std::string& context = "")
        : NLMError(message, ErrorCategory::Cognition, context) {
        addRecoverySuggestion("Check planner and concept formation settings");
        addRecoverySuggestion("Verify attention system parameters");
        addRecoverySuggestion("Try simplifying cognitive models");
    }
};

class DevelopmentError : public NLMError {
public:
    DevelopmentError(const std::string& message, const std::string& context = "")
        : NLMError(message, ErrorCategory::Development, context) {
        addRecoverySuggestion("Check developmental stage parameters");
        addRecoverySuggestion("Verify synaptogenesis and pruning rates");
        addRecoverySuggestion("Try disabling development to isolate the issue");
    }
};

class IOError : public NLMError {
public:
    IOError(const std::string& message, const std::string& context = "")
        : NLMError(message, ErrorCategory::IO, context) {
        addRecoverySuggestion("Check file permissions");
        addRecoverySuggestion("Verify file paths and names");
        addRecoverySuggestion("Try using absolute paths");
    }
};

// Error reporting and logging utilities
class ErrorReporter {
public:
    static void reportError(const NLMError& error, bool throwException = true);
    
    static void reportWarning(const std::string& message, 
                              ErrorCategory category = ErrorCategory::None,
                              const std::string& context = "");
    
    static void clearErrorHistory();
    
    static std::vector<std::string> getErrorHistory() { return errorHistory_; }
    
    static size_t getErrorCount(ErrorCategory category = ErrorCategory::None) {
        if (category == ErrorCategory::None) {
            return errorHistory_.size();
        }
        return errorCounts_[category];
    }
    
    static void setMaxErrorsPerCategory(size_t maxErrors) {
        maxErrorsPerCategory_ = maxErrors;
    }
    
    static bool shouldReport(ErrorCategory category) {
        return errorCounts_[category] < maxErrorsPerCategory_;
    }
    
private:
    static std::vector<std::string> errorHistory_;
    static std::map<ErrorCategory, size_t> errorCounts_;
    static size_t maxErrorsPerCategory_;
};

// Error recovery utilities
class ErrorRecovery {
public:
    // Attempt to recover from a specific error type
    static bool recoverFromConfigurationError(const NLMError& error);
    static bool recoverFromMemoryError(const NLMError& error);
    static bool recoverFromSimulationError(const NLMError& error);
    static bool recoverFromPlasticityError(const NLMError& error);
    static bool recoverFromMemorySystemError(const NLMError& error);
    static bool recoverFromPredictionError(const NLMError& error);
    static bool recoverFromCognitionError(const NLMError& error);
    static bool recoverFromDevelopmentError(const NLMError& error);
    static bool recoverFromIOError(const NLMError& error);
    
    // Generic error recovery
    static bool attemptRecovery(const NLMError& error) {
        switch (error.getCategory()) {
            case ErrorCategory::Configuration:
                return recoverFromConfigurationError(error);
            case ErrorCategory::Memory:
                return recoverFromMemoryError(error);
            case ErrorCategory::Simulation:
                return recoverFromSimulationError(error);
            case ErrorCategory::Plasticity:
                return recoverFromPlasticityError(error);
            case ErrorCategory::MemorySystem:
                return recoverFromMemorySystemError(error);
            case ErrorCategory::Prediction:
                return recoverFromPredictionError(error);
            case ErrorCategory::Cognition:
                return recoverFromCognitionError(error);
            case ErrorCategory::Development:
                return recoverFromDevelopmentError(error);
            case ErrorCategory::IO:
                return recoverFromIOError(error);
            default:
                return false;
        }
    }
    
    // Reset all recovered states
    static void resetAllRecoveries() {
        // Reset recovery states for all system components
    }
};

} // namespace nlm

#endif // NLM_ERROR_HANDLING_HPP
