//! Plugin Configuration for NLM Neural Simulation Project
//! 
//! This module implements the PluginConfiguration class which provides
//! configuration management for plugins. The PluginConfiguration:
//! - Manages plugin-specific configuration parameters
//! - Validates plugin configurations
//! - Supports configuration inheritance and overrides
//! - Provides configuration schema and validation
//! - Manages configuration history and versioning
//! - Integrates with the NLM configuration system
//! - Supports configuration templates and defaults
//! 
//! The PluginConfiguration class provides a unified interface for managing
//! plugin configurations while maintaining consistency with the NLM
//! configuration system and enabling advanced configuration features.

#pragma once

#include "PluginBase.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <mutex>
#include <any>
#include <unordered_set>
#include <chrono>
#include <algorithm>
#include <set>

namespace nlm {

class PluginConfiguration {
public:
    PluginConfiguration();
    ~PluginConfiguration();
    
    // Disable copying
    PluginConfiguration(const PluginConfiguration&) = delete;
    PluginConfiguration& operator=(const PluginConfiguration&) = delete;
    
    // Enable moving
    PluginConfiguration(PluginConfiguration&&) noexcept = default;
    PluginConfiguration& operator=(PluginConfiguration&&) noexcept = default;
    
    // Configuration management
    bool setConfiguration(const PluginConfig& config);
    bool updateConfiguration(const PluginConfig& config);
    bool mergeConfiguration(const PluginConfig& config);
    bool overrideConfiguration(const PluginConfig& config);
    bool replaceConfiguration(const PluginConfig& config);
    bool clearConfiguration();
    
    // Configuration access
    const PluginConfig& getConfiguration() const;
    PluginConfig& getConfiguration();
    std::any getParameter(const std::string& key) const;
    std::any getParameter(const std::string& key, const std::any& defaultValue) const;
    bool hasParameter(const std::string& key) const;
    std::vector<std::string> getParameterKeys() const;
    
    // Configuration validation
    bool validateConfiguration() const;
    std::vector<std::string> getValidationErrors() const;
    bool addValidationRule(const std::string& ruleName,
                          const std::function<bool(const PluginConfig&)>& validator);
    bool removeValidationRule(const std::string& ruleName);
    bool clearValidationRules();
    
    // Configuration schema management
    bool addParameterSchema(const std::string& key, const std::any& schema);
    bool removeParameterSchema(const std::string& key);
    const std::any* getParameterSchema(const std::string& key) const;
    std::unordered_map<std::string, std::any> getParameterSchemas() const;
    bool updateParameterSchema(const std::string& key, const std::any& schema);
    
    // Configuration inheritance
    bool addParentConfiguration(std::shared_ptr<PluginConfiguration> parent);
    bool removeParentConfiguration(std::shared_ptr<PluginConfiguration> parent);
    bool removeParentConfiguration(const std::string& pluginName);
    std::vector<std::shared_ptr<PluginConfiguration>> getParentConfigurations() const;
    std::any getInheritedParameter(const std::string& key) const;
    bool hasInheritedParameter(const std::string& key) const;
    std::vector<std::string> getInheritedParameterKeys() const;
    
    // Configuration overrides
    bool addOverride(const std::string& key, const std::any& value);
    bool removeOverride(const std::string& key);
    bool clearOverrides();
    std::unordered_map<std::string, std::any> getOverrides() const;
    bool hasOverride(const std::string& key) const;
    std::any getOverride(const std::string& key) const;
    
    // Configuration history and versioning
    bool enableHistory(bool enable);
    bool isHistoryEnabled() const;
    std::vector<PluginConfig> getConfigurationHistory() const;
    std::vector<std::chrono::system_clock::time_point> getConfigurationHistoryTimestamps() const;
    bool addConfigurationToHistory();
    bool rollbackToConfiguration(size_t index);
    bool getConfigurationAtIndex(size_t index, PluginConfig& config) const;
    
    // Configuration templates
    bool registerConfigurationTemplate(const std::string& templateName,
                                      const PluginConfig& templateConfig);
    bool unregisterConfigurationTemplate(const std::string& templateName);
    std::unique_ptr<PluginConfiguration> createFromTemplate(const std::string& templateName,
                                                           const PluginConfig& overrides = {});
    std::vector<std::string> getRegisteredTemplateNames() const;
    
    // Configuration export/import
    bool exportConfiguration(const std::string& filePath) const;
    bool importConfiguration(const std::string& filePath);
    std::string exportConfigurationAsJSON() const;
    bool importConfigurationFromJSON(const std::string& jsonData);
    
    // Configuration comparison
    bool equals(const PluginConfiguration& other) const;
    bool differsFrom(const PluginConfiguration& other) const;
    double getConfigurationDifference(const PluginConfiguration& other) const;
    
    // Configuration statistics
    struct ConfigurationStats {
        size_t totalParameters;
        size_t validParameters;
        size_t invalidParameters;
        size_t overrideCount;
        size_t inheritedParameterCount;
        double configurationEntropy;
        std::chrono::system_clock::time_point lastChanged;
        
        ConfigurationStats() : totalParameters(0), validParameters(0), invalidParameters(0),
            overrideCount(0), inheritedParameterCount(0), configurationEntropy(0.0),
            lastChanged(std::chrono::system_clock::now()) {}
    };
    
    ConfigurationStats getStatistics() const;
    
    // Configuration system integration
    void setNLMConfig(std::shared_ptr<Config> nlmConfig);
    std::shared_ptr<Config> getNLMConfig() const;
    bool syncWithNLMConfig(const std::string& configPrefix = "");
    bool applyNLMConfigToPlugin(const std::string& configPrefix = "");
    
    // Configuration change notification
    using ConfigurationChangeCallback = std::function<void(const PluginConfig&, const PluginConfig&)>;
    bool addConfigurationChangeCallback(const ConfigurationChangeCallback& callback);
    bool removeConfigurationChangeCallback(const std::string& callbackId);
    void notifyConfigurationChange(const PluginConfig& oldConfig, const PluginConfig& newConfig);
    
    // Configuration dependency management
    bool addConfigurationDependency(const std::string& pluginName, const std::string& parameterKey);
    bool removeConfigurationDependency(const std::string& pluginName, const std::string& parameterKey);
    std::vector<std::string> getConfigurationDependencies(const std::string& pluginName) const;
    std::unordered_map<std::string, std::vector<std::string>> getAllConfigurationDependencies() const;
    
    // Configuration validation by plugin
    bool validatePluginConfiguration(const PluginConfig& pluginConfig) const;
    std::vector<std::string> getPluginConfigurationErrors(const PluginConfig& pluginConfig) const;
    bool addPluginConfigurationValidator(const std::string& pluginName,
                                        const std::function<bool(const PluginConfig&)>& validator);
    
    // Configuration lifecycle
    void pauseConfigurationUpdates();
    void resumeConfigurationUpdates();
    void shutdownConfigurationSystem();
    bool isConfigurationSystemActive() const;
    
    // Configuration system integration
    void setPluginManager(std::shared_ptr<PluginManager> pluginManager);
    void setRegistry(std::shared_ptr<PluginRegistry> registry);
    void setSecurityManager(std::shared_ptr<PluginSecurityManager> securityManager);
    void setUpdateManager(std::shared_ptr<PluginUpdateManager> updateManager);
    void setHookSystem(std::shared_ptr<HookSystem> hookSystem);
    void setEventSystem(std::shared_ptr<EventSystem> eventSystem);
    void setFactory(std::shared_ptr<ComponentFactory> factory);
    
    // Configuration debugging and monitoring
    void enableDebugLogging(bool enable);
    bool isDebugLoggingEnabled() const;
    void logConfigurationChange(const std::string& changeType, const std::string& parameterKey, const std::any& oldValue, const std::any& newValue) const;
    void logConfigurationValidationError(const std::string& error) const;
    void logConfigurationSystemError(const std::string& error) const;
    
    // Configuration system optimization
    void optimizeConfigurationStorage();
    void consolidateConfigurationOverwrites();
    void cleanInvalidConfigurations();
    void balanceConfigurationHistory();
    
    // Configuration system export/import
    bool exportConfigurationTemplate(const std::string& filePath) const;
    bool importConfigurationTemplate(const std::string& filePath);
    std::string exportConfigurationTemplateAsJSON() const;
    bool importConfigurationTemplateFromJSON(const std::string& jsonData);
    
    // Configuration system testing
    bool testConfigurationSystem() const;
    std::vector<std::string> getConfigurationTestResults() const;
    bool testConfigurationValidation() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void initializeImpl();
    void cleanupImpl();
    
    // Thread-safe operations
    template<typename T>
    T withConfigLock(T func) {
        std::unique_lock<std::mutex> lock(configMutex_);
        return func();
    }
    
    // Configuration validation
    bool validateConfigurationAgainstSchema(const PluginConfig& config) const;
    bool validateConfigurationAgainstRules(const PluginConfig& config) const;
    bool validateConfigurationAgainstParents(const PluginConfig& config) const;
    
    // Configuration inheritance resolution
    std::any resolveInheritedParameter(const std::string& key) const;
    std::unordered_map<std::string, std::any> resolveInheritedParameters() const;
    
    // Configuration change detection
    bool detectConfigurationChange(const PluginConfig& oldConfig, const PluginConfig& newConfig) const;
    
    // Configuration serialization
    std::string serializeConfigurationToJSON(const PluginConfig& config) const;
    bool deserializeConfigurationFromJSON(const std::string& jsonData, PluginConfig& config);
    
    // Configuration statistics
    void updateConfigurationStatistics();
    double calculateConfigurationEntropy() const;
    
    // Configuration cleanup
    void cleanupExpiredConfigurations();
    void cleanupInvalidConfigurations();
}; // namespace nlm