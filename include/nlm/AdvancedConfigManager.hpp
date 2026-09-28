#ifndef NLM_ADVANCED_CONFIG_H
#define NLM_ADVANCED_CONFIG_H

#include "core/Config/Config.hpp"
#include <string>
#include <map>
#include <functional>
#include <any>
#include <memory>
#include <vector>
#include <iostream>

namespace nlm {

/**
 * Advanced Configuration Manager for NLM
 * 
 * Provides enhanced configuration capabilities including:
 * - Dynamic configuration validation
 * - Type-safe configuration access
 * - Configuration templates
 * - Runtime configuration modification
 * - Configuration logging and monitoring
 * - Advanced command-line interface
 * 
 * This is intended for advanced users who need more control over
 * the neural simulation configuration.
 */
class AdvancedConfigManager {
public:
    AdvancedConfigManager(std::shared_ptr<Config> baseConfig);
    ~AdvancedConfigManager() = default;
    
    // Disable copying
    AdvancedConfigManager(const AdvancedConfigManager&) = delete;
    AdvancedConfigManager& operator=(const AdvancedConfigManager&) = delete;
    
    // Enable moving
    AdvancedConfigManager(AdvancedConfigManager&&) noexcept = default;
    AdvancedConfigManager& operator=(AdvancedConfigManager&&) noexcept = default;
    
    /**
     * Register a configuration parameter with validation and defaults
     * 
     * @param key Configuration key
     * @param defaultValue Default value
     * @param validator Validation function (returns true if valid)
     * @param description Parameter description
     * @param typeName Human-readable type name
     */
    template<typename T>
    void registerParameter(const std::string& key, 
                          const T& defaultValue,
                          std::function<bool(const T&)> validator,
                          const std::string& description = "",
                          const std::string& typeName = "unknown");
    
    /**
     * Get configuration parameter value
     * 
     * @param key Configuration key
     * @return Parameter value or default if not set
     */
    template<typename T>
    T getParameter(const std::string& key) const;
    
    /**
     * Set configuration parameter value
     * 
     * @param key Configuration key
     * @param value New value
     * @return true if successful, false if validation fails
     */
    template<typename T>
    bool setParameter(const std::string& key, const T& value);
    
    /**
     * Check if parameter is valid
     * 
     * @param key Configuration key
     * @return true if parameter exists and is valid
     */
    template<typename T>
    bool isValid(const std::string& key) const;
    
    /**
     * Get all registered parameter keys
     * 
     * @return Vector of parameter keys
     */
    std::vector<std::string> getRegisteredKeys() const;
    
    /**
     * Get parameter description
     * 
     * @param key Configuration key
     * @return Parameter description
     */
    std::string getDescription(const std::string& key) const;
    
    /**
     * Get parameter type name
     * 
     * @param key Configuration key
     * @return Parameter type name
     */
    std::string getTypeName(const std::string& key) const;
    
    /**
     * Validate all registered parameters
     * 
     * @return true if all parameters are valid, false otherwise
     */
    bool validateAll() const;
    
    /**
     * Get validation error messages
     * 
     * @return Vector of error messages
     */
    std::vector<std::string> getValidationErrors() const;
    
    /**
     * Apply configuration to the base config
     * 
     * @param source Which configuration source to apply to
     */
    void applyToBase(ConfigSource source = ConfigSource::Runtime);
    
    /**
     * Generate configuration report
     * 
     * @return Formatted configuration report
     */
    std::string generateReport() const;
    
    /**
     * Load configuration from file
     * 
     * @param filepath Configuration file path
     * @return true if successful
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * Save configuration to file
     * 
     * @param filepath Configuration file path
     * @return true if successful
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * Get base configuration
     * 
     * @return Pointer to base configuration
     */
    std::shared_ptr<Config> getBaseConfig() const { return baseConfig_; }
    
    /**
     * Add validation callback
     * 
     * @param callback Function to call for validation
     */
    void addValidationCallback(std::function<bool(const std::string&, const std::any&)> callback);
    
    /**
     * Clear all registered parameters
     */
    void clear();

private:
    struct ParameterInfo {
        std::string key;
        std::any value;
        std::string description;
        std::string typeName;
        std::function<bool(const std::any&)> validator;
        std::function<std::string(const std::any&)> formatter;
        bool isValid;
        std::vector<std::string> errors;
        
        ParameterInfo() : isValid(true) {}
    };
    
    std::shared_ptr<Config> baseConfig_;
    std::map<std::string, ParameterInfo> parameters_;
    std::vector<std::function<bool(const std::string&, const std::any&)>> validationCallbacks_;
    mutable std::vector<std::string> validationErrors_;
    
    // Internal helper methods
    std::string getTypeNameFromAny(const std::any& value) const;
    bool validateAny(const std::string& key, const std::any& value) const;
    std::any getDefaultFromTypeName(const std::string& typeName) const;
};

// Template implementations

template<typename T>
void AdvancedConfigManager::registerParameter(const std::string& key,
                                             const T& defaultValue,
                                             std::function<bool(const T&)> validator,
                                             const std::string& description,
                                             const std::string& typeName) {
    ParameterInfo info;
    info.key = key;
    info.value = defaultValue;
    info.description = description;
    info.typeName = typeName.empty() ? getTypeNameFromAny(defaultValue) : typeName;
    info.validator = [validator](const std::any& value) {
        try {
            return validator(std::any_cast<T>(value));
        } catch (...) {
            return false;
        }
    };
    info.isValid = validator(defaultValue);
    parameters_[key] = info;
}

template<typename T>
T AdvancedConfigManager::getParameter(const std::string& key) const {
    auto it = parameters_.find(key);
    if (it == parameters_.end()) {
        // Return default value for common types if not found
        return T();
    }
    
    try {
        return std::any_cast<T>(it->second.value);
    } catch (...) {
        // Return default if type mismatch
        return T();
    }
}

template<typename T>
bool AdvancedConfigManager::setParameter(const std::string& key, const T& value) {
    auto it = parameters_.find(key);
    if (it == parameters_.end()) {
        return false;  // Parameter not registered
    }
    
    bool valid = true;
    if (it->second.validator) {
        valid = it->second.validator(std::any(value));
    }
    
    if (valid) {
        it->second.value = value;
        it->second.isValid = true;
        it->second.errors.clear();
    } else {
        it->second.isValid = false;
        it->second.errors.push_back("Parameter validation failed");
    }
    
    // Call validation callbacks
    for (const auto& callback : validationCallbacks_) {
        if (!callback(key, std::any(value))) {
            valid = false;
        }
    }
    
    return valid;
}

template<typename T>
bool AdvancedConfigManager::isValid(const std::string& key) const {
    auto it = parameters_.find(key);
    if (it == parameters_.end()) {
        return false;
    }
    
    bool valid = it->second.isValid;
    if (valid && it->second.validator) {
        valid = it->second.validator(it->second.value);
    }
    
    return valid;
}

} // namespace nlm

#endif // NLM_ADVANCED_CONFIG_H
