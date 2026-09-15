#include "ConfigValidator.hpp"
#include "Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

struct ConfigValidator::Impl {
    struct Rule {
        std::string key;
        ValidationRule rule;
        std::string message;
        std::variant<std::string, double, int, std::vector<std::string>> value;
        std::function<bool(const ConfigValue&)> validator;
    };
    
    std::vector<Rule> rules;
    std::vector<ValidationError> errors;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> customValidators;
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}

ConfigValidator::~ConfigValidator() = default;

void ConfigValidator::addRule(const std::string& key, ValidationRule rule, 
                             const std::string& message,
                             const std::variant<std::string, double, int, std::vector<std::string>>& value) {
    Impl::Rule ruleData;
    ruleData.key = key;
    ruleData.rule = rule;
    ruleData.message = message;
    ruleData.value = value;
    
    // Create a simple validator based on rule
    switch (rule) {
        case ValidationRule::Required:
            ruleData.validator = [key](const ConfigValue& val) {
                return val != ConfigValue{}; // Check if value is set
            };
            break;
        case ValidationRule::Positive:
            ruleData.validator = [value](const ConfigValue& val) {
                if constexpr (std::is_same_v<std::decay_t<decltype(value)>, int>) {
                    return std::get<int>(value) > 0;
                } else if constexpr (std::is_same_v<std::decay_t<decltype(value)>, double>) {
                    return std::get<double>(value) > 0.0;
                }
                return false;
            };
            break;
        case ValidationRule::NonNegative:
            ruleData.validator = [value](const ConfigValue& val) {
                if constexpr (std::is_same_v<std::decay_t<decltype(value)>, int>) {
                    return std::get<int>(value) >= 0;
                } else if constexpr (std::is_same_v<std::decay_t<decltype(value)>, double>) {
                    return std::get<double>(value) >= 0.0;
                }
                return false;
            };
            break;
        case ValidationRule::Range:
            ruleData.validator = [value](const ConfigValue& val) {
                if constexpr (std::is_same_v<std::decay_t<decltype(value)>, double>) {
                    double min = std::get<double>(value);
                    // Simplified - would need two values for range
                    return min >= 0.0 && min <= 1.0;
                }
                return false;
            };
            break;
        case ValidationRule::Choice:
            ruleData.validator = [value](const ConfigValue& val) {
                if constexpr (std::is_same_v<std::decay_t<decltype(value)>, std::vector<std::string>>) {
                    const auto& choices = std::get<std::vector<std::string>>(value);
                    std::string strVal;
                    std::visit([&](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            strVal = arg;
                        }
                    }, val);
                    return std::find(choices.begin(), choices.end(), strVal) != choices.end();
                }
                return false;
            };
            break;
        case ValidationRule::Pattern:
            ruleData.validator = [value](const ConfigValue& val) {
                if constexpr (std::is_same_v<std::decay_t<decltype(value)>, std::string>) {
                    const std::string& pattern = std::get<std::string>(value);
                    std::string strVal;
                    std::visit([&](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            strVal = arg;
                        }
                    }, val);
                    // Simple pattern check - would use regex in full implementation
                    return pattern.empty() || strVal.find(pattern) != std::string::npos;
                }
                return false;
            };
            break;
        case ValidationRule::Callback:
            // For Callback, validator must be provided separately
            break;
        default:
            ruleData.validator = [](const ConfigValue&) { return true; };
            break;
    }
    
    pImpl->rules.push_back(std::move(ruleData));
}

void ConfigValidator::addValidator(const std::string& key, 
                                   std::function<bool(const ConfigValue&)> validator,
                                   const std::string& message) {
    pImpl->customValidators[key] = validator;
}

std::vector<ValidationError> ConfigValidator::validate(const Config& config) const {
    pImpl->errors.clear();
    
    // Check built-in rules
    for (const auto& rule : pImpl->rules) {
        ValidationError error;
        error.key = rule.key;
        error.rule = rule.rule;
        error.message = rule.message.empty() ? "Validation failed" : rule.message;
        error.expected = "Not specified";
        
        // Get actual value
        std::optional<ConfigValue> val = config.get<ConfigValue>(rule.key);
        if (!val.has_value()) {
            error.actual = "Not present";
            if (rule.rule == ValidationRule::Required) {
                pImpl->errors.push_back(error);
            }
        } else {
            error.actual = "Present";
            
            // Apply validator
            if (rule.validator && !rule.validator(val.value())) {
                pImpl->errors.push_back(error);
            }
        }
    }
    
    // Check custom validators
    for (const auto& [key, validator] : pImpl->customValidators) {
        std::optional<ConfigValue> val = config.get<ConfigValue>(key);
        if (val.has_value() && !validator(val.value())) {
            ValidationError error;
            error.key = key;
            error.rule = ValidationRule::Callback;
            error.message = "Custom validation failed";
            error.expected = "Passes custom validator";
            error.actual = "Failed custom validator";
            pImpl->errors.push_back(error);
        }
    }
    
    return pImpl->errors;
}

bool ConfigValidator::isValid(const Config& config) const {
    return validate(config).empty();
}

std::vector<ValidationError> ConfigValidator::getErrorsForKey(const Config& config, const std::string& key) const {
    std::vector<ValidationError> keyErrors;
    
    // Validate to populate errors first
    validate(config);
    
    for (const auto& error : pImpl->errors) {
        if (error.key == key) {
            keyErrors.push_back(error);
        }
    }
    
    return keyErrors;
}

void ConfigValidator::clear() {
    pImpl->rules.clear();
    pImpl->errors.clear();
    pImpl->customValidators.clear();
}

bool ConfigValidator::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open validation rules file: " + filepath);
        return false;
    }
    
    // Simple line-based parsing for now
    // In a full implementation, this would parse a structured format (JSON/YAML)
    clear();
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        line.erase(0, line.find_first_not_of("# \t"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) continue;
        
        // Parse "key:rule:message:value" format
        size_t colon1 = line.find(':');
        if (colon1 == std::string::npos) continue;
        
        std::string key = line.substr(0, colon1);
        size_t colon2 = line.find(':', colon1 + 1);
        if (colon2 == std::string::npos) continue;
        
        std::string ruleStr = line.substr(colon1 + 1, colon2 - colon1 - 1);
        size_t colon3 = line.find(':', colon2 + 1);
        
        std::string message = "";
        std::string valueStr = "";
        
        if (colon3 != std::string::npos) {
            message = line.substr(colon2 + 1, colon3 - colon2 - 1);
            valueStr = line.substr(colon3 + 1);
        } else {
            message = line.substr(colon2 + 1);
        }
        
        // Map rule string to enum
        ValidationRule rule;
        if (ruleStr == "Required") rule = ValidationRule::Required;
        else if (ruleStr == "Positive") rule = ValidationRule::Positive;
        else if (ruleStr == "NonNegative") rule = ValidationRule::NonNegative;
        else if (ruleStr == "Range") rule = ValidationRule::Range;
        else if (ruleStr == "Choice") rule = ValidationRule::Choice;
        else if (ruleStr == "Pattern") rule = ValidationRule::Pattern;
        else if (ruleStr == "Callback") rule = ValidationRule::Callback;
        else continue;
        
        addRule(key, rule, message, valueStr);
    }
    
    NLM_LOG_INFO("Loaded validation rules from: " + filepath);
    return true;
}

bool ConfigValidator::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot save validation rules to: " + filepath);
        return false;
    }
    
    for (const auto& rule : pImpl->rules) {
        file << rule.key << ":";
        
        // Map enum to string
        std::string ruleStr;
        switch (rule.rule) {
            case ValidationRule::Required: ruleStr = "Required"; break;
            case ValidationRule::Positive: ruleStr = "Positive"; break;
            case ValidationRule::NonNegative: ruleStr = "NonNegative"; break;
            case ValidationRule::Range: ruleStr = "Range"; break;
            case ValidationRule::Choice: ruleStr = "Choice"; break;
            case ValidationRule::Pattern: ruleStr = "Pattern"; break;
            case ValidationRule::Callback: ruleStr = "Callback"; break;
        }
        
        file << ruleStr << ":" << rule.message << ":";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ",";
                    file << arg[i];
                }
            }
        }, rule.value);
        
        file << "\n";
    }
    
    return true;
}

std::string ConfigValidator::summary() const {
    std::ostringstream oss;
    oss << "ConfigValidator: " << pImpl->rules.size() << " validation rules loaded, " 
        << pImpl->customValidators.size() << " custom validators";
    return oss.str();
}

} // namespace nlm