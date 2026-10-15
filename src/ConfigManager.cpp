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
    ConfigManager() {
        coreConfig = std::make_shared<Config>();
        initializeSchemaRegistry();
    }
    
    void initializeSchemaRegistry() {
        // Core configuration schema
        schemaRegistry["random_seed"] = {
            "random_seed", "Random seed for reproducible simulations", "int", "42", "0", "2147483647"
        };
        
        schemaRegistry["simulation_timestep"] = {
            "simulation_timestep", "Simulation timestep in seconds", "float", "0.001", "0.0001", "1.0"
        };
        
        schemaRegistry["neuron_count"] = {
            "neuron_count", "Total number of neurons", "int", "500", "1", "10000"
        };
        
        schemaRegistry["region_count"] = {
            "region_count", "Number of brain regions", "int", "1", "1", "10"
        };
        
        schemaRegistry["connection_probability"] = {
            "connection_probability", "Probability of neuron connection", "float", "0.15", "0.0", "1.0"
        };
        
        // STDP parameters
        schemaRegistry["stdp_ltp_weight"] = {
            "stdp_ltp_weight", "STDP LTP weight factor", "float", "0.02", "0.0", "1.0"
        };
        
        schemaRegistry["stdp_ltd_weight"] = {
            "stdp_ltd_weight", "STDP LTD weight factor", "float", "0.015", "0.0", "1.0"
        };
        
        schemaRegistry["stdp_tau"] = {
            "stdp_tau", "STDP time constant (ms)", "float", "20.0", "1.0", "100.0"
        };
        
        // Structural plasticity
        schemaRegistry["synaptogenesis_rate"] = {
            "synaptogenesis_rate", "Rate of new synapse formation", "float", "0.0001", "0.0", "0.01"
        };
        
        schemaRegistry["pruning_rate"] = {
            "pruning_rate", "Rate of synapse pruning", "float", "0.00001", "0.0", "0.001"
        };
        
        // Enhanced NLM specific
        schemaRegistry["max_steps"] = {
            "max_steps", "Maximum simulation steps", "int", "1000", "1", "100000"
        };
        
        schemaRegistry["connectivity_test_steps"] = {
            "connectivity_test_steps", "Steps for connectivity test", "int", "50", "1", "1000"
        };
        
        schemaRegistry["plasticity_test_steps"] = {
            "plasticity_test_steps", "Steps for plasticity test", "int", "1000", "1", "10000"
        };
        
        schemaRegistry["stdp_test_trials"] = {
            "stdp_test_trials", "Trials for STDP test", "int", "50", "1", "1000"
        };
        
        schemaRegistry["enable_profiling"] = {
            "enable_profiling", "Enable performance profiling", "bool", "false"
        };
        
        schemaRegistry["verbose_output"] = {
            "verbose_output", "Enable verbose output", "bool", "true"
        };
        
        schemaRegistry["save_results"] = {
            "save_results", "Save results to file", "bool", "false"
        };
        
        schemaRegistry["output_dir"] = {
            "output_dir", "Output directory path", "string", "."
        };
        
        // Test selection
        schemaRegistry["run_connectivity_test"] = {
            "run_connectivity_test", "Run connectivity test", "bool", "true"
        };
        
        schemaRegistry["run_plasticity_test"] = {
            "run_plasticity_test", "Run plasticity test", "bool", "true"
        };
        
        schemaRegistry["run_stdp_test"] = {
            "run_stdp_test", "Run STDP test", "bool", "true"
        };
        
        schemaRegistry["run_learning_experiment"] = {
            "run_learning_experiment", "Run learning experiment", "bool", "true"
        };
    }
    
    // Load configuration from file
    bool loadFromFile(const std::string& filename) {
        try {
            std::ifstream file(filename);
            if (!file.is_open()) {
                validationResult.warnings.push_back("Configuration file not found: " + filename);
                return false;
            }
            
            json configJson;
            file >> configJson;
            
            // Load core config
            for (auto& [key, schema] : schemaRegistry) {
                if (configJson.contains(key)) {
                    std::string valueStr = configJson[key].dump();
                    if (validateKey(key, valueStr)) {
                        setCoreConfig(key, valueStr);
                    }
                }
            }
            
            // Load enhanced config
            loadEnhancedConfigFromJson(configJson);
            
            return true;
        } catch (const std::exception& e) {
            validationResult.errors.push_back("Failed to load config file: " + std::string(e.what()));
            return false;
        }
    }
    
    // Load configuration from JSON
    bool loadFromJson(const std::string& jsonString) {
        try {
            json configJson = json::parse(jsonString);
            return loadFromJsonObject(configJson);
        } catch (const std::exception& e) {
            validationResult.errors.push_back("Invalid JSON configuration: " + std::string(e.what()));
            return false;
        }
    }
    
    bool loadFromJsonObject(const json& configJson) {
        // Load core config
        for (auto& [key, schema] : schemaRegistry) {
            if (configJson.contains(key)) {
                std::string valueStr = configJson[key].dump();
                if (validateKey(key, valueStr)) {
                    setCoreConfig(key, valueStr);
                }
            }
        }
        
        // Load enhanced config
        loadEnhancedConfigFromJson(configJson);
        
        return true;
    }
    
    void loadEnhancedConfigFromJson(const json& configJson) {
        if (configJson.contains("max_steps")) {
            enhancedConfig.maxSteps = configJson["max_steps"];
        }
        if (configJson.contains("connectivity_test_steps")) {
            enhancedConfig.connectivityTestSteps = configJson["connectivity_test_steps"];
        }
        if (configJson.contains("plasticity_test_steps")) {
            enhancedConfig.plasticityTestSteps = configJson["plasticity_test_steps"];
        }
        if (configJson.contains("stdp_test_trials")) {
            enhancedConfig.stdpTestTrials = configJson["stdp_test_trials"];
        }
        if (configJson.contains("enable_profiling")) {
            enhancedConfig.enableProfiling = configJson["enable_profiling"];
        }
        if (configJson.contains("verbose_output")) {
            enhancedConfig.verboseOutput = configJson["verbose_output"];
        }
        if (configJson.contains("save_results")) {
            enhancedConfig.saveResults = configJson["save_results"];
        }
        if (configJson.contains("output_dir")) {
            enhancedConfig.outputDir = configJson["output_dir"];
        }
        if (configJson.contains("run_connectivity_test")) {
            enhancedConfig.runConnectivityTest = configJson["run_connectivity_test"];
        }
        if (configJson.contains("run_plasticity_test")) {
            enhancedConfig.runPlasticityTest = configJson["run_plasticity_test"];
        }
        if (configJson.contains("run_stdp_test")) {
            enhancedConfig.runStdpTest = configJson["run_stdp_test"];
        }
        if (configJson.contains("run_learning_experiment")) {
            enhancedConfig.runLearningExperiment = configJson["run_learning_experiment"];
        }
    }
    
    // Set configuration from command line arguments
    void setFromArgs(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--config" || arg == "-c") {
                if (i + 1 < argc) {
                    loadFromFile(argv[++i]);
                }
            } else if (arg == "--neuron-count" || arg == "-n") {
                if (i + 1 < argc) {
                    int64_t value = std::stoll(argv[++i]);
                    enhancedConfig.neuronCount = value;
                }
            } else if (arg == "--steps" || arg == "-s") {
                if (i + 1 < argc) {
                    enhancedConfig.maxSteps = std::stoi(argv[++i]);
                }
            } else if (arg == "--random-seed") {
                if (i + 1 < argc) {
                    enhancedConfig.randomSeed = std::stoll(argv[++i]);
                }
            } else if (arg == "--timestep") {
                if (i + 1 < argc) {
                    enhancedConfig.simulationTimestep = std::stod(argv[++i]);
                }
            } else if (arg == "--connection-probability") {
                if (i + 1 < argc) {
                    enhancedConfig.connectionProbability = std::stof(argv[++i]);
                }
            } else if (arg == "--no-connectivity") {
                enhancedConfig.runConnectivityTest = false;
            } else if (arg == "--no-plasticity") {
                enhancedConfig.runPlasticityTest = false;
            } else if (arg == "--no-stdp") {
                enhancedConfig.runStdpTest = false;
            } else if (arg == "--verbose" || arg == "-v") {
                enhancedConfig.verboseOutput = true;
            } else if (arg == "--quiet" || arg == "-q") {
                enhancedConfig.verboseOutput = false;
            } else if (arg == "--profile") {
                enhancedConfig.enableProfiling = true;
            }
        }
    }
    
    // Apply configuration to brain
    void applyToBrain(std::shared_ptr<Brain> brain) {
        if (!brain) return;
        
        // Set core configuration
        coreConfig->set("random_seed", static_cast<int64_t>(enhancedConfig.randomSeed), ConfigSource::Default);
        coreConfig->set("simulation_timestep", enhancedConfig.simulationTimestep, ConfigSource::Default);
        coreConfig->set("neuron_count", static_cast<int64_t>(enhancedConfig.neuronCount), ConfigSource::Default);
        coreConfig->set("region_count", static_cast<int64_t>(enhancedConfig.regionCount), ConfigSource::Default);
        coreConfig->set("connection_probability", enhancedConfig.connectionProbability, ConfigSource::Default);
        
        coreConfig->set("stdp_ltp_weight", enhancedConfig.stdpLtpWeight, ConfigSource::Default);
        coreConfig->set("stdp_ltd_weight", enhancedConfig.stdpLtdWeight, ConfigSource::Default);
        coreConfig->set("stdp_tau", enhancedConfig.stdpTau, ConfigSource::Default);
        
        coreConfig->set("synaptogenesis_rate", enhancedConfig.synaptogenesisRate, ConfigSource::Default);
        coreConfig->set("pruning_rate", enhancedConfig.pruningRate, ConfigSource::Default);
        
        // Create a new brain with the configuration
        // Note: In practice, you'd need to reinitialize the brain
        // This is a simplified example
    }
    
    // Validate configuration
    ValidationResult validate() {
        validationResult = ValidationResult();
        
        // Check required fields
        for (auto& [key, schema] : schemaRegistry) {
            if (schema.required) {
                // Check if field is set (this is a simplified check)
                if (!coreConfig->has(key)) {
                    validationResult.errors.push_back("Required configuration field missing: " + key);
                }
            }
        }
        
        // Validate enhanced config
        if (enhancedConfig.neuronCount <= 0) {
            validationResult.errors.push_back("Neuron count must be positive");
        }
        
        if (enhancedConfig.maxSteps <= 0) {
            validationResult.errors.push_back("Max steps must be positive");
        }
        
        if (enhancedConfig.simulationTimestep <= 0.0) {
            validationResult.errors.push_back("Simulation timestep must be positive");
        }
        
        if (enhancedConfig.connectionProbability < 0.0 || enhancedConfig.connectionProbability > 1.0) {
            validationResult.errors.push_back("Connection probability must be between 0 and 1");
        }
        
        // Warnings
        if (enhancedConfig.neuronCount > 10000) {
            validationResult.warnings.push_back("Large neuron count may impact performance: " + 
                                              std::to_string(enhancedConfig.neuronCount));
        }
        
        if (enhancedConfig.maxSteps > 100000) {
            validationResult.warnings.push_back("Large step count may impact performance: " + 
                                              std::to_string(enhancedConfig.maxSteps));
        }
        
        validationResult.valid = validationResult.errors.empty();
        return validationResult;
    }
    
    // Get configuration values
    template<typename T>
    T get(const std::string& key) const {
        return coreConfig->getOr<T>(key, enhancedConfig.getDefaultValue(key));
    }
    
    const EnhancedConfig& getEnhancedConfig() const { return enhancedConfig; }
    const ValidationResult& getValidationResult() const { return validationResult; }
    
    // Check if configuration is valid
    bool isValid() const { return validationResult.valid; }
    
    // Get error messages
    std::string getErrorMessages() const {
        std::string result;
        for (const auto& error : validationResult.errors) {
            result += "ERROR: " + error + "\n";
        }
        return result;
    }
    
    // Get warning messages
    std::string getWarningMessages() const {
        std::string result;
        for (const auto& warning : validationResult.warnings) {
            result += "WARNING: " + warning + "\n";
        }
        return result;
    }
    
private:
    bool validateKey(const std::string& key, const std::string& value) {
        auto it = schemaRegistry.find(key);
        if (it == schemaRegistry.end()) {
            validationResult.warnings.push_back("Unknown configuration key: " + key);
            return true;  // Allow unknown keys for backward compatibility
        }
        
        const auto& schema = it->second;
        if (!schema.validate(value)) {
            validationResult.errors.push_back("Invalid value for key " + key + ": " + value);
            return false;
        }
        
        return true;
    }
    
    void setCoreConfig(const std::string& key, const std::string& value) {
        auto it = schemaRegistry.find(key);
        if (it == schemaRegistry.end()) return;
        
        const auto& schema = it->second;
        
        if (schema.type == "int") {
            coreConfig->set(key, std::stoll(value), ConfigSource::Default);
        } else if (schema.type == "float") {
            coreConfig->set(key, std::stod(value), ConfigSource::Default);
        } else if (schema.type == "bool") {
            coreConfig->set(key, value == "true" || value == "1", ConfigSource::Default);
        } else if (schema.type == "string") {
            coreConfig->set(key, value, ConfigSource::Default);
        }
    }
    
    std::string enhancedConfig.getDefaultValue(const std::string& key) const {
        auto it = schemaRegistry.find(key);
        if (it == schemaRegistry.end()) return "";
        return it->second.defaultValue;
    }
};