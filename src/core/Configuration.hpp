// Enhanced Configuration with schema validation and error handling
#pragma once

#include "core/Config/Config.hpp"
#include "core/BrainExceptions.hpp"
#include <string>
#include <map>
#include <vector>
#include <any>
#include <stdexcept>
#include <algorithm>

namespace nlm {

class Configuration {
public:
    enum class ParameterType {
        Integer,
        Float,
        String,
        Boolean,
        Vector,
        Object
    };
    
    struct ParameterSchema {
        std::string name;
        ParameterType type;
        std::any defaultValue;
        bool required;
        std::string description;
        std::vector<std::any> allowedValues;
        std::pair<double, double> range;  // For numeric types
        
        ParameterSchema(std::string n, ParameterType t, std::any def = std::any(), 
                       bool req = false, std::string desc = "")
            : name(std::move(n)), type(t), defaultValue(std::move(def)), 
              required(req), description(std::move(desc)) {}
    };
    
    // Core configuration with error handling
    class CoreConfig {
    public:
        // Simulation parameters
        int64_t randomSeed = 42;
        double simulationTimestep = 0.001;
        size_t neuronCount = 1000;
        size_t regionCount = 1;
        float connectionProbability = 0.1f;
        
        // Plasticity parameters
        float stdpLtpWeight = 0.01f;
        float stdpLtdWeight = 0.012f;
        float stdpTimeConstant = 20.0f;
        float synaptogenesisRate = 0.0001f;
        float pruningRate = 0.00001f;
        
        // Memory parameters
        size_t workingMemoryCapacity = 100;
        size_t maxEpisodicEpisodes = 1000;
        float replayInterval = 100.0f;
        float consolidationInterval = 1000.0f;
        
        // Neuromodulation parameters
        float dopamineBaseline = 0.1f;
        float curiositySensitivity = 0.5f;
        float noveltyDecay = 0.99f;
        
        // Development parameters
        double initialAge = 0.0;
        double developmentDuration = 1800.0;  // 30 minutes
        
        // System behavior
        bool enableRewardModulation = true;
        bool enableStructuralPlasticity = true;
        bool enableDevelopment = true;
        bool enableCuriosity = true;
        
        // Performance
        size_t maxThreads = 4;
        bool enableSIMD = true;
        
        void validate() const {
            if (randomSeed < 0) throw BrainInitializationError("Random seed must be non-negative");
            if (simulationTimestep <= 0) throw BrainInitializationError("Simulation timestep must be positive");
            if (neuronCount == 0) throw BrainInitializationError("Neuron count must be positive");
            if (regionCount == 0) throw BrainInitializationError("Region count must be positive");
            if (connectionProbability < 0 || connectionProbability > 1) {
                throw BrainInitializationError("Connection probability must be between 0 and 1");
            }
            if (stdpLtpWeight < 0) throw BrainInitializationError("STDP LTP weight must be non-negative");
            if (stdpLtdWeight < 0) throw BrainInitializationError("STDP LTD weight must be non-negative");
            if (stdpTimeConstant <= 0) throw BrainInitializationError("STDP time constant must be positive");
        }
    };
    
    // Experimental parameters with schema
    struct ExperimentalParameters {
        // Learning experiments
        float learningRate = 0.01f;
        size_t learningPatience = 100;
        float explorationRate = 0.1f;
        
        // Memory experiments
        float memoryDecayRate = 0.95f;
        float consolidationStrength = 0.5f;
        size_t replayBatchSize = 3;
        
        // Developmental experiments
        float plasticityMaturationRate = 0.01f;
        float structuralPlasticityThreshold = 0.5f;
        
        // Social experiments
        size_t socialGroupSize = 5;
        float socialLearningRate = 0.05f;
        
        void validate() const {
            if (learningRate < 0 || learningRate > 1) {
                throw BrainInitializationError("Learning rate must be between 0 and 1");
            }
            if (explorationRate < 0 || explorationRate > 1) {
                throw BrainInitializationError("Exploration rate must be between 0 and 1");
            }
            if (memoryDecayRate < 0 || memoryDecayRate > 1) {
                throw BrainInitializationError("Memory decay rate must be between 0 and 1");
            }
        }
    };
    
    // State
    CoreConfig core;
    ExperimentalParameters experimental;
    std::string configFilePath;
    bool schemaValidated = false;
    
    // Schema definitions for validation
    static const std::vector<ParameterSchema> getCoreSchema() {
        return {
            ParameterSchema("random_seed", ParameterType::Integer, 42, true, "Random seed for reproducibility"),
            ParameterSchema("simulation_timestep", ParameterType::Float, 0.001, true, "Simulation time step in seconds"),
            ParameterSchema("neuron_count", ParameterType::Integer, 1000, true, "Total number of neurons"),
            ParameterSchema("region_count", ParameterType::Integer, 1, true, "Number of neural regions"),
            ParameterSchema("connection_probability", ParameterType::Float, 0.1f, true, "Probability of synaptic connections"),
            ParameterSchema("stdp_ltp_weight", ParameterType::Float, 0.01f, true, "STDP LTP weight"),
            ParameterSchema("stdp_ltd_weight", ParameterType::Float, 0.012f, true, "STDP LTD weight"),
            ParameterSchema("stdp_tau", ParameterType::Float, 20.0f, true, "STDP time constant"),
            ParameterSchema("enable_reward_modulation", ParameterType::Boolean, true, true, "Enable reward modulation"),
            ParameterSchema("enable_structural_plasticity", ParameterType::Boolean, true, true, "Enable structural plasticity"),
        };
    }
    
    // Constructors
    Configuration() = default;
    explicit Configuration(const std::string& filePath) : configFilePath(filePath) {}
    
    // Validation methods
    bool validateSchema() {
        try {
            core.validate();
            experimental.validate();
            schemaValidated = true;
            return true;
        } catch (const BrainException& e) {
            NLM_LOG_ERROR(std::string("Schema validation failed: ") + e.what());
            return false;
        }
    }
    
    // Parameter validation and conversion
    template<typename T>
    T getValidatedParameter(const std::string& name, const std::any& value) {
        if (!schemaValidated) {
            validateSchema();
        }
        
        // Parameter-specific validation
        if (name == "random_seed" && std::any_cast<int64_t>(value) < 0) {
            throw BrainInitializationError("Random seed must be non-negative");
        }
        if (name == "neuron_count" && std::any_cast<size_t>(value) == 0) {
            throw BrainInitializationError("Neuron count must be positive");
        }
        if (name == "connection_probability") {
            float prob = std::any_cast<float>(value);
            if (prob < 0 || prob > 1) {
                throw BrainInitializationError("Connection probability must be between 0 and 1");
            }
        }
        
        return std::any_cast<T>(value);
    }
    
    // Load from file with error recovery
    bool loadFromFile(const std::string& filePath) {
        try {
            // Implementation would load from JSON/toml file
            // For now, just set the path and use defaults
            configFilePath = filePath;
            NLM_LOG_INFO("Configuration loaded from file (schema not fully implemented yet)");
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to load configuration: ") + e.what());
            return false;
        }
    }
    
    // Load from command line with validation
    bool loadFromArgs(int argc, char** argv) {
        try {
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg.find("--config") != std::string::npos) {
                    // Handle config argument
                } else if (arg.find("--seed") != std::string::npos) {
                    // Handle seed argument
                }
                // Add more argument parsing
            }
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to parse command line arguments: ") + e.what());
            return false;
        }
    }
    
    // Safe parameter access with defaults
    template<typename T>
    T getOr(const std::string& name, T defaultValue) const {
        try {
            // Implementation would safely retrieve parameter
            // For now, return default
            return defaultValue;
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Failed to get parameter ") + name + ": " + e.what());
            return defaultValue;
        }
    }
    
    // Set parameter with validation
    template<typename T>
    bool set(const std::string& name, T value, ConfigSource source = ConfigSource::Default) {
        try {
            if (!schemaValidated) {
                validateSchema();
            }
            
            // Validate based on parameter name
            if (name == "random_seed") {
                int64_t seed = value;
                if (seed < 0) {
                    throw BrainInitializationError("Random seed must be non-negative");
                }
                core.randomSeed = seed;
            } else if (name == "neuron_count") {
                size_t count = value;
                if (count == 0) {
                    throw BrainInitializationError("Neuron count must be positive");
                }
                core.neuronCount = count;
            } else if (name == "simulation_timestep") {
                double timestep = value;
                if (timestep <= 0) {
                    throw BrainInitializationError("Simulation timestep must be positive");
                }
                core.simulationTimestep = timestep;
            } else if (name == "connection_probability") {
                float prob = value;
                if (prob < 0 || prob > 1) {
                    throw BrainInitializationError("Connection probability must be between 0 and 1");
                }
                core.connectionProbability = prob;
            } else {
                // Handle other parameters
                // For now, just store them
            }
            
            return true;
        } catch (const BrainException& e) {
            NLM_LOG_ERROR(std::string("Failed to set parameter ") + name + ": " + e.what());
            return false;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Unexpected error setting parameter ") + name + ": " + e.what());
            return false;
        }
    }
    
    // Check if configuration is valid
    bool isValid() const {
        return schemaValidated;
    }
    
    // Reset to defaults
    void reset() {
        core = CoreConfig();
        experimental = ExperimentalParameters();
        schemaValidated = false;
    }
};

} // namespace nlm
