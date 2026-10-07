// NLM Error Handling System
// Comprehensive exception hierarchy for the NLM framework

#ifndef SRC_CORE_ERROR_HANDLING_H
#define SRC_CORE_ERROR_HANDLING_H

#include <string>
#include <vector>
#include <stdexcept>
#include <typeinfo>
#include <chrono>
#include <sstream>

namespace nlm {

// Error categories for different types of failures
enum class ErrorCategory {
    Configuration,      // Configuration loading, validation, parsing errors
    Neural,             // Neural computation, neuron/synapse errors
    Memory,             // Memory system errors
    Integration,        // Brain integration, system coordination errors
    Resource,           // Memory allocation, file I/O errors
    Validation,         // Input validation, parameter checking errors
    Network,            // Network communication errors
    Developmental,      // Development system errors
    Performance         // Performance-related errors
};

// Base NLM exception class
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, ErrorCategory category = ErrorCategory::Neural)
        : std::runtime_error(message), category(category), timestamp(std::chrono::system_clock::now()) {}
    
    NLMException(const std::string& message, const std::string& context, ErrorCategory category = ErrorCategory::Neural)
        : std::runtime_error(formatMessage(message, context)), category(category), timestamp(std::chrono::system_clock::now()) {}
    
    ErrorCategory getCategory() const { return category; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp; }
    const std::string& getContext() const { return context; }
    
    virtual std::string getTypeName() const { return "NLMException"; }
    
protected:
    static std::string formatMessage(const std::string& message, const std::string& context) {
        return "[" + context + "] " + message;
    }
    
private:
    ErrorCategory category;
    std::chrono::system_clock::time_point timestamp;
    std::string context;
};

// Configuration-specific exceptions
class ConfigException : public NLMException {
public:
    ConfigException(const std::string& message, const std::string& context = "Config")
        : NLMException(message, context, ErrorCategory::Configuration) {}
    
    std::string getTypeName() const override { return "ConfigException"; }
};

class ConfigParseException : public ConfigException {
public:
    ConfigParseException(const std::string& message, const std::string& filepath = "")
        : ConfigException(message + (filepath.empty() ? "" : " in file: " + filepath), "ConfigParse") {}
    
    std::string getTypeName() const override { return "ConfigParseException"; }
};

class ConfigValidationException : public ConfigException {
public:
    ConfigValidationException(const std::string& message, const std::string& key = "")
        : ConfigException(message + (key.empty() ? "" : " for key: " + key), "ConfigValidation") {}
    
    std::string getTypeName() const override { return "ConfigValidationException"; }
};

// Neural computation exceptions
class NeuralException : public NLMException {
public:
    NeuralException(const std::string& message, const std::string& context = "Neural")
        : NLMException(message, context, ErrorCategory::Neural) {}
    
    std::string getTypeName() const override { return "NeuralException"; }
};

class NeuronException : public NeuralException {
public:
    NeuronException(const std::string& message, uint64_t neuronId = 0)
        : NeuralException(message + (neuronId ? " (neuron " + std::to_string(neuronId) + ")" : ""), "Neuron") {}
    
    std::string getTypeName() const override { return "NeuronException"; }
};

class SynapseException : public NeuralException {
public:
    SynapseException(const std::string& message, uint64_t synapseId = 0)
        : NeuralException(message + (synapseId ? " (synapse " + std::to_string(synapseId) + ")" : ""), "Synapse") {}
    
    std::string getTypeName() const override { return "SynapseException"; }
};

// Memory system exceptions
class MemoryException : public NLMException {
public:
    MemoryException(const std::string& message, const std::string& context = "Memory")
        : NLMException(message, context, ErrorCategory::Memory) {}
    
    std::string getTypeName() const override { return "MemoryException"; }
};

// Integration-specific exceptions
class IntegrationException : public NLMException {
public:
    IntegrationException(const std::string& message, const std::string& context = "Integration")
        : NLMException(message, context, ErrorCategory::Integration) {}
    
    std::string getTypeName() const override { return "IntegrationException"; }
};

// Resource management exceptions
class ResourceException : public NLMException {
public:
    ResourceException(const std::string& message, const std::string& resource = "")
        : NLMException(message + (resource.empty() ? "" : " (resource: " + resource + ")"), "Resource") {}
    
    std::string getTypeName() const override { return "ResourceException"; }
};

// Development system exceptions
class DevelopmentException : public NLMException {
public:
    DevelopmentException(const std::string& message, const std::string& context = "Development")
        : NLMException(message, context, ErrorCategory::Developmental) {}
    
    std::string getTypeName() const override { return "DevelopmentException"; }
};

// Performance-related exceptions
class PerformanceException : public NLMException {
public:
    PerformanceException(const std::string& message, const std::string& context = "Performance")
        : NLMException(message, context, ErrorCategory::Performance) {}
    
    std::string getTypeName() const override { return "PerformanceException"; }
};

// Error collector for collecting multiple errors
class ErrorCollector {
public:
    ErrorCollector() = default;
    
    void add(const std::exception& error) {
        errors_.push_back(formatError(error));
    }
    
    void add(const std::string& message, ErrorCategory category = ErrorCategory::Neural) {
        errors_.push_back("{" + errorCategoryToString(category) + "} " + message);
    }
    
    bool hasErrors() const { return !errors_.empty(); }
    
    size_t errorCount() const { return errors_.size(); }
    
    const std::vector<std::string>& getErrors() const { return errors_; }
    
    std::string getSummary() const {
        if (errors_.empty()) return "No errors";
        
        std::stringstream ss;
        ss << errors_.size() << " error(s):\n";
        for (const auto& error : errors_) {
            ss << "  - " << error << "\n";
        }
        return ss.str();
    }
    
    void clear() { errors_.clear(); }
    
    void throwIfHasErrors() {
        if (hasErrors()) {
            throw IntegrationException(getSummary(), "ErrorCollector");
        }
    }
    
private:
    static std::string errorCategoryToString(ErrorCategory category) {
        switch (category) {
            case ErrorCategory::Configuration: return "CONFIG";
            case ErrorCategory::Neural: return "NEURAL";
            case ErrorCategory::Memory: return "MEMORY";
            case ErrorCategory::Integration: return "INTEGRATION";
            case ErrorCategory::Resource: return "RESOURCE";
            case ErrorCategory::Validation: return "VALIDATION";
            case ErrorCategory::Network: return "NETWORK";
            case ErrorCategory::Developmental: return "DEVELOPMENTAL";
            case ErrorCategory::Performance: return "PERFORMANCE";
            default: return "UNKNOWN";
        }
    }
    
    static std::string formatError(const std::exception& error) {
        std::stringstream ss;
        ss << "{" << typeid(error).name() << "} " << error.what();
        return ss.str();
    }
    
    std::vector<std::string> errors_;
};

// Error context for better error reporting
class ErrorContext {
public:
    ErrorContext(const std::string& operation, const std::string& component = "")
        : operation(operation), component(component), startTime(std::chrono::system_clock::now()) {}
    
    void addError(const std::exception& error) {
        errors_.push_back(formatErrorWithContext(error));
    }
    
    void addError(const std::string& message, ErrorCategory category = ErrorCategory::Neural) {
        errors_.push_back(formatErrorWithContext(message, category));
    }
    
    bool hasErrors() const { return !errors_.empty(); }
    
    std::string getContextDescription() const {
        auto endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::stringstream ss;
        ss << "Operation: " << operation;
        if (!component.empty()) {
            ss << " (component: " << component << ")";
        }
        ss << ", Duration: " << duration.count() << "ms";
        ss << ", Errors: " << errors_.size();
        
        return ss.str();
    }
    
    std::string getErrorDetails() const {
        std::stringstream ss;
        ss << "Operation: " << operation << "\n";
        if (!component.empty()) {
            ss << "Component: " << component << "\n";
        }
        ss << "Duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - startTime).count() << "ms\n";
        ss << "Errors: " << errors_.size() << "\n";
        for (const auto& error : errors_) {
            ss << "  - " << error << "\n";
        }
        return ss.str();
    }
    
    void throwIfHasErrors() {
        if (hasErrors()) {
            throw IntegrationException(getErrorDetails(), "ErrorContext");
        }
    }
    
private:
    static std::string formatErrorWithContext(const std::exception& error) {
        return "{" + typeid(error).name() + "} " + error.what();
    }
    
    static std::string formatErrorWithContext(const std::string& message, ErrorCategory category) {
        return "{" + errorCategoryToString(category) + "} " + message;
    }
    
    static std::string errorCategoryToString(ErrorCategory category) {
        switch (category) {
            case ErrorCategory::Configuration: return "CONFIG";
            case ErrorCategory::Neural: return "NEURAL";
            case ErrorCategory::Memory: return "MEMORY";
            case ErrorCategory::Integration: return "INTEGRATION";
            case ErrorCategory::Resource: return "RESOURCE";
            case ErrorCategory::Validation: return "VALIDATION";
            case ErrorCategory::Network: return "NETWORK";
            case ErrorCategory::Developmental: return "DEVELOPMENTAL";
            case ErrorCategory::Performance: return "PERFORMANCE";
            default: return "UNKNOWN";
        }
    }
    
    std::string operation;
    std::string component;
    std::chrono::system_clock::time_point startTime;
    std::vector<std::string> errors_;
};

// Macro for error handling with context
#define NLM_ERROR_HANDLER(operation, component) \
    nlm::ErrorContext errorContext(operation, component)

#define NLM_TRY_WITH_ERROR_HANDLING(errorContext, operation) \
    try { \
        errorContext.addError(operation); \
    } catch (const std::exception& e) { \
        errorContext.addError(e); \
    }

#define NLM_THROW_WITH_CONTEXT(message, category) \
    throw nlm::NLMException(message, #message, category)

// Safe operation macros
#define NLM_SAFE_OPERATION(operation, errorCategory) \
    try { operation; } catch (const std::exception& e) { \
        throw nlm::NLMException(std::string("Operation failed: ") + e.what(), errorCategory); \
    }

#define NLM_SAFE_RESOURCE_OPERATION(operation, resourceName) \
    try { operation; } catch (const std::exception& e) { \
        throw nlm::ResourceException(std::string("Resource operation failed: ") + e.what(), resourceName); \
    }

} // namespace nlm

#endif // SRC_CORE_ERROR_HANDLING_H