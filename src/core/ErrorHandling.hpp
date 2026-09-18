#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <iostream>

namespace nlm {

// Base exception class for NLM
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error(message) {}
    
    NLMException(const std::string& message, const std::string& context)
        : std::runtime_error(formatMessage(message, context)) {}
    
protected:
    static std::string formatMessage(const std::string& message, const std::string& context) {
        std::ostringstream oss;
        oss << "NLM Error: " << message;
        if (!context.empty()) {
            oss << " (Context: " << context << ")";
        }
        return oss.str();
    }
};

// Exception for configuration errors
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message) 
        : NLMException(message) {}
    
    ConfigException(const std::string& message, const std::string& key)
        : NLMException(message, "Key: " + key) {}
};

// Exception for brain initialization errors
class BrainInitializationException : public NLMException {
public:
    explicit BrainInitializationException(const std::string& message)
        : NLMException(message) {}
    
    BrainInitializationException(const std::string& message, size_t neurons, size_t regions)
        : NLMException(message, "Neurons: " + std::to_string(neurons) + ", Regions: " + std::to_string(regions)) {}
};

// Exception for neural dynamics errors
class NeuralDynamicsException : public NLMException {
public:
    explicit NeuralDynamicsException(const std::string& message)
        : NLMException(message) {}
    
    NeuralDynamicsException(const std::string& message, NeuronType type)
        : NLMException(message, "Neuron type: " + std::to_string(static_cast<int>(type))) {}
};

// Exception for memory system errors
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message)
        : NLMException(message) {}
    
    MemoryException(const std::string& message, size_t capacity)
        : NLMException(message, "Capacity: " + std::to_string(capacity)) {}
};

// Exception for plasticity errors
class PlasticityException : public NLMException {
public:
    explicit PlasticityException(const std::string& message)
        : NLMException(message) {}
    
    PlasticityException(const std::string& message, const std::string& rule)
        : NLMException(message, "Rule: " + rule) {}
};

// Exception for spike system errors
class SpikeSystemException : public NLMException {
public:
    explicit SpikeSystemException(const std::string& message)
        : NLMException(message) {}
    
    SpikeSystemException(const std::string& message, SimulationStep step)
        : NLMException(message, "Step: " + std::to_string(step)) {}
};

// Exception for reward-modulated STDP
class RewardModulatedSTDPException : public PlasticityException {
public:
    explicit RewardModulatedSTDPException(const std::string& message)
        : PlasticityException(message) {}
    
    RewardModulatedSTDPException(const std::string& message, float reward, float predicted)
        : PlasticityException(message, "Reward: " + std::to_string(reward) + ", Predicted: " + std::to_string(predicted)) {}
};

// Exception for neuromodulation errors
class NeuromodulationException : public NLMException {
public:
    explicit NeuromodulationException(const std::string& message)
        : NLMException(message) {}
    
    NeuromodulationException(const std::string& message, const std::string& modulator)
        : NLMException(message, "Modulator: " + modulator) {}
};

// Exception for development errors
class DevelopmentException : public NLMException {
public:
    explicit DevelopmentException(const std::string& message)
        : NLMException(message) {}
    
    DevelopmentException(const std::string& message, const std::string& stage)
        : NLMException(message, "Stage: " + stage) {}
};

// Exception for world/environment errors
class WorldException : public NLMException {
public:
    explicit WorldException(const std::string& message)
        : NLMException(message) {}
    
    WorldException(const std::string& message, const std::string& objectType)
        : NLMException(message, "Object type: " + objectType) {}
};

// Error handling utilities
class ErrorHandler {
public:
    // Log error with context
    static void logError(const std::string& message, const std::string& context = "") {
        std::string formattedMessage = formatErrorMessage(message, context);
        logToSystem(formattedMessage);
    }
    
    // Log warning with context
    static void logWarning(const std::string& message, const std::string& context = "") {
        std::string formattedMessage = formatWarningMessage(message, context);
        logToSystem(formattedMessage);
    }
    
    // Log information with context
    static void logInfo(const std::string& message, const std::string& context = "") {
        std::string formattedMessage = formatInfoMessage(message, context);
        logToSystem(formattedMessage);
    }
    
    // Handle configuration error gracefully
    static void handleConfigError(const std::string& key, const std::string& value, 
                                 const std::string& expected) {
        logWarning("Invalid configuration value", 
                  "Key: " + key + ", Value: " + value + ", Expected: " + expected);
    }
    
    // Handle brain initialization error
    static void handleBrainInitError(size_t neurons, size_t regions) {
        logError("Brain initialization failed", 
                "Neurons: " + std::to_string(neurons) + ", Regions: " + std::to_string(regions));
    }
    
    // Handle neural dynamics error
    static void handleNeuralDynamicsError(NeuronType type, const std::string& operation) {
        logError("Neural dynamics operation failed", 
                "Type: " + std::to_string(static_cast<int>(type)) + ", Operation: " + operation);
    }
    
    // Handle memory system error
    static void handleMemoryError(const std::string& operation, size_t capacity) {
        logError("Memory system error", 
                "Operation: " + operation + ", Capacity: " + std::to_string(capacity));
    }
    
    // Handle plasticity error
    static void handlePlasticityError(const std::string& rule, const std::string& details) {
        logError("Plasticity error", "Rule: " + rule + ", Details: " + details);
    }
    
    // Handle spike system error
    static void handleSpikeError(SimulationStep step, const std::string& issue) {
        logError("Spike system error", "Step: " + std::to_string(step) + ", Issue: " + issue);
    }
    
    // Handle neuromodulation error
    static void handleNeuromodulationError(const std::string& modulator, float value) {
        logError("Neuromodulation error", 
                "Modulator: " + modulator + ", Value: " + std::to_string(value));
    }
    
    // Handle development error
    static void handleDevelopmentError(const std::string& stage, float age) {
        logError("Development error", "Stage: " + stage + ", Age: " + std::to_string(age));
    }
    
    // Handle world/environment error
    static void handleWorldError(const std::string& objectType, const std::string& operation) {
        logError("World error", "Type: " + objectType + ", Operation: " + operation);
    }
    
    // Create and throw exception
    static void throwConfigException(const std::string& message, const std::string& key = "") {
        if (!key.empty()) {
            throw ConfigException(message, key);
        } else {
            throw ConfigException(message);
        }
    }
    
    static void throwBrainInitException(const std::string& message, size_t neurons, size_t regions = 1) {
        throw BrainInitializationException(message, neurons, regions);
    }
    
    static void throwNeuralDynamicsException(const std::string& message, NeuronType type) {
        throw NeuralDynamicsException(message, type);
    }
    
    static void throwMemoryException(const std::string& message, size_t capacity = 0) {
        throw MemoryException(message, capacity);
    }
    
    static void throwPlasticityException(const std::string& message, const std::string& rule = "") {
        throw PlasticityException(message, rule);
    }
    
    static void throwSpikeSystemException(const std::string& message, SimulationStep step = 0) {
        throw SpikeSystemException(message, step);
    }
    
    static void throwRewardModulatedSTDPException(const std::string& message, float reward, float predicted) {
        throw RewardModulatedSTDPException(message, reward, predicted);
    }
    
    static void throwNeuromodulationException(const std::string& message, const std::string& modulator = "") {
        throw NeuromodulationException(message, modulator);
    }
    
    static void throwDevelopmentException(const std::string& message, const std::string& stage = "") {
        throw DevelopmentException(message, stage);
    }
    
    static void throwWorldException(const std::string& message, const std::string& objectType = "") {
        throw WorldException(message, objectType);
    }
    
    // Try-catch utility
    template<typename T>
    std::optional<T> tryExecute(std::function<T()> operation, const std::string& errorContext = "") {
        try {
            return operation();
        } catch (const std::exception& e) {
            logError("Operation failed", errorContext + ": " + std::string(e.what()));
            return std::nullopt;
        }
    }
    
    // Safe configuration get with error handling
    template<typename T>
    std::optional<T> safeGet(const Config& config, const std::string& key, 
                           const T& defaultValue = T{}) {
        try {
            auto value = config.get<T>(key);
            if (value.has_value()) {
                return value;
            }
            return defaultValue;
        } catch (const std::exception& e) {
            logWarning("Failed to get configuration value", "Key: " + key + ", Error: " + std::string(e.what()));
            return defaultValue;
        }
    }
    
    // Safe configuration set with validation
    static bool safeSet(Config& config, const std::string& key, const ConfigValue& value, 
                       ConfigSource source = ConfigSource::Runtime) {
        try {
            config.set(key, value, source);
            return true;
        } catch (const std::exception& e) {
            logError("Failed to set configuration", "Key: " + key + ", Error: " + std::string(e.what()));
            return false;
        }
    }
    
private:
    static std::string formatErrorMessage(const std::string& message, const std::string& context) {
        std::ostringstream oss;
        oss << "[ERROR] " << message;
        if (!context.empty()) {
            oss << " (Context: " << context << ")";
        }
        return oss.str();
    }
    
    static std::string formatWarningMessage(const std::string& message, const std::string& context) {
        std::ostringstream oss;
        oss << "[WARNING] " << message;
        if (!context.empty()) {
            oss << " (Context: " << context << ")";
        }
        return oss.str();
    }
    
    static std::string formatInfoMessage(const std::string& message, const std::string& context) {
        std::ostringstream oss;
        oss << "[INFO] " << message;
        if (!context.empty()) {
            oss << " (Context: " << context << ")";
        }
        return oss.str();
    }
    
    static void logToSystem(const std::string& message) {
        // In production, this would use the Logger class
        // For now, output to stderr for debugging
        std::cerr << message << std::endl;
    }
};

// Macro for consistent error handling
#define NLM_ERROR_HANDLER(operation, context) \
    try { \
        operation; \
    } catch (const std::exception& e) { \
        ErrorHandler::logError(#operation, context + std::string(" - ") + std::string(e.what())); \
    }

#define NLM_THROW_CONFIG_ERROR(message, key) \
    ErrorHandler::throwConfigException(message, key)

#define NLM_THROW_BRAIN_INIT_ERROR(message, neurons) \
    ErrorHandler::throwBrainInitException(message, neurons)

#define NLM_THROW_NEURAL_DYNAMICS_ERROR(message, type) \
    ErrorHandler::throwNeuralDynamicsException(message, type)

#define NLM_THROW_MEMORY_ERROR(message, capacity) \
    ErrorHandler::throwMemoryException(message, capacity)

#define NLM_THROW_PLASTICITY_ERROR(message, rule) \
    ErrorHandler::throwPlasticityException(message, rule)

#define NLM_THROW_SPIKE_SYSTEM_ERROR(message, step) \
    ErrorHandler::throwSpikeSystemException(message, step)

#define NLM_THROW_REWARD_MODULATED_STDP_ERROR(message, reward, predicted) \
    ErrorHandler::throwRewardModulatedSTDPException(message, reward, predicted)

#define NLM_THROW_NEUROMODULATION_ERROR(message, modulator) \
    ErrorHandler::throwNeuromodulationException(message, modulator)

#define NLM_THROW_DEVELOPMENT_ERROR(message, stage) \
    ErrorHandler::throwDevelopmentException(message, stage)

#define NLM_THROW_WORLD_ERROR(message, objectType) \
    ErrorHandler::throwWorldException(message, objectType)

} // namespace nlm
