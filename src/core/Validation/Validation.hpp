#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <optional>
#include <utility>

namespace nlm {

// Validation error types
enum class ValidationSeverity {
    INFO = 0,
    WARNING = 1,
    ERROR = 2
};

struct ValidationError {
    std::string key;
    std::string message;
    ValidationSeverity severity;
    std::string expected;
    std::string actual;
    
    ValidationError(const std::string& k, const std::string& msg, 
                   ValidationSeverity sev = ValidationSeverity::ERROR,
                   const std::string& expected = "",
                   const std::string& actual = "")
        : key(k), message(msg), severity(sev), expected(expected), actual(actual) {}
    
    bool operator<(const ValidationError& other) const {
        return key < other.key;
    }
};

// Base validation rule interface
class ValidationRule {
public:
    virtual ~ValidationRule() = default;
    virtual ValidationSeverity validate(const std::string& key, 
                                      const ConfigValue& value,
                                      const Config& config) const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
};

// Type validation rule
class TypeValidationRule : public ValidationRule {
    std::string expectedType;
    
public:
    explicit TypeValidationRule(const std::string& type) : expectedType(type) {}
    
    ValidationSeverity validate(const std::string& key, 
                              const ConfigValue& value,
                              const Config& config) const override {
        std::string actualType;
        
        if (std::holds_alternative<int>(value)) actualType = "int";
        else if (std::holds_alternative<int64_t>(value)) actualType = "int64_t";
        else if (std::holds_alternative<double>(value)) actualType = "double";
        else if (std::holds_alternative<bool>(value)) actualType = "bool";
        else if (std::holds_alternative<std::string>(value)) actualType = "string";
        else if (std::holds_alternative<std::vector<int>>(value)) actualType = "std::vector<int>";
        else if (std::holds_alternative<std::vector<double>>(value)) actualType = "std::vector<double>";
        else if (std::holds_alternative<std::vector<std::string>>(value)) actualType = "std::vector<std::string>";
        else actualType = "unknown";
        
        if (actualType == expectedType) {
            return ValidationSeverity::INFO;
        }
        return ValidationSeverity::ERROR;
    }
    
    std::string getName() const override { return "TypeValidation"; }
    std::string getDescription() const override { 
        return "Validates that values match expected types: " + expectedType; 
    }
};

// Range validation rule
class RangeValidationRule : public ValidationRule {
    double min;
    double max;
    std::string type;
    
public:
    RangeValidationRule(double min, double max, const std::string& type = "double")
        : min(min), max(max), type(type) {}
    
    ValidationSeverity validate(const std::string& key, 
                              const ConfigValue& value,
                              const Config& config) const override {
        try {
            double val = 0.0;
            if (type == "int" && std::holds_alternative<int>(value)) {
                val = static_cast<double>(std::get<int>(value));
            } else if (type == "int64_t" && std::holds_alternative<int64_t>(value)) {
                val = static_cast<double>(std::get<int64_t>(value));
            } else if (type == "double" && std::holds_alternative<double>(value)) {
                val = std::get<double>(value);
            } else {
                return ValidationSeverity::INFO;
            }
            
            if (val < min || val > max) {
                return ValidationSeverity::ERROR;
            }
            
            if (val < min * 1.1 || val > max * 0.9) {
                return ValidationSeverity::WARNING;
            }
            
            return ValidationSeverity::INFO;
            
        } catch (const std::bad_variant_access&) {
            return ValidationSeverity::INFO;
        }
    }
    
    std::string getName() const override { return "RangeValidation"; }
    std::string getDescription() const override { 
        return "Validates numeric values are within range [" + 
               std::to_string(min) + ", " + std::to_string(max) + "]"; 
    }
};

// Custom validation rule
class CustomValidationRule : public ValidationRule {
    std::function<ValidationSeverity(const std::string&, const ConfigValue&, const Config&)> validator;
    std::string name;
    std::string description;
    
public:
    CustomValidationRule(
        const std::function<ValidationSeverity(const std::string&, const ConfigValue&, const Config&)>& func,
        const std::string& ruleName,
        const std::string& ruleDescription)
        : validator(func), name(ruleName), description(ruleDescription) {}
    
    ValidationSeverity validate(const std::string& key, 
                              const ConfigValue& value,
                              const Config& config) const override {
        return validator(key, value, config);
    }
    
    std::string getName() const override { return name; }
    std::string getDescription() const override { return description; }
};

// Configuration metadata for validation
struct ConfigMetadata {
    std::string key;
    std::string description;
    std::string type;  // "int", "int64_t", "double", "bool", "string", "vector"
    std::optional<std::pair<double, double>> range;
    std::optional<std::string> validationRule;
    std::vector<std::string> dependsOn;
    bool required;
    std::string defaultValue;
    
    ConfigMetadata(const std::string& k, const std::string& desc = "", 
                   const std::string& t = "", bool req = false)
        : key(k), description(desc), type(t), required(req), dependsOn(), 
          defaultValue("") {}
};

// Validation registry
class ValidationRegistry {
private:
    std::vector<std::unique_ptr<ValidationRule>> rules;
    std::unordered_map<std::string, std::vector<std::string>> 
        configTypeMetadata;
    std::unordered_map<std::string, std::pair<double, double>> 
        configRanges;
    std::unordered_map<std::string, std::vector<std::string>> 
        configDependencies;
    std::unordered_map<std::string, ConfigMetadata> configMetadata;
    
public:
    // Registration methods
    template<typename RuleType, typename... Args>
    void registerRule(Args&&... args) {
        rules.push_back(std::make_unique<RuleType>(std::forward<Args>(args)...));
    }
    
    void registerConfigType(const std::string& key, 
                          const std::string& typeName) {
        configTypeMetadata[key] = typeName;
    }
    
    void registerConfigRange(const std::string& key,
                           double min, double max,
                           const std::string& type = "double") {
        configRanges[key] = {min, max};
        if (!configTypeMetadata.count(key)) {
            configTypeMetadata[key] = type;
        }
    }
    
    void registerConfigMetadata(const std::string& key, const ConfigMetadata& metadata) {
        configMetadata[key] = metadata;
    }
    
    // Validation execution
    std::vector<ValidationError> validate(const Config& config, 
                                        ValidationSeverity minSeverity = ValidationSeverity::ERROR) const;
    
    std::vector<ValidationError> validateAll(const Config& config) const {
        return validate(config, ValidationSeverity::ERROR);
    }
    
    std::vector<ValidationError> validateWithWarnings(const Config& config) const {
        return validate(config, ValidationSeverity::WARNING);
    }
    
    // Rule management
    void clear() { rules.clear(); }
    size_t ruleCount() const { return rules.size(); }
    
    // Configuration metadata access
    const ConfigMetadata* getMetadata(const std::string& key) const {
        auto it = configMetadata.find(key);
        return it != configMetadata.end() ? &it->second : nullptr;
    }
    
    // Validation rule type registration helpers
    template<typename T>
    void addBuiltinTypeValidation() {
        registerRule<TypeValidationRule>("");
    }
    
    void addNumericRangeValidation() {
        registerRule<RangeValidationRule>(0.0, 1.0, "double");
    }
    
    // Built-in rule registration
    void addNeuralSimulationValidations() {
        // Learning rate validation (0.0 to 1.0)
        registerConfigRange("plasticity_learning_rate", 0.0, 1.0, "double");
        registerConfigType("plasticity_learning_rate", "double");
        registerConfigMetadata("plasticity_learning_rate", {
            "plasticity_learning_rate", 
            "Learning rate for plasticity rules (0.0 = no learning, 1.0 = maximum)",
            "double", {0.0, 1.0}, std::nullopt, {}, false, "0.01"
        });
        
        // Connection probability (0.0 to 1.0)
        registerConfigRange("connection_probability", 0.0, 1.0, "double");
        registerConfigMetadata("connection_probability", {
            "connection_probability",
            "Probability of synaptic connections between neurons",
            "double", {0.0, 1.0}, std::nullopt, {}, false, "0.1"
        });
        
        // Neuron count (reasonable bounds)
        registerConfigRange("neuron_count", 1, 100000, "int");
        registerConfigMetadata("neuron_count", {
            "neuron_count",
            "Number of neurons in the brain",
            "int", {1, 100000}, std::nullopt, {}, false, "500"
        });
        
        // Time constant (positive values)
        registerConfigRange("stdp_time_constant", 0.0, 1000.0, "double");
        registerConfigMetadata("stdp_time_constant", {
            "stdp_time_constant",
            "STDP time constant (ms) for spike-timing dependent plasticity",
            "double", {0.0, 1000.0}, std::nullopt, {}, false, "20.0"
        });
        
        // Reward discount factor (0.0 to 1.0)
        registerConfigRange("reward_discount_factor", 0.0, 1.0, "double");
        registerConfigMetadata("reward_discount_factor", {
            "reward_discount_factor",
            "Discount factor for reward prediction (0.0 = immediate, 1.0 = no discount)",
            "double", {0.0, 1.0}, std::nullopt, {}, false, "0.99"
        });
        
        // Environment sizes (positive)
        registerConfigRange("environment_width", 1, 1000, "int");
        registerConfigRange("environment_height", 1, 1000, "int");
        
        // Synaptogenesis rate (small positive)
        registerConfigRange("synaptogenesis_rate", 0.0, 0.01, "double");
        registerConfigMetadata("synaptogenesis_rate", {
            "synaptogenesis_rate",
            "Rate of new synapse formation during development",
            "double", {0.0, 0.01}, std::nullopt, {}, false, "0.001"
        });
    }
};

} // namespace nlm
