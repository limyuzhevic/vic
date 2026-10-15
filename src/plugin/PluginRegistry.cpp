#include "PluginRegistry.hpp"
#include <mutex>

namespace nlm {

PluginRegistry::PluginRegistry() {
}

PluginRegistry::~PluginRegistry() {
}

PluginRegistry& PluginRegistry::getInstance() {
    static PluginRegistry instance;
    return instance;
}

void PluginRegistry::registerPlugin(const std::string& name, const std::string& version,
                                   const std::string& description, PluginFactory factory,
                                   const std::string& type) {
    pluginFactories[name] = factory;
    pluginInfos[name] = PluginInfo{name, version, description, type, "", false};
}

void PluginRegistry::unregisterPlugin(const std::string& name) {
    pluginFactories.erase(name);
    pluginInfos.erase(name);
}

PluginFactory PluginRegistry::getPluginFactory(const std::string& name) const {
    auto it = pluginFactories.find(name);
    if (it == pluginFactories.end()) {
        return nullptr;
    }
    return it->second;
}

std::vector<std::string> PluginRegistry::getPluginNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pluginFactories) {
        names.push_back(pair.first);
    }
    return names;
}

PluginInfo PluginRegistry::getPluginInfo(const std::string& name) const {
    auto it = pluginInfos.find(name);
    if (it == pluginInfos.end()) {
        return PluginInfo();
    }
    return it->second;
}

bool PluginRegistry::isPluginRegistered(const std::string& name) const {
    return pluginFactories.find(name) != pluginFactories.end();
}

void PluginRegistry::clear() {
    pluginFactories.clear();
    pluginInfos.clear();
}

}} // namespace nlm