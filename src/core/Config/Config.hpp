#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<float>,
    std::vector<bool>,
    std::unordered_map<std::string, std::string>,
    std::unordered_map<std::string, double>,
    std::unordered_map<std::string, int>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Template,
    Merged
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;
    std::vector<std::string> tags;
    bool validated;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   category("general"), validated(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
               const std::string& desc = "", const std::string& cat = "general")
        : key(k), value(v), source(s), description(desc), category(cat), validated(false) {}
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::vector<std::string>& errs,
                   const std::vector<std::string>& warns = {})
        : valid(v), errors(errs), warnings(warns) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        valid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Configuration template
struct ConfigTemplate {
    std::string name;
    std::string description;
    std::vector<ConfigEntry> entries;
    std::string category;
    bool isExperimental;
    
    ConfigTemplate() : isExperimental(false) {}
    ConfigTemplate(const std::string& n, const std::string& desc)
        : name(n), description(desc), isExperimental(false) {}
};

// Configuration comparison result
struct ConfigComparison {
    std::vector<std::string> differences;
    std::vector<std::string> similarities;
    
    bool hasDifferences() const { return !differences.empty(); }
};

// Configuration statistics
struct ConfigStats {
    size_t totalEntries;
    size_t entriesBySource[4];  // Default, File, CommandLine, Runtime
    size_t entriesByCategory;
    std::vector<std::string> categories;
    
    ConfigStats() : totalEntries(0) {
        std::fill(std::begin(entriesBySource), std::end(entriesBySource), 0);
    }
};

// Main configuration class
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
    bool loadFromFile(const std::string& filepath, ConfigSource source = ConfigSource::File);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& json, ConfigSource source = ConfigSource::File);
    
    // Load from YAML string (if YAML support available)
    // bool loadFromYAML(const std::string& yaml, ConfigSource source = ConfigSource::File);
    
    // Save to file
    bool saveToFile(const std::string& filepath, bool pretty = false) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Nested configuration access
    std::string getNested(const std::string& key) const;
    double getNestedFloat(const std::string& key, double defaultValue = 0.0) const;
    int getNestedInt(const std::string& key, int defaultValue = 0) const;
    bool getNestedBool(const std::string& key, bool defaultValue = false) const;
    
    // Set nested configuration
    void setNested(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setNestedFloat(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setNestedInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by category
    std::vector<std::string> getKeys(const std::string& category) const;
    
    // Get categories
    std::vector<std::string> getCategories() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration statistics
    ConfigStats getStats() const;
    
    // Validation
    ValidationResult validate() const;
    
    // Configuration templates
    static std::vector<std::string> getAvailableTemplates();
    static std::vector<ConfigTemplate> getAllTemplates();
    static ConfigTemplate getTemplate(const std::string& name);
    
    // Apply template
    bool applyTemplate(const std::string& name, ConfigSource source = ConfigSource::Template);
    bool applyTemplate(const ConfigTemplate& template);
    
    // Configuration comparison
    ConfigComparison compare(const Config& other) const;
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::Merged);
    
    // Configuration subset
    Config subset(const std::vector<std::string>& keys) const;
    
    // Configuration filtering
    Config filter(const std::function<bool(const ConfigEntry&)>& predicate) const;
    
    // Configuration transformation
    Config transform(const std::function<ConfigValue(const ConfigEntry&)>& transform) const;
    
    // Configuration conversion
    std::string toJSON() const;
    std::string toString() const;
    
    // Configuration cloning
    Config clone() const;
    
    // Configuration serialization
    bool saveToArchive(const std::string& filepath) const;
    bool loadFromArchive(const std::string& filepath);
    
    // Configuration backup
    bool createBackup(const std::string& name) const;
    bool restoreBackup(const std::string& name);
    
    // Configuration history
    std::vector<Config> getHistory() const;
    void addToHistory();
    
    // Configuration statistics
    std::string getStatisticsReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string extractNestedKey(const std::string& fullKey);
    static std::string extractParentKey(const std::string& fullKey);
    static std::string extractFieldKey(const std::string& fullKey);
    static bool isNestedKey(const std::string& key);
    
    // Validation helpers
    ValidationResult validateTypes() const;
    ValidationResult validateRanges() const;
    ValidationResult validateDependencies() const;
    ValidationResult validateConsistency() const;
    
    // Template management
    static void initializeBuiltInTemplates();
    static std::unordered_map<std::string, ConfigTemplate> builtInTemplates;
};

} // namespace nlm
