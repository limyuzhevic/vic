// Experiment configuration management system
// Provides centralized configuration for neural learning experiments
// with improved parameter validation and management

#pragma once

#include <memory>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

namespace nlm {

// Experiment type enumerator
enum class ExperimentType {
    BasicConnectivity,
    PlasticityLearning,
    STDPVerification,
    ComprehensiveLearning
};

// Experiment configuration structure
struct ExperimentConfig {
    std::string name;
    ExperimentType type;
    double duration;              // Duration in simulation steps
    double timestep;              // Simulation timestep
    size_t neuronCount;            // Number of neurons
    float connectionProbability;   // Probability of synaptic connections
    bool enablePlasticity;         // Enable plasticity mechanisms
    bool enableNeuromodulation;    // Enable neuromodulation
    bool enableDevelopment;        // Enable developmental processes
    float noveltyThreshold;        // Threshold for novelty detection
    float curiosityThreshold;      // Threshold for curiosity-based exploration
    float rewardScale;             // Scale factor for reward signals
    float predictionErrorWeight;   // Weight for prediction error
    std::map<std::string, float> additionalParameters;
    
    ExperimentConfig() 
        : name("DefaultExperiment")
        , type(ExperimentType::ComprehensiveLearning)
        , duration(1000.0)
        , timestep(0.001)
        , neuronCount(500)
        , connectionProbability(0.15f)
        , enablePlasticity(true)
        , enableNeuromodulation(true)
        , enableDevelopment(true)
        , noveltyThreshold(0.5f)
        , curiosityThreshold(0.5f)
        , rewardScale(1.0f)
        , predictionErrorWeight(1.0f) {}
    
    // Validate configuration parameters
    bool validate() const {
        if (duration <= 0.0) return false;
        if (timestep <= 0.0 || timestep > 1.0) return false;
        if (neuronCount == 0) return false;
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) return false;
        if (noveltyThreshold < 0.0f || noveltyThreshold > 1.0f) return false;
        if (curiosityThreshold < 0.0f || curiosityThreshold > 1.0f) return false;
        if (rewardScale < 0.0f) return false;
        if (predictionErrorWeight < 0.0f) return false;
        return true;
    }
    
    // Get parameter with validation and defaults
    float getParameter(const std::string& key, float defaultValue) const {
        auto it = additionalParameters.find(key);
        if (it != additionalParameters.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
    // Set parameter with validation
    void setParameter(const std::string& key, float value, float minValue = -1000.0f, float maxValue = 1000.0f) {
        if (value < minValue || value > maxValue) {
            throw std::invalid_argument("Parameter " + key + " value " + std::to_string(value) +
                                       " is out of valid range [" + std::to_string(minValue) + ", " + std::to_string(maxValue) + "]");
        }
        additionalParameters[key] = value;
    }
};

// Experiment parameter validator
class ExperimentParameterValidator {
public:
    // Validate experiment duration
    static bool validateDuration(double duration) {
        return duration > 0.0 && duration < 10000.0; // Reasonable bounds
    }
    
    // Validate neuron count
    static bool validateNeuronCount(size_t count) {
        return count > 0 && count <= 10000; // Reasonable bounds
    }
    
    // Validate connection probability
    static bool validateConnectionProbability(float prob) {
        return prob >= 0.0f && prob <= 1.0f;
    }
    
    // Validate threshold values
    static bool validateThreshold(float threshold) {
        return threshold >= 0.0f && threshold <= 1.0f;
    }
    
    // Validate timestep
    static bool validateTimestep(double timestep) {
        return timestep > 0.0 && timestep <= 0.1; // Reasonable bounds
    }
    
    // Apply validation to experiment config
    static std::string validateConfig(const ExperimentConfig& config) {
        std::vector<std::string> errors;
        
        if (!validateDuration(config.duration)) {
            errors.push_back("Experiment duration must be positive and less than 10000");
        }
        
        if (!validateNeuronCount(config.neuronCount)) {
            errors.push_back("Neuron count must be between 1 and 10000");
        }
        
        if (!validateConnectionProbability(config.connectionProbability)) {
            errors.push_back("Connection probability must be between 0.0 and 1.0");
        }
        
        if (!validateThreshold(config.noveltyThreshold)) {
            errors.push_back("Novelty threshold must be between 0.0 and 1.0");
        }
        
        if (!validateThreshold(config.curiosityThreshold)) {
            errors.push_back("Curiosity threshold must be between 0.0 and 1.0");
        }
        
        if (!validateTimestep(config.timestep)) {
            errors.push_back("Timestep must be positive and less than or equal to 0.1");
        }
        
        if (config.rewardScale < 0.0f) {
            errors.push_back("Reward scale must be non-negative");
        }
        
        if (config.predictionErrorWeight < 0.0f) {
            errors.push_back("Prediction error weight must be non-negative");
        }
        
        if (!errors.empty()) {
            return "Configuration validation failed:\n" + 
                   joinStrings(errors, "\n");
        }
        return "";
    }
    
private:
    static std::string joinStrings(const std::vector<std::string>& strings, const std::string& separator) {
        if (strings.empty()) return "";
        std::string result = strings[0];
        for (size_t i = 1; i < strings.size(); ++i) {
            result += separator + strings[i];
        }
        return result;
    }
};

// Experiment configuration manager
class ExperimentConfigManager {
private:
    std::map<ExperimentType, ExperimentConfig> experimentConfigs_;
    ExperimentConfig defaultConfig_;
    
public:
    ExperimentConfigManager() {
        initializeDefaultConfigs();
    }
    
    void initializeDefaultConfigs() {
        // Basic connectivity experiment
        ExperimentConfig basicConfig;
        basicConfig.name = "BasicConnectivityExperiment";
        basicConfig.type = ExperimentType::BasicConnectivity;
        basicConfig.duration = 50.0;
        basicConfig.enablePlasticity = false;
        basicConfig.enableNeuromodulation = false;
        basicConfig.enableDevelopment = false;
        basicConfig.neuronCount = 100;
        experimentConfigs_[ExperimentType::BasicConnectivity] = basicConfig;
        
        // Plasticity learning experiment
        ExperimentConfig plasticityConfig;
        plasticityConfig.name = "PlasticityLearningExperiment";
        plasticityConfig.type = ExperimentType::PlasticityLearning;
        plasticityConfig.duration = 1000.0;
        plasticityConfig.enablePlasticity = true;
        plasticityConfig.enableNeuromodulation = true;
        plasticityConfig.enableDevelopment = true;
        plasticityConfig.neuronCount = 500;
        plasticityConfig.noveltyThreshold = 0.3f;
        plasticityConfig.curiosityThreshold = 0.4f;
        plasticityConfig.rewardScale = 0.5f;
        experimentConfigs_[ExperimentType::PlasticityLearning] = plasticityConfig;
        
        // STDP verification experiment
        ExperimentConfig stdpConfig;
        stdpConfig.name = "STDPVerificationExperiment";
        stdpConfig.type = ExperimentType::STDPVerification;
        stdpConfig.duration = 200.0;
        stdpConfig.enablePlasticity = true;
        stdpConfig.enableNeuromodulation = true;
        stdpConfig.enableDevelopment = false;
        stdpConfig.neuronCount = 200;
        stdpConfig.predictionErrorWeight = 2.0f;
        experimentConfigs_[ExperimentType::STDPVerification] = stdpConfig;
        
        // Comprehensive learning experiment
        ExperimentConfig comprehensiveConfig;
        comprehensiveConfig.name = "ComprehensiveLearningExperiment";
        comprehensiveConfig.type = ExperimentType::ComprehensiveLearning;
        comprehensiveConfig.duration = 2000.0;
        comprehensiveConfig.enablePlasticity = true;
        comprehensiveConfig.enableNeuromodulation = true;
        comprehensiveConfig.enableDevelopment = true;
        comprehensiveConfig.neuronCount = 1000;
        comprehensiveConfig.noveltyThreshold = 0.2f;
        comprehensiveConfig.curiosityThreshold = 0.6f;
        comprehensiveConfig.rewardScale = 1.0f;
        comprehensiveConfig.predictionErrorWeight = 1.0f;
        experimentConfigs_[ExperimentType::ComprehensiveLearning] = comprehensiveConfig;
    }
    
    // Get configuration for experiment type
    ExperimentConfig getConfig(ExperimentType type) const {
        auto it = experimentConfigs_.find(type);
        if (it != experimentConfigs_.end()) {
            return it->second;
        }
        return defaultConfig_;
    }
    
    // Set configuration for experiment type
    void setConfig(ExperimentType type, const ExperimentConfig& config) {
        if (!config.validate()) {
            throw std::invalid_argument("Invalid experiment configuration");
        }
        experimentConfigs_[type] = config;
    }
    
    // Apply configuration to Brain object
    void applyConfig(std::shared_ptr<Brain> brain, const ExperimentConfig& config) {
        if (!brain) {
            throw std::invalid_argument("Cannot apply configuration to null brain");
        }
        
        // Set configuration parameters on the brain
        // This is a placeholder - actual implementation would depend on Brain API
        brain->setNeuronCount(config.neuronCount);
        brain->setConnectionProbability(config.connectionProbability);
        brain->enablePlasticity(config.enablePlasticity);
        brain->enableNeuromodulation(config.enableNeuromodulation);
        brain->enableDevelopment(config.enableDevelopment);
        
        // Set additional parameters
        for (const auto& param : config.additionalParameters) {
            brain->setParameter(param.first, param.second);
        }
    }
    
    // Create experiment-specific configuration
    ExperimentConfig createExperimentConfig(ExperimentType type, const std::string& customName = "") {
        ExperimentConfig config = getConfig(type);
        
        if (!customName.empty()) {
            config.name = customName;
        }
        
        return config;
    }
    
    // Update experiment configuration with custom parameters
    void updateConfig(ExperimentType type, const std::map<std::string, float>& customParams) {
        ExperimentConfig config = getConfig(type);
        
        for (const auto& param : customParams) {
            config.setParameter(param.first, param.second);
        }
        
        setConfig(type, config);
    }
    
    // Get all available experiment types
    std::vector<ExperimentType> getAvailableExperimentTypes() const {
        std::vector<ExperimentType> types;
        for (const auto& pair : experimentConfigs_) {
            types.push_back(pair.first);
        }
        return types;
    }
    
    // Get experiment name for type
    std::string getExperimentName(ExperimentType type) const {
        auto it = experimentConfigs_.find(type);
        if (it != experimentConfigs_.end()) {
            return it->second.name;
        }
        return "UnknownExperiment";
    }
};

} // namespace nlm
