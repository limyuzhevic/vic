// ConfigManager.hpp - Improved configuration system with validation and defaults
#pragma once

#include "src/core/Config/Config.hpp"
#include "src/utils/ErrorHandlingUtils.hpp"
#include <map>
#include <string>
#include <memory>
#include <functional>
#include <type_traits>
#include <optional>

namespace nlm {

// Configuration schema definition
struct ConfigSchemaEntry {
    std::string key;
    ConfigValue defaultValue;
    ConfigSource defaultSource;
    std::string description;
    std::string category; // Group related settings
    bool required = false;
    std::function<bool(const ConfigValue&)> validator;
    std::map<std::string, std::string> validationMessages;
};

// Configuration categories
namespace ConfigCategories {
    constexpr const char* General = "General";
    constexpr const char* Simulation = "Simulation";
    constexpr const char* Neural = "Neural";
    constexpr const char* Plasticity = "Plasticity";
    constexpr const char* Memory = "Memory";
    constexpr const char* Development = "Development";
    constexpr const char* Neuromodulation = "Neuromodulation";
    constexpr const char* Cognition = "Cognition";
    constexpr const char* Output = "Output";
}

// Enhanced configuration validation
class ConfigValidator {
public:
    // Validate a configuration value against schema
    static bool validate(const std::string& key, const ConfigValue& value,
                        ErrorCategory category = ErrorCategory::Validation);
    
    // Validate all current configuration values
    static bool validateAll(const Config& config,
                           ErrorCategory category = ErrorCategory::Validation);
    
    // Get validation error messages for a key
    static std::string getValidationError(const std::string& key, const ConfigValue& value);
    
    // Register a schema entry
    static void registerSchema(const ConfigSchemaEntry& schema);
    
    // Get schema entry for a key
    static std::optional<ConfigSchemaEntry> getSchema(const std::string& key);
    
    // Check if a key has a schema
    static bool hasSchema(const std::string& key);
    
    // Load default schema
    static void loadDefaultSchema();
    
    // Clear all schema entries
    static void clearSchema();
    
private:
    static std::map<std::string, ConfigSchemaEntry> schemaMap_;
};

// Environment variable support
class EnvironmentVariableLoader {
public:
    // Load configuration from environment variables
    static bool loadFromEnvironment(Config& config,
                                    const std::string& prefix = "NLM_",
                                    ConfigSource source = ConfigSource::Runtime);
    
    // Convert environment variable value to appropriate type
    template<typename T>
    static T convertEnvValue(const std::string& str);
    
    // Check if environment variable exists and is non-empty
    static bool hasEnvVar(const std::string& name);
    
    // Get environment variable value
    static std::string getEnvVar(const std::string& name);
};

// Configuration manager with enhanced features
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    // Disable copying, enable moving
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    ConfigManager(ConfigManager&&) noexcept;
    ConfigManager& operator=(ConfigManager&&) noexcept;
    
    // Load configuration from multiple sources
    bool loadConfiguration(const std::string& configFile = "",
                          const std::vector<std::string>& commandLineArgs = {});
    
    // Merge configuration from another config
    bool mergeConfiguration(const Config& otherConfig,
                           ConfigSource source = ConfigSource::Runtime);
    
    // Apply configuration changes
    bool applyConfiguration(const Config& newConfig);
    
    // Validate and normalize configuration
    bool validateAndNormalize(Config& config) const;
    
    // Get configuration with validation
    template<typename T>
    std::optional<T> getValidated(const std::string& key) const;
    
    // Get configuration with validation and default
    template<typename T>
    T getValidatedOr(const std::string& key, const T& defaultValue) const;
    
    // Check if configuration is valid
    bool isValid() const { return validationError_.empty(); }
    
    // Get validation errors
    const std::string& getValidationErrors() const { return validationError_; }
    
    // Get configuration summary with validation info
    std::string getConfigurationSummary() const;
    
    // Reset to defaults
    void resetToDefaults();
    
    // Clear configuration
    void clear();
    
    // Get configuration source count
    size_t getConfigurationSourceCount() const;
    
    // Get configuration entry count
    size_t getConfigurationEntryCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Validation error
    std::string validationError_;
    
    // Configuration categories
    std::map<std::string, std::vector<std::string>> categoryIndex_;
    
    // Update category index
    void updateCategoryIndex() const;
    
    // Validate single configuration value
    bool validateSingleValue(const std::string& key, const ConfigValue& value,
                            ErrorCategory category) const;
    
    // Apply default values
    void applyDefaultValues(Config& config) const;
    
    // Handle environment variable overrides
    void applyEnvironmentVariableOverrides(Config& config) const;
};

// Convenience functions for common configuration patterns
inline bool loadConfigFile(ConfigManager& manager, const std::string& filepath = "configs/default.cfg") {
    return manager.loadConfiguration(filepath);
}

inline bool loadConfigArgs(ConfigManager& manager, int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return manager.loadConfiguration("", args);
}

inline bool loadConfigFromEnv(ConfigManager& manager, const std::string& prefix = "NLM_") {
    Config config;
    return EnvironmentVariableLoader::loadFromEnvironment(config, prefix) && 
           manager.mergeConfiguration(config);
}

inline std::string getConfigCategoryDescription(const std::string& category) {
    if (category == ConfigCategories::General) return "General simulation settings";
    if (category == ConfigCategories::Simulation) return "Core simulation parameters";
    if (category == ConfigCategories::Neural) return "Neural system parameters";
    if (category == ConfigCategories::Plasticity) return "Plasticity and learning rules";
    if (category == ConfigCategories::Memory) return "Memory system settings";
    if (category == ConfigCategories::Development) return "Developmental processes";
    if (category == ConfigCategories::Neuromodulation) return "Neuromodulation systems";
    if (category == ConfigCategories::Cognition) return "Cognitive processing";
    if (category == ConfigCategories::Output) return "Output and logging";
    return "Unknown configuration category";
}

// Configuration helper macros
#define NLM_CONFIG_VALIDATE(key, value) \
    nlm::ConfigValidator::validate(key, value)

#define NLM_CONFIG_SET(manager, key, value) \
    do { \
        auto& config = manager.getConfig(); \
        config.set(key, value); \
        config.validate(); \
    } while(0)

#define NLM_CONFIG_GET(manager, key) \
    nlm::ConfigValidator::getValidated<decltype(std::declval<nlm::Config>().getOr(0))>(key)

} // namespace nlm