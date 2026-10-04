#include "IPlugin.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

namespace nlm {

// Static plugin registry
namespace {
    std::map<std::string, IPlugin::PluginFactory> g_pluginRegistry;
    std::map<std::string, std::unique_ptr<IPlugin>> g_loadedPlugins;
    std::map<std::string, PluginStatus> g_pluginStatuses;
    std::string g_lastError;
}

// IPlugin implementation
bool IPlugin::registerPlugin(const std::string& name, PluginFactory factory) {
    if (!PluginUtils::validatePluginName(name)) {
        g_lastError = "Invalid plugin name: " + name;
        return false;
    }
    
    if (g_pluginRegistry.find(name) != g_pluginRegistry.end()) {
        g_lastError = "Plugin already registered: " + name;
        return false;
    }
    
    g_pluginRegistry[name] = std::move(factory);
    g_lastError.clear();
    return true;
}

std::vector<std::string> IPlugin::getRegisteredPluginNames() {
    std::vector<std::string> names;
    names.reserve(g_pluginRegistry.size());
    for (const auto& pair : g_pluginRegistry) {
        names.push_back(pair.first);
    }
    return names;
}

// PluginLoader implementation
PluginLoader& PluginLoader::getInstance() {
    static PluginLoader instance;
    return instance;
}

struct PluginLoader::Impl {
    std::map<std::string, std::unique_ptr<IPlugin>> plugins;
    std::map<std::string, std::map<std::string, std::string>> pluginConfigs;
    std::map<std::string, PluginStatus> pluginStatus;
    std::string pluginDirectory;
    std::string configDirectory;
    std::string dataDirectory;
    
    Impl() {
        pluginDirectory = PluginUtils::getPluginDirectory();
        configDirectory = PluginUtils::getPluginConfigDirectory();
        dataDirectory = PluginUtils::getPluginDataDirectory();
        
        // Initialize all plugins to unloaded status
        for (const auto& pair : g_pluginRegistry) {
            pluginStatus[pair.first] = PluginStatus::UNLOADED;
        }
    }
};

PluginLoader::PluginLoader() : pImpl(std::make_unique<Impl>()) {}

bool PluginLoader::loadPlugin(const std::string& pluginName,
                            const std::string& configPath,
                            std::map<std::string, std::string> config) {
    // Check if plugin is already loaded
    if (hasPlugin(pluginName)) {
        g_lastError = "Plugin already loaded: " + pluginName;
        return false;
    }
    
    // Check if plugin is registered
    auto it = g_pluginRegistry.find(pluginName);
    if (it == g_pluginRegistry.end()) {
        g_lastError = "Plugin not found: " + pluginName;
        return false;
    }
    
    // Update status
    pImpl->pluginStatus[pluginName] = PluginStatus::LOADING;
    
    try {
        // Create plugin instance
        auto factory = it->second;
        auto plugin = factory();
        
        // Load configuration
        std::map<std::string, std::string> mergedConfig = std::move(config);
        if (!configPath.empty()) {
            auto fileConfig = PluginUtils::parseConfigFile(configPath);
            mergedConfig.insert(fileConfig.begin(), fileConfig.end());
        }
        
        // Initialize plugin
        if (!plugin->initialize(mergedConfig)) {
            pImpl->pluginStatus[pluginName] = PluginStatus::ERROR;
            g_lastError = "Plugin initialization failed: " + pluginName;
            return false;
        }
        
        // Store plugin and config
        pImpl->plugins[pluginName] = std::move(plugin);
        pImpl->pluginConfigs[pluginName] = std::move(mergedConfig);
        pImpl->pluginStatus[pluginName] = PluginStatus::LOADED;
        
        g_lastError.clear();
        return true;
        
    } catch (const std::exception& e) {
        pImpl->pluginStatus[pluginName] = PluginStatus::ERROR;
        g_lastError = "Plugin loading exception: " + std::string(e.what());
        return false;
    }
}

bool PluginLoader::unloadPlugin(const std::string& pluginName) {
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        g_lastError = "Plugin not loaded: " + pluginName;
        return false;
    }
    
    // Update status
    pImpl->pluginStatus[pluginName] = PluginStatus::UNLOADING;
    
    try {
        // Shutdown plugin
        it->second->shutdown();
        
        // Remove plugin and its data
        pImpl->plugins.erase(it);
        pImpl->pluginConfigs.erase(pluginName);
        pImpl->pluginStatus[pluginName] = PluginStatus::UNLOADED;
        
        g_lastError.clear();
        return true;
        
    } catch (const std::exception& e) {
        pImpl->pluginStatus[pluginName] = PluginStatus::ERROR;
        g_lastError = "Plugin unloading exception: " + std::string(e.what());
        return false;
    }
}

std::unique_ptr<IPlugin>& PluginLoader::getPlugin(const std::string& pluginName) {
    static std::unique_ptr<IPlugin> nullPlugin;
    
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        g_lastError = "Plugin not found: " + pluginName;
        return nullPlugin;
    }
    
    return it->second;
}

bool PluginLoader::hasPlugin(const std::string& pluginName) const {
    return pImpl->plugins.find(pluginName) != pImpl->plugins.end();
}

std::vector<std::string> PluginLoader::getPluginsByCategory(PluginCategory category) const {
    std::vector<std::string> plugins;
    
    for (const auto& pair : pImpl->plugins) {
        auto plugin = pair.second.get();
        if (plugin && plugin->getCategory() == category) {
            plugins.push_back(pair.first);
        }
    }
    
    return plugins;
}

PluginStatus PluginLoader::getPluginStatus(const std::string& pluginName) const {
    auto it = pImpl->pluginStatus.find(pluginName);
    if (it == pImpl->pluginStatus.end()) {
        return PluginStatus::UNLOADED;
    }
    return it->second;
}

std::map<std::string, PluginStatus> PluginLoader::getAllPluginStatuses() const {
    return pImpl->pluginStatus;
}

bool PluginLoader::updatePluginConfig(const std::string& pluginName,
                                     const std::map<std::string, std::string>& config) {
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        g_lastError = "Plugin not found: " + pluginName;
        return false;
    }
    
    // Update configuration
    pImpl->pluginConfigs[pluginName] = config;
    
    // Apply configuration to plugin
    for (const auto& param : config) {
        it->second->setParameter(param.first, param.second);
    }
    
    g_lastError.clear();
    return true;
}

std::map<std::string, std::string> PluginLoader::getPluginConfig(const std::string& pluginName) const {
    auto it = pImpl->pluginConfigs.find(pluginName);
    if (it == pImpl->pluginConfigs.end()) {
        return std::map<std::string, std::string>();
    }
    return it->second;
}

bool PluginLoader::enablePlugin(const std::string& pluginName) {
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        g_lastError = "Plugin not found: " + pluginName;
        return false;
    }
    
    it->second->setEnabled(true);
    g_lastError.clear();
    return true;
}

bool PluginLoader::disablePlugin(const std::string& pluginName) {
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        g_lastError = "Plugin not found: " + pluginName;
        return false;
    }
    
    it->second->setEnabled(false);
    g_lastError.clear();
    return true;
}

bool PluginLoader::restartPlugin(const std::string& pluginName) {
    if (!unloadPlugin(pluginName)) {
        return false;
    }
    
    // Reload with existing config
    auto config = getPluginConfig(pluginName);
    return loadPlugin(pluginName, "", config);
}

// PluginLoader factory methods
std::unique_ptr<IPlugin> PluginLoader::createPlasticityRulePlugin(const std::string& ruleType) {
    if (ruleType == "STDP") {
        auto plugin = std::make_unique<STDP>();
        return plugin;
    } else if (ruleType == "Hebbian") {
        auto plugin = std::make_unique<HebbianRule>();
        return plugin;
    } else if (ruleType == "StructuralPlasticity") {
        auto plugin = std::make_unique<StructuralPlasticity>();
        return plugin;
    } else if (ruleType == "AntiHebbian") {
        auto plugin = std::make_unique<AntiHebbianRule>();
        return plugin;
    }
    
    g_lastError = "Unknown plasticity rule type: " + ruleType;
    return nullptr;
}

std::unique_ptr<IPlugin> PluginLoader::createNeuromodulatorPlugin(const std::string& neuromodType) {
    if (neuromodType == "Dopamine") {
        auto plugin = std::make_unique<Dopamine>();
        return plugin;
    } else if (neuromodType == "Curiosity") {
        auto plugin = std::make_unique<Curiosity>();
        return plugin;
    } else if (neuromodType == "Novelty") {
        auto plugin = std::make_unique<Novelty>();
        return plugin;
    } else if (neuromodType == "Acetylcholine") {
        auto plugin = std::make_unique<Acetylcholine>();
        return plugin;
    }
    
    g_lastError = "Unknown neuromodulator type: " + neuromodType;
    return nullptr;
}

std::unique_ptr<IPlugin> PluginLoader::createRewardFunctionPlugin(const std::string& rewardType) {
    if (rewardType == "PredictionError") {
        auto plugin = std::make_unique<PredictionError>();
        return plugin;
    } else if (rewardType == "Reward") {
        auto plugin = std::make_unique<Reward>();
        return plugin;
    }
    
    g_lastError = "Unknown reward function type: " + rewardType;
    return nullptr;
}

std::unique_ptr<IPlugin> PluginLoader::createDevelopmentModelPlugin(const std::string& modelType) {
    if (modelType == "DevelopmentSystem") {
        auto plugin = std::make_unique<DevelopmentSystem>();
        return plugin;
    } else if (modelType == "Synaptogenesis") {
        auto plugin = std::make_unique<Synaptogenesis>();
        return plugin;
    } else if (modelType == "Pruning") {
        auto plugin = std::make_unique<Pruning>();
        return plugin;
    } else if (modelType == "Maturation") {
        auto plugin = std::make_unique<Maturation>();
        return plugin;
    }
    
    g_lastError = "Unknown development model type: " + modelType;
    return nullptr;
}

// PluginUtils implementation
std::map<std::string, std::string> PluginUtils::parseConfigFile(const std::string& configPath) {
    std::map<std::string, std::string> config;
    
    std::ifstream file(configPath);
    if (!file.is_open()) {
        g_lastError = "Cannot open config file: " + configPath;
        return config;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
        
        if (line.empty()) {
            continue;
        }
        
        size_t separator = line.find('=');
        if (separator == std::string::npos) {
            g_lastError = "Invalid config line (no '='): " + line;
            continue;
        }
        
        std::string key = line.substr(0, separator);
        std::string value = line.substr(separator + 1);
        
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        config[key] = value;
    }
    
    return config;
}

std::string PluginUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

bool PluginUtils::stringToFloat(const std::string& str, float& value) {
    std::istringstream iss(str);
    iss >> value;
    return !iss.fail();
}

bool PluginUtils::stringToInt(const std::string& str, int& value) {
    std::istringstream iss(str);
    iss >> value;
    return !iss.fail();
}

bool PluginUtils::validatePluginName(const std::string& name) {
    if (name.empty()) return false;
    if (name.length() > 100) return false;
    
    // Check for invalid characters
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool PluginUtils::validatePluginVersion(const std::string& version) {
    if (version.empty()) return false;
    
    // Simple version validation (semver format: major.minor.patch)
    size_t major = version.find('.');
    if (major == std::string::npos) return false;
    
    size_t minor = version.find('.', major + 1);
    if (minor == std::string::npos) return false;
    
    return true;
}

std::string PluginUtils::getPluginDirectory() {
    // This should be configured via build system or environment variable
    return "plugins/";
}

std::string PluginUtils::getPluginConfigDirectory() {
    return "plugins/config/";
}

std::string PluginUtils::getPluginDataDirectory() {
    return "plugins/data/";
}

std::string PluginUtils::getLastPluginError() {
    return g_lastError;
}

void PluginUtils::clearLastPluginError() {
    g_lastError.clear();
}

} // namespace nlm
