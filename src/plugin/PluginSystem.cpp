#include <chrono>
#include <algorithm>
#include <numeric>

namespace nlm {

struct PluginSystem::Impl {
    Brain* brain = nullptr;
    std::shared_ptr<const Config> config;
    std::unordered_map<std::string, std::unique_ptr<IBrainPlugin>> loadedPlugins;
    PluginRegistry registry;
    std::atomic<size_t> activePluginCount{0};
    std::vector<float> pluginLoadTimes;
    bool initialized = false;
};

PluginSystem::PluginSystem() : pImpl(std::make_unique<Impl>()) {
}

PluginSystem::~PluginSystem() {
    shutdown();
}

bool PluginSystem::initialize(Brain* brain, std::shared_ptr<const Config> config) {
    if (!brain || !config) {
        return false;
    }

    pImpl->brain = brain;
    pImpl->config = config;

    // Load built-in plugins
    pImpl->registry.loadPluginsFromDirectory("plugins/");

    // Set up plugin system for each plugin
    for (const auto& pluginName : pImpl->registry.getAvailablePlugins()) {
        loadPlugin(pluginName);
    }

    pImpl->initialized = true;
    return true;
}

void PluginSystem::update(SimulationStep step) {
    if (!pImpl->initialized) {
        return;
    }

    for (auto& pair : pImpl->loadedPlugins) {
        if (pair.second) {
            pair.second->update(step);
        }
    }
}

void PluginSystem::shutdown() {
    if (!pImpl->initialized) {
        return;
    }

    // Shutdown plugins in reverse order
    for (auto it = pImpl->loadedPlugins.rbegin(); it != pImpl->loadedPlugins.rend(); ++it) {
        if (it->second) {
            it->second->shutdown();
        }
    }

    pImpl->loadedPlugins.clear();
    pImpl->brain = nullptr;
    pImpl->config.reset();
    pImpl->initialized = false;
}

bool PluginSystem::loadPlugin(const std::string& pluginName) {
    if (!pImpl->initialized) {
        return false;
    }

    if (isPluginLoaded(pluginName)) {
        return true;
    }

    auto startTime = std::chrono::high_resolution_clock::now();

    std::unique_ptr<IBrainPlugin> plugin = pImpl->registry.createPlugin(pluginName);
    if (!plugin) {
        return false;
    }

    // Configure plugin
    if (!plugin->initialize(pImpl->config)) {
        return false;
    }

    // Set up integration
    plugin->setBrain(pImpl->brain);
    plugin->setPluginSystem(this);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto loadTime = std::chrono::duration<float, std::chrono::milliseconds::period>(endTime - startTime).count();

    pImpl->pluginLoadTimes.push_back(loadTime);
    pImpl->loadedPlugins[pluginName] = std::move(plugin);
    pImpl->activePluginCount.fetch_add(1, std::memory_order_relaxed);

    return true;
}

bool PluginSystem::unloadPlugin(const std::string& pluginName) {
    auto it = pImpl->loadedPlugins.find(pluginName);
    if (it == pImpl->loadedPlugins.end()) {
        return false;
    }

    if (it->second) {
        it->second->shutdown();
    }

    pImpl->loadedPlugins.erase(it);
    pImpl->activePluginCount.fetch_sub(1, std::memory_order_relaxed);
    return true;
}

bool PluginSystem::isPluginLoaded(const std::string& pluginName) const {
    return pImpl->loadedPlugins.find(pluginName) != pImpl->loadedPlugins.end();
}

bool PluginSystem::configurePlugin(const std::string& pluginName, const std::string& key, const std::any& value) {
    auto it = pImpl->loadedPlugins.find(pluginName);
    if (it == pImpl->loadedPlugins.end()) {
        return false;
    }

    return it->second->configure(key, value);
}

std::any PluginSystem::getPluginConfig(const std::string& pluginName, const std::string& key) const {
    auto it = pImpl->loadedPlugins.find(pluginName);
    if (it == pImpl->loadedPlugins.end()) {
        return std::any();
    }

    return it->second->getConfig(key);
}

PluginMetadata PluginSystem::getPluginMetadata(const std::string& pluginName) const {
    return pImpl->registry.getPluginMetadata(pluginName);
}

std::vector<std::string> PluginSystem::getLoadedPlugins() const {
    std::vector<std::string> loadedPlugins;
    loadedPlugins.reserve(pImpl->loadedPlugins.size());

    for (const auto& pair : pImpl->loadedPlugins) {
        loadedPlugins.push_back(pair.first);
    }

    return loadedPlugins;
}

std::vector<std::string> PluginSystem::getAvailablePlugins() const {
    return pImpl->registry.getAvailablePlugins();
}

void PluginSystem::publishEvent(const std::string& eventType, const std::any& eventData) {
    for (auto& pair : pImpl->loadedPlugins) {
        if (pair.second) {
            pair.second->onEvent(eventType, eventData);
        }
    }
}

size_t PluginSystem::getActivePluginCount() const {
    return pImpl->activePluginCount.load(std::memory_order_relaxed);
}

float PluginSystem::getAveragePluginLoadTime() const {
    if (pImpl->pluginLoadTimes.empty()) {
        return 0.0f;
    }

    float sum = std::accumulate(pImpl->pluginLoadTimes.begin(), pImpl->pluginLoadTimes.end(), 0.0f);
    return sum / pImpl->pluginLoadTimes.size();
}

PluginSystem& PluginSystem::getInstance() {
    static PluginSystem instance;
    return instance;
}

} // namespace nlm