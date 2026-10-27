#pragma once

#include <stdexcept>
#include <string>
#include <map>
#include <functional>

namespace nlm {

// Base NLM exception class
class NLMException : public std::runtime_error {
public:
    enum class ErrorCode {
        // Memory and resource errors
        MemoryAllocationFailed,
        ResourceNotAvailable,
        InvalidResource,
        
        // Configuration errors
        ConfigKeyNotFound,
        ConfigValueInvalid,
        ConfigValidationFailed,
        ConfigLoadFailed,
        ConfigSaveFailed,
        
        // Neural system errors
        NeuronNotFound,
        SynapseNotFound,
        RegionNotFound,
        PopulationNotFound,
        InvalidNeuronState,
        InvalidSynapseState,
        
        // Simulation errors
        SimulationNotInitialized,
        InvalidSimulationStep,
        SimulationAlreadyRunning,
        SimulationNotRunning,
        
        // Plasticity errors
        PlasticityNotEnabled,
        InvalidPlasticityParameters,
        STDPError,
        HebbianError,
        
        // Memory system errors
        MemoryNotInitialized,
        MemoryFull,
        MemoryInvalidAddress,
        
        // Development errors
        DevelopmentNotInitialized,
        InvalidDevelopmentalStage,
        
        // Neuromodulation errors
        NeuromodulatorNotFound,
        InvalidNeuromodulationParameters,
        
        // Agent errors
        AgentNotInitialized,
        InvalidAgentState,
        
        // World errors
        WorldNotInitialized,
        InvalidWorldPosition,
        InvalidWorldObject,
        
        // Experiment errors
        ExperimentNotFound,
        ExperimentAlreadyRunning,
        InvalidExperimentState,
        
        // General errors
        OperationNotSupported,
        InvalidParameter,
        Timeout,
        ConcurrentModification,
        FileSystemError,
        SerializationError,
        DeserializationError,
        UnknownError
    };
    
    NLMException(const std::string& message, ErrorCode code = ErrorCode::UnknownError)
        : std::runtime_error(message), code_(code) {}
    
    NLMException(const std::string& message, const std::string& context, ErrorCode code = ErrorCode::UnknownError)
        : std::runtime_error(message + " (Context: " + context + ")"), code_(code) {}
    
    ErrorCode getCode() const { return code_; }
    std::string getCodeString() const;
    
private:
    ErrorCode code_;
};

// Memory management exception
class NLMMemoryException : public NLMException {
public:
    NLMMemoryException(const std::string& message, const std::string& operation)
        : NLMException(message + " (Operation: " + operation + ")", ErrorCode::MemoryAllocationFailed) {}
};

// Configuration exception
class NLMConfigException : public NLMException {
public:
    NLMConfigException(const std::string& message, const std::string& key = "")
        : NLMException(message, ErrorCode::ConfigValidationFailed) {
        if (!key.empty()) {
            context_ = "Key: " + key;
        }
    }
    
    const std::string& getContext() const { return context_; }
    
private:
    std::string context_;
};

// Neural system exception
class NLMNeuralException : public NLMException {
public:
    NLMNeuralException(const std::string& message, ErrorCode code = ErrorCode::UnknownError)
        : NLMException(message, code) {}
    
    NLMNeuralException(const std::string& message, const std::string& entity, ErrorCode code = ErrorCode::UnknownError)
        : NLMException(message + " (Entity: " + entity + ")", code) {}
};

// Simulation exception
class NLMSimulationException : public NLMException {
public:
    NLMSimulationException(const std::string& message, ErrorCode code = ErrorCode::UnknownError)
        : NLMException(message, code) {}
    
    NLMSimulationException(const std::string& message, uint64_t step, ErrorCode code = ErrorCode::UnknownError)
        : NLMException(message + " (Step: " + std::to_string(step) + ")", code) {}
};

// Error handler class for centralized error management
class ErrorHandler {
public:
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }
    
    // Set global error callback
    void setErrorCallback(std::function<void(const NLMException&)> callback) {
        errorCallback_ = callback;
    }
    
    // Log error
    void logError(const NLMException& error) {
        if (errorCallback_) {
            errorCallback_(error);
        }
        // Also log to system log
        std::cerr << "[NLM ERROR] " << error.what() << std::endl;
    }
    
    // Handle error (could be overridden by application)
    virtual void handleError(const NLMException& error) {
        // Default implementation: log and rethrow
        logError(error);
    }
    
    // Register error handler for specific error codes
    void registerErrorHandler(ErrorCode code, std::function<void(const NLMException&)> handler) {
        codeHandlers_[code] = handler;
    }
    
    // Handle error with specific handler
    void handleError(ErrorCode code, const NLMException& error) {
        auto it = codeHandlers_.find(code);
        if (it != codeHandlers_.end()) {
            it->second(error);
        } else {
            handleError(error);
        }
    }
    
    // Check if error is recoverable
    virtual bool isRecoverable(const NLMException& error) {
        switch (error.getCode()) {
            case ErrorCode::MemoryAllocationFailed:
            case ErrorCode::FileSystemError:
            case ErrorCode::SerializationError:
                return false;
            case ErrorCode::InvalidParameter:
            case ErrorCode::OperationNotSupported:
                return false;
            case ErrorCode::NeuronNotFound:
            case ErrorCode::SynapseNotFound:
                return false;
            case ErrorCode::SimulationNotRunning:
            case ErrorCode::InvalidSimulationStep:
                return true;
            case ErrorCode::PlasticityNotEnabled:
            case ErrorCode::NeuromodulatorNotFound:
                return true;
            default:
                return true;
        }
    }
    
protected:
    ErrorHandler() = default;
    ~ErrorHandler() = default;
    
private:
    std::function<void(const NLMException&)> errorCallback_;
    std::map<ErrorCode, std::function<void(const NLMException&)>> codeHandlers_;
};

// Exception helper functions
inline std::string getErrorCodeString(NLMException::ErrorCode code) {
    switch (code) {
        case NLMException::ErrorCode::MemoryAllocationFailed: return "MemoryAllocationFailed";
        case NLMException::ErrorCode::ResourceNotAvailable: return "ResourceNotAvailable";
        case NLMException::ErrorCode::InvalidResource: return "InvalidResource";
        case NLMException::ErrorCode::ConfigKeyNotFound: return "ConfigKeyNotFound";
        case NLMException::ErrorCode::ConfigValueInvalid: return "ConfigValueInvalid";
        case NLMException::ErrorCode::ConfigValidationFailed: return "ConfigValidationFailed";
        case NLMException::ErrorCode::ConfigLoadFailed: return "ConfigLoadFailed";
        case NLMException::ErrorCode::ConfigSaveFailed: return "ConfigSaveFailed";
        case NLMException::ErrorCode::NeuronNotFound: return "NeuronNotFound";
        case NLMException::ErrorCode::SynapseNotFound: return "SynapseNotFound";
        case NLMException::ErrorCode::RegionNotFound: return "RegionNotFound";
        case NLMException::ErrorCode::PopulationNotFound: return "PopulationNotFound";
        case NLMException::ErrorCode::InvalidNeuronState: return "InvalidNeuronState";
        case NLMException::ErrorCode::InvalidSynapseState: return "InvalidSynapseState";
        case NLMException::ErrorCode::SimulationNotInitialized: return "SimulationNotInitialized";
        case NLMException::ErrorCode::InvalidSimulationStep: return "InvalidSimulationStep";
        case NLMException::ErrorCode::SimulationAlreadyRunning: return "SimulationAlreadyRunning";
        case NLMException::ErrorCode::SimulationNotRunning: return "SimulationNotRunning";
        case NLMException::ErrorCode::PlasticityNotEnabled: return "PlasticityNotEnabled";
        case NLMException::ErrorCode::InvalidPlasticityParameters: return "InvalidPlasticityParameters";
        case NLMException::ErrorCode::STDPError: return "STDPError";
        case NLMException::ErrorCode::HebbianError: return "HebbianError";
        case NLMException::ErrorCode::MemoryNotInitialized: return "MemoryNotInitialized";
        case NLMException::ErrorCode::MemoryFull: return "MemoryFull";
        case NLMException::ErrorCode::MemoryInvalidAddress: return "MemoryInvalidAddress";
        case NLMException::ErrorCode::DevelopmentNotInitialized: return "DevelopmentNotInitialized";
        case NLMException::ErrorCode::InvalidDevelopmentalStage: return "InvalidDevelopmentalStage";
        case NLMException::ErrorCode::NeuromodulatorNotFound: return "NeuromodulatorNotFound";
        case NLMException::ErrorCode::InvalidNeuromodulationParameters: return "InvalidNeuromodulationParameters";
        case NLMException::ErrorCode::AgentNotInitialized: return "AgentNotInitialized";
        case NLMException::ErrorCode::InvalidAgentState: return "InvalidAgentState";
        case NLMException::ErrorCode::WorldNotInitialized: return "WorldNotInitialized";
        case NLMException::ErrorCode::InvalidWorldPosition: return "InvalidWorldPosition";
        case NLMException::ErrorCode::InvalidWorldObject: return "InvalidWorldObject";
        case NLMException::ErrorCode::ExperimentNotFound: return "ExperimentNotFound";
        case NLMException::ErrorCode::ExperimentAlreadyRunning: return "ExperimentAlreadyRunning";
        case NLMException::ErrorCode::InvalidExperimentState: return "InvalidExperimentState";
        case NLMException::ErrorCode::OperationNotSupported: return "OperationNotSupported";
        case NLMException::ErrorCode::InvalidParameter: return "InvalidParameter";
        case NLMException::ErrorCode::Timeout: return "Timeout";
        case NLMException::ErrorCode::ConcurrentModification: return "ConcurrentModification";
        case NLMException::ErrorCode::FileSystemError: return "FileSystemError";
        case NLMException::ErrorCode::SerializationError: return "SerializationError";
        case NLMException::ErrorCode::DeserializationError: return "DeserializationError";
        case NLMException::ErrorCode::UnknownError: return "UnknownError";
        default: return "UnknownError";
    }
}

// Safe memory allocation with error handling
template<typename T>
T* safeAllocate(size_t count, const std::string& operation) {
    try {
        T* ptr = new T[count];
        if (!ptr) {
            throw NLMMemoryException("Memory allocation failed", operation);
        }
        return ptr;
    } catch (const std::bad_alloc&) {
        throw NLMMemoryException("Memory allocation failed (bad_alloc)", operation);
    }
}

// Safe vector allocation with error handling
template<typename T>
std::vector<T> safeVectorAlloc(size_t count, const std::string& operation) {
    try {
        return std::vector<T>(count);
    } catch (const std::bad_alloc&) {
        throw NLMMemoryException("Memory allocation failed (bad_alloc)", operation);
    }
}

// Safe container operations
template<typename Container>
void safeInsert(Container& container, const typename Container::value_type& value,
               const std::string& operation) {
    try {
        container.insert(value);
    } catch (const std::exception& e) {
        throw NLMException("Failed to insert into container: " + std::string(e.what()), 
                          operation, NLMException::ErrorCode::ConcurrentModification);
    }
}

// Configuration validation helper
inline bool validateConfigKey(const std::string& key, const std::map<std::string, bool>& requiredKeys) {
    if (requiredKeys.count(key) && !requiredKeys.at(key)) {
        throw NLMConfigException("Missing required configuration key: " + key, key);
    }
    return true;
}

// Neural entity validation helpers
inline void validateNeuronExists(bool exists, const std::string& entityName) {
    if (!exists) {
        throw NLMNeuralException("Neuron not found: " + entityName, 
                               NLMException::ErrorCode::NeuronNotFound);
    }
}

inline void validateSynapseExists(bool exists, const std::string& entityName) {
    if (!exists) {
        throw NLMNeuralException("Synapse not found: " + entityName,
                               NLMException::ErrorCode::SynapseNotFound);
    }
}

inline void validateRegionExists(bool exists, const std::string& entityName) {
    if (!exists) {
        throw NLMNeuralException("Region not found: " + entityName,
                               NLMException::ErrorCode::RegionNotFound);
    }
}

inline void validatePopulationExists(bool exists, const std::string& entityName) {
    if (!exists) {
        throw NLMNeuralException("Population not found: " + entityName,
                               NLMException::ErrorCode::PopulationNotFound);
    }
}

inline void validateSimulationState(bool isRunning, const std::string& operation) {
    if (operation == "step" && !isRunning) {
        throw NLMSimulationException("Cannot step simulation when not running", 
                                   NLMException::ErrorCode::SimulationNotRunning);
    } else if (operation == "initialize" && isRunning) {
        throw NLMSimulationException("Cannot initialize simulation when already running",
                                   NLMException::ErrorCode::SimulationAlreadyRunning);
    }
}

// Range validation helper
template<typename T>
inline void validateRange(const T& value, const T& min, const T& max, const std::string& paramName) {
    if (value < min || value > max) {
        throw NLMConfigException("Parameter " + paramName + " out of range: " + 
                               std::to_string(value) + " (valid: " + 
                               std::to_string(min) + " - " + std::to_string(max) + ")",
                               paramName);
    }
}

// Null pointer validation
inline void validatePointer(const void* ptr, const std::string& operation) {
    if (!ptr) {
        throw NLMException("Null pointer in operation: " + operation,
                          NLMException::ErrorCode::InvalidParameter);
    }
}

// File operation validation
inline void validateFileAccess(const std::string& filepath, bool write = false) {
    // Basic file existence check (full implementation would use filesystem)
    if (filepath.empty()) {
        throw NLMException("Empty filepath provided", NLMException::ErrorCode::FileSystemError);
    }
    if (write && !std::filesystem::exists(std::filesystem::path(filepath).parent_path())) {
        throw NLMException("Directory does not exist: " + filepath,
                          NLMException::ErrorCode::FileSystemError);
    }
}

// Error recovery helper
inline bool attemptRecovery(const NLMException& error) {
    ErrorHandler& handler = ErrorHandler::getInstance();
    return handler.isRecoverable(error);
}

} // namespace nlm
