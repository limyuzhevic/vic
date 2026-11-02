#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <variant>
#include <functional>
#include <limits>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class Config;
class ConfigValidator;

using json = nlohmann::json;

// Enhanced configuration schema
struct ConfigSchema {
    std::string key;
    std::string type;
    std::string description;
    std::variant<
        int,
        int64_t,
        double,
        bool,
        std::string
    > default_value;
    std::vector<std::variant<int, int64_t, double, bool, std::string>> allowed_values;
    bool required = false;
    double min = std::numeric_limits<double>::lowest();
    double max = std::numeric_limits<double>::max();
    
    // Validation functions
    std::function<bool(const json&)> validator;
    
    ConfigSchema() : required(false) {}
    
    ConfigSchema(const std::string& k, const std::string& t, const std::string& desc = "",
                 const std::variant<int, int64_t, double, bool, std::string>& def = {}, 
                 bool req = false, double mn = std::numeric_limits<double>::lowest(),
                 double mx = std::numeric_limits<double>::max())
        : key(k), type(t), description(desc), default_value(def), required(req),
          min(mn), max(mx) {}
};

// Configuration preset definitions
struct ConfigPreset {
    std::string name;
    std::string description;
    json config;
    
    ConfigPreset(const std::string& n, const std::string& desc, const json& c)
        : name(n), description(desc), config(c) {}
};

// Enhanced configuration validator
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Disable copying, enable moving
    ConfigValidator(const ConfigValidator&) = delete;
    ConfigValidator& operator=(const ConfigValidator&) = delete;
    ConfigValidator(ConfigValidator&&) noexcept;
    ConfigValidator& operator=(ConfigValidator&&) noexcept;
    
    // Load schema from JSON file
    bool loadSchemaFromFile(const std::string& filepath);
    
    // Load schema from JSON string
    bool loadSchemaFromString(const std::string& json_str);
    
    // Get schema
    const json& getSchema() const { return schema_; }
    
    // Get preset list
    const std::vector<ConfigPreset>& getPresets() const { return presets_; }
    
    // Find preset by name
    std::optional<ConfigPreset> findPreset(const std::string& name) const;
    
    // Validate configuration against schema
    std::vector<std::string> validate(const json& config) const;
    
    // Validate configuration object
    std::vector<std::string> validate(const std::shared_ptr<Config>& config) const;
    
    // Get validation errors for a specific key
    std::vector<std::string> getKeyErrors(const std::string& key, const json& config) const;
    
    // Check if configuration is valid
    bool isValid(const json& config) const;
    bool isValid(const std::shared_ptr<Config>& config) const;
    
    // Get validation statistics
    json getValidationStats(const json& config) const;
    
    // Create default configuration from schema
    json createDefaultConfig() const;
    
    // Get all configuration keys from schema
    std::vector<std::string> getAllKeys() const;
    
    // Check if key exists in schema
    bool hasKey(const std::string& key) const;
    
    // Get key information from schema
    std::optional<json> getKeyInfo(const std::string& key) const;
    
    // Merge configurations
    static json mergeConfigs(const std::vector<json>& configs);
    
    // Merge configuration objects
    static json mergeConfigs(const std::vector<std::shared_ptr<Config>>& configs);
    
    // Diff two configurations
    static json diffConfig(const json& config1, const json& config2);
    
    // Get configuration diff in human-readable format
    static std::vector<std::string> diffConfigHuman(const json& config1, const json& config2);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    json schema_;
    std::vector<ConfigPreset> presets_;
    
    void loadDefaultPresets();
    bool validateJSONSchema(const json& schema) const;
    std::vector<std::string> validateJSONValue(const std::string& key, const json& value, const json& schema) const;
    std::vector<std::string> getConstraintMessages(const std::string& key, const json& value, const json& schema) const;
};

// Enhanced configuration class with schema support
class EnhancedConfig {
public:
    EnhancedConfig(std::shared_ptr<ConfigValidator> validator = nullptr);
    ~EnhancedConfig();
    
    // Disable copying, enable moving
    EnhancedConfig(const EnhancedConfig&) = delete;
    EnhancedConfig& operator=(const EnhancedConfig&) = delete;
    EnhancedConfig(EnhancedConfig&&) noexcept;
    EnhancedConfig& operator=(EnhancedConfig&&) noexcept;
    
    // Create enhanced configuration from preset
    static std::shared_ptr<EnhancedConfig> createFromPreset(const std::string& preset_name,
                                                           std::shared_ptr<ConfigValidator> validator = nullptr);
    
    // Create from JSON string
    static std::shared_ptr<EnhancedConfig> createFromJSON(const std::string& json_str,
                                                         std::shared_ptr<ConfigValidator> validator = nullptr);
    
    // Create from file
    static std::shared_ptr<EnhancedConfig> createFromFile(const std::string& filepath,
                                                         std::shared_ptr<ConfigValidator> validator = nullptr);
    
    // Validate configuration
    std::vector<std::string> validate() const;
    bool isValid() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Get validation statistics
    json getValidationStats() const;
    
    // Get the underlying config
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    // Access configuration as JSON
    json toJSON() const;
    
    // Access specific configuration value
    template<typename T>
    T get(const std::string& key) const {
        auto value = config_->getOr<T>(key, T{});
        return value;
    }
    
    // Set configuration value with validation
    void set(const std::string& key, const json& value);
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, int value);
    void set(const std::string& key, int64_t value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);
    
    // Check if configuration has key
    bool has(const std::string& key) const { return config_->has(key); }
    
    // Get all keys
    std::vector<std::string> getKeys() const { return config_->getKeys(); }
    
    // Get keys from schema
    std::vector<std::string> getValidKeys() const;
    
    // Get configuration info from schema
    json getKeyInfo(const std::string& key) const;
    
    // Merge with another configuration
    void merge(const std::shared_ptr<EnhancedConfig>& other);
    
    // Merge with JSON configuration
    void mergeFromJSON(const json& other);
    
    // Get configuration diff
    json diff(const std::shared_ptr<EnhancedConfig>& other) const;
    
    // Compare configurations
    bool equals(const std::shared_ptr<EnhancedConfig>& other) const;
    
    // Save to JSON file
    bool saveToFile(const std::string& filepath) const;
    
    // Get configuration summary
    std::string summary() const;
    
    // Get JSON schema description
    std::string getSchemaDescription() const;
    
    // Create configuration helper for common operations
    class ConfigHelper {
    public:
        static std::shared_ptr<EnhancedConfig> createMinimal();
        static std::shared_ptr<EnhancedConfig> createResearch();
        static std::shared_ptr<EnhancedConfig> createRealTime();
        static std::shared_ptr<EnhancedConfig> createCustom(const std::string& name);
        
        static std::vector<std::string> getAllPresetNames();
        static std::shared_ptr<EnhancedConfig> createFromPreset(const std::string& preset_name);
        
        static std::shared_ptr<ConfigValidator> createValidator();
        static std::shared_ptr<ConfigValidator> createValidator(const std::string& schema_file);
    };
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<Config> config_;
    std::shared_ptr<ConfigValidator> validator_;
    
    void validateKey(const std::string& key, const json& value) const;
};

} // namespace nlm