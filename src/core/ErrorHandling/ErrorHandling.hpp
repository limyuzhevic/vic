#pragma once

/**
 * NLM Error Handling Infrastructure
 * 
 * Provides comprehensive exception hierarchy and error handling for the
 * Neural Learning Machine project. Includes specific error types for different
 * failure scenarios and mechanisms for error recovery and reporting.
 */

#include <string>
#include <vector>
#include <stdexcept>
#include <memory>

namespace nlm {

// === EXCEPTION HIERARCHY ===

/**
 * Base exception for all NLM errors
 */
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message)
        : std::runtime_error(message), timestamp(std::time(nullptr)) {}
    
    const char* what() const noexcept override {
        return message.c_str();
    }
    
    virtual const char* getTypeName() const { return "NLMError"; }
    
    std::string getFormattedMessage() const {
        return "[NLM " + std::to_string(timestamp) + "] " + message;
    }
    
protected:
    std::time_t timestamp;
    std::string message;
};

/**
 * Configuration-related errors
 */
class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message) : NLMError("Config: " + message) {}
    const char* getTypeName() const override { return "ConfigError"; }
};

/**
 * Neural system errors (neuron, synapse, brain)
 */
class NeuralError : public NLMError {
public:
    explicit NeuralError(const std::string& message) : NLMError("Neural: " + message) {}
    const char* getTypeName() const override { return "NeuralError"; }
};

/**
 * Memory system errors
 */
class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message) : NLMError("Memory: " + message) {}
    const char* getTypeName() const override { return "MemoryError"; }
};

/**
 * Neuromodulation errors
 */
class NeuromodulationError : public NLMError {
public:
    explicit NeuromodulationError(const std::string& message) : NLMError("Neuromodulation: " + message) {}
    const char* getTypeName() const override { return "NeuromodulationError"; }
};

/**
 * Plasticity errors
 */
class PlasticityError : public NLMError {
public:
    explicit PlasticityError(const std::string& message) : NLMError("Plasticity: " + message) {}
    const char* getTypeName() const override { return "PlasticityError"; }
};

/**
 * Simulation errors
 */
class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& message) : NLMError("Simulation: " + message) {}
    const char* getTypeName() const override { return "SimulationError"; }
};

/**
 * Experiment errors
 */
class ExperimentError : public NLMError {
public:
    explicit ExperimentError(const std::string& message) : NLMError("Experiment: " + message) {}
    const char* getTypeName() const override { return "ExperimentError"; }
};

/**
 * Integration test errors
 */
class IntegrationError : public NLMError {
public:
    explicit IntegrationError(const std::string& message) : NLMError("Integration: " + message) {}
    const char* getTypeName() const override { return "IntegrationError"; }
};

/**
 * Error collection for multiple errors
 */
class ErrorCollection : public NLMError {
public:
    ErrorCollection(const std::string& message) : NLMError("Multiple errors: " + message) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
    }
    
    const std::vector<std::string>& getErrors() const { return errors; }
    
    bool hasErrors() const { return !errors.empty(); }
    
    const char* getTypeName() const override { return "ErrorCollection"; }
    
private:
    std::vector<std::string> errors;
};

// === ERROR HANDLER INTERFACE ===

/**
 * Base error handler interface
 */
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    
    virtual void handleError(const NLMError& error) = 0;
    virtual void handleError(const std::string& error) = 0;
    virtual bool shouldAbortOnError(const NLMError& error) const = 0;
};

/**
 * Console error handler
 */
class ConsoleErrorHandler : public IErrorHandler {
public:
    void handleError(const NLMError& error) override {
        std::cerr << "[ERROR] " << error.getFormattedMessage() << std::endl;
    }
    
    void handleError(const std::string& error) override {
        std::cerr << "[ERROR] " << error << std::endl;
    }
    
    bool shouldAbortOnError(const NLMError& error) const override {
        // Always abort on certain error types
        if (dynamic_cast<const std::bad_alloc*>(&error)) return true;
        if (dynamic_cast<const IntegrationError*>(&error)) return false; // Allow retry
        return false;
    }
};

/**
 * Error collector (collects errors without aborting)
 */
class ErrorCollector : public IErrorHandler {
public:
    void handleError(const NLMError& error) override {
        errors.push_back(error.getFormattedMessage());
    }
    
    void handleError(const std::string& error) override {
        errors.push_back("[LOG] " + error);
    }
    
    bool shouldAbortOnError(const NLMError& error) const override {
        return false;
    }
    
    const std::vector<std::string>& getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
private:
    std::vector<std::string> errors;
};

// === ERROR HANDLING UTILITIES ===

/**
 * Wraps code in try-catch with error handling
 */
template<typename T>
T safeExecute(const std::string& operation, std::function<T()> func,
              IErrorHandler& handler, const T& defaultValue = T()) {
    try {
        return func();
    } catch (const NLMError& e) {
        handler.handleError(e);
        if (handler.shouldAbortOnError(e)) {
            throw;
        }
    } catch (const std::exception& e) {
        handler.handleError(std::string("Unexpected error in " + operation + ": " + std::string(e.what())));
        if (handler.shouldAbortOnError(NLMError("Unexpected error"))) {
            throw;
        }
    }
    return defaultValue;
}

/**
 * Safe execute without return value
 */
void safeExecute(const std::string& operation, std::function<void()> func,
                 IErrorHandler& handler);

/**
 * Error reporting utility
 */
class ErrorReporter {
public:
    static std::string formatError(const NLMError& error) {
        std::stringstream ss;
        ss << "[" << error.getTypeName() << "] " << error.getFormattedMessage();
        return ss.str();
    }
    
    static std::string formatErrors(const std::vector<std::string>& errors) {
        std::stringstream ss;
        ss << "Multiple errors encountered (" << errors.size() << "):";
        for (size_t i = 0; i < errors.size(); ++i) {
            ss << "\n  " << (i + 1) << ". " << errors[i];
        }
        return ss.str();
    }
};

} // namespace nlm
