#pragma once

// Plugin Configuration System for NLM (Neural Learning Machine)
// Manages plugin configuration, validation, and persistence

#include "PluginCore.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <regex>

namespace nlm {

// Plugin Configuration Schema
struct PluginConfigSchema {
    std::string name;
    std::string type;
    std::string description;
    bool required;
    PluginConfigValue defaultValue;
    std::vector<std::string> allowedValues;
    std::map<std::string, std::string> validationRules;
    
    bool validate(const PluginConfigValue& value) const {
        // TODO: Implement validation logic based on type and rules
        return true;
    }
    
    std::string valueToString(const PluginConfigValue& value) const {
        if (std::holds_alternative<int>(value)) {
            return std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::to_string(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value) ? "true" : "false";
        } else if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            std::stringstream ss;
            ss << "[";
            const auto& vec = std::get<std::vector<int>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << vec[i];
            }
            ss << "]";
            return ss.str();
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            std::stringstream ss;
            ss << "[";
            const auto& vec = std::get<std::vector<double>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << vec[i];
            }
            ss << "]";
            return ss.str();
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            std::stringstream ss;
            ss << "[";
            const auto& vec = std::get<std::vector<std::string>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << vec[i] << "\"";
            }
            ss << "]";
            return ss.str();
        }
        return "";
    }
};

// Plugin Configuration Entry
struct PluginConfigEntry {
    std::string key;
    PluginConfigSchema schema;
    PluginConfigValue value;
    std::string source; // "default", "user", "runtime", "file"
    std::chrono::system_clock::time_point timestamp;
    bool modified;
    
    PluginConfigEntry() : key(), required(false), modified(false) {
        timestamp = std::chrono::system_clock::now();
    }
    
    PluginConfigEntry(const std::string& k, const PluginConfigSchema& s, const PluginConfigValue& v)
        : key(k), schema(s), value(v), source("runtime"), modified(true) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Plugin Configuration Manager
class PluginConfigSystem {
public:
    PluginConfigSystem();
    ~PluginConfigSystem();
    
    // Disable copying, enable moving
    PluginConfigSystem(const PluginConfigSystem&) = delete;
    PluginConfigSystem& operator=(const PluginConfigSystem&) = delete;
    PluginConfigSystem(PluginConfigSystem&&) noexcept;
    PluginConfigSystem& operator=(PluginConfigSystem&&) noexcept;
    
    // Initialize configuration system
    bool initialize();
    
    // Shutdown configuration system
    void shutdown();
    
    // Check if configuration system is active
    bool isActive() const;
    
    // Plugin configuration management
    bool setPluginConfig(const std::string& name, const std::string& key, const PluginConfigValue& value,
                        const std::string& source = "runtime");
    bool setPluginConfig(const std::string& name, const std::map<std::string, PluginConfigValue>& config,
                        const std::string& source = "runtime");
    PluginConfigValue getPluginConfig(const std::string& name, const std::string& key) const;
    std::map<std::string, PluginConfigValue> getPluginConfig(const std::string& name) const;
    
    // Configuration schema management
    bool registerConfigSchema(const std::string& pluginName, const PluginConfigSchema& schema);
    bool unregisterConfigSchema(const std::string& pluginName, const std::string& key);
    PluginConfigSchema getConfigSchema(const std::string& pluginName, const std::string& key) const;
    std::vector<PluginConfigSchema> getPluginConfigSchemas(const std::string& pluginName) const;
    
    // Configuration validation
    bool validatePluginConfig(const std::string& name, const std::string& key, const PluginConfigValue& value) const;
    bool validatePluginConfig(const std::string& name, const std::map<std::string, PluginConfigValue>& config) const;
    std::vector<std::string> getPluginConfigValidationErrors(const std::string& name) const;
    std::vector<std::string> getPluginConfigValidationErrors(const std::string& name, const std::string& key) const;
    
    // Configuration default values
    bool setPluginDefaultConfig(const std::string& name, const std::map<std::string, PluginConfigValue>& config);
    std::map<std::string, PluginConfigValue> getPluginDefaultConfig(const std::string& name) const;
    bool resetPluginConfigToDefaults(const std::string& name);
    bool resetAllPluginConfigsToDefaults();
    
    // Configuration persistence
    bool savePluginConfig(const std::string& name, const std::string& filepath) const;
    bool loadPluginConfig(const std::string& name, const std::string& filepath);
    bool saveAllPluginConfigs(const std::string& directory) const;
    bool loadAllPluginConfigs(const std::string& directory);
    
    // Configuration monitoring
    bool markPluginConfigModified(const std::string& name, const std::string& key);
    bool isPluginConfigModified(const std::string& name, const std::string& key) const;
    std::chrono::system_clock::time_point getPluginConfigTimestamp(const std::string& name, const std::string& key) const;
    
    // Configuration statistics
    size_t getPluginConfigCount(const std::string& name) const;
    size_t getSystemConfigCount() const;
    double getSystemMemoryUsage() const;
    
    // System configuration
    bool setGlobalConfig(const std::map<std::string, PluginConfigValue>& config);
    std::map<std::string, PluginConfigValue> getGlobalConfig() const;
    bool clearGlobalConfig();n    
    // Configuration commands
    bool executeConfigCommand(const std::string& name, const std::string& command,
                             const std::map<std::string, std::string>& args = {});
    
    // System commands
    bool executeSystemConfigCommand(const std::string& command,
                                   const std::map<std::string, std::string>& args = {});
    
    // System maintenance
    bool cleanConfigCache();
    bool validateAllConfigs();
    bool repairAllConfigs();
    bool optimizeConfigSystem();
    
    // System information
    std::string getConfigStatus() const;
    std::string getConfigInfo() const;
    std::vector<std::string> getConfigMessages() const;
    std::vector<std::string> getConfigWarnings() const;
    std::vector<std::string> getConfigErrors() const;
    
    // System commands
    bool exportConfigData(const std::string& filename, const std::string& format = "json") const;
    bool importConfigData(const std::string& filename, const std::string& format = "json");
    
    // System about information
    std::string getAbout() const;
    std::string getVersion() const;
    std::string getBuildInfo() const;
    
    // Configuration validation helpers
    static bool validateConfigValue(const PluginConfigValue& value, const std::string& type);
    static std::string configValueToString(const PluginConfigValue& value);
    static PluginConfigValue configValueFromString(const std::string& str, const std::string& type);
    static bool matchConfigPattern(const std::string& value, const std::string& pattern);
    
    // JSON serialization
    static nlohmann::json configValueToJson(const PluginConfigValue& value);
    static PluginConfigValue jsonToConfigValue(const nlohmann::json& json, const std::string& type);
    static nlohmann::json pluginConfigToJson(const std::string& pluginName, const std::map<std::string, PluginConfigValue>& config);
    static bool jsonToPluginConfig(const nlohmann::json& json, std::string& pluginName, std::map<std::string, PluginConfigValue>& config);
    
    // Configuration lifecycle callbacks
    virtual void onConfigChanged(const std::string& pluginName, const std::string& key, const PluginConfigValue& value);
    virtual void onConfigValidationError(const std::string& pluginName, const std::string& key, const std::string& error);
    virtual void onConfigSaved(const std::string& pluginName, const std::string& filepath);
    virtual void onConfigLoaded(const std::string& pluginName, const std::string& filepath);
    
    // System lifecycle callbacks
    virtual void onSystemInitialized();
    virtual void onSystemShutdown();
    virtual void onSystemError(const std::string& error);
    virtual void onSystemWarning(const std::string& warning);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    bool setPluginConfigInternal(const std::string& name, const std::string& key, const PluginConfigValue& value,
                               const std::string& source);
    PluginConfigValue getPluginConfigInternal(const std::string& name, const std::string& key) const;
    bool registerConfigSchemaInternal(const std::string& pluginName, const PluginConfigSchema& schema);
    bool validatePluginConfigInternal(const std::string& name, const std::string& key, const PluginConfigValue& value) const;
    bool validatePluginConfigInternal(const std::string& name, const std::map<std::string, PluginConfigValue>& config) const;
    bool savePluginConfigInternal(const std::string& name, const std::string& filepath) const;
    bool loadPluginConfigInternal(const std::string& name, const std::string& filepath);
    bool saveAllPluginConfigsInternal(const std::string& directory) const;
    bool loadAllPluginConfigsInternal(const std::string& directory);
    bool validateAllConfigsInternal() const;
    
    void logConfigMessage(const std::string& message, const std::string& level = "info");
    void updateSystemStatistics();
    
    std::string getConfigFilePath(const std::string& pluginName) const;
    std::string getConfigDirectoryPath() const;
    
    nlohmann::json configToJson(const std::map<std::string, PluginConfigEntry>& configEntries) const;
    bool jsonToConfig(const nlohmann::json& json, std::map<std::string, PluginConfigEntry>& configEntries);
    
    bool validateConfigValueInternal(const PluginConfigValue& value, const std::string& type) const;
    bool validateConfigPattern(const std::string& value, const std::string& pattern) const;
};

} // namespace nlm