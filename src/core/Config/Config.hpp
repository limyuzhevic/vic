#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <variant>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types - extended for better type support
using ConfigValue = std::variant<
    int,
    int64_t,
    uint64_t,
    double,
    float,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<uint64_t>,
    std::vector<double>,
    std::vector<float>,
    std::vector<std::string>
>;

// Enhanced error types for configuration operations
enum class ConfigError {
    Success,
    FileNotFound,
    ParseError,
    InvalidFormat,
    PermissionDenied,
    InvalidType,
    KeyNotFound,
    MemoryError
};

// Configuration source with more detailed tracking
enum class ConfigSource {
    Default = 0,
    File = 1,
    CommandLine = 2,
    Runtime = 3,
    Environment = 4
};

// Enhanced configuration entry with validation
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    mutable ConfigError lastError;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), lastError(ConfigError::Success) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), lastError(ConfigError::Success) {}
    
    // Type checking helper
    template<typename T>
    bool isType() const {
        return std::holds_alternative<T>(value);
    }
    
    // Type conversion helper
    template<typename T>
    std::optional<T> getTypedValue() const {
        if (std::holds_alternative<T>(value)) {
            return std::get<T>(value);
        }
        return std::nullopt;
    }
};

// Main configuration class with enhanced functionality
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced file loading with better error handling
    [[nodiscard]] bool loadFromFile(const std::string& filepath);
    [[nodiscard]] bool loadFromFile(const std::string& filepath, ConfigSource source);
    
    // Enhanced command line parsing with better validation
    [[nodiscard]] bool loadFromArgs(int argc, char** argv);
    [[nodiscard]] bool loadFromArgs(int argc, char** argv, ConfigSource source);
    
    // Enhanced file saving with better formatting
    [[nodiscard]] bool saveToFile(const std::string& filepath) const;
    [[nodiscard]] bool saveToFile(const std::string& filepath, bool prettyPrint = false) const;
    
    // Enhanced value getting with type checking and error reporting
    template<typename T>
    [[nodiscard]] std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Enhanced value setting with type inference
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, uint64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Type-safe setters with compile-time type checking
    template<typename T>
    void setTyped(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced existence checking
    [[nodiscard]] bool has(const std::string& key) const;
    
    // Remove and batch operations
    void remove(const std::string& key);
    void removeAll(); // Clear all entries
    
    // Get all keys with optional filtering
    [[nodiscard]] std::vector<std::string> getKeys() const;
    [[nodiscard]] std::vector<std::string> getKeys(ConfigSource source) const;
    [[nodiscard]] std::vector<std::string> getKeys(const std::string& prefix) const;
    
    // Configuration management
    void clear();
    void clear(ConfigSource source);
    
    // Get configuration summary with detailed information
    [[nodiscard]] std::string summary() const;
    [[nodiscard]] std::string summary(ConfigSource source) const;
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Validation and integrity checks
    [[nodiscard]] bool validate() const;
    [[nodiscard]] std::string validationErrors() const;
    
    // Statistical information
    [[nodiscard]] size_t entryCount() const;
    [[nodiscard]] size_t entryCount(ConfigSource source) const;
    
    // Configuration cloning
    [[nodiscard]] Config clone(ConfigSource source = ConfigSource::Runtime) const;
    
    // Constants and defaults
    static constexpr const char* DEFAULT_CONFIG_FILE = "config.json";
    static constexpr const char* SIMPLE_CONFIG_FILE = "config.txt";
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers with enhanced error handling
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool isNumeric(const std::string& str, double* value = nullptr);
    static bool isInteger(const std::string& str, int64_t* value = nullptr);
    static bool isBoolean(const std::string& str, bool* value = nullptr);
    
    // Enhanced JSON parsing support
#ifdef HAVE_NLOHMANN_JSON
    [[nodiscard]] bool loadFromJSON(const std::string& filepath, ConfigSource source);
    void exportToJSON(nlohmann::json& j, ConfigSource source) const;
#endif
    
    // Simple format parsing with better error handling
    [[nodiscard]] bool loadFromSimpleFormat(const std::string& filepath, ConfigSource source);
    [[nodiscard]] bool saveToSimpleFormat(std::ostream& stream, ConfigSource source) const;
    
    // Helper functions for type conversion
    static bool parseValue(const std::string& str, ConfigValue& value);
    static std::string formatValue(const ConfigValue& value);
    
    // Validation helpers
    [[nodiscard]] static bool isValidKey(const std::string& key);
    [[nodiscard]] static ConfigError validateKey(const std::string& key);
    [[nodiscard]] static ConfigError validateValue(const ConfigValue& value);
    
    // Memory management helper
    void cleanup();
    
public:
    // Static factory methods for common use cases
    static Config createDefault();
    static Config createFromEnvironment();
    static Config createFromCommandLine(int argc, char** argv);
};
