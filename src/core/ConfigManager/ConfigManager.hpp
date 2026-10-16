#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <variant>
#include <optional>
#include <functional>
#include <chrono>
#include "core/Error.hpp"

namespace nlm {

// Configuration value types
enum class ConfigType {
    INT,
    INT64,
    DOUBLE,
    FLOAT,
    BOOL,
    STRING,
    STRING_VEC,
    INT_VEC,
    DOUBLE_VEC,
    FLOAT_VEC,
    OBJECT,
    ARRAY
};

// Configuration validation rule
struct ValidationRule {
    std::string paramName;
    ConfigType type;
    std::variant<int, int64_t, double, float, std::string> minValue;
    std::variant<int, int64_t, double, float, std::string> maxValue;
    std::string description;
    bool required = false;
    std::function<bool(const ConfigValue&)> customValidator;
};

// Configuration schema for validation
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    // Add a parameter to the schema
    void addParameter(const std::string& name, ConfigType type, 
                     const std::string& description, bool required = false,
                     const std::string& min = "", const std::string& max = "",
                     const std::function<bool(const ConfigValue&)>& validator = nullptr);
    
    // Validate configuration against schema
    bool validate(const Config& config, std::vector<std::string>& errors) const;
    
    // Get parameter description
    std::string getParameterDescription(const std::string& name) const;
    
    // Get all parameter names
    std::vector<std::string> getParameterNames() const;
    
    // Check if parameter exists in schema
    bool hasParameter(const std::string& name) const;
    
    // Get parameter type
    ConfigType getParameterType(const std::string& name) const;
    
    // Get validation rule for parameter
    const ValidationRule* getValidationRule(const std::string& name) const;
    
    // Save schema to file
    bool saveToFile(const std::string& filepath) const;
    
    // Load schema from file
    bool loadFromFile(const std::string& filepath);
    
    // Generate help documentation
    std::string generateHelp() const;

private:
    std::map<std::string, ValidationRule> parameters_;
};

// Configuration inheritance and merging
class ConfigInheritance {
public:
    enum class MergeStrategy {
        OVERRIDE,
        MERGE,
        IGNORE
    };
    
    ConfigInheritance() = default;
    
    // Set inheritance chain (parent configs)
    void setInheritanceChain(const std::vector<std::string>& chain);
    
    // Merge configuration with inheritance
    Config mergeWithInheritance(const Config& baseConfig) const;
    
    // Override merge strategy for specific keys
    void setOverrideStrategy(const std::string& key, MergeStrategy strategy);
    
    // Get all inheritance chains
    std::vector<std::string> getInheritanceChain() const;

private:
    std::vector<std::string> inheritanceChain_;
    std::map<std::string, MergeStrategy> keyStrategies_;
};

// Configuration backup and restore
class ConfigBackup {
public:
    ConfigBackup() = default;
    
    // Create backup of configuration
    bool createBackup(const Config& config, const std::string& backupPath);
    
    // Restore configuration from backup
    bool restoreBackup(Config& config, const std::string& backupPath);
    
    // List available backups
    std::vector<std::string> listBackups(const std::string& directory) const;
    
    // Get backup metadata
    std::string getBackupMetadata(const std::string& backupPath) const;
    
    // Clean up old backups
    bool cleanupOldBackups(const std::string& directory, int keepCount = 10);

private:
    static std::string generateBackupPath(const std::string& baseDir, 
                                         const std::string& prefix = "backup");
};

// Enhanced configuration manager with schema validation
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    // Disable copying, enable moving
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    ConfigManager(ConfigManager&&) noexcept;
    ConfigManager& operator=(ConfigManager&&) noexcept;
    
    // Load configuration from file with schema validation
    bool loadFromFile(const std::string& filepath, bool validate = true);
    
    // Load configuration from command line arguments
    bool loadFromArgs(int argc, char** argv, bool validate = true);
    
    // Load configuration from environment variables
    bool loadFromEnv(const std::map<std::string, std::string>& envVars, 
                    bool validate = true);
    
    // Merge configuration from multiple sources
    bool mergeConfigs(const std::vector<std::string>& configPaths,
                     bool validate = true);
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get configuration value with type conversion
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T()) const;
    
    // Set configuration value
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Check if configuration has a key
    bool has(const std::string& key) const;
    
    // Get all configuration keys
    std::vector<std::string> getKeys() const;
    
    // Get configuration source for a key
    ConfigSource getSource(const std::string& key) const;
    
    // Validate configuration against schema
    bool validate(const ConfigSchema& schema, std::vector<std::string>& errors) const;
    
    // Set configuration schema
    void setSchema(const ConfigSchema& schema);
    
    // Get configuration schema
    const ConfigSchema& getSchema() const;
    
    // Set inheritance settings
    void setInheritance(const ConfigInheritance& inheritance);
    
    // Get inheritance settings
    const ConfigInheritance& getInheritance() const;
    
    // Create backup
    bool createBackup(const std::string& backupPath);
    
    // Restore from backup
    bool restoreBackup(const std::string& backupPath);
    
    // Generate configuration summary
    std::string summary() const;
    
    // Generate configuration help
    std::string generateHelp() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration converter for type safety
class ConfigConverter {
public:
    static bool convertTo(ConfigValue& target, const std::string& value, 
                         ConfigType targetType, const std::string& key = "");
    
    static std::string valueToString(const ConfigValue& value, ConfigType type);
    
    static ConfigValue stringToValue(const std::string& str, ConfigType type);
    
    static bool validateValue(const ConfigValue& value, ConfigType type);
};

} // namespace nlm