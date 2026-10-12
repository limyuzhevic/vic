// NLM (熙然) - Neural Learning Machine
// Phase 8: Enhanced Error Handling System
//
// This phase enhances the error handling system with:
// - Comprehensive exception hierarchy
// - Better error reporting and logging
// - Error classification and handling
// - User-friendly error messages
// - Integration with NLM components

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <map>
#include <functional>

namespace nlm {

// Base exception class for all NLM errors
class NLMException : public std::runtime_error {
public:
    enum class ErrorCode {
        // General errors
        Unknown = 0,
        MemoryAllocation,
        FileIOError,
        ParseError,
        ValidationError,
        ConfigurationError,
        InitializationError,
        SimulationError,
        
        // Neural network specific errors
        NeuronError,
        SynapseError,
        NetworkError,
        PlasticityError,
        DevelopmentError,
        
        // Sensory/Motor errors
        SensoryError,
        MotorError,
        PerceptionError,
        
        // Environment errors
        EnvironmentError,
        WorldError,
        
        // Agent errors
        AgentError,
        BehaviorError,
        LearningError,
        
        // Experiment errors
        ExperimentError,
        MetricsError,
        
        // Python binding errors
        PythonBindingError,
        APIError,
        
        // System errors
        SystemError,
        ResourceError,
        CompatibilityError
    };
    
    NLMException(const std::string& message, ErrorCode code = ErrorCode::Unknown,
                 const std::string& component = "", int line = 0,
                 const std::string& file = "", const std::string& function = "");
    
    ErrorCode getErrorCode() const { return errorCode; }
    const std::string& getComponent() const { return component; }
    int getLine() const { return line; }
    const std::string& getFile() const { return file; }
    const std::string& getFunction() const { return function; }
    
    std::string getFullError() const;
    
    // Exception chaining support
    void addCause(const NLMException& cause) { causes.push_back(cause); }
    const std::vector<NLMException>& getCauses() const { return causes; }
    
    // Format error for user display
    virtual std::string formatUserMessage() const;
    
    // Format error for developer/debugging
    virtual std::string formatDebugMessage() const;
    
protected:
    ErrorCode errorCode;
    std::string component;
    int line;
    std::string file;
    std::string function;
    std::vector<NLMException> causes;
};

// Memory management exceptions
class MemoryAllocationError : public NLMException {
public:
    MemoryAllocationError(const std::string& message, size_t size,
                         const std::string& hint = "")
        : NLMException(message, ErrorCode::MemoryAllocation, "MemoryManager", 0, "", "") {
        this->size = size;
        this->hint = hint;
    }
    
    size_t getSize() const { return size; }
    const std::string& getHint() const { return hint; }
    
    std::string formatUserMessage() const override {
        std::ostringstream oss;
        oss << "Memory allocation failed: " << what() << "\n";
        oss << "Size requested: " << formatSize(size) << "\n";
        if (!hint.empty()) {
            oss << "Hint: " << hint << "\n";
        }
        return oss.str();
    }
    
private:
    size_t size;
    std::string hint;
    
    static std::string formatSize(size_t size) {
        static const char* suffixes[] = {"B", "KB", "MB", "GB"};
        int suffixIndex = 0;
        double formattedSize = static_cast<double>(size);
        
        while (formattedSize > 1024.0 && suffixIndex < 3) {
            formattedSize /= 1024.0;
            suffixIndex++;
        }
        
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << formattedSize << " " << suffixes[suffixIndex];
        return oss.str();
    }
};

class MemoryCorruptionError : public NLMException {
public:
    MemoryCorruptionError(const std::string& description, void* address)
        : NLMException("Memory corruption detected: " + description, ErrorCode::MemoryAllocation,
                      "MemoryValidator", 0, "", "") {
        this->address = address;
    }
    
    void* getAddress() const { return address; }
    
    std::string formatUserMessage() const override {
        std::ostringstream oss;
        oss << "Memory corruption detected.\n";
        oss << "Description: " << what() << "\n";
        oss << "Address: 0x" << std::hex << reinterpret_cast<uintptr_t>(address) << std::dec << "\n";
        oss << "This may be caused by buffer overflows or use-after-free errors.\n";
        oss << "Please check your code for memory management issues.";
        return oss.str();
    }
    
private:
    void* address;
};

// Configuration exceptions
class ConfigurationError : public NLMException {
public:
    enum class Type {
        InvalidValue,
        MissingRequired,
        InvalidFormat,
        ValidationFailed,
        FileNotFound,
        ParseError
    };
    
    ConfigurationError(const std::string& message, Type type,
                       const std::string& key = "", const std::string& expected = "")
        : NLMException(message, ErrorCode::ConfigurationError, "Config", 0, "", "") {
        this->type = type;
        this->key = key;
        this->expected = expected;
    }
    
    Type getType() const { return type; }
    const std::string& getKey() const { return key; }
    const std::string& getExpected() const { return expected; }
    
    std::string formatUserMessage() const override {
        std::ostringstream oss;
        oss << "Configuration error: " << what() << "\n";
        
        if (!key.empty()) {
            oss << "Key: " << key << "\n";
        }
        
        oss << "Type: ";
        switch (type) {
            case Type::InvalidValue: oss << "Invalid value"; break;
            case Type::MissingRequired: oss << "Missing required field"; break;
            case Type::InvalidFormat: oss << "Invalid format"; break;
            case Type::ValidationFailed: oss << "Validation failed"; break;
            case Type::FileNotFound: oss << "File not found"; break;
            case Type::ParseError: oss << "Parse error"; break;
        }
        
        if (!expected.empty()) {
            oss << "\nExpected: " << expected;
        }
        
        oss << "\n\nPlease check your configuration file or command line arguments.";
        return oss.str();
    }
    
private:
    Type type;
    std::string key;
    std::string expected;
};

// Neural network exceptions
class NeuronError : public NLMException {
public:
    enum class ErrorType {
        InvalidParameter,
        NetworkNotReady,
        FiringError,
        PlasticityError,
        DevelopmentError
    };
    
    NeuronError(const std::string& message, ErrorType errorType,
                const std::string& neuronId = "")
        : NLMException(message, ErrorCode::NeuronError, "Neuron", 0, "", "") {
        this->errorType = errorType;
        this->neuronId = neuronId;
    }
    
    ErrorType getErrorType() const { return errorType; }
    const std::string& getNeuronId() const { return neuronId; }
    
    std::string formatUserMessage() const override {
        std::ostringstream oss;
        oss << "Neuron error: " << what() << "\n";
        
        if (!neuronId.empty()) {
            oss << "Neuron ID: " << neuronId << "\n";
        }
        
        oss << "Error type: ";
        switch (errorType) {
            case ErrorType::InvalidParameter: oss << "Invalid parameter"; break;
            case ErrorType::NetworkNotReady: oss << "Network not ready"; break;
            case ErrorType::FiringError: oss << "Firing error"; break;
            case ErrorType::PlasticityError: oss << "Plasticity error"; break;
            case ErrorType::DevelopmentError: oss << "Development error"; break;
        }
        
        return oss.str();
    }
    
private:
    ErrorType errorType;
    std::string neuronId;
};

class SynapseError : public NLMException {
public:
    enum class ErrorType {
        ConnectionError,
        WeightError,
        PlasticityError,
        DelayError
    };
    
    SynapseError(const std::string& message, ErrorType errorType,
                 const std::string& synapseId = "")
        : NLMException(message, ErrorCode::SynapseError, "Synapse", 0, "", "") {
        this->errorType = errorType;
        this->synapseId = synapseId;
    }
    
    ErrorType getErrorType() const { return errorType; }
    const std::string& getSynapseId() const { return synapseId; }
    
    std::string formatUserMessage() const override {
        std::ostringstream oss;
        oss << "Synapse error: " << what() << "\n";
        
        if (!synapseId.empty()) {
            oss << "Synapse ID: " << synapseId << "\n";
        }
        
        oss << "Error type: ";
        switch (errorType) {
            case ErrorType::ConnectionError: oss << "Connection error"; break;
            case ErrorType::WeightError: oss << "Weight error"; break;
            case ErrorType::PlasticityError: oss << "Plasticity error"; break;
            case ErrorType::DelayError: oss << "Delay error"; break;
        }
        
        return oss.str();
    }
    
private:
    ErrorType errorType;
    std::string synapseId;
};

class NetworkError : public NLMException {
public:
    enum class ErrorType {
        NotInitialized,
        AlreadyInitialized,
        InvalidConfiguration,
        ConnectivityError,
        ResourceExhaustion
    };
    
    NetworkError(const std::string& message, ErrorType errorType)
        : NLMException(message, ErrorCode::NetworkError, "Network", 0, "", "") {
        this->errorType = errorType;
    }
    
    ErrorType getErrorType() const { return errorType; }
    
    std::string formatUserMessage() const override {
        std::ostringstream oss;
        oss << "Network error: " << what() << "\n";
        
        oss << "Error type: ";
        switch (errorType) {
            case ErrorType::NotInitialized: oss << "Network not initialized"; break;
            case ErrorType::AlreadyInitialized: oss << "Network already initialized"; break;
            case ErrorType::InvalidConfiguration: oss << "Invalid configuration"; break;
            case ErrorType::ConnectivityError: oss << "Connectivity error"; break;
            case ErrorType::ResourceExhaustion: oss << "Resource exhaustion"; break;
        }
        
        return oss.str();
    }
    
private:
    ErrorType errorType;
};

// Simulation exceptions
class SimulationError : public NLMException {
public:
    enum class ErrorType {
        StepError,
        TimeStepError,
        IntegrationError,
        ConvergenceError,
        Stalling
    };
    
    SimulationError(const std::string& message, ErrorType errorType, SimulationStep step)
        : NLMException(message, ErrorCode::SimulationError, "Simulation", 0, "", "") {
        this->errorType = errorType;
        this->step = step;
    }
    
    ErrorType getErrorType() const { return errorType; }
    SimulationStep getStep() const { return step; }
    
    std::string formatUserMessage() const override {
        std::ostringstream oss;
        oss << "Simulation error: " << what() << "\n";
        oss << "Step: " << step << "\n";
        
        oss << "Error type: ";
        switch (errorType) {
            case ErrorType::StepError: oss << "Step error"; break;
            case ErrorType::TimeStepError: oss << "Time step error"; break;
            case ErrorType::IntegrationError: oss << "Integration error"; break;
            case ErrorType::ConvergenceError: oss << "Convergence error"; break;
            case ErrorType::Stalling: oss << "Stalling detected"; break;
        }
        
        return oss.str();
    }
    
private:
    ErrorType errorType;
    SimulationStep step;
};

// Python binding exceptions
class PythonBindingError : public NLMException {
public:
    enum class ErrorType {
        ImportError,
        TypeError,
        MethodError,
        ConversionError,
        MissingModule
    };
    
    PythonBindingError(const std::string& message, ErrorType errorType,
                       const std::string& pythonFunction = "")
        : NLMException(message, ErrorCode::PythonBindingError, "PythonBindings", 0, "", "") {
        this->errorType = errorType;
        this->pythonFunction = pythonFunction;
    }
    
    ErrorType getErrorType() const { return errorType; }
    const std::string& getPythonFunction() const { return pythonFunction; }
    
    std::string formatUserMessage() const override {
        std::ostringstream oss;
        oss << "Python binding error: " << what() << "\n";
        
        if (!pythonFunction.empty()) {
            oss << "Python function: " << pythonFunction << "\n";
        }
        
        oss << "Error type: ";
        switch (errorType) {
            case ErrorType::ImportError: oss << "Import error"; break;
            case ErrorType::TypeError: oss << "Type error"; break;
            case ErrorType::MethodError: oss << "Method error"; break;
            case ErrorType::ConversionError: oss << "Conversion error"; break;
            case ErrorType::MissingModule: oss << "Missing module"; break;
        }
        
        oss << "\n\nThis may be a Python package dependency issue. "
            << "Please ensure all required Python packages are installed.";
        
        return oss.str();
    }
    
private:
    ErrorType errorType;
    std::string pythonFunction;
};

// Error reporting and handling
class ErrorReporter {
public:
    using ErrorHandler = std::function<void(const NLMException&)>;
    
    static void registerErrorHandler(ErrorHandler handler);
    static void unregisterErrorHandler();
    static void reportError(const NLMException& error);
    static void reportError(const std::exception& error);
    static void reportError(const std::string& message);
    
    // Error collection and analysis
    static void collectErrors(const std::vector<NLMException>& errors);
    static std::vector<NLMException> getCollectedErrors();
    static void clearCollectedErrors();
    
    // Error statistics
    static size_t getErrorCount(ErrorCode code = ErrorCode::Unknown) const;
    static double getErrorRate() const;
    
    // Error formatting
    static std::string formatErrorReport(const std::vector<NLMException>& errors) const;
    static std::string formatErrorSummary(const std::vector<NLMException>& errors) const;
    
    // Error filtering
    static std::vector<NLMException> filterErrors(ErrorCode code) const;
    static std::vector<NLMException> filterErrors(const std::string& component) const;
    
private:
    static ErrorHandler errorHandler;
    static std::vector<NLMException> collectedErrors;
    static std::mutex errorMutex;
};

// Error context management
class ErrorContext {
public:
    static void setContext(const std::string& key, const std::string& value);
    static std::string getContext(const std::string& key);
    static void clearContext(const std::string& key);
    static void clearAllContexts();
    static std::map<std::string, std::string> getAllContexts();
    
    // Context-aware error reporting
    static void reportErrorWithContext(const NLMException& error);
    
    // Performance context
    static void setSimulationStep(SimulationStep step) { currentStep = step; }
    static SimulationStep getSimulationStep() { return currentStep; }
    
    static void setNetworkSize(size_t neurons, size_t synapses) {
        networkNeurons = neurons;
        networkSynapses = synapses;
    }
    
    static size_t getNetworkNeurons() { return networkNeurons; }
    static size_t getNetworkSynapses() { return networkSynapses; }
    
private:
    static std::map<std::string, std::string> contexts;
    static std::mutex contextMutex;
    static SimulationStep currentStep;
    static size_t networkNeurons;
    static size_t networkSynapses;
};

// Global error reporting functions
inline void registerErrorHandler(ErrorReporter::ErrorHandler handler) {
    ErrorReporter::registerErrorHandler(handler);
}

inline void reportError(const NLMException& error) {
    ErrorReporter::reportError(error);
}

inline void reportError(const std::exception& error) {
    ErrorReporter::reportError(error);
}

inline void reportError(const std::string& message) {
    ErrorReporter::reportError(message);
}

// Error utilities
inline std::string formatError(const NLMException& error) {
    return error.formatUserMessage();
}

inline std::string formatError(const std::exception& error) {
    return error.what();
}

// Error checking macros
#define CHECK_ERROR(condition, message, errorCode) \
    if (condition) { \
        throw nlm::NLMException(message, errorCode, "ErrorCheck", __LINE__, __FILE__, __func__); \
    }

#define CHECK_ERROR_WITH_CAUSE(condition, message, errorCode, cause) \
    if (condition) { \
        nlm::NLMException error(message, errorCode, "ErrorCheck", __LINE__, __FILE__, __func__); \
        error.addCause(cause); \
        throw error; \
    }

#define VALIDATE(condition, message, errorType) \
    if (!(condition)) { \
        throw nlm::ConfigurationError(message, nlm::ConfigurationError::Type::ValidationFailed, "", #condition); \
    }

// Exception handling helper
#define TRY_CATCH_ERROR(description) \
    try { \

#define CATCH_ERROR \
    } catch (const nlm::NLMException& e) { \
        nlm::reportError(e); \
        throw; \
    } catch (const std::exception& e) { \
        nlm::reportError(e); \
        throw; \
    }

} // namespace nlm

// Global error reporting instance
inline nlm::ErrorReporter& getErrorReporter() {
    static nlm::ErrorReporter reporter;
    return reporter;
}

// Convenience functions for common error scenarios
inline void throwMemoryError(size_t size, const std::string& hint = "") {
    throw nlm::MemoryAllocationError("Memory allocation failed", size, hint);
}

inline void throwConfigError(const std::string& message, nlm::ConfigurationError::Type type,
                            const std::string& key = "", const std::string& expected = "") {
    throw nlm::ConfigurationError(message, type, key, expected);
}

inline void throwNeuronError(const std::string& message, nlm::NeuronError::ErrorType errorType,
                            const std::string& neuronId = "") {
    throw nlm::NeuronError(message, errorType, neuronId);
}

inline void throwSimulationError(const std::string& message, nlm::SimulationError::ErrorType errorType,
                                 nlm::SimulationStep step) {
    throw nlm::SimulationError(message, errorType, step);
}