#pragma once

#include <string>
#include <system_error>
#include <stdexcept>
#include <iostream>

namespace nlm {

// Error categories for NLM
enum class NLMErrors {
    Success = 0,
    // Configuration errors
    ConfigFileNotFound = 1,
    ConfigParseError = 2,
    ConfigInvalidValue = 3,
    // Memory errors
    MemoryAllocationFailed = 4,
    MemoryCorruption = 5,
    // Simulation errors
    SimulationNotInitialized = 6,
    SimulationAlreadyRunning = 7,
    InvalidStepDuration = 8,
    // Neuron errors
    NeuronNotFound = 9,
    InvalidNeuronType = 10,
    NeuronAlreadyDead = 11,
    // Synapse errors
    SynapseNotFound = 12,
    InvalidSynapseConnection = 13,
    // Neuromodulator errors
    NeuromodulatorNotFound = 14,
    InvalidNeuromodulatorState = 15,
    // Memory system errors
    MemorySystemNotInitialized = 16,
    MemoryFull = 17,
    InvalidMemoryAddress = 18,
    // Development errors
    DevelopmentSystemNotInitialized = 19,
    InvalidDevelopmentalStage = 20,
    // Environment errors
    EnvironmentNotInitialized = 21,
    InvalidEnvironmentState = 22,
    // Cognition errors
    CognitiveSystemNotInitialized = 23,
    InvalidCognitiveState = 24,
    // Prediction errors
    PredictionSystemNotInitialized = 25,
    InvalidPrediction = 26,
    // General errors
    UnknownError = 999
};

// NLM exception base class
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, NLMErrors errorCode = NLMErrors::UnknownError)
        : std::runtime_error(message), errorCode_(errorCode) {}
    
    virtual ~NLMException() = default;
    
    NLMErrors getErrorCode() const { return errorCode_; }
    const char* what() const noexcept override {
        return std::runtime_error::what();
    }
    
    // Convert to std::error_code for standard compatibility
    std::error_code toErrorCode() const {
        return std::make_error_code(static_cast<int>(errorCode_));
    }
    
protected:
    NLMErrors errorCode_;
};

// Specific NLM exception types
class ConfigException : public NLMException {
public:
    ConfigException(const std::string& message, NLMErrors errorCode = NLMErrors::ConfigParseError)
        : NLMException(message, errorCode) {}
};

class MemoryException : public NLMException {
public:
    MemoryException(const std::string& message, NLMErrors errorCode = NLMErrors::MemoryAllocationFailed)
        : NLMException(message, errorCode) {}
};

class SimulationException : public NLMException {
public:
    SimulationException(const std::string& message, NLMErrors errorCode = NLMErrors::SimulationNotInitialized)
        : NLMException(message, errorCode) {}
};

class NeuronException : public NLMException {
public:
    NeuronException(const std::string& message, NLMErrors errorCode = NLMErrors::NeuronNotFound)
        : NLMException(message, errorCode) {}
};

class SynapseException : public NLMException {
public:
    SynapseException(const std::string& message, NLMErrors errorCode = NLMErrors::SynapseNotFound)
        : NLMException(message, errorCode) {}
};

class NeuromodulatorException : public NLMException {
public:
    NeuromodulatorException(const std::string& message, NLMErrors errorCode = NLMErrors::NeuromodulatorNotFound)
        : NLMException(message, errorCode) {}
};

class MemorySystemException : public NLMException {
public:
    MemorySystemException(const std::string& message, NLMErrors errorCode = NLMErrors::MemorySystemNotInitialized)
        : NLMException(message, errorCode) {}
};

class DevelopmentException : public NLMException {
public:
    DevelopmentException(const std::string& message, NLMErrors errorCode = NLMErrors::DevelopmentSystemNotInitialized)
        : NLMException(message, errorCode) {}
};

class EnvironmentException : public NLMException {
public:
    EnvironmentException(const std::string& message, NLMErrors errorCode = NLMErrors::EnvironmentNotInitialized)
        : NLMException(message, errorCode) {}
};

class CognitionException : public NLMException {
public:
    CognitionException(const std::string& message, NLMErrors errorCode = NLMErrors::CognitiveSystemNotInitialized)
        : NLMException(message, errorCode) {}
};

class PredictionException : public NLMException {
public:
    PredictionException(const std::string& message, NLMErrors errorCode = NLMErrors::PredictionSystemNotInitialized)
        : NLMException(message, errorCode) {}
};

// Error reporting and handling functions
void reportError(const std::string& context, const std::exception& e, bool throwException = true);

void reportError(NLMErrors errorCode, const std::string& context, const std::string& message, bool throwException = true);

// Error handling macros
#define NLM_ERROR_IF(condition, errorCode, message) \
    if (condition) { \
        nlm::reportError(errorCode, __func__, message, true); \
    }

#define NLM_THROW_ERROR(errorCode, message) \
    { \
        nlm::reportError(errorCode, __func__, message, true); \
    }

#define NLM_RETURN_ERROR(result, errorCode, message) \
    { \
        nlm::reportError(errorCode, __func__, message, false); \
        return result; \
    }

// Error handler interface
class IErrorHandler {
public:
    virtual ~IErrorHandler() = default;
    virtual void handleError(const NLMException& e) = 0;
    virtual bool shouldThrowOnError() const = 0;
};

// Default error handler
class DefaultErrorHandler : public IErrorHandler {
public:
    void handleError(const NLMException& e) override {
        std::cerr << "NLM Error: " << e.what() << " (Error Code: " << static_cast<int>(e.getErrorCode()) << ")" << std::endl;
    }
    
    bool shouldThrowOnError() const override {
        return true; // Default behavior: throw exceptions
    }
};

// Global error handler management
IErrorHandler* getGlobalErrorHandler();
void setGlobalErrorHandler(IErrorHandler* handler);

} // namespace nlm

