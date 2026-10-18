// Forward declarations
class Config;
class ConfigManager;
class ConfigChangeListener;

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
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

// Advanced configuration manager with validation, schema, and monitoring
class ConfigManager;

// Configuration change notification
class ConfigChangeListener {
public:
    virtual ~ConfigChangeListener() = default;
    virtual void onConfigChanged(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) = 0;
};

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    ConfigValue defaultValue;
    ConfigSource source;
    std::string description;
    bool required;
    
    ConfigSchema() : source(ConfigSource::Default), required(false) {}
    ConfigSchema(const std::string& k, const ConfigValue& v, 
                 const std::string& desc = "", bool req = false)
        : key(k), defaultValue(v), source(ConfigSource::Default), 
          description(desc), required(req) {}
    ConfigSchema(const std::string& k, const std::string& v,
                 const std::string& desc = "", bool req = false)
        : key(k), defaultValue(v), source(ConfigSource::Default),
          description(desc), required(req) {}
    ConfigSchema(const std::string& k, double v,
                 const std::string& desc = "", bool req = false)
        : key(k), defaultValue(v), source(ConfigSource::Default),
          description(desc), required(req) {}
    ConfigSchema(const std::string& k, int v,
                 const std::string& desc = "", bool req = false)
        : key(k), defaultValue(v), source(ConfigSource::Default),
          description(desc), required(req) {}
    ConfigSchema(const std::string& k, bool v,
                 const std::string& desc = "", bool req = false)
        : key(k), defaultValue(v), source(ConfigSource::Default),
          description(desc), required(req) {}
};

// Helper functions for configuration validation
namespace config {
    // Validation functions
    bool isNonEmptyString(const ConfigValue& value);
    bool isPositiveInt(const ConfigValue& value);
    bool isPositiveDouble(const ConfigValue& value);
    bool isBoolean(const ConfigValue& value);
    bool isWithinRange(const ConfigValue& value, double min, double max);
    bool matchesPattern(const ConfigValue& value, const std::string& pattern);
    
    // Type conversion utilities
    std::string toString(const ConfigValue& value);
    std::string formatValue(const ConfigValue& value);
    
    // Configuration utilities
    std::vector<std::string> getConfigKeysWithPrefix(const Config& config, const std::string& prefix);
    void stripPrefixFromKeys(Config& config, const std::string& prefix);
    
    // Command line parsing
    struct ParsedArgs {
        std::vector<std::string> unknownArgs;
        std::vector<std::pair<std::string, std::string>> knownArgs;
    };
    
    ParsedArgs parseAdvancedArgs(int argc, char** argv, 
                                 const std::vector<std::string>& knownKeys);
}

} // namespace nlm
