#include "ConfigValidator.hpp"
#include "ConfigError.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {
    namespace ConfigValidatorImpl {
        
        // Helper functions for configuration validation
        inline std::string getCurrentTimestamp() {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::tm tm_buf;
            gmtime_r(&time, &tm_buf);
            
            std::ostringstream oss;
            oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
            return oss.str();
        }
        
    } // namespace ConfigValidatorImpl
    
    // Implementation of ConfigValidator methods
    
    // Configuration validation
    
    // Validate entire configuration against schema
    ValidationResult ConfigValidator::validate(const std::shared_ptr<Config>& config) const {
        totalValidations_++;
        
        std::shared_ptr<Config> configToValidate = config ? config : config_;
        if (!configToValidate) {
            return ValidationResult::failure("No configuration provided");
        }
        
        ValidationResult overallResult = ValidationResult::success();
        
        // Validate against schema if present
        if (schema_ && schema_->isStrictValidation()) {
            ValidationResult schemaResult = schema_->validate(configToValidate);
            if (!schemaResult.isValid) {
                overallResult.isValid = false;
                overallResult.errorMessage = schemaResult.errorMessage;
                overallResult.details = schemaResult.details;
            }
        }
        
        // Validate against parameter resolver
        if (parameterResolver_) {
            ValidationResult paramResult = validateParameters(configToValidate);
            if (!paramResult.isValid) {
                overallResult.isValid = false;
                if (overallResult.errorMessage.empty()) {
                    overallResult.errorMessage = paramResult.errorMessage;
                } else {
                    overallResult.errorMessage += "\n" + paramResult.errorMessage;
                }
                overallResult.details.insert(paramResult.details.begin(), paramResult.details.end());
            }
        }
        
        // Run custom validation callbacks
        if (runtimeValidationEnabled_) {
            ValidationResult callbackResult = runValidationCallbacks(configToValidate);
            if (!callbackResult.isValid) {
                overallResult.isValid = false;
                if (overallResult.errorMessage.empty()) {
                    overallResult.errorMessage = callbackResult.errorMessage;
                } else {
                    overallResult.errorMessage += "\n" + callbackResult.errorMessage;
                }
                overallResult.details.insert(callbackResult.details.begin(), callbackResult.details.end());
            }
        }
        
        if (overallResult.isValid) {
            successfulValidations_++;
        } else {
            failedValidations_++;
        }
        
        return overallResult;
    }
    
    // Validate a specific configuration key
    ValidationResult ConfigValidator::validateKey(const std::string& key, const ConfigValue& value) const {
        ValidationResult result;
        
        // Validate against schema
        if (schema_) {
            ConfigEntry entry{key, value, ConfigSource::Runtime};
            std::map<std::string, ConfigEntry> allEntries = {{key, entry}};
            result = schema_->validateEntry(entry, allEntries);
            if (!result.isValid) {
                return result;
            }
        }
        
        // Validate against parameter resolver
        if (parameterResolver_) {
            result = parameterResolver_->validateParameterValue(key, value);
            if (!result.isValid) {
                return result;
            }
        }
        
        return ValidationResult::success();
    }
    
    // Validate component factory configuration
    ValidationResult ConfigValidator::validateComponentFactory(const IComponentFactory& factory) const {
        ValidationResult overallResult = ValidationResult::success();
        
        // Run factory-specific validators
        for (const auto& validator : factoryValidators_) {
            ValidationResult result = validator(config_);
            if (!result.isValid) {
                overallResult.isValid = false;
                if (overallResult.errorMessage.empty()) {
                    overallResult.errorMessage = result.errorMessage;
                } else {
                    overallResult.errorMessage += "\n" + result.errorMessage;
                }
                overallResult.details.insert(result.details.begin(), result.details.end());
            }
        }
        
        // Validate plugin configurations
        if (parameterResolver_) {
            ValidationResult pluginResult = validatePluginConfigurations(factory);
            if (!pluginResult.isValid) {
                overallResult.isValid = false;
                if (overallResult.errorMessage.empty()) {
                    overallResult.errorMessage = pluginResult.errorMessage;
                } else {
                    overallResult.errorMessage += "\n" + pluginResult.errorMessage;
                }
                overallResult.details.insert(pluginResult.details.begin(), pluginResult.details.end());
            }
        }
        
        return overallResult;
    }
    
    // Plugin configuration validation
    ValidationResult ConfigValidator::validatePluginConfigurations(const IComponentFactory& factory) const {
        ValidationResult overallResult = ValidationResult::success();
        
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
                    pluginResult.details.insert(result.details.begin(), result.details.end());
                }
            }
            
            if (!pluginResult.isValid) {
                overallResult.isValid = false;
                if (overallResult.errorMessage.empty()) {
                    overallResult.errorMessage = "Plugin \"" + pluginName + "\" validation failed:" + pluginResult.errorMessage;
                } else {
                    overallResult.errorMessage += "\nPlugin \"" + pluginName + "\" validation failed:" + pluginResult.errorMessage;
                }
                overallResult.details.insert(pluginResult.details.begin(), pluginResult.details.end());
            }
        }
        
        return overallResult;
    }
    
    // Parameter validation
    ValidationResult ConfigValidator::validateParameters(const std::shared_ptr<Config>& config) const {
        if (!parameterResolver_) {
            return ValidationResult::success();
        }
        
        ValidationResult overallResult = ValidationResult::success();
        
        for (const auto& key : config->getKeys()) {
            ConfigEntry entry;
            auto it = std::find_if(config->pImpl->entries.begin(), config->pImpl->entries.end(),
                                 [&key](const ConfigEntry& e) { return e.key == key; });
            if (it != config->pImpl->entries.end()) {
                entry = *it;
                ValidationResult result = parameterResolver_->validateParameterValue(key, entry.value);
                if (!result.isValid) {
                    overallResult.isValid = false;
                    if (overallResult.errorMessage.empty()) {
                        overallResult.errorMessage = "Parameter validation failed for key '" + key + "': " + result.errorMessage;
                    } else {
                        overallResult.errorMessage += "\nParameter validation failed for key '" + key + "': " + result.errorMessage;
                    }
                    overallResult.details.insert(result.details.begin(), result.details.end());
                }
            }
        }
        
        return overallResult;
    }
    
    // Change callbacks
    
    // Run validation callbacks
    ValidationResult ConfigValidator::runValidationCallbacks(const std::shared_ptr<Config>& config) const {
        ValidationResult overallResult = ValidationResult::success();
        
        for (const auto& callback : validationCallbacks_) {
            ValidationResult result = ValidationResult::success();
            
            // Get all config entries
            std::map<std::string, ConfigEntry> allEntries;
            for (const auto& key : config->getKeys()) {
                auto it = std::find_if(config->pImpl->entries.begin(), config->pImpl->entries.end(),
                                     [&key](const ConfigEntry& e) { return e.key == key; });
                if (it != config->pImpl->entries.end()) {
                    allEntries[key] = *it;
                }
            }
            
            for (const auto& pair : allEntries) {
                ValidationResult entryResult = callback(pair.first, pair.second.value, &pair.second);
                if (!entryResult.isValid) {
                    overallResult.isValid = false;
                    if (overallResult.errorMessage.empty()) {
                        overallResult.errorMessage = entryResult.errorMessage;
                    } else {
                        overallResult.errorMessage += "\n" + entryResult.errorMessage;
                    }
                    overallResult.details.insert(entryResult.details.begin(), entryResult.details.end());
                }
            }
        }
        
        return overallResult;
    }
    
    // Configuration validation with parameter resolver
    bool ConfigValidator::validateAndResolve(const std::string& parameterName, 
                                            ParameterConversionResult& result,
                                            const std::string& defaultValue) const {
        if (!parameterResolver_) {
            return false;
        }
        
        // Try to get parameter
        if (parameterResolver_->hasParameter(parameterName)) {
            result = parameterResolver_->getString(parameterName, defaultValue);
            return result.success;
        }
        
        // Try to get as int
        ParameterConversionResult intResult = parameterResolver_->getInt(parameterName, std::stoi(defaultValue));
        if (intResult.success) {
            result = intResult;
            return true;
        }
        
        // Try to get as double
        ParameterConversionResult doubleResult = parameterResolver_->getDouble(parameterName, std::stod(defaultValue));
        if (doubleResult.success) {
            result = doubleResult;
            return true;
        }
        
        // Try to get as bool
        ParameterConversionResult boolResult = parameterResolver_->getBool(parameterName, defaultValue == "true");
        if (boolResult.success) {
            result = boolResult;
            return true;
        }
        
        // Use default value
        result = ParameterConversionResult::success(defaultValue);
        return true;
    }
    
    // Get configuration
    std::shared_ptr<Config> ConfigValidator::getConfig() const {
        return config_;
    }
    
    // Get parameter resolver
    ParameterResolver* ConfigValidator::getParameterResolver() const {
        return parameterResolver_.get();
    }
    
    // Get schema
    const ConfigSchema* ConfigValidator::getSchema() const {
        return schema_.get();
    }
    
    // Configuration summary
    std::string ConfigValidator::getValidationSummary() const {
        std::ostringstream oss;
        oss << "ConfigValidator Summary:" << std::endl;
        oss << "  Configuration present: " << (config_ ? "Yes" : "No") << std::endl;
        oss << "  Schema present: " << (schema_ ? "Yes" : "No") << std::endl;
        oss << "  Parameter resolver present: " << (parameterResolver_ ? "Yes" : "No") << std::endl;
        oss << "  Monitoring enabled: " << (monitoringEnabled_ ? "Yes" : "No") << std::endl;
        oss << "  Runtime validation enabled: " << (runtimeValidationEnabled_ ? "Yes" : "No") << std::endl;
        oss << "  Validation statistics:" << std::endl;
        oss << "    Total validations: " << totalValidations_.load() << std::endl;
        oss << "    Successful validations: " << successfulValidations_.load() << std::endl;
        oss << "    Failed validations: " << failedValidations_.load() << std::endl;
        if (totalValidations_.load() > 0) {
            oss << "    Success rate: " << (static_cast<double>(successfulValidations_.load()) / totalValidations_.load() * 100.0) << "%" << std::endl;
        }
        
        return oss.str();
    }
    
    // Clear all callbacks and validators
    void ConfigValidator::clear() {
        std::lock_guard<std::mutex> lock(monitoringMutex_);
        changeCallbacks_.clear();
        validationCallbacks_.clear();
        factoryValidators_.clear();
        pluginValidators_.clear();
        runtimeValidationEnabled_ = false;
    }
    
    // Monitoring thread function
    void ConfigValidator::monitoringThreadFunc() {
        while (monitoringEnabled_.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            if (config_) {
                std::lock_guard<std::mutex> lock(monitoringMutex_);
                
                // Check for changes
                for (const auto& key : config_->getKeys()) {
                    auto it = std::find_if(config_->pImpl->entries.begin(), config_->pImpl->entries.end(),
                                         [&key](const ConfigEntry& e) { return e.key == key; });
                    if (it != config_->pImpl->entries.end()) {
                        ConfigChange change = {key, it->value, it->value, it->source, "check"};
                        for (const auto& callback : changeCallbacks_) {
                            callback(change);
                        }
                    }
                }
            }
        }
    }
    
    // Runtime validation thread function
    void ConfigValidator::runtimeValidationThreadFunc() {
        while (runtimeValidationEnabled_.load()) {
            std::this_thread::sleep_for(validationIntervalMs_);
            
            if (config_) {
                ValidationResult result = validate(config_);
                if (!result.isValid) {
                    // In a real implementation, this would use the logger
                    std::ostringstream oss;
                    oss << "Runtime validation failed:" << std::endl;
                    oss << result.errorMessage;
                    // Log error
                }
            }
        }
    }
    
} // namespace nlm