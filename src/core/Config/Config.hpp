#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <unordered_set>

namespace nlm {

// Forward declarations
class Config;
class ConfigSection;
class ConfigSchema;
class ConfigManager;

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
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::vector<std::string>& errs, const std::vector<std::string>& warns = {})
        : valid(v), errors(errs), warnings(warns) {}
};

// Configuration schema definition
struct SchemaEntry {
    std::string key;
    std::string type;
    std::string description;
    std::optional<ConfigValue> defaultValue;
    std::function<bool(const ConfigValue&)> validator;
    bool required;
    
    SchemaEntry(const std::string& k, const std::string& t, const std::string& desc = "")
        : key(k), type(t), description(desc), required(false) {}
};

// Configuration section for organized settings
class ConfigSection {
public:
    ConfigSection(const std::string& name, const std::string& description = "");
    
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    
    void addEntry(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Default, const std::string& desc = "");
    std::optional<ConfigValue> getValue(const std::string& key) const;
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    
    std::string summary() const;
    
private:
    std::string name;
    std::string description;
    std::map<std::string, ConfigEntry> entries;
};

// Configuration schema for validation and defaults
class ConfigSchema {
public:
    ConfigSchema();
    
    void addEntry(const SchemaEntry& entry);
    ValidationResult validate(const std::map<std::string, ConfigEntry>& entries) const;
    ConfigValue getDefaultValue(const std::string& key) const;
    bool hasEntry(const std::string& key) const;
    std::string getEntryType(const std::string& key) const;
    
    void addValidation(const std::string& key, const std::function<bool(const ConfigValue&)>& validator);
    
private:
    bool validateType(const ConfigValue& value, const std::string& type) const;
    
    std::map<std::string, SchemaEntry> schemaEntries;
    std::vector<std::function<bool(const ConfigValue&)>> customValidators;
};

// Configuration manager for validation and management
class ConfigManager {
public:
    ConfigManager();
    
    ValidationResult validate(const Config& config) const;
    bool validateAndFix(Config& config) const;
    bool applySchema(Config& config) const;
    std::string getValidationErrors(const Config& config) const;
    
    void addSchema(const std::string& name, const ConfigSchema& schema);
    ConfigSchema* getSchema(const std::string& name);
    void removeSchema(const std::string& name);
    
    void setDefaultValues(Config& config) const;
    void inheritConfig(Config& config, const Config& parent) const;
    
private:
    std::map<std::string, ConfigSchema> schemas;
};

// Main configuration class with enhanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON, YAML, or text format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (supports JSON/YAML/text based on extension)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Nested configuration access (key.format.key.nested)
    template<typename T>
    std::optional<T> get(const std::string& key, char delimiter) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue, char delimiter) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Check if nested key exists
    bool has(const std::string& key, char delimiter) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
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
    
    // Enhanced features
    
    // Section management
    void addSection(const std::string& name, const std::string& description = "");
    bool hasSection(const std::string& name) const;
    ConfigSection* getSection(const std::string& name);
    const ConfigSection* getSection(const std::string& name) const;
    void removeSection(const std::string& name);
    std::vector<std::string> getSectionNames() const;
    
    // Schema management
    void setSchema(const ConfigSchema& schema);
    ValidationResult validate() const;
    
    // Default values management
    void setDefault(const std::string& key, const ConfigValue& value);
    std::optional<ConfigValue> getDefault(const std::string& key) const;
    
    // Inheritance
    void inheritFrom(const Config& other);
    
    // Export/Import
    bool exportToFile(const std::string& filepath, const std::string& format = "json") const;
    bool importFromFile(const std::string& filepath);
    
    // Versioning
    void setVersion(const std::string& version);
    std::string getVersion() const { return version; }
    
    // Context-aware configuration
    void setContext(const std::string& context);
    std::string getContext() const { return context; }
    
    // Configuration merging
    void merge(const Config& other, bool override = true);
    
    // Configuration manager
    void setManager(const ConfigManager& manager) { this->manager = manager; }
    const ConfigManager& getManager() const { return manager; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::string version;
    std::string context;
    ConfigManager manager;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::vector<std::string> extractKeySegments(const std::string& key, char delimiter);
    static ConfigValue parseValue(const std::string& valueStr);
    static std::string valueToString(const ConfigValue& value);
    
    // File format specific loaders/savers
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromText(const std::string& filepath);
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    bool saveToText(const std::string& filepath) const;
    
    // Conversion helpers
    void fromJSON(const nlohmann::json& j, const std::string& prefix, ConfigSource source);
    void fromYAML(const YAML::Node& node, const std::string& prefix, ConfigSource source);
    ConfigValue jsonToConfigValue(const nlohmann::json& j);
    ConfigValue yamlToConfigValue(const YAML::Node& node);
    
    void setJSONValue(nlohmann::json& j, const std::string& key, const ConfigValue& value) const;
    void setYAMLValue(YAML::Emitter& out, const std::string& key, const ConfigValue& value) const;
    nlohmann::json configValueToJSON(const ConfigValue& value) const;
    void yamlValueToEmitter(YAML::Emitter& out, const ConfigValue& value) const;
};

// ConfigSection implementation details
class ConfigSection {
public:
    ConfigSection(const std::string& name, const std::string& description = "")
        : name(name), description(description) {}
    
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    
    void addEntry(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Default, const std::string& desc = "") {
        entries[key] = ConfigEntry(key, value, source, desc);
    }
    
    std::optional<ConfigValue> getValue(const std::string& key) const {
        auto it = entries.find(key);
        if (it != entries.end()) {
            return it->second.value;
        }
        return std::nullopt;
    }
    
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        auto valueOpt = getValue(key);
        if (!valueOpt) {
            return std::nullopt;
        }
        try {
            return std::get<T>(*valueOpt);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    bool has(const std::string& key) const {
        return entries.find(key) != entries.end();
    }
    
    void remove(const std::string& key) {
        entries.erase(key);
    }
    
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(entries.size());
        for (const auto& pair : entries) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    std::string summary() const {
        std::ostringstream oss;
        oss << "Section: " << name << " (" << description << ")\n";
        oss << "  Entries: " << entries.size() << "\n";
        for (const auto& pair : entries) {
            oss << "    " << pair.first << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, pair.second.value);
            oss << "] (" << static_cast<int>(pair.second.source) << ")\n";
        }
        return oss.str();
    }
    
private:
    std::string name;
    std::string description;
    std::map<std::string, ConfigEntry> entries;
};

// ConfigSchema implementation details
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    void addEntry(const SchemaEntry& entry) {
        schemaEntries[entry.key] = entry;
    }
    
    ValidationResult validate(const std::map<std::string, ConfigEntry>& entries) const {
        ValidationResult result;
        for (const auto& [key, entry] : schemaEntries) {
            auto it = entries.find(key);
            if (it == entries.end()) {
                if (entry.required) {
                    result.valid = false;
                    result.errors.push_back("Required config entry missing: " + key);
                }
                continue;
            }
            
            // Validate type
            if (!validateType(it->second.value, entry.type)) {
                result.valid = false;
                result.errors.push_back("Type mismatch for config entry: " + key);
            }
            
            // Run custom validators
            for (const auto& validator : customValidators) {
                if (!validator(it->second.value)) {
                    result.valid = false;
                    result.errors.push_back("Validation failed for config entry: " + key);
                }
            }
        }
        
        return result;
    }
    
    ConfigValue getDefaultValue(const std::string& key) const {
        auto it = schemaEntries.find(key);
        if (it != schemaEntries.end() && it->second.defaultValue.has_value()) {
            return it->second.defaultValue.value();
        }
        return ConfigValue();
    }
    
    bool hasEntry(const std::string& key) const {
        return schemaEntries.find(key) != schemaEntries.end();
    }
    
    std::string getEntryType(const std::string& key) const {
        auto it = schemaEntries.find(key);
        if (it != schemaEntries.end()) {
            return it->second.type;
        }
        return "unknown";
    }
    
    void addValidation(const std::string& key, const std::function<bool(const ConfigValue&)>& validator) {
        customValidators.push_back(validator);
    }
    
private:
    bool validateType(const ConfigValue& value, const std::string& type) const {
        if (type == "int") return std::holds_alternative<int>(value);
        if (type == "int64_t") return std::holds_alternative<int64_t>(value);
        if (type == "double") return std::holds_alternative<double>(value);
        if (type == "bool") return std::holds_alternative<bool>(value);
        if (type == "string") return std::holds_alternative<std::string>(value);
        if (type == "vector<int>") return std::holds_alternative<std::vector<int>>(value);
        if (type == "vector<double>") return std::holds_alternative<std::vector<double>>(value);
        if (type == "vector<string>") return std::holds_alternative<std::vector<std::string>>(value);
        return false;
    }
    
    std::map<std::string, SchemaEntry> schemaEntries;
    std::vector<std::function<bool(const ConfigValue&)>> customValidators;
};

// ConfigManager implementation details
class ConfigManager {
public:
    ConfigManager() = default;
    
    ValidationResult validate(const Config& config) const {
        ValidationResult result;
        // Convert entries to map for validation
        std::map<std::string, ConfigEntry> entryMap;
        for (const auto& entry : config.pImpl->entries) {
            entryMap[entry.key] = entry;
        }
        
        // Validate against schema if set
        if (!config.pImpl->schema.schemaEntries.empty()) {
            result = config.pImpl->schema.validate(entryMap);
        }
        
        // Additional validations can be added here
        return result;
    }
    
    bool validateAndFix(Config& config) const {
        ValidationResult result = validate(config);
        if (!result.valid) {
            return false;
        }
        // Fix config based on validation results
        return true;
    }
    
    bool applySchema(Config& config) const {
        // Apply schema to config
        return true;
    }
    
    std::string getValidationErrors(const Config& config) const {
        ValidationResult result = validate(config);
        std::string errors;
        for (const auto& error : result.errors) {
            errors += error + "\n";
        }
        for (const auto& warning : result.warnings) {
            errors += "Warning: " + warning + "\n";
        }
        return errors;
    }
    
    void addSchema(const std::string& name, const ConfigSchema& schema) {
        schemas[name] = schema;
    }
    
    ConfigSchema* getSchema(const std::string& name) {
        auto it = schemas.find(name);
        if (it != schemas.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    void removeSchema(const std::string& name) {
        schemas.erase(name);
    }
    
    void setDefaultValues(Config& config) const {
        for (const auto& [key, value] : config.pImpl->defaults) {
            if (!config.has(key)) {
                config.set(key, value, ConfigSource::Default);
            }
        }
    }
    
    void inheritConfig(Config& config, const Config& parent) const {
        for (const auto& entry : parent.pImpl->entries) {
            if (!config.has(entry.key)) {
                config.set(entry.key, entry.value, entry.source);
            }
        }
    }
    
private:
    std::map<std::string, ConfigSchema> schemas;
};

// Explicit template instantiations
namespace {
    // Common types for Config
    using IntConfig = nlm::Config;
    using FloatConfig = nlm::Config;
    using StringConfig = nlm::Config;
    using BoolConfig = nlm::Config;
    
    // Common types for ConfigSection
    using SectionStringConfig = nlm::ConfigSection;
    using SectionIntConfig = nlm::ConfigSection;
    using SectionBoolConfig = nlm::ConfigSection;
}

} // namespace nlm