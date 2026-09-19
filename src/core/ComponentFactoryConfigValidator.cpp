#include "ComponentFactoryConfigValidator.hpp"
#include <sstream>
#include <algorithm>

namespace nlm {
    
    // Implementation of ComponentFactoryConfigValidator methods
    
    // Set configuration
    void ComponentFactoryConfigValidator::setConfig(std::shared_ptr<Config> config) {
        config_ = config;
    }
    
    // Add validator
    void ComponentFactoryConfigValidator::addValidator(std::unique_ptr<ComponentFactoryValidator> validator) {
        validators_.push_back(std::move(validator));
    }
    
    // Add plugin validator
    void ComponentFactoryConfigValidator::addPluginValidator(const std::string& pluginName, PluginValidationFunction validator) {
        pluginValidators_[pluginName].push_back(validator);
    }
    
    // Validate configuration using all registered validators
    ValidationResult ComponentFactoryConfigValidator::validate(const IComponentFactory& factory) const {
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
                oss << "Plugin \"" << pluginName << "\" configuration validation failed:" << std::endl;
                oss << pluginResult.errorMessage;
                overallResult.errorMessage += (overallResult.errorMessage.empty() ? "" : "\n") + oss.str();
            }
        }
        
        return overallResult;
    }
    
    // Validate factory-specific configuration
    ValidationResult ComponentFactoryConfigValidator::validateFactoryConfiguration(const IComponentFactory& factory) const {
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
    ValidationResult ComponentFactoryConfigValidator::validatePluginConfiguration(const std::string& pluginName) const {
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
    std::vector<std::string> ComponentFactoryConfigValidator::getValidatorNames() const {
        std::vector<std::string> names;
        for (const auto& validator : validators_) {
            names.push_back(validator->getValidatorName());
        }
        return names;
    }
    
    // Get all registered plugin names
    std::vector<std::string> ComponentFactoryConfigValidator::getPluginNames() const {
        std::vector<std::string> names;
        names.reserve(pluginValidators_.size());
        for (const auto& pair : pluginValidators_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Configuration summary
    std::string ComponentFactoryConfigValidator::getValidationSummary() const {
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
    
} // namespace nlm