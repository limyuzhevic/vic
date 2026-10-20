#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <optional>
#include <chrono>
#include <set>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Configuration preset
struct ConfigurationPreset {
    std::string name;
    std::string description;
    std::map<std::string, std::string> defaultValues;
    std::vector<std::string> requiredParameters;
    std::vector<std::string> recommendedParameters;
    std::chrono::seconds timeout;
    std::string scenario;
    
    ConfigurationPreset(const std::string& presetName, const std::string& presetDesc)
        : name(presetName), description(presetDesc), timeout(std::chrono::seconds(3600)) {}
};

// Configuration change record
struct ConfigChange {
    std::string key;
    std::string oldValue;
    std::string newValue;
    ConfigSource source;
    std::chrono::steady_clock::time_point timestamp;
    std::string changedBy;
    
    ConfigChange(const std::string& k, const std::string& oldVal, const std::string& newVal,
                 ConfigSource src, const std::string& changedByBy)
        : key(k), oldValue(oldVal), newValue(newVal), source(src), changedBy(changedByBy) {
        timestamp = std::chrono::steady_clock::now();
    }
};

// Enhanced configuration validation rules
struct ValidationRule {
    std::string parameter;
    std::function<bool(const std::string&)> validator;
    std::string errorMessage;
    bool required;
    std::vector<std::string> allowedValues;
    std::pair<double, double> range; // for numeric parameters
    std::string dependency; // parameter must exist for this to be valid
    
    ValidationRule(const std::string& param, std::function<bool(const std::string&)> val,
                   const std::string& error, bool req = false)
        : parameter(param), validator(val), errorMessage(error), required(req) {}
};

// Enhanced configuration manager
class EnhancedConfig {
public:
    EnhancedConfig(std::shared_ptr<Config> baseConfig, std::shared_ptr<Logger> logger);
    ~EnhancedConfig();
    
    // Configuration preset management
    void registerPreset(const ConfigurationPreset& preset);
    bool applyPreset(const std::string& presetName);
    bool createCustomPreset(const std::string& name, const std::string& description,
                           const std::map<std::string, std::string>& values);
    std::vector<std::string> getAvailablePresets() const;
    ConfigurationPreset getPreset(const std::string& name) const;
    
    // Runtime configuration modification
    bool setValue(const std::string& key, const std::string& value, 
                 ConfigSource source = ConfigSource::Runtime,
                 const std::string& changedBy = "");
    bool setValue(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
                 const std::string& changedBy = "");
    bool setValue(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
                 const std::string& changedBy = "");
    bool setValue(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
                 const std::string& changedBy = "");
    
    // Configuration validation
    ValidationResult validateConfiguration() const;
    ValidationResult validateParameter(const std::string& key) const;
    bool addValidationRule(const ValidationRule& rule);
    void clearValidationRules();
    
    // Configuration history
    bool enableChangeHistory(bool enable);
    std::vector<ConfigChange> getChangeHistory(size_t limit = 0) const;
    std::vector<ConfigChange> getChangesBySource(ConfigSource source) const;
    std::vector<ConfigChange> getChangesByKey(const std::string& key) const;
    
    // Configuration comparison
    bool hasChangesSince(const std::chrono::steady_clock::time_point& since) const;
    std::map<std::string, std::pair<std::string, std::string>> getDifferences(
        const EnhancedConfig& other) const;
    
    // Configuration serialization
    bool saveToFile(const std::string& filepath, bool includeHistory = false) const;
    bool loadFromFile(const std::string& filepath, bool merge = true);
    bool exportToJSON(const std::string& filepath) const;
    bool importFromJSON(const std::string& filepath, bool merge = true);
    
    // Configuration scenarios
    bool createScenario(const std::string& name, const std::string& description);
    bool applyScenario(const std::string& name);
    bool deleteScenario(const std::string& name);
    std::vector<std::string> getAvailableScenarios() const;
    
    // Configuration analysis
    std::map<std::string, std::string> getSummary() const;
    std::vector<std::string> getInvalidParameters() const;
    std::vector<std::string> getOutdatedParameters() const;
    double getConfigurationScore() const; // based on completeness and validity
    
    // Access to base configuration
    std::shared_ptr<Config> getBaseConfig() const;
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Configuration statistics
    size_t getTotalParameters() const;
    size_t getValidParameters() const;
    size_t getInvalidParameters() const;
    std::chrono::steady_clock::time_point getLastModified() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    bool validateAgainstRules(const std::string& key, const std::string& value) const;
    void recordChange(const std::string& key, const std::string& oldValue, const std::string& newValue,
                     ConfigSource source, const std::string& changedBy);
    std::string getStringValue(const std::string& key) const;
    bool isValueChanged(const std::string& key, const std::string& value) const;
    void cleanupOldHistory(size_t maxHistorySize) const;
    
    // Validation rule evaluation
    bool evaluateValidationRules(const ValidationRule& rule, const std::string& value) const;
    std::string getParameterType(const std::string& key) const;
    std::vector<std::string> getParameterAllowedValues(const std::string& key) const;
    std::pair<double, double> getParameterRange(const std::string& key) const;
};

// Configuration validator class
class ConfigurationValidator {
public:
    ConfigurationValidator(std::shared_ptr<Logger> logger);
    ~ConfigurationValidator();
    
    // Validation by category
    ValidationResult validateForScenario(const std::string& scenario) const;
    ValidationResult validateForTest(const std::string& testName) const;
    ValidationResult validateForPerformance(const std::string& performanceGoal) const;
    
    // Validation by parameter
    ValidationResult validateParameter(const std::string& key, const std::string& value) const;
    bool isParameterValid(const std::string& key, const std::string& value) const;
    std::string getParameterValidationError(const std::string& key, const std::string& value) const;
    
    // Preset validation
    ValidationResult validatePreset(const ConfigurationPreset& preset) const;
    
    // Scenario validation
    ValidationResult validateScenario(const std::string& name) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    ValidationResult validateNeuralParameters() const;
    ValidationResult validatePlasticityParameters() const;
    ValidationResult validateMemoryParameters() const;
    ValidationResult validatePerformanceParameters() const;
    
    std::vector<std::string> getRequiredParametersForScenario(const std::string& scenario) const;
    bool checkParameterDependencies(const std::string& key, const std::map<std::string, std::string>& allValues) const;
};

} // namespace nlm
