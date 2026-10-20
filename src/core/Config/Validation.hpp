// Configuration Validation System
// File: /workspace/app/src/core/Config/Validation.hpp
// Provides validation infrastructure for NLM configuration

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <memory>
#include <set>
#include <sstream>
#include <numeric>

namespace nlm {

/**
 * Configuration validation exception
 */
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message) 
        : std::runtime_error("Configuration validation error: " + message) {}
};

/**
 * Configuration validation result
 */
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getSummary() const {
        std::stringstream ss;
        if (!valid) {
            ss << "CONFIGURATION INVALID - " << errors.size() << " errors, " << warnings.size() << " warnings";
        } else {
            ss << "Configuration valid - " << warnings.size() << " warnings";
        }
        return ss.str();
    }
};

/**
 * Configuration validator interface
 */
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    
    virtual ValidationResult validate(const std::string& key, const std::string& value) const = 0;
    
    virtual ValidationResult validate(const std::unordered_map<std::string, std::string>& config) const {
        ValidationResult result;
        
        for (const auto& pair : config) {
            ValidationResult keyResult = validate(pair.first, pair.second);
            if (!keyResult.valid) {
                result.errors.insert(result.errors.end(), 
                                   keyResult.errors.begin(), keyResult.errors.end());
                result.warnings.insert(result.warnings.end(),
                                      keyResult.warnings.begin(), keyResult.warnings.end());
            }
            result.warnings.insert(result.warnings.end(),
                                  keyResult.warnings.begin(), keyResult.warnings.end());
        }
        
        return result;
    }
};

/**
 * Default configuration validator
 */
class DefaultConfigValidator : public ConfigValidator {
public:
    ValidationResult validate(const std::string& key, const std::string& value) const override {
        ValidationResult result;
        
        if (key.empty()) {
            result.addError("Empty configuration key");
            return result;
        }
        
        // Validate known keys with specific constraints
        if (key == "random_seed") {
            validateIntegerRange(result, key, value, 0, std::numeric_limits<uint64_t>::max());
        } else if (key == "simulation_timestep") {
            validateFloatRange(result, key, value, 0.0001f, 1.0f);
        } else if (key == "neuron_count") {
            validateIntegerRange(result, key, value, 1, 1000000);
        } else if (key == "region_count") {
            validateIntegerRange(result, key, value, 1, 100);
        } else if (key == "connection_probability") {
            validateFloatRange(result, key, value, 0.0f, 1.0f);
        } else if (key == "stdp_ltp_weight") {
            validateFloatRange(result, key, value, 0.0f, 0.05f);
        } else if (key == "stdp_ltd_weight") {
            validateFloatRange(result, key, value, 0.0f, 0.05f);
        } else if (key == "synaptogenesis_rate") {
            validateFloatRange(result, key, value, 0.0f, 0.0001f);  // Maximum 0.01%
        } else if (key == "pruning_rate") {
            validateFloatRange(result, key, value, 0.0f, 0.00001f);  // Maximum 0.001%
        } else if (key == "development_critical_period_start") {
            validateFloatRange(result, key, value, 0.0f, 1000.0f);
        } else if (key == "development_critical_period_end") {
            validateFloatRange(result, key, value, 0.0f, 2000.0f);
        } else if (key == "development_plasticity_factor") {
            validateFloatRange(result, key, value, 0.1f, 2.0f);
        } else if (key == "dopamine_baseline") {
            validateFloatRange(result, key, value, -1.0f, 1.0f);
        } else if (key == "dopamine_decay_rate") {
            validateFloatRange(result, key, value, 0.0f, 1.0f);
        } else if (key == "curiosity_sensitivity") {
            validateFloatRange(result, key, value, 0.0f, 1.0f);
        } else if (key == "curiosity_decay_rate") {
            validateFloatRange(result, key, value, 0.0f, 1.0f);
        }
        
        return result;
    }
    
protected:
    void validateIntegerRange(ValidationResult& result, const std::string& key, 
                             const std::string& value, int64_t min, int64_t max) const {
        try {
            uint64_t intValue = std::stoull(value);
            if (intValue < min) {
                result.addError("Config " + key + " value " + value + " below minimum " + 
                              std::to_string(min));
            } else if (intValue > max) {
                result.addWarning("Config " + key + " value " + value + " above recommended maximum " + 
                                std::to_string(max));
            }
        } catch (const std::exception&) {
            result.addError("Config " + key + " has invalid integer format: " + value);
        }
    }
    
    void validateFloatRange(ValidationResult& result, const std::string& key,
                           const std::string& value, float min, float max) const {
        try {
            float floatValue = std::stof(value);
            if (floatValue < min) {
                result.addError("Config " + key + " value " + value + " below minimum " + 
                              std::to_string(min));
            } else if (floatValue > max) {
                result.addWarning("Config " + key + " value " + value + " above recommended maximum " + 
                                std::to_string(max));
            }
        } catch (const std::exception&) {
            result.addError("Config " + key + " has invalid float format: " + value);
        }
    }
};

/**
 * Dependent configuration validator
 * Validates that related settings are consistent with each other
 */
class DependentConfigValidator : public ConfigValidator {
public:
    ValidationResult validate(const std::string& key, const std::string& value) const override {
        ValidationResult result;
        
        // Temporarily set the value for dependency validation
        tempConfig_[key] = value;
        
        if (key == "synaptogenesis_rate") {
            validateRateDependencies(result);
        } else if (key == "pruning_rate") {
            validateRateDependencies(result);
        } else if (key == "development_critical_period_start") {
            validateDevelopmentTimeline(result);
        } else if (key == "development_critical_period_end") {
            validateDevelopmentTimeline(result);
        }
        
        // Clear temporary config
        tempConfig_.erase(key);
        
        return result;
    }
    
private:
    void validateRateDependencies(ValidationResult& result) const {
        auto synRateIt = tempConfig_.find("synaptogenesis_rate");
        auto pruneRateIt = tempConfig_.find("pruning_rate");
        
        if (synRateIt != tempConfig_.end() && pruneRateIt != tempConfig_.end()) {
            try {
                float synRate = std::stof(synRateIt->second);
                float pruneRate = std::stof(pruneRateIt->second);
                
                // Synaptogenesis rate should not vastly exceed pruning rate
                if (synRate > pruneRate * 1000) {
                    result.addWarning("Synaptogenesis rate much higher than pruning rate - may cause uncontrolled growth");
                }
                
                // Ensure both rates are within reasonable bounds
                if (synRate > 0.0001f) {
                    result.addWarning("Synaptogenesis rate above 0.01% - may impact performance");
                }
                
                if (pruneRate > 0.00001f) {
                    result.addWarning("Pruning rate above 0.001% - may cause excessive neuron loss");
                }
                
            } catch (const std::exception&) {
                // Ignore if conversion fails
            }
        }
    }
    
    void validateDevelopmentTimeline(ValidationResult& result) const {
        auto startIt = tempConfig_.find("development_critical_period_start");
        auto endIt = tempConfig_.find("development_critical_period_end");
        
        if (startIt != tempConfig_.end() && endIt != tempConfig_.end()) {
            try {
                float start = std::stof(startIt->second);
                float end = std::stof(endIt->second);
                
                if (end <= start) {
                    result.addError("Critical period end must be after start");
                }
                
                if (start < 0 || end < 0) {
                    result.addError("Critical period values must be non-negative");
                }
                
            } catch (const std::exception&) {
                // Ignore if conversion fails
            }
        }
    }
    
    mutable std::unordered_map<std::string, std::string> tempConfig_;
};

/**
 * Configuration validation manager
 * Manages multiple validators and provides unified validation interface
 */
class ConfigValidationManager {
private:
    std::vector<std::shared_ptr<ConfigValidator>> validators_;
    bool strictValidation_ = false;
    
public:
    ConfigValidationManager() {
        // Register standard validators
        validators_.push_back(std::make_shared<DefaultConfigValidator>());
        validators_.push_back(std::make_shared<DependentConfigValidator>());
    }
    
    void enableStrictValidation(bool enabled) {
        strictValidation_ = enabled;
    }
    
    ValidationResult validate(const std::unordered_map<std::string, std::string>& config) const {
        ValidationResult overallResult;
        
        for (const auto& validator : validators_) {
            ValidationResult validatorResult = validator->validate(config);
            
            // Combine results
            overallResult.errors.insert(overallResult.errors.end(),
                                      validatorResult.errors.begin(), validatorResult.errors.end());
            overallResult.warnings.insert(overallResult.warnings.end(),
                                         validatorResult.warnings.begin(), validatorResult.warnings.end());
        }
        
        // Apply strict validation if enabled
        if (strictValidation_) {
            overallResult.valid = overallResult.errors.empty();
        }
        
        return overallResult;
    }
    
    ValidationResult validate(const std::string& key, const std::string& value) const {
        ValidationResult overallResult;
        
        for (const auto& validator : validators_) {
            ValidationResult validatorResult = validator->validate(key, value);
            
            // Combine results
            overallResult.errors.insert(overallResult.errors.end(),
                                      validatorResult.errors.begin(), validatorResult.errors.end());
            overallResult.warnings.insert(overallResult.warnings.end(),
                                         validatorResult.warnings.begin(), validatorResult.warnings.end());
        }
        
        // Apply strict validation if enabled
        if (strictValidation_) {
            overallResult.valid = overallResult.errors.empty();
        }
        
        return overallResult;
    }
    
    std::string getValidationSummary(const ValidationResult& result) const {
        std::stringstream ss;
        ss << "Configuration validation results:";
        ss << "\n  Valid: " << (result.valid ? "YES" : "NO");
        ss << "\n  Errors: " << result.errors.size();
        ss << "\n  Warnings: " << result.warnings.size();
        
        if (!result.errors.empty()) {
            ss << "\n  Error details:";
            for (size_t i = 0; i < result.errors.size(); ++i) {
                ss << "\n    " << (i + 1) << ". " << result.errors[i];
            }
        }
        
        if (!result.warnings.empty()) {
            ss << "\n  Warning details:";
            for (size_t i = 0; i < result.warnings.size(); ++i) {
                ss << "\n    " << (i + 1) << ". " << result.warnings[i];
            }
        }
        
        return ss.str();
    }
};

/**
 * Global validation manager instance
 */
inline ConfigValidationManager& getGlobalValidator() {
    static ConfigValidationManager validator;
    return validator;
}

/**
 * Convenience functions for validation
 */
inline ValidationResult validateConfig(const std::unordered_map<std::string, std::string>& config) {
    return getGlobalValidator().validate(config);
}

inline ValidationResult validateConfigValue(const std::string& key, const std::string& value) {
    return getGlobalValidator().validate(key, value);
}

inline std::string validateConfigAndGetSummary(const std::unordered_map<std::string, std::string>& config) {
    ValidationResult result = validateConfig(config);
    return getGlobalValidator().getValidationSummary(result);
}

} // namespace nlm