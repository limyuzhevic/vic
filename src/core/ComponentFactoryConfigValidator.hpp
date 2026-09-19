#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include "../Config/Config.hpp"
#include "ConfigValidation.hpp"
#include "ComponentFactory/ComponentFactory.hpp"

namespace nlm {

// Factory-specific validation function type
using FactoryValidationFunction = std::function<ValidationResult(const std::shared_ptr<Config>& config)>;

// Plugin configuration validation function type
using PluginValidationFunction = std::function<ValidationResult(const std::string& pluginName, const ConfigValue& configValue, const ConfigEntry* entry)>;

// ComponentFactoryValidator interface
class ComponentFactoryValidator {
public:
    virtual ~ComponentFactoryValidator() = default;
    
    // Return the component type this validator handles
    virtual std::string getComponentType() const = 0;
    
    // Validate configuration for this component type
    virtual ValidationResult validateConfiguration(const std::shared_ptr<Config>& config) const = 0;
    
    // Get validator name
    virtual std::string getValidatorName() const = 0;
    
    // Get validator version
    virtual std::string getValidatorVersion() const = 0;
};

// Default ComponentFactoryValidator implementation
class DefaultComponentFactoryValidator : public ComponentFactoryValidator {
public:
    std::string getComponentType() const override {
        return "DefaultValidator";
    }
    
    ValidationResult validateConfiguration(const std::shared_ptr<Config>& config) const override {
        ValidationResult result = ValidationResult::success();
        
        if (!config) {
            return ValidationResult::failure("Configuration is null");
        }
        
        // Validate common configuration keys
        std::vector<std::string> expectedKeys = {
            "neuron_count",
            "region_count",
            "connection_probability",
            "stdp_ltp_weight",
            "stdp_ltd_weight",
            "stdp_tau",
            "synaptogenesis_rate",
            "pruning_rate",
            "working_memory_capacity",
            "episodic_memory_max_episodes",
            "associative_memory_capacity",
            "dopamine_baseline",
            "curiosity_rate",
            "novelty_threshold",
            "plasticity_modifier",
            "replay_interval",
            "consolidation_interval",
            "checkpoint_dir",
            "checkpoint_interval"
        };
        
        // Check for missing keys
        for (const auto& key : expectedKeys) {
            if (!config->has(key)) {
                result.isValid = false;
                std::ostringstream oss;
                oss << "Missing required configuration key: \"" << key << "\"";
                result.errorMessage += (result.errorMessage.empty() ? "" : "\n") + oss.str();
            }
        }
        
        // Validate numeric ranges
        struct NumericRange {
            std::string key;
            double min;
            double max;
            std::string description;
        };
        
        std::vector<NumericRange> numericRanges = {
            {"neuron_count", 10.0, 100000.0, "Neuron count must be between 10 and 100,000"},
            {"region_count", 1.0, 100.0, "Region count must be between 1 and 100"},
            {"connection_probability", 0.0, 1.0, "Connection probability must be between 0.0 and 1.0"},
            {"stdp_ltp_weight", -1.0, 1.0, "STDP LTP weight must be between -1.0 and 1.0"},
            {"stdp_ltd_weight", -1.0, 1.0, "STDP LTD weight must be between -1.0 and 1.0"},
            {"stdp_tau", 0.0, 1000.0, "STDP tau must be between 0.0 and 1000.0"},
            {"synaptogenesis_rate", 0.0, 1.0, "Synaptogenesis rate must be between 0.0 and 1.0"},
            {"pruning_rate", 0.0, 1.0, "Pruning rate must be between 0.0 and 1.0"},
            {"working_memory_capacity", 1.0, 10000.0, "Working memory capacity must be between 1 and 10,000"},
            {"episodic_memory_max_episodes", 1.0, 1000000.0, "Episodic memory max episodes must be between 1 and 1,000,000"},
            {"associative_memory_capacity", 1.0, 100000.0, "Associative memory capacity must be between 1 and 100,000"},
            {"dopamine_baseline", 0.0, 10.0, "Dopamine baseline must be between 0.0 and 10.0"},
            {"curiosity_rate", 0.0, 1.0, "Curiosity rate must be between 0.0 and 1.0"},
            {"novelty_threshold", 0.0, 1.0, "Novelty threshold must be between 0.0 and 1.0"},
            {"plasticity_modifier", 0.0, 10.0, "Plasticity modifier must be between 0.0 and 10.0"},
            {"replay_interval", 0.0, 10000.0, "Replay interval must be between 0.0 and 10,000"},
            {"consolidation_interval", 0.0, 1000000.0, "Consolidation interval must be between 0.0 and 1,000,000"},
            {"checkpoint_interval", 0.0, 1000000.0, "Checkpoint interval must be between 0.0 and 1,000,000"}
        };
        
        for (const auto& range : numericRanges) {
            if (config->has(range.key)) {
                std::optional<double> value = config->get<double>(range.key);
                if (value.has_value()) {
                    double val = value.value();
                    if (val < range.min || val > range.max) {
                        result.isValid = false;
                        std::ostringstream oss;
                        oss << range.description << ". Found: " << val;
                        result.errorMessage += (result.errorMessage.empty() ? "" : "\n") + oss.str();
                    }
                }
            }
        }
        
        return result;
    }
    
    std::string getValidatorName() const override {
        return "DefaultComponentFactoryValidator";
    }
    
    std::string getValidatorVersion() const override {
        return "1.0.0";
    }
};

// ComponentFactory configuration integration
class ComponentFactoryConfigValidator {
private:
    std::shared_ptr<Config> config_;
    std::vector<std::unique_ptr<ComponentFactoryValidator>> validators_;
    std::map<std::string, std::vector<PluginValidationFunction>> pluginValidators_;
    
public:
    ComponentFactoryConfigValidator(std::shared_ptr<Config> config = nullptr) : config_(config) {}
    
    ~ComponentFactoryConfigValidator() = default;
    
    // Set configuration
    void setConfig(std::shared_ptr<Config> config) {
        config_ = config;
    }
    
    // Add validator
    void addValidator(std::unique_ptr<ComponentFactoryValidator> validator) {
        validators_.push_back(std::move(validator));
    }
    
    // Add plugin validator
    void addPluginValidator(const std::string& pluginName, PluginValidationFunction validator) {
        pluginValidators_[pluginName].push_back(validator);
    }
    
    // Validate configuration using all registered validators
    ValidationResult validate(const IComponentFactory& factory) const {
        ValidationResult overallResult = ValidationResult::success();
        
        // Validate using component factory validators
        for (const auto& validator : validators_) {
            ValidationResult result = validator->validateConfiguration(config_);
            if (!result.isValid) {
                overallResult.isValid = false;
                std::ostringstream oss;
                oss << "Validator \"" << validator->getValidatorName() << \"\" failed:" << std::endl;
                oss << result.errorMessage;
                overallResult.errorMessage += (overallResult.errorMessage.empty() ? "" : "\n") + oss.str();
            }
        }
        
        // Validate plugin configurations
        for (const auto& pair : pluginValidators_) {
            const auto& pluginName = pair.first;
            const auto& validators = pair.second;
            
            ValidationResult pluginResult = ValidationResult::success();
            
            for (const auto& validator : validators) {
                ValidationResult result = validator(pluginName, ConfigValue(std::string("plugin_config")),
                                                   nullptr);
                if (!result.isValid) {
                    pluginResult.isValid = false;
                    if (pluginResult.errorMessage.empty()) {
                        pluginResult.errorMessage = result.errorMessage;
                    } else {
                        pluginResult.errorMessage += "\n" + result.errorMessage;
                    }
                }
            }
            
            if (!pluginResult.isValid) {
                overallResult.isValid = false;
                std::ostringstream oss;
                oss << "Plugin \"" << pluginName <<\" configuration validation failed:" << std::endl;
                oss << pluginResult.errorMessage;
                overallResult.errorMessage += (overallResult.errorMessage.empty() ? "" : "\n") + oss.str();
            }
        }
        
        return overallResult;
    }
    
    // Validate factory-specific configuration
    ValidationResult validateFactoryConfiguration(const IComponentFactory& factory) const {
        ValidationResult overallResult = ValidationResult::success();
        
        // Get factory configuration
        std::string factoryName = factory.getComponentTypeName();
        
        // Run factory-specific validation
        for (const auto& validator : validators_) {
            // Check if validator is relevant for this factory
            if (validator->getComponentType() == factoryName || 
                validator->getComponentType() == "DefaultValidator") {
                ValidationResult result = validator->validateConfiguration(config_);
                if (!result.isValid) {
                    overallResult.isValid = false;
                    std::ostringstream oss;
                    oss << "Factory-specific validation failed:" << std::endl;
                    oss << result.errorMessage;
                    overallResult.errorMessage += (overallResult.errorMessage.empty() ? "" : "\n") + oss.str();
                }
            }
        }
        
        return overallResult;
    }
    
    // Validate plugin-specific configuration
    ValidationResult validatePluginConfiguration(const std::string& pluginName) const {
        ValidationResult overallResult = ValidationResult::success();
        
        auto it = pluginValidators_.find(pluginName);
        if (it != pluginValidators_.end()) {
            const auto& validators = it->second;
            
            for (const auto& validator : validators) {
                ValidationResult result = validator(pluginName, ConfigValue(std::string("plugin_config")),
                                                   nullptr);
                if (!result.isValid) {
                    overallResult.isValid = false;
                    if (overallResult.errorMessage.empty()) {
                        overallResult.errorMessage = result.errorMessage;
                    } else {
                        overallResult.errorMessage += "\n" + result.errorMessage;
                    }
                }
            }
        }
        
        return overallResult;
    }
    
    // Get all registered validator names
    std::vector<std::string> getValidatorNames() const {
        std::vector<std::string> names;
        for (const auto& validator : validators_) {
            names.push_back(validator->getValidatorName());
        }
        return names;
    }
    
    // Get all registered plugin names
    std::vector<std::string> getPluginNames() const {
        std::vector<std::string> names;
        names.reserve(pluginValidators_.size());
        for (const auto& pair : pluginValidators_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Configuration summary
    std::string getValidationSummary() const {
        std::ostringstream oss;
        oss << "ComponentFactoryConfigValidator Summary:" << std::endl;
        oss << "  Validators registered: " << validators_.size() << std::endl;
        oss << "  Plugin validators registered: " << pluginValidators_.size() << std::endl;
        oss << "  Validators:" << std::endl;
        
        for (const auto& validator : validators_) {
            oss << "    - " << validator->getValidatorName() << " (component type: " 
                << validator->getComponentType() << ", version: " 
                << validator->getValidatorVersion() << ")" << std::endl;
        }
        
        if (!pluginValidators_.empty()) {
            oss << "  Plugin validators:" << std::endl;
            for (const auto& pair : pluginValidators_) {
                oss << "    - " << pair.first << ": " << pair.second.size() << " validator(s)" << std::endl;
            }
        }
        
        return oss.str();
    }
};

} // namespace nlm