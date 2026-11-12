// Configuration Validation and Error Handling System
//
// This header provides enhanced configuration validation and error handling
// for the NLM system, improving robustness and user experience.

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <variant>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <stdexcept>
#include <sstream>

namespace nlm {

// Forward declarations
class ConfigValidator;

// Configuration validation rules
struct ConfigValidationRule {
    std::string key;
    std::string type;  // "int", "float", "bool", "string"
    std::variant<int64_t, float, bool, std::string> defaultValue;
    bool required = false;
    std::string description;
    std::function<bool(std::variant<int64_t, float, bool, std::string>)> validator;
};

// Error codes for NLM errors
enum class NLMErrorCode {
    SUCCESS = 0,
    CONFIG_ERROR = 1,
    BRAIN_INITIALIZATION_ERROR = 2,
    SIMULATION_ERROR = 3,
    MEMORY_ERROR = 4,
    PLASTICITY_ERROR = 5,
    NEUROMODULATION_ERROR = 6,
    FILE_ERROR = 7,
    VALIDATION_ERROR = 8,
    UNKNOWN_ERROR = 9
};

// NLM Error class
class NLMError {
public:
    NLMErrorCode code;
    std::string message;
    std::string context;
    std::chrono::system_clock::time_point timestamp;
    
    NLMError(NLMErrorCode c, const std::string& msg, const std::string& ctx = "")
        : code(c), message(msg), context(ctx), 
          timestamp(std::chrono::system_clock::now()) {}
    
    std::string getFormattedError() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] "
           << "NLM Error " << static_cast<int>(code) << ": " << message;
        if (!context.empty()) {
            ss << " (Context: " << context << ")";
        }
        return ss.str();
    }
};

// Enhanced configuration class with validation
class ValidatedConfig : public Config {
public:
    ValidatedConfig() : Config() {}
    
    // Enhanced set method with validation
    void set(const std::string& key, const std::variant<int64_t, float, bool, std::string>& value, ConfigSource source = ConfigSource::Default) {
        Config::set(key, value, source);
    }
    
    // Convenience overloaded methods for common types
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Default) {
        Config::set(key, value, source);
    }
    
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Default) {
        Config::set(key, value, source);
    }
    
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Default) {
        Config::set(key, value, source);
    }
    
    // Validate and get configuration value
    template<typename T>
    T getValidated(const std::string& key, const T& defaultValue) const {
        T value = Config::getOr(key, defaultValue);
        return value;
    }
    
    // Load configuration with validation
    bool loadFromFile(const std::string& filepath) {
        return Config::loadFromFile(filepath);
    }
};

// Configuration validation manager
class ConfigValidator {
public:
    static ConfigValidator instance;
    
    std::vector<ConfigValidationRule> validationRules;
    std::map<std::string, std::vector<NLMError>> errors;
    std::function<void(const NLMError&)> errorHandler;
    
    ConfigValidator() {
        initializeValidationRules();
    }
    
    void initializeValidationRules() {
        // Brain configuration rules
        validationRules.push_back({
            "brain.neuron_count", "int", static_cast<int64_t>(1000), true,
            "Total number of neurons",
            [](auto val) { 
                int64_t v = std::get<int64_t>(val); 
                return v > 0 && v <= 10000; 
            }
        });
        
        validationRules.push_back({
            "brain.synapse_density", "float", 0.1f, false,
            "Synapse connectivity density",
            [](auto val) { 
                float v = std::get<float>(val); 
                return v >= 0.0f && v <= 1.0f; 
            }
        });
        
        validationRules.push_back({
            "brain.connection_probability", "float", 0.05f, false,
            "Probability of connection between neurons",
            [](auto val) { 
                float v = std::get<float>(val); 
                return v >= 0.0f && v <= 1.0f; 
            }
        });
        
        // Plasticity configuration rules
        validationRules.push_back({
            "plasticity.stdp.enable", "bool", true, false,
            "Enable STDP",
            [](auto val) { 
                bool v = std::get<bool>(val); 
                return v; 
            }
        });
        
        validationRules.push_back({
            "plasticity.stdp.learning_rate", "float", 0.001f, false,
            "STDP learning rate",
            [](auto val) { 
                float v = std::get<float>(val); 
                return v >= 0.0f && v <= 1.0f; 
            }
        });
        
        // Neuromodulation configuration rules
        validationRules.push_back({
            "neuromod.dopamine.scale", "float", 1.0f, false,
            "Dopamine modulation scale",
            [](auto val) { 
                float v = std::get<float>(val); 
                return v >= 0.0f; 
            }
        });
    }
    
    // Validate configuration
    std::vector<NLMError> validate(const std::shared_ptr<Config>& config) {
        std::vector<NLMError> result;
        
        for (const auto& rule : validationRules) {
            if (config->has(rule.key)) {
                std::variant<int64_t, float, bool, std::string> value;
                
                if (rule.type == "int") {
                    value = config->getOr<int64_t>(rule.key, std::get<int64_t>(rule.defaultValue));
                } else if (rule.type == "float") {
                    value = config->getOr<float>(rule.key, std::get<float>(rule.defaultValue));
                } else if (rule.type == "bool") {
                    value = config->getOr<bool>(rule.key, std::get<bool>(rule.defaultValue));
                } else if (rule.type == "string") {
                    // For strings, use empty string default
                    value = "";
                }
                
                if (rule.validator && !rule.validator(value)) {
                    result.push_back(NLMError(
                        NLMErrorCode::VALIDATION_ERROR,
                        "Configuration validation failed for key: " + rule.key,
                        "Value: " + std::to_string(static_cast<int>(value)) + " (type: " + rule.type + ")"
                    ));
                }
            } else if (rule.required) {
                // Key is required but not present
                result.push_back(NLMError(
                    NLMErrorCode::CONFIG_ERROR,
                    "Required configuration key missing: " + rule.key,
                    "Description: " + rule.description
                )));
            }
        }
        
        return result;
    }
    
    // Set global error handler
    void setErrorHandler(std::function<void(const NLMError&)> handler) {
        errorHandler = handler;
    }
    
    // Log errors
    void logErrors(const std::vector<NLMError>& errors) {
        NLM_LOG_INFO("=== Configuration Validation Results ===");
        for (const auto& error : errors) {
            NLM_LOG_ERROR(error.getFormattedError());
            if (errorHandler) {
                errorHandler(error);
            }
        }
    }
}; 

// Global error handler function
void setGlobalErrorHandler(std::function<void(const NLMError&)> handler);

// Convenience function to validate brain configuration
std::vector<NLMError> validateBrainConfig(const std::shared_ptr<Config>& config);

// Initialize validation with default rules
void initializeValidation();

} // namespace nlm