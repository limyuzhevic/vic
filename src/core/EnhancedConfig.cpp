#include "EnhancedConfig.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <toml.hpp>

namespace nlm {

struct ValidationRule::Impl {
    // Custom validator for complex validation rules
    std::function<bool(const std::any&)> customValidator;
};

ValidationRule::ValidationRule(RuleType t, const std::string& n, const std::any& p,
                               const std::string& err)
    : type(t), name(n), parameter(p), errorMessage(err), impl(std::make_unique<Impl>()) {
    setupValidator();
}

void ValidationRule::setupValidator() {
    switch (type) {
        case RuleType::Range:
            if (std::holds_alternative<std::pair<double, double>>(parameter)) {
                auto range = std::any_cast<std::pair<double, double>>(parameter);
                impl->customValidator = [range](const std::any& value) {
                    try {
                        double val = std::any_cast<double>(value);
                        return val >= range.first && val <= range.second;
                    } catch (...) {
                        return false;
                    }
                };
            }
            break;
            
        case RuleType::Enum:
            if (std::holds_alternative<std::vector<std::string>>(parameter)) {
                auto enumValues = std::any_cast<std::vector<std::string>>(parameter);
                impl->customValidator = [enumValues](const std::any& value) {
                    try {
                        std::string val = std::any_cast<std::string>(value);
                        return std::find(enumValues.begin(), enumValues.end(), val) != enumValues.end();
                    } catch (...) {
                        return false;
                    }
                };
            }
            break;
            
        case RuleType::Pattern:
            if (std::holds_alternative<std::string>(parameter)) {
                std::string pattern = std::any_cast<std::string>(parameter);
                impl->customValidator = [pattern](const std::any& value) {
                    try {
                        std::string val = std::any_cast<std::string>(value);
                        return std::regex_search(val, std::regex(pattern));
                    } catch (...) {
                        return false;
                    }
                };
            }
            break;
            
        case RuleType::Length:
            if (std::holds_alternative<std::pair<size_t, size_t>>(parameter)) {
                auto lengthRange = std::any_cast<std::pair<size_t, size_t>>(parameter);
                impl->customValidator = [lengthRange](const std::any& value) {
                    try {
                        if (std::holds_alternative<std::string>(value)) {
                            std::string val = std::any_cast<std::string>(value);
                            return val.size() >= lengthRange.first && val.size() <= lengthRange.second;
                        } else if (std::holds_alternative<std::vector<std::any>>(value)) {
                            std::vector<std::any> val = std::any_cast<std::vector<std::any>>(value);
                            return val.size() >= lengthRange.first && val.size() <= lengthRange.second;
                        }
                    } catch (...) {
                        return false;
                    }
                    return false;
                };
            }
            break;
            
        case RuleType::Type:
            if (std::holds_alternative<std::string>(parameter)) {
                std::string typeName = std::any_cast<std::string>(parameter);
                impl->customValidator = [typeName](const std::any& value) {
                    std::string actualType = getValueTypeName(value);
                    return actualType == typeName;
                };
            }
            break;
            
        case RuleType::Custom:
            impl->customValidator = parameter;
            break;
            
        default:
            impl->customValidator = [](const std::any&) { return true; };
            break;
    }
}

std::string ValidationRule::typeToString(RuleType type) const {
    switch (type) {
        case RuleType::Range: return "range";
        case RuleType::Enum: return "enum";
        case RuleType::Pattern: return "pattern";
        case RuleType::Custom: return "custom";
        case RuleType::Required: return "required";
        case RuleType::Exclusive: return "exclusive";
        case RuleType::Dependent: return "dependent";
        case RuleType::Length: return "length";
        case RuleType::Type: return "type";
        case RuleType::Format: return "format";
        default: return "unknown";
    }
}

std::string ValidationRule::toString(const std::any& value) const {
    try {
        if (std::holds_alternative<bool>(value)) {
            return std::any_cast<bool>(value) ? "true" : "false";
        } else if (std::holds_alternative<int>(value)) {
            return std::to_string(std::any_cast<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::to_string(std::any_cast<double>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            return "\"" + std::any_cast<std::string>(value) + "\"";
        } else if (std::holds_alternative<std::vector<std::any>>(value)) {
            std::vector<std::any> vec = std::any_cast<std::vector<std::any>>(value);
            std::string result = "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) result += ", ";
                result += toString(vec[i]);
            }
            result += "]";
            return result;
        }
    } catch (...) {
        return "<unrepresentable>";
    }
    return "<unknown>";
}

ConfigSchema& ConfigSchema::addRule(const std::string& key, const ValidationRule& rule) {
    validationRules[key] = rule;
    return *this;
}

ConfigSchema& ConfigSchema::addDefault(const std::string& key, const std::any& value) {
    defaultValues[key] = value;
    return *this;
}

ConfigSchema& ConfigSchema::addRequired(const std::string& key) {
    requiredKeys.push_back(key);
    return *this;
}

void ConfigSchema::validate(const std::vector<ConfigEntry>& config, ConfigSource source) const {
    // Check required keys
    for (const auto& requiredKey : requiredKeys) {
        bool found = false;
        for (const auto& entry : config) {
            if (entry.key == requiredKey) {
                found = true;
                break;
            }
        }
        if (!found) {
            throw ConfigError("Required key not found: " + requiredKey, 
                             ConfigError::ErrorType::MissingRequired, 
                             requiredKey, "");
        }
    }
    
    // Check unknown keys
    if (!allowUnknownKeys) {
        for (const auto& entry : config) {
            if (validationRules.find(entry.key) == validationRules.end() &&
                defaultValues.find(entry.key) == defaultValues.end()) {
                throw ConfigError("Unknown key in schema: " + entry.key,
                                 ConfigError::ErrorType::SchemaValidation,
                                 entry.key, "");
            }
        }
    }
    
    // Validate each key
    for (const auto& entry : config) {
        auto ruleIt = validationRules.find(entry.key);
        if (ruleIt != validationRules.end()) {
            if (!ruleIt->second.validate(entry.value)) {
                throw ConfigError(ruleIt->second.getErrorMessage(entry.value),
                                 ConfigError::ErrorType::SchemaValidation,
                                 entry.key, "");
            }
        }
    }
}

std::vector<ConfigEntry> ConfigSchema::applyDefaults(const std::vector<ConfigEntry>& config) const {
    std::vector<ConfigEntry> result = config;
    
    // Add missing default values
    for (const auto& defaultPair : defaultValues) {
        bool found = false;
        for (const auto& entry : result) {
            if (entry.key == defaultPair.first) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            ConfigEntry newEntry;
            newEntry.key = defaultPair.first;
            newEntry.value = defaultPair.second;
            newEntry.source = ConfigSource::Default;
            newEntry.description = "Default value for " + defaultPair.first;
            newEntry.category = "Default";
            newEntry.timestamp = std::chrono::system_clock::now();
            
            result.push_back(newEntry);
        }
    }
    
    return result;
}

std::vector<std::string> ConfigSchema::getValidationErrors(
    const std::vector<ConfigEntry>& config, ConfigSource source) const {
    
    std::vector<std::string> errors;
    
    // Check required keys
    for (const auto& requiredKey : requiredKeys) {
        bool found = false;
        for (const auto& entry : config) {
            if (entry.key == requiredKey) {
                found = true;
                break;
            }
        }
        if (!found) {
            errors.push_back("Required key not found: " + requiredKey);
        }
    }
    
    // Check unknown keys
    if (!allowUnknownKeys) {
        for (const auto& entry : config) {
            if (validationRules.find(entry.key) == validationRules.end() &&
                defaultValues.find(entry.key) == defaultValues.end()) {
                errors.push_back("Unknown key in schema: " + entry.key);
            }
        }
    }
    
    // Validate each key
    for (const auto& entry : config) {
        auto ruleIt = validationRules.find(entry.key);
        if (ruleIt != validationRules.end()) {
            if (!ruleIt->second.validate(entry.value)) {
                errors.push_back(ruleIt->second.getErrorMessage(entry.value));
            }
        }
    }
    
    return errors;
}

} // namespace nlm
