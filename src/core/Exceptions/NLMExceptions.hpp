// Custom exception classes for NLM Python API
// Enhanced error handling with exception chaining and validation

#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <chrono>
#include <stacktrace>
#include <stdexcept>
#include <pybind11/pybind11.h>

namespace nlm {

// Forward declarations
class NLMError;

// Base exception class for NLM errors
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) 
        : std::runtime_error(message), timestamp(std::chrono::system_clock::now()) {}
    
    NLMError(const std::string& message, std::exception_ptr cause)
        : std::runtime_error(message), cause(cause), timestamp(std::chrono::system_clock::now()) {}
    
    virtual ~NLMError() noexcept {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    // Get detailed error information
    std::string getDetails() const {
        std::stringstream ss;
        ss << "Error: " << message_ << "\n";
        ss << "Timestamp: " << getTimestampString() << "\n";
        ss << "Stack trace: " << getStackTrace() << "\n";
        if (cause) {
            ss << "Caused by: " << getCauseString();
        }
        return ss.str();
    }
    
    // Get error type as string
    virtual std::string getErrorType() const = 0;
    
    // Check if error can be recovered
    virtual bool isRecoverable() const { return false; }
    
protected:
    std::string message_;
    std::exception_ptr cause;
    std::chrono::system_clock::time_point timestamp;
    
    std::string getTimestampString() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string getStackTrace() const {
        try {
            return std::stacktrace::current().to_string();
        } catch (...) {
            return "Stack trace unavailable";
        }
    }
    
    std::string getCauseString() const {
        try {
            std::rethrow_exception(cause);
        } catch (const std::exception& e) {
            return std::string("std::exception: ") + e.what();
        } catch (...) {
            return "Unknown exception type";
        }
    }
};

// Configuration-related exceptions
class ConfigError : public NLMError {
public:
    explicit ConfigError(const std::string& message) : NLMError(message) {}
    ConfigError(const std::string& message, std::exception_ptr cause) 
        : NLMError(message, cause) {}
    
    std::string getErrorType() const override { return "ConfigError"; }
    bool isRecoverable() const override { return true; }
};

class ConfigValidationError : public ConfigError {
public:
    ConfigValidationError(const std::string& message, const std::string& key, const std::string& value)
        : ConfigError(message), key_(key), value_(value) {}
    
    std::string getErrorType() const override { return "ConfigValidationError"; }
    std::string getKey() const { return key_; }
    std::string getValue() const { return value_; }
    
private:
    std::string key_;
    std::string value_;
};

class ConfigTypeError : public ConfigError {
public:
    ConfigTypeError(const std::string& message, const std::string& key, const std::string& expectedType, const std::string& actualType)
        : ConfigError(message), key_(key), expectedType_(expectedType), actualType_(actualType) {}
    
    std::string getErrorType() const override { return "ConfigTypeError"; }
    std::string getKey() const { return key_; }
    std::string getExpectedType() const { return expectedType_; }
    std::string getActualType() const { return actualType_; }
    
private:
    std::string key_;
    std::string expectedType_;
    std::string actualType_;
};

class ConfigFileError : public ConfigError {
public:
    ConfigFileError(const std::string& message, const std::string& filepath, std::exception_ptr cause = nullptr)
        : ConfigError(message, cause), filepath_(filepath) {}
    
    std::string getErrorType() const override { return "ConfigFileError"; }
    std::string getFilePath() const { return filepath_; }
    
private:
    std::string filepath_;
};

// Brain-related exceptions
class BrainError : public NLMError {
public:
    explicit BrainError(const std::string& message) : NLMError(message) {}
    BrainError(const std::string& message, std::exception_ptr cause) 
        : NLMError(message, cause) {}
    
    std::string getErrorType() const override { return "BrainError"; }
    bool isRecoverable() const override { return true; }
};

class BrainValidationError : public BrainError {
public:
    BrainValidationError(const std::string& message, const std::string& context)
        : BrainError(message), context_(context) {}
    
    std::string getErrorType() const override { return "BrainValidationError"; }
    std::string getContext() const { return context_; }
    
private:
    std::string context_;
};

class BrainStateError : public BrainError {
public:
    BrainStateError(const std::string& message, const std::string& operation, bool recoverable = true)
        : BrainError(message), operation_(operation), recoverable_(recoverable) {}
    
    std::string getErrorType() const override { return "BrainStateError"; }
    std::string getOperation() const { return operation_; }
    bool isRecoverable() const override { return recoverable_; }
    
private:
    std::string operation_;
    bool recoverable_;
};

class NeuralNetworkError : public BrainError {
public:
    NeuralNetworkError(const std::string& message, const std::string& networkType, int neuronCount, int synapseCount)
        : BrainError(message), networkType_(networkType), neuronCount_(neuronCount), synapseCount_(synapseCount) {}
    
    std::string getErrorType() const override { return "NeuralNetworkError"; }
    std::string getNetworkType() const { return networkType_; }
    int getNeuronCount() const { return neuronCount_; }
    int getSynapseCount() const { return synapseCount_; }
    
private:
    std::string networkType_;
    int neuronCount_;
    int synapseCount_;
};

// Simulation-related exceptions
class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& message) : NLMError(message) {}
    SimulationError(const std::string& message, std::exception_ptr cause) 
        : NLMError(message, cause) {}
    
    std::string getErrorType() const override { return "SimulationError"; }
    bool isRecoverable() const override { return true; }
};

class SimulationParameterError : public SimulationError {
public:
    SimulationParameterError(const std::string& message, const std::string& paramName, double value, double min, double max)
        : SimulationError(message), paramName_(paramName), value_(value), min_(min), max_(max) {}
    
    std::string getErrorType() const override { return "SimulationParameterError"; }
    std::string getParamName() const { return paramName_; }
    double getValue() const { return value_; }
    double getMin() const { return min_; }
    double getMax() const { return max_; }
    
private:
    std::string paramName_;
    double value_;
    double min_;
    double max_;
};

class SimulationStepError : public SimulationError {
public:
    SimulationStepError(const std::string& message, int step, Timestamp time, std::exception_ptr cause = nullptr)
        : SimulationError(message, cause), step_(step), time_(time) {}
    
    std::string getErrorType() const override { return "SimulationStepError"; }
    int getStep() const { return step_; }
    Timestamp getTime() const { return time_; }
    
private:
    int step_;
    Timestamp time_;
};

// World-related exceptions
class WorldError : public NLMError {
public:
    explicit WorldError(const std::string& message) : NLMError(message) {}
    WorldError(const std::string& message, std::exception_ptr cause) 
        : NLMError(message, cause) {}
    
    std::string getErrorType() const override { return "WorldError"; }
    bool isRecoverable() const override { return true; }
};

class WorldValidationError : public WorldError {
public:
    WorldValidationError(const std::string& message, const std::string& worldType)
        : WorldError(message), worldType_(worldType) {}
    
    std::string getErrorType() const override { return "WorldValidationError"; }
    std::string getWorldType() const { return worldType_; }
    
private:
    std::string worldType_;
};

// Memory-related exceptions
class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message) : NLMError(message) {}
    MemoryError(const std::string& message, std::exception_ptr cause) 
        : NLMError(message, cause) {}
    
    std::string getErrorType() const override { return "MemoryError"; }
    bool isRecoverable() const override { return true; }
};

class MemoryCapacityError : public MemoryError {
public:
    MemoryCapacityError(const std::string& message, size_t requested, size_t available, const std::string& memoryType)
        : MemoryError(message), requested_(requested), available_(available), memoryType_(memoryType) {}
    
    std::string getErrorType() const override { return "MemoryCapacityError"; }
    size_t getRequestedCapacity() const { return requested_; }
    size_t getAvailableCapacity() const { return available_; }
    std::string getMemoryType() const { return memoryType_; }
    
private:
    size_t requested_;
    size_t available_;
    std::string memoryType_;
};

// Agent-related exceptions
class AgentError : public NLMError {
public:
    explicit AgentError(const std::string& message) : NLMError(message) {}
    AgentError(const std::string& message, std::exception_ptr cause) 
        : NLMError(message, cause) {}
    
    std::string getErrorType() const override { return "AgentError"; }
    bool isRecoverable() const override { return true; }
};

class AgentValidationError : public AgentError {
public:
    AgentValidationError(const std::string& message, const std::string& agentType)
        : AgentError(message), agentType_(agentType) {}
    
    std::string getErrorType() const override { return "AgentValidationError"; }
    std::string getAgentType() const { return agentType_; }
    
private:
    std::string agentType_;
};

// Performance-related exceptions
class PerformanceError : public NLMError {
public:
    explicit PerformanceError(const std::string& message) : NLMError(message) {}
    PerformanceError(const std::string& message, std::exception_ptr cause) 
        : NLMError(message, cause) {}
    
    std::string getErrorType() const override { return "PerformanceError"; }
    bool isRecoverable() const override { return true; }
};

class MemoryLeakError : public PerformanceError {
public:
    MemoryLeakError(const std::string& message, const std::string& leakType, size_t detectedBytes)
        : PerformanceError(message), leakType_(leakType), detectedBytes_(detectedBytes) {}
    
    std::string getErrorType() const override { return "MemoryLeakError"; }
    std::string getLeakType() const { return leakType_; }
    size_t getDetectedBytes() const { return detectedBytes_; }
    
private:
    std::string leakType_;
    size_t detectedBytes_;
};

class PerformanceBottleneckError : public PerformanceError {
public:
    PerformanceBottleneckError(const std::string& message, const std::string& bottleneckType, double thresholdExceeded, double thresholdLimit)
        : PerformanceError(message), bottleneckType_(bottleneckType), thresholdExceeded_(thresholdExceeded), thresholdLimit_(thresholdLimit) {}
    
    std::string getErrorType() const override { return "PerformanceBottleneckError"; }
    std::string getBottleneckType() const { return bottleneckType_; }
    double getThresholdExceeded() const { return thresholdExceeded_; }
    double getThresholdLimit() const { return thresholdLimit_; }
    
private:
    std::string bottleneckType_;
    double thresholdExceeded_;
    double thresholdLimit_;
};

// Exception factory functions
inline std::shared_ptr<NLMError> makeConfigError(const std::string& message) {
    return std::make_shared<ConfigError>(message);
}

inline std::shared_ptr<NLMError> makeConfigValidationError(const std::string& message, const std::string& key, const std::string& value) {
    return std::make_shared<ConfigValidationError>(message, key, value);
}

inline std::shared_ptr<NLMError> makeBrainError(const std::string& message) {
    return std::make_shared<BrainError>(message);
}

inline std::shared_ptr<NLMError> makeBrainValidationError(const std::string& message, const std::string& context) {
    return std::make_shared<BrainValidationError>(message, context);
}

inline std::shared_ptr<NLMError> makeSimulationError(const std::string& message) {
    return std::make_shared<SimulationError>(message);
}

inline std::shared_ptr<NLMError> makeMemoryError(const std::string& message) {
    return std::make_shared<MemoryError>(message);
}

inline std::shared_ptr<NLMError> makeAgentError(const std::string& message) {
    return std::make_shared<AgentError>(message);
}

inline std::shared_ptr<NLMError> makePerformanceError(const std::string& message) {
    return std::make_shared<PerformanceError>(message);
}

// Safe exception propagation wrapper
inline void safePropagateException(const std::exception& e) {
    try {
        throw;
    } catch (const std::exception& caught) {
        // Log the error before rethrowing
        nlm::Logger::getGlobal().error("Exception propagated: " + std::string(caught.what()));
        throw;
    }
}

} // namespace nlm