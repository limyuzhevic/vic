#pragma once

#include "Config.hpp"
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <limits>
#include <nlohmann/json.hpp>

namespace nlm {

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "double", "string", "bool", "vector"
    std::string description;
    std::string defaultValue;
    bool required = false;
    std::vector<std::string> allowedValues;
    double minValue = std::numeric_limits<double>::lowest();
    double maxValue = std::numeric_limits<double>::max();
    int minSize = 0;
    int maxSize = INT_MAX;
    bool isArray = false;
};

// Configuration template
struct ConfigTemplate {
    std::string name;
    std::string description;
    std::vector<ConfigSchema> schema;
    std::map<std::string, std::string> defaultValues;
};

// Dynamic configuration update callback
using ConfigUpdateCallback = std::function<void(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue)>;

class AdvancedConfigManager : public Config {
public:
    AdvancedConfigManager();
    ~AdvancedConfigManager();
    
    // Schema and template management
    void addSchema(const ConfigSchema& schema);
    void removeSchema(const std::string& key);
    bool validateSchema(const std::string& key, const ConfigValue& value) const;
    std::vector<std::string> getValidationErrors() const;
    
    // Template management
    void loadTemplate(const std::string& templateName);
    void saveTemplate(const std::string& templateName, const std::string& description);
    bool hasTemplate(const std::string& templateName) const;
    std::vector<std::string> getTemplateNames() const;
    
    // Schema validation
    void validateAll() const;
    bool isValid() const;
    
    // Dynamic reloading
    bool reloadFromFile(const std::string& filepath);
    bool enableAutoReload(const std::string& filepath, std::chrono::milliseconds interval = std::chrono::seconds(5000));
    void disableAutoReload();
    
    // Configuration validation with detailed error messages
    bool validateWithDetails(std::vector<std::pair<std::string, std::string>>& errors) const;
    std::string getValidationReport() const;
    
    // Configuration documentation generation
    std::string generateDocumentation(const std::string& format = "markdown") const;
    std::string generateSchemaDocumentation() const;
    std::string generateUsageExamples() const;
    
    // Dynamic configuration modification
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true, const std::string& reason = "");
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true, const std::string& reason = "");
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true, const std::string& reason = "");
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true, const std::string& reason = "");
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true, const std::string& reason = "");
    
    // Callbacks for configuration changes
    void addUpdateCallback(const ConfigUpdateCallback& callback);
    void removeUpdateCallback(const std::string& callbackId);
    void clearUpdateCallbacks();
    
    // Configuration export/import
    bool exportToJSON(const std::string& filepath, bool includeDefaults = false) const;
    bool importFromJSON(const std::string& filepath);
    
    // Configuration comparison
    bool hasChanges() const;
    std::map<std::string, std::pair<ConfigValue, ConfigValue>> getChanges() const;
    void resetChangeTracking();
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getValidEntryCount() const;
    std::chrono::system_clock::time_point getLastModified() const;
    
protected:
    // Internal helpers
    bool validateValue(const ConfigSchema& schema, const ConfigValue& value) const;
    std::string configValueToString(const ConfigValue& value) const;
    std::string getConfigValueDescription(const std::string& key) const;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
