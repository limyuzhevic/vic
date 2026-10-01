#include "PluginSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace nlm {

struct PluginSystem::Impl {
    bool active;
    std::map<std::string, std::unique_ptr<Plugin>> plugins;
    std::map<std::string, bool> pluginsEnabled;
    std::map<std::string, std::map<std::string, std::string>> pluginConfigs;
    std::map<std::string, std::function<std::unique_ptr<Plugin>(const std::string&)>> pluginFactories;
    std::vector<std::string> errorMessages;
    std::vector<std::string> warningMessages;
    std::vector<std::string> infoMessages;
    std::string logLevel;
    std::chrono::steady_clock::time_point systemStartTime;
    
    Impl() : active(false), logLevel("info"), systemStartTime(std::chrono::steady_clock::now()) {
        // Initialize default log level
        setLogLevel(logLevel);
    }
};

PluginSystem::PluginSystem() : pImpl(std::make_unique<Impl>()) {}

PluginSystem::~PluginSystem() {
    shutdown();
}

PluginSystem::PluginSystem(PluginSystem&& other) noexcept : pImpl(std::move(other.pImpl)) {}

PluginSystem& PluginSystem::operator=(PluginSystem&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool PluginSystem::initialize() {
    if (pImpl->active) {
        NLM_LOG_WARNING("PluginSystem: Already initialized");
        return true;
    }
    
    pImpl->active = true;
    NLM_LOG_INFO("PluginSystem: Initialized successfully");
    return true;
}

void PluginSystem::shutdown() {
    if (!pImpl->active) {
        return;
    }
    
    // Shutdown all plugins
    for (auto& pair : pImpl->plugins) {
        if (pair.second->isActive()) {
            pair.second->shutdown();
        }
    }
    
    pImpl->plugins.clear();
    pImpl->pluginsEnabled.clear();
    pImpl->pluginConfigs.clear();
    pImpl->active = false;
    NLM_LOG_INFO("PluginSystem: Shutdown successfully");
}

bool PluginSystem::isActive() const {
    return pImpl->active;
}

bool PluginSystem::registerPlugin(std::unique_ptr<Plugin> plugin) {
    if (!plugin) {
        NLM_LOG_WARNING("PluginSystem: Cannot register null plugin");
        return false;
    }
    
    const std::string& name = plugin->getName();
    
    if (pImpl->plugins.find(name) != pImpl->plugins.end()) {
        NLM_LOG_WARNING("PluginSystem: Plugin with name '" + name + "' already registered");
        return false;
    }
    
    if (!validatePlugin(*plugin)) {
        NLM_LOG_WARNING("PluginSystem: Plugin '" + name + "' validation failed");
        return false;
    }
    
    pImpl->plugins[name] = std::move(plugin);
    pImpl->pluginsEnabled[name] = true; // Enable by default
    
    // Initialize the plugin
    if (!pImpl->plugins[name]->initialize()) {
        NLM_LOG_WARNING("PluginSystem: Failed to initialize plugin '" + name + "'");
        pImpl->plugins.erase(name);
        return false;
    }
    
    NLM_LOG_INFO("PluginSystem: Registered plugin '" + name + "'");
    return true;
}

bool PluginSystem::unregisterPlugin(const std::string& name) {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_WARNING("PluginSystem: Plugin with name '" + name + "' not found");
        return false;
    }
    
    if (it->second->isActive()) {
        it->second->shutdown();
    }
    
    pImpl->plugins.erase(it);
    pImpl->pluginsEnabled.erase(name);
    pImpl->pluginConfigs.erase(name);
    
    NLM_LOG_INFO("PluginSystem: Unregistered plugin '" + name + "'");
    return true;
}

Plugin* PluginSystem::getPlugin(const std::string& name) {
    auto it = pImpl->plugins.find(name);
    return (it != pImpl->plugins.end()) ? it->second.get() : nullptr;
}

std::vector<std::string> PluginSystem::getRegisteredPlugins() const {
    std::vector<std::string> pluginNames;
    for (const auto& pair : pImpl->plugins) {
        pluginNames.push_back(pair.first);
    }
    return pluginNames;
}

std::vector<std::string> PluginSystem::getPluginCapabilities(const std::string& name) const {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        return {};
    }
    return it->second->getCapabilities();
}

void PluginSystem::enablePlugin(const std::string& name) {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_WARNING("PluginSystem: Plugin with name '" + name + "' not found");
        return;
    }
    
    pImpl->pluginsEnabled[name] = true;
    NLM_LOG_INFO("PluginSystem: Enabled plugin '" + name + "'");
}

void PluginSystem::disablePlugin(const std::string& name) {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_WARNING("PluginSystem: Plugin with name '" + name + "' not found");
        return;
    }
    
    if (it->second->isActive()) {
        it->second->setActive(false);
    }
    
    pImpl->pluginsEnabled[name] = false;
    NLM_LOG_INFO("PluginSystem: Disabled plugin '" + name + "'");
}

bool PluginSystem::isPluginEnabled(const std::string& name) const {
    auto it = pImpl->plugins.find(name);
    if (it == pImpl->plugins.end()) {
        return false;
    }
    return pImpl->pluginsEnabled[name];
}

void PluginSystem::updatePlugins(double deltaTime) {
    for (auto& pair : pImpl->plugins) {
        if (pImpl->pluginsEnabled[pair.first]) {
            pair.second->update(deltaTime);
        }
    }
}

std::string PluginSystem::getSystemStatus() const {
    std::stringstream ss;
    ss << "PluginSystem Status:" << std::endl;
    ss << "  Active: " << (pImpl->active ? "true" : "false") << std::endl;
    ss << "  Registered Plugins: " << pImpl->plugins.size() << std::endl;
    
    size_t activePlugins = 0;
    for (const auto& pair : pImpl->plugins) {
        if (pImpl->pluginsEnabled[pair.first]) {
            activePlugins++;
        }
    }
    ss << "  Active Plugins: " << activePlugins << std::endl;
    
    return ss.str();
}

bool PluginSystem::savePluginConfig(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_WARNING("PluginSystem: Failed to open file for writing: " + filename);
        return false;
    }
    
    file << "{" << std::endl;
    file << "  \"plugins\": [" << std::endl;
    
    bool first = true;
    for (const auto& pair : pImpl->plugins) {
        if (!first) {
            file << "," << std::endl;
        }
        first = false;
        
        file << "    {\"name\": \"" << pair.first << "\"," << std::endl;
        file << "     \"enabled\": " << (pImpl->pluginsEnabled.at(pair.first) ? "true" : "false") << "," << std::endl;
        
        // Save plugin config if exists
        auto configIt = pImpl->pluginConfigs.find(pair.first);
        if (configIt != pImpl->pluginConfigs.end()) {
            file << "     \"config\": {";
            bool configFirst = true;
            for (const auto& configPair : configIt->second) {
                if (!configFirst) {
                    file << ",\";
                }
                file << "\"" << configPair.first << "\": \"" << configPair.second << "\"";
                configFirst = false;
            }
            file << "}";
        } else {
            file << "     \"config\": {}";
        }
        file << "}";
    }
    
    file << std::endl << "  ]" << std::endl;
    file << "}" << std::endl;
    
    file.close();
    NLM_LOG_INFO("PluginSystem: Saved plugin configuration to " + filename);
    return true;
}

bool PluginSystem::loadPluginConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_WARNING("PluginSystem: Failed to open file for reading: " + filename);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // TODO: Parse JSON content (placeholder for actual implementation)
    // This would require a JSON parser like nlohmann::json
    
    NLM_LOG_INFO("PluginSystem: Loaded plugin configuration from " + filename);
    return true;
}

std::map<std::string, std::string> PluginSystem::getPluginConfig(const std::string& name) const {
    auto it = pImpl->pluginConfigs.find(name);
    return (it != pImpl->pluginConfigs.end()) ? it->second : std::map<std::string, std::string>();
}

void PluginSystem::setPluginConfig(const std::string& name, const std::map<std::string, std::string>& config) {
    pImpl->pluginConfigs[name] = config;
}

bool PluginSystem::validatePlugin(const Plugin& plugin) const {
    if (plugin.getName().empty()) {
        NLM_LOG_WARNING("PluginSystem: Plugin validation failed - empty name");
        return false;
    }
    
    if (plugin.getVersion().empty()) {
        NLM_LOG_WARNING("PluginSystem: Plugin validation failed - empty version");
        return false;
    }
    
    if (plugin.getDescription().empty()) {
        NLM_LOG_WARNING("PluginSystem: Plugin validation failed - empty description");
        return false;
    }
    
    if (plugin.getAuthor().empty()) {
        NLM_LOG_WARNING("PluginSystem: Plugin validation failed - empty author");
        return false;
    }
    
    return true;
}

std::vector<PluginSystem::PluginInfo> PluginSystem::getPluginsInfo() const {
    std::vector<PluginInfo> infoList;
    
    for (const auto& pair : pImpl->plugins) {
        PluginInfo info;
        info.name = pair.first;
        info.version = pair.second->getVersion();
        info.description = pair.second->getDescription();
        info.author = pair.second->getAuthor();
        info.capabilities = pair.second->getCapabilities();
        info.enabled = pImpl->pluginsEnabled[pair.first];
        info.active = pair.second->isActive();
        infoList.push_back(info);
    }
    
    return infoList;
}

PluginSystem::SystemResources PluginSystem::getSystemResources() const {
    SystemResources resources;
    
    // Count plugins
    resources.totalPluginCount = pImpl->plugins.size();
    
    size_t activeCount = 0;
    for (const auto& pair : pImpl->plugins) {
        if (pImpl->pluginsEnabled[pair.first]) {
            activeCount++;
        }
    }
    resources.activePluginCount = activeCount;
    
    return resources;
}

std::vector<std::string> PluginSystem::findPluginsByCapability(const std::string& capability) const {
    std::vector<std::string> matchingPlugins;
    
    for (const auto& pair : pImpl->plugins) {
        std::vector<std::string> capabilities = pair.second->getCapabilities();
        if (std::find(capabilities.begin(), capabilities.end(), capability) != capabilities.end()) {
            matchingPlugins.push_back(pair.first);
        }
    }
    
    return matchingPlugins;
}

std::unique_ptr<Plugin> PluginSystem::createPlugin(const std::string& pluginType, const std::string& name) {
    auto it = pImpl->pluginFactories.find(pluginType);
    if (it == pImpl->pluginFactories.end()) {
        NLM_LOG_WARNING("PluginSystem: No factory found for plugin type '" + pluginType + "'");
        return nullptr;
    }
    
    return it->second(name);
}

std::function<std::unique_ptr<Plugin>(const std::string&)> PluginSystem::getPluginFactory(const std::string& pluginType) {
    auto it = pImpl->pluginFactories.find(pluginType);
    return (it != pImpl->pluginFactories.end()) ? it->second : nullptr;
}

void PluginSystem::registerPluginFactory(const std::string& pluginType,
                                        std::function<std::unique_ptr<Plugin>(const std::string&)> factory) {
    pImpl->pluginFactories[pluginType] = factory;
}

std::map<std::string, std::string> PluginSystem::getDefaultConfig() {
    std::map<std::string, std::string> defaultConfig;
    defaultConfig["enabled"] = "true";
    defaultConfig["autoStart"] = "true";
    return defaultConfig;
}

bool PluginSystem::validateConfig(const std::map<std::string, std::string>& config) {
    // Basic validation for config
    return !config.empty();
}

std::string PluginSystem::getVersion() {
    return "1.0.0";
}

std::string PluginSystem::getBuildInfo() {
    return "PluginSystem Build Information";
}

std::string PluginSystem::getLicenseInfo() {
    return "PluginSystem License Information";
}

std::string PluginSystem::getCopyrightInfo() {
    return "PluginSystem Copyright Information";
}

std::string PluginSystem::getHelp() {
    return "PluginSystem Help";
}

std::string PluginSystem::getAbout() {
    return "PluginSystem About Information";
}

bool PluginSystem::hasSystemErrors() const {
    return !pImpl->errorMessages.empty();
}

std::vector<std::string> PluginSystem::getSystemErrorMessages() const {
    return pImpl->errorMessages;
}

void PluginSystem::clearSystemErrorMessages() {
    pImpl->errorMessages.clear();
}

std::vector<std::string> PluginSystem::getSystemWarningMessages() const {
    return pImpl->warningMessages;
}

void PluginSystem::clearSystemWarningMessages() {
    pImpl->warningMessages.clear();
}

std::vector<std::string> PluginSystem::getSystemInfoMessages() const {
    return pImpl->infoMessages;
}

void PluginSystem::clearSystemInfoMessages() {
    pImpl->infoMessages.clear();
}

void PluginSystem::logSystemMessage(const std::string& message, const std::string& level) {
    if (level == "warning") {
        pImpl->warningMessages.push_back(message);
    } else if (level == "error") {
        pImpl->errorMessages.push_back(message);
    } else {
        pImpl->infoMessages.push_back(message);
    }
    
    NLM_LOG_INFO("PluginSystem: " + level + ": " + message);
}

void PluginSystem::setLogLevel(const std::string& level) {
    pImpl->logLevel = level;
}

std::string PluginSystem::getLogLevel() const {
    return pImpl->logLevel;
}

bool PluginSystem::exportPluginSystem(const std::string& filename, const std::string& format) {
    NLM_LOG_INFO("PluginSystem: Exporting system data to " + filename + " in format " + format);
    // Export implementation would go here
    return true;
}

bool PluginSystem::importPluginSystem(const std::string& filename, const std::string& format) {
    NLM_LOG_INFO("PluginSystem: Importing system data from " + filename + " in format " + format);
    // Import implementation would go here
    return true;
}

} // namespace nlm