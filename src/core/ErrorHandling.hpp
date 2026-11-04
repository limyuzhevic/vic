// Error handling and validation utilities for NLM
//
// This module provides comprehensive error handling, validation,
// and exception safety for the NLM neural simulation framework.

#pragma once

#include <string>
#include <stdexcept>
#include <vector>
#include <map>
#include <unordered_set>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <memory>
#include <type_traits>

namespace nlm {

// Error categories for NLM errors
enum class NLMErrorCategory {
    Configuration,
    Brain,
    Memory,
    Plasticity,
    Neuromodulation,
    Simulation,
    Development,
    FileIO,
    ResourceAllocation,
    InvalidArgument,
    NotImplemented,
    Internal
};

// Custom exception class for NLM errors
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, NLMErrorCategory category = NLMErrorCategory::Internal,
                  const std::string& details = "", int errorCode = 0)
        : std::runtime_error(message), category_(category), details_(details), errorCode_(errorCode) {}
    
    NLMErrorCategory getCategory() const { return category_; }
    const std::string& getDetails() const { return details_; }
    int getErrorCode() const { return errorCode_; }
    
    // Convert to string representation
    std::string toString() const {
        std::ostringstream oss;
        oss << "NLM Error [" << static_cast<int>(category_) << "] Code: " << errorCode_ << ": " << what();
        if (!details_.empty()) {
            oss << " Details: " << details_;
        }
        return oss.str();
    }
    
    // Print to standard error
    void printError() const {
        std::cerr << toString() << std::endl;
    }

private:
    NLMErrorCategory category_;
    std::string details_;
    int errorCode_;
};

// Validation exceptions
class ValidationError : public NLMException {
public:
    ValidationError(const std::string& message, const std::string& field = "",
                    const std::string& expected = "", const std::string& actual = "")
        : NLMException(message, NLMErrorCategory::InvalidArgument), field_(field),
          expected_(expected), actual_(actual) {}
    
    const std::string& getField() const { return field_; }
    const std::string& getExpected() const { return expected_; }
    const std::string& getActual() const { return actual_; }

private:
    std::string field_;
    std::string expected_;
    std::string actual_;
};

// Configuration validation utilities
class ConfigValidator {
public:
    static void validateNeuronCount(int64_t neuronCount) {
        if (neuronCount <= 0) {
            throw ValidationError("Invalid neuron count", "neuron_count",
                                "positive integer", std::to_string(neuronCount));
        }
        if (neuronCount > 1000000) {
            throw ValidationError("Neuron count too large", "neuron_count",
                                "less than or equal to 1000000", std::to_string(neuronCount));
        }
    }
    
    static void validateConnectionProbability(float probability) {
        if (probability < 0.0f || probability > 1.0f) {
            throw ValidationError("Invalid connection probability", "connection_probability",
                                "between 0.0 and 1.0", std::to_string(probability));
        }
    }
    
    static void validateTimeStep(double timestep) {
        if (timestep <= 0.0) {
            throw ValidationError("Invalid time step", "simulation_timestep",
                                "positive value", std::to_string(timestep));
        }
        if (timestep > 0.1) {
            throw ValidationError("Time step too large", "simulation_timestep",
                                "less than or equal to 0.1", std::to_string(timestep));
        }
    }
    
    static void validatePlasticityParameters(float ltp, float ltd, float tau) {
        if (ltp < 0.0f || ltd < 0.0f) {
            throw ValidationError("Invalid plasticity parameters", "stdp weights",
                                "positive values", "LTP: " + std::to_string(ltp) + ", LTD: " + std::to_string(ltd));
        }
        if (tau <= 0.0f) {
            throw ValidationError("Invalid STDP tau", "stdp_tau", "positive value", std::to_string(tau));
        }
    }
    
    static void validateConfigValue(const std::string& key, const ConfigValue& value) {
        // Type-specific validation
        std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                if (val != val) throw ValidationError("NaN found in configuration", key);
                if (std::isinf(val)) throw ValidationError("Infinity found in configuration", key);
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                for (size_t i = 0; i < val.size(); ++i) {
                    if (val[i] != val[i]) throw ValidationError("NaN found in configuration array", key + ".[" + std::to_string(i) + "]");
                    if (std::isinf(val[i])) throw ValidationError("Infinity found in configuration array", key + ".[" + std::to_string(i) + "]");
                }
            }
        }, value);
    }
};

// Resource management and memory validation
class ResourceManager {
public:
    struct ResourceLimits {
        size_t maxNeurons = 1000000;
        size_t maxSynapses = 10000000;
        size_t maxMemoryMB = 2048;
        size_t maxFileHandles = 100;
        size_t maxThreads = 8;
        
        // Check if resources are available
        bool checkResources(const ResourceLimits& current) const {
            return current.maxNeurons <= maxNeurons &&
                   current.maxSynapses <= maxSynapses &&
                   current.maxMemoryMB <= maxMemoryMB &&
                   current.maxFileHandles <= maxFileHandles &&
                   current.maxThreads <= maxThreads;
        }
    };
    
    // Check available system resources
    static ResourceLimits getSystemResourceLimits() {
        ResourceLimits limits;
        // Implementation would depend on the target platform
        // For now, return default limits
        return limits;
    }
    
    // Allocate resources for brain simulation
    static void allocateBrainResources(size_t neuronCount, size_t synapseCount) {
        ResourceLimits limits = getSystemResourceLimits();
        
        if (neuronCount > limits.maxNeurons) {
            throw NLMException("Not enough memory for neurons", NLMErrorCategory::ResourceAllocation,
                              "Requested: " + std::to_string(neuronCount) + ", Available: " + std::to_string(limits.maxNeurons));
        }
        
        // Estimate memory requirements
        size_t estimatedMemoryMB = (neuronCount * 100 + synapseCount * 200) / (1024 * 1024);
        if (estimatedMemoryMB > limits.maxMemoryMB) {
            throw NLMException("Not enough memory", NLMErrorCategory::ResourceAllocation,
                              "Estimated: " + std::to_string(estimatedMemoryMB) + "MB, Available: " + std::to_string(limits.maxMemoryMB) + "MB");
        }
    }
};

// Simulation state validation
class SimulationValidator {
public:
    static void validateSimulationStep(SimulationStep step, Timestamp time) {
        if (step < 0) {
            throw ValidationError("Invalid simulation step", "step", "non-negative integer", std::to_string(step));
        }
        if (time < 0.0) {
            throw ValidationError("Invalid simulation time", "time", "non-negative value", std::to_string(time));
        }
    }
    
    static void validateBrainState(const Brain& brain) {
        // Check brain consistency
        size_t neuronCount = brain.getTotalNeuronCount();
        size_t synapseCount = brain.getTotalSynapseCount();
        
        if (neuronCount == 0 && synapseCount > 0) {
            throw ValidationError("Brain in inconsistent state", "neuron_count vs synapse_count",
                                "neuron_count should be 0 when synapse_count > 0");
        }
        
        // Check firing rate bounds
        float firingRate = brain.getAverageFiringRate();
        if (firingRate < 0.0f || firingRate > 10000.0f) {
            throw ValidationError("Invalid firing rate", "average_firing_rate",
                                "between 0.0 and 10000.0 Hz", std::to_string(firingRate));
        }
    }
};

// Error recovery and cleanup utilities
class ErrorRecovery {
public:
    // Safe cleanup for brain state
    static void safeBrainCleanup(std::shared_ptr<Brain> brain) {
        try {
            if (brain) {
                // Try to reset brain state cleanly
                brain->reset();
            }
        } catch (...) {
            // If cleanup fails, at least try to release memory
            // The brain's RAII destructors should handle this
        }
    }
    
    // Safe file operations with error handling
    static bool safeFileWrite(const std::string& filepath, const std::string& content) {
        try {
            std::ofstream file(filepath, std::ios::out | std::ios::binary);
            if (!file) {
                return false;
            }
            file << content;
            if (!file.good()) {
                return false;
            }
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    // Safe file read with error handling
    static std::string safeFileRead(const std::string& filepath) {
        try {
            std::ifstream file(filepath, std::ios::in | std::ios::binary);
            if (!file) {
                throw NLMException("Cannot open file for reading", NLMErrorCategory::FileIO, filepath);
            }
            
            file.seekg(0, std::ios::end);
            size_t fileSize = file.tellg();
            file.seekg(0, std::ios::beg);
            
            std::string content(fileSize, '\0');
            if (!file.read(&content[0], fileSize)) {
                throw NLMException("Failed to read file", NLMErrorCategory::FileIO, filepath);
            }
            
            return content;
        } catch (const std::exception& e) {
            throw NLMException("File read error", NLMErrorCategory::FileIO, filepath + ": " + e.what());
        }
    }
};

// Validation and error reporting utilities
class ErrorReporter {
public:
    // Log error with context information
    static void logError(const NLMException& error, const std::string& context = "") {
        std::cerr << "[NLM Error]";
        if (!context.empty()) {
            std::cerr << " Context: " << context;
        }
        std::cerr << " " << error.toString() << std::endl;
    }
    
    // Log validation error with details
    static void logValidationError(const ValidationError& error, const std::string& context = "") {
        std::cerr << "[NLM Validation Error]";
        if (!context.empty()) {
            std::cerr << " Context: " << context;
        }
        std::cerr << " Field: " << error.getField();
        if (!error.getExpected().empty()) {
            std::cerr << " Expected: " << error.getExpected();
        }
        if (!error.getActual().empty()) {
            std::cerr << " Actual: " << error.getActual();
        }
        std::cerr << ". Message: " << error.what() << std::endl;
    }
    
    // Check if error is recoverable
    static bool isRecoverable(const NLMException& error) {
        switch (error.getCategory()) {
            case NLMErrorCategory::InvalidArgument:
            case NLMErrorCategory::NotImplemented:
                return false;
            case NLMErrorCategory::ResourceAllocation:
                return true; // Can try to free resources and retry
            case NLMErrorCategory::FileIO:
                return true; // Can try to check permissions, disk space, etc.
            case NLMErrorCategory::Internal:
                return false; // Internal errors are usually not recoverable
            default:
                return true;
        }
    }
};

} // namespace nlm
