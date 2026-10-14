#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <iomanip>

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
    std::vector<std::string>,
    std::map<std::string, ConfigValue>,
    std::vector<std::map<std::string, ConfigValue>>
>;

// Forward declarations
class Config;
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath, bool validate = true);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    
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
    
    // Type conversion utilities
    static std::optional<ConfigValue> stringToConfigValue(const std::string& str, const std::string& typeHint = "auto");
    static std::string configValueToString(const ConfigValue& value) const;
    static bool validateType(const ConfigValue& value, const std::string& typeName) const;
    
    // Config file validation
    static bool validateConfigFile(const std::string& filepath, const std::string& format = "json");
    static bool validateConfigValue(const ConfigValue& value, const std::string& key = "", const std::string& typeHint = "auto") const;
    
    // Merge strategies
    bool mergeFromFile(const std::string& filepath, bool overwriteFileSettings = false);
    bool mergeFromArgs(int argc, char** argv, bool overwriteCommandLine = true);
    Config mergeWith(const Config& other, ConfigSource source = ConfigSource::Runtime) const;
    
    // Get configuration source
    ConfigSource getSource(const std::string& key) const;
    
    // Get all entries from a specific source
    std::vector<ConfigEntry> getEntriesFromSource(ConfigSource source) const;
    
    // Check if value is default
    bool isDefault(const std::string& key) const;
    
    // Get validation errors
    std::vector<ConfigError> getValidationErrors() const;
    std::string getLastError() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string normalizeKey(const std::string& key);
    static bool parseJSON(const std::string& content, Config& config);
    static bool parseYAML(const std::string& content, Config& config);
    static std::string serializeToJSON(const Config& config) const;
    static std::string serializeToYAML(const Config& config) const;
    
    // Error handling
    void setError(ConfigError error, const std::string& message);
    void clearErrors();
};

} // namespace nlm