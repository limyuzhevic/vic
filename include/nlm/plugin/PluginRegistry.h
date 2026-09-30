#ifndef NLM_PLUGIN_PLUGINREGISTRY_H
#define NLM_PLUGIN_PLUGINREGISTRY_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include "IPlugin.h"

namespace nlm {
namespace plugin {

enum class PluginLoadStatus {
    SUCCESS,
    FAILED,
    VERSION_MISMATCH,
    MISSING_DEPENDENCY,
    SECURITY_VIOLATION,
    ALREADY_LOADED
};

struct PluginInfo {
    std::string id;
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    std::string license;
    std::vector<std::string> dependencies;
    std::vector<PluginCapability> capabilities;
    std::string pluginPath;
    PluginLoadStatus loadStatus;
    PluginState state;
    std::map<std::string, std::string> metadata;
    bool isSystemPlugin;
};

class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();

    // Plugin registration
    bool registerPlugin(const PluginInfo& pluginInfo);
    bool unregisterPlugin(const std::string& pluginId);
    bool updatePluginInfo(const std::string& pluginId, const PluginInfo& pluginInfo);

    // Plugin discovery
    PluginInfo getPluginInfo(const std::string& pluginId) const;
    PluginInfo getPluginInfoByName(const std::string& name) const;
    std::vector<PluginInfo> findPluginsByCapability(PluginCapability capability) const;
    std::vector<PluginInfo> findPluginsByNamePattern(const std::string& pattern) const;
    std::vector<PluginInfo> getAllPlugins() const;
    std::vector<PluginInfo> getLoadedPlugins() const;
    std::vector<PluginInfo> getAvailablePlugins() const;

    // Plugin validation
    bool validatePlugin(const PluginInfo& pluginInfo) const;
    bool checkVersionCompatibility(const std::string& pluginVersion, const std::string& requiredVersion) const;
    bool checkDependencies(const PluginInfo& pluginInfo) const;

    // Plugin state management
    bool setPluginState(const std::string& pluginId, PluginState state);
    PluginState getPluginState(const std::string& pluginId) const;
    bool setPluginLoadStatus(const std::string& pluginId, PluginLoadStatus status);
    PluginLoadStatus getPluginLoadStatus(const std::string& pluginId) const;

    // Plugin filtering
    std::vector<PluginInfo> filterPlugins(std::function<bool(const PluginInfo&)> predicate) const;

    // Configuration management
    void setPluginConfig(const std::string& pluginId, const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getPluginConfig(const std::string& pluginId) const;

protected:
    std::map<std::string, PluginInfo> registeredPlugins_;
    std::map<std::string, PluginState> pluginStates_;
    mutable std::mutex mutex_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_PLUGINREGISTRY_H
