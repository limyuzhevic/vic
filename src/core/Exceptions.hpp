#include <string>
#include <stdexcept>

namespace nlm {

// Base NLM exception class
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message)
        : std::runtime_error(message) {}
    
    virtual ~NLMException() = default;
    
    virtual const char* what() const noexcept override {
        return std::runtime_error::what();
    }
};

// Configuration-related exceptions
class NLMConfigError : public NLMException {
public:
    explicit NLMConfigError(const std::string& message)
        : NLMException("Configuration Error: " + message) {}
};

class NLMInvalidConfig : public NLMConfigError {
public:
    explicit NLMInvalidConfig(const std::string& key, const std::string& reason)
        : NLMConfigError("Invalid configuration key '" + key + "': " + reason) {}
};

class NLMConfigLoadError : public NLMConfigError {
public:
    explicit NLMConfigLoadError(const std::string& filepath, const std::string& reason)
        : NLMConfigError("Failed to load configuration from '" + filepath + "': " + reason) {}
};

// Brain simulation exceptions
class NLMBrainError : public NLMException {
public:
    explicit NLMBrainError(const std::string& message)
        : NLMException("Brain Error: " + message) {}
};

class NLMNeuronError : public NLMBrainError {
public:
    explicit NLMNeuronError(const std::string& message)
        : NLMBrainError("Neuron Error: " + message) {}
};

class NMLSynapseError : public NLMBrainError {
public:
    explicit NMLSynapseError(const std::string& message)
        : NLMBrainError("Synapse Error: " + message) {}
};

class NLMBrainInitializationError : public NLMBrainError {
public:
    explicit NLMBrainInitializationError(const std::string& reason)
        : NLMBrainError("Brain initialization failed: " + reason) {}
};

// Memory management exceptions
class NLMMemoryError : public NLMException {
public:
    explicit NLMMemoryError(const std::string& message)
        : NLMException("Memory Error: " + message) {}
};

class NLMOutOfMemory : public NLMMemoryError {
public:
    explicit NLMOutOfMemory(size_t requested, size_t available)
        : NLMMemoryError("Out of memory: requested " + std::to_string(requested) +
                        " bytes, available " + std::to_string(available)) {}
};

class NLMMemoryCorruption : public NLMMemoryError {
public:
    explicit NLMMemoryCorruption(const std::string& location)
        : NLMMemoryError("Memory corruption detected at: " + location) {}
};

// Python-specific exceptions
class NLMPythonError : public NLMException {
public:
    explicit NLMPythonError(const std::string& message)
        : NLMException("Python Binding Error: " + message) {}
};

class NLMTypeError : public NLMPythonError {
public:
    explicit NLMTypeError(const std::string& expected, const std::string& actual)
        : NLMPythonError("Type error: expected " + expected + ", got " + actual) {}
};

class NLMBindingError : public NLMPythonError {
public:
    explicit NLMBindingError(const std::string& operation, const std::string& reason)
        : NLMPythonError("Binding error in operation '" + operation + "': " + reason) {}
};

// Performance-related exceptions
class NLMPerformanceError : public NLMException {
public:
    explicit NLMPerformanceError(const std::string& message)
        : NLMException("Performance Error: " + message) {}
};

class NLMInvalidParameter : public NLMPerformanceError {
public:
    explicit NLMInvalidParameter(const std::string& param, const std::string& value)
        : NLMPerformanceError("Invalid parameter '" + param + "': " + value) {}
};

// Error codes for programmatic error handling
enum class NLMErrorCode {
    // Success
    Success = 0,
    
    // Configuration errors
    ConfigNotFound = 1,
    ConfigInvalidType = 2,
    ConfigLoadFailed = 3,
    
    // Brain errors
    BrainNotInitialized = 4,
    NeuronNotFound = 5,
    SynapseNotFound = 6,
    BrainStepFailed = 7,
    
    // Memory errors
    OutOfMemory = 8,
    MemoryCorruption = 9,
    
    // Python binding errors
    PythonBindingFailed = 10,
    PythonTypeError = 11,
    PythonValueError = 12,
    
    // Performance errors
    InvalidParameter = 13,
    PerformanceLimitExceeded = 14,
    
    // General errors
    Unknown = 255
};

// Enhanced exception with error codes
class NLMErrors {
public:
    // Create exception with error code
    static std::runtime_error makeException(NLMErrorCode code, const std::string& message) {
        switch (code) {
            case NLMErrorCode::ConfigNotFound:
            case NLMErrorCode::ConfigInvalidType:
            case NLMErrorCode::ConfigLoadFailed:
                return NLMConfigError(message);
            
            case NLMErrorCode::BrainNotInitialized:
            case NLMErrorCode::NeuronNotFound:
            case NLMErrorCode::SynapseNotFound:
            case NLMErrorCode::BrainStepFailed:
                return NLMBrainError(message);
            
            case NLMErrorCode::OutOfMemory:
            case NLMErrorCode::MemoryCorruption:
                return NLMMemoryError(message);
            
            case NLMErrorCode::PythonBindingFailed:
            case NLMErrorCode::PythonTypeError:
            case NLMErrorCode::PythonValueError:
                return NLMPythonError(message);
            
            case NLMErrorCode::InvalidParameter:
            case NLMErrorCode::PerformanceLimitExceeded:
                return NLMPerformanceError(message);
            
            case NLMErrorCode::Success:
            case NLMErrorCode::Unknown:
            default:
                return NLMException(message);
        }
    }
    
    // Get error code from exception
    static NLMErrorCode getErrorCode(const std::exception& e) {
        const std::string& what = e.what();
        if (what.find("Configuration Error:") != std::string::npos) {
            if (what.find("Failed to load configuration") != std::string::npos) {
                return NLMErrorCode::ConfigLoadFailed;
            } else if (what.find("Invalid configuration key") != std::string::npos) {
                return NLMErrorCode::ConfigInvalidType;
            } else {
                return NLMErrorCode::ConfigNotFound;
            }
        } else if (what.find("Brain Error:") != std::string::npos) {
            if (what.find("initialization failed") != std::string::npos) {
                return NLMErrorCode::BrainNotInitialized;
            } else if (what.find("Neuron Error:") != std::string::npos) {
                return NLMErrorCode::NeuronNotFound;
            } else if (what.find("Synapse Error:") != std::string::npos) {
                return NLMErrorCode::SynapseNotFound;
            } else {
                return NLMErrorCode::BrainStepFailed;
            }
        } else if (what.find("Memory Error:") != std::string::npos) {
            if (what.find("Out of memory") != std::string::npos) {
                return NLMErrorCode::OutOfMemory;
            } else {
                return NLMErrorCode::MemoryCorruption;
            }
        } else if (what.find("Python Binding Error:") != std::string::npos) {
            if (what.find("Type error") != std::string::npos) {
                return NLMErrorCode::PythonTypeError;
            } else if (what.find("Binding error") != std::string::npos) {
                return NLMErrorCode::PythonBindingFailed;
            } else {
                return NLMErrorCode::PythonValueError;
            }
        } else if (what.find("Performance Error:") != std::string::npos) {
            if (what.find("Invalid parameter") != std::string::npos) {
                return NLMErrorCode::InvalidParameter;
            } else {
                return NLMErrorCode::PerformanceLimitExceeded;
            }
        }
        
        return NLMErrorCode::Unknown;
    }
};

} // namespace nlm