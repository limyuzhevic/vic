// Enhanced Error Handling Framework for NLM

#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <stdexcept>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

// Error categories for better organization
enum class ErrorCategory {
    Configuration,      // Configuration loading, validation errors
    Neural,             // Neural computation, neuron/synapse errors
    Memory,            // Memory management, working/episodic memory errors
    Neuromodulation,   // Dopamine, curiosity, novelty errors
    Cognition,         // Planning, concept formation errors
    Environment,       // Sensory, motor, world simulation errors
    Performance,       // Performance, resource management errors
    Unknown            // Catch-all for other errors
};

// Error severity levels for prioritization
enum class ErrorSeverity {
    FATAL,      // Critical errors that prevent system operation
    ERROR,     // Error conditions that affect operation
    WARNING,    // Warning conditions that may lead to errors
    INFO,       // Informational messages
    DEBUG       // Debug information
};

// Exception with context and recovery information
class NLMException : public std::exception {
public:
    NLMException() = default;
    
    NLMException(ErrorCategory category, ErrorSeverity severity,
                 const std::string& message, const std:
                 string& context = std::string(),
                 const std::string& recoveryAction = std::string());
    
    // Get error information
    const std::string& getMessage() const { return message_; }
    ErrorCategory getCategory() const { return category_; }
    ErrorSeverity getSeverity() const { return severity_; }
    const std::string& getContext() const { return context_; }
    const std::string& getRecoveryAction() const { return recoveryAction_; }
    const std::string& getTimestamp() const { return timestamp_; }
    const std::string& getCorrelationId() const { return correlationId_; }
    
    // Set additional information
    void setCorrelationId(const std::string& id) { correlationId_ = id; }
    void setTimestamp(const std::string& ts) { timestamp_ = ts; }
    
    // Convert to string for logging
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    // Check if error is recoverable
    bool isRecoverable() const;
    
    // Get severity string
    static std::string severityToString(ErrorSeverity severity);
    
    // Get category string
    static std::string categoryToString(ErrorCategory category);

private:
    ErrorCategory category_ = ErrorCategory::Unknown;
    ErrorSeverity severity_ = ErrorSeverity::INFO;
    std::string message_;
    std::string context_;
    std::string recoveryAction_;
    std::string timestamp_;
    std::string correlationId_;
};

// Exception for configuration-related errors
class ConfigException : public NLMException {
public:
    ConfigException(const std::string& message, const std::string& context = "",
                    const std::string& recoveryAction = "Check configuration file syntax")
        : NLMException(ErrorCategory::Configuration, ErrorSeverity::ERROR,
                       message, context, recoveryAction) {}
};

// Exception for neural computation errors
class NeuralException : public NLMException {
public:
    NeuralException(const std::string& message, const std::string& context = "",
                    const std::string& recoveryAction = "Check neural parameters or state")
        : NLMException(ErrorCategory::Neural, ErrorSeverity::ERROR,
                       message, context, recoveryAction) {}
};

// Exception for memory-related errors
class MemoryException : public NLMException {
public:
    MemoryException(const std::string& message, const std::string& context = "",
                    const std::string& recoveryAction = "Check memory allocation or usage")
        : NLMException(ErrorCategory::Memory, ErrorSeverity::ERROR,
                       message, context, recoveryAction) {}
};

// Exception for neuromodulation errors
class NeuromodulationException : public NLMException {
public:
    NeuromodulationException(const std::string& message, const std::string& context = "",
                              const std::string& recoveryAction = "Check neuromodulator parameters")
        : NLMException(ErrorCategory::Neuromodulation, ErrorSeverity::ERROR,
                       message, context, recoveryAction) {}
};

// Exception for cognitive errors
class CognitiveException : public NLMException {
public:
    CognitiveException(const std::string& message, const std::string& context = "",
                      const std::string& recoveryAction = "Check cognitive parameters")
        : NLMException(ErrorCategory::Cognition, ErrorSeverity::ERROR,
                       message, context, recoveryAction) {}
};

// Exception for environment errors
class EnvironmentException : public NLMException {
public:
    EnvironmentException(const std::string& message, const std::string& context = "",
                        const std::string& recoveryAction = "Check sensory/motor parameters")
        : NLMException(ErrorCategory::Environment, ErrorSeverity::ERROR,
                       message, context, recoveryAction) {}
};

// Exception for performance errors
class PerformanceException : public NLMException {
public:
    PerformanceException(const std::string& message, const std::string& context = "",
                        const std::string& recoveryAction = "Check system resources")
        : NLMException(ErrorCategory::Performance, ErrorSeverity::WARNING,
                       message, context, recoveryAction) {}
};