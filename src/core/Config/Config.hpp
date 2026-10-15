// Configuration schema for validation and documentation
struct ConfigSchema {
    std::string key;
    std::string type;  // "int", "double", "string", "bool", "int_array", "double_array", "string_array"
    ConfigValue defaultValue;
    std::string description;
    std::vector<std::string> allowedValues;
    
    ConfigSchema() : key(), type(), defaultValue(), description() {}
    ConfigSchema(const std::string& k, const std::string& t, const ConfigValue& dv, 
                 const std::string& desc = "", const std::vector<std::string>& av = {})
        : key(k), type(t), defaultValue(dv), description(desc), allowedValues(av) {}
};

class ConfigSchemaValidator {
public:
    static std::vector<ConfigSchema> getDefaultSchema();
    static bool validate(const ConfigSchema& schema, const std::string& key, const ConfigValue& value);
    static std::string valueToString(const ConfigValue& value);
    static bool stringToValue(const std::string& type, const std::string& str, ConfigValue& outValue);
};

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
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from arguments with type validation
    bool loadFromArgsWithValidation(int argc, char** argv);
    
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
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
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
    
    // Help and validation
    std::string help() const;
    bool validate() const;
    std::string getUsage() const;
    
    // Schema validation
    static std::vector<ConfigSchema> getDefaultSchema();
    bool validateAgainstSchema(const ConfigSchema& schema) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
