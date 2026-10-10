// Plugin manifest system for metadata and versioning
#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <chrono>

// Forward declarations
namespace nlm {
    enum class PluginCategory;
    struct PluginConfig;
    struct PluginEvent;
    class IEventSubscriber;
    class IPlugin;
}

namespace nlm {

// Plugin manifest for describing plugin metadata and requirements
struct PluginManifest {
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    std::string email;
    std::string website;
    PluginCategory category;
    
    // Version compatibility
    std::string minCoreVersion;
    std::string maxCoreVersion;
    std::set<std::string> compatibleVersions;
    
    // Dependencies
    std::vector<std::string> dependencies;
    std::map<std::string, std::string> optionalDependencies;
    
    // Features
    std::vector<std::string> features;
    std::vector<std::string> tags;
    
    // Plugin metadata
    std::string pluginFilePath;
    std::string configFilePath;
    bool isSharedLibrary;
    std::string buildHash;
    
    // Lifecycle
    std::chrono::system_clock::time_point creationDate;
    std::chrono::system_clock::time_point lastModified;
    std::string license;
    bool isPublic;
    
    // Additional data
    std::map<std::string, std::string> customMetadata;
};

// Plugin loader interface for dynamic library loading
class IPluginLoader {
public:
    virtual ~IPluginLoader() = default;
    
    // Load a plugin from file
    virtual std::shared_ptr<IPlugin> loadPlugin(const std::string& pluginPath) = 0;
    
    // Load plugin from manifest
    virtual std::shared_ptr<IPlugin> loadPlugin(const PluginManifest& manifest) = 0;
    
    // Check if plugin can be loaded
    virtual bool canLoadPlugin(const std::string& pluginPath) const = 0;
    
    // Get available plugins from directory
    virtual std::vector<std::string> getAvailablePlugins(const std::string& directory) const = 0;
    
    // Error handling
    virtual std::string getLastError() const = 0;
};

// Default plugin manifest loader
class PluginManifestLoader {
public:
    static bool loadManifest(const std::string& manifestPath, PluginManifest& manifest);
    static bool saveManifest(const std::string& manifestPath, const PluginManifest& manifest);
    static std::string generateManifestPath(const std::string& pluginPath);
};

} // namespace nlm