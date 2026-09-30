// Enhanced Configuration Manager for NLM
// Provides comprehensive configuration management with validation, schema, and multiple input sources

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// Enhanced configuration value types
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
    Environment,
    Runtime
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::string path;  // Key path for nested validation
    
    static ValidationResult success() { return {true, "", ""}; }
    static ValidationResult error(const std::string& msg, const std::string& path = "") {
        return {false, msg, path};
    }
};

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string type;  // "int", "int64_t", "double", "bool", "string", "list<int>", etc.
    ConfigValue defaultValue;
    std::string description;
    bool required = false;
    std::vector<std::string> allowedValues;
    std::function<ValidationResult(const ConfigValue&)> validator;
    
    ConfigSchema() = default;
    ConfigSchema(const std::string& k, const ConfigValue& def, const std::string& desc)
        : key(k), type(typeName(def)), defaultValue(def), description(desc), required(false) {}
};

// Enhanced configuration manager with schema validation
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from JSON file
    bool loadFromFile(const std::string& filepath);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to JSON file
    bool saveToFile(const std::string& filepath, bool pretty = true) const;
    
    // Merge with another configuration
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Get values with enhanced error handling
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with source tracking
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    
    // Enhanced set methods with type safety
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void setList(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Advanced configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    
    // Schema management
    void addSchema(const ConfigSchema& schema);
    void addDefaultSchema(const std::vector<ConfigSchema>& schemas);
    bool validateSchema() const;
    std::vector<ValidationResult> validate() const;
    
    // Enhanced summary and debugging
    std::string summary() const;
    std::string toJSON() const;
    void printValidationErrors() const;
    
    // Configuration import/export
    bool importFromJSON(const json& j, ConfigSource source = ConfigSource::File);
    json exportToJSON() const;
    
    // Statistics and monitoring
    size_t getEntryCount() const;
    ConfigSource getSource(const std::string& key) const;
    std::string getDescription(const std::string& key) const;
    
    // Configuration helper methods
    static std::string normalizeKey(const std::string& key);
    static bool isValidKey(const std::string& key);
    static std::string getConfigFilePath(const std::string& filename);
    
    // Configuration presets
    static Config getDefaultConfig();
    static Config getPhase3Config();
    static Config getPhase4Config();
    static Config getPhase5Config();
    static Config getPhase6Config();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string sanitizeJSON(const std::string& input);
    
    // Validation helpers
    ValidationResult validateValue(const ConfigSchema& schema, const ConfigValue& value, const std::string& path = "") const;
    bool validateType(const ConfigValue& value, const std::string& expectedType) const;
    std::vector<ValidationResult> validateAllSchemas() const;
    std::string getTypeName(const ConfigValue& value) const;
};

// Configuration manager factory
class ConfigManager {
public:
    static ConfigManager& getInstance();
    
    // Initialize with default schemas
    void initialize();
    
    // Get the global configuration instance
    static Config& getGlobalConfig();
    
    // Configuration management
    static void loadFromFile(const std::string& filepath);
    static void loadFromCommandLine(int argc, char** argv);
    static void loadFromEnvironment(const std::string& prefix = "NLM_");
    static void saveToFile(const std::string& filepath, bool pretty = true);
    
    // Utility functions
    static std::string getConfigPath(const std::string& filename);
    static bool validateConfig(const Config& config);
    
private:
    ConfigManager();
    ~ConfigManager();
    
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    std::unique_ptr<Config> globalConfig;
};

// Configuration presets for different phases
namespace ConfigPresets {
    static std::vector<ConfigSchema> getPhase3Schema();
    static std::vector<ConfigSchema> getPhase4Schema();
    static std::vector<ConfigSchema> getPhase5Schema();
    static std::vector<ConfigSchema> getPhase6Schema();
    static std::vector<ConfigSchema> getDefaultSchema();
}

// Convenience template functions for common configuration operations
namespace ConfigUtils {
    // Value conversion utilities
    template<typename T>
    T toValue(const ConfigValue& value);
    
    // Type checking utilities
    template<typename T>
    bool isType(const ConfigValue& value);
    
    // Validation utilities
    bool isInRange(double value, double min, double max);
    bool isInRange(int value, int min, int max);
    bool isInRange(int64_t value, int64_t min, int64_t max);
    
    // String utilities
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    bool startsWith(const std::string& str, const std::string& prefix);
    bool endsWith(const std::string& str, const std::string& suffix);
    std::vector<std::string> split(const std::string& str, char delimiter);
}

// Explicit template instantiations for Config class
namespace ConfigInternal {
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    
    template int Config::getOr<int>(const std::string&, const int&) const;
    template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
}

} // namespace nlm

// Include implementation file
#include "ConfigEnhanced.cpp"