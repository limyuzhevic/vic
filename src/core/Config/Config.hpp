#include <stdexcept>
#include <system_error>

namespace nlm {

// Custom exceptions
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message) 
        : std::runtime_error("Config Error: " + message) {}
};

class ConfigFileNotFoundException : public ConfigException {
public:
    explicit ConfigFileNotFoundException(const std::string& filepath)
        : ConfigException("File not found: " + filepath) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message)
        : ConfigException("Validation Error: " + message) {}
};

class ConfigParseException : public ConfigException {
public:
    explicit ConfigParseException(const std::string& message)
        : ConfigException("Parse Error: " + message) {}
};

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

// Configuration statistics
struct ConfigStats {
    size_t totalEntries = 0;
    std::map<int, size_t> entriesBySource;
    std::map<std::string, size_t> valuesByType;
    
    ConfigStats() : totalEntries(0) {}
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
    
    // Validation methods
    bool validate() const;
    bool hasNonDefaultValues() const;
    bool validateKey(const std::string& key) const;
    std::vector<std::string> getValidatedKeys() const;
    
    // Config entry management
    void addEntry(const std::string& key, const ConfigValue& value,
                 const std::string& description = "",
                 ConfigSource source = ConfigSource::Runtime);
    std::vector<ConfigEntry> getAllEntries() const;
    
    // Configuration statistics
    ConfigStats getStats() const;
    
    // Serialization
    std::string serialize(const std::string& format = "") const;
    
    // Legacy methods
    bool loadFromJSON(const std::string& filepath);
    bool saveToSimpleFormat(const std::string& filepath) const;
    bool saveToJSON(const std::string& filepath) const;
    
    // Enhanced Python-style API methods (for bindings)
    bool isLoaded() const { return hasNonDefaultValues(); }
    size_t entryCount() const { return getKeys().size(); }
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        return getOr<T>(key, defaultValue);
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
