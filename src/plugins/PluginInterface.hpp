// Plugin Interface Base Class
#pragma once

#include <string>
#include <memory>

namespace nlm {

// Plugin interface for NLM extensions
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Get plugin name
    virtual std::string getName() const = 0;
    
    // Get plugin version
    virtual std::string getVersion() const = 0;
    
    // Get plugin description
    virtual std::string getDescription() const = 0;
    
    // Called when plugin is loaded
    virtual void onLoad() {}
    
    // Called when plugin is unloaded
    virtual void onUnload() {}
    
    // Check if plugin can handle a specific configuration key
    virtual bool canHandle(const std::string& key) const { return false; }
    
    // Handle configuration values
    virtual std::string handleConfig(const std::string& key, const std::string& value) { return value; }
};

// Plugin registry for managing plugins
class PluginRegistry {
public:
    static PluginRegistry& getInstance() {
        static PluginRegistry instance;
        return instance;
    }
    
    // Register a plugin
    void registerPlugin(std::shared_ptr<IPlugin> plugin) {
        plugins_.push_back(plugin);
        plugin->onLoad();
    }
    
    // Unregister a plugin
    void unregisterPlugin(const std::string& name) {
        auto it = std::find_if(plugins_.begin(), plugins_.end(),
            [&name](const std::shared_ptr<IPlugin>& plugin) {
                return plugin->getName() == name;
            });
        if (it != plugins_.end()) {
            (*it)->onUnload();
            plugins_.erase(it);
        }
    }
    
    // Get plugin by name
    std::shared_ptr<IPlugin> getPlugin(const std::string& name) {
        auto it = std::find_if(plugins_.begin(), plugins_.end(),
            [&name](const std::shared_ptr<IPlugin>& plugin) {
                return plugin->getName() == name;
            });
        return (it != plugins_.end()) ? *it : nullptr;
    }
    
    // Get all plugins
    const std::vector<std::shared_ptr<IPlugin>>& getPlugins() const {
        return plugins_;
    }
    
    // Process configuration with plugins
    std::string processConfig(const std::string& key, const std::string& value) {
        std::string result = value;
        for (const auto& plugin : plugins_) {
            if (plugin->canHandle(key)) {
                result = plugin->handleConfig(key, result);
            }
        }
        return result;
    }
    
private:
    PluginRegistry() = default;
    std::vector<std::shared_ptr<IPlugin>> plugins_;
};

} // namespace nlm
