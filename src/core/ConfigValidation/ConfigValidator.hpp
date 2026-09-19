#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include "ConfigError.hpp"
#include "ValidationRules.hpp"
#include "ConfigSchema.hpp"
#include "ParameterResolver.hpp"
#include "../ComponentFactory/ComponentFactory.hpp"
#include "../Config/Config.hpp"

namespace nlm {

// Configuration change notification
struct ConfigChange {
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    ConfigSource source;
    std::chrono::system_clock::time_point timestamp;
    std::string changeType; // "set", "remove", "clear"
    
    ConfigChange() : timestamp(std::chrono::system_clock::now()) {}
    
    ConfigChange(const std::string& k, const ConfigValue& oldVal, const ConfigValue& newVal,
                ConfigSource s, const std::string& type = "set")
        : key(k), oldValue(oldVal), newValue(newVal), source(s), changeType(type) {}
};

// Configuration change callback
using ConfigChangeCallback = std::function<void(const ConfigChange& change)>;

// Validation callback for dynamic validation
using ConfigValidationCallback = std::function<ValidationResult(const std::string& key, const ConfigValue& value, const ConfigEntry* entry)>;

// Component factory validator
using ComponentFactoryValidator = std::function<ValidationResult(const std::shared_ptr<Config>& config)>;

// Main configuration validator
class ConfigValidator {
private:
    std::shared_ptr<Config> config_;
    std::unique_ptr<ConfigSchema> schema_;
    std::unique_ptr<ParameterResolver> parameterResolver_;
    
    // Change monitoring
    std::atomic<bool> monitoringEnabled_ = false;
    std::thread monitoringThread_;
    std::mutex monitoringMutex_;
    std::vector<ConfigChangeCallback> changeCallbacks_;
    std::vector<ConfigValidationCallback> validationCallbacks_;
    
    // Component factory integration
    std::vector<ComponentFactoryValidator> factoryValidators_;
    
    // Plugin configuration validation
    std::map<std::string, std::vector<ConfigValidationCallback>> pluginValidators_;
    
    // Runtime monitoring
    std::atomic<bool> runtimeValidationEnabled_ = false;
    std::chrono::milliseconds validationIntervalMs_ = std::chrono::milliseconds(100);
    std::thread runtimeValidationThread_;
    
    // Statistics
    std::atomic<size_t> totalValidations_ = 0;
    std::atomic<size_t> successfulValidations_ = 0;
    std::atomic<size_t> failedValidations_ = 0;
    
public:
    ConfigValidator(std::shared_ptr<Config> config = nullptr)
        : config_(config), monitoringEnabled_(false) {
        parameterResolver_ = std::make_unique<ParameterResolver>(config);
    }
    
    ~ConfigValidator() {
        stopRuntimeValidation();
        stopMonitoring();
    }
    
    // Set configuration
    void setConfig(std::shared_ptr<Config> config) {
        std::lock_guard<std::mutex> lock(monitoringMutex_);
        config_ = config;
        parameterResolver_->setConfig(config);
        
        // Notify of configuration change
        if (config_) {
            for (const auto& callback : changeCallbacks_) {
                ConfigChange change("config_set", ConfigValue(std::string("new_config")),
                                  ConfigValue(std::string("new_config")),
                                  ConfigSource::Runtime, "set");
                callback(change);
            }
        }
    }
    
    // Set schema
    void setSchema(std::unique_ptr<ConfigSchema> schema) {
        schema_ = std::move(schema);
    }
    
    // Set parameter resolver
    void setParameterResolver(std::unique_ptr<ParameterResolver> resolver) {
        parameterResolver_ = std::move(resolver);
    }
    
    // Configuration validation
    
    // Validate entire configuration against schema
    ValidationResult validate(const std::shared_ptr<Config>& config = nullptr) const {
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
    ValidationResult validateKey(const std::string& key, const ConfigValue& value) const {
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
    ValidationResult validateComponentFactory(const IComponentFactory& factory) const {
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
    ValidationResult validatePluginConfigurations(const IComponentFactory& factory) const {
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
    ValidationResult validateParameters(const std::shared_ptr<Config>& config) const {
        if (!parameterResolver_) {
            return ValidationResult::success();
        }
        
        ValidationResult overallResult = ValidationResult::success();
        
        for (const auto& key : config->getKeys()) {
            ConfigEntry entry;
            // Get the actual config entry
            auto it = std::find_if(config->pImpl->entries.begin(), config->pImpl->entries.end(),
                                 [&key](const ConfigEntry& e) { return e.key == key; });n            if (it != config->pImpl->entries.end()) {
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
    
    // Add change callback
    void addChangeCallback(const ConfigChangeCallback& callback) {
        std::lock_guard<std::mutex> lock(monitoringMutex_);
        changeCallbacks_.push_back(callback);
    }
    
    // Add validation callback
    void addValidationCallback(const ConfigValidationCallback& callback) {
        validationCallbacks_.push_back(callback);
    }
    
    // Add factory validator
    void addFactoryValidator(const ComponentFactoryValidator& validator) {
        factoryValidators_.push_back(validator);
    }
    
    // Add plugin validator
    void addPluginValidator(const std::string& pluginName, const ConfigValidationCallback& validator) {
        pluginValidators_[pluginName].push_back(validator);
    }
    
    // Run validation callbacks
    ValidationResult runValidationCallbacks(const std::shared_ptr<Config>& config) const {
        ValidationResult overallResult = ValidationResult::success();
        
        for (const auto& callback : validationCallbacks_) {
            ValidationResult result = ValidationResult::success();
            
            // Get all config entries
            std::map<std::string, ConfigEntry> allEntries;
            for (const auto& key : config->getKeys()) {
                auto it = std::find_if(config->pImpl->entries.begin(), config->pImpl->entries.end(),
                                     [&key](const ConfigEntry& e) { return e.key == key; });n                if (it != config->pImpl->entries.end()) {
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
    
    // Change monitoring
    
    // Start monitoring configuration changes
    void startMonitoring() {
        if (monitoringEnabled_.exchange(true)) {
            return; // Already monitoring
        }
        
        monitoringThread_ = std::thread(&ConfigValidator::monitoringThreadFunc, this);
    }
    
    // Stop monitoring configuration changes
    void stopMonitoring() {
        if (!monitoringEnabled_.exchange(false)) {
            return; // Already stopped
        }
        
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
    }
    
    // Start runtime validation
    void startRuntimeValidation(const std::chrono::milliseconds& interval = std::chrono::milliseconds(100)) {
        if (runtimeValidationEnabled_.exchange(true)) {
            return; // Already enabled
        }
        
        validationIntervalMs_ = interval;
        runtimeValidationThread_ = std::thread(&ConfigValidator::runtimeValidationThreadFunc, this);
    }
    
    // Stop runtime validation
    void stopRuntimeValidation() {
        if (!runtimeValidationEnabled_.exchange(false)) {
            return; // Already stopped
        }
        
        if (runtimeValidationThread_.joinable()) {
            runtimeValidationThread_.join();
        }
    }
    
    // Get validation statistics
    struct ValidationStats {
        size_t totalValidations = 0;
        size_t successfulValidations = 0;
        size_t failedValidations = 0;
        double successRate() const {
            return totalValidations > 0 ? (static_cast<double>(successfulValidations) / totalValidations) * 100.0 : 0.0;
        }
    };
    
    ValidationStats getValidationStats() const {
        ValidationStats stats;
        stats.totalValidations = totalValidations_.load();
        stats.successfulValidations = successfulValidations_.load();
        stats.failedValidations = failedValidations_.load();
        return stats;
    }
    
    // Configuration validation with parameter resolver
    bool validateAndResolve(const std::string& parameterName, 
                           ParameterConversionResult& result,
                           const std::string& defaultValue = "") const {
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
    std::shared_ptr<Config> getConfig() const {
        return config_;
    }
    
    // Get parameter resolver
    ParameterResolver* getParameterResolver() const {
        return parameterResolver_.get();
    }
    
    // Get schema
    const ConfigSchema* getSchema() const {
        return schema_.get();
    }
    
    // Configuration summary
    std::string getValidationSummary() const {
        std::ostringstream oss;
        oss << "ConfigValidator Summary:" << std::endl;
        oss << "  Configuration present: " << (config_ ? "Yes" : "No") << std::endl;
        oss << "  Schema present: " << (schema_ ? "Yes" : "No") << std::endl;
        oss << "  Parameter resolver present: " << (parameterResolver_ ? "Yes" : "No") << std::endl;
        oss << "  Monitoring enabled: " << (monitoringEnabled_ ? "Yes" : "No") << std::endl;
        oss << "  Runtime validation enabled: " << (runtimeValidationEnabled_ ? "Yes" : "No") << std::endl;
        oss << "  Validation statistics:" << std::endl;
        oss << "    Total validations: " << totalValidations_.load() << std::endl;
        oss << "    Successful: " << successfulValidations_.load() << std::endl;
        oss << "    Failed: " << failedValidations_.load() << std::endl;
        if (totalValidations_.load() > 0) {
            oss << "    Success rate: " << (static_cast<double>(successfulValidations_.load()) / totalValidations_.load() * 100.0) << "%" << std::endl;
        }
        
        return oss.str();
    }
    
    // Clear all callbacks and validators
    void clear() {
        std::lock_guard<std::mutex> lock(monitoringMutex_);
        changeCallbacks_.clear();
        validationCallbacks_.clear();
        factoryValidators_.clear();
        pluginValidators_.clear();
        runtimeValidationEnabled_ = false;
    }
    
private:
    void monitoringThreadFunc() {
        while (monitoringEnabled_.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            if (config_) {
                std::lock_guard<std::mutex> lock(monitoringMutex_);
                
                // Check for changes
                for (const auto& key : config_->getKeys()) {
                    auto it = std::find_if(config->pImpl->entries.begin(), config->pImpl->entries.end(),
                                         [&key](const ConfigEntry& e) { return e.key == key; });n                    if (it != config->pImpl->entries.end()) {
                        ConfigChange change = {key, it->value, it->value, it->source, "check"};
                        for (const auto& callback : changeCallbacks_) {
                            callback(change);
                        }
                    }
                }
            }
        }
    }
    
    void runtimeValidationThreadFunc() {
        while (runtimeValidationEnabled_.load()) {
            std::this_thread::sleep_for(validationIntervalMs_);
            
            if (config_) {
                ValidationResult result = validate(config_);
                if (!result.isValid) {
                    // Log validation error
                    std::ostringstream oss;
                    oss << "Runtime validation failed:" << std::endl;
                    oss << result.errorMessage;
                    // In a real implementation, this would use the logger
                }
            }
        }
    }
};

} // namespace nlm