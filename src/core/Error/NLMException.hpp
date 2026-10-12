// Error handling system for NLM
// Provides structured error handling with detailed information and recovery mechanisms

#pragma once

#include <string>
#include <stdexcept>
#include <system_error>
#include <memory>
#include <chrono>
#include <sstream>
#include <vector>

namespace nlm {

// Error categories for NLM
enum class ErrorCategory {
    Configuration,     // Configuration-related errors
    Memory,            // Memory allocation and management errors
    Neural,            // Neural computation errors
    Plasticity,        // Plasticity rule errors
    Neuromodulation,   // Neuromodulation errors
    Developmental,     // Development system errors
    I/O,               // File and input/output errors
    Integration,       // Component integration errors
    Simulation,        // Simulation control errors
    Resource,          // Resource management errors
    Validation         // Data validation errors
};

// Exception severity levels
enum class ErrorSeverity {
    Warning,           // Non-critical issue that may affect results
    Error,             // Serious issue that affects functionality
    Critical,          // Critical failure that prevents operation
    Fatal              // Unrecoverable failure
};

// Base NLM exception class
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message,
                 ErrorCategory category,
                 ErrorSeverity severity = ErrorSeverity::Error,
                 const std::string& component = "",
                 const std::string& file = "",
                 int line = 0,
                 const std::string& function = "");

    // Get error information
    const std::string& getMessage() const { return message_; }
    ErrorCategory getCategory() const { return category_; }
    ErrorSeverity getSeverity() const { return severity_; }
    const std::string& getComponent() const { return component_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }

    // Get formatted error string
    std::string getFormattedError() const;

    // Convert to string for backward compatibility
    const char* what() const noexcept override { return getFormattedError().c_str(); }

protected:
    std::string message_;
    ErrorCategory category_;
    ErrorSeverity severity_;
    std::string component_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
};

// Specific NLM exception types
class ConfigurationException : public NLMException {
public:
    ConfigurationException(const std::string& message,
                           const std::string& key = "",
                           const std::string& component = "",
                           const std::string& file = "",
                           int line = 0,
                           const std::string& function = "");
    const std::string& getKey() const { return key_; }
private:
    std::string key_;
};

class MemoryException : public NLMException {
public:
    MemoryException(const std::string& message,
                    size_t sizeRequested = 0,
                    const std::string& component = "",
                    const std::string& file = "",
                    int line = 0,
                    const std::string& function = "");
    size_t getSizeRequested() const { return sizeRequested_; }
private:
    size_t sizeRequested_;
};

class NeuralException : public NLMException {
public:
    NeuralException(const std::string& message,
                    NeuronId neuronId = INVALID_NEURON_ID,
                    const std::string& component = "",
                    const std::string& file = "",
                    int line = 0,
                    const std::string& function = "");
    NeuronId getNeuronId() const { return neuronId_; }
private:
    NeuronId neuronId_;
};

class PlasticityException : public NLMException {
public:
    PlasticityException(const std::string& message,
                       const std::string& rule = "",
                       const std::string& component = "",
                       const std::string& file = "",
                       int line = 0,
                       const std::string& function = "");
    const std::string& getRule() const { return rule_; }
private:
    std::string rule_;
};

class IOError : public NLMException {
public:
    IOError(const std::string& message,
            const std::string& filepath = "",
            IOError::Operation op = IOError::Operation::Read,
            const std::string& component = "",
            const std::string& file = "",
            int line = 0,
            const std::string& function = "");
    const std::string& getFilepath() const { return filepath_; }
    enum class Operation { Read, Write, Open, Close, Validate };
    Operation getOperation() const { return operation_; }
private:
    std::string filepath_;
    Operation operation_;
};

class IntegrationException : public NLMException {
public:
    IntegrationException(const std::string& message,
                        const std::string& component = "",
                        const std::string& file = "",
                        int line = 0,
                        const std::string& function = "");
};

// Error recovery strategies
enum class RecoveryStrategy {
    Retry,             // Retry the operation
    Fallback,          // Use fallback behavior
    Skip,              // Skip the operation
    Shutdown,          // Shut down gracefully
    Abort              // Abort the operation
};

// Error context for tracking
struct ErrorContext {
    std::chrono::system_clock::time_point timestamp;
    std::string operation;
    std::string component;
    std::string details;
    RecoveryStrategy suggestedRecovery;
    std::vector<std::string> suggestions;
    int retryCount;
    bool recoverable;
};

// Global error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    
    // Handle an error
    virtual void handleError(const NLMException& error, const ErrorContext& context) = 0;
    
    // Check if error should be handled
    virtual bool shouldHandle(const NLMException& error, const ErrorContext& context) = 0;
    
    // Get recovery action for error
    virtual RecoveryStrategy getRecoveryStrategy(const NLMException& error, const ErrorContext& context) = 0;
};

// Default error handler (logs errors)
class DefaultErrorHandler : public IErrorHandler {
public:
    void handleError(const NLMException& error, const ErrorContext& context) override;
    bool shouldHandle(const NLMException& error, const ErrorContext& context) override;
    RecoveryStrategy getRecoveryStrategy(const NLMException& error, const ErrorContext& context) override;
};

// Error reporting system
class ErrorReporter {
public:
    static ErrorReporter& getInstance();
    
    // Add error handler
    void addErrorHandler(std::shared_ptr<IErrorHandler> handler);
    
    // Remove error handler
    void removeErrorHandler(std::shared_ptr<IErrorHandler> handler);
    
    // Report an error
    void reportError(const NLMException& error, const ErrorContext& context = ErrorContext());
    
    // Set global error handler
    void setGlobalErrorHandler(std::shared_ptr<IErrorHandler> handler);
    
    // Get global error handler
    std::shared_ptr<IErrorHandler> getGlobalErrorHandler() const;
    
    // Configure error reporting
    void setLogLevel(LogLevel level);
    void setLogFile(const std::string& filepath);
    void setMaxErrors(size_t maxErrors);
    
    // Get error statistics
    size_t getTotalErrors() const;
    size_t getErrorsByCategory(ErrorCategory category) const;
    size_t getErrorsBySeverity(ErrorSeverity severity) const;
    
    // Clear error statistics
    void clearStatistics();

private:
    ErrorReporter();
    ~ErrorReporter();
    ErrorReporter(const ErrorReporter&) = delete;
    ErrorReporter& operator=(const ErrorReporter&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for error handling
#define NLM_THROW_ERROR(msg, category) \
    throw nlm::NLMException(msg, nlm::ErrorCategory::category, nlm::ErrorSeverity::Error, #component, __FILE__, __LINE__, __func__)

#define NLM_THROW_CONFIG_ERROR(msg, key) \
    throw nlm::ConfigurationException(msg, key, #component, __FILE__, __LINE__, __func__)

#define NLM_THROW_MEMORY_ERROR(msg, size) \
    throw nlm::MemoryException(msg, size, #component, __FILE__, __LINE__, __func__)

#define NLM_THROW_NEURAL_ERROR(msg, neuron) \
    throw nlm::NeuralException(msg, neuron, #component, __FILE__, __LINE__, __func__)

#define NLM_THROW_PLASTICITY_ERROR(msg, rule) \
    throw nlm::PlasticityException(msg, rule, #component, __FILE__, __LINE__, __func__)

#define NLM_THROW_IO_ERROR(msg, filepath, op) \
    throw nlm::IOError(msg, filepath, nlm::IOError::Operation::op, #component, __FILE__, __LINE__, __func__)

#define NLM_THROW_INTEGRATION_ERROR(msg) \
    throw nlm::IntegrationException(msg, #component, __FILE__, __LINE__, __func__)

// Safe execution macro (executes operation and catches errors)
#define NLM_SAFE_EXEC(operation, errorHandler) \
    try { operation; } catch (const nlm::NLMException& e) { errorHandler(e); } catch (const std::exception& e) { \
        nlm::ErrorContext context; context.operation = #operation; \
        nlm::ErrorReporter::getInstance().reportError(nlm::NLMException(std::string("Standard exception: ") + e.what(), nlm::ErrorCategory::Simulation), context); \
    }

} // namespace nlm
