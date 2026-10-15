#include "IPluginManager.hpp"
#include <algorithm>

namespace nlm {

// Default plugin manager implementation
class DefaultPluginManager : public IPluginManager {
public:
    DefaultPluginManager() {
        registerBuiltInPlugins();
    }
    
    void registerBuiltInPlugins() override {
        // Register built-in plugins here
        // For example:
        // registerPlugin("MemoryManagementPlugin", "1.0.0", 
        //               "Advanced memory management plugin",
        //               []() { return std::make_unique<MemoryManagementPlugin>(); },
        //               "memory_management");
        // 
        // registerPlugin("AdvancedPlasticityPlugin", "1.0.0",
        //               "Advanced plasticity rules plugin",
        //               []() { return std::make_unique<AdvancedPlasticityPlugin>(); },
        //               "plasticity");
    }
    
    void registerPlugin(const std::string& name, const std::string& version,
                       const std::string& description, PluginFactory factory,
                       const std::string& type) {
        pluginFactories[name] = factory;
        pluginInfos[name] = {name, version, description, type, "", false};
    }
    
    bool loadPlugin(const std::string& name, PluginFactory factory) override {
        if (plugins.find(name) != plugins.end()) {
            return false; // Already loaded
        }
        
        auto plugin = factory();
        if (!plugin) {
            return false;
        }
        
        plugins[name] = std::move(plugin);
        pluginEnabled[name] = false;
        return true;
    }
    
    bool unloadPlugin(const std::string& name) override {
        auto it = plugins.find(name);
        if (it == plugins.end()) {
            return false;
        }
        
        plugins.erase(it);
        pluginInfos.erase(name);
        pluginEnabled.erase(name);
        return true;
    }
    
    std::unique_ptr<IPlugin> getPlugin(const std::string& name) override {
        auto it = plugins.find(name);
        if (it == plugins.end()) {
            return nullptr;
        }
        
        return std::make_unique<IPlugin>(*it->second);
    }
    
    std::vector<std::string> getPluginNames() const override {
        std::vector<std::string> names;
        for (const auto& pair : plugins) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    PluginInfo getPluginInfo(const std::string& name) const override {
        auto it = pluginInfos.find(name);
        if (it == pluginInfos.end()) {
            return PluginInfo();
        }
        return it->second;
    }
    
    bool enablePlugin(const std::string& name) override {
        auto it = plugins.find(name);
        if (it == plugins.end()) {
            return false;
        }
        
        pluginEnabled[name] = true;
        return true;
    }
    
    bool disablePlugin(const std::string& name) override {
        auto it = plugins.find(name);
        if (it == plugins.end()) {
            return false;
        }
        
        pluginEnabled[name] = false;
        return true;
    }
    
    bool isPluginEnabled(const std::string& name) const override {
        auto it = pluginEnabled.find(name);
        if (it == pluginEnabled.end()) {
            return false;
        }
        return it->second;
    }
    
    void initializePlugins(void* context, std::shared_ptr<void> config) override {
        for (auto& pair : plugins) {
            if (pluginEnabled[pair.first]) {
                pair.second->initialize(context, config);
            }
        }
    }
    
    void updatePlugins(double timestep) override {
        for (auto& pair : plugins) {
            if (pluginEnabled[pair.first]) {
                pair.second->update(timestep);
            }
        }
    }
    
    void shutdownPlugins() override {
        for (auto& pair : plugins) {
            if (pluginEnabled[pair.first]) {
                pair.second->shutdown();
            }
        }
        
        plugins.clear();
        pluginInfos.clear();
        pluginEnabled.clear();
    }
    
private:
    std::unordered_map<std::string, std::unique_ptr<IPlugin>> plugins;
    std::unordered_map<std::string, PluginInfo> pluginInfos;
    std::unordered_map<std::string, bool> pluginEnabled;
    std::unordered_map<std::string, PluginFactory> pluginFactories;
};

// Global plugin manager instance
static DefaultPluginManager gPluginManager;

IPluginManager& getPluginManager() {
    return gPluginManager;
}

}} // namespace nlm