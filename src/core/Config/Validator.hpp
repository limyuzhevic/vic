#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <memory>

namespace nlm {

/**
 * @brief Configuration validation rules and constraints
 * 
 * The ConfigValidator class provides validation for NLM configuration values,
 * ensuring that configurations are valid and complete before use in brain
 * initialization and simulation.
 */
class ConfigValidator {
public:
    /**
     * @brief Validation rule type - function that returns error message or empty string if valid
     */
    using ValidationRule = std::function<std::string(const std::string&, const ConfigValue&)>;
    
    /**
     * @brief Validation error structure
     */
    struct ValidationError {
        std::string key;           ///< Configuration key that failed validation
        std::string message;        ///< Error message describing the issue
        ConfigSource source;       ///< Source of the configuration value
        
        ValidationError(const std::string& k, const std::string& msg, ConfigSource s)
            : key(k), message(msg), source(s) {}
    };
    
    /**
     * @brief Default configuration schema
     * 
     * Contains validation rules for all standard NLM configuration parameters.
     */
    struct DefaultSchema {
        // Brain configuration
        static void createBrainSchema(std::unordered_map<std::string, ValidationRule>& schema);
        
        // Plasticity configuration
        static void createPlasticitySchema(std::unordered_map<std::string, ValidationRule>& schema);
        
        // Neuromodulation configuration
        static void createNeuromodulationSchema(std::unordered_map<std::string, ValidationRule>& schema);
        
        // Memory system configuration
        static void createMemorySchema(std::unordered_map<std::string, ValidationRule>& schema);
        
        // Prediction system configuration
        static void createPredictionSchema(std::unordered_map<std::string, ValidationRule>& schema);
        
        // Cognition system configuration
        static void createCognitionSchema(std::unordered_map<std::string, ValidationRule>& schema);
        
        // Development system configuration
        static void createDevelopmentSchema(std::unordered_map<std::string, ValidationRule>& schema);
        
        // World simulation configuration
        static void createWorldSchema(std::unordered_map<std::string, ValidationRule>& schema);
    };
    
    ConfigValidator();
    ~ConfigValidator();
    
    /**
     * @brief Validate configuration against schema
     * 
     * @param config Configuration to validate
     * @return Vector of validation errors (empty if valid)
     */
    std::vector<ValidationError> validate(const Config& config) const;
    
    /**
     * @brief Validate specific configuration key
     * 
     * @param config Configuration to validate
     * @param key Configuration key to validate
     * @return Error message if invalid, empty string if valid
     */
    std::string validateKey(const Config& config, const std::string& key) const;
    
    /**
     * @brief Add custom validation rule
     * 
     * @param key Configuration key pattern to match
     * @param rule Validation function
     */
    void addValidationRule(const std::string& key, const ValidationRule& rule);
    
    /**
     * @brief Remove validation rule
     * 
     * @param key Configuration key pattern to remove
     */
    void removeValidationRule(const std::string& key);
    
    /**
     * @brief Check if configuration is complete
     * 
     * @param config Configuration to check
     * @return true if configuration has all required keys, false otherwise
     */
    bool isComplete(const Config& config) const;
    
    /**
     * @brief Get list of required configuration keys
     * 
     * @return Vector of required key patterns
     */
    std::vector<std::string> getRequiredKeys() const;
    
    /**
     * @brief Get validation errors for specific category
     * 
     * @param config Configuration to validate
     * @param category Validation category ("brain", "plasticity", "memory", etc.)
     * @return Vector of validation errors in that category
     */
    std::vector<ValidationError> getErrorsByCategory(const Config& config, const std::string& category) const;
    
    /**
     * @brief Create default validator with NLM-standard validation
     * 
     * @return Shared pointer to validated validator
     */
    static std::shared_ptr<ConfigValidator> createDefaultValidator();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // === INTERNAL VALIDATION HELPERS ===
    
    static bool isNumeric(const ConfigValue& value);
    static bool isString(const ConfigValue& value);
    static bool isBoolean(const ConfigValue& value);
    static bool isArray(const ConfigValue& value);
    static double extractNumericValue(const ConfigValue& value);
    static std::string configValueToString(const ConfigValue& value);
};

} // namespace nlm