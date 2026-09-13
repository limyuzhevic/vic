// NLM Configuration management system
// Handles loading and saving of brain configuration parameters

#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include "core/Types/Types.hpp"

namespace nlm {

// Configuration value types
enum class ConfigType {
    Integer,
    Float,
    String,
    Boolean,
    Unknown
};

// Configuration sources for value precedence
enum class ConfigSource {
    Default,    // Default/hardcoded values
    Runtime,    // Set at runtime
    File,       // Loaded from file
    CommandLine // Set via command line
};

// Individual configuration entry
struct ConfigEntry {
    std::string key;
    ConfigType type;
    ConfigSource source;
    
    // Union for different value types
    union {
        int64_t intValue;
        float floatValue;
        std::string stringValue;
        bool boolValue;
    };
    
    ConfigEntry() : type(ConfigType::Unknown), source(ConfigSource::Default) {}
    
    // Helper methods for type checking
    bool isInteger() const { return type == ConfigType::Integer; }
    bool isFloat() const { return type == ConfigType::Float; }
    bool isString() const { return type == ConfigType::String; }
    bool isBoolean() const { return type == ConfigType::Boolean; }
    
    // Get value as specific type with defaults
    int64_t getAsInt(int64_t defaultValue = 0) const;
    float getAsFloat(float defaultValue = 0.0f) const;
    const std::string& getAsString(const std::string& defaultValue = "") const;
    bool getAsBoolean(bool defaultValue = false) const;
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
    
    // Set a configuration value
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Get a configuration value with default
    int64_t getOr(const std::string& key, int64_t defaultValue = 0) const;
    float getOr(const std::string& key, float defaultValue = 0.0f) const;
    const std::string& getOr(const std::string& key, const std::string& defaultValue = "") const;
    bool getOr(const std::string& key, bool defaultValue = false) const;
    
    // Get value by type (template-based)
    template<typename T>
    T get(const std::string& key) const;
    
    // Check if a key exists
    bool has(const std::string& key) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get source of a configuration value
    ConfigSource getSource(const std::string& key) const;
    
    // Reset configuration to defaults
    void reset();
    
    // Merge another config into this one
    void merge(const Config& other, bool overwrite = true);
    
    // Get configuration size
    size_t size() const { return entries.size(); }
    
    // Check if empty
    bool empty() const { return entries.empty(); }
    
    // Clear all entries
    void clear() { entries.clear(); }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm