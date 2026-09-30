#include "include/nlm/plugin/PluginFactory.h"
#include "include/nlm/plugin/IPlugin.h"
#include <iostream>
#include <algorithm>

namespace nlm {
namespace plugin {

PluginFactory::PluginFactory() : maxPluginCacheSize_(10) {
}

PluginFactory::~PluginFactory() {
}

PluginCreationResult PluginFactory::createPlugin(const PluginCreationParameters& params) {
    PluginCreationResult result;

    if (!validatePluginCreationParameters(params)) {
        result.success = false;
        result.errorMessage = "Invalid plugin creation parameters";
        return result;
    }

    std::string cacheKey = params.pluginId + ":" + std::to_string(std::hash<std::string>{}(params.pluginName));

    auto it = pluginCache_.find(cacheKey);
    if (it != pluginCache_.end()) {
        result.plugin = it->second;
        result.success = true;
        return result;
    }

    try {
        std::shared_ptr<IPlugin> plugin = std::make_shared<IPlugin>();
        result.plugin = plugin;

        if (configurePlugin(plugin, params.config)) {
            result.success = true;
            result.loadStatus = PluginLoadStatus::SUCCESS;

            pluginCache_[cacheKey] = plugin;
            if (pluginCache_.size() > maxPluginCacheSize_) {
                pluginCache_.erase(pluginCache_.begin());
            }
        } else {
            result.success = false;
            result.errorMessage = "Failed to configure plugin";
        }
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = std::string("Exception during plugin creation: ") + e.what();
    }

    return result;
}

PluginCreationResult PluginFactory::createPlugin(const std::string& pluginId) {
    PluginCreationParameters params;
    params.pluginId = pluginId;
    params.pluginName = "Plugin " + pluginId;
    params.mode = PluginCreationMode::DEFAULT;

    return createPlugin(params);
}

PluginCreationResult PluginFactory::createPlugin(const std::string& pluginId, const std::map<std::string, std::string>& config) {
    PluginCreationParameters params;
    params.pluginId = pluginId;
    params.pluginName = "Plugin " + pluginId;
    params.config = config;
    params.mode = PluginCreationMode::WITH_CONFIG;

    return createPlugin(params);
}

PluginCreationResult PluginFactory::clonePlugin(const std::string& pluginId, const std::string& newPluginId) {
    PluginCreationResult result;

    auto it = pluginCache_.begin();
    for (; it != pluginCache_.end(); ++it) {
        if (it->second && it->second->getID() == pluginId) {
            break;
        }
    }

    if (it == pluginCache_.end()) {
        result.success = false;
        result.errorMessage = "Source plugin not found in cache";
        return result;
    }

    PluginCreationParameters params;
    params.pluginId = newPluginId;
    params.pluginName = it->second->getName() + " (Clone)";
    params.config = it->second->getConfig();
    params.mode = PluginCreationMode::CUSTOM;

    return createPlugin(params);
}

bool PluginFactory::duplicatePlugin(const std::string& pluginId, const std::string& newPluginId) {
    PluginCreationResult result = clonePlugin(pluginId, newPluginId);
    if (result.success && result.plugin) {
        pluginCache_[newPluginId + ":duplicate"] = result.plugin;
        return true;
    }

    return false;
}

bool PluginFactory::registerPluginTemplate(const std::string& name, const PluginCreationParameters& templateParams) {
    if (name.empty()) {
        return false;
    }

    pluginTemplates_[name] = templateParams;
    return true;
}

PluginCreationResult PluginFactory::createFromTemplate(const std::string& templateName, const std::map<std::string, std::string>& overrides) {
    auto it = pluginTemplates_.find(templateName);
    if (it == pluginTemplates_.end()) {
        PluginCreationResult result;
        result.success = false;
        result.errorMessage = "Template not found: " + templateName;
        return result;
    }

    PluginCreationParameters params = it->second;

    for (const auto& override : overrides) {
        params.customParameters[override.first] = override.second;
    }

    return createPlugin(params);
}

bool PluginFactory::configurePlugin(std::shared_ptr<IPlugin> plugin, const std::map<std::string, std::string>& config) {
    if (!plugin) {
        return false;
    }

    return plugin->initialize(config);
}

std::map<std::string, std::string> PluginFactory::getPluginConfig(std::shared_ptr<IPlugin> plugin) const {
    if (!plugin) {
        return {};
    }

    std::map<std::string, std::string> config;
    config["name"] = plugin->getName();
    config["version"] = plugin->getVersion();
    config["state"] = std::to_string(static_cast<int>(plugin->getState()));

    return config;
}

bool PluginFactory::validatePluginCreationParameters(const PluginCreationParameters& params) const {
    if (params.pluginId.empty() || params.pluginName.empty()) {
        return false;
    }

    if (params.mode == PluginCreationMode::WITH_CONFIG && params.config.empty()) {
        return false;
    }

    if (params.mode == PluginCreationMode::WITH_DEPENDENCIES && params.customParameters.empty()) {
        return false;
    }

    return true;
}

std::vector<std::string> PluginFactory::getPluginCreationErrors(const PluginCreationParameters& params) const {
    std::vector<std::string> errors;

    if (params.pluginId.empty()) {
        errors.push_back("Plugin ID cannot be empty");
    }

    if (params.pluginName.empty()) {
        errors.push_back("Plugin name cannot be empty");
    }

    if (params.mode == PluginCreationMode::WITH_CONFIG && params.config.empty()) {
        errors.push_back("Configuration is required when creating plugin with config");
    }

    if (params.mode == PluginCreationMode::WITH_DEPENDENCIES && params.customParameters.empty()) {
        errors.push_back("Dependencies are required when creating plugin with dependencies");
    }

    return errors;
}

std::vector<std::string> PluginFactory::getPluginNames() const {
    std::vector<std::string> names;

    for (const auto& pair : pluginCache_) {
        if (pair.second) {
            names.push_back(pair.second->getName());
        }
    }

    return names;
}

size_t PluginFactory::getPluginCount() const {
    return pluginCache_.size();
}

std::map<std::string, int> PluginFactory::getPluginStatistics() const {
    std::map<std::string, int> stats;

    for (const auto& pair : pluginCache_) {
        if (pair.second) {
            std::string name = pair.second->getName();
            stats[name]++;
        }
    }

    return stats;
}

void PluginFactory::clearPluginCache() {
    std::lock_guard<std::mutex> lock(mutex_);
    pluginCache_.clear();
}

void PluginFactory::setMaxPluginCacheSize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(mutex_);
    maxPluginCacheSize_ = maxSize;
}

} // namespace plugin
} // namespace nlm
