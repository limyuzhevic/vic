#pragma once

#include "Config.hpp"
#include <unordered_map>
#include <functional>
#include <string>
#include <vector>

namespace nlm {

class ConfigValidator {
public:
    using ValidationRule = std::function<bool(const ConfigValue&, const std::string& key)>;
    
    struct ValidationResult {
        bool valid;
        std::string errorMessage;
    };
    
    ConfigValidator() {
        // Register built-in validation rules
        registerRule("random_seed", [](const ConfigValue&, const std::string&) {
            return true;  // Int - accept any value
        });
        
        registerRule("simulation_timestep", [](const ConfigValue&, const std::string&) {
            return true;  // double - accept any value
        });
        
        registerRule("neuron_count", [](const ConfigValue& value, const std::string&) {
            try {
                return std::get<int64_t>(value) > 0;
            } catch (...) {
                return false;
            }
        });
        
        registerRule("region_count", [](const ConfigValue& value, const std::string&) {
            try {
                return std::get<int64_t>(value) > 0 && std::get<int64_t>(value) <= 10;
            } catch (...) {
                return false;
            }
        });
        
        registerRule("connection_probability", [](const ConfigValue& value, const std::string&) {
            try {
                float prob = std::get<float>(value);
                return prob >= 0.0f && prob <= 1.0f;
            } catch (...) {
                return false;
            }
        });
        
        registerRule("stdp_ltp_weight", [](const ConfigValue&, const std::string&) {
            return true;  // float - accept any
        });
        
        registerRule("stdp_ltd_weight", [](const ConfigValue&, const std::string&) {
            return true;  // float - accept any
        });
        
        registerRule("synaptogenesis_rate", [](const ConfigValue&, const std::string&) {
            return true;  // float - accept any
        });
        
        registerRule("pruning_rate", [](const ConfigValue&, const std::string&) {
            return true;  // float - accept any
        });
        
        registerRule("reward_discount_factor", [](const ConfigValue& value, const std::string&) {
            try {
                float factor = std::get<float>(value);
                return factor >= 0.0f && factor <= 1.0f;
            } catch (...) {
                return false;
            }
        });
        
        registerRule("environment_width", [](const ConfigValue& value, const std::string&) {
            try {
                return std::get<int64_t>(value) > 0 && std::get<int64_t>(value) <= 100;
            } catch (...) {
                return false;
            }
        });
        
        registerRule("environment_height", [](const ConfigValue& value, const std::string&) {
            try {
                return std::get<int64_t>(value) > 0 && std::get<int64_t>(value) <= 100;
            } catch (...) {
                return false;
            }
        });
        
        registerRule("visualization_update_rate", [](const ConfigValue& value, const std::string&) {
            try {
                float rate = std::get<float>(value);
                return rate > 0.0f && rate <= 1000.0f;
            } catch (...) {
                return false;
            }
        });
    }
    
    void registerRule(const std::string& key, ValidationRule rule) {
        validationRules_[key] = rule;
    }
    
    ValidationResult validate(const Config& config) const {
        ValidationResult result{true, ""};
        
        for (const auto& entry : config.getEntries()) {
            const auto& key = entry.key;
            const auto& value = entry.value;
            
            auto it = validationRules_.find(key);
            if (it != validationRules_.end()) {
                if (!it->second(value, key)) {
                    result.valid = false;
                    result.errorMessage = "Invalid value for configuration key '" + key + "': " + toString(value);
                    break;
                }
            }
        }
        
        return result;
    }
    
    std::string toString(const ConfigValue& value) const {
        std::ostringstream oss;
        std::visit([&oss](const auto& val) {
            oss << val;
        }, value);
        return oss.str();
    }
    
    std::vector<std::string> getRequiredKeys() const {
        // Returns empty vector for now - can be extended with required key tracking
        return std::vector<std::string>();
    }
    
    ValidationResult validateRequired(const Config& config) const {
        ValidationResult result{true, ""};
        
        std::vector<std::string> required = getRequiredKeys();
        for (const auto& key : required) {
            if (!config.has(key)) {
                result.valid = false;
                result.errorMessage = "Missing required configuration key: '" + key + "'";
                break;
            }
        }
        
        return result;
    }

private:
    std::unordered_map<std::string, ValidationRule> validationRules_;
};

} // namespace nlm
