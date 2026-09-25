#pragma once
 
 #include <string>
 #include <memory>
 #include <vector>
 #include <variant>
 #include <optional>
 #include <filesystem>
 #include <stdexcept>
 #include <nlohmann/json.hpp>
 #include "Error.hpp"
 #include "Validation.hpp"
 
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
 
 // Enhanced configuration exception with proper inheritance
 class ConfigError : public nlm::ConfigError {
 public:
     explicit ConfigError(const std::string& message, 
                         const std::shared_ptr<nlm::NLMError>& cause = nullptr)
         : nlm::ConfigError(message, cause) {}
     
     ConfigError(const std::string& message, const std::exception& cause)
         : nlm::ConfigError(message, std::make_shared<nlm::NLMError>(
               "Wrapped exception: " + std::string(cause.what()), 
               std::dynamic_pointer_cast<nlm::NLMError>(
                   std::make_shared<std::runtime_error>(cause.what())))) {}
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
    bool loadFromFile(const std::string& filepath);
    
    // Load from file (backward compatibility - key=value format)
    bool loadFromFileSimple(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJsonString(const std::string& jsonString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON string
    std::string saveToJsonString() const;
    
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
    
    // Validate configuration with enhanced error reporting
    void validate() const;
    
    // Get validation errors with structured reporting
    std::vector<std::string> getValidationErrors() const;
    
    // Convert ConfigValue to JSON with error handling
    static nlohmann::json configValueToJson(const ConfigValue& value);
    
    // Convert JSON to ConfigValue with validation
    static std::optional<ConfigValue> jsonToConfigValue(const nlohmann::json& j);
    
    // Enhanced file I/O with proper error handling
    static bool checkFileExists(const std::string& filepath) noexcept;
    static bool ensureDirectoryExists(const std::string& filepath) noexcept;
    static std::string getCurrentTimestamp() noexcept;
    
    // Configuration schema validation
    static bool validateSchema(const std::string& key, const ConfigValue& value) noexcept;
    
    // Type conversion error handling
    template<typename T>
    static std::optional<T> safeGetValue(const ConfigValue& value) noexcept;
    
    // Helper functions for validation
    static bool isValidKey(const std::string& key) noexcept;
    static bool isValueWithinRange(const std::string& key, const ConfigValue& value) noexcept;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // File I/O error handling
    static bool checkFileExists(const std::string& filepath);
    static bool ensureDirectoryExists(const std::string& filepath);
};

} // namespace nlm
