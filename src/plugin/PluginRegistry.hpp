//! Plugin Registry for NLM Neural Simulation Project
//! 
//! This module implements the PluginRegistry class which serves as a catalog
//! and metadata store for all available and loaded plugins. It provides:
//! - Plugin discovery and registration
//! - Metadata management
//! - Plugin type categorization
//! - Tag-based filtering
//! - Version compatibility checking
//! - Plugin dependency resolution
//! - Metadata validation
//! 
//! The PluginRegistry acts as a central repository for plugin information,
//! enabling plugins to discover and interact with each other while maintaining
//! a consistent metadata schema across the plugin system.

#pragma once

#include "PluginBase.hpp"
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <set>

namespace nlm {

class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();
    
    // Disable copying
    PluginRegistry(const PluginRegistry&) = delete;
    PluginRegistry& operator=(const PluginRegistry&) = delete;
    
    // Enable moving
    PluginRegistry(PluginRegistry&&) noexcept = default;
    PluginRegistry& operator=(PluginRegistry&&) noexcept = default;
    
    // Plugin registration
    bool registerPlugin(const PluginMetadata& metadata);
    bool registerBuiltInPlugin(const PluginMetadata& metadata);
    bool unregisterPlugin(const std::string& pluginName);
    bool updatePluginMetadata(const std::string& pluginName, const PluginMetadata& metadata);
    
    // Plugin discovery
    const PluginMetadata* findPlugin(const std::string& pluginName) const;
    const PluginMetadata* findPluginByType(const std::string& pluginType) const;
    std::vector<const PluginMetadata*> findPlugins(const std::string& pluginName) const;
    std::vector<const PluginMetadata*> findPluginsByType(const std::string& pluginType) const;
    std::vector<const PluginMetadata*> findPluginsByTag(const std::string& tag) const;
    
    // Plugin type categorization
    std::vector<std::string> getAllPluginTypes() const;
    std::vector<std::string> getPluginTypesForComponent(const std::string& component) const;
    bool isPluginTypeRegistered(const std::string& pluginType) const;
    
    // Plugin tag management
    bool addPluginTag(const std::string& pluginName, const std::string& tag);
    bool removePluginTag(const std::string& pluginName, const std::string& tag);
    std::unordered_set<std::string> getPluginTags(const std::string& pluginName) const;
    std::unordered_map<std::string, std::unordered_set<std::string>> getAllTags() const;
    
    // Plugin dependency management
    bool addPluginDependency(const std::string& pluginName, const std::string& dependency);
    bool removePluginDependency(const std::string& pluginName, const std::string& dependency);
    std::vector<std::string> getPluginDependencies(const std::string& pluginName) const;
    std::vector<std::string> getPluginOptionalDependencies(const std::string& pluginName) const;
    std::vector<std::string> getUnsatisfiedDependencies(const std::string& pluginName) const;
    
    // Version management
    bool checkVersionCompatibility(const std::string& pluginName, const PluginVersion& requiredVersion) const;
    std::vector<PluginVersion> getCompatibleVersions(const std::string& pluginName) const;
    bool updatePluginVersion(const std::string& pluginName, const PluginVersion& newVersion);
    
    // Metadata validation
    bool validatePluginMetadata(const PluginMetadata& metadata) const;
    std::vector<std::string> getPluginValidationErrors(const PluginMetadata& metadata) const;
    bool isPluginMetadataValid(const PluginMetadata& metadata) const;
    
    // Plugin configuration schema management
    bool addConfigParameter(const std::string& pluginName, const std::string& key, const std::any& schema);
    bool removeConfigParameter(const std::string& pluginName, const std::string& key);
    const std::any* getConfigParameterSchema(const std::string& pluginName, const std::string& key) const;
    std::unordered_map<std::string, std::any> getPluginConfigSchema(const std::string& pluginName) const;
    
    // Plugin capability management
    bool addPluginCapability(const std::string& pluginName, const std::string& capability);
    bool removePluginCapability(const std::string& pluginName, const std::string& capability);
    bool hasPluginCapability(const std::string& pluginName, const std::string& capability) const;
    std::vector<std::string> getPluginCapabilities(const std::string& pluginName) const;
    std::unordered_map<std::string, std::vector<std::string>> getAllCapabilities() const;
    
    // Plugin metadata queries
    std::vector<const PluginMetadata*> getAllPlugins() const;
    std::vector<const PluginMetadata*> getEnabledPlugins() const;
    std::vector<const PluginMetadata*> getDisabledPlugins() const;
    std::vector<const PluginMetadata*> getPluginsByAuthor(const std::string& author) const;
    std::vector<const PluginMetadata*> getPluginsByLicense(const std::string& license) const;
    
    // Plugin lifecycle tracking
    bool setPluginEnabled(const std::string& pluginName, bool enabled);
    bool isPluginEnabled(const std::string& pluginName) const;
    std::chrono::system_clock::time_point getPluginCreationTime(const std::string& pluginName) const;
    std::chrono::system_clock::time_point getPluginLastUpdatedTime(const std::string& pluginName) const;
    
    // Plugin loading statistics
    size_t getLoadCount(const std::string& pluginName) const;
    size_t getUnloadCount(const std::string& pluginName) const;
    size_t getLoadCount() const;
    size_t getUnloadCount() const;
    std::unordered_map<std::string, size_t> getLoadStatistics() const;
    
    // Plugin discovery from directory
    std::vector<std::string> discoverPlugins(const std::string& directoryPath);
    std::vector<std::string> discoverPlugins(const std::vector<std::string>& filePaths);
    bool validatePluginFile(const std::string& filePath) const;
    
    // Plugin metadata export/import
    bool exportPluginMetadata(const std::string& filePath) const;
    bool importPluginMetadata(const std::string& filePath);
    std::string exportPluginMetadataAsJSON(const std::string& pluginName) const;
    bool importPluginMetadataFromJSON(const std::string& pluginName, const std::string& jsonData);
    
    // Plugin compatibility checking
    bool checkAllPluginDependencies() const;
    std::unordered_map<std::string, std::vector<std::string>> getAllDependencyIssues() const;
    std::vector<std::string> getPluginsWithDependencyIssues() const;
    
    // Plugin type discovery
    static std::vector<std::string> getBuiltInPluginTypes();
    static std::vector<PluginMetadata> getBuiltInPluginMetadata();
    static bool isBuiltInPlugin(const std::string& pluginName);
    static PluginMetadata createBuiltInPluginMetadata(const std::string& name, const std::string& description,
                                                     const std::string& pluginType, const PluginVersion& version,
                                                     const std::string& author = "", const std::string& license = "MIT");
    
    // Registry management
    void clear();
    void clearBuiltInPlugins();
    size_t getPluginCount() const;
    size_t getEnabledPluginCount() const;
    size_t getDisabledPluginCount() const;
    
    // Registry statistics
    struct RegistryStats {
        size_t totalPlugins;
        size_t enabledPlugins;
        size_t disabledPlugins;
        size_t builtInPlugins;
        size_t externalPlugins;
        size_t pluginTypes;
        size_t pluginTags;
        size_t totalDependencies;
        double averagePluginVersion;
        
        RegistryStats() : totalPlugins(0), enabledPlugins(0), disabledPlugins(0),
            builtInPlugins(0), externalPlugins(0), pluginTypes(0), pluginTags(0),
            totalDependencies(0), averagePluginVersion(0.0) {}
    };
    
    RegistryStats getStatistics() const;
    
    // Plugin component type mapping
    static std::unordered_map<std::string, std::vector<std::string>> getComponentPluginMapping();
    static std::vector<std::string> getPluginsForComponent(const std::string& component);
    
    // Plugin loading state tracking
    bool setPluginLoadingState(const std::string& pluginName, PluginState state);
    PluginState getPluginLoadingState(const std::string& pluginName) const;
    std::unordered_map<std::string, PluginState> getAllPluginLoadingStates() const;
    
    // Error handling
    std::string getLastError() const;
    std::vector<std::string> getRecentErrors() const;
    void clearErrors();
    
    // Plugin metadata modification
    bool batchUpdatePluginMetadata(const std::unordered_map<std::string, PluginMetadata>& updates);
    bool batchRegisterPlugins(const std::vector<PluginMetadata>& metadataList);
    bool batchUnregisterPlugins(const std::vector<std::string>& pluginNames);
    
    // Plugin validation
    bool validateAllPluginMetadata() const;
    std::vector<std::pair<std::string, std::vector<std::string>>> getAllValidationErrors() const;
    
    // Plugin search and filtering
    std::vector<const PluginMetadata*> searchPlugins(const std::function<bool(const PluginMetadata&)>& predicate) const;
    std::vector<const PluginMetadata*> filterPlugins(const std::function<bool(const PluginMetadata&)>& filter) const;
    
    // Plugin configuration integration
    void setConfigurationSystem(std::shared_ptr<PluginConfiguration> configSystem);
    void setSecurityManager(std::shared_ptr<PluginSecurityManager> securityManager);
    void setUpdateManager(std::shared_ptr<PluginUpdateManager> updateManager);
    void setHookSystem(std::shared_ptr<HookSystem> hookSystem);
    void setEventSystem(std::shared_ptr<EventSystem> eventSystem);
    
    // Debug and monitoring
    void enableDebugLogging(bool enable);
    bool isDebugLoggingEnabled() const;
    void logRegistryOperation(const std::string& operation, const std::string& pluginName = "") const;
    void logRegistryError(const std::string& error) const;
    
    // Plugin metadata export/import helpers
    static std::string metadataToJSON(const PluginMetadata& metadata);
    static PluginMetadata metadataFromJSON(const std::string& jsonData);
    static std::string configToJSON(const PluginConfig& config);
    static PluginConfig configFromJSON(const std::string& jsonData);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void initializeImpl();
    void cleanupImpl();
    
    // Thread-safe operations
    template<typename T>
    T withRegistryLock(T func) {
        std::unique_lock<std::mutex> lock(registryMutex_);
        return func();
    }
}; // namespace nlm