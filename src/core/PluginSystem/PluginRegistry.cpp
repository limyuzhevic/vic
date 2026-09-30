#include "PluginRegistry.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct DefaultPluginRegistry::Impl {
    // Plugin storage
    struct PluginInfo {
        PluginMetadata metadata;
        std::unique_ptr<Plugin> instance;
        std::unordered_map<std::string, std::any> config;
        bool loaded = false;
        bool active = false;
        std::vector<std::string> errors;
        std::chrono::system_clock::time_point loadTime;
        size_t memoryUsage = 0;
    };
    
    std::unordered_map<std::string, PluginInfo> plugins;
    std::vector<PluginMetadata> pluginFactories;
    
    // Factory function types
    using PluginFactoryFunc = std::function<std::unique_ptr<Plugin>()>;
    using ExperimentFactoryFunc = std::function<std::unique_ptr<ExperimentPlugin>()>;
    using CommandFactoryFunc = std::function<std::unique_ptr<CommandPlugin>()>;
    using FeatureFactoryFunc = std::function<std::unique_ptr<FeaturePlugin>()>;
    
    std::unordered_map<std::string, PluginFactoryFunc> pluginFactories;
    std::unordered_map<std::string, ExperimentFactoryFunc> experimentFactories;
    std::unordered_map<std::string, CommandFactoryFunc> commandFactories;
    std::unordered_map<std::string, FeatureFactoryFunc> featureFactories;
    
    // Plugin tags
    std::unordered_map<std::string, std::vector<std::string>> pluginTags;
    
    // Plugin validation
    std::unordered_map<std::string, std::function<bool(const std::unordered_map<std::string, std::any>&)>> validators;
};

DefaultPluginRegistry::DefaultPluginRegistry() : pImpl(std::make_unique<Impl>()) {}

DefaultPluginRegistry::~DefaultPluginRegistry() = default;

// Plugin registration with factory functions
bool DefaultPluginRegistry::registerPlugin(const PluginMetadata& metadata, PluginFactory factory) {
    // Validate metadata
    if (metadata.name.empty()) {
        return false;
    }
    
    // Check if plugin already exists
    if (pImpl->plugins.find(metadata.name) != pImpl->plugins.end()) {
        return false;
    }
    
    // Store factory
    pImpl->pluginFactories[metadata.name] = [factory]() { return factory(); };
    pImpl->pluginFactories[metadata.name] = [factory]() { return factory(); };
    
    // Store metadata
    pImpl->pluginFactories.push_back(metadata);
    
    // Store tags
    for (const auto& tag : metadata.tags) {
        pImpl->pluginTags[tag].push_back(metadata.name);
    }
    
    return true;
}

bool DefaultPluginRegistry::registerPlugin(const PluginMetadata& metadata, ExperimentFactory factory) {
    // Similar to Plugin registration but for Experiment plugins
    if (metadata.name.empty()) {
        return false;
    }
    
    if (pImpl->plugins.find(metadata.name) != pImpl->plugins.end()) {
        return false;
    }
    
    pImpl->experimentFactories[metadata.name] = [factory]() { return factory(); };
    pImpl->pluginFactories.push_back(metadata);
    
    for (const auto& tag : metadata.tags) {
        pImpl->pluginTags[tag].push_back(metadata.name);
    }
    
    return true;
}

bool DefaultPluginRegistry::registerPlugin(const PluginMetadata& metadata, CommandFactory factory) {
    // Similar to Plugin registration but for Command plugins
    if (metadata.name.empty()) {
        return false;
    }
    
    if (pImpl->plugins.find(metadata.name) != pImpl->plugins.end()) {
        return false;
    }
    
    pImpl->commandFactories[metadata.name] = [factory]() { return factory(); };
    pImpl->pluginFactories.push_back(metadata);
    
    for (const auto& tag : metadata.tags) {
        pImpl->pluginTags[tag].push_back(metadata.name);
    }
    
    return true;
}

bool DefaultPluginRegistry::registerPlugin(const PluginMetadata& metadata, FeatureFactory factory) {
    // Similar to Plugin registration but for Feature plugins
    if (metadata.name.empty()) {
        return false;
    }
    
    if (pImpl->plugins.find(metadata.name) != pImpl->plugins.end()) {
        return false;
    }
    
    pImpl->featureFactories[metadata.name] = [factory]() { return factory(); };
    pImpl->pluginFactories.push_back(metadata);
    
    for (const auto& tag : metadata.tags) {
        pImpl->pluginTags[tag].push_back(metadata.name);
    }
    
    return true;
}

std::vector<PluginMetadata> DefaultPluginRegistry::getPlugins() const {
    std::vector<PluginMetadata> result;
    for (const auto& [name, info] : pImpl->plugins) {
        result.push_back(info.metadata);
    }
    return result;
}

std::vector<PluginMetadata> DefaultPluginRegistry::getPlugins(PluginType type) const {
    std::vector<PluginMetadata> result;
    for (const auto& [name, info] : pImpl->plugins) {
        if (info.metadata.type == type) {
            result.push_back(info.metadata);
        }
    }
    return result;
}

std::vector<PluginMetadata> DefaultPluginRegistry::getPluginsByTag(const std::string& tag) const {
    std::vector<PluginMetadata> result;
    if (pImpl->pluginTags.find(tag) == pImpl->pluginTags.end()) {
        return result;
    }
    
    for (const auto& name : pImpl->pluginTags.at(tag)) {
        auto it = pImpl->plugins.find(name);
        if (it != pImpl->plugins.end()) {
            result.push_back(it->second.metadata);
        }
    }
    
    return result;
}

PluginMetadata DefaultPluginRegistry::getPluginMetadata(const std::string& name) const {
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end()) {
        return it->second.metadata;
    }
    return PluginMetadata(); // Default constructor returns empty metadata
}

bool DefaultPluginRegistry::loadPlugin(const std::string& name) {
    auto it = pImpl->pluginFactories.find(name);
    if (it == pImpl->pluginFactories.end()) {
        return false;
    }
    
    auto& pluginInfo = pImpl->plugins[name];
    
    if (pluginInfo.loaded) {
        return true; // Already loaded
    }
    
    // Create plugin instance based on type
    bool success = false;
    switch (pluginInfo.metadata.type) {
        case PluginType::Experiment:
            if (pImpl->experimentFactories.find(name) != pImpl->experimentFactories.end()) {
                pluginInfo.instance = pImpl->experimentFactories[name]();
                success = true;
            }
            break;
        case PluginType::Command:
            if (pImpl->commandFactories.find(name) != pImpl->commandFactories.end()) {
                pluginInfo.instance = pImpl->commandFactories[name]();
                success = true;
            }
            break;
        case PluginType::Feature:
            if (pImpl->featureFactories.find(name) != pImpl->featureFactories.end()) {
                pluginInfo.instance = pImpl->featureFactories[name]();
                success = true;
            }
            break;
        default: // PluginType::Plugin
            pluginInfo.instance = it->second();
            success = true;
    }
    
    if (success && pluginInfo.instance) {
        pluginInfo.loaded = true;
        pluginInfo.active = true;
        pluginInfo.loadTime = std::chrono::system_clock::now();
        pluginInfo.errors.clear();
        
        // Initialize plugin with empty config
        pluginInfo.instance->setActive(true);
        return true;
    }
    
    return false;
}

bool DefaultPluginRegistry::unloadPlugin(const std::string& name) {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end() || !it->second.loaded) {
        return false;
    }
    
    it->second.instance->setActive(false);
    it->second.instance->cleanup();
    it->second.instance.reset();
    it->second.loaded = false;
    it->second.active = false;
    it->second.errors.clear();
    
    return true;
}

bool DefaultPluginRegistry::activatePlugin(const std::string& name) {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end() || !it->second.loaded) {
        return false;
    }
    
    it->second.active = true;
    it->second.instance->setActive(true);
    
    // Validate plugin with current config
    if (!validatePluginConfig(name, it->second.config)) {
        it->second.active = false;
        it->second.instance->setActive(false);
        return false;
    }
    
    return true;
}

bool DefaultPluginRegistry::deactivatePlugin(const std::string& name) {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        return false;
    }
    
    if (it->second.active) {
        it->second.instance->setActive(false);
    }
    it->second.active = false;
    
    return true;
}

bool DefaultPluginRegistry::isPluginLoaded(const std::string& name) const {
    auto it = pImpl->plugins.find(name);
    return it != pImpl->plugins.end() && it->second.loaded;
}

bool DefaultPluginRegistry::isPluginActive(const std::string& name) const {
    auto it = pImpl->plugins.find(name);
    return it != pImpl->plugins.end() && it->second.active;
}

bool DefaultPluginRegistry::executePluginCommand(const std::string& pluginName, const std::string& command,
                                                const std::vector<std::string>& args) {
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end() || !it->second.active) {
        return false;
    }
    
    try {
        it->second.instance->execute(command, args);
        return true;
    } catch (const std::exception& e) {
        it->second.errors.push_back(std::string("Command execution failed: ") + e.what());
        return false;
    }
}

std::any DefaultPluginRegistry::getPluginResult(const std::string& pluginName, const std::string& query) {
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end() || !it->second.active) {
        return std::any();
    }
    
    try {
        return it->second.instance->getResult(query);
    } catch (const std::exception& e) {
        it->second.errors.push_back(std::string("Query failed: ") + e.what());
        return std::any();
    }
}

bool DefaultPluginRegistry::configurePlugin(const std::string& name, const std::unordered_map<std::string, std::any>& config) {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        return false;
    }
    
    // Validate config
    if (!validatePluginConfig(name, config)) {
        return false;
    }
    
    it->second.config = config;
    
    // Apply config to plugin if active
    if (it->second.active && it->second.instance) {
        if constexpr (std::is_base_of_v<FeaturePlugin, std::remove_pointer_t<decltype(it->second.instance.get())>>) {
            it->second.instance->applyFeature(config);
        }
    }
    
    return true;
}

std::unordered_map<std::string, std::any> DefaultPluginRegistry::getPluginConfig(const std::string& name) const {
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end()) {
        return it->second.config;
    }
    return std::unordered_map<std::string, std::any>();
}

bool DefaultPluginRegistry::validatePluginConfig(const std::string& name, const std::unordered_map<std::string, std::any>& config) const {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        return false;
    }
    
    // Check for required fields in metadata config schema
    const auto& schema = it->second.metadata.configSchema;
    for (const auto& [key, schemaValue] : schema) {
        if (config.find(key) == config.end()) {
            return false;
        }
        
        // Type validation (simplified)
        const auto& configValue = config.at(key);
        // Add type validation logic here
    }
    
    // Call custom validator if registered
    auto validatorIt = pImpl->validators.find(name);
    if (validatorIt != pImpl->validators.end()) {
        return validatorIt->second(config);
    }
    
    return true;
}

std::vector<std::string> DefaultPluginRegistry::getPluginErrors(const std::string& name) const {
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end()) {
        return it->second.errors;
    }
    return std::vector<std::string>();
}
}
