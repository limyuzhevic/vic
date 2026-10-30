#pragma once

#include "../Config/Config.hpp"
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <iostream>

namespace nlm {

// Configuration parameter validation types
using ValidationRule = std::function<bool(const ConfigValue&, std::string& errorMessage)>;
using DefaultProvider = std::function<ConfigValue()>;

// Configuration parameter metadata
struct ConfigParameter {
    std::string key;
    std::string type;
    ConfigValue defaultValue;
    std::string description;
    std::vector<ValidationRule> validators;
    bool required;
    
    ConfigParameter(const std::string& k, const std::string& t, 
                   const ConfigValue& defaultVal, const std::string& desc,
                   bool req = false)
        : key(k), type(t), defaultValue(defaultVal), description(desc), required(req) {}
    
    bool validate(const ConfigValue& value, std::string& error) const {
        for (const auto& validator : validators) {
            if (!validator(value, error)) {
                return false;
            }
        }
        return true;
    }
};

// Configuration validator class
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Register a parameter schema
    void registerParameter(const std::string& key, const std::string& type, 
                          const ConfigValue& defaultValue, 
                          const std::string& description,
                          bool required = false);
    
    // Add validation rule for a parameter
    template<typename T>
    void addValidator(const std::string& key, 
                     std::function<bool(const T&, std::string& error)> validator);
    
    // Validate configuration
    bool validate(const Config& config, std::vector<std::string>& errors) const;
    
    // Get all registered parameters
    std::vector<ConfigParameter> getParameters() const { return parameters; }
    
    // Get parameter by key
    const ConfigParameter* getParameter(const std::string& key) const;
    
    // Get default value for parameter
    template<typename T>
    T getDefaultValue(const std::string& key) const;
    
    // Generate default configuration
    Config generateDefaultConfig() const;
    
    // Get configuration summary
    std::string getValidationSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::vector<ConfigParameter> parameters;
    
    // Validation helper functions
    static bool validateIntRange(const ConfigValue& value, std::string& error);
    static bool validateDoubleRange(const ConfigValue& value, std::string& error);
    static bool validateStringNotEmpty(const ConfigValue& value, std::string& error);
    static bool validatePositive(const ConfigValue& value, std::string& error);
    static bool validateNonNegative(const ConfigValue& value, std::string& error);
    static bool validateBoolean(const ConfigValue& value, std::string& error);
    static bool validateFileExists(const ConfigValue& value, std::string& error);
    
    // Default value providers
    static ConfigValue getDefaultInt(const std::string& key);
    static ConfigValue getDefaultDouble(const std::string& key);
    static ConfigValue getDefaultString(const std::string& key);
    static ConfigValue getDefaultBool(const std::string& key);
    static ConfigValue getDefaultVectorInt(const std::string& key);
};

} // namespace nlm