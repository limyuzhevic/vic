// ValidatedConfig.cpp - Implementation of configuration validation system

#include "ValidatedConfig.hpp"
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace nlm {

ConfigSchema::ConfigSchema() : validationEnabled_(true) {}

void ConfigSchema::addRule(const std::string& key, 
                           const std::function<bool(const ConfigValue&)>& validator,
                           const std::string& errorMessage,
                           bool required,
                           const std::vector<std::string>& allowedValues,
                           const std::pair<double, double>& range,
                           const std::string& description) {
    ConfigValidationRule rule;
    rule.key = key;
    rule.validator = validator;
    rule.errorMessage = errorMessage;
    rule.required = required;
    rule.allowedValues = allowedValues;
    rule.range = range;
    rule.description = description;
    
    rules_.push_back(rule);
    descriptions_[key] = description;
}

void ConfigSchema::addIntRule(const std::string& key, const std::pair<int64_t, int64_t>& range, bool required) {
    addRule(key,
            [range](const ConfigValue& value) {
                if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<int>(value)) {
                    return false;
                }
                int64_t val;
                if (std::holds_alternative<int64_t>(value)) {
                    val = std::get<int64_t>(value);
                } else {
                    val = static_cast<int64_t>(std::get<int>(value));
                }
                return val >= range.first && val <= range.second;
            },
            "Value must be an integer within range [" + std::to_string(range.first) + ", " + std::to_string(range.second) + "]",
            required,
            {},
            {static_cast<double>(range.first), static_cast<double>(range.second)},
            "Integer parameter within specified range");
}

void ConfigSchema::addFloatRule(const std::string& key, const std::pair<double, double>& range, bool required) {
    addRule(key,
            [range](const ConfigValue& value) {
                if (!std::holds_alternative<double>(value) && !std::holds_alternative<float>(value)) {
                    return false;
                }
                double val;
                if (std::holds_alternative<double>(value)) {
                    val = std::get<double>(value);
                } else {
                    val = static_cast<double>(std::get<float>(value));
                }
                return val >= range.first && val <= range.second;
            },
            "Value must be a float within range [" + std::to_string(range.first) + ", " + std::to_string(range.second) + "]",
            required,
            {},
            range,
            "Float parameter within specified range");
}

void ConfigSchema::addStringRule(const std::string& key, bool required, const std::vector<std::string>& allowedValues) {
    addRule(key,
            [allowedValues](const ConfigValue& value) {
                if (!std::holds_alternative<std::string>(value)) {
                    return false;
                }
                if (allowedValues.empty()) {
                    return true; // Any string is valid
                }
                return std::find(allowedValues.begin(), allowedValues.end(), std::get<std::string>(value)) != allowedValues.end();
            },
            "Value must be one of the allowed strings: " + formatStringList(allowedValues),
            required,
            allowedValues,
            {-1e6, 1e6},
            "String parameter with allowed values");
}

void ConfigSchema::addBoolRule(const std::string& key, bool required) {
    addRule(key,
            [](const ConfigValue& value) {
                return std::holds_alternative<bool>(value);
            },
            "Value must be a boolean (true/false)",
            required,
            {},
            {-1e6, 1e6},
            "Boolean parameter");
}

std::vector<ConfigValidationError> ConfigSchema::validate(const ConfigEntry& entry) const {
    std::vector<ConfigValidationError> errors;
    
    if (!validationEnabled_) {
        return errors;
    }
    
    // Find matching rule for this key
    auto rule = std::find_if(rules_.begin(), rules_.end(), 
                            [&entry](const ConfigValidationRule& r) { return r.key == entry.key; });
    
    if (rule == rules_.end()) {
        // No specific rule for this key, check for description
        if (descriptions_.find(entry.key) != descriptions_.end()) {
            // Key has a description, but no validation rule
            // This is okay - not all keys need validation
        }
        return errors;
    }
    
    // Apply validation rules
    bool valid = rule->validator(entry.value);
    
    if (!valid) {
        errors.push_back(ConfigValidationError::InvalidValue);
    }
    
    return errors;
}

std::vector<std::pair<ConfigValidationError, std::string>> ConfigSchema::validate(const Config& config) const {
    std::vector<std::pair<ConfigValidationError, std::string>> results;
    
    if (!validationEnabled_) {
        return results;
    }
    
    // Get all keys from config
    std::vector<std::string> keys = config.getKeys();
    
    for (const std::string& key : keys) {
        // Get or attempt to get value from config
        auto valueOpt = getValueFromConfig(config, key);
        if (!valueOpt) {
            results.push_back(std::make_pair(ConfigValidationError::MissingRequired, 
                                           "Key '" + key + "' has no value"));
            continue;
        }
        
        const ConfigValue& value = *valueOpt;
        
        // Validate this entry
        std::vector<ConfigValidationError> entryErrors = validate(ConfigEntry(key, value, ConfigSource::Runtime));
        
        for (ConfigValidationError error : entryErrors) {
            std::string errorMsg = generateValidationErrorMessage(key, error, value);
            results.push_back(std::make_pair(error, errorMsg));
        }
    }
    
    // Check for missing required keys
    std::vector<std::string> requiredKeys = getRequiredKeys();
    
    for (const std::string& key : requiredKeys) {
        if (std::find(keys.begin(), keys.end(), key) == keys.end()) {
            results.push_back(std::make_pair(ConfigValidationError::MissingRequired, 
                                           "Required key '" + key + "' is missing"));
        }
    }
    
    return results;
}

std::vector<std::string> ConfigSchema::getRequiredKeys() const {
    std::vector<std::string> requiredKeys;
    
    for (const ConfigValidationRule& rule : rules_) {
        if (rule.required) {
            requiredKeys.push_back(rule.key);
        }
    }
    
    return requiredKeys;
}

std::string ConfigSchema::getDescription(const std::string& key) const {
    auto it = descriptions_.find(key);
    if (it != descriptions_.end()) {
        return it->second;
    }
    return "";
}

std::unordered_map<std::string, std::string> ConfigSchema::getAllDescriptions() const {
    return descriptions_;
}

bool ConfigSchema::isKnownKey(const std::string& key) const {
    return descriptions_.find(key) != descriptions_.end();
}

std::vector<ConfigValidationRule> ConfigSchema::getRulesForKey(const std::string& key) const {
    std::vector<ConfigValidationRule> keyRules;
    
    for (const ConfigValidationRule& rule : rules_) {
        if (rule.key == key) {
            keyRules.push_back(rule);
        }
    }
    
    return keyRules;
}

// ValidatedConfig implementation

ValidatedConfig::ValidatedConfig() : Config(), schema_(std::make_shared<ConfigSchema>()) {}

bool ValidatedConfig::validateBeforeSet(const std::string& key, const ConfigValue& value) const {
    if (!schema_ || !schema_->isValidationEnabled()) {
        return true; // No validation
    }
    
    ConfigEntry entry(key, value, ConfigSource::Runtime);
    std::vector<ConfigValidationError> errors = schema_->validate(entry);
    
    if (!errors.empty()) {
        for (ConfigValidationError error : errors) {
            std::string errorMsg = generateValidationErrorMessage(key, error, value);
            validationMessages_.push_back(errorMsg);
        }
        return false;
    }
    
    return true;
}

std::vector<std::string> ValidatedConfig::getValidationErrors(const std::string& key, const ConfigValue& value) const {
    std::vector<std::string> errors;
    
    if (schema_ && schema_->isValidationEnabled()) {
        ConfigEntry entry(key, value, ConfigSource::Runtime);
        std::vector<ConfigValidationError> validationErrors = schema_->validate(entry);
        
        for (ConfigValidationError error : validationErrors) {
            errors.push_back(generateValidationErrorMessage(key, error, value));
        }
    }
    
    return errors;
}

void ValidatedConfig::clearAndValidate() {
    Config::clear();
    validationMessages_.clear();
}

bool ValidatedConfig::loadFromFile(const std::string& filepath) {
    bool success = Config::loadFromFile(filepath);
    if (success && schema_) {
        // Validate after loading
        std::vector<std::pair<ConfigValidationError, std::string>> validationResults = schema_->validate(*this);
        for (const auto& result : validationResults) {
            validationMessages_.push_back(result.second);
        }
    }
    return success;
}

bool ValidatedConfig::loadFromArgs(int argc, char** argv) {
    bool success = Config::loadFromArgs(argc, argv);
    if (success && schema_) {
        // Validate after loading
        std::vector<std::pair<ConfigValidationError, std::string>> validationResults = schema_->validate(*this);
        for (const auto& result : validationResults) {
            validationMessages_.push_back(result.second);
        }
    }
    return success;
}

// ConfigValidation namespace implementation

bool ConfigValidation::validateInt(const ConfigValue& value) {
    return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
}

bool ConfigValidation::validateFloat(const ConfigValue& value) {
    return std::holds_alternative<float>(value) || std::holds_alternative<double>(value);
}

bool ConfigValidation::validateString(const ConfigValue& value) {
    return std::holds_alternative<std::string>(value);
}

bool ConfigValidation::validateBool(const ConfigValue& value) {
    return std::holds_alternative<bool>(value);
}

bool ConfigValidation::validateVector(const ConfigValue& value) {
    return std::holds_alternative<std::vector<int>>(value) || 
           std::holds_alternative<std::vector<double>>(value) || 
           std::holds_alternative<std::vector<std::string>>(value);
}

bool ConfigValidation::validateRange(const ConfigValue& value, const std::pair<double, double>& range) {
    if (std::holds_alternative<float>(value)) {
        float val = std::get<float>(value);
        return val >= range.first && val <= range.second;
    } else if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        return val >= range.first && val <= range.second;
    }
    return false;
}

bool ConfigValidation::validateIntRange(const ConfigValue& value, const std::pair<int64_t, int64_t>& range) {
    if (std::holds_alternative<int>(value)) {
        int val = std::get<int>(value);
        return val >= range.first && val <= range.second;
    } else if (std::holds_alternative<int64_t>(value)) {
        int64_t val = std::get<int64_t>(value);
        return val >= range.first && val <= range.second;
    }
    return false;
}

bool ConfigValidation::validateAllowedValues(const ConfigValue& value, const std::vector<std::string>& allowedValues) {
    if (!std::holds_alternative<std::string>(value)) {
        return false;
    }
    const std::string& val = std::get<std::string>(value);
    return std::find(allowedValues.begin(), allowedValues.end(), val) != allowedValues.end();
}

template<typename T>
bool ConfigValidation::convertAndValidate(const ConfigValue& from, T& to) {
    if constexpr (std::is_same_v<T, int>) {
        if (std::holds_alternative<int>(from)) {
            to = std::get<int>(from);
            return true;
        }
    } else if constexpr (std::is_same_v<T, int64_t>) {
        if (std::holds_alternative<int64_t>(from)) {
            to = std::get<int64_t>(from);
            return true;
        }
    } else if constexpr (std::is_same_v<T, float>) {
        if (std::holds_alternative<float>(from)) {
            to = std::get<float>(from);
            return true;
        }
    } else if constexpr (std::is_same_v<T, double>) {
        if (std::holds_alternative<double>(from)) {
            to = std::get<double>(from);
            return true;
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        if (std::holds_alternative<bool>(from)) {
            to = std::get<bool>(from);
            return true;
        }
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (std::holds_alternative<std::string>(from)) {
            to = std::get<std::string>(from);
            return true;
        }
    }
    return false;
}

std::string ConfigValidation::generateValidationErrorMessage(const std::string& key, 
                                                           ConfigValidationError error,
                                                           const ConfigValue& value) {
    std::string typeStr;
    
    if (std::holds_alternative<int>(value)) {
        typeStr = "int";
    } else if (std::holds_alternative<int64_t>(value)) {
        typeStr = "int64_t";
    } else if (std::holds_alternative<float>(value)) {
        typeStr = "float";
    } else if (std::holds_alternative<double>(value)) {
        typeStr = "double";
    } else if (std::holds_alternative<bool>(value)) {
        typeStr = "bool";
    } else if (std::holds_alternative<std::string>(value)) {
        typeStr = "string";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        typeStr = "int[]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        typeStr = "float[]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        typeStr = "string[]";
    } else {
        typeStr = "unknown";
    }
    
    std::ostringstream oss;
    oss << "Configuration validation error for key '" << key << "': ";
    
    switch (error) {
        case ConfigValidationError::InvalidType:
            oss << "Expected value of type " << typeStr << ", but received incompatible type";
            break;
        case ConfigValidationError::InvalidRange:
            oss << "Value is out of valid range";
            break;
        case ConfigValidationError::InvalidValue:
            oss << "Value '" << getValueString(value) << "' is not valid for key '" << key << "'";
            break;
        case ConfigValidationError::MissingRequired:
            oss << "Required key '" << key << "' is missing";
            break;
        case ConfigValidationError::DuplicateKey:
            oss << "Key '" << key << "' is specified multiple times";
            break;
        case ConfigValidationError::SchemaViolation:
            oss << "Configuration violates schema requirements";
            break;
        case ConfigValidationError::FileParseError:
            oss << "Error parsing configuration file";
            break;
        case ConfigValidationError::CommandLineError:
            oss << "Error parsing command line arguments";
            break;
        default:
            oss << "Unknown validation error";
            break;
    }
    
    return oss.str();
}

// Helper function to get string representation of ConfigValue
std::string ConfigValidation::getValueString(const ConfigValue& value) {
    std::ostringstream oss;
    
    if (std::holds_alternative<int>(value)) {
        oss << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        oss << std::get<int64_t>(value);
    } else if (std::holds_alternative<float>(value)) {
        oss << std::get<float>(value);
    } else if (std::holds_alternative<double>(value)) {
        oss << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        oss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        oss << "\"" << std::get<std::string>(value) << "\"";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        oss << "[";
        const auto& vec = std::get<std::vector<int>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        oss << "[";
        const auto& vec = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        oss << "[";
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << vec[i] << "\"";
        }
        oss << "]";
    } else {
        oss << "<empty>";
    }
    
    return oss.str();
}

// Helper function to get value from Config
ConfigValue ConfigValidation::getValueFromConfig(const Config& config, const std::string& key) {
    // Try to get value using various type-specific methods
    if (config.has(key + ".int")) {
        return config.get<int>(key + ".int").value_or(ConfigValue{});
    } else if (config.has(key + ".float")) {
        return config.get<float>(key + ".float").value_or(ConfigValue{});
    } else if (config.has(key + ".bool")) {
        return config.get<bool>(key + ".bool").value_or(ConfigValue{});
    } else if (config.has(key + ".string")) {
        return config.get<std::string>(key + ".string").value_or(ConfigValue{});
    } else {
        // Try direct key access
        return ConfigValue{}; // Empty ConfigValue as placeholder
    }
}

std::string ConfigValidation::formatStringList(const std::vector<std::string>& strings) {
    std::ostringstream oss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << strings[i] << "\"";
    }
    return oss.str();
}

} // namespace nlm