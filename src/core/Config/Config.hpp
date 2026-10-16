#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>

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
    Runtime,
    Environment
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string filePath;  // Track which file loaded this value
    
    ConfigEntry() : source(ConfigSource::Default) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& file = "")
        : key(k), value(v), source(s), description(desc), filePath(file) {}
};

// Simple configuration schema for validation
struct ConfigSchema {
    std::string key;
    std::string type;  // "int", "float", "bool", "string", "list"
    std::string description;
    std::optional<ConfigValue> defaultValue;
    bool required = false;
    
    ConfigSchema() = default;
    ConfigSchema(const std::string& k, const std::string& t, const std::string& desc = "",
                const std::optional<ConfigValue>& def = std::nullopt, bool req = false)
        : key(k), type(t), description(desc), defaultValue(def), required(req) {}
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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from string content (JSON/YAML)
    bool loadFromString(const std::string& content, const std::string& format = "auto");
    
    // Load from environment variables
    bool loadFromEnv(const std::map<std::string, std::string>& envVars);
    
    // Save to file
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    std::string saveToString(const std::string& format = "json") const;
    
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
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
    // Clear all
    void clear();
    
    // Statistics
    size_t getEntryCount() const;
    
    // Get configuration summary
    std::string summary() const;
    
    // Schema support (for validation)
    void addSchema(const ConfigSchema& schema);
    bool removeSchema(const std::string& key);
    void clearSchemas();
    bool hasSchema(const std::string& key) const;
    bool validate() const;
    bool validate(const std::string& key) const;
    bool isValid(const std::string& key) const;
    std::string getValidationError(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
