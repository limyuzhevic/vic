#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <typeinfo>
#include <sstream>
#include <chrono>
#include <ctime>

namespace nlm {

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

// Configuration schema definition
struct ConfigSchemaEntry {
    std::string key;
    std::type_index type;
    ConfigSource source;
    std::string description;
    std::string defaultValue;
    bool required;
    
    ConfigSchemaEntry(const std::string& k, const std::type_index& t, ConfigSource s,
                     const std::string& desc, const std::string& def, bool req)
        : key(k), type(t), source(s), description(desc), defaultValue(def), required(req) {}
};

// Schema validation result
struct SchemaValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    SchemaValidationResult() : valid(true) {}
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Main configuration class with improvements
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
    
    // Save to file (now saves actual values)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Advanced set with type checking
    template<typename T>
    bool setTyped(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Schema management
    void addSchema(const std::string& key, const std::type_index& type, ConfigSource source,
                   const std::string& description, const std::string& defaultValue = "", 
                   bool required = false);
    bool hasSchema(const std::string& key) const;
    SchemaValidationResult validateSchema() const;
    
    // Command line parsing improvements
    struct CommandLineOption {
        std::string flag;
        std::string longFlag;
        std::string description;
        std::string type; // "int", "float", "string", "bool", "double"
        std::string defaultValue;
        bool required;
        bool hasArg;
    };
    
    // Enhanced help and version
    void printHelp() const;
    std::string getVersion() const;
    static std::string getVersionString();
    
    // Configuration statistics
    size_t getEntryCount() const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
    // File format support
    static bool isValidJsonFile(const std::string& filepath);
    static bool isValidYamlFile(const std::string& filepath);
    static std::string detectFileFormat(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringToBool(const std::string& str, bool& result);
    static int stringToInt(const std::string& str, int& result);
    static double stringToDouble(const std::string& str, double& result);
    
    // Schema validation helpers
    template<typename T>
    bool validateValueType(const ConfigValue& value) const;
    std::string typeIndexToString(const std::type_index& type) const;
    
    // Command line parsing helpers
    static std::vector<std::string> parseCommandLineArgs(int argc, char** argv);
    static std::string normalizeFlag(const std::string& flag);
    
    // File format helpers
    static bool parseJsonFile(const std::string& filepath, std::vector<ConfigEntry>& entries);
    static bool parseSimpleFile(const std::string& filepath, std::vector<ConfigEntry>& entries);
    static bool writeJsonFile(const std::string& filepath, const std::vector<ConfigEntry>& entries) const;
    static bool writeSimpleFile(const std::string& filepath, const std::vector<ConfigEntry>& entries) const;
};