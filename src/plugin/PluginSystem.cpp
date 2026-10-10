#include "PluginSystem.hpp"
#include <dlfcn.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace nlm {

// Dynamic library plugin loader
class DynamicLibraryPluginLoader : public IPluginLoader {
public:
    DynamicLibraryPluginLoader() = default;
    
    bool canLoad(const std::string& filepath) const override {
        // Check if file exists
        if (!std::filesystem::exists(filepath)) {
            return false;
        }
        
        // Check file extension
        std::string ext = getFileExtension(filepath);
        auto supportedExtensions = getSupportedExtensions();
        return std::find(supportedExtensions.begin(), supportedExtensions.end(), ext) != supportedExtensions.end();
    }
    
    std::shared_ptr<IPlugin> loadPlugin(const std::string& filepath) override {
        // Open dynamic library
        void* handle = dlopen(filepath.c_str(), RTLD_LAZY);
        if (!handle) {
            std::cerr << "Failed to load plugin " << filepath << ": " << dlerror() << std::endl;
            return nullptr;
        }
        
        try {
            // Get plugin factory function
            typedef std::shared_ptr<IPlugin>(*)() createPluginFunc;
            auto createFunc = (createPluginFunc)dlsym(handle, "createPlugin");
            if (!createFunc) {
                std::cerr << "Plugin " << filepath << " does not have createPlugin function" << std::endl;
                dlclose(handle);
                return nullptr;
            }
            
            // Create plugin instance
            auto plugin = createFunc();
            if (!plugin) {
                std::cerr << "Plugin factory returned null for " << filepath << std::endl;
                dlclose(handle);
                return nullptr;
            }
            
            // Store handle for cleanup
            plugin->setPluginManager(nullptr); // Will be set by manager later
            
            // Plugin loading successful
            return plugin;
            
        } catch (const std::exception& e) {
            std::cerr << "Exception loading plugin " << filepath << ": " << e.what() << std::endl;
            dlclose(handle);
            return nullptr;
        }
    }
    
    std::string getName() const override {
        return "Dynamic Library Plugin Loader";
    }
    
    std::vector<std::string> getSupportedExtensions() const override {
        return {".so", ".dll", ".dylib"};
    }
    
private:
    std::string getFileExtension(const std::string& filepath) const {
        size_t dotPos = filepath.rfind('.');
        if (dotPos == std::string::npos) {
            return "";
        }
        return filepath.substr(dotPos);
    }
};

// Simple file-based plugin loader
class FileBasedPluginLoader : public IPluginLoader {
public:
    FileBasedPluginLoader() = default;
    
    bool canLoad(const std::string& filepath) const override {
        if (!std::filesystem::exists(filepath) || !std::filesystem::is_regular_file(filepath)) {
            return false;
        }
        
        std::string ext = getFileExtension(filepath);
        auto supportedExtensions = getSupportedExtensions();
        return std::find(supportedExtensions.begin(), supportedExtensions.end(), ext) != supportedExtensions.end();
    }
    
    std::shared_ptr<IPlugin> loadPlugin(const std::string& filepath) override {
        // Read plugin file
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open plugin file: " << filepath << std::endl;
            return nullptr;
        }
        
        // For now, just return a null plugin (placeholder for real implementation)
        std::cerr << "File-based plugin loading not yet implemented for " << filepath << std::endl;
        return nullptr;
    }
    
    std::string getName() const override {
        return "File-Based Plugin Loader";
    }
    
    std::vector<std::string> getSupportedExtensions() const override {
        return {".nlmplugin", ".json", ".toml"};
    }
    
private:
    std::string getFileExtension(const std::string& filepath) const {
        size_t dotPos = filepath.rfind('.');
        if (dotPos == std::string::npos) {
            return "";
        }
        return filepath.substr(dotPos);
    }
};

// Default plugin manager
class DefaultPluginManager : public IPluginManager {
public:
    DefaultPluginManager() = default;
    
    bool loadPlugin(const std::string& filepath) override {
        std::string pluginName = extractPluginName(filepath);
        
        if (isPluginLoaded(pluginName)) {
            std::cerr << "Plugin " << pluginName << " is already loaded" << std::endl;
            return false;
        }
        
        // Find appropriate loader
        auto loader = findLoaderForFile(filepath);
        if (!loader) {
            std::cerr << "No suitable plugin loader found for " << filepath << std::endl;
            return false;
        }
        
        // Load plugin
        auto plugin = loader->loadPlugin(filepath);
        if (!plugin) {
            std::cerr << "Failed to load plugin " << filepath << std::endl;
            return false;
        }
        
        // Initialize plugin
        bool initSuccess = plugin->initialize(globalConfig_);
        if (!initSuccess) {
            std::cerr << "Failed to initialize plugin " << pluginName << std::endl;
            return false;
        }
        
        // Store plugin
        plugins_[pluginName] = plugin;
        pluginStats_[pluginName] = PluginStats(
            pluginName, 
            plugin->getCapability().type,
            plugin->getVersion(),
            plugin->getCapability().description
        );
        pluginStats_[pluginName].isInitialized = true;
        pluginStats_[pluginName].loadCount++;
        
        return true;
    }
    
    bool unloadPlugin(const std::string& pluginName) override {
        auto it = plugins_.find(pluginName);
        if (it == plugins_.end()) {
            std::cerr << "Plugin " << pluginName << " is not loaded" << std::endl;
            return false;
        }
        
        // Cleanup plugin
        it->second->cleanup();
        
        // Remove from maps
        plugins_.erase(it);
        pluginStats_.erase(pluginName);
        
        return true;
    }
    
    bool isPluginLoaded(const std::string& pluginName) const override {
        return plugins_.find(pluginName) != plugins_.end();
    }
    
    std::vector<std::string> getLoadedPlugins() const override {
        std::vector<std::string> names;
        names.reserve(plugins_.size());
        for (const auto& pair : plugins_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    std::shared_ptr<IPlugin> getPlugin(const std::string& pluginName) override {
        auto it = plugins_.find(pluginName);
        if (it != plugins_.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    std::vector<std::shared_ptr<IPlugin>> getPluginsByType(PluginType type) override {
        std::vector<std::shared_ptr<IPlugin>> plugins;
        for (const auto& pair : plugins_) {
            if (pair.second->getCapability().type == type) {
                plugins.push_back(pair.second);
            }
        }
        return plugins;
    }
    
    bool configurePlugin(const std::string& pluginName, 
                         const std::map<std::string, std::string>& config) override {
        auto it = plugins_.find(pluginName);
        if (it == plugins_.end()) {
            std::cerr << "Plugin " << pluginName << " is not loaded" << std::endl;
            return false;
        }
        
        // Apply configuration
        for (const auto& pair : config) {
            // Store in plugin's config (implementation-specific)
            pluginConfigs_[pluginName][pair.first] = pair.second;
        }
        
        // Reinitialize plugin with new config
        it->second->cleanup();
        return it->second->initialize(pluginConfigs_[pluginName]);
    }
    
    std::map<std::string, std::string> getGlobalConfig() const override {
        return globalConfig_;
    }
    
    void setGlobalConfig(const std::map<std::string, std::string>& config) override {
        globalConfig_ = config;
        
        // Apply to all loaded plugins
        for (auto& pair : plugins_) {
            pair.second->initialize(globalConfig_);
        }
    }
    
    bool initializeAllPlugins() override {
        bool success = true;
        for (auto& pair : plugins_) {
            if (!pair.second->isInitialized()) {
                if (!pair.second->initialize(globalConfig_)) {
                    std::cerr << "Failed to initialize plugin " << pair.first << std::endl;
                    success = false;
                } else {
                    pluginStats_[pair.first].isInitialized = true;
                }
            }
        }
        return success;
    }
    
    void cleanupAllPlugins() override {
        for (auto& pair : plugins_) {
            if (pair.second->isInitialized()) {
                pair.second->cleanup();
            }
        }
        
        // Clear stats
        for (auto& stat : pluginStats_) {
            stat.second.isInitialized = false;
        }
    }
    
    bool reloadAllPlugins() override {
        bool success = true;
        
        // Cleanup all plugins
        cleanupAllPlugins();
        
        // Unload all plugins
        for (const auto& name : getLoadedPlugins()) {
            if (!unloadPlugin(name)) {
                success = false;
            }
        }
        
        // Reload all plugins (implementation would need plugin configuration)
        // For now, this is a placeholder
        std::cerr << "Plugin reload not yet implemented" << std::endl;
        
        return success;
    }
    
    std::map<std::string, PluginStats> getPluginStats() const {
        return pluginStats_;
    }
    
    bool registerPlugin(const std::string& pluginName, PluginType type, 
                       const std::string& version, const std::string& description,
                       bool required = false) {
        // Create capability
        PluginCapability capability(pluginName, type, version, description, required);
        
        // Store in internal registry (implementation-specific)
        // This would integrate with the actual plugin loading system
        
        return true;
    }
    
private:
    std::string extractPluginName(const std::string& filepath) {
        std::filesystem::path path(filepath);
        return path.stem().string();
    }
    
    std::unique_ptr<IPluginLoader> findLoaderForFile(const std::string& filepath) {
        std::string ext = getFileExtension(filepath);
        
        for (const auto& loader : PluginRegistry::getInstance().getAllLoaders()) {
            auto supportedExtensions = loader->getSupportedExtensions();
            if (std::find(supportedExtensions.begin(), supportedExtensions.end(), ext) != supportedExtensions.end()) {
                return std::move(loader); // Note: This transfers ownership, careful with usage
            }
        }
        
        return nullptr;
    }
    
    std::string getFileExtension(const std::string& filepath) const {
        size_t dotPos = filepath.rfind('.');
        if (dotPos == std::string::npos) {
            return "";
        }
        return filepath.substr(dotPos);
    }
    
    std::map<std::string, std::shared_ptr<IPlugin>> plugins_;
    std::map<std::string, PluginStats> pluginStats_;
    std::map<std::string, std::map<std::string, std::string>> pluginConfigs_;
    std::map<std::string, std::string> globalConfig_;
};

// Plugin registry implementation
void PluginRegistry::registerLoader(std::unique_ptr<IPluginLoader> loader) {
    loaders_.push_back(std::move(loader));
}

std::unique_ptr<IPluginLoader> PluginRegistry::getLoaderByExtension(const std::string& extension) {
    for (const auto& loader : loaders_) {
        auto supportedExtensions = loader->getSupportedExtensions();
        if (std::find(supportedExtensions.begin(), supportedExtensions.end(), extension) != supportedExtensions.end()) {
            return std::move(loader); // Transfers ownership
        }
    }
    return nullptr;
}

std::vector<std::unique_ptr<IPluginLoader>> PluginRegistry::getAllLoaders() const {
    return loaders_;
}

std::vector<std::string> PluginRegistry::getAllPluginTypes() const {
    std::vector<std::string> types;
    for (const auto& pair : pluginFactories_) {
        types.push_back(pair.first);
    }
    return types;
}

} // namespace nlm