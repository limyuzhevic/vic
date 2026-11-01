#ifndef NLM_CONFIG_HPP
#define NLM_CONFIG_HPP

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <ctime>

// JSON library - use nlohmann/json if available, otherwise simple implementation
#ifdef HAS_NLOHMANN_JSON
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#else
// Simple JSON implementation for systems without nlohmann/json
namespace nlm {
    struct JsonValue;
    class JsonParser {
    public:
        static bool parse(const std::string& str, JsonValue& out);
        static std::string serialize(const JsonValue& val);
    };
    
    struct JsonValue {
        enum class Type { Object, Array, String, Number, Boolean, Null };
        Type type;
        std::string stringValue;
        double numberValue;
        bool boolValue;
        std::vector<JsonValue> arrayValue;
        std::vector<std::pair<std::string, JsonValue>> objectValue;
        
        JsonValue() : type(Type::Null) {}
        explicit JsonValue(const std::string& s) : type(Type::String), stringValue(s) {}
        explicit JsonValue(double n) : type(Type::Number), numberValue(n) {}
        explicit JsonValue(bool b) : type(Type::Boolean), boolValue(b) {}
        explicit JsonValue(const std::vector<JsonValue>& arr) : type(Type::Array), arrayValue(arr) {}
        explicit JsonValue(const std::vector<std::pair<std::string, JsonValue>>& obj) : type(Type::Object), objectValue(obj) {}
    };
}
#endif

namespace nlm {

class Config;

// Enhanced configuration value types
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
    std::vector<bool>,
    std::vector<std::string>,
    std::vector<std::vector<double>>  // For matrix values
>;

// Enhanced configuration source with timestamps
enum class ConfigSource {
    Default = 0,
    File = 1,
    CommandLine = 2,
    Runtime = 3,
    Environment = 4  // Environment variable source
};

struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;  // When entry was set
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                    timestamp(std::chrono::system_clock::now()) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), 
          timestamp(std::chrono::system_clock::now()) {}
};

// Callback for configuration changes
using ConfigChangeCallback = std::function<void(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue)>;

// Main enhanced configuration class
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
    bool loadFromFile(const std::string& filepath, bool validateSchema = true);
    
    // Load from YAML file
    bool loadFromYaml(const std::string& filepath, bool validateSchema = true);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_", bool validateKeys = true);
    
    // Load from command line arguments (enhanced)
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file with format support
    bool saveToFile(const std::string& filepath, bool prettyPrint = false) const;
    bool saveToJson(const std::string& filepath, bool prettyPrint = false) const;
    bool saveToYaml(const std::string& filepath, bool prettyPrint = false) const;
    
    // Get values with enhanced type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get value with validation
    template<typename T>
    bool tryGet(const std::string& key, T& out) const;
    
    // Set values with callbacks
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, int64_t value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, double value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, bool value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    
    // Enhanced validation and schema support
    bool validate() const;
    bool validateKey(const std::string& key) const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    
    // Get configuration metadata
    ConfigSource getSource(const std::string& key) const;
    std::string getDescription(const std::string& key) const;
    std::chrono::system_clock::time_point getTimestamp(const std::string& key) const;
    std::string getSourceString(ConfigSource source) const;
    
    // Check existence and comparison
    bool has(const std::string& key) const;
    bool hasKey(const std::string& key) const { return has(key); }
    
    // Remove key with callback support
    void remove(const std::string& key);
    
    // Get all keys (sorted)
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getSortedKeys() const;
    
    // Get all entries for inspection
    std::vector<ConfigEntry> getAllEntries() const;
    
    // Clear all configuration
    void clear();
    
    // Get configuration summary with enhanced info
    std::string summary() const;
    std::string summary(bool includeSources) const;
    std::string summary(bool includeSources, bool includeTimestamps) const;
    
    // Configuration change tracking
    void addChangeCallback(const ConfigChangeCallback& callback);
    void removeChangeCallback(const ConfigChangeCallback& callback);
    void clearChangeCallbacks();
    
    // Copy configuration
    std::shared_ptr<Config> clone() const;
    
    // Merge another configuration
    void merge(const Config& other, bool overwrite = false);
    
    // Validate configuration against rules
    struct ValidationRule {
        std::string key;
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
    };
    
    bool validateRules(const std::vector<ValidationRule>& rules) const;
    
    // Configuration group support
    void beginGroup(const std::string& groupName);
    void endGroup();
    bool inGroup() const;
    std::string getCurrentGroup() const;
    std::string getFullKey(const std::string& key) const;
    
    // Profile/save/load support
    bool saveProfile(const std::string& name, const std::string& directory = "profiles");
    bool loadProfile(const std::string& name, const std::string& directory = "profiles");
    std::vector<std::string> getAvailableProfiles() const;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool stringToBool(const std::string& str);
    static double stringToDouble(const std::string& str);
    static int64_t stringToInt64(const std::string& str);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Group management
    std::vector<std::string> groupStack;
    
    // Configuration value serialization/deserialization
    static JsonValue configValueToJson(const ConfigValue& value);
    static bool jsonToConfigValue(const JsonValue& json, ConfigValue& value);
    
    // Environment variable parsing
    static bool parseEnvVar(const std::string& value, ConfigValue& out);
};

#endif // NLM_CONFIG_HPP
