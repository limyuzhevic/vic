#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <any>
#include <map>

// Include the original Config for ConfigSource enum
#include "Config.hpp"

namespace nlm {

// Type aliases for common config types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    float,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<float>,
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

// Configuration parameter schema
struct ConfigSchemaEntry {
    std::string key;
    std::string type;
    std::any defaultValue;
    std::string description;
    std::function<ValidationResult(const std::any&)> validator;
    bool required;
    
    ConfigSchemaEntry(const std::string& k, const std::string& t, 
                      const std::any& def = nullptr, const std::string& desc = "",
                      std::function<ValidationResult(const std::any&)> val = nullptr,
                      bool req = false)
        : key(k), type(t), defaultValue(def), description(desc), 
          validator(val), required(req) {}
};

// File format enum
enum class ConfigFormat {
    Unknown,
    Ini,
    PlainText
};

class ConfigImproved {
public:
    ConfigImproved();
    ~ConfigImproved();
    
    // Disable copying, enable moving
    ConfigImproved(const ConfigImproved&) = delete;
    ConfigImproved& operator=(const ConfigImproved&) = delete;
    ConfigImproved(ConfigImproved&&) noexcept;
    ConfigImproved& operator=(ConfigImproved&&) noexcept;
    
    // Set schema for validation
    void setSchema(const std::vector<ConfigSchemaEntry>& schema);
    
    // Load from file (INI format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from string (INI format)
    bool loadFromString(const std::string& content);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (INI format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type checking
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
    
    // Configuration validation
    ValidationResult validate(const std::string& key) const;
    ValidationResult validateAll() const;
    
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
    
    // Helper for CLI parsing
    static std::string generateUsage(const std::string& programName, 
                                     const std::vector<ConfigSchemaEntry>& schema);
    
    // Detect file format
    static ConfigFormat detectFormat(const std::string& filepath);
    
    // Common configuration helper functions
    static std::string formatValue(const ConfigValue& value);
    static ConfigValue parseValue(const std::string& str, const std::string& typeHint = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration validation helper
    ValidationResult validateValue(const std::string& key, const ConfigValue& value) const;
    
    // Utility functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringToBool(const std::string& str);
    static std::string boolToString(bool value);
    static std::string formatAnyToString(const std::any& value);
    
    // INI format parsing
    bool parseIniFormat(const std::string& content);
    bool parseIniFormat(const std::string& filepath);
};

} // namespace nlm