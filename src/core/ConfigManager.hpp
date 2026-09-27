// NLM Configuration Manager
// Advanced configuration management with file loading, saving, and validation

#pragma once

#include "core/Config/Config.hpp"
#include "core/Utilities.hpp"
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <functional>

namespace nlm {

// Configuration value types
enum class ConfigValueType {
    INTEGER,
    FLOAT,
    STRING,
    BOOLEAN,
    DOUBLE,
    VECTOR_FLOAT,
    VECTOR_INT
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::string warningMessage;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::string& error, const std::string& warning = "")
        : valid(v), errorMessage(error), warningMessage(warning) {}
};

// Configuration schema for validation
struct ConfigSchema {
    std::string name;
    ConfigValueType type;
    std::string defaultValue;
    std::string description;
    bool required;
    std::vector<std::string> allowedValues;
    
    std::function<bool(const std::string&)> validator;
    
    ConfigSchema(const std::string& n, ConfigValueType t, const std::string& def = "",
                const std::string& desc = "", bool req = false,
                const std::vector<std::string>& allowed = {})
        : name(n), type(t), defaultValue(def), description(desc), required(req), allowedValues(allowed) {}
};

// Configuration manager with schema validation and hot-reloading
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    // Load configuration from file
    bool loadFromFile(const std::string& filename);
    
    // Load configuration from string
    bool loadFromString(const std::string& configStr);
    
    // Save configuration to file
    bool saveToFile(const std::string& filename) const;
    
    // Add configuration schema
    void addSchema(const ConfigSchema& schema);
    
    // Get value with type conversion and validation
    template<typename T>
    bool getValue(const std::string& key, T& value) const;
    
    // Set value with validation
    template<typename T>
    bool setValue(const std::string& key, const T& value);
    
    // Check if key exists
    bool hasValue(const std::string& key) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Validate entire configuration
    ValidationResult validate() const;
    
    // Get schema for a specific key
    const ConfigSchema* getSchema(const std::string& key) const;
    
    // Clear configuration
    void clear();
    
    // Merge with another config
    void merge(const ConfigManager& other);
    
    // Export configuration as string
    std::string exportToString() const;
    
    // Hot reload configuration from file
    bool hotReload(const std::string& filename, double maxFileAge = 1.0);
    
    // Get configuration statistics
    struct Stats {
        size_t totalKeys;
        size_t requiredKeysMissing;
        size_t invalidKeys;
        double fileAge;
        bool fileModified;
    };
    
    Stats getStats() const;
    
    // Configuration change callback
    using ConfigChangeCallback = std::function<void(const std::string& key, const std::string& oldValue, const std::string& newValue)>;
    
    void addChangeCallback(const ConfigChangeCallback& callback);
    
    // Enable/disable validation
    void setValidationEnabled(bool enabled) { validationEnabled_ = enabled; }
    bool isValidationEnabled() const { return validationEnabled_; }
    
private:
    // Internal value storage
    struct ConfigValue {
        std::string stringValue;
        double floatValue;
        int64_t intValue;
        bool boolValue;
        ConfigValueType type;
        std::chrono::system_clock::time_point lastModified;
        
        ConfigValue() : type(ConfigValueType::STRING) {}
    };
    
    std::map<std::string, ConfigValue> configData_;
    std::map<std::string, ConfigSchema> configSchemas_;
    std::vector<ConfigChangeCallback> changeCallbacks_;
    bool validationEnabled_;
    std::string lastLoadedFile_;
    std::chrono::system_clock::time_point lastFileCheck_;
    
    // Private helper methods
    bool validateKey(const std::string& key, const std::string& value) const;
    void notifyChange(const std::string& key, const std::string& oldValue, const std::string& newValue);
    std::string convertToString(const ConfigValue& value) const;
    bool convertFromString(const std::string& str, ConfigValue& value, ConfigValueType targetType) const;
};

// Advanced configuration presets
class ConfigPresets {
public:
    static std::map<std::string, std::string> getPerformancePreset();
    static std::map<std::string, std::string> getQualityPreset();
    static std::map<std::string, std::string> getBalancedPreset();
    static std::map<std::string, std::string> getMinimalPreset();
    static std::map<std::string, std::string> getResearchPreset();
    static std::map<std::string, std::string> getRealTimePreset();
    
    // Get all available preset names
    static std::vector<std::string> getPresetNames();
    
    // Validate preset compatibility with current system
    static bool isPresetCompatible(const std::string& presetName);
};

// Configuration file format utilities
class ConfigFormatUtils {
public:
    // Supported configuration file formats
    enum class Format {
        INI,
        JSON,
        YAML,
        TOML,
        XML,
        PLAIN
    };
    
    // Parse configuration from different formats
    static bool parseFormat(const std::string& filename, Format format, ConfigManager& config);
    
    // Detect file format from extension and content
    static Format detectFormat(const std::string& filename);
    
    // Convert configuration to different formats
    static std::string exportToFormat(const ConfigManager& config, Format format);
    
    // Get supported format extensions
    static std::vector<std::string> getSupportedExtensions();
    
    // Validate configuration file syntax
    static bool validateFileSyntax(const std::string& filename, Format format);
    
    // Merge multiple configuration files
    static bool mergeFiles(const std::vector<std::string>& filenames, ConfigManager& outputConfig);
};

} // namespace nlm