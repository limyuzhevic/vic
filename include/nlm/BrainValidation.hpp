// Enhanced validation and error handling for NLM brain operations
#ifndef BRAIN_VALIDATION_HPP
#define BRAIN_VALIDATION_HPP

#include "../core/Logger/Logger.hpp"
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace nlm {

class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& message) 
        : std::runtime_error("Validation Error: " + message) {}
};

class BrainValidation {
public:
    // Validate neuron creation parameters
    static void validateNeuronCreation(const std::string& type, float threshold, float decayRate) {
        if (type.empty()) {
            throw ValidationError("Neuron type cannot be empty");
        }
        if (threshold < 0.0f || threshold > 1.0f) {
            throw ValidationError("Threshold must be between 0.0 and 1.0");
        }
        if (decayRate < 0.0f || decayRate > 1.0f) {
            throw ValidationError("Decay rate must be between 0.0 and 1.0");
        }
    }
    
    // Validate connection parameters
    static void validateConnection(NeuronId source, NeuronId target, float weight) {
        if (source == target) {
            throw ValidationError("Source and target neurons cannot be the same");
        }
        if (weight < -1.0f || weight > 1.0f) {
            throw ValidationError("Connection weight must be between -1.0 and 1.0");
        }
    }
    
    // Validate region parameters
    static void validateRegion(const std::string& name, size_t neuronCount) {
        if (name.empty()) {
            throw ValidationError("Region name cannot be empty");
        }
        if (neuronCount == 0) {
            throw ValidationError("Region must have at least one neuron");
        }
        if (neuronCount > 1000000) {  // Reasonable upper limit
            throw ValidationError("Region neuron count exceeds maximum limit (1,000,000)");
        }
    }
    
    // Validate simulation parameters
    static void validateSimulationParameters(double timestep, size_t steps) {
        if (timestep <= 0.0) {
            throw ValidationError("Time step must be positive");
        }
        if (timestep > 1.0) {
            NLM_LOG_WARNING("Large time step may affect simulation accuracy: " + std::to_string(timestep));
        }
        if (steps == 0) {
            throw ValidationError("Step count must be positive");
        }
        if (steps > 1000000) {
            throw ValidationError("Step count exceeds maximum limit (1,000,000)");
        }
    }
    
    // Validate world parameters
    static void validateWorldParameters(float width, float height, float visionWidth, float visionHeight) {
        if (width <= 0.0f) {
            throw ValidationError("World width must be positive");
        }
        if (height <= 0.0f) {
            throw ValidationError("World height must be positive");
        }
        if (visionWidth <= 0.0f || visionHeight <= 0.0f) {
            throw ValidationError("Vision dimensions must be positive");
        }
        if (visionWidth > width || visionHeight > height) {
            throw ValidationError("Vision dimensions cannot exceed world dimensions");
        }
    }
    
    // Validate memory system parameters
    static void validateMemoryParameters(float capacity, float decayRate) {
        if (capacity < 0.0f || capacity > 1000.0f) {
            throw ValidationError("Memory capacity must be between 0.0 and 1000.0");
        }
        if (decayRate < 0.0f || decayRate > 1.0f) {
            throw ValidationError("Memory decay rate must be between 0.0 and 1.0");
        }
    }
    
    // Validate neuromodulation parameters
    static void validateNeuromodulationParameters(float baseline, float range) {
        if (baseline < -1.0f || baseline > 1.0f) {
            throw ValidationError("Neuromodulation baseline must be between -1.0 and 1.0");
        }
        if (range < 0.0f || range > 2.0f) {
            throw ValidationError("Neuromodulation range must be between 0.0 and 2.0");
        }
    }
    
    // Validate plasticity parameters
    static void validatePlasticityParameters(float learningRate, float stability) {
        if (learningRate < 0.0f || learningRate > 1.0f) {
            throw ValidationError("Plasticity learning rate must be between 0.0 and 1.0");
        }
        if (stability < 0.0f || stability > 1.0f) {
            throw ValidationError("Plasticity stability must be between 0.0 and 1.0");
        }
    }
    
    // Generic validation helper for arrays
    template<typename T>
    static void validateArray(const std::vector<T>& arr, const std::string& name, 
                             size_t minSize = 1, size_t maxSize = 1000000) {
        if (arr.size() < minSize) {
            throw ValidationError(name + " array size too small: " + std::to_string(arr.size()));
        }
        if (arr.size() > maxSize) {
            throw ValidationError(name + " array size too large: " + std::to_string(arr.size()));
        }
        
        for (size_t i = 0; i < arr.size(); ++i) {
            if (std::isnan(arr[i]) || std::isinf(arr[i])) {
                throw ValidationError(name + " array contains invalid values at index " + std::to_string(i));
            }
        }
    }
    
    // Validate configuration consistency
    static void validateConfigurationConsistency(const BrainConfig& config) {
        // Check for logical consistency
        if (config.enableDevelopment && config.enableCuriosity) {
            NLM_LOG_WARNING("Both development and curiosity enabled - may affect simulation behavior");
        }
        
        // Validate that neuromodulation is enabled when needed
        if (config.enableRewardModulation && !config.enableNeuromodulation) {
            NLM_LOG_WARNING("Reward modulation enabled but neuromodulation is disabled");
        }
        
        // Validate that memory systems are configured appropriately
        if (config.useEpisodicMemory && !config.useWorkingMemory) {
            NLM_LOG_WARNING("Episodic memory enabled but working memory is disabled");
        }
    }
    
    // Safe parameter extraction with validation
    template<typename T>
    static T safeExtract(const std::unordered_map<std::string, T>& params, 
                        const std::string& key, T defaultValue) {
        auto it = params.find(key);
        if (it != params.end()) {
            T value = it->second;
            // Additional validation based on type
            if constexpr (std::is_floating_point_v<T>) {
                if (std::isnan(value) || std::isinf(value)) {
                    throw ValidationError("Invalid parameter value for key: " + key);
                }
            }
            return value;
        }
        return defaultValue;
    }
    
    // Validation result with detailed information
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        ValidationResult() : valid(true) {}
        
        void addError(const std::string& error) {
            valid = false;
            errors.push_back(error);
            NLM_LOG_ERROR(error);
        }
        
        void addWarning(const std::string& warning) {
            warnings.push_back(warning);
            NLM_LOG_WARNING(warning);
        }
    };
};

// Enhanced configuration validation with detailed feedback
class ConfigValidator {
public:
    static ValidationResult validateFullConfiguration(const BrainConfig& config) {
        ValidationResult result;
        
        // Validate basic parameters
        if (config.neuronCount == 0) {
            result.addError("Neuron count cannot be zero");
        }
        
        if (config.simulationSteps == 0) {
            result.addError("Simulation steps cannot be zero");
        }
        
        // Validate derived parameters
        float expectedMemoryUsage = config.neuronCount * sizeof(NeuronState);
        if (expectedMemoryUsage > 100.0f * 1024 * 1024) {  // 100MB limit
            result.addWarning("Expected memory usage may exceed system limits");
        }
        
        // Validate time step consistency
        float maxExpectedTimestep = 1.0f / config.neuronCount;
        if (config.timestep > maxExpectedTimestep * 10) {
            result.addWarning("Time step may be too large for neuron count");
        }
        
        return result;
    }
    
    // Validate component configuration
    static ValidationResult validateComponentConfiguration(const ComponentConfig& component) {
        ValidationResult result;
        
        if (component.enabled && component.priority < 0.0f) {
            result.addError("Enabled component cannot have negative priority");
        }
        
        if (component.enabled && component.updateFrequency <= 0.0f) {
            result.addError("Enabled component must have positive update frequency");
        }
        
        return result;
    }
    
    // Validate experimental setup
    static ValidationResult validateExperimentalSetup(const ExperimentSetup& setup) {
        ValidationResult result;
        
        if (setup.repetitions <= 0) {
            result.addError("Experiment repetitions must be positive");
        }
        
        if (setup.duration <= 0.0f) {
            result.addError("Experiment duration must be positive");
        }
        
        if (setup.duration > 3600.0f) {  // 1 hour limit
            result.addWarning("Experiment duration exceeds reasonable limit (1 hour)");
        }
        
        return result;
    }
};

// Exception-safe validation macros for easier debugging
#define VALIDATE(condition, message) \
    do { \
        if (!(condition)) { \
            throw ValidationError(message); \
        } \
    } while(0)

#define VALIDATE_ARRAY(arr, minSize, maxSize, name) \
    do { \
        if ((arr).size() < (minSize) || (arr).size() > (maxSize)) { \
            throw ValidationError(name " array size validation failed"); \
        } \
    } while(0)

} // namespace nlm

#endif // BRAIN_VALIDATION_HPP
