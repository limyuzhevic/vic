#include "include/nlm/plugin/PluginRegistry.h"
#include <iostream>
#include <algorithm>
#include <ctime>

namespace nlm {
namespace plugin {

PluginRegistry::PluginRegistry() {
}

PluginRegistry::~PluginRegistry() {
}

bool PluginRegistry::registerPlugin(const PluginInfo& pluginInfo) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (registeredPlugins_.find(pluginInfo.id) != registeredPlugins_.end()) {
        return false;
    }

    if (!validatePlugin(pluginInfo)) {
        return false;
    }

    registeredPlugins_[pluginInfo.id] = pluginInfo;
    pluginStates_[pluginInfo.id] = pluginInfo.state;
    return true;
}

bool PluginRegistry::unregisterPlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = registeredPlugins_.find(pluginId);
    if (it == registeredPlugins_.end()) {
        return false;
    }

    registeredPlugins_.erase(it);
    pluginStates_.erase(pluginId);
    return true;
}

bool PluginRegistry::updatePluginInfo(const std::string& pluginId, const PluginInfo& pluginInfo) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = registeredPlugins_.find(pluginId);
    if (it == registeredPlugins_.end()) {
        return false;
    }

    registeredPlugins_[pluginId] = pluginInfo;
    pluginStates_[pluginId] = pluginInfo.state;
    return true;
}

PluginInfo PluginRegistry::getPluginInfo(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = registeredPlugins_.find(pluginId);
    if (it != registeredPlugins_.end()) {
        return it->second;
    }

    PluginInfo emptyInfo;
    return emptyInfo;
}

PluginInfo PluginRegistry::getPluginInfoByName(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);

    for (const auto& pair : registeredPlugins_) {
        if (pair.second.name == name) {
            return pair.second;
        }
    }

    PluginInfo emptyInfo;
    return emptyInfo;
}

std::vector<PluginInfo> PluginRegistry::findPluginsByCapability(PluginCapability capability) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PluginInfo> result;

    for (const auto& pair : registeredPlugins_) {
        if (std::find(pair.second.capabilities.begin(), pair.second.capabilities.end(), capability) != pair.second.capabilities.end()) {
            result.push_back(pair.second);
        }
    }

    return result;
}

std::vector<PluginInfo> PluginRegistry::findPluginsByNamePattern(const std::string& pattern) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PluginInfo> result;

    for (const auto& pair : registeredPlugins_) {
        if (pair.second.name.find(pattern) != std::string::npos) {
            result.push_back(pair.second);
        }
    }

    return result;
}

std::vector<PluginInfo> PluginRegistry::getAllPlugins() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PluginInfo> allPlugins;

    for (const auto& pair : registeredPlugins_) {
        allPlugins.push_back(pair.second);
    }

    return allPlugins;
}

std::vector<PluginInfo> PluginRegistry::getLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PluginInfo> loadedPlugins;

    for (const auto& pair : registeredPlugins_) {
        if (pair.second.state == PluginState::LOADED || pair.second.state == PluginState::INITIALIZED) {
            loadedPlugins.push_back(pair.second);
        }
    }

    return loadedPlugins;
}

std::vector<PluginInfo> PluginRegistry::getAvailablePlugins() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PluginInfo> availablePlugins;

    for (const auto& pair : registeredPlugins_) {
        if (pair.second.state == PluginState::UNLOADED) {
            availablePlugins.push_back(pair.second);
        }
    }

    return availablePlugins;
}

bool PluginRegistry::validatePlugin(const PluginInfo& pluginInfo) const {
    if (pluginInfo.id.empty() || pluginInfo.name.empty()) {
        return false;
    }

    if (pluginInfo.version.empty()) {
        return false;
    }

    if (!checkVersionCompatibility(pluginInfo)) {
        return false;
    }

    if (!checkDependencies(pluginInfo)) {
        return false;
    }

    return true;
}

bool PluginRegistry::checkVersionCompatibility(const PluginInfo& pluginInfo) const {
    if (pluginInfo.version.empty()) {
        return false;
    }

    if (pluginInfo.dependencies.empty()) {
        return true;
    }

    for (const auto& dep : pluginInfo.dependencies) {
        auto depInfo = getPluginInfo(dep);
        if (depInfo.id.empty()) {
            return false;
        }

        if (!checkVersionCompatibility(depInfo)) {
            return false;
        }
    }

    return true;
}

bool PluginRegistry::checkDependencies(const PluginInfo& pluginInfo) const {
    if (pluginInfo.dependencies.empty()) {
        return true;
    }

    for (const auto& dep : pluginInfo.dependencies) {
        auto depInfo = getPluginInfo(dep);
        if (depInfo.id.empty()) {
            return false;
        }

        if (depInfo.loadStatus != PluginLoadStatus::SUCCESS) {
            return false;
        }
    }

    return true;
}

bool PluginRegistry::setPluginState(const std::string& pluginId, PluginState state) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = pluginStates_.find(pluginId);
    if (it == pluginStates_.end()) {
        return false;
    }

    it->second = state;

    auto pluginIt = registeredPlugins_.find(pluginId);
    if (pluginIt != registeredPlugins_.end()) {
        pluginIt->second.state = state;
    }

    return true;
}

PluginState PluginRegistry::getPluginState(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = pluginStates_.find(pluginId);
    if (it != pluginStates_.end()) {
        return it->second;
    }

    return PluginState::UNLOADED;
}

bool PluginRegistry::setPluginLoadStatus(const std::string& pluginId, PluginLoadStatus status) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = registeredPlugins_.find(pluginId);
    if (it == registeredPlugins_.end()) {
        return false;
    }

    it->second.loadStatus = status;
    return true;
}

PluginLoadStatus PluginRegistry::getPluginLoadStatus(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = registeredPlugins_.find(pluginId);
    if (it != registeredPlugins_.end()) {
        return it->second.loadStatus;
    }

    return PluginLoadStatus::FAILED;
}

void PluginRegistry::setPluginConfig(const std::string& pluginId, const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = registeredPlugins_.find(pluginId);
    if (it != registeredPlugins_.end()) {
        it->second.metadata["config"] = "{" + std::to_string(config.size()) + " items}";
    }
}

std::map<std::string, std::string> PluginRegistry::getPluginConfig(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = registeredPlugins_.find(pluginId);
    if (it != registeredPlugins_.end()) {
        auto metadataIt = it->second.metadata.find("config");
        if (metadataIt != it->second.metadata.end()) {
            return {};
        }
    }

    return {};
}

std::vector<PluginInfo> PluginRegistry::filterPlugins(std::function<bool(const PluginInfo&)> predicate) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PluginInfo> result;

    for (const auto& pair : registeredPlugins_) {
        if (predicate(pair.second)) {
            result.push_back(pair.second);
        }
    }

    return result;
}

} // namespace plugin
} // namespace nlm
