#ifndef NLM_PLUGIN_PLUGINREGISTRY_HPP
#define NLM_PLUGIN_PLUGINREGISTRY_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <shared_mutex>
#include <algorithm>
#include <filesystem>
#include "PluginBase.hpp"

namespace nlm {
namespace plugin {

class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();
    
    // Plugin metadata management
    bool registerPluginMetadata(const PluginMetadata& metadata);
    bool unregisterPluginMetadata(const std::string& name);
    bool updatePluginMetadata(const PluginMetadata& metadata);
    
    // Plugin lookup
    PluginMetadata getPluginMetadata(const std::string& name) const;
    std::vector<PluginMetadata> getAllPlugins() const;
    std::vector<PluginMetadata> getPluginsByType(PluginType type) const;
    std::vector<PluginMetadata> getPluginsByAuthor(const std::string& author) const;
    std::vector<PluginMetadata> getPluginsByVersion(const PluginVersion& version) const;
    std::vector<PluginMetadata> getCompatiblePlugins(const PluginVersion& minVersion) const;
    
    // Plugin discovery
    bool addSearchPath(const std::string& path);
    bool removeSearchPath(const std::string& path);
    std::vector<std::string> getSearchPaths() const;
    std::vector<PluginMetadata> discoverPlugins() const;
    std::vector<PluginMetadata> discoverPlugins(const std::vector<std::string>& directories) const;
    
    // Plugin validation
    bool validateMetadata(const PluginMetadata& metadata) const;
    bool validatePluginSignature(const std::string& name, const std::string& signature) const;
    bool checkVersionCompatibility(const PluginMetadata& metadata, const PluginVersion& hostVersion) const;
    bool checkDependencies(const PluginMetadata& metadata, const std::unordered_map<std::string, PluginMetadata>& availablePlugins) const;
    
    // Plugin lifecycle
    bool enablePlugin(const std::string& name);
    bool disablePlugin(const std::string& name);
    bool setPluginPriority(const std::string& name, int priority);
    int getPluginPriority(const std::string& name) const;
    
    // Plugin statistics and monitoring
    struct PluginStats {
        std::string name;
        PluginState state;
        std::chrono::system_clock::time_point lastUpdate;
        uint64_t loadCount = 0;
        uint64_t activateCount = 0;
        uint64_t shutdownCount = 0;
        uint64_t errorCount = 0;
        std::string lastError;
        std::chrono::duration<double> averageLoadTime;
        std::chrono::duration<double> averageActivateTime;
        std::chrono::duration<double> averageShutdownTime;
    };
    
    std::vector<PluginStats> getAllPluginStats() const;
    PluginStats getPluginStats(const std::string& name) const;
    
    // Plugin versioning
    bool setPluginVersion(const std::string& name, const PluginVersion& version);
    PluginVersion getPluginVersion(const std::string& name) const;
    std::vector<PluginVersion> getAvailableVersions(const std::string& name) const;
    bool addPluginVersion(const std::string& name, const PluginVersion& version);
    
    // Plugin migration support
    bool supportsMigration(const std::string& name) const;
    bool migratePlugin(const std::string& name, const PluginVersion& targetVersion);
    std::vector<std::string> getMigrationPath(const std::string& name, const PluginVersion& targetVersion) const;
    
    // Plugin conflict detection and resolution
    bool checkAllConflicts();
    std::vector<std::pair<std::string, std::string>> getConflicts() const;
    bool resolveConflicts();
    
    // Plugin testing and validation
    bool validatePlugin(const std::string& name) const;
    bool runPluginTests(const std::string& name) const;
    std::vector<std::string> getPluginTestResults(const std::string& name) const;
    
    // Plugin security and isolation
    bool setPluginSecurityPolicy(const std::string& name, const std::string& policy);
    std::string getPluginSecurityPolicy(const std::string& name) const;
    bool enablePluginSandboxing(const std::string& name, bool enabled);
    bool isPluginSandboxed(const std::string& name) const;
    
    // Plugin resource management
    bool setPluginResourceLimit(const std::string& name, const std::string& resource, uint64_t limit);
    uint64_t getPluginResourceUsage(const std::string& name, const std::string& resource) const;
    
    // Plugin event system
    using PluginEventCallback = std::function<void(const std::string& name, PluginEvent event, const std::string& data)>;
    void addEventCallback(const PluginEventCallback& callback);
    void removeEventCallback(const PluginEventCallback& callback);
    void emitPluginEvent(PluginEvent event, const std::string& pluginName, const std::string& data);
    
    // Plugin configuration
    bool configurePlugin(const std::string& name, const std::unordered_map<std::string, std::string>& config);
    std::unordered_map<std::string, std::string> getPluginConfig(const std::string& name) const;
    
private:
    struct PluginEntry {
        PluginMetadata metadata;
        PluginStats stats;
        std::unordered_map<std::string, std::string> configuration;
        std::chrono::system_clock::time_point lastUpdate;
        bool enabled;
        int priority;
        std::string securityPolicy;
        bool sandboxed;
        std::unordered_map<std::string, uint64_t> resourceLimits;
        std::unordered_map<std::string, uint64_t> resourceUsage;
        std::vector<PluginVersion> versions;
        std::vector<std::string> testResults;
    };
    
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, PluginEntry> plugins_;
    std::vector<std::string> searchPaths_;
    std::vector<PluginEventCallback> eventCallbacks_;
    
    bool validatePluginMetadata(const PluginMetadata& metadata) const;
    std::vector<PluginMetadata> scanDirectoryForPlugins(const std::string& directory) const;
    PluginMetadata parsePluginManifest(const std::string& manifestPath) const;
    bool writePluginManifest(const std::string& pluginPath) const;
    
    void updatePluginStats(const std::string& name, PluginState state);
    void notifyEventCallbacks(PluginEvent event, const std::string& pluginName, const std::string& data);
};

// PluginComposer for combining multiple plugins
class PluginComposer {
public:
    PluginComposer();
    ~PluginComposer();
    
    // Plugin composition
    bool composePlugins(const std::vector<std::string>& pluginNames);
    bool composePlugin(const std::string& pluginName, const std::string& compositionName);
    bool decomposePlugin(const std::string& composedPluginName, const std::string& componentPluginName);
    
    // Plugin nesting
    bool nestPlugin(const std::string& parentName, const std::string& childName);
    bool unnestPlugin(const std::string& parentName, const std::string& childName);
    std::vector<std::string> getPluginChildren(const std::string& parentName) const;
    std::string getPluginParent(const std::string& childName) const;
    
    // Plugin inheritance
    bool setPluginInheritance(const std::string& childName, const std::string& parentName);
    std::vector<std::string> getPluginInheritanceChain(const std::string& name) const;
    
    // Plugin combination
    bool combinePlugins(const std::vector<std::string>& pluginNames, const std::string& combinedName);
    bool splitPlugin(const std::string& combinedName, std::vector<std::string>& componentNames);
    
    // Plugin validation
    bool validateComposition(const std::vector<std::string>& pluginNames) const;
    bool validateNesting(const std::string& parentName, const std::string& childName) const;
    
private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, std::vector<std::string>> pluginChildren_;
    std::unordered_map<std::string, std::string> pluginParent_;
    std::unordered_map<std::string, std::vector<std::string>> pluginInheritance_;
    std::unordered_map<std::string, std::vector<std::string>> pluginComposition_;
};

// PluginValidator for plugin validation
class PluginValidator {
public:
    PluginValidator();
    ~PluginValidator();
    
    // Plugin validation
    bool validatePlugin(const std::string& name) const;
    bool validatePlugin(const PluginMetadata& metadata) const;
    std::vector<std::string> getValidationErrors(const std::string& name) const;
    std::vector<std::string> getValidationWarnings(const std::string& name) const;
    
    // Plugin testing
    bool runTests(const std::string& name) const;
    std::vector<std::string> getTestResults(const std::string& name) const;
    std::vector<std::string> getTestFailures(const std::string& name) const;
    
    // Plugin compatibility
    bool checkCompatibility(const std::string& name, const PluginVersion& hostVersion) const;
    bool checkDependencies(const std::string& name, const std::unordered_map<std::string, PluginMetadata>& availablePlugins) const;
    
    // Plugin security
    bool checkPluginSecurity(const std::string& name) const;
    bool checkPluginSandboxing(const std::string& name) const;
    bool checkPluginResourceLimits(const std::string& name) const;
    
    // Plugin performance
    bool checkPluginPerformance(const std::string& name) const;
    std::chrono::duration<double> getPluginLoadTime(const std::string& name) const;
    std::chrono::duration<double> getPluginActivateTime(const std::string& name) const;
    
private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, std::vector<std::string>> validationErrors_;
    std::unordered_map<std::string, std::vector<std::string>> validationWarnings_;
    std::unordered_map<std::string, std::vector<std::string>> testResults_;
    std::unordered_map<std::string, std::chrono::duration<double>> loadTimes_;
    std::unordered_map<std::string, std::chrono::duration<double>> activateTimes_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_PLUGINREGISTRY_HPP