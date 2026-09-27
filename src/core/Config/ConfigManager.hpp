// Advanced Configuration Management System
// Comprehensive configuration handling for NLM

#ifndef NLM_CONFIG_MANAGER_H
#define NLM_CONFIG_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cassert>

namespace nlm {
namespace config {

// Configuration file format
enum class ConfigFormat {
    INI,
    JSON,
    YAML,
    XML,
    TOML
};

// Configuration value types
enum class ConfigType {
    STRING,
    INT,
    FLOAT,
    DOUBLE,
    BOOL,
    VECTOR_STRING,
    VECTOR_INT,
    VECTOR_FLOAT,
    MAP_STRING_STRING
};

// Configuration option definition
struct ConfigOption {
    std::string key;
    std::string description;
    ConfigType type;
    bool required;
    std::string defaultValue;
    std::vector<std::string> allowedValues;
    std::string validationRegex;
    bool sensitive; // For passwords/api keys
    
    ConfigOption() : type(ConfigType::STRING), required(false), sensitive(false) {}
    
    ConfigOption(const std::string& key, const std::string& description,
                 ConfigType type = ConfigType::STRING, bool required = false,
                 const std::string& defaultValue = "")
        : key(key), description(description), type(type), required(required),
          defaultValue(defaultValue), sensitive(false) {}
    
    // Type conversion helpers
    std::string getString() const;
    int getInt() const;
    float getFloat() const;
    double getDouble() const;
    bool getBool() const;
    std::vector<std::string> getVectorString() const;
    std::vector<int> getVectorInt() const;
    std::vector<float> getVectorFloat() const;
    std::map<std::string, std::string> getMapStringString() const;
    
    // Validation
    bool validateValue(const std::string& value) const;
    std::string getValidationError(const std::string& value) const;
    
    // Serialization
    std::string serialize() const;
    bool deserialize(const std::string& str);
    
    // Comparison
    bool operator==(const ConfigOption& other) const;
    bool operator!=(const ConfigOption& other) const { return !(*this == other); }
};

// Configuration value
struct ConfigValue {
    std::string key;
    std::string rawValue;
    std::string source; // "default", "config_file", "command_line"
    std::chrono::system_clock::time_point timestamp;
    bool modified;
    
    ConfigValue() : modified(false) {
        timestamp = std::chrono::system_clock::now();
    }
    
    ConfigValue(const std::string& key, const std::string& value, 
                const std::string& source = "default")
        : key(key), rawValue(value), source(source), modified(false) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Configuration group
struct ConfigGroup {
    std::string name;
    std::string description;
    std::vector<std::string> optionKeys;
    bool editable;
    
    ConfigGroup(const std::string& name, const std::string& description = "")
        : name(name), description(description), editable(true) {}
};

// Main configuration manager
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    // Configuration management
    void registerOption(const ConfigOption& option);
    void registerOption(const std::string& key, const std::string& description,
                       ConfigType type = ConfigType::STRING, bool required = false,
                       const std::string& defaultValue = "",
                       const std::vector<std::string>& allowedValues = {});
    
    // Get configuration values
    std::string getString(const std::string& key) const;
    int getInt(const std::string& key) const;
    float getFloat(const std::string& key) const;
    double getDouble(const std::string& key) const;
    bool getBool(const std::string& key) const;
    
    // Set configuration values
    void setString(const std::string& key, const std::string& value,
                   const std::string& source = "command_line");
    void setInt(const std::string& key, int value,
                const std::string& source = "command_line");
    void setFloat(const std::string& key, float value,
                  const std::string& source = "command_line");
    void setDouble(const std::string& key, double value,
                   const std::string& source = "command_line");
    void setBool(const std::string& key, bool value,
                 const std::string& source = "command_line");
    
    // Configuration file I/O
    bool loadFromFile(const std::string& filename, ConfigFormat format = ConfigFormat::INI);
    bool saveToFile(const std::string& filename, ConfigFormat format = ConfigFormat::INI) const;
    
    // Command line configuration
    bool loadFromCommandLine(int argc, char* argv[]);
    bool loadFromEnvironment(const std::map<std::string, std::string>& env);
    
    // Configuration templates
    bool saveTemplate(const std::string& name, const std::string& description = "");
    bool loadTemplate(const std::string& name);
    std::vector<std::string> getAvailableTemplates() const;
    bool deleteTemplate(const std::string& name);
    
    // Configuration groups
    void createGroup(const std::string& name, const std::string& description = "");
    void addOptionToGroup(const std::string& groupName, const std::string& optionKey);
    void removeGroup(const std::string& name);
    std::vector<ConfigGroup> getGroups() const;
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    bool isValid() const { return validate(); }
    
    // Configuration history
    void enableHistory(bool enabled, size_t maxHistoryEntries = 1000);
    std::vector<ConfigValue> getHistory(const std::string& key) const;
    bool hasChanged(const std::string& key) const;
    std::string getCurrentSource(const std::string& key) const;
    
    // Configuration comparison
    bool equals(const ConfigManager& other) const;
    std::vector<std::string> diff(const ConfigManager& other) const;
    
    // Configuration export/import
    std::string exportToJSON() const;
    std::string exportToYAML() const;
    std::string exportToINI() const;
    bool importFromJSON(const std::string& json);
    bool importFromYAML(const std::string& yaml);
    bool importFromINI(const std::string& ini);
    
    // Configuration statistics
    size_t getOptionCount() const;
    size_t getSetCount() const;
    double getCompleteness() const;
    
    // Configuration search
    std::vector<ConfigOption> searchOptions(const std::string& pattern) const;
    std::vector<ConfigValue> searchValues(const std::string& pattern) const;
    
    // Configuration reset
    void resetToDefaults();
    void resetGroup(const std::string& groupName);
    void resetToSource(const std::string& key, const std::string& source);
    
    // Configuration utilities
    std::string getConfigFilePath() const;
    void setConfigFilePath(const std::string& path);
    bool configFileExists() const;
    
    // Configuration monitoring
    void setChangeCallback(std::function<void(const std::string& key, const std::string& oldValue, const std::string& newValue)> callback);
    
private:
    std::map<std::string, ConfigOption> options;
    std::map<std::string, ConfigValue> values;
    std::vector<ConfigGroup> groups;
    
    std::vector<std::string> validationErrors;
    
    std::string configFilePath;
    bool historyEnabled;
    size_t maxHistoryEntries;
    
    std::map<std::string, std::vector<ConfigValue>> history;
    
    std::function<void(const std::string& key, const std::string& oldValue, const std::string& newValue)> changeCallback;
    
    mutable std::mutex mutex;
    
    // Internal helpers
    void updateHistory(const std::string& key, const std::string& value);
    std::string getOptionTypeName(ConfigType type) const;
    std::string convertToString(const std::any& value) const;
    std::any convertToType(const std::string& value, ConfigType type) const;
    
    // File format handlers
    bool loadINI(const std::string& filename);
    bool loadJSON(const std::string& filename);
    bool loadYAML(const std::string& filename);
    bool saveINI(const std::string& filename) const;
    bool saveJSON(const std::string& filename) const;
    bool saveYAML(const std::string& filename) const;
    
    // Template handlers
    std::string getTemplateDir() const;
    std::string getTemplateFilePath(const std::string& name) const;
    
    // Validation
    void validateOption(const ConfigOption& option, const std::string& value) const;
};

// Configuration presets for common scenarios
class ConfigPresets {
public:
    static ConfigManager getDefaultConfig();
    static ConfigManager getResearchConfig();
    static ConfigManager getProductionConfig();
    static ConfigManager getDebugConfig();
    static ConfigManager getPerformanceConfig();
    
    static ConfigManager getMinimalConfig();
    static ConfigManager getFullConfig();
    static ConfigManager getCloudConfig();
    
    // Custom preset loading
    static bool loadPreset(ConfigManager& config, const std::string& presetName);
    static bool savePreset(const ConfigManager& config, const std::string& presetName);
    
    static std::vector<std::string> getAvailablePresets();
};

// Configuration helpers
namespace helpers {
    // Command line argument parsing for configuration
    struct CLIConfigOptions {
        std::string configFile;
        std::string outputFile;
        std::string logLevel;
        bool verbose;
        bool help;
        bool version;
        bool benchmark;
        bool profile;
        std::map<std::string, std::string> customOptions;
    };
    
    CLIConfigOptions parseCLIConfigOptions(int argc, char* argv[]);
    
    // Environment variable configuration
    std::map<std::string, std::string> loadEnvironmentConfig(const std::string& prefix = "NLM_");
    
    // Configuration validation
    bool validateConfigFile(const std::string& filename, ConfigFormat format = ConfigFormat::INI);
    std::vector<std::string> getConfigValidationErrors(const std::string& filename, ConfigFormat format = ConfigFormat::INI);
    
    // Configuration comparison
    double compareConfigs(const ConfigManager& config1, const ConfigManager& config2);
    std::vector<std::string> findConfigDifferences(const ConfigManager& config1, const ConfigManager& config2);
    
    // Configuration utilities
    std::string getConfigSummary(const ConfigManager& config);
    std::string getConfigStatistics(const ConfigManager& config);
    
    // Configuration backup/restore
    bool backupConfig(const ConfigManager& config, const std::string& backupFile);
    bool restoreConfig(ConfigManager& config, const std::string& backupFile);
}

} // namespace config
} // namespace nlm

#endif // NLM_CONFIG_MANAGER_H
