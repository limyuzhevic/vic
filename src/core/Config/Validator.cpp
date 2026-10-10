#include "core/Config/Validator.hpp"
#include <algorithm>
#include <sstream>

namespace nlm {

struct ConfigValidator::Impl {
    std::unordered_map<std::string, ValidationRule> validationRules;
    std::unordered_map<std::string, std::string> categories;
    
    Impl() {
        // Initialize default categories
        categories["brain"] = "brain.";
        categories["plasticity"] = "plasticity.";
        categories["neuromodulation"] = "neuromodulation.";
        categories["memory"] = "memory.";
        categories["prediction"] = "prediction.";
        categories["cognition"] = "cognition.";
        categories["development"] = "development.";
        categories["world"] = "world.";
        categories["agent"] = "agent.";
    }
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}

ConfigValidator::~ConfigValidator() = default;

std::vector<ConfigValidator::ValidationError> ConfigValidator::validate(const Config& config) const {
    std::vector<ValidationError> errors;
    
    try {
        for (const auto& entry : pImpl->validationRules) {
            const std::string& key = entry.first;
            const ValidationRule& rule = entry.second;
            
            // Check if config has this key
            if (config.has(key)) {
                const ConfigValue& value = getConfigValueFromEntry(config, key);
                std::string error = rule(key, value);
                if (!error.empty()) {
                    errors.emplace_back(key, error, ConfigSource::Runtime);
                }
            }
        }
        
    } catch (const std::exception& e) {
        // Add validation system error
        errors.emplace_back("validator", "Validation system error: " + std::string(e.what()), ConfigSource::Runtime);
    }
    
    return errors;
}

std::string ConfigValidator::validateKey(const Config& config, const std::string& key) const {
    try {
        // Validate key format
        if (key.empty()) {
            return "Empty key not allowed";
        }
        
        // Find matching validation rules
        for (const auto& entry : pImpl->validationRules) {
            const std::string& pattern = entry.first;
            if (matchesKeyPattern(key, pattern)) {
                const ValidationRule& rule = entry.second;
                if (config.has(key)) {
                    const ConfigValue& value = getConfigValueFromEntry(config, key);
                    return rule(key, value);
                }
            }
        }
        
        // No validation rule found for this key
        return ""; // Valid (no rule means no constraints)
        
    } catch (const std::exception& e) {
        return "Validation error: " + std::string(e.what());
    }
}

void ConfigValidator::addValidationRule(const std::string& key, const ValidationRule& rule) {
    try {
        if (!key.empty() && rule) {
            pImpl->validationRules[key] = rule;
        }
    } catch (const std::exception& e) {
        // Silently ignore errors
    }
}

void ConfigValidator::removeValidationRule(const std::string& key) {
    try {
        pImpl->validationRules.erase(key);
    } catch (const std::exception& e) {
        // Silently ignore errors
    }
}

bool ConfigValidator::isComplete(const Config& config) const {
    try {
        // Required keys for basic NLM operation
        const std::vector<std::string> requiredKeys = {
            "brain.neuron_count",
            "brain.region_count",
            "brain.connection_probability",
            "plasticity.stdp.enable",
            "plasticity.hebbian.enable",
            "memory.working_memory.enabled",
            "memory.episodic_memory.enabled",
            "cognition.attention.enabled",
            "cognition.planning.enabled",
            "development.enabled"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                return false;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::string> ConfigValidator::getRequiredKeys() const {
    return {
        "brain.neuron_count",
        "brain.region_count", 
        "brain.connection_probability",
        "plasticity.stdp.enable",
        "plasticity.hebbian.enable",
        "memory.working_memory.enabled",
        "memory.episodic_memory.enabled",
        "cognition.attention.enabled",
        "cognition.planning.enabled",
        "development.enabled"
    };
}

std::vector<ConfigValidator::ValidationError> ConfigValidator::getErrorsByCategory(const Config& config, const std::string& category) const {
    std::vector<ValidationError> errors;
    
    try {
        auto it = pImpl->categories.find(category);
        if (it == pImpl->categories.end()) {
            return errors; // Empty if category not found
        }
        
        const std::string& prefix = it->second;
        
        for (const auto& entry : pImpl->validationRules) {
            const std::string& key = entry.first;
            if (startsWith(key, prefix)) {
                if (config.has(key)) {
                    const ConfigValue& value = getConfigValueFromEntry(config, key);
                    std::string error = entry.second(key, value);
                    if (!error.empty()) {
                        errors.emplace_back(key, error, ConfigSource::Runtime);
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        // Silently ignore errors
    }
    
    return errors;
}

std::shared_ptr<ConfigValidator> ConfigValidator::createDefaultValidator() {
    auto validator = std::make_shared<ConfigValidator>();
    
    // Add standard validation rules
    validator->addValidationRule("brain.neuron_count", [](const std::string&, const ConfigValue& value) {
        if (!isNumeric(value)) return "Neuron count must be numeric";
        double count = extractNumericValue(value);
        if (count < 1.0 || count > 1000000.0) return "Neuron count must be between 1 and 1,000,000";
        if (std::floor(count) != count) return "Neuron count must be an integer";
        return "";
    });
    
    validator->addValidationRule("brain.region_count", [](const std::string&, const ConfigValue& value) {
        if (!isNumeric(value)) return "Region count must be numeric";
        double count = extractNumericValue(value);
        if (count < 1.0 || count > 100.0) return "Region count must be between 1 and 100";
        if (std::floor(count) != count) return "Region count must be an integer";
        return "";
    });
    
    validator->addValidationRule("brain.connection_probability", [](const std::string&, const ConfigValue& value) {
        if (!isNumeric(value)) return "Connection probability must be numeric";
        double prob = extractNumericValue(value);
        if (prob < 0.0 || prob > 1.0) return "Connection probability must be between 0.0 and 1.0";
        return "";
    });
    
    validator->addValidationRule("plasticity.stdp.learning_rate", [](const std::string&, const ConfigValue& value) {
        if (!isNumeric(value)) return "STDP learning rate must be numeric";
        double rate = extractNumericValue(value);
        if (rate < 0.0 || rate > 1.0) return "STDP learning rate must be between 0.0 and 1.0";
        return "";
    });
    
    validator->addValidationRule("memory.working_memory.capacity", [](const std::string&, const ConfigValue& value) {
        if (!isNumeric(value)) return "Working memory capacity must be numeric";
        double cap = extractNumericValue(value);
        if (cap < 0.0 || cap > 1000000.0) return "Working memory capacity must be between 0 and 1,000,000";
        if (std::floor(cap) != cap) return "Working memory capacity must be an integer";
        return "";
    });
    
    return validator;
}

// === INTERNAL HELPERS ===

bool ConfigValidator::isNumeric(const ConfigValue& value) {
    return std::holds_alternative<int>(value) ||
           std::holds_alternative<int64_t>(value) ||
           std::holds_alternative<double>(value) ||
           std::holds_alternative<float>(value);
}

bool ConfigValidator::isString(const ConfigValue& value) {
    return std::holds_alternative<std::string>(value);
}

bool ConfigValidator::isBoolean(const ConfigValue& value) {
    return std::holds_alternative<bool>(value);
}

bool ConfigValidator::isArray(const ConfigValue& value) {
    return std::holds_alternative<std::vector<int>>(value) ||
           std::holds_alternative<std::vector<double>>(value) ||
           std::holds_alternative<std::vector<std::string>>(value);
}

double ConfigValidator::extractNumericValue(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return static_cast<double>(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<float>(value)) {
        return static_cast<double>(std::get<float>(value));
    }
    return 0.0;
}

std::string ConfigValidator::configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

const ConfigValue& ConfigValidator::getConfigValueFromEntry(const Config& config, const std::string& key) const {
    // This is a helper - in practice we'd need to access the config's internal storage
    // For now, we'll create a const cast workaround
    auto& constConfig = const_cast<Config&>(config);
    
    auto it = std::find_if(constConfig.pImpl->entries.begin(), constConfig.pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it != constConfig.pImpl->entries.end()) {
        return it->value;
    }
    
    // Return a default value if key not found
    static const int defaultInt = 0;
    return defaultInt;
}

bool ConfigValidator::matchesKeyPattern(const std::string& key, const std::string& pattern) {
    // Simple pattern matching - in practice you'd want more sophisticated pattern matching
    if (pattern.empty()) return key.empty();
    
    // Check for wildcard match (e.g., "brain.*" should match "brain.neuron_count")
    if (pattern.back() == '*') {
        std::string prefix = pattern.substr(0, pattern.size() - 1);
        return startsWith(key, prefix);
    }
    
    return key == pattern;
}

bool ConfigValidator::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

} // namespace nlm