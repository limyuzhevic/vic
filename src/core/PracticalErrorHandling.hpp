// NLM Practical Error Handling and Validation
// 
// A practical error handling system for NLM that integrates with existing code:
// - Error reporting and logging
// - Configuration validation
// - Input validation
// - Error recovery mechanisms
// 
// This provides a practical approach to error handling without requiring
// a complete rewrite of the error system.

#ifndef NLM_PRACTICAL_ERROR_HANDLING_HPP
#define NLM_PRACTICAL_ERROR_HANDLING_HPP

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <map>
#include <algorithm>
#include <chrono>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Info = 0,
    Warning = 1,
    Error = 2,
    Critical = 3,
    Fatal = 4
};

// Error categories
enum class ErrorCategory {
    Configuration = 0,
    Memory = 1,
    Simulation = 2,
    Plasticity = 3,
    Development = 4,
    Neuromodulation = 5,
    Cognition = 6,
    I_O = 7,
    Checkpoint = 8,
    System = 9
};

// Custom exception class
class NLMException : public std::runtime_error {
public:
    ErrorSeverity severity;
    ErrorCategory category;
    std::string component;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> context;
    
    NLMException(ErrorSeverity sev, ErrorCategory cat, const std::string& msg,
                 const std::string& comp = "Unknown")
        : std::runtime_error(msg), severity(sev), category(cat), component(comp) {
        timestamp = std::chrono::system_clock::now();
    }
    
    NLMException& addContext(const std::string& ctx) {
        context.push_back(ctx);
        return *this;
    }
    
    std::string getFullMessage() const {
        std::ostringstream oss;
        oss << "[NLM " << static_cast<int>(category) << "] " << component << ": ";
        oss << what();
        if (!context.empty()) {
            oss << " (Context: ";
            for (size_t i = 0; i < context.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << context[i];
            }
            oss << ")";
        }
        return oss.str();
    }
};

// Simple validation result structure
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        isValid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    
    std::string getSummary() const {
        std::ostringstream oss;
        if (isValid) {
            oss << "Validation PASSED";
            if (!warnings.empty()) {
                oss << " (" << warnings.size() << " warnings)";
            }
        } else {
            oss << "Validation FAILED";
            if (!errors.empty()) {
                oss << " (" << errors.size() << " errors, " << warnings.size() << " warnings)";
            }
        }
        return oss.str();
    }
};

// Configuration validator with practical rules
class ConfigurationValidator {
public:
    struct ParameterRules {
        std::string type; // int, float, string, bool
        std::string range;
        std::string description;
        bool required;
        std::string defaultValue;
    };
    
private:
    std::map<std::string, ParameterRules> parameterRules;
    
public:
    ConfigurationValidator() {
        initializeDefaultRules();
    }
    
    void initializeDefaultRules() {
        // Basic configuration
        parameterRules["random_seed"] = {"int", "0-2147483647", "Random seed for reproducible experiments", true, "42"};
        parameterRules["simulation_timestep"] = {"float", "0.0001-1.0", "Simulation timestep in seconds", true, "0.001"};
        parameterRules["neuron_count"] = {"int", "10-100000", "Number of neurons in the brain", true, "1000"};
        parameterRules["region_count"] = {"int", "1-1000", "Number of brain regions", true, "1"};
        parameterRules["connection_probability"] = {"float", "0.0-1.0", "Connection probability for random connectivity", true, "0.1"};
        
        // Plasticity
        parameterRules["plasticity_learning_rate"] = {"float", "0.0-1.0", "Plasticity learning rate", false, "0.01"};
        parameterRules["stdp_ltp_weight"] = {"float", "0.0-1.0", "STDP LTP weight", false, "0.01"};
        parameterRules["stdp_ltd_weight"] = {"float", "0.0-1.0", "STDP LTD weight", false, "0.012"};
        parameterRules["stdp_time_constant"] = {"float", "1.0-1000.0", "STDP time constant in ms", false, "20.0"};
        
        // Development
        parameterRules["synaptogenesis_rate"] = {"float", "0.000001-0.1", "Synaptogenesis rate", false, "0.001"};
        parameterRules["pruning_rate"] = {"float", "0.00000001-0.01", "Pruning rate", false, "0.00001"};
        
        // Neuromodulation
        parameterRules["dopamine_scale"] = {"float", "0.0-10.0", "Dopamine modulation scale", false, "1.0"};
        parameterRules["reward_discount_factor"] = {"float", "0.0-1.0", "Reward discount factor", false, "0.99"};
        
        // Environment
        parameterRules["environment_width"] = {"int", "1-1000", "Environment width", false, "10"};
        parameterRules["environment_height"] = {"int", "1-1000", "Environment height", false, "10"};
        
        // Logging
        parameterRules["log_level"] = {"string", "DEBUG|INFO|WARNING|ERROR|FATAL", "Log level", true, "INFO"};
        
        // Simulation
        parameterRules["max_simulation_steps"] = {"int", "1-1000000", "Maximum simulation steps", false, "10000"};
        parameterRules["simulation_time_limit"] = {"float", "0.0-1000000.0", "Simulation time limit in seconds", false, "0.0"};
    }
    
    void addParameter(const std::string& key, const ParameterRules& rules) {
        parameterRules[key] = rules;
    }
    
    ValidationResult validate(const std::shared_ptr<Config>& config) const {
        ValidationResult result;
        
        // Validate all registered parameters
        for (const auto& pair : parameterRules) {
            const std::string& key = pair.first;
            const ParameterRules& rules = pair.second;
            
            if (rules.required && !config->has(key)) {
                result.addError("Missing required parameter: " + key);
            }
            
            if (config->has(key)) {
                std::string value;
                if (rules.type == "int") {
                    auto val = config->getOr<int>(key, std::stoi(rules.defaultValue));
                    value = std::to_string(val);
                } else if (rules.type == "float") {
                    auto val = config->getOr<float>(key, std::stof(rules.defaultValue));
                    value = std::to_string(val);
                } else if (rules.type == "string") {
                    auto val = config->getOr<std::string>(key, rules.defaultValue);
                    value = val;
                } else if (rules.type == "bool") {
                    auto val = config->getOr<bool>(key, rules.defaultValue == "true");
                    value = rules.defaultValue == "true" ? "true" : "false";
                }
                
                // Validate type
                if (!validateType(key, value, rules.type)) {
                    result.addError("Invalid type for parameter " + key + ": " + value + " (expected " + rules.type + ")");
                }
                
                // Validate range
                if (!rules.range.empty() && !validateRange(key, value, rules.range)) {
                    result.addError("Parameter " + key + " value " + value + " outside range " + rules.range);
                }
                
                // Validate string values
                if (rules.type == "string") {
                    if (!validateString(key, value, rules.range)) {
                        result.addError("Invalid value for string parameter " + key + ": " + value);
                    }
                }
            }
        }
        
        // Validate parameter consistency
        validateParameterConsistency(config, result);
        
        return result;
    }
    
private:
    bool validateType(const std::string& key, const std::string& value, const std::string& type) const {
        if (type == "int") {
            return std::all_of(value.begin(), value.end(), ::isdigit) ||
                   (value[0] == '-' && std::all_of(value.begin() + 1, value.end(), ::isdigit));
        } else if (type == "float") {
            try {
                std::stod(value);
                return true;
            } catch (...) {
                return false;
            }
        } else if (type == "bool") {
            return value == "true" || value == "false";
        }
        return !value.empty();
    }
    
    bool validateRange(const std::string& key, const std::string& value, const std::string& range) const {
        if (range.empty()) return true;
        
        if (range.find('-') != std::string::npos) {
            // Numeric range: min-max
            size_t dashPos = range.find('-');
            std::string minStr = range.substr(0, dashPos);
            std::string maxStr = range.substr(dashPos + 1);
            
            double valueNum = std::stod(value);
            double min = std::stod(minStr);
            double max = std::stod(maxStr);
            
            return valueNum >= min && valueNum <= max;
        }
        return true;
    }
    
    bool validateString(const std::string& key, const std::string& value, const std::string& allowedValues) const {
        if (allowedValues.empty()) return true;
        
        if (allowedValues.find('|') != std::string::npos) {
            // Check if value is in allowed list
            std::string pattern = "|" + value + "|";
            return allowedValues.find(pattern) != std::string::npos;
        }
        return true;
    }
    
    void validateParameterConsistency(const std::shared_ptr<Config>& config, ValidationResult& result) const {
        // Check neuron_count >= region_count
        if (config->has("neuron_count") && config->has("region_count")) {
            auto neurons = config->getOr<size_t>("neuron_count", 0);
            auto regions = config->getOr<size_t>("region_count", 1);
            if (neurons < regions) {
                result.addWarning("neuron_count (" + std::to_string(neurons) + ") is less than region_count (" + std::to_string(regions) + ")");
            }
        }
        
        // Check connection_probability in valid range
        if (config->has("connection_probability")) {
            auto prob = config->getOr<float>("connection_probability", 0.0f);
            if (prob < 0.0f || prob > 1.0f) {
                result.addError("connection_probability (" + std::to_string(prob) + ") must be between 0.0 and 1.0");
            }
        }
    }
};

// Macro for practical error handling
#define NLM_THROW_ERROR(severity, category, component, message) \
    do { \
        throw nlm::NLMException(severity, category, message, component); \
    } while (0)

#define NLM_WARN(category, component, message) \
    do { \
        std::cerr << "[WARNING] " << static_cast<int>(category) << " " << component << ": " << message << std::endl; \
    } while (0)

#define NLM_INFO(category, component, message) \
    do { \
        std::cout << "[INFO] " << static_cast<int>(category) << " " << component << ": " << message << std::endl; \
    } while (0)

// Macro for configuration validation
#define NLM_VALIDATE_CONFIG(config, validator) \
    do { \
        nlm::ConfigurationValidator validator; \
        auto validationResult = validator.validate(config); \
        if (validationResult.hasErrors()) { \
            std::cerr << "Configuration validation failed:" << std::endl; \
            for (const auto& error : validationResult.errors) { \
                std::cerr << "  ERROR: " << error << std::endl; \
            } \
            NLM_THROW_ERROR(nlm::ErrorSeverity::Error, nlm::ErrorCategory::Configuration, \
                           "ConfigurationValidator", "Configuration validation failed"); \
        } \
        if (validationResult.hasWarnings()) { \
            for (const auto& warning : validationResult.warnings) { \
                std::cerr << "  WARNING: " << warning << std::endl; \
            } \
        } \
    } while (0)

} // namespace nlm

#endif // NLM_PRACTICAL_ERROR_HANDLING_HPP