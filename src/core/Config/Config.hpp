#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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

// Configuration options for predefined schemas
enum class ConfigSchema {
    Phase1,      // Phase 1 skeleton
    Phase2,      // Phase 2 neural computation
    Phase3,      // Phase 3 world interaction
    Phase4,      // Phase 4 cognition
    Phase5,      // Phase 5 performance
    Phase6       // Phase 6 final integration
};

// Configuration file format
enum class FileFormat {
    JSON,
    YAML,
    KeyValue
};

// Configuration schema (backwards compatibility)
enum class Schema {
    Phase1,
    Phase2,
    Phase3,
    Phase4,
    Phase5,
    Phase6
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

// Main configuration class
class Config {
public:
    enum class FileFormat {
        JSON,
        YAML,
        KeyValue
    };
    
    enum class Schema {
        Phase1,
        Phase2,
        Phase3,
        Phase4,
        Phase5,
        Phase6
    };
    
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
    
    // Configuration schema operations
    void applySchema(ConfigSchema schema);
    void applySchema(Schema schema);
    
    // Configuration management
    void clear();
    void validateConfig() const;
    void mergeConfig(const Config& other);
    
    // Configuration summary and debugging
    std::string summary() const;
    
    // Helper methods
    std::string getFileExtension(const std::string& filepath) const;
    Config::FileFormat detectFileFormat(const std::string& filepath) const;
    std::string trim(const std::string& str) const;
    std::string toLower(const std::string& str) const;
    ConfigSchema stringToSchema(const std::string& schemaStr) const;
    
    // Configuration entry creation
    ConfigEntry createEntryFromSchema(const std::string& key, const ConfigValue& value, Schema schema) const;
    
    // Debug and information
    void showHelp() const;
    void showVersion() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal implementation details
    bool loadFromKeyValue(const std::string& content);
    bool loadFromJSON(const std::string& content);
    bool loadFromYAML(const std::string& content);
    
    void applyPhase1Schema();
    void applyPhase2Schema();
    void applyPhase3Schema();
    void applyPhase4Schema();
    void applyPhase5Schema();
    void applyPhase6Schema();
    
    std::string getPhase1Description(const std::string& key) const;
    std::string getPhase2Description(const std::string& key) const;
    std::string getPhase3Description(const std::string& key) const;
    std::string getPhase4Description(const std::string& key) const;
    std::string getPhase5Description(const std::string& key) const;
    std::string getPhase6Description(const std::string& key) const;
    
    // Configuration state
    bool verboseMode = false;
    bool batchMode = false;
    ConfigSchema currentSchema = ConfigSchema::Phase6;
    std::string logFile;
    std::string configFormat;
    
    // Backwards compatibility
    static constexpr Schema deprecatedSchema = Schema::Phase2;
};

} // namespace nlm
