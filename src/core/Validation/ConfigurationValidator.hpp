#pragma once

#include <string>
#include <map>
#include <functional>
#include <stdexcept>
#include <vector>
#include <variant>

namespace nlm {

// Configuration validation error types
enum class ValidationErrorType {
    InvalidType,
    OutOfRange,
    InvalidValue,
    MissingRequired,
    DuplicateKey,
    ValidationFailed
};

class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string& message, ValidationErrorType type = ValidationErrorType::ValidationFailed)
        : std::runtime_error(message), errorType(type) {}
    
    ValidationErrorType getErrorType() const { return errorType; }
    
private:
    ValidationErrorType errorType;
};

// Validation rule for configuration values
class ValidationRule {
public:
    using Validator = std::function<bool(const ConfigValue&)>;
    
    ValidationRule(const std::string& key, const std::string& description,
                   Validator validator, bool required = false)
        : key(key), description(description), validator(validator), required(required) {}
    
    const std::string& getKey() const { return key; }
    const std::string& getDescription() const { return description; }
    bool isRequired() const { return required; }
    bool validate(const ConfigValue& value) const { return validator(value); }
    
private:
    std::string key;
    std::string description;
    Validator validator;
    bool required;
};

// Range validation helper templates
struct RangeValidator {
    template<typename T>
    static ValidationRule forRange(const std::string& key, const std::string& description,
                                  T min, T max, bool required = false) {
        return ValidationRule(key, description,
            [min, max](const ConfigValue& value) {
                if constexpr (std::is_integral_v<T>) {
                    return std::holds_alternative<T>(value) && 
                           std::get<T>(value) >= min && std::get<T>(value) <= max;
                } else if constexpr (std::is_floating_point_v<T>) {
                    return std::holds_alternative<T>(value) && 
                           std::get<T>(value) >= min && std::get<T>(value) <= max;
                }
                return false;
            }, required);
    }
};

// String validation helper templates
struct StringValidator {
    static ValidationRule forEnum(const std::string& key, const std::string& description,
                                 const std::vector<std::string>& validValues, bool required = false) {
        return ValidationRule(key, description,
            [validValues](const ConfigValue& value) {
                return std::holds_alternative<std::string>(value) &&
                       std::find(validValues.begin(), validValues.end(), std::get<std::string>(value)) != validValues.end();
            }, required);
    }
    
    static ValidationRule forPattern(const std::string& key, const std::string& description,
                                     const std::string& pattern, bool required = false) {
        return ValidationRule(key, description,
            [pattern](const ConfigValue& value) {
                if (!std::holds_alternative<std::string>(value)) return false;
                const std::string& str = std::get<std::string>(value);
                // Simple pattern matching - can be enhanced later
                return str.find(pattern) != std::string::npos;
            }, required);
    }
};

// Configuration validator class
class ConfigurationValidator {
public:
    ConfigurationValidator() {
        initializeDefaultValidationRules();
    }
    
    // Add validation rule
    void addRule(const ValidationRule& rule) {
        rules[rule.getKey()] = rule;
    }
    
    // Validate configuration
    void validate(const std::map<std::string, ConfigValue>& config) const {
        std::vector<ValidationError> errors;
        
        // Check for required keys
        for (const auto& [key, rule] : rules) {
            if (rule.isRequired()) {
                if (config.find(key) == config.end()) {
                    errors.emplace_back("Required configuration key missing: " + key,
                                      ValidationErrorType::MissingRequired);
                }
            }
        }
        
        // Validate values for all provided keys
        for (const auto& [key, value] : config) {
            auto ruleIt = rules.find(key);
            if (ruleIt != rules.end()) {
                if (!ruleIt->second.validate(value)) {
                    errors.emplace_back("Validation failed for key: " + key +
                                      ". Value: " + valueToString(value) +
                                      ". Description: " + ruleIt->second.getDescription(),
                                      ValidationErrorType::ValidationFailed);
                }
            }
        }
        
        if (!errors.empty()) {
            throwValidationErrors(errors);
        }
    }
    
    // Convert ConfigValue to string for error messages
    static std::string valueToString(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) {
            return std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::to_string(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::to_string(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value) ? "true" : "false";
        } else if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            return vectorToString(std::get<std::vector<int>>(value));
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            return vectorToString(std::get<std::vector<double>>(value));
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            return vectorToString(std::get<std::vector<std::string>>(value));
        }
        return "<unknown>";
    }
    
    // Clear all validation rules
    void clearRules() {
        rules.clear();
    }
    
    // Get all validation rules
    std::vector<ValidationRule> getAllRules() const {
        std::vector<ValidationRule> result;
        for (const auto& pair : rules) {
            result.push_back(pair.second);
        }
        return result;
    }
    
private:
    std::map<std::string, ValidationRule> rules;
    
    // Initialize default validation rules
    void initializeDefaultValidationRules() {
        // Neural network parameters
        addRule(RangeValidator::forRange<int>("neuron_count", 
            "Number of neurons in the brain", 1, 100000, true));
        
        addRule(RangeValidator::forRange<int>("region_count", 
            "Number of brain regions", 1, 100, true));
        
        addRule(RangeValidator::forRange<double>("connection_probability", 
            "Connection probability for random connectivity", 0.0, 1.0, true));
        
        // Plasticity parameters
        addRule(RangeValidator::forRange<double>("plasticity_learning_rate", 
            "Plasticity learning rate", 0.0, 1.0, true));
        
        addRule(RangeValidator::forRange<double>("stdp_ltp_weight", 
            "STDP long-term potentiation weight", 0.0, 1.0, true));
        
        addRule(RangeValidator::forRange<double>("stdp_ltd_weight", 
            "STDP long-term depression weight", 0.0, 1.0, true));
        
        addRule(RangeValidator::forRange<double>("stdp_time_constant", 
            "STDP time constant", 1.0, 1000.0, true));
        
        // Development parameters
        addRule(RangeValidator::forRange<double>("development_synaptogenesis_rate", 
            "Development synaptogenesis rate", 0.0, 1.0, true));
        
        addRule(RangeValidator::forRange<double>("development_pruning_rate", 
            "Development pruning rate", 0.0, 1.0, true));
        
        // Neuromodulation parameters
        addRule(RangeValidator::forRange<double>("dopamine_baseline", 
            "Dopamine baseline level", 0.0, 10.0, true));
        
        // Reward parameters
        addRule(RangeValidator::forRange<double>("reward_discount_factor", 
            "Reward discount factor", 0.0, 1.0, true));
        
        // Environment parameters
        addRule(StringValidator::forEnum("environment_name", 
            "Environment type", {"GridWorld", "Empty", "Maze", "Custom"}, true));
        
        addRule(RangeValidator::forRange<int>("environment_width", 
            "Environment width", 1, 1000, true));
        
        addRule(RangeValidator::forRange<int>("environment_height", 
            "Environment height", 1, 1000, true));
        
        // Simulation parameters
        addRule(RangeValidator::forRange<double>("simulation_timestep", 
            "Simulation timestep in seconds", 0.0001, 1.0, true));
        
        addRule(RangeValidator::forRange<int>("max_simulation_steps", 
            "Maximum simulation steps", 1, 1000000, true));
        
        addRule(RangeValidator::forRange<double>("simulation_time_limit", 
            "Simulation time limit in seconds", 0.0, 1000000.0, true));
        
        // Logging parameters
        addRule(StringValidator::forEnum("log_level", 
            "Logging level", {"OFF", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"}, true));
        
        addRule(StringValidator::forPattern("log_filename", 
            "Log filename pattern", "^[a-zA-Z0-9_\-\.]+$", false));
        
        // Visualization parameters
        addRule(RangeValidator::forRange<double>("visualization_update_rate", 
            "Visualization update rate", 1.0, 1000.0, true));
        
        // Random seed
        addRule(RangeValidator::forRange<int>("random_seed", 
            "Random seed for reproducible experiments", 0, 2147483647, false));
    }
    
    // Convert vector to string helper
    template<typename T>
    static std::string vectorToString(const std::vector<T>& vec) {
        std::string result = "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) result += ", ";
            result += std::to_string(vec[i]);
        }
        result += "]";
        return result;
    }
    
    // Throw validation errors
    void throwValidationErrors(const std::vector<ValidationError>& errors) const {
        std::string message = "Configuration validation failed with " +
                             std::to_string(errors.size()) + " error(s):\n";
        
        for (const auto& error : errors) {
            message += "  - " + error.what() + "\n";
        }
        
        throw ValidationError(message, ValidationErrorType::ValidationFailed);
    }
};

} // namespace nlm