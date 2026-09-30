#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <stdexcept>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

class Exception;
class ErrorContext;

/**
 * Base exception for all NLM exceptions with stack trace support
 */
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message), timestamp(std::chrono::system_clock::now()) {}
    virtual ~NLMException() = default;
    
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp; }
    const std::string& getMessage() const { return what(); }
    
protected:
    std::chrono::system_clock::time_point timestamp;
};

/**
 * Configuration-specific exceptions
 */
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message) : NLMException("Config: " + message) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message) : ConfigException("Validation error: " + message) {}
};

class ConfigParseException : public ConfigException {
public:
    explicit ConfigParseException(const std::string& message) : ConfigException("Parse error: " + message) {}
};

/**
 * Neural network component exceptions
 */
class NeuralException : public NLMException {
public:
    explicit NeuralException(const std::string& message) : NLMException("Neural: " + message) {}
};

class NeuronException : public NeuralException {
public:
    explicit NeuronException(const std::string& message) : NeuralException("Neuron: " + message) {}
};

class SynapseException : public NeuralException {
public:
    explicit SynapseException(const std::string& message) : NeuralException("Synapse: " + message) {}
};

/**
 * Plasticity mechanism exceptions
 */
class PlasticityException : public NeuralException {
public:
    explicit PlasticityException(const std::string& message) : NeuralException("Plasticity: " + message) {}
};

/**
 * Memory system exceptions
 */
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message) : NLMException("Memory: " + message) {}
};

/**
 * Error context for detailed error reporting
 */
struct ErrorContext {
    std::string component;
    std::string operation;
    std::string file;
    int line;
    std::map<std::string, std::string> details;
    
    ErrorContext(const std::string& comp, const std::string& op) 
        : component(comp), operation(op), file(""), line(0) {}
    
    ErrorContext& setFile(const std::string& f) { file = f; return *this; }
    ErrorContext& setLine(int l) { line = l; return *this; }
    ErrorContext& addDetail(const std::string& key, const std::string& value) { 
        details[key] = value; 
        return *this; 
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "[Error] Component: " << component << "\n";
        oss << "       Operation: " << operation << "\n";
        if (!file.empty()) oss << "       File: " << file << "\n";
        if (line > 0) oss << "       Line: " << line << "\n";
        if (!details.empty()) {
            oss << "       Details:";
            for (const auto& detail : details) {
                oss << "\n           " << detail.first << " = " << detail.second;
            }
        }
        return oss.str();
    }
};

/**
 * Centralized error handling class
 */
class ErrorHandler {
public:
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }
    
    // Register error handler callback
    using ErrorCallback = std::function<void(const ErrorContext&, const NLMException&)>;
    
    void setErrorCallback(ErrorCallback callback) { errorCallback = std::move(callback); }
    
    // Handle exceptions with context
    template<typename T>
    T handle(const ErrorContext& context, T (*operation)(), 
             std::function<void(const ErrorContext&)> onError = nullptr) {
        try {
            T result = operation();
            if (errorCallback) errorCallback(context, NLMException("Success"));
            return result;
        } catch (const NLMException& e) {
            if (errorCallback) errorCallback(context, e);
            if (onError) onError(context);
            throw;
        } catch (const std::exception& e) {
            ErrorContext errorContext = context;
            errorContext.addDetail("std_exception", e.what());
            NLMException exc(std::string("Standard exception: ") + e.what());
            if (errorCallback) errorCallback(errorContext, exc);
            if (onError) onError(context);
            throw exc;
        }
    }
    
    // Create error context
    static ErrorContext createContext(const std::string& component, const std::string& operation) {
        return ErrorContext(component, operation);
    }
    
private:
    ErrorHandler() = default;
    ErrorCallback errorCallback;
};

} // namespace nlm
