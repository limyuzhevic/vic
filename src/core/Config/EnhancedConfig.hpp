#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <filesystem>
#include <json.hpp>

namespace nlm {

using json = nlohmann::json;

// Enhanced configuration system for NLM
// Provides comprehensive configuration management with validation, templates, and advanced features

class ConfigValidator {
public:
    using ValidationRule = std::function<bool(const ConfigValue&, const std::string&)>;
    
    // Validation rules
    static bool validateInteger(const ConfigValue& value, const std::string& key);
    static bool validateDouble(const ConfigValue& value, const std::string& key);
    static bool validateBool(const ConfigValue& value, const std::string& key);
    static bool validateString(const ConfigValue& value, const std::string& key);
    static bool validateRange(const ConfigValue& value, const std::string& key, double min, double max);
    static bool validateLength(const ConfigValue& value, const std::string& key, size_t maxLength);
    static bool validatePattern(const ConfigValue& value, const std::string& key, const std::string& pattern);
    
    // Validation groups
    static bool validateBrainParameters(const json& config);
    static bool validatePlasticityParameters(const json& config);
    static bool validateMemoryParameters(const json& config);
    static bool validateNeuromodulationParameters(const json& config);
    static bool validateDevelopmentParameters(const json& config);
    
    // Configuration templates
    static json getBrainDefaults();
    static json getPlasticityDefaults();
    static json getMemoryDefaults();
    static json getNeuromodulationDefaults();
    static json getDevelopmentDefaults();
};

class ConfigProfile {
public:
    std::string name;
    std::string description;
    json parameters;
    bool isActive;
    
    ConfigProfile() : isActive(false) {}
    ConfigProfile(const std::string& n, const std::string& d) : name(n), description(d), isActive(false) {}
};

class EnhancedConfig {
public:
    EnhancedConfig();
    ~EnhancedConfig();
    
    // Disable copying, enable moving
    EnhancedConfig(const EnhancedConfig&) = delete;
    EnhancedConfig& operator=(const EnhancedConfig&) = delete;
    EnhancedConfig(EnhancedConfig&&) noexcept;
    EnhancedConfig& operator=(EnhancedConfig&&) noexcept;
    
    // Load configuration from various sources
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& jsonString);
    bool loadFromYAML(const std::string& yamlString);
    bool loadFromCommandLine(int argc, char** argv);
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Save configuration
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    bool saveToJSON(const std::string& filepath) const;
    std::string toJSON() const;
    std::string toYAML() const;
    
    // Get configuration values with advanced type conversion
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T()) const;
    
    // Enhanced get methods with validation
    template<typename T>
    std::optional<T> getValidated(const std::string& key, const T& defaultValue = T()) const;
    
    // Set configuration values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence and validation
    bool has(const std::string& key) const;
    bool validate(const std::string& key) const;
    
    // Configuration validation
    bool validateConfig() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Remove and clear
    void remove(const std::string& key);
    void clear();
    
    // Configuration profile management
    void addProfile(const ConfigProfile& profile);
    bool removeProfile(const std::string& name);
    bool activateProfile(const std::string& name);
    std::vector<std::string> getProfileNames() const;
    bool hasProfile(const std::string& name) const;
    
    // Configuration merging
    void merge(const EnhancedConfig& other, ConfigSource source = ConfigSource::Runtime);
    void mergeFromJSON(const std::string& jsonString, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration inheritance
    void setParent(const EnhancedConfig* parent);
    EnhancedConfig* getParent() const;
    EnhancedConfig getInheritedConfig() const;
    
    // Configuration presets
    static EnhancedConfig getPreset(const std::string& presetName);
    static std::vector<std::string> getPresetNames();
    
    // Configuration exploration
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysOfType(const std::string& type) const;
    json getAll() const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    ConfigSource getSource(const std::string& key) const;
    std::string getDescription(const std::string& key) const;
    void setDescription(const std::string& key, const std::string& description);
    
    // Configuration comparison
    bool equals(const EnhancedConfig& other) const;
    std::vector<std::string> getDifferences(const EnhancedConfig& other) const;
    
    // Configuration backup and restore
    bool backup(const std::string& filename) const;
    bool restore(const std::string& filename);
    
    // Configuration logging and debugging
    std::string summary() const;
    std::string getValidationSummary() const;
    void logConfiguration() const;
    
    // Configuration import/export
    bool importFromXML(const std::string& filepath);
    bool exportToXML(const std::string& filepath) const;
    
    // Configuration monitoring
    void enableMonitoring(bool enable);
    bool isMonitoringEnabled() const;
    std::vector<std::pair<std::string, ConfigValue>> getChangedValues() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    bool validateKey(const std::string& key) const;
    void applyValidationRules();
    void mergeProfiles();
    void resolveInheritance();
    json toJSONInternal() const;
    void fromJSONInternal(const json& j);
};

} // namespace nlm