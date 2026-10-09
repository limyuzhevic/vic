// Error handling system for NLM neural simulation framework
// Provides comprehensive error classes and recovery mechanisms

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

namespace nlm {

// Base exception class for all NLM errors
class NLMException : public std::runtime_error {
public:
    enum class ErrorCategory {
        General,
        Memory,
        Validation,
        Configuration,
        IO,
        NeuralDynamics,
        Plasticity,
        MemorySystem,
        Cognitive,
        Prediction,
        Neuromodulation,
        Development,
        Checkpoint
    };
    
    NLMException(const std::string& message, ErrorCategory category = ErrorCategory::General,
                  int errorCode = -1, const std::string& details = "")
        : std::runtime_error(message), category_(category), errorCode_(errorCode), details_(details) {}
    
    ErrorCategory getCategory() const { return category_; }
    int getErrorCode() const { return errorCode_; }
    const std::string& getDetails() const { return details_; }
    
private:
    ErrorCategory category_;
    int errorCode_;
    std::string details_;
};

// Configuration validation errors
class ConfigValidationError : public NLMException {
public:
    ConfigValidationError(const std::string& message, const std::string& field = "",
                         const std::string& expected = "")
        : NLMException(message, ErrorCategory::Validation, -1, 
                      "Field: " + field + ", Expected: " + expected) {}
};

// Memory system errors
class MemorySystemError : public NLMException {
public:
    MemorySystemError(const std::string& message, const std::string& memoryType = "")
        : NLMException(message, ErrorCategory::Memory, -2,
                      "Memory type: " + memoryType) {}
};

// Neural dynamics errors
class NeuralDynamicsError : public NLMException {
public:
    NeuralDynamicsError(const std::string& message, const std::string& neuronType = "")
        : NLMException(message, ErrorCategory::NeuralDynamics, -3,
                      "Neuron type: " + neuronType) {}
};

// Plasticity errors
class PlasticityError : public NLMException {
public:
    PlasticityError(const std::string& message, const std::string& plasticityType = "")
        : NLMException(message, ErrorCategory::Plasticity, -4,
                      "Plasticity type: " + plasticityType) {}
};

// Cognitive system errors
class CognitiveError : public NLMException {
public:
    CognitiveError(const std::string& message, const std::string& cognitiveType = "")
        : NLMException(message, ErrorCategory::Cognitive, -5,
                      "Cognitive type: " + cognitiveType) {}
};

// Prediction system errors
class PredictionError : public NLMException {
public:
    PredictionError(const std::string& message, const std::string& predictionType = "")
        : NLMException(message, ErrorCategory::Prediction, -6,
                      "Prediction type: " + predictionType) {}
};

// Neuromodulation errors
class NeuromodulationError : public NLMException {
public:
    NeuromodulationError(const std::string& message, const std::string& neuromodulator = "")
        : NLMException(message, ErrorCategory::Neuromodulation, -7,
                      "Neuromodulator: " + neuromodulator) {}
};

// Development system errors
class DevelopmentError : public NLMException {
public:
    DevelopmentError(const std::string& message, const std::string& stage = "")
        : NLMException(message, ErrorCategory::Development, -8,
                      "Developmental stage: " + stage) {}
};

// Checkpoint errors
class CheckpointError : public NLMException {
public:
    CheckpointError(const std::string& message, const std::string& operation = "")
        : NLMException(message, ErrorCategory::Checkpoint, -9,
                      "Operation: " + operation) {}
};

// Error recovery utilities
class ErrorRecovery {
public:
    // Configuration recovery - attempts to restore defaults for invalid values
    static bool recoverConfig(std::shared_ptr<Config> config) {
        if (!config) return false;
        
        bool recovered = false;
        
        // Recover neuron count
        auto neuronCount = config->get<size_t>("neuron_count");
        if (!neuronCount || *neuronCount < MIN_NEURON_COUNT) {
            config->set("neuron_count", DEFAULT_NEURON_COUNT, ConfigSource::Default);
            recovered = true;
        }
        
        // Recover region count  
        auto regionCount = config->get<size_t>("region_count");
        if (!regionCount || *regionCount < 1) {
            config->set("region_count", 1, ConfigSource::Default);
            recovered = true;
        }
        
        // Recover connection probability
        auto connectionProb = config->get<float>("connection_probability");
        if (!connectionProb || connectionProb < 0.0f || connectionProb > 1.0f) {
            config->set("connection_probability", DEFAULT_CONNECTION_PROBABILITY, ConfigSource::Default);
            recovered = true;
        }
        
        return recovered;
    }
    
    // Simulation recovery - attempts to recover from simulation state
    static bool recoverFromSimulationError(Brain* brain, const std::string& errorType) {
        if (!brain) return false;
        
        try {
            if (errorType == "memory_overflow") {
                // Clear working memory and try to continue
                if (brain->getWorkingMemory()) {
                    brain->getWorkingMemory()->clear();
                }
                return true;
            } else if (errorType == "plasticity_error") {
                // Reset plasticity parameters
                auto* stdp = brain->getSTDP();
                if (stdp) {
                    stdp->configure(DEFAULT_STDP_LTP_WEIGHT, DEFAULT_STDP_LTD_WEIGHT, DEFAULT_STDP_TAU);
                }
                return true;
            } else if (errorType == "neuron_overflow") {
                // Reduce neuron count if possible
                auto* stdp = brain->getSTDP();
                if (stdp) {
                    stdp->setMaxNeurons(DEFAULT_NEURON_COUNT);
                }
                return true;
            }
        } catch (const std::exception& e) {
            // Recovery failed
            return false;
        }
        
        return false;
    }
    
    // Safe neural operation wrapper with error handling
    template<typename Func>
    static bool safeNeuralOperation(Func&& operation, const std::string& operationName) {
        try {
            operation();
            return true;
        } catch (const NLMException& e) {
            NLM_LOG_ERROR("Neural operation '" + operationName + "' failed: " + e.what());
            return false;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Neural operation '" + operationName + "' failed: " + std::string(e.what()));
            return false;
        }
    }
};

// Global error handler
class ErrorHandler {
public:
    static void setGlobalErrorHandler(std::function<void(const NLMException&)>&& handler) {
        globalHandler_ = std::move(handler);
    }
    
    static void handleError(const NLMException& error) {
        NLM_LOG_ERROR("NLM Error [Category: " + std::to_string(static_cast<int>(error.getCategory())) + 
                     ", Code: " + std::to_string(error.getErrorCode()) + "]: " + error.what());
        
        if (error.getDetails().empty()) {
            NLM_LOG_ERROR("Details: " + error.getDetails());
        }
        
        if (globalHandler_) {
            globalHandler_(error);
        }
    }
    
    static bool shouldRecover(const NLMException& error) {
        switch (error.getCategory()) {
            case NLMException::ErrorCategory::Validation:
            case NLMException::ErrorCategory::Configuration:
                return true; // Recoverable configuration errors
            case NLMException::ErrorCategory::Memory:
                return true; // Recoverable memory errors
            case NLMException::ErrorCategory::IO:
                return true; // Recoverable IO errors
            case NLMException::ErrorCategory::General:
                return error.getErrorCode() >= 0; // Non-fatal general errors
            default:
                return false; // Non-recoverable errors
        }
    }
    
private:
    static std::function<void(const NLMException&)> globalHandler_;
};

} // namespace nlm
