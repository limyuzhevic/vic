#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <variant>
#include <optional>
#include <memory>
#include <chrono>
#include <functional>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include "Config.hpp"

namespace nlm {

// Forward declarations
class ConfigurationManager;
class ConfigurationPresets;
class EnvironmentManager;
class ConfigurationValidator;
class ConfigurationTemplate;
class ConfigurationHistory;

// Configuration schema definition
struct ConfigurationSchema {
    std::string key;
    std::string type;  // "int", "double", "string", "bool", "vector<int>", etc.
    std::string description;
    std::variant<int, double, std::string, bool> defaultValue;
    bool required = false;
    std::string validationPattern;
    std::vector<std::string> allowedValues;
    double minValue = std::numeric_limits<double>::lowest();
    double maxValue = std::numeric_limits<double>::max();
    std::function<bool(const ConfigValue&)> validator;
    std::map<std::string, std::string> dependencies; // key -> constraint
};

// Environment types
enum class Environment {
    Development,
    Test,
    Staging,
    Production,
    Research,
    Performance,
    Debug,
    Custom
};

// Configuration preset types
enum class PresetType {
    Default,
    Development,
    Debug,
    Performance,
    Research,
    Custom
};

// Configuration change tracking
struct ConfigurationChange {
    std::string key;
    std::optional<ConfigValue> oldValue;
    std::optional<ConfigValue> newValue;
    ConfigSource source;
    std::chrono::system_clock::time_point timestamp;
    std::string userId;
    std::string sessionId;
};

// Configuration optimization metrics
struct OptimizationMetrics {
    double memoryUsage;
    double updateTime;
    double validationTime;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
};

// Main ConfigurationManager class
class ConfigurationManager {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    ConfigurationManager(); // Private constructor - use factory methods
    
public:
    static std::unique_ptr<ConfigurationManager> create();
    static std::unique_ptr<ConfigurationManager> create(Environment env);
    static std::unique_ptr<ConfigurationManager> create(const std::string& presetName);
    
    // Configuration loading
    bool loadFromFile(const std::string& filepath, Environment env = Environment::Development);
    bool loadFromDirectory(const std::string& directory, const std::string& extension = ".json");
    bool loadFromJSON(const std::string& jsonContent);
    bool loadFromYAML(const std::string& yamlContent);
    
    // Configuration saving
    bool saveToFile(const std::string& filepath, Environment env = Environment::Development) const;
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    std::string toJSON() const;
    std::string toYAML() const;
    
    // Configuration management
    void clear(Environment env = Environment::Development);
    void merge(const ConfigurationManager& other, Environment env = Environment::Development);
    void override(const ConfigurationManager& other, Environment env = Environment::Development);
    
    // Environment management
    void setEnvironment(Environment env);
    Environment getEnvironment() const;
    bool isEnvironmentCompatible(const ConfigurationSchema& schema) const;
    
    // Presets management
    bool savePreset(const std::string& name, PresetType type = PresetType::Custom);
    bool loadPreset(const std::string& name);
    bool deletePreset(const std::string& name);
    std::vector<std::string> getPresetNames() const;
    bool applyPreset(const std::string& name, bool merge = true);
    
    // Validation
    bool validate() const;
    bool validateEnvironment(Environment env) const;
    std::vector<std::string> getValidationErrors() const;
    bool addSchema(const ConfigurationSchema& schema);
    bool removeSchema(const std::string& key);
    std::vector<ConfigurationSchema> getSchemas() const;
    
    // Advanced features
    bool createTemplate(const std::string& name, const std::vector<std::string>& keys);
    std::unique_ptr<ConfigurationTemplate> getTemplate(const std::string& name) const;
    bool generateFromTemplate(const std::string& templateName, Environment env = Environment::Development);
    
    // Versioning and history
    bool enableHistory(bool enable = true);
    bool trackChanges(bool track = true);
    std::vector<ConfigurationChange> getChanges() const;
    bool rollbackToChange(size_t index);
    std::string getCurrentVersion() const;
    bool saveVersion(const std::string& versionName);
    
    // Optimization
    OptimizationMetrics optimize() const;
    bool compress() const;
    bool validateRelationships() const;
    
    // Comparison and diffing
    std::string diff(const ConfigurationManager& other) const;
    bool compare(const ConfigurationManager& other) const;
    std::vector<std::string> getAddedKeys(const ConfigurationManager& other) const;
    std::vector<std::string> getRemovedKeys(const ConfigurationManager& other) const;
    std::map<std::string, std::pair<ConfigValue, ConfigValue>> getModifiedKeys(const ConfigurationManager& other) const;
    
    // Statistics and analytics
    size_t getEntryCount(Environment env = Environment::Development) const;
    double getMemoryUsage(Environment env = Environment::Development) const;
    double getLastUpdateTime(Environment env = Environment::Development) const;
    std::chrono::system_clock::time_point getCreationTime() const;
    
    // Export/import
    bool exportToFile(const std::string& filepath, const std::string& format = "json") const;
    bool importFromFile(const std::string& filepath, const std::string& format = "json");
    
    // Predefined presets
    static std::unique_ptr<ConfigurationManager> createDevelopmentPreset();
    static std::unique_ptr<ConfigurationManager> createDebugPreset();
    static std::unique_ptr<ConfigurationManager> createPerformancePreset();
    static std::unique_ptr<ConfigurationManager> createResearchPreset();
    static std::unique_ptr<ConfigurationManager> createProductionPreset();
    
    // Factory methods for specific use cases
    static std::unique_ptr<ConfigurationManager> createBrainSimulationConfig();
    static std::unique_ptr<ConfigurationManager> createMemorySystemConfig();
    static std::unique_ptr<ConfigurationManager> createNeuromodulationConfig();
    static std::unique_ptr<ConfigurationManager> createEnvironmentConfig();
    
    // Get internal Config instance for compatibility
    std::shared_ptr<Config> getConfig(Environment env = Environment::Development) const;
    
    ~ConfigurationManager();
    
    // Delete copy operations
    ConfigurationManager(const ConfigurationManager&) = delete;
    ConfigurationManager& operator=(const ConfigurationManager&) = delete;
    
    // Enable move operations
    ConfigurationManager(ConfigurationManager&&) noexcept = default;
    ConfigurationManager& operator=(ConfigurationManager&&) noexcept = default;
};

// ConfigurationTemplate class
class ConfigurationTemplate {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    ConfigurationTemplate(const std::string& name);
    ~ConfigurationTemplate();
    
    void addKey(const std::string& key, const std::string& type, const std::string& description = "");
    void setDefaultValue(const std::string& key, const ConfigValue& value);
    void setValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator);
    void setConstraint(const std::string& key, const std::string& constraint);
    
    std::string getName() const;
    std::vector<std::string> getKeys() const;
    bool hasKey(const std::string& key) const;
    std::string getKeyType(const std::string& key) const;
    std::string getKeyDescription(const std::string& key) const;
    std::optional<ConfigValue> getDefaultValue(const std::string& key) const;
    std::function<bool(const ConfigValue&)> getValidator(const std::string& key) const;
    std::string getConstraint(const std::string& key) const;
    
    std::string toString() const;
    bool validate(const ConfigurationManager& config) const;
    std::string getValidationErrors(const ConfigurationManager& config) const;
};
