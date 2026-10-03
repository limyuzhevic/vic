#include <algorithm>
#include <cstring>
#include <functional>
#include <vector>

namespace nlm {

struct PluginRegistry::Impl {
    std::vector<std::unique_ptr<IPluginRegistration>> registrations;
    std::unordered_map<std::string, std::unique_ptr<IPluginRegistration>> registrationMap;
    std::unordered_map<std::string, std::unique_ptr<DynamicPluginLoader>> loaders;
};

PluginRegistry::PluginRegistry() : pImpl(std::make_unique<Impl>()) {
}

PluginRegistry::~PluginRegistry() = default;

void PluginRegistry::registerPlugin(std::unique_ptr<IPluginRegistration> registration) {
    if (!registration) {
        return;
    }

    PluginMetadata metadata = registration->getMetadata();
    pImpl->registrationMap[metadata.name] = std::move(registration);
    pImpl->registrations.push_back(pImpl->registrationMap[metadata.name].get());
}

bool PluginRegistry::loadPluginsFromDirectory(const std::string& directory) {
    std::vector<std::string> pluginFiles = PluginDiscovery::discoverPlugins(directory);
    bool success = true;

    for (const auto& file : pluginFiles) {
        std::unique_ptr<DynamicPluginLoader> loader = std::make_unique<DynamicPluginLoader>(file);
        if (loader->load()) {
            pImpl->loaders[file] = std::move(loader);
        } else {
            success = false;
        }
    }

    return success;
}

PluginMetadata PluginRegistry::getPluginMetadata(const std::string& pluginName) const {
    auto it = pImpl->registrationMap.find(pluginName);
    if (it != pImpl->registrationMap.end()) {
        return it->second->getMetadata();
    }
    return PluginMetadata{};
}

bool PluginRegistry::isPluginAvailable(const std::string& pluginName) const {
    return pImpl->registrationMap.find(pluginName) != pImpl->registrationMap.end();
}

std::vector<std::string> PluginRegistry::getAvailablePlugins() const {
    std::vector<std::string> pluginNames;
    pluginNames.reserve(pImpl->registrationMap.size());

    for (const auto& pair : pImpl->registrationMap) {
        pluginNames.push_back(pair.first);
    }

    return pluginNames;
}

std::unique_ptr<IBrainPlugin> PluginRegistry::createPlugin(const std::string& pluginName) {
    auto it = pImpl->registrationMap.find(pluginName);
    if (it == pImpl->registrationMap.end()) {
        return nullptr;
    }

    return it->second->getFactory()();
}

std::vector<std::string> PluginRegistry::getPluginsInCategory(const std::string& category) const {
    std::vector<std::string> pluginsInCategory;

    for (const auto& pair : pImpl->registrationMap) {
        PluginMetadata metadata = pair.second->getMetadata();
        if (metadata.category == category) {
            pluginsInCategory.push_back(pair.first);
        }
    }

    return pluginsInCategory;
}

} // namespace nlm