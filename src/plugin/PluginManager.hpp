// Plugin loader implementation for dynamic library loading
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <dlfcn.h>
#include "PluginBase.hpp"
#include "PluginManifest.hpp"

namespace nlm {

#ifdef _WIN32
    using PluginHandle = void*;
#else
    using PluginHandle = void*;
#endif

// Function signatures for plugin entry points
using PluginCreateFunc = IPlugin*(*)(void);
using PluginDestroyFunc = void(*)(IPlugin*);

class DynamicPluginLoader : public IPluginLoader {
private:
    std::map<std::string, PluginHandle> loadedHandles;
    mutable std::mutex mutex;
    std::string lastError;
    
    // Platform-specific loading functions
    bool loadLibrary(const std::string& path, PluginHandle& handle);
    void unloadLibrary(const std::string& path, PluginHandle& handle);
    void* getSymbol(PluginHandle handle, const std::string& symbol);
    
public:
    DynamicPluginLoader() = default;
    ~DynamicPluginLoader() override;
    
    std::shared_ptr<IPlugin> loadPlugin(const std::string& pluginPath) override;
    std::shared_ptr<IPlugin> loadPlugin(const PluginManifest& manifest) override;
    
    bool canLoadPlugin(const std::string& pluginPath) const override;
    std::vector<std::string> getAvailablePlugins(const std::string& directory) const override;
    
    std::string getLastError() const override { return lastError; }
    
    // Plugin management
    void unloadPlugin(const std::string& pluginPath);
    bool isPluginLoaded(const std::string& pluginPath) const;
    std::vector<std::string> getLoadedPlugins() const;
};

// Plugin registry for managing loaded plugins
class PluginRegistry {
private:
    std::map<std::string, std::shared_ptr<IPlugin>> plugins;
    std::map<std::string, PluginManifest> pluginManifests;
    std::shared_ptr<IPluginLoader> loader;
    mutable std::mutex mutex;
    
public:
    PluginRegistry(std::shared_ptr<IPluginLoader> loader = nullptr);
    
    // Plugin management
    bool registerPlugin(const std::string& name, std::shared_ptr<IPlugin> plugin);
    std::shared_ptr<IPlugin> getPlugin(const std::string& name) const;
    bool unregisterPlugin(const std::string& name);
    bool isPluginRegistered(const std::string& name) const;
    
    // Manifest management
    bool registerManifest(const PluginManifest& manifest);
    PluginManifest getManifest(const std::string& name) const;
    std::vector<PluginManifest> getAllManifests() const;
    
    // Plugin discovery and loading
    std::vector<std::string> discoverPlugins(const std::string& directory);
    bool loadPluginFromFile(const std::string& pluginPath);
    bool loadPluginFromManifest(const PluginManifest& manifest);
    
    // Plugin information
    std::vector<std::string> getLoadedPluginNames() const;
    PluginCategory getPluginCategory(const std::string& name) const;
    std::vector<std::string> getPluginDependencies(const std::string& name) const;
    
    // Error handling
    std::string getLastError() const;
};