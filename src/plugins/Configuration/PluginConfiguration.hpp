#pragma once

/**
 * Plugin Configuration
 * 
 * Plugin configuration management system with support for plugin-specific configuration options,
 * plugin configuration templates, plugin configuration validation, and dynamic plugin configuration
 * for the NLM brain simulation project.
 * 
 * Features:
 * - Plugin configuration management
 * - Plugin-specific configuration options
 * - Plugin configuration templates
 * - Plugin configuration validation
 * - Dynamic plugin configuration
 * - Plugin configuration inheritance
 * - Plugin configuration monitoring
 * - Plugin configuration backup
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <functional>

namespace nlm {
namespace plugins {

class ConfigurationManager;

/**
 * Plugin configuration type
 */
enum class PluginConfigType {
    CORE,
    DYNAMICS,
    NEURAL,
    MEMORY,
    BEHAVIOR,
    DEVELOPMENTAL,
    PERFORMANCE,
    INTEGRATION,
    CUSTOM
};

/**
 * Plugin configuration metadata
 */
struct PluginConfigMetadata {
    std::string pluginName;
    PluginConfigType configType;
    std::string version;
    std::string description;
    std::string author;
    double createdAt;
    double updatedAt;
    std::vector<std::string> dependencies;
    std::map<std::string, std::string> tags;
    
    PluginConfigMetadata(const std::string& name, PluginConfigType type)
        : pluginName(name), configType(type), version("1.0"), createdAt(0.0), updatedAt(0.0) {}
};

/**
 * Plugin configuration parameter
 */
struct PluginConfigParameter {
    std::string name;
    std::string value;
    ParameterType type;
    std::string description;
    bool required;
    std::string defaultValue;
    std::vector<std::string> allowedValues;
    double minValue;
    double maxValue;
    bool isValid;
    std::vector<std::string> validationErrors;
    
    PluginConfigParameter(const std::string& n, const std::string& v, ParameterType t)
        : name(n), value(v), type(t), required(false), minValue(-std::numeric_limits<double>::max()),
          maxValue(std::numeric_limits<double>::max()), isValid(true) {}
};

/**
 * Plugin configuration change event
 */
struct PluginConfigChangeEvent {
    std::string pluginName;
    std::string parameterName;
    std::string oldValue;
    std::string newValue;
    ParameterType type;
    ConfigSource source;
    double timestamp;
    std::string sessionId;
    
    PluginConfigChangeEvent(const std::string& plugin, const std::string& param,
                           const std::string& oldVal, const std::string& newVal,
                           ParameterType t, ConfigSource src, double ts,
                           const std::string& session = "")
        : pluginName(plugin), parameterName(param), oldValue(oldVal), newValue(newVal),
          type(t), source(src), timestamp(ts), sessionId(session) {}
};

/**
 * Plugin configuration
 */
class PluginConfiguration {
public:
    PluginConfiguration();
    explicit PluginConfiguration(const std::string& pluginName);
    ~PluginConfiguration();
    
    // Disable copying, enable moving
    PluginConfiguration(const PluginConfiguration&) = delete;
    PluginConfiguration& operator=(const PluginConfiguration&) = delete;
    PluginConfiguration(PluginConfiguration&&) noexcept = default;
    PluginConfiguration& operator=(PluginConfiguration&&) noexcept = default;
    
    // Plugin metadata
    PluginConfigMetadata& getMetadata() { return metadata_; }
    const PluginConfigMetadata& getMetadata() const { return metadata_; }
    
    void setPluginName(const std::string& name) { metadata_.pluginName = name; }
    void setVersion(const std::string& version) { metadata_.version = version; }
    
    // Configuration parameter management
    bool addParameter(const PluginConfigParameter& param);
    bool removeParameter(const std::string& paramName);
    PluginConfigParameter* getParameter(const std::string& paramName);
    const PluginConfigParameter* getParameter(const std::string& paramName) const;
    std::vector<std::string> getParameterNames() const;
    
    // Configuration value management
    void setParameterValue(const std::string& paramName, const std::string& value, ConfigSource source = ConfigSource::RUNTIME);
    std::string getParameterValue(const std::string& paramName) const;
    bool hasParameter(const std::string& paramName) const;
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    bool validateParameter(const std::string& paramName) const;
    
    // Configuration template operations
    bool applyTemplate(const std::string& templateName);
    bool saveAsTemplate(const std::string& templateName);
    std::vector<std::string> getAvailableTemplates() const;
    
    // Configuration inheritance
    bool addInheritance(const std::string& parentPluginName, const std::string& parentVersion = "1.0");
    bool removeInheritance(const std::string& parentPluginName);
    std::vector<std::string> getInheritedPlugins() const;
    
    // Configuration merging
    bool merge(const PluginConfiguration& otherConfig);
    bool mergeDefaultValues();
    
    // Configuration monitoring
    using PluginConfigChangeCallback = std::function<void(const PluginConfigChangeEvent& event)>;
    void subscribeToChanges(const PluginConfigChangeCallback& callback);
    void unsubscribeFromChanges(const PluginConfigChangeCallback& callback);
    
    // Configuration history
    struct PluginConfigHistoryEntry {
        std::string pluginName;
        std::string parameterName;
        std::string oldValue;
        std::string newValue;
        double timestamp;
    };
    std::vector<PluginConfigHistoryEntry> getConfigHistory() const;
    void clearConfigHistory();
    
    // Configuration statistics
    size_t getParameterCount() const { return parameters_.size(); }
    double getLastModifiedTime() const { return last_modified_time_; }
    
    // Configuration loading/saving
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    bool saveToString(std::string& output) const;
    bool loadFromString(const std::string& configString);
    
    // Configuration backup
    bool createBackup(const std::string& backupFile) const;
    bool restoreFromBackup(const std::string& backupFile);
    std::string getCurrentSessionId() const { return current_session_id_; }
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Debug and diagnostics
    std::string dump() const;
    std::string getSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    PluginConfigMetadata metadata_;
    
    std::vector<PluginConfigParameter> parameters_;
    std::map<std::string, std::string> parameterValues_;
    std::vector<std::string> inheritedPlugins_;
    double last_modified_time_ = 0.0;
    std::string current_session_id_;
    
    // Internal helpers
    void notifyChange(const PluginConfigChangeEvent& event);
    PluginConfigParameter* findParameter(const std::string& paramName);
    const PluginConfigParameter* findParameter(const std::string& paramName) const;
    bool applyInheritance(const PluginConfiguration& parentConfig) const;
    PluginConfigParameter getInheritedParameter(const std::string& paramName) const;
    
    friend class ConfigurationManager;
    friend class PluginConfigurationValidator;
};

/**
 * Plugin configuration validator
 */
class PluginConfigurationValidator {
public:
    PluginConfigurationValidator();
    ~PluginConfigurationValidator();
    
    // Disable copying, enable moving
    PluginConfigurationValidator(const PluginConfigurationValidator&) = delete;
    PluginConfigurationValidator& operator=(const PluginConfigurationValidator&) = delete;
    PluginConfigurationValidator(PluginConfigurationValidator&&) noexcept = default;
    PluginConfigurationValidator& operator=(PluginConfigurationValidator&&) noexcept = default;
    
    // Validation operations
    bool validatePluginConfiguration(const PluginConfiguration& config) const;
    std::vector<std::string> getValidationErrors(const PluginConfiguration& config) const;
    
    // Constraint validation
    bool validateParameterConstraints(const PluginConfiguration& config, const std::string& paramName) const;
    bool validateParameterType(const std::string& value, ParameterType type) const;
    bool validateParameterRange(const std::string& value, double min, double max) const;
    bool validateParameterEnum(const std::string& value, const std::vector<std::string>& allowedValues) const;
    
    // Plugin-specific validation
    bool validatePluginSpecificRules(const PluginConfiguration& config) const;
    bool validatePluginDependencies(const PluginConfiguration& config) const;
    
    // Validation results
    struct PluginValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        PluginValidationResult() : valid(true) {}
    };
    
    PluginValidationResult validatePlugin(const PluginConfiguration& config) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
};

} // namespace plugins
} // namespace nlm
