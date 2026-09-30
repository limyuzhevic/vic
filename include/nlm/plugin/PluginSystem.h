#ifndef NLM_PLUGIN_PLUGINSYSTEM_H
#define NLM_PLUGIN_PLUGINSYSTEM_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include "IPlugin.h"

namespace nlm {
namespace plugin {

class IPluginLoader;
class IPluginRegistry;
class IPluginEventSystem;

class PluginSystem {
public:
    PluginSystem();
    ~PluginSystem();

    // Plugin management
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginId);
    bool enablePlugin(const std::string& pluginId);
    bool disablePlugin(const std::string& pluginId);

    // Plugin discovery and access
    std::vector<std::string> getAvailablePlugins() const;
    std::vector<std::string> getLoadedPlugins() const;
    std::shared_ptr<IPlugin> getPlugin(const std::string& pluginId) const;
    std::shared_ptr<IPlugin> getPluginByName(const std::string& name) const;

    // System management
    bool initialize(const std::map<std::string, std::string>& config);
    bool shutdown();
    bool update(double deltaTime);

    // Configuration
    void setConfig(const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getConfig() const;

    // Event handling
    void onPluginEvent(const std::string& pluginId, const std::string& eventName, const std::map<std::string, std::string>& data);

    // Plugin information
    std::string getSystemVersion() const;
    std::vector<std::string> getSupportedPlatforms() const;

protected:
    std::unique_ptr<IPluginLoader> loader_;
    std::unique_ptr<IPluginRegistry> registry_;
    std::unique_ptr<IPluginEventSystem> eventSystem_;
    std::map<std::string, std::shared_ptr<IPlugin>> loadedPlugins_;
    std::map<std::string, PluginState> pluginStates_;
    std::map<std::string, std::string> pluginConfigs_;
    mutable std::mutex mutex_;
    bool initialized_;
    std::map<std::string, std::string> systemConfig_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_PLUGINSYSTEM_H
