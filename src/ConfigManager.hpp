// Configuration Management for Enhanced NLM Phase 2
// Provides robust configuration handling with multiple sources and validation

#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Configuration validation errors
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& field = "")
        : std::runtime_error(message), field(field) {}
    
    std::string getField() const { return field; }
    
private:
    std::string field;
};

// Configuration schema definition
struct ConfigSchema {
    std::string name;
    std::string description;
    std::string type; // "int", "float", "bool", "string"
    std::string defaultValue;
    std::string minValue;
    std::string maxValue;
    bool required = false;
    std::vector<std::string> allowedValues;
    
    bool validate(const std::string& value) const {
        if (required && value.empty()) return false;
        if (!minValue.empty() && std::stod(value) < std::stod(minValue)) return false;
        if (!maxValue.empty() && std::stod(value) > std::stod(maxValue)) return false;
        if (!allowedValues.empty()) {
            bool found = false;
            for (const auto& allowed : allowedValues) {
                if (value == allowed) {
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }
        return true;
    }
};

// Enhanced configuration structure
struct EnhancedConfig {
    uint64_t randomSeed = 42;
    double simulationTimestep = 0.001;
    int64_t neuronCount = 500;
    int64_t regionCount = 1;
    float connectionProbability = 0.15f;
    
    // STDP parameters
    float stdpLtpWeight = 0.02f;
    float stdpLtdWeight = 0.015f;
    float stdpTau = 20.0f;
    
    // Structural plasticity
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    
    // Test configuration
    int maxSteps = 1000;
    int connectivityTestSteps = 50;
    int plasticityTestSteps = 1000;
    int stdpTestTrials = 50;
    
    // Performance and output
    bool enableProfiling = false;
    bool verboseOutput = true;
    bool saveResults = false;
    std::string outputDir = ".";
    
    // Error handling
    int maxRetries = 3;
    double retryDelay = 0.1;
    
    // Test selection
    bool runConnectivityTest = true;
    bool runPlasticityTest = true;
    bool runStdpTest = true;
    bool runLearningExperiment = true;
};

// Advanced configuration manager
class ConfigManager {
private:
    std::shared_ptr<Config> coreConfig;
    EnhancedConfig enhancedConfig;
    
    // Configuration schema registry
    std::map<std::string, ConfigSchema> schemaRegistry;
    
    // Validation results
    struct ValidationResult {
        bool valid = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    } validationResult;
    
public:
    ConfigManager();
    
    // Load configuration from file
    bool loadFromFile(const std::string& filename);
    
    // Load configuration from JSON
    bool loadFromJson(const std::string& jsonString);
    bool loadFromJsonObject(const json& configJson);
    
    // Set configuration from command line arguments
    void setFromArgs(int argc, char** argv);
    
    // Apply configuration to brain
    void applyToBrain(std::shared_ptr<Brain> brain);
    
    // Validate configuration
    ValidationResult validate();
    
    // Get configuration values
    template<typename T>
    T get(const std::string& key) const;
    
    const EnhancedConfig& getEnhancedConfig() const { return enhancedConfig; }
    const ValidationResult& getValidationResult() const { return validationResult; }
    
    // Check if configuration is valid
    bool isValid() const { return validationResult.valid; }
    
    // Get error messages
    std::string getErrorMessages() const;
    
    // Get warning messages
    std::string getWarningMessages() const;
    
private:
    void initializeSchemaRegistry();
    void loadEnhancedConfigFromJson(const json& configJson);
    
    bool validateKey(const std::string& key, const std::string& value);
    void setCoreConfig(const std::string& key, const std::string& value);
    
    std::string getDefaultValue(const std::string& key) const;
};