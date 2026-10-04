#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>
#include <algorithm>

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

// Configuration validation function type
using ValidationFunction = std::function<bool(const ConfigValue&)>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Validation error types
enum class ValidationErrorType {
    InvalidType,
    OutOfRange,
    RequiredValueMissing,
    InvalidFormat,
    MissingRequiredValue
};

struct ValidationError {
    std::string key;
    ValidationErrorType type;
    std::string message;
    std::string expectedFormat;
    
    ValidationError() : key(), type(ValidationErrorType::InvalidType), message(), expectedFormat() {}
    ValidationError(const std::string& k, ValidationErrorType t, const std::string& msg, const std::string& fmt = "")
        : key(k), type(t), message(msg), expectedFormat(fmt) {}
};

// Enhanced configuration entry with type information
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    // Type information for validation
    std::type_index valueType;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), valueType(typeid(void)) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "",
                const std::type_index& type = typeid(void))
        : key(k), value(v), source(s), description(desc), valueType(type) {}
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
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file with improved format support
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with better type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Type-safe setters
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Type-safe getters with validation
    template<typename T>
    bool getValidated(const std::string& key, T& out) const;
    
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
    
    // Validation and schema management
    void addValidator(const std::string& key, ValidationFunction validator);
    void addValidator(const std::string& key, ValidationFunction validator, const std::string& errorMessage);
    bool validate() const;
    std::vector<ValidationError> getValidationErrors() const;
    
    // Enhanced file loading with JSON support
    bool loadFromFileEnhanced(const std::string& filepath, bool validateOnLoad = true);
    
    // Command line argument parsing with validation
    bool parseCommandLineArguments(int argc, char** argv);
    
    // Configuration reset with source tracking
    void resetToDefaults();
    void resetToSource(ConfigSource source);
    
    // Get configuration by source
    std::vector<ConfigEntry> getEntriesBySource(ConfigSource source) const;
    
    // Search configuration
    std::vector<ConfigEntry> search(const std::string& pattern) const;
    
    // Export/import configuration
    std::string exportToJSON() const;
    bool importFromJSON(const std::string& jsonString);
    
    // Configuration statistics
    size_t getEntryCount() const;
    bool isEmpty() const;
    
protected:
    // Internal validation helpers
    static bool validateInt(const ConfigValue& value, int min, int max);
    static bool validateDouble(const ConfigValue& value, double min, double max);
    static bool validateString(const ConfigValue& value, size_t minLength, size_t maxLength);
    static bool validateBool(const ConfigValue& value);
    
    // Error message helpers
    static std::string getDefaultErrorMessage(const std::string& key, const std::type_index& expectedType);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string convertToString(const ConfigValue& value);
    static std::type_index getValueTypeIndex(const ConfigValue& value);
};

// Utility functions
inline std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

inline std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace nlm
