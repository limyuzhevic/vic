#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "Config.hpp"

namespace nlm {

/**
 * JSON parser for NLM configuration files
 * Supports basic JSON objects, arrays, strings, numbers, and booleans
 */
class JSONParser {
public:
    static bool parse(const std::string& content, std::vector<ConfigEntry>& entries);
    static std::string serialize(const std::vector<ConfigEntry>& entries);

private:
    static bool parseValue(const std::string& content, size_t& pos, ConfigValue& value);
    static bool parseString(const std::string& content, size_t& pos, std::string& str);
    static bool parseNumber(const std::string& content, size_t& pos, ConfigValue& value);
    static bool parseBoolean(const std::string& content, size_t& pos, ConfigValue& value);
    static bool parseArray(const std::string& content, size_t& pos, ConfigValue& value);
    static bool parseObject(const std::string& content, size_t& pos, std::vector<ConfigEntry>& entries);
    static void skipWhitespace(const std::string& content, size_t& pos);
    static bool expect(const std::string& content, size_t& pos, char expected);
    static std::string trim(const std::string& str);
    static bool isWhitespace(char c);
    static bool isDigit(char c);
    static bool isHexDigit(char c);
};

/**
 * YAML parser for NLM configuration files
 * Supports basic YAML structures with key=value format and nested structures
 */
class YAMLParser {
public:
    static bool parse(const std::string& content, std::vector<ConfigEntry>& entries);
    static std::string serialize(const std::vector<ConfigEntry>& entries);

private:
    static bool parseYAML(const std::string& content, size_t& pos, std::vector<ConfigEntry>& entries, int indent = 0);
    static bool parseYAMLKeyValue(const std::string& content, size_t& pos, std::vector<ConfigEntry>& entries);
    static bool parseYAMLArray(const std::string& content, size_t& pos, ConfigValue& value);
    static bool parseYAMLString(const std::string& content, size_t& pos, std::string& str);
    static bool parseYAMLScalar(const std::string& content, size_t& pos, ConfigValue& value);
    static void skipWhitespace(const std::string& content, size_t& pos);
    static void skipComment(const std::string& content, size_t& pos);
    static size_t countIndent(const std::string& line);
};

/**
 * Configuration schema validation
 * Provides validation rules for configuration values
 */
class ConfigSchema {
public:
    struct ValidationRule {
        std::string key;
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
        bool required;
    };

    ConfigSchema();
    
    void addValidationRule(const std::string& key, 
                          std::function<bool(const ConfigValue&)> validator,
                          const std::string& errorMessage = "",
                          bool required = false);
    
    bool validate(const std::vector<ConfigEntry>& entries, std::vector<std::string>& errors) const;
    
    void setDefaultValidation();

private:
    std::vector<ValidationRule> rules_;
};

/**
 * Advanced configuration manager
 * Extends basic configuration with runtime modification and change tracking
 */
class AdvancedConfig {
public:
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Disable copying
    AdvancedConfig(const AdvancedConfig&) = delete;
    AdvancedConfig& operator=(const AdvancedConfig&) = delete;
    
    bool loadFromFile(const std::string& filepath, const std::string& format = "auto");
    bool saveToFile(const std::string& filepath, const std::string& format = "yaml") const;
    
    std::shared_ptr<Config> getConfig() const { return config_; }
    void setConfig(std::shared_ptr<Config> config) { config_ = config; }
    
    // Runtime configuration modification
    bool setValue(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation
    bool validate(std::vector<std::string>& errors) const;
    
    // Change tracking
    void enableChangeTracking(bool enable) { trackChanges_ = enable; }
    bool hasChanges() const { return hasChanges_; }
    void clearChanges() { hasChanges_ = false; }
    
    // Schema management
    void setSchema(std::shared_ptr<ConfigSchema> schema) { schema_ = schema; }

private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<ConfigSchema> schema_;
    bool trackChanges_;
    bool hasChanges_;
    
    void markChanged() { if (trackChanges_) hasChanges_ = true; }
};

} // namespace nlm
