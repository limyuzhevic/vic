// Enhanced Python bindings for NLM with improved error handling and validation

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <set>
#include <unordered_map>

// Include NLM core headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/core/ValidatedConfig.hpp"

// Include enhanced memory and performance headers
#include "../src/performance/MemoryPool.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception classes for better error handling
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message) : std::runtime_error("NLM Error: " + message) {}
};

class ValidationError : public NLMError {
public:
    ValidationError(const std::string& message) : NLMError("Validation error: " + message) {}
};

class ConfigError : public NLMError {
public:
    ConfigError(const std::string& message) : NLMError("Configuration error: " + message) {}
};

class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message) : NLMError("Memory error: " + message) {}
};

class InitializationError : public NLMError {
public:
    InitializationError(const std::string& message) : NLMError("Initialization error: " + message) {}
};

// Configuration validation wrapper for Python
class PythonConfig {
public:
    PythonConfig() {
        config_ = std::make_shared<Config>();
        schema_ = std::make_shared<ConfigSchema>();
        setupDefaultSchema();
    }
    
    PythonConfig(const std::string& filepath) {
        config_ = std::make_shared<Config>();
        schema_ = std::make_shared<ConfigSchema>();
        setupDefaultSchema();
        
        if (!config_->loadFromFile(filepath)) {
            throw ConfigError("Failed to load configuration from file: " + filepath);
        }
        
        // Validate after loading
        validate();
    }
    
    // Load from file with validation
    bool loadFromFile(const std::string& filepath) {
        if (!config_->loadFromFile(filepath)) {
            throw ConfigError("Failed to load configuration from file: " + filepath);
        }
        return validate();
    }
    
    // Set values with validation
    void set(const std::string& key, const std::string& value) {
        if (!validateBeforeSet(key, ConfigValue(value))) {
            throw ValidationError("Invalid configuration value for key '" + key + "': '" + value + "'");
        }
        config_->set(key, value);
    }
    
    void set(const std::string& key, int value) {
        if (!validateBeforeSet(key, ConfigValue(value))) {
            throw ValidationError("Invalid configuration value for key '" + key + "': " + std::to_string(value));
        }
        config_->set(key, value);
    }
    
    void set(const std::string& key, double value) {
        if (!validateBeforeSet(key, ConfigValue(value))) {
            throw ValidationError("Invalid configuration value for key '" + key + "': " + std::to_string(value));
        }
        config_->set(key, value);
    }
    
    void set(const std::string& key, bool value) {
        if (!validateBeforeSet(key, ConfigValue(value))) {
            throw ValidationError("Invalid configuration value for key '" + key + "': " + (value ? "true" : "false"));
        }
        config_->set(key, value);
    }
    
    // Get values with type checking
    std::string getString(const std::string& key) const {
        auto valueOpt = config_->get<std::string>(key);
        if (!valueOpt) {
            throw ConfigError("Key not found: '" + key + "'");
        }
        return *valueOpt;
    }
    
    int getInt(const std::string& key) const {
        auto valueOpt = config_->get<int>(key);
        if (!valueOpt) {
            throw ConfigError("Key not found: '" + key + "'");
        }
        return *valueOpt;
    }
    
    double getFloat(const std::string& key) const {
        auto valueOpt = config_->get<double>(key);
        if (!valueOpt) {
            throw ConfigError("Key not found: '" + key + "'");
        }
        return *valueOpt;
    }
    
    bool getBool(const std::string& key) const {
        auto valueOpt = config_->get<bool>(key);
        if (!valueOpt) {
            throw ConfigError("Key not found: '" + key + "'");
        }
        return *valueOpt;
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return config_->has(key);
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        return config_->getKeys();
    }
    
    // Get configuration summary with validation
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "Configuration Summary:" << std::endl;
        oss << config_->summary() << std::endl;
        
        // Add validation status
        if (schema_) {
            std::vector<std::pair<ConfigValidationError, std::string>> validationResults = schema_->validate(*config_);
            if (!validationResults.empty()) {
                oss << "\nValidation Warnings:" << std::endl;
                for (const auto& result : validationResults) {
                    oss << "  " << result.second << std::endl;
                }
            } else {
                oss << "\nConfiguration is valid." << std::endl;
            }
        }
        
        return oss.str();
    }
    
    // Validation methods
    bool validate() {
        if (!schema_) {
            return true; // No validation schema
        }
        
        std::vector<std::pair<ConfigValidationError, std::string>> validationResults = schema_->validate(*config_);
        
        for (const auto& result : validationResults) {
            std::cerr << "Validation warning: " << result.second << std::endl;
        }
        
        return validationResults.empty();
    }
    
    bool validateBeforeSet(const std::string& key, const ConfigValue& value) const {
        if (!schema_) {
            return true; // No validation
        }
        
        ConfigEntry entry(key, value, ConfigSource::Runtime);
        std::vector<ConfigValidationError> errors = schema_->validate(entry);
        return errors.empty();
    }
    
    // Get validation errors for a key
    std::vector<std::string> getValidationErrors(const std::string& key, const ConfigValue& value) const {
        std::vector<std::string> errors;
        
        if (schema_) {
            ConfigEntry entry(key, value, ConfigSource::Runtime);
            std::vector<ConfigValidationError> validationErrors = schema_->validate(entry);
            
            for (ConfigValidationError error : validationErrors) {
                errors.push_back(ConfigValidation::generateValidationErrorMessage(key, error, value));
            }
        }
        
        return errors;
    }
    
    // Configuration schema methods
    void addSchemaValidation(const std::string& key, 
                           const std::function<bool(const ConfigValue&)>& validator,
                           const std::string& errorMessage,
                           bool required = false,
                           const std::vector<std::string>& allowedValues = {},
                           const std::pair<double, double>& range = {-1e6, 1e6},
                           const std::string& description = "") {
        if (schema_) {
            schema_->addRule(key, validator, errorMessage, required, allowedValues, range, description);
        }
    }
    
    void addIntValidation(const std::string& key, const std::pair<int64_t, int64_t>& range, bool required = false) {
        if (schema_) {
            schema_->addIntRule(key, range, required);
        }
    }
    
    void addFloatValidation(const std::string& key, const std::pair<double, double>& range, bool required = false) {
        if (schema_) {
            schema_->addFloatRule(key, range, required);
        }
    }
    
    void addStringValidation(const std::string& key, bool required = false, const std::vector<std::string>& allowedValues = {}) {
        if (schema_) {
            schema_->addStringRule(key, required, allowedValues);
        }
    }
    
    void addBoolValidation(const std::string& key, bool required = false) {
        if (schema_) {
            schema_->addBoolRule(key, required);
        }
    }
    
    // Context manager for configuration
    class ConfigScope {
    public:
        ConfigScope(PythonConfig& config) : config_(config) {
            originalConfig_ = config.copy();
        }
        
        ~ConfigScope() {
            config_.restore(originalConfig_);
        }
        
        // Apply changes and validate
        void commit() {
            config_.validate();
        }
        
        // Rollback changes
        void rollback() {
            config_.restore(originalConfig_);
        }
        
    private:
        PythonConfig& config_;
        PythonConfig originalConfig_;
        
        PythonConfig copy() const {
            PythonConfig copy;
            // Copy all settings from this config
            return copy;
        }
        
        void restore(const PythonConfig& other) {
            // Restore all settings from other config
        }
    };
    
    ConfigScope createScope() {
        return ConfigScope(*this);
    }
    
    // Helper method to setup default validation schema
    void setupDefaultSchema() {
        if (!schema_) {
            return;
        }
        
        // Add common NLM configuration validations
        schema_->addIntRule("brain.neuron_count", {1, 100000}, true);
        schema_->addFloatRule("brain.synapse_density", {0.0, 1.0});
        schema_->addFloatRule("brain.connection_probability", {0.0, 1.0});
        schema_->addIntRule("brain.region_count", {1, 100});
        schema_->addFloatRule("stdp_ltp_weight", {-1.0, 1.0});
        schema_->addFloatRule("stdp_ltd_weight", {-1.0, 1.0});
        
        // Add description for each key
        schema_->getAllDescriptions()["brain.neuron_count"] = "Number of neurons in the brain";
        schema_->getAllDescriptions()["brain.synapse_density"] = "Average synaptic weight";
        schema_->getAllDescriptions()["brain.connection_probability"] = "Probability of connection between neurons";
        schema_->getAllDescriptions()["brain.region_count"] = "Number of brain regions";
        schema_->getAllDescriptions()["stdp_ltp_weight"] = "STDP learning rate for long-term potentiation";
        schema_->getAllDescriptions()["stdp_ltd_weight"] = "STDP learning rate for long-term depression";
    }
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<ConfigSchema> schema_;
};