#include "include/nlm/plugin/PluginSystem.h"
#include "include/nlm/plugin/PluginLoader.h"
#include "include/nlm/plugin/PluginRegistry.h"
#include "include/nlm/plugin/PluginEventSystem.h"
#include <iostream>

namespace nlm {
namespace plugin {

PluginSystem::PluginSystem() : initialized_(false) {
}

PluginSystem::~PluginSystem() {
    shutdown();
}

bool PluginSystem::loadPlugin(const std::string& pluginPath) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string pluginId = "plugin_" + std::to_string(std::hash<std::string>{}(pluginPath));

    try {
        if (loader_->loadPlugin(pluginPath, pluginId)) {
            PluginInfo pluginInfo;
            pluginInfo.id = pluginId;
            pluginInfo.pluginPath = pluginPath;
            pluginInfo.loadStatus = PluginLoadStatus::SUCCESS;

            if (registry_->registerPlugin(pluginInfo)) {
                loadedPlugins_[pluginId] = loader_->getPluginSymbol<IPlugin>(pluginId, "create");
                pluginStates_[pluginId] = PluginState::LOADED;
                return true;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading plugin " << pluginPath << ": " << e.what() << std::endl;
    }

    return false;
}

bool PluginSystem::unloadPlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = loadedPlugins_.find(pluginId);
    if (it == loadedPlugins_.end()) {
        return false;
    }

    if (it->second) {
        it->second->disable();
        it->second->shutdown();
    }

    if (loader_->unloadPlugin(pluginId)) {
        registry_->unregisterPlugin(pluginId);
        pluginStates_[pluginId] = PluginState::UNLOADED;
        loadedPlugins_.erase(it);
        return true;
    }

    return false;
}

bool PluginSystem::enablePlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = loadedPlugins_.find(pluginId);
    if (it == loadedPlugins_.end() || !it->second) {
        return false;
    }

    if (it->second->enable()) {
        pluginStates_[pluginId] = PluginState::INITIALIZED;
        return true;
    }

    return false;
}

bool PluginSystem::disablePlugin(const std::string& pluginId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = loadedPlugins_.find(pluginId);
    if (it == loadedPlugins_.end() || !it->second) {
        return false;
    }

    if (it->second->disable()) {
        pluginStates_[pluginId] = PluginState::LOADED;
        return true;
    }

    return false;
}

std::vector<std::string> PluginSystem::getAvailablePlugins() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> availablePlugins;

    for (const auto& pair : registry_->getAvailablePlugins()) {
        availablePlugins.push_back(pair.name);
    }

    return availablePlugins;
}

std::vector<std::string> PluginSystem::getLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> loadedPlugins;

    for (const auto& pair : loadedPlugins_) {
        loadedPlugins.push_back(pair.first);
    }

    return loadedPlugins;
}

std::shared_ptr<IPlugin> PluginSystem::getPlugin(const std::string& pluginId) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = loadedPlugins_.find(pluginId);
    if (it != loadedPlugins_.end()) {
        return it->second;
    }

    return nullptr;
}

std::shared_ptr<IPlugin> PluginSystem::getPluginByName(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);

    PluginInfo pluginInfo = registry_->getPluginInfoByName(name);
    if (!pluginInfo.id.empty() && loadedPlugins_.find(pluginInfo.id) != loadedPlugins_.end()) {
        return loadedPlugins_[pluginInfo.id];
    }

    return nullptr;
}

bool PluginSystem::initialize(const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (initialized_) {
        return false;
    }

    try {
        loader_ = std::make_unique<PluginLoader>();
        registry_ = std::make_unique<PluginRegistry>();
        eventSystem_ = std::make_unique<PluginEventSystem>();

        setConfig(config);

        initialized_ = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error initializing plugin system: " << e.what() << std::endl;
        return false;
    }
}

bool PluginSystem::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        return false;
    }

    for (const auto& pair : loadedPlugins_) {
        if (pair.second) {
            pair.second->disable();
            pair.second->shutdown();
        }
    }

    loadedPlugins_.clear();
    pluginStates_.clear();
    systemConfig_.clear();

    loader_.reset();
    registry_.reset();
    eventSystem_.reset();

    initialized_ = false;
    return true;
}

bool PluginSystem::update(double deltaTime) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) {
        return false;
    }

    bool allUpdated = true;

    for (const auto& pair : loadedPlugins_) {
        if (pair.second && pair.second->getState() == PluginState::RUNNING) {
            if (!pair.second->update(deltaTime)) {
                allUpdated = false;
            }
        }
    }

    return allUpdated;
}

void PluginSystem::setConfig(const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    systemConfig_ = config;
}

std::map<std::string, std::string> PluginSystem::getConfig() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return systemConfig_;
}

void PluginSystem::onPluginEvent(const std::string& pluginId, const std::string& eventName, const std::map<std::string, std::string>& data) {
    if (eventSystem_) {
        PluginEvent event;
        event.eventName = eventName;
        event.sourcePluginId = pluginId;
        event.data = data;
        event.timestamp = 0.0;
        event.propagate = true;

        eventSystem_->publishEvent(event);
    }
}

std::string PluginSystem::getSystemVersion() const {
    return "1.0.0";
}

std::vector<std::string> PluginSystem::getSupportedPlatforms() const {
    return {"Windows", "Linux", "macOS"};
}

} // namespace plugin
} // namespace nlm
