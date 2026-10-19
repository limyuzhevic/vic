// Advanced configuration system with JSON/YAML support
// Enhanced configuration management with validation and type safety

#ifndef ADVANCED_CONFIG_HPP
#define ADVANCED_CONFIG_HPP

#include "Config.hpp"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <variant>
#include <nlohmann/json.hpp>

namespace nlm {

// Advanced configuration manager with JSON/YAML support
// Provides schema validation, type safety, and better error handling
class AdvancedConfig {
public:
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Load from JSON file
    bool loadFromJSON(const std::string& filepath);
    
    // Load from YAML file
    bool loadFromYAML(const std::string& filepath);
    
    // Load from existing Config object
    bool loadFromConfig(const Config& config);
    
    // Save to JSON file with pretty formatting
    bool saveToJSON(const std::string& filepath, bool pretty = true) const;
    
    // Save to YAML file
    bool saveToYAML(const std::string& filepath) const;
    
    // Get brain configuration parameters with defaults
    struct BrainConfig {
        // Network topology
        size_t neuronCount = 1000;
        size_t regionCount = 1;
        float connectionProbability = 0.1f;
        
        // Neuron parameters
        float excitationWeight = 1.0f;
        float inhibitionWeight = -1.0f;
        float membraneRestPotential = -70.0f;
        float membraneThreshold = -55.0f;
        float membraneTimeConstant = 20.0f;
        float refractoryPeriod = 2.0f;
        
        // Plasticity parameters
        float stdpLTPWeight = 0.01f;
        float stdpLTDWeight = 0.012f;
        float stdpTimeConstant = 20.0f;
        float hebbianLearningRate = 0.01f;
        float structuralPlasticityRate = 0.0001f;
        
        // Neuromodulation
        float dopamineDecayRate = 0.95f;
        float curiosityThreshold = 0.3f;
        
        // Memory parameters
        size_t workingMemoryCapacity = 100;
        size_t episodicMemoryMax = 1000;
        float memoryDecayRate = 0.01f;
        
        // Development parameters
        float plasticityModifier = 1.0f;
        float synaptogenesisRate = 0.0001f;
        float pruningRate = 0.00001f;
        
        // Simulation parameters
        double timestep = 0.001;
        size_t replayInterval = 100;
        size_t consolidationInterval = 1000;
        
        // Action space
        std::vector<std::string> actionTypes = {
            "MoveForward", "MoveBackward", "TurnLeft", "TurnRight",
            "Look", "LookUp", "LookDown", "Interact", "Eat", "Drink", "Rest", "Wait"
        };
    };
    
    // Get brain configuration with validation
    BrainConfig getBrainConfig() const;
    
    // Validate configuration
    bool validateConfig() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Set configuration from advanced JSON structure
    void setFromAdvancedJSON(const nlohmann::json& json);
    
    // Export configuration to advanced JSON structure
    nlohmann::json exportToAdvancedJSON() const;
    
    // Get configuration summary
    std::string getSummary() const;
    
    // Merge configuration with defaults
    void mergeWithDefaults(const BrainConfig& defaults);
    
    // Reset to default configuration
    void resetToDefaults();
    
    // Get configuration parameter description
    std::string getParameterDescription(const std::string& key) const;
    
    // Set custom parameter with validation
    template<typename T>
    bool setParameter(const std::string& key, const T& value);
    
    // Get configuration parameter
    template<typename T>
    std::optional<T> getParameter(const std::string& key) const;
    
    // Check if parameter exists
    bool hasParameter(const std::string& key) const;
    
    // Get all parameter keys
    std::vector<std::string> getParameterKeys() const;
    
    // Import configuration from advanced file formats
    bool importFromFile(const std::string& filepath);
    
    // Export configuration to advanced file formats
    bool exportToFile(const std::string& filepath) const;
    
    // Get configuration change count
    size_t getChangeCount() const { return changeCount_; }
    
    // Set configuration change callback
    void setChangeCallback(std::function<void(const std::string&)> callback);
    
    // Clear configuration
    void clear();
    
    // Copy configuration
    AdvancedConfig copy() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    size_t changeCount_;
    
    // Parameter validation functions
    bool validateParameter(const std::string& key, const nlohmann::json& value) const;
    std::string getParameterType(const std::string& key) const;
    nlohmann::json getParameterDefault(const std::string& key) const;
    bool isParameterRequired(const std::string& key) const;
    
    // Schema definitions
    nlohmann::json createDefaultSchema() const;
    nlohmann::json createBrainConfigSchema() const;
    
    // File format detection
    bool detectFileFormat(const std::string& filepath) const;
    
    // Error handling
    void addError(const std::string& error);
    std::vector<std::string> getErrors() const;
};

} // namespace nlm

#endif // ADVANCED_CONFIG_HPP
