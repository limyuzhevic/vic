// NLM (熙然) - Neural Learning Machine
// Plugin system for extending NLM with third-party components
// Provides plugin discovery, loading, and management capabilities

#include "Plugin.hpp"
#include <dlfcn.h>  // For dynamic loading
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <memory>
#include <thread>
#include <chrono>

namespace nlm {

// Global plugin manager instance
class PluginManager {
public:
    static PluginManager& getInstance() {
        static PluginManager instance;
        return instance;
    }
    
    // Plugin discovery and loading
    bool loadPlugin(const std::string& pluginPath) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!std::filesystem::exists(pluginPath)) {
            std::cerr << "Plugin error: File does not exist: " << pluginPath << std::endl;
            return false;
        }
        
        std::string pluginName = extractPluginName(pluginPath);
        
        // Check if plugin already loaded
        if (isPluginLoaded(pluginName)) {
            std::cout << "Plugin info: Plugin already loaded: " << pluginName << std::endl;
            return true;
        }
        
        try {
            // Load plugin library
            void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY | RTLD_LOCAL);
            if (!handle) {
                std::cerr << "Plugin error: Failed to load library " << pluginPath 
                         << ": " << dlerror() << std::endl;
                return false;
            }
            
            // Get plugin entry point
            using CreatePluginFunc = Plugin*(*)();
            auto createPlugin = reinterpret_cast<CreatePluginFunc>(dlsym(handle, "createPlugin"));
            if (!createPlugin) {
                std::cerr << "Plugin error: No createPlugin function found in " << pluginPath << std::endl;
                dlclose(handle);
                return false;
            }
            
            // Create plugin instance
            Plugin* plugin = createPlugin();
            if (!plugin) {
                std::cerr << "Plugin error: Failed to create plugin instance from " << pluginPath << std::endl;
                dlclose(handle);
                return false;
            }
            
            // Verify plugin interface
            if (!plugin->isValid()) {
                std::cerr << "Plugin error: Invalid plugin interface in " << pluginPath << std::endl;
                delete plugin;
                dlclose(handle);
                return false;
            }
            
            // Get plugin metadata
            PluginMetadata metadata = plugin->getMetadata();
            
            // Check version compatibility
            if (!checkVersionCompatibility(metadata.requiredNLMVersion)) {
                std::cerr << "Plugin error: Version incompatibility for plugin " << metadata.name << std::endl;
                delete plugin;
                dlclose(handle);
                return false;
            }
            
            // Register plugin
            loadedPlugins_[pluginName] = std::make_unique<LoadedPluginInfo>();
            LoadedPluginInfo& info = *loadedPlugins_[pluginName];
            info.plugin = std::unique_ptr<Plugin>(plugin);
            info.handle = handle;
            info.path = pluginPath;
            info.metadata = metadata;
            info.loadTime = std::chrono::system_clock::now();
            info.state = PluginState::Loaded;
            
            // Initialize plugin
            if (!info.plugin->initialize()) {
                std::cerr << "Plugin error: Failed to initialize plugin " << metadata.name << std::endl;
                info.state = PluginState::Failed;
                // Don't delete here, let cleanup handle it
            } else {
                info.state = PluginState::Initialized;
                std::cout << "Plugin success: Loaded and initialized plugin " << metadata.name 
                         << " (version " << metadata.version << ")" << std::endl;
            }
            
            // Call plugin-specific initialization
            callPluginSetup(metadata.name, handle);
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Plugin error: Exception loading plugin " << pluginPath 
                     << ": " << e.what() << std::endl;
            return false;
        }
    }
    
    // Unload a plugin
    bool unloadPlugin(const std::string& pluginName) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = loadedPlugins_.find(pluginName);
        if (it == loadedPlugins_.end()) {
            std::cerr << "Plugin error: Plugin not found: " << pluginName << std::endl;
            return false;
        }
        
        LoadedPluginInfo& info = *it->second;
        
        try {
            // Call plugin cleanup
            callPluginCleanup(info.metadata.name, info.handle);
            
            // Shutdown plugin
            info.plugin->shutdown();
            
            // Close library
            if (info.handle) {
                dlclose(info.handle);
                info.handle = nullptr;
            }
            
            // Remove plugin from loaded list
            loadedPlugins_.erase(it);
            
            std::cout << "Plugin success: Unloaded plugin " << info.metadata.name << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Plugin error: Exception unloading plugin " << pluginName 
                     << ": " << e.what() << std::endl;
            return false;
        }
    }
    
    // Get all loaded plugins
    std::vector<std::string> getLoadedPlugins() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> pluginNames;
        pluginNames.reserve(loadedPlugins_.size());
        
        for (const auto& pair : loadedPlugins_) {
            pluginNames.push_back(pair.first);
        }
        
        return pluginNames;
    }
    
    // Get plugin by name
    std::shared_ptr<Plugin> getPlugin(const std::string& pluginName) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = loadedPlugins_.find(pluginName);
        if (it == loadedPlugins_.end()) {
            return nullptr;
        }
        
        return std::shared_ptr<Plugin>(it->second->plugin.get(), 
                                      [this, pluginName](Plugin*) {
                                          // Custom deleter that removes plugin from manager
                                          unloadPlugin(pluginName);
                                      });
    }
    
    // Get plugin metadata
    PluginMetadata getPluginMetadata(const std::string& pluginName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = loadedPlugins_.find(pluginName);
        if (it == loadedPlugins_.end()) {
            throw std::runtime_error("Plugin not found: " + pluginName);
        }
        
        return it->second->metadata;
    }
    
    // Check if plugin is loaded
    bool isPluginLoaded(const std::string& pluginName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return loadedPlugins_.find(pluginName) != loadedPlugins_.end();
    }
    
    // Get plugin state
    PluginState getPluginState(const std::string& pluginName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = loadedPlugins_.find(pluginName);
        if (it == loadedPlugins_.end()) {
            return PluginState::NotLoaded;
        }
        
        return it->second->state;
    }
    
    // Discover plugins in a directory
    std::vector<std::string> discoverPlugins(const std::string& directory) {
        std::vector<std::string> discoveredPlugins;
        
        if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
            std::cerr << "Plugin error: Directory does not exist or is not a directory: " << directory << std::endl;
            return discoveredPlugins;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                
                // Check if file is a dynamic library
                if (isDynamicLibrary(filename)) {
                    std::string pluginPath = entry.path().string();
                    discoveredPlugins.push_back(pluginPath);
                }
            }
        }
        
        return discoveredPlugins;
    }
    
    // Reload all plugins
    bool reloadAllPlugins() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<std::string> pluginNames;
        for (const auto& pair : loadedPlugins_) {
            pluginNames.push_back(pair.first);
        }
        
        bool success = true;
        for (const auto& name : pluginNames) {
            if (!unloadPlugin(name)) {
                std::cerr << "Plugin warning: Failed to unload plugin " << name << " before reload" << std::endl;
                success = false;
            }
        }
        
        if (!success) {
            std::cerr << "Plugin error: Some plugins failed to unload, reload aborted" << std::endl;
            return false;
        }
        
        // Reload plugins from their original locations
        for (const auto& pair : loadedPlugins_) {
            bool loaded = loadPlugin(pair.second->path);
            if (!loaded) {
                std::cerr << "Plugin error: Failed to reload plugin " << pair.first << std::endl;
                success = false;
            }
        }
        
        return success;
    }
    
    // Get plugin manager statistics
    PluginManagerStats getStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        PluginManagerStats stats;
        stats.totalPlugins = loadedPlugins_.size();
        stats.loadedPlugins = 0;
        stats.failedPlugins = 0;
        
        for (const auto& pair : loadedPlugins_) {
            switch (pair.second->state) {
                case PluginState::Loaded:
                    stats.loadedPlugins++;
                    break;
                case PluginState::Failed:
                    stats.failedPlugins++;
                    break;
                default:
                    stats.loadedPlugins++;  // Other states count as loaded
            }
        }
        
        return stats;
    }
    
    // Save plugin configuration
    bool savePluginConfig(const std::string& pluginName, const std::string& configFile) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = loadedPlugins_.find(pluginName);
        if (it == loadedPlugins_.end()) {
            return false;
        }
        
        try {
            std::ofstream file(configFile, std::ios::trunc);
            if (!file.is_open()) {
                return false;
            }
            
            file << "plugin_name = " << pluginName << "\n";
            file << "plugin_state = " << static_cast<int>(it->second->state) << "\n";
            file << "load_time = " << std::chrono::system_clock::to_time_t(it->second->loadTime) << "\n";
            
            // Save plugin-specific configuration
            if (it->second->plugin) {
                bool configSuccess = it->second->plugin->saveConfig(configFile + ".plugin");
                if (!configSuccess) {
                    std::cerr << "Plugin error: Failed to save configuration for plugin " << pluginName << std::endl;
                }
            }
            
            file.close();
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Plugin error: Failed to save configuration for plugin " << pluginName 
                     << ": " << e.what() << std::endl;
            return false;
        }
    }
    
    // Load plugin configuration
    bool loadPluginConfig(const std::string& pluginName, const std::string& configFile) {
        if (!std::filesystem::exists(configFile)) {
            return false;
        }
        
        auto it = loadedPlugins_.find(pluginName);
        if (it == loadedPlugins_.end()) {
            return false;
        }
        
        try {
            std::ifstream file(configFile);
            if (!file.is_open()) {
                return false;
            }
            
            std::string line;
            while (std::getline(file, line)) {
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    
                    // Trim whitespace
                    key.erase(0, key.find_first_not_of(" \t"));
                    key.erase(key.find_last_not_of(" \t") + 1);
                    value.erase(0, value.find_first_not_of(" \t"));
                    value.erase(value.find_last_not_of(" \t") + 1);
                    
                    if (key == "plugin_name") {
                        // Ignore, already known
                    } else if (key == "plugin_state") {
                        int state = std::stoi(value);
                        it->second->state = static_cast<PluginState>(state);
                    } else if (key == "load_time") {
                        // Could parse load time if needed
                    }
                }
            }
            
            file.close();
            
            // Load plugin-specific configuration
            if (it->second->plugin) {
                bool configSuccess = it->second->plugin->loadConfig(configFile + ".plugin");
                if (!configSuccess) {
                    std::cerr << "Plugin error: Failed to load configuration for plugin " << pluginName << std::endl;
                }
            }
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Plugin error: Failed to load configuration for plugin " << pluginName 
                     << ": " << e.what() << std::endl;
            return false;
        }
    }
    
private:
    PluginManager() = default;
    ~PluginManager() {
        // Clean up all plugins
        for (auto& pair : loadedPlugins_) {
            unloadPlugin(pair.first);
        }
    }
    
    // Disable copy and assignment
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    
    std::string extractPluginName(const std::string& pluginPath) {
        size_t pos = pluginPath.find_last_of('/');
        if (pos != std::string::npos) {
            return pluginPath.substr(pos + 1);
        }
        
        pos = pluginPath.find_last_of('\\');
        if (pos != std::string::npos) {
            return pluginPath.substr(pos + 1);
        }
        
        // Remove file extension
        size_t dotPos = pluginPath.find_last_of('.');
        if (dotPos != std::string::npos) {
            return pluginPath.substr(0, dotPos);
        }
        
        return pluginPath;
    }
    
    bool isDynamicLibrary(const std::string& filename) {
        // Check for common dynamic library extensions
        std::string ext = filename.substr(filename.find_last_of('.') + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        return (ext == "so") || (ext == "dll") || (ext == "dylib") || 
               (ext == "plugin") || (ext == "plug");
    }
    
    bool checkVersionCompatibility(const std::string& requiredVersion) {
        // Simple version checking - compare major version numbers
        // In a real implementation, this would be more sophisticated
        std::string currentVersion = "6.0"; // Current NLM version
        
        size_t reqMajorPos = requiredVersion.find('.');
        size_t curMajorPos = currentVersion.find('.');
        
        if (reqMajorPos != std::string::npos && curMajorPos != std::string::npos) {
            std::string reqMajor = requiredVersion.substr(0, reqMajorPos);
            std::string curMajor = currentVersion.substr(0, curMajorPos);
            
            // Check if major version is compatible (equal or less than)
            int reqMaj = std::stoi(reqMajor);
            int curMaj = std::stoi(curMajor);
            
            return curMaj >= reqMaj;
        }
        
        return true; // If can't parse, assume compatible
    }
    
    void callPluginSetup(const std::string& pluginName, void* handle) {
        // Call plugin setup function if available
        using SetupFunc = void(*)(void*);
        auto setupFunc = reinterpret_cast<SetupFunc>(dlsym(handle, "pluginSetup"));
        if (setupFunc) {
            setupFunc(nullptr);
        }
    }
    
    void callPluginCleanup(const std::string& pluginName, void* handle) {
        // Call plugin cleanup function if available
        using CleanupFunc = void(*)(void*);
        auto cleanupFunc = reinterpret_cast<CleanupFunc>(dlsym(handle, "pluginCleanup"));
        if (cleanupFunc) {
            cleanupFunc(nullptr);
        }
    }
    
    struct LoadedPluginInfo {
        std::unique_ptr<Plugin> plugin;
        void* handle = nullptr;
        std::string path;
        PluginMetadata metadata;
        PluginState state = PluginState::NotLoaded;
        std::chrono::system_clock::time_point loadTime;
    };
    
    std::mutex mutex_;
    std::unordered_map<std::string, std::unique_ptr<LoadedPluginInfo>> loadedPlugins_;
};

// Global plugin manager instance
inline PluginManager& getPluginManager() {
    return PluginManager::getInstance();
}

// Convenience functions for plugin management
inline bool loadPlugin(const std::string& pluginPath) {
    return getPluginManager().loadPlugin(pluginPath);
}

inline bool unloadPlugin(const std::string& pluginName) {
    return getPluginManager().unloadPlugin(pluginName);
}

inline std::vector<std::string> getLoadedPlugins() {
    return getPluginManager().getLoadedPlugins();
}

inline std::shared_ptr<Plugin> getPlugin(const std::string& pluginName) {
    return getPluginManager().getPlugin(pluginName);
}

inline bool discoverAndLoadPlugins(const std::string& directory) {
    std::vector<std::string> plugins = getPluginManager().discoverPlugins(directory);
    bool success = true;
    
    for (const auto& pluginPath : plugins) {
        if (!loadPlugin(pluginPath)) {
            std::cerr << "Plugin warning: Failed to load plugin " << pluginPath << std::endl;
            success = false;
        }
    }
    
    return success;
}

// Plugin extension point helper functions
namespace PluginExtensions {
    // Register a new extension point
    bool registerExtensionPoint(const std::string& name, 
                               const std::type_info& interface,
                               const PluginExtension& extension);
    
    // Get extension point
    std::shared_ptr<void> getExtensionPoint(const std::string& name);
    
    // Get all extensions for an interface
    template<typename T>
    std::vector<std::shared_ptr<T>> getExtensions() {
        std::vector<std::shared_ptr<T>> extensions;
        // Implementation would iterate through loaded plugins and collect extensions
        return extensions;
    }
}

} // namespace nlm

// Plugin entry point macro
#define NLM_PLUGIN_EXPORT __attribute__((visibility("default")))
#define NLM_PLUGIN_ENTRY_POINT extern "C" NLM_PLUGIN_EXPORT

#define NLM_DEFINE_PLUGIN(ClassName, Name, Version, RequiredVersion) \
    NLM_PLUGIN_ENTRY_POINT Plugin* createPlugin() { \
        return new ClassName(); \
    } \
    NLM_PLUGIN_ENTRY_POINT const char* getPluginName() { \
        return Name; \
    } \
    NLM_PLUGIN_ENTRY_POINT const char* getPluginVersion() { \
        return Version; \
    } \
    NLM_PLUGIN_ENTRY_POINT const char* getRequiredNLMVersion() { \
        return RequiredVersion; \
    }

// Plugin base class implementation macros
#define NLM_PLUGIN_CLASS_BEGIN(ClassName) \
class ClassName : public Plugin { \
public: \
    ClassName() : Plugin() {}

#define NLM_PLUGIN_CLASS_END };

#define NLM_PLUGIN_EXTENSION_POINT(Name, Interface) \
static bool register_##Name##_extension() { \
    return PluginExtensions::registerExtensionPoint(#Name, typeid(Interface), nullptr); \
}

// Auto-registration helper
#define NLM_PLUGIN_AUTO_REGISTER(ClassName) \
namespace { \
    bool auto_register_##ClassName = []() { \
        // Auto-register the plugin \
        return true; \
    }(); \
}

// Plugin interface macros for easier development
#define NLM_PLUGIN_IMPLEMENTS(Type) \
    virtual bool implements() const override { \
        return typeid(Type) == typeid(*this); \
    }

#define NLM_PLUGIN_EXTENSION(Type) \
    virtual std::shared_ptr<Type> getExtension() override { \
        // Implementation would return extension if available \
        return nullptr; \
    }