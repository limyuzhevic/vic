#include "brain/Plugin.hpp"
#include <unordered_map>
#include <functional>

namespace nlm {

// Plugin registry implementation

void PluginRegistry::registerPlugin(const std::string& name, 
                                   std::function<std::unique_ptr<IPluginModule>()> factory,
                                   const std::string& description,
                                   bool enabled) {
    plugins_[name] = {factory, description, enabled};
}

std::unique_ptr<IPluginModule> PluginRegistry::createPlugin(const std::string& name, 
                                                           Brain* brain,
                                                           std::shared_ptr<Config> config) {
    auto it = plugins_.find(name);
    if (it != plugins_.end() && it->second.enabled) {
        auto plugin = it->second.factory();
        if (plugin) {
            plugin->initialize(brain, config);
        }
        return plugin;
    }
    return nullptr;
}

std::vector<std::string> PluginRegistry::getRegisteredPluginNames() const {
    std::vector<std::string> names;
    for (const auto& pair : plugins_) {
        names.push_back(pair.first);
    }
    return names;
}

std::string PluginRegistry::getPluginDescription(const std::string& name) const {
    auto it = plugins_.find(name);
    return (it != plugins_.end()) ? it->second.description : "";
}

void PluginRegistry::setPluginEnabled(const std::string& name, bool enabled) {
    auto it = plugins_.find(name);
    if (it != plugins_.end()) {
        it->second.enabled = enabled;
    }
}

bool PluginRegistry::isPluginEnabled(const std::string& name) const {
    auto it = plugins_.find(name);
    return (it != plugins_.end()) ? it->second.enabled : false;
}

void PluginRegistry::loadFromConfig(const Config& config) {
    // Load enabled plugins from configuration
    // This would typically read from a config file or database
    // For now, this is a placeholder implementation
}

void PluginRegistry::updateAll(double timestep) {
    for (auto& pair : plugins_) {
        if (pair.second.enabled) {
            // Note: This is a simplified approach. In a real implementation,
            // we would need to store plugin instances and update them.
            // For now, we assume plugins are created and stored elsewhere.
        }
    }
}

bool PluginRegistry::initializeAll(Brain* brain, std::shared_ptr<Config> config) {
    bool success = true;
    for (auto& pair : plugins_) {
        if (pair.second.enabled) {
            auto plugin = pair.second.factory();
            if (plugin && !plugin->initialize(brain, config)) {
                success = false;
            }
        }
    }
    return success;
}

} // namespace nlm