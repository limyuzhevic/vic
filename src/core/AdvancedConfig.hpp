#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <functional>
#include <memory>

namespace nlm {

/**
 * Advanced configuration system with validation and schema support
 * 
 * This provides:
 * 1. Configuration schema definitions
 * 2. Runtime validation
 * 3. Environment variable support
 * 4. Command-line argument parsing
 * 5. Configuration inheritance
 * 6. Hot-reload capability
 */

class AdvancedConfig {
public:
    /**
     * Configuration validation error
     */
    struct ValidationError {
        std::string key;
        std::string message;
        std::string value;
        
        ValidationError(const std::string& k, const std::string& m, const std::string& v = "")
            : key(k), message(m), value(v) {}
    };

    /**
     * Configuration schema definition
     */
    struct ConfigSchema {
        using ValidatorFunc = std::function<bool(const ConfigValue&)>;
        using TransformerFunc = std::function<ConfigValue(const ConfigValue&)>;
        
        std::string key;
        std::string description;
        ConfigValue defaultValue;
        bool required;
        std::vector<ValidatorFunc> validators;
        std::vector<TransformerFunc> transformers;
        std::string category;
        int order;
        
        ConfigSchema(const std::string& k, const std::string& desc, const ConfigValue& def,
                    bool req = false, const std::string& cat = "General", int ord = 0)
            : key(k), description(desc), defaultValue(def), required(req),
              category(cat), order(ord) {}
        
        bool validate(const ConfigValue& value) const;
        ConfigValue transform(const ConfigValue& value) const;
        std::string getTypeName() const;
    };

    /**
     * Configuration category for organization
     */
    enum class Category {
        Core,
        Memory,
        Plasticity,
        Neuromodulation,
        Cognition,
        Development,
        Performance,
        Debugging,
        Experimental
    };

    AdvancedConfig();
    ~AdvancedConfig();
    
    // Disable copying, enable moving
    AdvancedConfig(const AdvancedConfig&) = delete;
    AdvancedConfig& operator=(const AdvancedConfig&) = delete;
    AdvancedConfig(AdvancedConfig&&) noexcept;
    AdvancedConfig& operator=(AdvancedConfig&&) noexcept;

    /**
     * Add configuration schema
     */
    void addSchema(const ConfigSchema& schema);
    
    /**
     * Add schema with type specialization
     */
    template<typename T>
    void addSchema(const std::string& key, const std::string& description,
                   const T& defaultValue, bool required = false,
                   const std::string& category = "General", int order = 0);

    /**
     * Load configuration from file
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * Save configuration to file
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * Load from environment variables
     */
    void loadFromEnvironment(const std::string& prefix = "NLM_");
    
    /**
     * Load from command line arguments
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * Validate current configuration
     */
    std::vector<ValidationError> validate() const;
    
    /**
     * Get validation errors (convenience)
     */
    std::vector<std::string> getValidationErrors() const;
    
    /**
     * Check if configuration is valid
     */
    bool isValid() const;
    
    /**
     * Get configuration value with type safety
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * Get value or default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * Set value with validation
     */
    bool set(const std::string& key, const ConfigValue& value);
    template<typename T>
    bool set(const std::string& key, const T& value);
    
    /**
     * Check if key exists
     */
    bool has(const std::string& key) const;
    
    /**
     * Get schema for a key
     */
    const ConfigSchema* getSchema(const std::string& key) const;
    
    /**
     * Get all keys in a category
     */
    std::vector<std::string> getKeysInCategory(Category category) const;
    
    /**
     * Get configuration summary
     */
    std::string summary() const;
    
    /**
     * Print validation errors
     */
    void printValidationErrors(std::ostream& out = std::cout) const;
    
    /**
     * Generate JSON schema for configuration
     */
    std::string generateJSONSchema() const;
    
    /**
     * Get default configuration
     */
    static std::shared_ptr<AdvancedConfig> createDefaultConfig();
    
    /**
     * Merge another configuration
     */
    void merge(const AdvancedConfig& other);
    
    /**
     * Reset to defaults
     */
    void resetToDefaults();
    
    /**
     * Hot-reload configuration from file
     */
    bool hotReload(const std::string& filepath);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void initializeSchemas();
    std::string getCategoryName(Category category) const;
};

// Inline template implementations

template<typename T>
void AdvancedConfig::addSchema(const std::string& key, const std::string& description,
                               const T& defaultValue, bool required,
                               const std::string& category, int order) {
    ConfigSchema schema(key, description, ConfigValue(defaultValue), required, category, order);
    addSchema(schema);
}

template<typename T>
std::optional<T> AdvancedConfig::get(const std::string& key) const {
    auto schema = getSchema(key);
    if (!schema) {
        return std::nullopt;
    }
    
    // Get value from base config
    auto value = pImpl->config->get<T>(key);
    if (value) {
        return value;
    }
    
    // Return default if not set
    if (std::holds_alternative<T>(schema->defaultValue)) {
        return std::get<T>(schema->defaultValue);
    }
    
    return std::nullopt;
}

template<typename T>
T AdvancedConfig::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value) {
        return *value;
    }
    return defaultValue;
}

template<typename T>
bool AdvancedConfig::set(const std::string& key, const T& value) {
    return set(key, ConfigValue(value));
}

// Predefined schema categories
class ConfigSchemaRegistry {
public:
    static AdvancedConfig::ConfigSchema createCoreSchemas();
    static AdvancedConfig::ConfigSchema createMemorySchemas();
    static AdvancedConfig::ConfigSchema createPlasticitySchemas();
    static AdvancedConfig::ConfigSchema createNeuromodulationSchemas();
    static AdvancedConfig::ConfigSchema createCognitionSchemas();
    static AdvancedConfig::ConfigSchema createDevelopmentSchemas();
    static AdvancedConfig::ConfigSchema createPerformanceSchemas();
    static AdvancedConfig::ConfigSchema createDebuggingSchemas();
    static AdvancedConfig::ConfigSchema createExperimentalSchemas();
    
    static std::vector<AdvancedConfig::ConfigSchema> getAllSchemas();
};

} // namespace nlm
