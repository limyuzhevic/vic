#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <limits>

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
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    
    static ValidationResult success() { return {true, ""}; }
    static ValidationResult failure(const std::string& error) { return {false, error}; }
};

// Configuration validator function type
using ValidatorFunc = std::function<ValidationResult(const std::string&, const ConfigValue&)>;

// Configuration metadata
struct ConfigMetadata {
    std::string description;
    std::string category;
    bool required = false;
    std::string defaultValue;
    std::vector<std::string> allowedValues;
    double minValue = std::numeric_limits<double>::lowest();
    double maxValue = std::numeric_limits<double>::max();
    std::string units;
    ValidatorFunc validator;
    size_t minLength = 0;
    size_t maxLength = std::numeric_limits<size_t>::max();
    
    ConfigMetadata() : description(""), category("General"), required(false) {}
    ConfigMetadata(const std::string& desc, const std::string& cat = "General")
        : description(desc), category(cat), required(false) {}
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    ConfigMetadata metadata;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const ConfigMetadata& meta = ConfigMetadata())
        : key(k), value(v), source(s), metadata(meta) {}
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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath, bool validate = true);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv, bool validate = true);
    
    // Save to file (supports JSON/YAML)
    bool saveToFile(const std::string& filepath, bool pretty = false) const;
    
    // Get values with validation
    template<typename T>
    std::optional<T> get(const std::string& key, bool validate = false) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue, bool validate = false) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    
    // Configuration validation
    bool addValidator(const std::string& key, const ValidatorFunc& validator);
    ValidationResult validate(const std::string& key) const;
    std::vector<std::string> validateAll() const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get configuration by category
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Configuration management
    bool registerConfigOption(const std::string& key, const ConfigMetadata& metadata);
    ConfigMetadata getMetadata(const std::string& key) const;
    bool isRegistered(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseFileLine(const std::string& line, std::string& key, std::string& value);
    static std::string serializeValue(const ConfigValue& value);
    static std::optional<ConfigValue> parseValue(const std::string& str, const std::string& key);
    
    // Validation helpers
    static ValidationResult validateNumeric(const std::string& key, const ConfigValue& value, double min, double max);
    static ValidationResult validateEnum(const std::string& key, const ConfigValue& value, const std::vector<std::string>& allowed);
    static ValidationResult validateString(const std::string& key, const ConfigValue& value, size_t minLength, size_t maxLength);
};

} // namespace nlm
