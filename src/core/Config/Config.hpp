#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>

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

// JSON/YAML configuration schema validation
declaration of struct SchemaValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

// Configuration option definition
struct ConfigOption {
    std::string key;
    ConfigValue defaultValue;
    std::string description;
    std::function<bool(const ConfigValue&)> validator;
    ConfigSource source;
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
    
    // Load from file (JSON, YAML, or simple key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJsonString(const std::string& jsonString);
    
    // Load from YAML string  
    bool loadFromYamlString(const std::string& yamlString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON string
    std::string saveToJsonString() const;
    
    // Save to YAML string
    std::string saveToYamlString() const;
    
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
    
    // Validate configuration
    bool validate(const std::vector<ConfigOption>& schema) const;
    SchemaValidationResult validateWithDetails(const std::vector<ConfigOption>& schema) const;
    
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
    
    // Get configuration differences
    static std::string diff(const Config& config1, const Config& config2);
    
    // Configuration performance monitoring
    struct PerformanceMetrics {
        size_t totalAccesses = 0;
        size_t successfulAccesses = 0;
        double accessTime = 0.0;
        size_t validationTime = 0;
        size_t parseTime = 0;
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    void resetPerformanceMetrics();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::vector<std::string> splitString(const std::string& str, char delimiter);
    static bool isJsonFile(const std::string& filepath);
    static bool isYamlFile(const std::string& filepath);
    
    // JSON parsing helpers
    bool parseJsonValue(const std::string& jsonStr, ConfigValue& value) const;
    bool parseJsonString(const std::string& jsonStr, ConfigValue& value) const;
    bool parseJsonNumber(const std::string& jsonStr, ConfigValue& value) const;
    bool parseJsonBoolean(const std::string& jsonStr, ConfigValue& value) const;
    bool parseJsonArray(const std::string& jsonStr, ConfigValue& value) const;
    
    // YAML parsing helpers  
    bool parseYamlValue(const std::string& yamlStr, ConfigValue& value) const;
    
    // Validation helpers
    bool validateValue(const ConfigValue& value, const std::function<bool(const ConfigValue&)>& validator) const;
    
    // Performance monitoring helpers
    void recordAccess(bool success, double time); // TODO: Implement timing
};

} // namespace nlm

} // namespace nlm
