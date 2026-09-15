#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Forward declarations
class Config;
class JSONConfigParser;
class YAMLConfigParser;

// Configuration validation
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    virtual bool validate(const std::string& key, const ConfigValue& value) const = 0;
    virtual std::string getErrorMessage(const std::string& key, const ConfigValue& value) const = 0;
};

// Configuration inheritance
class ConfigInheritance {
public:
    virtual ~ConfigInheritance() = default;
    virtual bool hasParent() const = 0;
    virtual std::shared_ptr<Config> getParent() const = 0;
    virtual std::string getMergeStrategy() const = 0;
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
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from file with format specification
    bool loadFromFile(const std::string& filepath, const std::string& format);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Save to file with format specification
    bool saveToFile(const std::string& filepath, const std::string& format) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Set with validation
    bool setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by prefix
    std::vector<std::string> getKeys(const std::string& prefix) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Configuration management
    bool mergeFrom(const Config& other);
    bool inheritFrom(const Config& parent);
    
    // Configuration validation
    void addValidator(std::shared_ptr<ConfigValidator> validator);
    void removeValidator(const std::string& key);
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    std::map<ConfigSource, size_t> getSourceCounts() const;
    double getMemoryUsage() const;
    
    // File format detection
    static std::string detectFormat(const std::string& filepath);
    static bool isFormatSupported(const std::string& format);
    
    // Configuration comparison
    bool equals(const Config& other) const;
    size_t hash() const;
    
    // Persistent configuration
    bool saveToRegistry(const std::string& keyPrefix = "NLM_") const;
    bool loadFromRegistry(const std::string& keyPrefix = "NLM_");
    
    // Configuration export/import
    std::string exportToString(const std::string& format = "json") const;
    bool importFromString(const std::string& data, const std::string& format = "json");
    
    // Configuration diff
    std::vector<std::pair<std::string, std::pair<ConfigValue, ConfigValue>>> diff(const Config& other) const;
    
    // Configuration templates
    bool loadTemplate(const std::string& templateName);
    std::vector<std::string> getAvailableTemplates() const;
    
    // Configuration presets
    bool saveAsPreset(const std::string& presetName);
    bool loadPreset(const std::string& presetName);
    std::vector<std::string> getAvailablePresets() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringEndsWith(const std::string& str, const std::string& suffix);
    
    // JSON/YAML parser helpers
    static bool isValidJSON(const std::string& jsonStr);
    static bool isValidYAML(const std::string& yamlStr);
    
    // Configuration validation helpers
    bool validateEntry(const ConfigEntry& entry) const;
    static bool isValidKey(const std::string& key);
    static bool isValidType(const ConfigValue& value);
    
    // Configuration comparison helpers
    static bool valuesEqual(const ConfigValue& a, const ConfigValue& b);
    static size_t hashValue(const ConfigValue& value);
};
