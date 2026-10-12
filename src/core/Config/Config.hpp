#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <any>
#include <functional>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <type_traits>
#include <chrono>
#include <ctime>
#include <mutex>
#include "../validation/InputValidator.hpp"
#include "errors/ErrorTypes.hpp"
#include "errors/ErrorContext.hpp"
#include "error_reporting/ErrorReporter.hpp"
#include "monitoring/SystemMonitor.hpp"

namespace nlm {

// Configuration value types (enhanced)
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<bool>
>;

// Forward declarations for nested config
class ConfigNode;
using ConfigMap = std::map<std::string, std::any>;
using ConfigSchema = std::map<std::string, std::any>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment,
    Template
};

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "", 
                         const std::string& expected = "", const std::string& actual = "")
        : std::runtime_error(message), key_(key), expected_(expected), actual_(actual) {}
    
    std::string getKey() const { return key_; }
    std::string getExpected() const { return expected_; }
    std::string getActual() const { return actual_; }
    
private:
    std::string key_;
    std::string expected_;
    std::string actual_;
};

// Configuration validation result
struct ValidationResult {
    bool valid = true;
    std::string error;
    std::string key;
    
    static ValidationResult success() { return {true, "", ""}; }
    static ValidationResult error(const std::string& err, const std::string& key = "") {
        return {false, err, key};
    }
};

// Schema validation function type
using SchemaValidator = std::function<ValidationResult(const std::string&, const ConfigValue&)>;

// Configuration template
struct ConfigTemplate {
    std::string name;
    std::map<std::string, ConfigValue> defaults;
    ConfigSource source = ConfigSource::Default;
    bool required = false;
};

// Environment configuration
struct EnvironmentConfig {
    std::string name;
    std::map<std::string, ConfigValue> overrides;
    std::string file;
    bool isDevelopment = false;
};

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
    
    // Static factory methods
    static Config fromFile(const std::string& filepath);
    static Config fromArgs(int argc, char** argv);
    static Config fromEnvironment(const std::string& envName = "default");
    static Config fromTemplate(const ConfigTemplate& template);
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath, ConfigSource source = ConfigSource::File);
    
    // Load from environment variables
    void loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv, ConfigSource source = ConfigSource::CommandLine);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get nested values
    template<typename T>
    std::optional<T> get(const std::vector<std::string>& keyPath) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set nested values
    void set(const std::vector<std::string>& keyPath, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    bool has(const std::vector<std::string>& keyPath) const;
    
    // Remove key
    void remove(const std::string& key);
    void remove(const std::vector<std::string>& keyPath);
    
    // Get all keys (flat)
    std::vector<std::string> getKeys() const;
    
    // Get all nested keys
    std::vector<std::string> getAllKeys() const;
    
    // Get keys by prefix
    std::vector<std::string> getKeysWithPrefix(const std::string& prefix) const;
    
    // Clear all
    void clear();
    
    // Environment support
    void setEnvironment(const std::string& envName);
    std::string getEnvironment() const;
    bool isDevelopment() const;
    
    // Validation framework
    void addValidator(const std::string& key, SchemaValidator validator);
    ValidationResult validate() const;
    ValidationResult validate(const std::string& key) const;
    
    // Schema validation
    void setSchema(const ConfigSchema& schema);
    ValidationResult validateSchema() const;
    
    // Value interpolation
    void enableInterpolation(bool enable = true);
    void addInterpolationReference(const std::string& target, const std::string& source);
    bool resolveInterpolations();
    
    // Template support
    void addTemplate(const ConfigTemplate& template);
    bool applyTemplate(const std::string& templateName);
    
    // Get configuration summary
    std::string summary() const;
    
    // Error handling
    void setLogger(std::function<void(const std::string&)>& logger);
    
    // File I/O utilities
    static bool validateFileFormat(const std::string& filepath);
    static std::string getDefaultConfigFile();
    
    // Configuration comparison
    bool operator==(const Config& other) const;
    bool operator!=(const Config& other) const;
    
    // Configuration merge
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string replacePlaceholders(const std::string& str, const std::function<std::string(const std::string&)>& resolver);
    static std::vector<std::string> splitPath(const std::string& path);
    static std::string joinPath(const std::vector<std::string>& path);
    
    // JSON parsing utilities
    static ConfigValue parseJsonValue(const std::string& str);
    static std::string serializeJsonValue(const ConfigValue& value);
    static std::string jsonToConfigFormat(const std::string& jsonStr);
    static std::string configToJsonFormat(const std::string& configStr);
    
    // Schema validation utilities
    static ValidationResult validateType(const std::string& key, const ConfigValue& value, const std::any& typeInfo);
    static ValidationResult validateRange(const std::string& key, const ConfigValue& value, const std::any& rangeInfo);
    static ValidationResult validateEnum(const std::string& key, const ConfigValue& value, const std::any& enumInfo);
    static ValidationResult validatePattern(const std::string& key, const ConfigValue& value, const std::any& patternInfo);
};

// Alias for backward compatibility
using ConfigPtr = std::shared_ptr<Config>;

} // namespace nlm
