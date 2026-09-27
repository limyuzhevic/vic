#pragma once

#include "IPlugin.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>

namespace nlm {
namespace plugins {

// Plugin parameter types
enum class ParameterType {
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    STRING_LIST,
    FLOAT_LIST,
    INTEGER_LIST,
    OBJECT,
    ARRAY
};

// Parameter constraint
struct ParameterConstraint {
    ParameterType type;
    std::string name;
    std::string description;
    std::string defaultValue;
    std::string validationRegex;
    std::vector<std::string> allowedValues;
    bool required;
    double minValue;
    double maxValue;
    int minItems;
    int maxItems;
    
    ParameterConstraint(const std::string& n, ParameterType t)
        : type(t), name(n), required(false), minValue(-std::numeric_limits<double>::max()),
          maxValue(std::numeric_limits<double>::max()), minItems(0), maxItems(std::numeric_limits<int>::max()) {}
};

// Plugin parameter
struct PluginParameter {
    std::string name;
    std::string value;
    ParameterType type;
    std::vector<std::string> validationErrors;
    bool isValid;
    
    PluginParameter(const std::string& n, const std::string& v, ParameterType t)
        : name(n), value(v), type(t), isValid(true) {}
};

// Plugin configuration group
struct PluginConfigGroup {
    std::string name;
    std::string description;
    std::vector<PluginParameter> parameters;
    std::map<std::string, ParameterConstraint> constraints;
    
    PluginConfigGroup(const std::string& n, const std::string& desc = "")
        : name(n), description(desc) {}
    
    bool addParameter(const std::string& name, const std::string& value, ParameterType type);
    bool addParameter(const std::string& name, const PluginParameter& param);
    bool addConstraint(const std::string& name, const ParameterConstraint& constraint);
    PluginParameter* getParameter(const std::string& name);
    const PluginParameter* getParameter(const std::string& name) const;
    ParameterConstraint* getConstraint(const std::string& name);
    const ParameterConstraint* getConstraint(const std::string& name) const;
    bool validateParameters() const;
    std::vector<std::string> getValidationErrors() const;
};

// Plugin configuration management
class PluginConfig {
public:
    PluginConfig();
    explicit PluginConfig(const std::string& filename);
    ~PluginConfig();
    
    // Configuration schema
    bool addConfigGroup(const PluginConfigGroup& group);
    bool removeConfigGroup(const std::string& groupName);
    PluginConfigGroup* getConfigGroup(const std::string& groupName);
    const PluginConfigGroup* getConfigGroup(const std::string& groupName) const;
    std::vector<std::string> getConfigGroupNames() const;
    
    // Plugin configuration
    bool setPluginConfig(const std::string& pluginName, const PluginConfigGroup& config);
    PluginConfigGroup* getPluginConfig(const std::string& pluginName);
    const PluginConfigGroup* getPluginConfig(const std::string& pluginName) const;
    bool removePluginConfig(const std::string& pluginName);
    std::vector<std::string> getPluginNames() const;
    
    // Parameter management
    bool setParameter(const std::string& pluginName, const std::string& groupName, 
                     const std::string& paramName, const std::string& value);
    std::string getParameter(const std::string& pluginName, const std::string& groupName,
                           const std::string& paramName) const;
    std::map<std::string, std::string> getGroupParameters(const std::string& pluginName, 
                                                       const std::string& groupName) const;
    std::map<std::string, std::map<std::string, std::string>> getPluginParameters(const std::string& pluginName) const;
    
    // Validation and constraints
    bool validatePluginConfig(const std::string& pluginName) const;
    std::vector<std::string> getValidationErrors(const std::string& pluginName) const;
    bool validateParameterConstraints(const std::string& pluginName, const std::string& groupName,
                                     const std::string& paramName) const;
    
    // Configuration loading/saving
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    bool loadFromString(const std::string& configString);
    std::string exportToString() const;
    
    // Configuration templates
    static PluginConfigGroup getDefaultConfigForPlugin(const std::string& pluginName);
    static std::vector<std::string> getRequiredParametersForPlugin(const std::string& pluginName);
    static std::vector<std::string> getOptionalParametersForPlugin(const std::string& pluginName);
    static std::map<std::string, PluginConfigGroup> getAllDefaultConfigs();
    
    // Configuration merging
    bool mergeConfig(const PluginConfig& otherConfig);
    bool applyDefaults();
    
    // Configuration monitoring
    using ConfigChangeCallback = std::function<void(const std::string& pluginName, const std::string& groupName, const std::string& paramName, const std::string& oldValue, const std::string& newValue)>;
    void subscribeToConfigChanges(const ConfigChangeCallback& callback);
    void unsubscribeFromConfigChanges(const ConfigChangeCallback& callback);
    
    // Configuration history
    struct ConfigHistoryEntry {
        std::string pluginName;
        std::string groupName;
        std::string paramName;
        std::string oldValue;
        std::string newValue;
        double timestamp;
    };
    std::vector<ConfigHistoryEntry> getConfigHistory() const;
    void clearConfigHistory();
    
    // Configuration statistics
    size_t getConfigSize() const;
    size_t getParameterCount(const std::string& pluginName) const;
    double getLastModifiedTime(const std::string& pluginName) const;
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace plugins
} // namespace nlm
