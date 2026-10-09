// Plugin Loader and Manager Implementation
// Comprehensive plugin management system for NLM

#pragma once

#include "UPIP.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <dlfcn.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>

namespace nlm {
namespace plugins {

// Plugin file extension
static const std::string PLUGIN_FILE_EXTENSION = ".nlmplugin";
static const std::string PLUGIN_METADATA_FILE = "plugin.json";
static const std::string PLUGIN_SIGNATURE_FILE = "plugin.sig";
static const int MAX_PLUGIN_SIZE = 10 * 1024 * 1024; // 10MB

// Plugin loader implementation
class PluginLoader {
public:
    virtual ~PluginLoader() = default;
    
    // Load plugin from file
    virtual std::unique_ptr<IPlugin> loadPlugin(const std::string& pluginPath) = 0;
    
    // Unload plugin
    virtual bool unloadPlugin(std::unique_ptr<IPlugin>& plugin) = 0;
    
    // Check if loader can handle the file
    virtual bool canLoad(const std::string& pluginPath) const = 0;
    
    // Get loader capabilities
    virtual std::vector<std::string> getCapabilities() const = 0;
    
    // Get loader type
    virtual std::string getType() const = 0;
};

// Dynamic library plugin loader
class DynamicLibraryPluginLoader : public PluginLoader {
public:
    DynamicLibraryPluginLoader() = default;
    ~DynamicLibraryPluginLoader() override {
        cleanup();
    }
    
    std::unique_ptr<IPlugin> loadPlugin(const std::string& pluginPath) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Check file existence and size
        if (!std::filesystem::exists(pluginPath)) {
            throw std::runtime_error("Plugin file does not exist: " + pluginPath);
        }
        
        auto fileSize = std::filesystem::file_size(pluginPath);
        if (fileSize > MAX_PLUGIN_SIZE) {
            throw std::runtime_error("Plugin file too large: " + pluginPath);
        }
        
        // Open dynamic library
        void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
        if (!handle) {
            std::string error = "Failed to load plugin: " + std::string(dlerror());
            throw std::runtime_error(error);
        }
        
        // Get plugin factory function
        using CreatePluginFunc = std::unique_ptr<IPlugin>(*)();
        auto createFunc = reinterpret_cast<CreatePluginFunc>(dlsym(handle, "create_plugin"));
        if (!createFunc) {
            dlclose(handle);
            std::string error = "Plugin does not export create_plugin function: " + std::string(dlerror());
            throw std::runtime_error(error);
        }
        
        // Create plugin instance
        auto plugin = createFunc();
        if (!plugin) {
            dlclose(handle);
            throw std::runtime_error("Failed to create plugin instance");
        }
        
        // Store handle for later cleanup
        pluginHandles_[plugin->getId()] = handle;
        
        return plugin;
    }
    
    bool unloadPlugin(std::unique_ptr<IPlugin>& plugin) override {
        if (!plugin) return false;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = pluginHandles_.find(plugin->getId());
        if (it != pluginHandles_.end()) {
            dlclose(it->second);
            pluginHandles_.erase(it);
        }
        
        plugin.reset();
        return true;
    }
    
    bool canLoad(const std::string& pluginPath) const override {
        // Check file extension
        if (!std::filesystem::exists(pluginPath)) {
            return false;
        }
        
        std::filesystem::path path(pluginPath);
        std::string extension = path.extension().string();
        return extension == PLUGIN_FILE_EXTENSION;
    }
    
    std::vector<std::string> getCapabilities() const override {
        return {"Dynamic library loading", "Plugin factory functions", "Runtime plugin management"};
    }
    
    std::string getType() const override {
        return "DynamicLibrary";
    }
    
    void cleanup() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& pair : pluginHandles_) {
            dlclose(pair.second);
        }
        pluginHandles_.clear();
    }
    
private:
    std::mutex mutex_;
    std::unordered_map<std::string, void*> pluginHandles_;
};

// Plugin manager implementation
class PluginManager : public IPluginManager {
public:
    PluginManager(std::shared_ptr<IPluginSecurityManager> securityManager,
                  std::shared_ptr<IPluginVersionManager> versionManager);
    ~PluginManager() override;
    
    // IPluginManager implementation
    bool loadPlugin(const std::string& pluginPath) override;
    bool unloadPlugin(const std::string& pluginId) override;
    bool activatePlugin(const std::string& pluginId) override;
    bool deactivatePlugin(const std::string& pluginId) override;
    std::vector<std::string> getLoadedPlugins() const override;
    
    PluginMetadata getPluginMetadata(const std::string& pluginId) const override;
    PluginStatistics getPluginStatistics(const std::string& pluginId) const override;
    std::vector<PluginMetadata> getAllPluginMetadata() const override;
    
    json executePlugin(const std::string& pluginId, const std::string& operation, const json& parameters) override;
    bool handlePluginEvent(const std::string& pluginId, const std::string& eventName, const json& eventData) override;
    
    std::shared_ptr<IPluginScripting> getPluginScripting(const std::string& pluginId) override;
    ScriptResult executeScript(const std::string& pluginId, const std::string& script, const ScriptContext& context) override;
    
    bool checkDependencies(const std::string& pluginId) const override;
    std::vector<std::string> getMissingDependencies(const std::string& pluginId) const override;
    
    bool validatePlugin(const std::string& pluginPath) const override;
    bool verifyPluginSignature(const std::string& pluginPath) const override;
    
    bool updatePluginConfig(const std::string& pluginId, const json& config) override;
    json getPluginConfig(const std::string& pluginId) const override;
    
    std::vector<std::string> discoverPlugins(const std::string& directory) override;
    bool registerPlugin(const PluginMetadata& metadata) override;
    
    bool hotSwapPlugin(const std::string& pluginId) override;
    bool canHotSwap(const std::string& pluginId) const override;
    
    void updatePluginStatistics(const std::string& pluginId, const PluginStatistics& stats) override;
    std::map<std::string, PluginStatistics> getAllPluginStatistics() const override;
    
    std::vector<std::string> searchPlugins(const std::string& query) const override;
    std::vector<std::string> filterPluginsByCapability(const PluginCapability& capability) const override;
    std::vector<std::string> filterPluginsByCategory(const std::string& category) const override;
    
    // Additional functionality
    bool savePluginMetadata(const PluginMetadata& metadata);
    bool loadPluginMetadata(const PluginMetadata& metadata);
    
    // Plugin discovery and scanning
    std::vector<std::string> scanDirectoryForPlugins(const std::string& directory, bool recursive = true);
    bool isPluginValid(const PluginMetadata& metadata) const;
    
    // Plugin signature verification
    bool verifyPluginSignature(const std::string& pluginPath, const std::string& signature) const;
    std::string generatePluginSignature(const std::string& pluginPath) const;
    
    // Plugin dependency resolution
    bool resolveDependencies(const std::string& pluginId, const std::vector<PluginDependency>& dependencies);
    bool installMissingDependencies(const std::string& pluginId);
    
    // Plugin security
    bool enforcePluginSecurity(const std::string& pluginId);
    bool quarantinePlugin(const std::string& pluginId);
    bool restorePlugin(const std::string& pluginId);
    
    // Plugin statistics and monitoring
    void logPluginActivity(const std::string& pluginId, const std::string& activity, const json& details);
    std::vector<json> getPluginActivityLog(const std::string& pluginId, const std::string& activity) const;
    
    // Plugin configuration
    void setPluginConfigPath(const std::string& configPath);
    json loadPluginDefaultConfig(const std::string& pluginId) const;
    bool savePluginConfig(const std::string& pluginId, const json& config);
    
    // Plugin event handling
    void registerPluginEventHandler(const std::string& pluginId, const std::string& eventType, std::function<void(const json&)> handler);
    void unregisterPluginEventHandler(const std::string& pluginId, const std::string& eventType);
    
    // Plugin scripting interface
    bool registerScriptEngine(const std::string& language, std::shared_ptr<IPluginScripting> scriptingEngine);
    std::shared_ptr<IPluginScripting> getScriptEngine(const std::string& language) const;
    
    // Plugin monitoring
    bool monitorPlugin(const std::string& pluginId, bool enabled);
    bool isPluginMonitored(const std::string& pluginId) const;
    json getPluginHealthStatus(const std::string& pluginId) const;
    
    // Plugin lifecycle
    void cleanupPlugins();
    bool restartPlugin(const std::string& pluginId);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    std::string generatePluginId(const std::string& pluginPath) const;
    PluginMetadata extractPluginMetadata(const std::string& pluginPath) const;
    std::unique_ptr<IPlugin> createPluginFromPath(const std::string& pluginPath);
    bool validatePluginBinary(const std::string& pluginPath) const;
    std::string computePluginHash(const std::string& pluginPath) const;
    bool savePluginSignature(const std::string& pluginPath, const std::string& signature) const;
    bool loadPluginSignature(const std::string& pluginPath, std::string& signature) const;
};

// Plugin script engine
class PluginScriptEngine : public IPluginScripting {
public:
    PluginScriptEngine(std::shared_ptr<IPluginManager> pluginManager);
    ~PluginScriptEngine() override;
    
    // IPluginScripting implementation
    ScriptResult executeScript(const std::string& script, const ScriptContext& context) override;
    ScriptResult executeScriptFile(const std::string& filepath, const ScriptContext& context) override;
    
    std::vector<std::string> listScripts() const override;
    bool saveScript(const std::string& scriptId, const std::string& script, const std::string& language) override;
    bool deleteScript(const std::string& scriptId) override;
    std::string getScript(const std::string& scriptId) const override;
    
    ScriptContext createContext(const std::string& pluginId, const std::string& language, bool sandboxed = true) override;
    ScriptResult evaluateExpression(const std::string& expression, const ScriptContext& context) override;
    
    bool addBreakpoint(const std::string& scriptId, int lineNumber) override;
    bool removeBreakpoint(const std::string& scriptId, int lineNumber) override;
    std::vector<int> getBreakpoints(const std::string& scriptId) const override;
    void setDebugMode(bool enabled) override;
    bool isDebugMode() const override;
    
    void startProfiling(const std::string& scriptId) override;
    void stopProfiling(const std::string& scriptId) override;
    json getProfilingData(const std::string& scriptId) const override;
    void clearProfilingData(const std::string& scriptId) override;
    
    bool validateScript(const std::string& script, const std::string& language) const override;
    std::string lintScript(const std::string& script, const std::string& language) const override;
    
    std::vector<std::string> getErrorHistory(const std::string& scriptId) const override;
    void clearErrorHistory(const std::string& scriptId) override;
    json getLastError(const std::string& scriptId) const override;
    
    // Script language support
    bool registerScriptLanguage(const std::string& language, std::function<ScriptResult(const std::string&, const ScriptContext&)> executor);
    
    // Script context management
    std::vector<std::string> getActiveScriptContexts() const;
    void cleanupScriptContext(const std::string& scriptId);
    
    // Script security
    bool validateScriptSecurity(const std::string& script, const ScriptContext& context) const;
    bool sandboxScript(const std::string& script, const ScriptContext& context);
    
    // Script performance
    void recordScriptPerformance(const std::string& scriptId, const ScriptResult& result);
    json getScriptPerformanceStats(const std::string& scriptId) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    std::string generateScriptId() const;
    bool validateScriptSyntax(const std::string& script, const std::string& language) const;
    std::string getLanguageFromExtension(const std::string& filepath) const;
};

// Plugin security manager implementation
class PluginSecurityManager : public IPluginSecurityManager {
public:
    PluginSecurityManager(std::shared_ptr<IPluginManager> pluginManager);
    ~PluginSecurityManager() override;
    
    // IPluginSecurityManager implementation
    bool validatePluginIntegrity(const std::string& pluginPath) const override;
    bool verifyPluginSignature(const std::string& pluginPath, const std::string& signature) const override;
    std::string generatePluginSignature(const std::string& pluginPath) const override;
    
    bool checkPluginPermissions(const std::string& pluginId, PluginPermission requiredPermission) const override;
    bool grantPluginPermission(const std::string& pluginId, PluginPermission permission) override;
    bool revokePluginPermission(const std::string& pluginId, PluginPermission permission) override;
    PluginPermission getPluginPermission(const std::string& pluginId) const override;
    
    bool enablePluginSandboxing(const std::string& pluginId) override;
    bool disablePluginSandboxing(const std::string& pluginId) override;
    bool isPluginSandboxed(const std::string& pluginId) const override;
    void setPluginIsolationLevel(const std::string& pluginId, int level) override;
    int getPluginIsolationLevel(const std::string& pluginId) const override;
    
    bool addPluginToWhitelist(const std::string& pluginId) override;
    bool removePluginFromWhitelist(const std::string& pluginId) override;
    bool isPluginWhitelisted(const std::string& pluginId) const override;
    bool addPluginToBlacklist(const std::string& pluginId) override;
    bool removePluginFromBlacklist(const std::string& pluginId) override;
    bool isPluginBlacklisted(const std::string& pluginId) const override;
    
    void logSecurityEvent(const std::string& pluginId, const std::string& eventType, const json& eventData) override;
    std::vector<json> getSecurityEvents(const std::string& pluginId, const std::string& eventType) const override;
    
    bool recoverPlugin(const std::string& pluginId) override;
    bool quarantinePlugin(const std::string& pluginId) override;
    bool restorePlugin(const std::string& pluginId) override;
    
    // Additional security features
    bool scanPluginForMaliciousContent(const std::string& pluginPath) const;
    bool validatePluginCodeQuality(const std::string& pluginPath) const;
    bool checkPluginCompliance(const std::string& pluginId) const;
    bool auditPluginChanges(const std::string& pluginId, const json& changes) const;
    
    // Plugin recovery and backup
    bool createPluginBackup(const std::string& pluginId);
    bool restorePluginFromBackup(const std::string& pluginId);
    bool deletePluginBackup(const std::string& pluginId);
    std::vector<std::string> getPluginBackups(const std::string& pluginId) const;
    
    // Plugin vulnerability management
    bool checkPluginVulnerabilities(const std::string& pluginId) const;
    bool patchPluginVulnerabilities(const std::string& pluginId, const json& patches);
    bool reportPluginVulnerability(const std::string& pluginId, const json& vulnerability);
    
    // Plugin integrity verification
    bool verifyPluginIntegrity(const std::string& pluginPath, const std::string& checksum) const;
    bool computePluginIntegrity(const std::string& pluginPath, std::string& checksum) const;
    
    // Plugin privacy protection
    bool maskPluginSensitiveData(const std::string& pluginId, const json& data) const;
    bool validatePluginPrivacyPolicy(const std::string& pluginId) const;
    
    // Plugin compliance
    bool validatePluginCompliance(const std::string& pluginId, const std::string& standard) const;
    std::vector<std::string> getPluginComplianceIssues(const std::string& pluginId) const;
    
    // Plugin monitoring
    bool monitorPluginSecurity(const std::string& pluginId, bool enabled);
    bool isPluginSecurityMonitored(const std::string& pluginId) const;
    json getPluginSecurityStatus(const std::string& pluginId) const;
    
    // Plugin recovery and restoration
    bool automaticPluginRecovery(const std::string& pluginId, const json& recoveryData);
    bool manualPluginRecovery(const std::string& pluginId);
    bool preventPluginRecovery(const std::string& pluginId);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    std::string computeFileHash(const std::string& filepath) const;
    bool checkFilePermissions(const std::string& filepath) const;
    bool validateFileSignatures(const std::string& filepath) const;
    bool scanFileForMalware(const std::string& filepath) const;
    std::string getFileInformation(const std::string& filepath) const;
    
    // Security event logging
    void logSecurityEventInternal(const std::string& pluginId, const std::string& eventType, const json& eventData, bool isSecurityEvent = true);
    
    // Plugin backup management
    std::string getPluginBackupPath(const std::string& pluginId) const;
    std::string getPluginBackupDirectory() const;
    
    // Plugin quarantine management
    std::string getPluginQuarantinePath(const std::string& pluginId) const;
    std::string getPluginQuarantineDirectory() const;
    
    // Plugin vulnerability database
    bool loadVulnerabilityDatabase();
    bool saveVulnerabilityDatabase();
    std::vector<json> getPluginVulnerabilities(const std::string& pluginId) const;
    
    // Plugin compliance database
    bool loadComplianceDatabase();
    bool saveComplianceDatabase();
    std::vector<std::string> getPluginComplianceStandards(const std::string& pluginId) const;
};

// Plugin version manager implementation
class PluginVersionManager : public IPluginVersionManager {
public:
    PluginVersionManager(std::shared_ptr<IPluginManager> pluginManager);
    ~PluginVersionManager() override;
    
    // IPluginVersionManager implementation
    bool registerPluginVersion(const std::string& pluginId, const PluginVersion& version, const std::string& description) override;
    bool unregisterPluginVersion(const std::string& pluginId, const PluginVersion& version) override;
    PluginVersion getLatestVersion(const std::string& pluginId) const override;
    std::vector<PluginVersion> getAvailableVersions(const std::string& pluginId) const override;
    
    bool checkForUpdates(const std::string& pluginId) override;
    bool downloadUpdate(const std::string& pluginId, const PluginVersion& version) override;
    bool installUpdate(const std::string& pluginId, const PluginVersion& version) override;
    
    bool rollbackPlugin(const std::string& pluginId, const PluginVersion& version) override;
    std::vector<PluginVersion> getRollbackVersions(const std::string& pluginId) const override;
    
    // Additional functionality
    bool createPluginVersionSnapshot(const std::string& pluginId, const PluginVersion& version);
    bool restorePluginVersionSnapshot(const std::string& pluginId, const PluginVersion& version);
    std::vector<PluginVersion> getPluginVersionSnapshots(const std::string& pluginId) const;
    
    // Plugin update management
    bool validatePluginUpdate(const std::string& pluginId, const PluginVersion& newVersion) const;
    bool preparePluginUpdate(const std::string& pluginId, const PluginVersion& newVersion);
    bool commitPluginUpdate(const std::string& pluginId, const PluginVersion& newVersion);
    
    // Plugin version comparison
    bool isVersionCompatible(const PluginVersion& current, const PluginVersion& required) const;
    int compareVersions(const PluginVersion& v1, const PluginVersion& v2) const;
    
    // Plugin changelog management
    bool addPluginChangelogEntry(const std::string& pluginId, const PluginVersion& version, const std::string& entry);
    std::vector<std::string> getPluginChangelog(const std::string& pluginId, const PluginVersion& version) const;
    std::vector<std::pair<PluginVersion, std::string>> getPluginChangelog(const std::string& pluginId) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    std::string getPluginVersionDirectory(const std::string& pluginId, const PluginVersion& version) const;
    bool savePluginVersionMetadata(const std::string& pluginId, const PluginVersion& version, const json& metadata);
    bool loadPluginVersionMetadata(const std::string& pluginId, const PluginVersion& version, json& metadata);
    
    // Plugin update management
    std::string getPluginUpdateDirectory(const std::string& pluginId) const;
    bool downloadPluginUpdate(const std::string& pluginId, const PluginVersion& version, const std::string& updateUrl);
    bool validatePluginUpdateSignature(const std::string& pluginId, const PluginVersion& version, const std::string& signature) const;
    
    // Plugin version snapshots
    std::string getPluginVersionSnapshotPath(const std::string& pluginId, const PluginVersion& version) const;
    bool createPluginVersionSnapshot(const std::string& pluginId, const PluginVersion& version, const json& snapshotData);
    bool loadPluginVersionSnapshot(const std::string& pluginId, const PluginVersion& version, json& snapshotData);
};

// Plugin factory implementation
class PluginFactory {
public:
    static std::unique_ptr<IPlugin> createPlugin(const std::string& pluginType, const json& config);
    static bool registerPluginFactory(const std::string& pluginType, std::function<std::unique_ptr<IPlugin>(const json&)> factory);
    static bool unregisterPluginFactory(const std::string& pluginType);
    static std::vector<std::string> getRegisteredPluginTypes();
    static bool canCreatePlugin(const std::string& pluginType);
    
    // Plugin class registration
    template<typename T, typename... Args>
    static bool registerPluginClass(const std::string& pluginType, const std::string& name);
    static std::string getPluginClassName(const std::string& pluginType);
    
    // Plugin instance management
    static void cleanupPluginInstances();
    static bool destroyPlugin(std::unique_ptr<IPlugin>& plugin);
    static std::vector<std::string> getActivePluginInstances();
    
private:
    static std::unordered_map<std::string, std::function<std::unique_ptr<IPlugin>(const json&)>> pluginFactories_;
    static std::unordered_map<std::string, std::string> pluginClassNames_;
    static std::mutex factoryMutex_;
};

// Plugin registry implementation
class PluginRegistry {
public:
    static PluginRegistry& getInstance();
    
    // Plugin registration
    bool registerPlugin(const PluginMetadata& metadata);
    bool unregisterPlugin(const std::string& pluginId);
    bool updatePluginMetadata(const PluginMetadata& metadata);
    
    // Plugin discovery
    std::vector<PluginMetadata> discoverPlugins(const std::string& directory = "plugins");
    std::vector<PluginMetadata> getRegisteredPlugins() const;
    PluginMetadata getPluginMetadata(const std::string& pluginId) const;
    
    // Plugin search
    std::vector<PluginMetadata> searchPlugins(const std::string& query) const;
    std::vector<PluginMetadata> filterPluginsByCapability(const PluginCapability& capability) const;
    std::vector<PluginMetadata> filterPluginsByCategory(const std::string& category) const;
    std::vector<PluginMetadata> filterPluginsByAuthor(const std::string& author) const;
    
    // Plugin information
    std::vector<std::string> getAllPluginIds() const;
    std::vector<std::string> getCategories() const;
    std::vector<std::string> getAuthors() const;
    
    // Plugin lifecycle management
    bool loadPlugin(const std::string& pluginId, const std::string& pluginPath = "");
    bool unloadPlugin(const std::string& pluginId);
    bool activatePlugin(const std::string& pluginId);
    bool deactivatePlugin(const std::string& pluginId);
    
    // Plugin configuration
    bool updatePluginConfig(const std::string& pluginId, const json& config);
    json getPluginConfig(const std::string& pluginId) const;
    
    // Plugin security
    bool validatePlugin(const std::string& pluginId, const std::string& pluginPath = "");
    bool verifyPluginSignature(const std::string& pluginId, const std::string& pluginPath = "");
    
    // Plugin monitoring
    PluginStatistics getPluginStatistics(const std::string& pluginId) const;
    std::map<std::string, PluginStatistics> getAllPluginStatistics() const;
    
    // Plugin event handling
    bool registerEventHandler(const std::string& pluginId, const std::string& eventType, std::function<void(const json&)> handler);
    bool unregisterEventHandler(const std::string& pluginId, const std::string& eventType);
    void notifyEventHandlers(const std::string& eventType, const json& eventData);
    
    // Plugin scripting
    ScriptResult executeScript(const std::string& pluginId, const std::string& script, const ScriptContext& context);
    bool registerScriptEngine(const std::string& language, std::shared_ptr<IPluginScripting> scriptingEngine);
    std::shared_ptr<IPluginScripting> getScriptEngine(const std::string& language) const;
    
    // Plugin dependency management
    bool checkDependencies(const std::string& pluginId) const;
    std::vector<std::string> getMissingDependencies(const std::string& pluginId) const;
    
    // Plugin cleanup
    void cleanup();
    
private:
    PluginRegistry();
    ~PluginRegistry();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};