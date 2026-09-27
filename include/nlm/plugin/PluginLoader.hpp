#ifndef NLM_PLUGIN_PLUGINLOADER_HPP
#define NLM_PLUGIN_PLUGINLOADER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include "PluginBase.hpp"

namespace nlm {
namespace plugin {

class PluginLoader {
public:
    PluginLoader();
    ~PluginLoader();
    
    // Load plugins from various sources
    bool loadPlugin(const std::string& path);
    bool loadPluginFromMemory(const std::string& name, const std::vector<uint8_t>& data);
    bool loadPluginFromSource(const std::string& name, const std::string& sourceCode);
    
    // Unload plugins
    bool unloadPlugin(const std::string& name);
    void unloadAllPlugins();
    
    // Plugin discovery
    std::vector<std::string> discoverPlugins(const std::string& directory) const;
    std::vector<std::string> discoverPlugins(const std::vector<std::string>& directories) const;
    
    // Plugin information
    std::string getPluginName(const std::string& path) const;
    PluginVersion getPluginVersion(const std::string& path) const;
    std::vector<std::string> getDependencies(const std::string& path) const;
    PluginType getPluginType(const std::string& path) const;
    std::string getPluginDescription(const std::string& path) const;
    
    // Plugin validation
    bool validatePlugin(const std::string& path) const;
    bool validatePlugin(const std::vector<uint8_t>& data) const;
    
    // Plugin creation
    std::unique_ptr<IPlugin> createPlugin(const std::string& name, const std::string& path);
    
    // Plugin state
    PluginState getPluginState(const std::string& name) const;
    std::string getPluginError(const std::string& name) const;
    
    // Plugin configuration
    bool configurePlugin(const std::string& name, const PluginMetadata& metadata);
    PluginMetadata getPluginConfiguration(const std::string& name) const;
    
    // Plugin lifecycle management
    bool initializePlugin(const std::string& name);
    bool activatePlugin(const std::string& name);
    bool shutdownPlugin(const std::string& name);
    
    // Plugin extension access
    template<typename TExtension>
    std::shared_ptr<TExtension> getPluginExtension(const std::string& pluginName, const std::string& extensionName);
    
    // Callback functions
    using PluginLoadCallback = std::function<void(const std::string& name, bool success, const std::string& error)>;
    using PluginStateChangeCallback = std::function<void(const std::string& name, PluginState oldState, PluginState newState)>;
    
    void addLoadCallback(const PluginLoadCallback& callback);
    void addStateChangeCallback(const PluginStateChangeCallback& callback);
    
    // Plugin statistics
    struct PluginStats {
        std::string name;
        PluginState state;
        std::chrono::system_clock::time_point loadTime;
        std::chrono::system_clock::time_point lastActivity;
        uint64_t loadCount = 0;
        uint64_t activateCount = 0;
        uint64_t shutdownCount = 0;
        uint64_t errorCount = 0;
        std::string lastError;
    };
    
    std::vector<PluginStats> getAllPluginStats() const;
    
    // Plugin security and isolation
    bool setPluginSecurityPolicy(const std::string& name, const std::string& policy);
    std::string getPluginSecurityPolicy(const std::string& name) const;
    
    // Plugin sandboxing
    bool enablePluginSandboxing(const std::string& name, bool enabled);
    bool isPluginSandboxed(const std::string& name) const;
    
    // Plugin resource management
    bool setPluginResourceLimit(const std::string& name, const std::string& resource, uint64_t limit);
    uint64_t getPluginResourceUsage(const std::string& name, const std::string& resource) const;
    
    // Plugin dependency management
    bool addPluginDependency(const std::string& plugin, const std::string& dependency);
    bool removePluginDependency(const std::string& plugin, const std::string& dependency);
    std::vector<std::string> getPluginDependencies(const std::string& name) const;
    
    // Plugin conflict detection
    bool checkAllConflicts();
    std::vector<std::pair<std::string, std::string>> getConflicts() const;
    
private:
    struct PluginEntry {
        std::unique_ptr<IPlugin> plugin;
        std::string path;
        PluginState state;
        std::string error;
        std::chrono::system_clock::time_point loadTime;
        std::unordered_map<std::string, std::string> configuration;
        std::vector<std::string> dependencies;
        std::vector<std::string> conflictingPlugins;
        bool sandboxed;
        std::unordered_map<std::string, uint64_t> resourceLimits;
        std::unordered_map<std::string, uint64_t> resourceUsage;
        std::string securityPolicy;
    };
    
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, PluginEntry> plugins_;
    bool threadSafe_;
    
    std::vector<PluginLoadCallback> loadCallbacks_;
    std::vector<PluginStateChangeCallback> stateChangeCallbacks_;
    
    void notifyLoadCallbacks(const std::string& name, bool success, const std::string& error);
    void notifyStateChangeCallbacks(const std::string& name, PluginState oldState, PluginState newState);
    
    bool loadDynamicLibrary(const std::string& path);
    bool loadStaticLibrary(const std::string& path);
    bool loadSourceCode(const std::string& name, const std::string& sourceCode);
    
    void* loadSymbol(const std::string& library, const std::string& symbol) const;
    bool validatePluginSignature(const std::string& path) const;
    bool checkPluginDependencies(const PluginEntry& entry) const;
    bool checkPluginConflicts(const PluginEntry& entry) const;
};

// PluginBuilder for constructing plugins
class PluginBuilder {
public:
    PluginBuilder& setName(const std::string& name);
    PluginBuilder& setVersion(const PluginVersion& version);
    PluginBuilder& setType(PluginType type);
    PluginBuilder& setDescription(const std::string& description);
    PluginBuilder& setAuthor(const std::string& author);
    PluginBuilder& setLicense(const std::string& license);
    PluginBuilder& addDependency(const std::string& dependency);
    PluginBuilder& addConfiguration(const std::string& key, const std::string& value);
    PluginBuilder& addExtension(const std::string& name, void* extension);
    
    std::unique_ptr<IPlugin> build() const;
    
    // Factory methods for common plugin types
    template<typename TPlugin, typename... Args>
    static std::unique_ptr<TPlugin> create(Args&&... args);
    
private:
    PluginMetadata metadata_;
    std::unordered_map<std::string, void*> extensions_;
};

// PluginFactory for creating plugin instances
class PluginFactory {
public:
    static std::unique_ptr<IPlugin> createPlugin(const std::string& name, const std::string& path);
    static std::unique_ptr<IPlugin> createPlugin(const PluginMetadata& metadata);
    static std::unique_ptr<IPlugin> createPlugin(std::unique_ptr<IPlugin> plugin);
    
    template<typename TPlugin, typename... Args>
    static std::unique_ptr<TPlugin> createPlugin(Args&&... args);
    
    // Plugin template loading
    static bool loadPluginTemplate(const std::string& templateName, const std::string& outputPath);
    static std::vector<std::string> getAvailableTemplates() const;
    
private:
    static std::unordered_map<std::string, std::function<std::unique_ptr<IPlugin>()>> pluginFactories_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_PLUGINLOADER_HPP