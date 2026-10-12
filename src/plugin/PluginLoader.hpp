#pragma once

#include <string>
#include <memory>
#include <map>
#include <functional>
#include "Plugin.hpp"

namespace nlm {

class PluginLoader {
public:
    // Plugin loading result
    struct LoadResult {
        std::shared_ptr<Plugin> plugin;
        bool success;
        std::string errorMessage;
        PluginVersion loadedVersion;
        
        LoadResult() : success(false), loadedVersion(0, 0, 0) {}
        LoadResult(std::shared_ptr<Plugin> p, bool s, const std::string& err,
                   const PluginVersion& ver = PluginVersion())
            : plugin(p), success(s), errorMessage(err), loadedVersion(ver) {}
    };
    
    using PluginFactory = std::function<std::shared_ptr<Plugin>()>;
    
    PluginLoader();
    ~PluginLoader();
    
    // Load plugin from file path
    LoadResult loadPlugin(const std::string& filepath, PluginType expectedType = PluginType::General);
    
    // Register plugin factory (for static plugins)
    void registerPluginFactory(const std::string& name, PluginFactory factory, PluginType type);
    
    // Load plugin using factory
    LoadResult createPlugin(const std::string& name);
    
    // Unload plugin from memory
    bool unloadPlugin(const std::string& instanceId);
    
    // Check if plugin file exists and is valid
    bool isValidPluginFile(const std::string& filepath) const;
    
    // Get supported plugin file extensions
    static std::vector<std::string> getSupportedExtensions();
    
    // Platform-specific library operations
    void* loadNativeLibrary(const std::string& filepath);
    void unloadNativeLibrary(void* handle);
    void* getNativeSymbol(void* handle, const std::string& symbol);
    
    // Get factory function from library
    PluginFactory getFactoryFromLibrary(void* handle, const std::string& factorySymbol = "create_plugin");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm