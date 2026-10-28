// Config.hpp - Improved Configuration System
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default = 0,
    File = 1,
    CommandLine = 2,
    Runtime = 3
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::string errorMessage;
    std::string path; // Where the validation failed
    
    ValidationResult(bool valid = true, const std::string& error = "", 
                     const std::string& path = "") 
        : isValid(valid), errorMessage(error), path(path) {}
};

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string key;
    std::string type; // "int", "double", "bool", "string", "list<int>", etc.
    std::string description;
    ConfigValue defaultValue;
    bool required;
    std::vector<std::string> allowedValues; // For string/enum types
    std::function<ValidationResult(const ConfigValue&)> validator;
    
    ConfigSchemaEntry() : required(false), validator(nullptr) {}
};

// Configuration schema
class ConfigSchema {
private:
    std::map<std::string, ConfigSchemaEntry> schema;
    std::vector<std::string> requiredKeys;
    
public:
    // Add a schema entry
    void addEntry(const std::string& key, const std::string& type, 
                  const std::string& description, bool required = false,
                  const ConfigValue& defaultValue = {}, 
                  const std::vector<std::string>& allowedValues = {});
    
    // Add a custom validator
    void addValidator(const std::string& key, 
                      std::function<ValidationResult(const ConfigValue&)> validator);
    
    // Validate a configuration value
    ValidationResult validate(const std::string& key, const ConfigValue& value) const;
    
    // Validate all configuration entries
    std::vector<ValidationResult> validateAll(const std::map<std::string, ConfigValue>& config) const;
    
    // Get schema entry
    const ConfigSchemaEntry* getEntry(const std::string& key) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Check if key exists in schema
    bool hasKey(const std::string& key) const;
    
    // Get required keys
    std::vector<std::string> getRequiredKeys() const;
    
    // Get default value for key
    ConfigValue getDefaultValue(const std::string& key) const;
    
    // Convert schema to JSON
    nlohmann::json toJson() const;
    
    // Load schema from JSON
    bool loadFromJson(const nlohmann::json& json);
    
    // Load schema from file
    bool loadFromFile(const std::string& filepath);
    
    // Save schema to file
    bool saveToFile(const std::string& filepath) const;
};

// Configuration manager
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from JSON string
    bool loadFromJson(const std::string& jsonString);
    
    // Load from YAML string (if YAML support available)
    bool loadFromYaml(const std::string& yamlString);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Set schema
    void setSchema(std::shared_ptr<ConfigSchema> schema);
    
    // Get schema
    std::shared_ptr<ConfigSchema> getSchema() const;
    
    // Set default schema (built-in configuration)
    void setDefaultSchema();
    
    // Get default schema instance
    static std::shared_ptr<ConfigSchema> getDefaultSchema();
    
    // Get values with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    bool set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validate configuration against schema
    std::vector<ValidationResult> validate() const;
    
    // Check if configuration is valid
    bool isValid() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Merge with another configuration
    void merge(const Config& other);
    
    // Override with command line arguments
    void overrideWithArgs(int argc, char** argv);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    std::shared_ptr<ConfigSchema> defaultSchema;
    std::vector<ValidationResult> validationResults;
};