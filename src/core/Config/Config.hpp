#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <stdexcept>

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

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    
    ValidationResult(bool v = true, const std::string& msg = "") 
        : valid(v), errorMessage(msg) {}
    
    static ValidationResult success() { return ValidationResult(true); }
    static ValidationResult error(const std::string& msg) { return ValidationResult(false, msg); }
};

// Configuration constraint/validator function
using ConfigValidator = std::function<ValidationResult(const ConfigValue&)>;

// Configuration type information
struct ConfigTypeInfo {
    std::string name;
    std::string description;
    bool required;
    std::optional<ConfigValue> defaultValue;
    ConfigValidator validator;
    
    ConfigTypeInfo(const std::string& n, const std::string& desc, bool req = false,
                   const std::optional<ConfigValue>& def = std::nullopt,
                   const ConfigValidator& val = nullptr)
        : name(n), description(desc), required(req), defaultValue(def), validator(val) {}
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

// Configuration class
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
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with validation
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
    
    // Configuration schema management
    void registerKey(const std::string& key, const std::string& description, bool required = false,
                    const std::optional<ConfigValue>& defaultValue = std::nullopt,
                    const ConfigValidator& validator = nullptr);
    void validateAll() const;
    bool isValid(const std::string& key) const;
    
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
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringToBool(const std::string& str);
    static double stringToDouble(const std::string& str);
    static int stringToInt(const std::string& str);
    static int64_t stringToInt64(const std::string& str);
};

} // namespace nlm
