#ifndef NLM_PLUGIN_PLUGINFACTORY_H
#define NLM_PLUGIN_PLUGINFACTORY_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include "IPlugin.h"

namespace nlm {
namespace plugin {

enum class PluginCreationMode {
    DEFAULT,
    WITH_CONFIG,
    WITH_DEPENDENCIES,
    CUSTOM
};

struct PluginCreationParameters {
    std::string pluginId;
    std::string pluginName;
    std::map<std::string, std::string> config;
    std::vector<std::string> enabledFeatures;
    PluginCreationMode mode;
    std::map<std::string, std::string> customParameters;
};

struct PluginCreationResult {
    std::shared_ptr<IPlugin> plugin;
    bool success;
    std::string errorMessage;
    PluginLoadStatus loadStatus;
    std::map<std::string, std::string> metadata;
};

class PluginFactory {
public:
    PluginFactory();
    ~PluginFactory();

    // Plugin creation
    PluginCreationResult createPlugin(const PluginCreationParameters& params);
    PluginCreationResult createPlugin(const std::string& pluginId);
    PluginCreationResult createPlugin(const std::string& pluginId, const std::map<std::string, std::string>& config);

    // Plugin cloning
    PluginCreationResult clonePlugin(const std::string& pluginId, const std::string& newPluginId);
    bool duplicatePlugin(const std::string& pluginId, const std::string& newPluginId);

    // Plugin templates
    bool registerPluginTemplate(const std::string& name, const PluginCreationParameters& templateParams);
    PluginCreationResult createFromTemplate(const std::string& templateName, const std::map<std::string, std::string>& overrides);

    // Plugin configuration
    bool configurePlugin(std::shared_ptr<IPlugin> plugin, const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getPluginConfig(std::shared_ptr<IPlugin> plugin) const;

    // Plugin validation
    bool validatePluginCreationParameters(const PluginCreationParameters& params) const;
    std::vector<std::string> getPluginCreationErrors(const PluginCreationParameters& params) const;

    // Plugin statistics
    size_t getPluginCount() const;
    std::map<std::string, int> getPluginStatistics() const;

    // System management
    void clearPluginCache();
    void setMaxPluginCacheSize(size_t maxSize);

protected:
    std::map<std::string, PluginCreationParameters> pluginTemplates_;
    std::map<std::string, std::shared_ptr<IPlugin>> pluginCache_;
    std::map<std::string, PluginCreationResult> creationResults_;
    mutable std::mutex mutex_;
    size_t maxPluginCacheSize_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_PLUGINFACTORY_H
