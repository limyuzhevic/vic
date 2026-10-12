// Error handling utilities for NLM
#pragma once

#include <string>
#include <stdexcept>
#include <vector>

namespace nlm {

// Base exception class for NLM errors
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error(message) {}
    
    virtual const char* what() const noexcept override {
        return std::runtime_error::what();
    }
};

// Configuration-specific errors
class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& message) 
        : NLMException("Config Error: " + message) {}
};

class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& message) 
        : ConfigError("Validation Error: " + message) {}
};

class ConfigFileError : public ConfigError {
public:
    explicit ConfigFileError(const std::string& message) 
        : ConfigError("File Error: " + message) {}
};

// Brain initialization errors
class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& message) 
        : NLMException("Brain Error: " + message) {}
};

class BrainInitializationError : public BrainError {
public:
    explicit BrainInitializationError(const std::string& message) 
        : BrainError("Initialization Error: " + message) {}
};

class BrainPlasticityError : public BrainError {
public:
    explicit BrainPlasticityError(const std::string& message) 
        : BrainError("Plasticity Error: " + message) {}
};

// World system errors
class WorldError : public NLMException {
public:
    explicit WorldError(const std::string& message) 
        : NLMException("World Error: " + message) {}
};

class WorldCollisionError : public WorldError {
public:
    explicit WorldCollisionError(const std::string& message) 
        : WorldError("Collision Error: " + message) {}
};

// Experiment errors
class ExperimentError : public NLMException {
public:
    explicit ExperimentError(const std::string& message) 
        : NLMException("Experiment Error: " + message) {}
};

class ExperimentRunError : public ExperimentError {
public:
    explicit ExperimentRunError(const std::string& message) 
        : ExperimentError("Run Error: " + message) {}
};

// Validation helper functions
class ValidationHelper {
public:
    // Validate configuration keys
    static bool isValidKey(const std::string& key) {
        if (key.empty()) return false;
        for (char c : key) {
            if (!std::isalnum(c) && c != '_' && c != '-') {
                return false;
            }
        }
        return true;
    }
    
    // Validate numeric ranges
    static bool isInRange(int64_t value, int64_t min, int64_t max) {
        return value >= min && value <= max;
    }
    
    static bool isInRange(double value, double min, double max) {
        return value >= min && value <= max && !std::isinf(value) && !std::isnan(value);
    }
    
    static bool isInRange(float value, float min, float max) {
        return value >= min && value <= max && !std::isinf(value) && !std::isnan(value);
    }
    
    // Validate string values
    static bool isNotEmpty(const std::string& value) {
        return !value.empty();
    }
    
    static bool isValidFilePath(const std::string& path) {
        // Basic validation - can be extended
        return !path.empty() && path.find('/') != std::string::npos || path.find('\\') != std::string::npos;
    }
    
    // Validate brain parameters
    static bool validateNeuronCount(size_t count) {
        return count >= ConfigConstants::MIN_NEURON_COUNT && 
               count <= ConfigConstants::MAX_NEURON_COUNT;
    }
    
    static bool validateRegionCount(size_t count) {
        return count >= ConfigConstants::MIN_REGION_COUNT && 
               count <= ConfigConstants::MAX_REGION_COUNT;
    }
    
    static bool validateConnectionProbability(float prob) {
        return prob >= ConfigConstants::MIN_CONNECTION_PROBABILITY && 
               prob <= ConfigConstants::MAX_CONNECTION_PROBABILITY;
    }
    
    static bool validateSimulationTimestep(double timestep) {
        return timestep >= ConfigConstants::MIN_SIMULATION_TIMESTEP && 
               timestep <= ConfigConstants::MAX_SIMULATION_TIMESTEP;
    }
    
    // Validate world parameters
    static bool validateWorldSize(float width, float height) {
        return width >= ConfigConstants::MIN_WORLD_SIZE && 
               width <= ConfigConstants::MAX_WORLD_SIZE &&
               height >= ConfigConstants::MIN_WORLD_SIZE && 
               height <= ConfigConstants::MAX_WORLD_SIZE;
    }
    
    static bool validateEnergy(float energy) {
        return energy >= ConfigConstants::MIN_ENERGY && 
               energy <= ConfigConstants::MAX_ENERGY;
    }
    
    // Validate experimental parameters
    static bool validateTrialCount(size_t count) {
        return count > 0 && count <= ConfigConstants::MAX_TRIALS;
    }
    
    static bool validateEpisodeCount(size_t count) {
        return count > 0 && count <= ConfigConstants::MAX_EPISODES;
    }
    
    // Validate object consistency
    static bool isConsistent(const std::string& key, const std::string& value) {
        // Custom validation rules for specific keys
        if (key == "random_seed") {
            try {
                int64_t seed = std::stoll(value);
                return seed >= ConfigConstants::MIN_RANDOM_SEED && 
                       seed <= ConfigConstants::MAX_RANDOM_SEED;
            } catch (...) {
                return false;
            }
        }
        return true;
    }
    
    // Get validation error messages
    static std::string getKeyValidationError(const std::string& key) {
        return "Invalid key: '" + key + "'. Keys must contain only alphanumeric characters, underscores, or hyphens.";
    }
    
    static std::string getRangeValidationError(const std::string& paramName, double actual, double min, double max) {
        return "Invalid value for " + paramName + ": " + std::to_string(actual) + ". Must be between " + 
               std::to_string(min) + " and " + std::to_string(max) + ".";
    }
    
    static std::string getFileError(const std::string& path) {
        return "Cannot access file: '" + path + "'. Please check file permissions and path.";
    }
    
    static std::string getBrainInitializationError(const std::string& reason) {
        return "Brain initialization failed: " + reason;
    }
    
    static std::string getWorldError(const std::string& reason) {
        return "World operation failed: " + reason;
    }
    
    static std::string getExperimentError(const std::string& reason) {
        return "Experiment failed: " + reason;
    }
};

// Smart pointer with ownership tracking
class SafePointer {
public:
    template<typename T>
    static std::shared_ptr<T> createSafe(const std::string& name, std::shared_ptr<T> ptr) {
        if (!ptr) {
            throw BrainError("Cannot create safe pointer: " + name + " is null");
        }
        return ptr;
    }
    
    template<typename T>
    static T* getRaw(std::shared_ptr<T> ptr, const std::string& name) {
        if (!ptr) {
            throw BrainError("Cannot get raw pointer: " + name + " is null");
        }
        return ptr.get();
    }
};

// Resource management helper
class ResourceManager {
public:
    static void validateResource(const std::string& name, bool isValid, const std::string& errorMsg) {
        if (!isValid) {
            throw BrainError("Resource validation failed for " + name + ": " + errorMsg);
        }
    }
    
    static void validateResourceExists(const std::string& name, bool exists) {
        if (!exists) {
            throw BrainError("Resource does not exist: " + name);
        }
    }
    
    static void validateResourceCapacity(const std::string& name, size_t used, size_t capacity, const std::string& resourceType) {
        if (used > capacity) {
            throw BrainError("Resource capacity exceeded for " + name + ": " + std::to_string(used) + 
                           " > " + std::to_string(capacity) + " " + resourceType);
        }
    }
};

// Error recovery helper
class ErrorRecovery {
public:
    // Try to recover from a configuration error by providing a fallback
    template<typename T>
    static T recoverWithFallback(const std::string& paramName, const T& fallback, 
                                std::function<bool(const T&)> validator,
                                std::function<T(const std::string&)> parser) {
        try {
            T value = parser(paramName);
            if (validator(value)) {
                return value;
            } else {
                throw ConfigValidationError("Value validation failed for " + paramName);
            }
        } catch (...) {
            // Use fallback value
            if (validator(fallback)) {
                return fallback;
            } else {
                throw ConfigValidationError("Fallback value validation failed for " + paramName);
            }
        }
    }
    
    // Safe file operations with error recovery
    static bool safeFileOperation(const std::string& filepath, 
                                 std::function<bool(const std::string&)> operation,
                                 const std::string& operationName) {
        try {
            return operation(filepath);
        } catch (const std::exception& e) {
            // Log error and attempt recovery
            throw ConfigFileError("Failed to " + operationName + " file '" + filepath + 
                                "': " + std::string(e.what()));
        }
    }
};

} // namespace nlm