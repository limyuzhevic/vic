#include "ConfigurationValidator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <sstream>
#include <limits>
#include <unordered_map>

namespace nlm {

struct ConfigurationValidator::Impl {
    std::vector<ValidationRule> rules;
    ValidationStats stats;
    std::unordered_map<std::string, ConfigDataType> typeCache;
    
    Impl() {
        stats.totalValidations = 0;
        stats.successfulValidations = 0;
        stats.failedValidations = 0;
    }
};

ConfigurationValidator::ConfigurationValidator()
    : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_DEBUG("ConfigurationValidator initialized");
}

ConfigurationValidator::~ConfigurationValidator() {
    NLM_LOG_DEBUG("ConfigurationValidator destroyed");
}

ConfigurationValidator::ConfigurationValidator(ConfigurationValidator&& other) noexcept
    : pImpl(std::move(other.pImpl)) {
}

ConfigurationValidator& ConfigurationValidator::operator=(ConfigurationValidator&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void ConfigurationValidator::addRule(const ValidationRule& rule) {
    pImpl->rules.push_back(rule);
    pImpl->typeCache[rule.key] = rule.expectedType;
    NLM_LOG_DEBUG("Added validation rule for key: " + rule.key + " (type: " + 
                 std::to_string(static_cast<int>(rule.expectedType)) + ")");
}

void ConfigurationValidator::addRule(std::string key, ConfigDataType type, std::string description, bool required) {
    ValidationRule rule(std::move(key), type, std::move(description), required);
    addRule(rule);
}

bool ConfigurationValidator::validate(const std::shared_ptr<const Config>& config, std::string& errorMessage) const {
    pImpl->stats.totalValidations++;
    
    bool allValid = true;
    errorMessage.clear();
    
    for (const auto& rule : pImpl->rules_) {
        bool ruleValid = true;
        std::string ruleError;
        
        // Get value from config
        std::string error;
        std::optional<std::string> valueOpt;
        
        // Try to get value based on expected type
        bool valueFound = false;
        std::string convertedValue;
        
        if (rule.expectedType == ConfigDataType::String) {
            auto strOpt = config->get<std::string>(rule.key);
            if (strOpt) {
                valueFound = true;
                convertedValue = *strOpt;
            }
        } else if (rule.expectedType == ConfigDataType::Int64) {
            auto intOpt = config->get<int64_t>(rule.key);
            if (intOpt) {
                valueFound = true;
                convertedValue = std::to_string(*intOpt);
            }
        } else if (rule.expectedType == ConfigDataType::Float) {
            auto floatOpt = config->get<float>(rule.key);
            if (floatOpt) {
                valueFound = true;
                convertedValue = std::to_string(*floatOpt);
            }
        } else if (rule.expectedType == ConfigDataType::Double) {
            auto doubleOpt = config->get<double>(rule.key);
            if (doubleOpt) {
                valueFound = true;
                convertedValue = std::to_string(*doubleOpt);
            }
        } else if (rule.expectedType == ConfigDataType::Boolean) {
            auto boolOpt = config->get<bool>(rule.key);
            if (boolOpt) {
                valueFound = true;
                convertedValue = *boolOpt ? "true" : "false";
            }
        }
        
        if (!valueFound) {
            if (rule.required) {
                ruleValid = false;
                ruleError = "Missing required configuration key: '" + rule.key + "'";
            } else {
                // Not required, so skip validation
                continue;
            }
        } else {
            // Validate the found value
            if (!rule.customValidator || rule.customValidator(convertedValue)) {
                if (!rule.allowedValues.empty()) {
                    if (std::find(rule.allowedValues.begin(), rule.allowedValues.end(), convertedValue) == rule.allowedValues.end()) {
                        ruleValid = false;
                        ruleError = "Invalid value for key '" + rule.key + "': expected one of [";
                        for (size_t i = 0; i < rule.allowedValues.size(); ++i) {
                            if (i > 0) ruleError += ", ";
                            ruleError += "'" + rule.allowedValues[i] + "'";
                        }
                        ruleError += "], got '" + convertedValue + "'";
                    }
                }
                
                if (ruleValid && (rule.range.min != std::numeric_limits<double>::lowest() || rule.range.max != std::numeric_limits<double>::max())) {
                    double numericValue;
                    try {
                        numericValue = std::stod(convertedValue);
                        if (numericValue < rule.range.min || numericValue > rule.range.max) {
                            ruleValid = false;
                            ruleError = "Value for key '" + rule.key + "' out of range: [" +
                                       std::to_string(rule.range.min) + ", " + std::to_string(rule.range.max) +
                                       "], got " + std::to_string(numericValue);
                        }
                    } catch (const std::exception&) {
                        ruleValid = false;
                        ruleError = "Invalid numeric value for key '" + rule.key + "': '" + convertedValue + "'";
                    }
                }
                
                if (ruleValid && rule.customValidator) {
                    if (!rule.customValidator(convertedValue)) {
                        ruleValid = false;
                        ruleError = "Custom validation failed for key '" + rule.key + "'";
                    }
                }
            }
        }
        
        if (!ruleValid) {
            allValid = false;
            if (!errorMessage.empty()) errorMessage += "\n";
            errorMessage += "Rule error for key '" + rule.key + "': " + ruleError;
            pImpl->stats.failedValidations++;
        } else if (rule.required) {
            pImpl->stats.successfulValidations++;
        }
    }
    
    // Log validation result
    if (allValid) {
        NLM_LOG_INFO("Configuration validation passed: " + std::to_string(pImpl->stats.totalValidations) + " rules checked");
    } else {
        NLM_LOG_ERROR("Configuration validation failed: " + errorMessage);
    }
    
    return allValid;
}

bool ConfigurationValidator::hasRule(const std::string& key) const {
    return pImpl->typeCache.find(key) != pImpl->typeCache.end();
}

const ValidationRule* ConfigurationValidator::getRule(const std::string& key) const {
    for (const auto& rule : pImpl->rules_) {
        if (rule.key == key) {
            return &rule;
        }
    }
    return nullptr;
}

void ConfigurationValidator::clearRules() {
    pImpl->rules_.clear();
    pImpl->typeCache.clear();
    NLM_LOG_DEBUG("All validation rules cleared");
}

static std::shared_ptr<ConfigurationValidator>& defaultValidatorPtr() {
    static std::shared_ptr<ConfigurationValidator> ptr = std::make_shared<ConfigurationValidator>();
    return ptr;
}

ConfigurationValidator& ConfigurationValidator::getDefaultValidator() {
    return *defaultValidatorPtr();
}

void ConfigurationValidator::setDefaultValidator(const ConfigurationValidator& validator) {
    *defaultValidatorPtr() = validator;
}

// Convenience functions implementation
namespace ConfigValidation {
    ValidationRule RequiredString(const std::string& key, const std::string& description) {
        return ValidationRule(key, ConfigDataType::String, description, true);
    }
    
    ValidationRule RequiredInt(const std::string& key, const std::string& description) {
        return ValidationRule(key, ConfigDataType::Int64, description, true);
    }
    
    ValidationRule RequiredFloat(const std::string& key, const std::string& description) {
        return ValidationRule(key, ConfigDataType::Float, description, true);
    }
    
    ValidationRule RequiredDouble(const std::string& key, const std::string& description) {
        return ValidationRule(key, ConfigDataType::Double, description, true);
    }
    
    ValidationRule RequiredBool(const std::string& key, const std::string& description) {
        return ValidationRule(key, ConfigDataType::Boolean, description, true);
    }
    
    ValidationRule StringWithAllowedValues(const std::string& key, 
                                          const std::vector<std::string>& allowedValues,
                                          const std::string& description) {
        ValidationRule rule(key, ConfigDataType::String, description, true);
        return rule.withAllowedValues(allowedValues);
    }
    
    ValidationRule IntInRange(const std::string& key, int64_t min, int64_t max, 
                            const std::string& description) {
        ValidationRule rule(key, ConfigDataType::Int64, description, true);
        return rule.withRange(min, max);
    }
    
    ValidationRule FloatInRange(const std::string& key, float min, float max, 
                              const std::string& description) {
        ValidationRule rule(key, ConfigDataType::Float, description, true);
        return rule.withRange(min, max);
    }
    
    ValidationRule DoubleInRange(const std::string& key, double min, double max, 
                               const std::string& description) {
        ValidationRule rule(key, ConfigDataType::Double, description, true);
        return rule.withRange(min, max);
    }
    
    ValidationRule CustomValidator(const std::string& key, 
                                   std::function<bool(const std::string&)> validator,
                                   const std::string& description) {
        ValidationRule rule(key, ConfigDataType::String, description, true);
        return rule.withValidator(validator);
    }
}

} // namespace nlm
