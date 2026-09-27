#ifndef NLM_PLUGIN_CONFIGPLUGINSYSTEM_HPP
#define NLM_PLUGIN_CONFIGPLUGINSYSTEM_HPP

#include "PluginBase.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace nlm {
namespace plugin {

class ConfigPluginSystem {
public:
    ConfigPluginSystem();
    ~ConfigPluginSystem();
    
    // Plugin registration
    bool registerConfigurationPlugin(std::unique_ptr<IPlugin> plugin);
    bool unregisterConfigurationPlugin(const std::string& name);
    
    // Plugin access
    IPlugin* getConfigurationPlugin(const std::string& name) const;
    
    // Plugin configuration management
    bool configureSystem(const std::string& configName, const std::unordered_map<std::string, std::string>& config);
    std::unordered_map<std::string, std::string> getSystemConfig(const std::string& configName) const;
    
    // Configuration validation
    bool validateConfiguration(const std::string& configName) const;
    std::vector<std::string> getValidationErrors(const std::string& configName) const;
    
    // Configuration application
    bool applyConfiguration(const std::string& configName);
    bool revertConfiguration(const std::string& configName);
    bool saveConfiguration(const std::string& configName, const std::string& filename) const;
    bool loadConfiguration(const std::string& configName, const std::string& filename);
    
    // Configuration lifecycle
    bool initializeConfiguration(const std::string& configName);
    bool activateConfiguration(const std::string& configName);
    bool shutdownConfiguration(const std::string& configName);
    
    // Configuration monitoring
    void onConfigurationChanged(const std::string& configName, const std::unordered_map<std::string, std::string>& newConfig);
    void onConfigurationValidated(const std::string& configName, bool success);
    void onConfigurationApplied(const std::string& configName, bool success);
    
    // Configuration statistics
    struct ConfigurationStats {
        std::string name;
        PluginState state;
        uint64_t validationCount = 0;
        uint64_t applicationCount = 0;
        uint64_t errorCount = 0;
        std::string lastError;
        std::chrono::duration<double> averageValidationTime;
        std::chrono::duration<double> averageApplicationTime;
    };
    
    std::vector<ConfigurationStats> getAllConfigurationStats() const;
    ConfigurationStats getConfigurationStats(const std::string& configName) const;
    
    // Configuration composition
    bool composeConfigurations(const std::vector<std::string>& configNames, const std::string& composedConfigName);
    bool decomposeConfiguration(const std::string& composedConfigName, std::vector<std::string>& componentConfigNames);
    
    // Configuration conflict detection
    std::vector<std::pair<std::string, std::string>> getConfigurationConflicts() const;
    bool resolveConfigurationConflicts();
    
    // Configuration backup and restore
    bool createConfigurationBackup(const std::string& configName, const std::string& backupPath);
    bool restoreConfigurationBackup(const std::string& configName, const std::string& backupPath);
    std::vector<std::string> getConfigurationBackups(const std::string& configName) const;
    
    // Configuration versioning
    bool setConfigurationVersion(const std::string& configName, const PluginVersion& version);
    PluginVersion getConfigurationVersion(const std::string& configName) const;
    std::vector<PluginVersion> getConfigurationVersions(const std::string& configName) const;
    
    // Configuration integration
    bool integrateWithExternalSystem(const std::string& externalSystemName, const std::unordered_map<std::string, std::string>& config);
    bool deintegrateFromExternalSystem(const std::string& externalSystemName);
    bool isIntegratedWithExternalSystem(const std::string& externalSystemName) const;
    
    // Configuration security
    bool setConfigurationSecurityPolicy(const std::string& configName, const std::string& policy);
    std::string getConfigurationSecurityPolicy(const std::string& configName) const;
    bool enableConfigurationEncryption(const std::string& configName, bool enabled);
    bool isConfigurationEncrypted(const std::string& configName) const;
    
    // Configuration debugging
    std::string getConfigurationDebugInfo(const std::string& configName) const;
    void dumpConfigurationState(const std::string& filename) const;
    
private:
    struct ConfigurationEntry {
        std::unique_ptr<IPlugin> plugin;
        PluginMetadata metadata;
        std::unordered_map<std::string, std::string> config;
        PluginState state;
        std::string error;
        std::chrono::system_clock::time_point creationTime;
        std::chrono::system_clock::time_point lastUpdate;
        PluginVersion version;
        std::string securityPolicy;
        bool encrypted;
        std::vector<std::string> externalSystems;
        ConfigurationStats stats;
    };
    
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, ConfigurationEntry> configurations_;
    
    std::vector<std::function<void(const std::string&, const std::unordered_map<std::string, std::string>&)>> configChangeCallbacks_;
    std::vector<std::function<void(const std::string&, bool)>> configValidatedCallbacks_;
    std::vector<std::function<void(const std::string&, bool)>> configAppliedCallbacks_;
    
    bool validateConfigurationEntry(const ConfigurationEntry& entry) const;
    bool applyConfigurationEntry(const std::string& configName);
    void updateConfigurationStats(const std::string& configName, PluginState state);
    std::vector<std::pair<std::string, std::string>> detectConfigurationConflicts() const;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_CONFIGPLUGINSYSTEM_HPP