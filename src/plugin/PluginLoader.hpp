#include "PluginBase.hpp"
#include <dlfcn.h>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace nlm {

class PluginLoader {
public:
    static std::unique_ptr<PluginBase> loadPlugin(const std::string& libraryPath) {
        void* handle = dlopen(libraryPath.c_str(), RTLD_LAZY);
        if (!handle) {
            throw std::runtime_error("Failed to load plugin: " + std::string(dlerror()));
        }
        
        // Get factory function
        void* factoryFunc = dlsym(handle, "createPlugin");
        if (!factoryFunc) {
            dlclose(handle);
            throw std::runtime_error("Plugin does not export createPlugin function");
        }
        
        // Cast factory function
        auto factory = reinterpret_cast<PluginFactory*>(factoryFunc);
        auto plugin = (*factory)();
        
        // Set library handle for cleanup
        plugin->setLibraryHandle(handle);
        return plugin;
    }
    
    static void unloadPlugin(std::unique_ptr<PluginBase>& plugin) {
        if (plugin) {
            void* handle = plugin->getLibraryHandle();
            if (handle) {
                dlclose(handle);
            }
            plugin.reset();
        }
    }
    
private:
    PluginLoader() = default;
};

} // namespace nlm