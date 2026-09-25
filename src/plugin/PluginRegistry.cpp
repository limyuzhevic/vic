// PluginRegistry implementation
// Manages built-in plugin registration and discovery

#include "PluginInterface.hpp"
#include <algorithm>
#include <fstream>
#include <filesystem>

namespace nlm {
namespace plugin {

struct PluginRegistry::Impl {
    std::unordered_map<std::string, std::unique_ptr<IPluginFactory>> factories;
    std::unordered_map<std::string, std::shared_ptr<IPlugin>> loadedPlugins;
    std::mutex mutex;
    bool initialized = false;
};

PluginRegistry& PluginRegistry::getInstance() {
    static PluginRegistry instance;
    return instance;
}

void PluginRegistry::registerPluginFactory(const std::string& name, std::unique_ptr<IPluginFactory> factory) {
    if (!factory || name.empty()) {
        return;  // Error logging would go here
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->factories[name] = std::move(factory);
}

std::unique_ptr<IPluginFactory> PluginRegistry::getPluginFactory(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->factories.find(name);
    return it != pImpl->factories.end() ? std::move(it->second) : nullptr;
}

std::vector<std::pair<std::string, std::unique_ptr<IPluginFactory>>> PluginRegistry::getAllFactories() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::pair<std::string, std::unique_ptr<IPluginFactory>>> result;
    result.reserve(pImpl->factories.size());
    
    for (const auto& pair : pImpl->factories) {
        result.emplace_back(pair.first, std::make_unique<IPluginFactory>(*pair.second));
    }
    
    return result;
}

bool PluginRegistry::loadBuiltInPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->factories.find(name);
    if (it == pImpl->factories.end()) {
        return false;  // Plugin not found
    }
    
    // Check if already loaded
    if (pImpl->loadedPlugins.find(name) != pImpl->loadedPlugins.end()) {
        return true;  // Already loaded
    }
    
    // Create plugin instance
    std::unique_ptr<IPlugin> plugin = it->second->createPlugin();
    if (!plugin) {
        return false;
    }
    
    // Initialize plugin
    if (!plugin->initialize({})) {
        return false;
    }
    
    // Start plugin
    if (!plugin->start()) {
        return false;
    }
    
    // Store plugin
    pImpl->loadedPlugins[name] = std::move(plugin);
    return true;
}

bool PluginRegistry::unloadBuiltInPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->loadedPlugins.find(name);
    if (it == pImpl->loadedPlugins.end()) {
        return false;  // Plugin not loaded
    }
    
    std::shared_ptr<IPlugin> plugin = std::move(it->second);
    pImpl->loadedPlugins.erase(it);
    
    // Shutdown plugin
    if (!plugin->shutdown()) {
        return false;
    }
    
    return true;
}

void PluginRegistry::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->factories.clear();
    pImpl->loadedPlugins.clear();
}

} // namespace plugin
} // namespace nlm