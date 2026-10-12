#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <stdexcept>

// Forward declaration for JSON support
namespace nlohmann {
    template<typename T> struct adl_serializer;
}

namespace nlm {

// Forward declarations
class Config;
class ConfigValidator;

// Configuration value types (extended)
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<bool>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool required;
    std::string validationRegex;
    std::string validationErrorMessage;
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default)
        , description(), required(false), validationRegex()
        , validationErrorMessage(), validator(nullptr) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s,
                const std::string& desc = "", bool req = false)
        : key(k), value(v), source(s), description(desc)
        , required(req), validationRegex(), validationErrorMessage()
        , validator(nullptr) {}
    
    // Validate this entry
    bool validate() const {
        if (validator) {
            return validator(value);
        }
        if (!validationRegex.empty()) {
            // Simple regex validation (would use regex library in production)
            return true; // Placeholder
        }
        return true;
    }
    
    // Get value as specific type with type safety
    template<typename T>
    T as() const {
        try {
            return std::get<T>(value);
        } catch (const std::bad_variant_access&) {
            throw std::runtime_error("Invalid type conversion for key: " + key);
        }
    }
    
    // Get value as string representation
    std::string toString() const {
        std::string result;
        std::visit([&result](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, bool>) {
                result = val ? "true" : "false";
            } else {
                result = std::to_string(val);
            }
        }, value);
        return result;
    }
};

// Configuration validation interface
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    virtual bool validate(const ConfigEntry& entry) const = 0;
    virtual std::string getErrorMessage(const ConfigEntry& entry) const = 0;
};

// Default validator for basic types
class DefaultValidator : public ConfigValidator {
public:
    bool validate(const ConfigEntry& entry) const override {
        return entry.validate();
    }
    
    std::string getErrorMessage(const ConfigEntry& entry) const override {
        return "Validation failed for key: " + entry.key;
    }
};

// Type-safe configuration getters
class TypedConfigGetter {
public:
    template<typename T>
    static std::optional<T> get(const std::vector<ConfigEntry>& entries, const std::string& key) {
        auto it = std::find_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == entries.end()) {
            return std::nullopt;
        }
        
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    template<typename T>
    static T getOr(const std::vector<ConfigEntry>& entries, const std::string& key, const T& defaultValue) {
        auto val = get<T>(entries, key);
        return val.has_value() ? val.value() : defaultValue;
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
    
    // Load from file (JSON/YAML support)
    bool loadFromFile(const std::string& filepath);
    
    // Load from YAML file
    bool loadFromYamlFile(const std::string& filepath);
    
    // Load from JSON file
    bool loadFromJsonFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to YAML file
    bool saveToYamlFile(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        return TypedConfigGetter::get<T>(pImpl->entries, key);
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        return TypedConfigGetter::getOr<T>(pImpl->entries, key, defaultValue);
    }
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", bool required = false);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", bool required = false);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", bool required = false);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", bool required = false);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", bool required = false);
    
    // Set with validator
    void set(const std::string& key, const ConfigValue& value, ConfigSource source,
             const std::string& description, bool required,
             std::function<bool(const ConfigValue&)> validator);
    
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
    
    // Validate all entries
    std::vector<std::string> validateAll() const;
    
    // Get source of a key
    ConfigSource getSource(const std::string& key) const;
    
    // Check if configuration was overridden
    bool isOverridden(const std::string& key) const;
    
    // Merge with another config (command line overrides file)
    void merge(const Config& other);
    
    // Get entry count
    size_t getEntryCount() const { return pImpl->entries.size(); }
    
    // Set validator for a specific key
    void setValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator);
    
    // Add default values for missing keys
    void applyDefaults(const std::vector<std::pair<std::string, ConfigValue>>& defaults);
    
    // Get raw entries (internal use)
    const std::vector<ConfigEntry>& getEntries() const { return pImpl->entries; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    std::string trim(const std::string& str) const;
    bool parseLine(const std::string& line, std::string& key, std::string& value) const;
    bool loadFromStream(std::istream& stream);
    bool saveToStream(std::ostream& stream) const;
    
    // JSON/YAML helpers
    bool loadFromJson(const nlohmann::json& json);
    nlohmann::json saveToJson() const;
    
    // Validation helpers
    bool validateEntry(const ConfigEntry& entry) const;
    std::string getValidationError(const ConfigEntry& entry) const;
};