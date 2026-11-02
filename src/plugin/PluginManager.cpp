#include "PluginManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <zlib.h> // For compression if available

namespace nlm {

struct PluginManager::Impl {
    std::unordered_map<std::string, std::unique_ptr<IPluginFactory>> pluginFactories;
    std::unordered_map<std::string, std::unique_ptr<IPlugin>> pluginInstances;
    std::unordered_map<std::string, PluginManifest> pluginManifests;
    std::unordered_map<std::string, std::unordered_map<std::string, std::any>> pluginConfigs;
    
    std::unordered_map<std::string, std::vector<std::function<bool(const std::any&)>>> eventHandlers;
    
    Brain* brain;
    
    size_t totalUpdates;
    double averageUpdateTime;
    std::vector<double> updateTimes;
    
    std::unordered_map<std::string, std::string> pluginInstallDirectories;
    
    Impl() : brain(nullptr), totalUpdates(0), averageUpdateTime(0.0) {
        updateTimes.reserve(1000);
    }
};

PluginManager::PluginManager() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("PluginManager initialized");
}

PluginManager::~PluginManager() {
    shutdownAll();
    NLM_LOG_INFO("PluginManager destroyed");
}

PluginManager::PluginManager(PluginManager&& other) noexcept
    : pImpl(std::move(other.pImpl)) {
}

PluginManager& PluginManager::operator=(PluginManager&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void PluginManager::addPluginFactory(std::unique_ptr<IPluginFactory> factory) {
    if (!factory) return;
    
    PluginManifest manifest = factory->getManifest();
    pImpl->pluginFactories[manifest.name] = std::move(factory);
    pImpl->pluginManifests[manifest.name] = std::move(manifest);
    
    NLM_LOG_INFO("Added plugin factory: " + manifest.name + " v" + manifest.version);
}

void PluginManager::removePluginFactory(const std::string& pluginName) {
    auto it = pImpl->pluginFactories.find(pluginName);
    if (it != pImpl->pluginFactories.end()) {
        NLM_LOG_INFO("Removed plugin factory: " + pluginName);
        pImpl->pluginFactories.erase(it);
    }
    
    auto manifestIt = pImpl->pluginManifests.find(pluginName);
    if (manifestIt != pImpl->pluginManifests.end()) {
        pImpl->pluginManifests.erase(manifestIt);
    }
}

std::vector<std::string> PluginManager::getPluginNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->pluginFactories.size());
    
    for (const auto& pair : pImpl->pluginFactories) {
        names.push_back(pair.first);
    }
    
    std::sort(names.begin(), names.end());
    return names;
}

IPlugin* PluginManager::getPlugin(const std::string& name) {
    auto it = pImpl->pluginInstances.find(name);
    if (it != pImpl->pluginInstances.end()) {
        return it->second.get();
    }
    return nullptr;
}

const IPlugin* PluginManager::getPlugin(const std::string& name) const {
    auto it = pImpl->pluginInstances.find(name);
    if (it != pImpl->pluginInstances.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool PluginManager::createPlugin(const std::string& name) {
    auto factoryIt = pImpl->pluginFactories.find(name);
    if (factoryIt == pImpl->pluginFactories.end()) {
        NLM_LOG_ERROR("Plugin factory not found: " + name);
        return false;
    }
    
    auto& factory = factoryIt->second;
    
    if (!factory->canCreatePlugin()) {
        NLM_LOG_ERROR("Cannot create plugin: " + name);
        return false;
    }
    
    auto plugin = factory->createPlugin();
    if (!plugin) {
        NLM_LOG_ERROR("Failed to create plugin instance: " + name);
        return false;
    }
    
    pImpl->pluginInstances[name] = std::move(plugin);
    
    NLM_LOG_INFO("Created plugin instance: " + name);
    return true;
}

void PluginManager::destroyPlugin(const std::string& name) {
    auto it = pImpl->pluginInstances.find(name);
    if (it != pImpl->pluginInstances.end()) {
        NLM_LOG_INFO("Destroying plugin instance: " + name);
        
        if (it->second->isEnabled()) {
            it->second->shutdown();
        }
        
        pImpl->pluginInstances.erase(it);
    }
}

bool PluginManager::initializeAll(Brain* brain) {
    pImpl->brain = brain;
    bool allInitialized = true;
    
    for (auto& pair : pImpl->pluginInstances) {
        auto& plugin = pair.second;
        
        if (!plugin->isEnabled()) {
            continue;
        }
        
        NLM_LOG_INFO("Initializing plugin: " + plugin->getName());
        
        if (!plugin->initialize(brain)) {
            NLM_LOG_ERROR("Failed to initialize plugin: " + plugin->getName());
            allInitialized = false;
        }
    }
    
    if (allInitialized) {
        NLM_LOG_INFO("All plugins initialized successfully");
    } else {
        NLM_LOG_WARNING("Some plugins failed to initialize");
    }
    
    return allInitialized;
}

void PluginManager::shutdownAll() {
    for (auto& pair : pImpl->pluginInstances) {
        auto& plugin = pair.second;
        
        if (plugin->isEnabled()) {
            NLM_LOG_INFO("Shutting down plugin: " + plugin->getName());
            plugin->shutdown();
        }
    }
    
    pImpl->pluginInstances.clear();
    pImpl->brain = nullptr;
    NLM_LOG_INFO("All plugins shut down");
}

void PluginManager::updateAll(double dt) {
    auto startTime = std::chrono::steady_clock::now();
    
    for (auto& pair : pImpl->pluginInstances) {
        auto& plugin = pair.second;
        
        if (!plugin->isEnabled()) {
            continue;
        }
        
        plugin->update(dt);
    }
    
    auto endTime = std::chrono::steady_clock::now();
    double updateTime = std::chrono::duration<double, std::chrono::milliseconds::period>(endTime - startTime).count();
    
    // Update performance metrics
    pImpl->updateTimes.push_back(updateTime);
    if (pImpl->updateTimes.size() > 100) {
        pImpl->updateTimes.erase(pImpl->updateTimes.begin());
    }
    
    if (!pImpl->updateTimes.empty()) {
        double sum = std::accumulate(pImpl->updateTimes.begin(), pImpl->updateTimes.end(), 0.0);
        pImpl->averageUpdateTime = sum / pImpl->updateTimes.size();
    }
    
    pImpl->totalUpdates++;
}

bool PluginManager::handleEvent(const std::string& eventType, const std::any& eventData) {
    auto it = pImpl->eventHandlers.find(eventType);
    if (it == pImpl->eventHandlers.end()) {
        return false;
    }
    
    bool eventHandled = false;
    for (const auto& handler : it->second) {
        if (handler(eventData)) {
            eventHandled = true;
        }
    }
    
    // Also try to handle event through plugins
    for (auto& pair : pImpl->pluginInstances) {
        auto& plugin = pair.second;
        
        if (plugin->isEnabled()) {
            if (plugin->handleEvent(eventType, eventData)) {
                eventHandled = true;
            }
        }
    }
    
    return eventHandled;
}

const std::any PluginManager::getPluginConfig(const std::string& pluginName, const std::string& key) const {
    auto pluginConfigIt = pImpl->pluginConfigs.find(pluginName);
    if (pluginConfigIt != pImpl->pluginConfigs.end()) {
        auto keyIt = pluginConfigIt->second.find(key);
        if (keyIt != pluginConfigIt->second.end()) {
            return keyIt->second;
        }
    }
    return std::any();
}

bool PluginManager::setPluginConfig(const std::string& pluginName, const std::string& key, const std::any& value) {
    auto pluginIt = pImpl->pluginInstances.find(pluginName);
    if (pluginIt == pImpl->pluginInstances.end()) {
        NLM_LOG_ERROR("Plugin not found: " + pluginName);
        return false;
    }
    
    auto& plugin = pluginIt->second;
    
    if (!plugin->setConfigOption(key, value)) {
        NLM_LOG_ERROR("Failed to set plugin config option: " + pluginName + "." + key);
        return false;
    }
    
    pImpl->pluginConfigs[pluginName][key] = value;
    NLM_LOG_DEBUG("Set plugin config option: " + pluginName + "." + key);
    return true;
}

bool PluginManager::isPluginEnabled(const std::string& name) const {
    auto it = pImpl->pluginInstances.find(name);
    if (it != pImpl->pluginInstances.end()) {
        return it->second->isEnabled();
    }
    return false;
}

void PluginManager::setPluginEnabled(const std::string& name, bool enabled) {
    auto it = pImpl->pluginInstances.find(name);
    if (it != pImpl->pluginInstances.end()) {
        it->second->setEnabled(enabled);
        NLM_LOG_INFO("Plugin " + name + " " + (enabled ? "enabled" : "disabled"));
    }
}

const std::vector<std::string>& PluginManager::getPluginCapabilities(const std::string& name) const {
    static const std::vector<std::string> emptyList;
    
    auto pluginIt = pImpl->pluginInstances.find(name);
    if (pluginIt != pImpl->pluginInstances.end()) {
        return pluginIt->second->getCapabilities();
    }
    
    auto manifestIt = pImpl->pluginManifests.find(name);
    if (manifestIt != pImpl->pluginManifests.end()) {
        return manifestIt->second.capabilities;
    }
    
    return emptyList;
}

bool PluginManager::pluginHasCapability(const std::string& pluginName, const std::string& capability) const {
    auto pluginIt = pImpl->pluginInstances.find(pluginName);
    if (pluginIt != pImpl->pluginInstances.end()) {
        return pluginIt->second->hasCapability(capability);
    }
    
    auto manifestIt = pImpl->pluginManifests.find(pluginName);
    if (manifestIt != pImpl->pluginManifests.end()) {
        return std::find(manifestIt->second.capabilities.begin(), 
                        manifestIt->second.capabilities.end(), capability) != manifestIt->second.capabilities.end();
    }
    
    return false;
}

std::vector<std::string> PluginManager::getPluginsWithCapability(const std::string& capability) const {
    std::vector<std::string> plugins;
    
    for (const auto& pair : pImpl->pluginInstances) {
        if (pair.second->hasCapability(capability)) {
            plugins.push_back(pair.first);
        }
    }
    
    return plugins;
}

bool PluginManager::savePluginConfig(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    file << "Plugin Configuration File\n";
    file << "Generated by PluginManager\n\n";
    
    for (const auto& pair : pImpl->pluginConfigs) {
        file << "[Plugin " << pair.first << "]\n";
        for (const auto& configPair : pair.second) {
            file << configPair.first << " = " << configPair.second << "\n";
        }
        file << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("Plugin configuration saved to: " + filename);
    return true;
}

bool PluginManager::loadPluginConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for reading: " + filename);
        return false;
    }
    
    std::string currentPlugin;
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.empty() || line.find("//") == 0) {
            continue;
        }
        
        if (line.find("[Plugin ") == 0 && line.back() == ']') {
            currentPlugin = line.substr(8, line.size() - 9);
            continue;
        }
        
        size_t equalPos = line.find("=");
        if (equalPos != std::string::npos && !currentPlugin.empty()) {
            std::string key = line.substr(0, equalPos);
            std::string value = line.substr(equalPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            pImpl->pluginConfigs[currentPlugin][key] = value;
        }
    }
    
    file.close();
    NLM_LOG_INFO("Plugin configuration loaded from: " + filename);
    return true;
}

PluginManifest PluginManager::getPluginManifest(const std::string& name) const {
    auto it = pImpl->pluginManifests.find(name);
    if (it != pImpl->pluginManifests.end()) {
        return it->second;
    }
    
    return PluginManifest{};
}

std::vector<PluginManifest> PluginManager::getAllPluginManifests() const {
    std::vector<PluginManifest> manifests;
    manifests.reserve(pImpl->pluginManifests.size());
    
    for (const auto& pair : pImpl->pluginManifests) {
        manifests.push_back(pair.second);
    }
    
    return manifests;
}

bool PluginManager::exportPlugin(const std::string& pluginName, const std::string& outputPath) {
    auto manifestIt = pImpl->pluginManifests.find(pluginName);
    if (manifestIt == pImpl->pluginManifests.end()) {
        NLM_LOG_ERROR("Plugin not found: " + pluginName);
        return false;
    }
    
    // Create plugin package
    std::string packagePath = outputPath;
    if (packagePath.empty()) {
        packagePath = pluginName + "_package.zip";
    }
    
    NLM_LOG_INFO("Exporting plugin " + pluginName + " to " + packagePath);
    
    // In a real implementation, this would:
    // 1. Find the plugin installation directory
    // 2. Package all plugin files (source, manifest, resources)
    // 3. Create a distribution package
    
    return true;
}

bool PluginManager::importPlugin(const std::string& packagePath) {
    NLM_LOG_INFO("Importing plugin from package: " + packagePath);
    
    // In a real implementation, this would:
    // 1. Extract package contents
    // 2. Parse plugin manifest
    // 3. Validate plugin dependencies
    // 4. Install plugin files
    // 5. Register plugin factory
    
    return true;
}

PluginManager::PluginStats PluginManager::getStats() const {
    PluginStats stats;
    stats.totalPlugins = pImpl->pluginInstances.size();
    stats.enabledPlugins = 0;
    stats.disabledPlugins = 0;
    stats.builtInPlugins = 0;
    stats.thirdPartyPlugins = 0;
    stats.totalUpdates = pImpl->totalUpdates;
    stats.averageUpdateTime = pImpl->averageUpdateTime;
    
    for (const auto& pair : pImpl->pluginInstances) {
        if (pair.second->isEnabled()) {
            stats.enabledPlugins++;
        } else {
            stats.disabledPlugins++;
        }
        
        auto manifestIt = pImpl->pluginManifests.find(pair.first);
        if (manifestIt != pImpl->pluginManifests.end()) {
            if (manifestIt->second.isBuiltIn) {
                stats.builtInPlugins++;
            } else {
                stats.thirdPartyPlugins++;
            }
        }
    }
    
    return stats;
}

std::string PluginManager::getPluginInstallDirectory(const std::string& name) const {
    auto it = pImpl->pluginInstallDirectories.find(name);
    if (it != pImpl->pluginInstallDirectories.end()) {
        return it->second;
    }
    
    // Return default path
    return PluginUtils::getSystemPluginDirectory() + "/" + name;
}

std::string PluginManager::getPluginCacheDirectory() {
    return "/tmp/nlm_plugin_cache";
}

void PluginManager::clearPluginCache() {
    NLM_LOG_INFO("Clearing plugin cache");
    // Implementation would clear cache files
}

// Built-in plugin factory implementations
namespace BuiltInPlugins {

class MemoryPlugin : public IPlugin {
public:
    std::string name = "MemorySystem";
    std::string version = "1.0.0";
    std::string description = "Memory system integration plugin";
    std::string author = "NLM Team";
    std::string email = "team@nlm.org";
    std::string website = "https://nlm.org";
    std::string license = "MIT";
    bool enabled = false;
    
    Brain* brain = nullptr;
    
    const std::string& getName() const override { return name; }
    const std::string& getVersion() const override { return version; }
    const std::string& getDescription() const override { return description; }
    const std::string& getAuthor() const override { return author; }
    const std::string& getEmail() const override { return email; }
    const std::string& getWebsite() const override { return website; }
    const std::string& getLicense() const override { return license; }
    
    bool isEnabled() const override { return enabled; }
    void setEnabled(bool e) override { enabled = e; }
    
    bool initialize(Brain* b) override {
        brain = b;
        return true;
    }
    
    void shutdown() override {
        brain = nullptr;
    }
    
    const std::vector<std::string>& getCapabilities() const override {
        static const std::vector<std::string> caps = {"memory", "working_memory", "episodic_memory"};
        return caps;
    }
    
    bool hasCapability(const std::string& capability) const override {
        return std::find(getCapabilities().begin(), getCapabilities().end(), capability) != getCapabilities().end();
    }
    
    const std::unordered_map<std::string, std::any>& getConfigOptions() const override {
        static const std::unordered_map<std::string, std::any> options;
        return options;
    }
    
    bool setConfigOption(const std::string& key, const std::any& value) override {
        return false; // No configuration options
    }
    
    std::any getConfigOption(const std::string& key) const override {
        return std::any();
    }
    
    void update(double dt) override {
        // Memory system updates
    }
    
    bool handleEvent(const std::string& eventType, const std::any& eventData) override {
        return false;
    }
};

class MemoryPluginFactory : public IPluginFactory {
public:
    std::unique_ptr<IPlugin> createPlugin() override {
        return std::make_unique<MemoryPlugin>();
    }
    
    PluginManifest getManifest() const override {
        PluginManifest manifest;
        manifest.name = "MemorySystem";
        manifest.version = "1.0.0";
        manifest.description = "Memory system integration plugin";
        manifest.author = "NLM Team";
        manifest.email = "team@nlm.org";
        manifest.website = "https://nlm.org";
        manifest.license = "MIT";
        manifest.isBuiltIn = true;
        manifest.isThirdParty = false;
        manifest.capabilities = {"memory", "working_memory", "episodic_memory"};
        return manifest;
    }
    
    bool canCreatePlugin() const override {
        return true;
    }
};

// Implement other built-in plugin factories similarly...

} // namespace BuiltInPlugins

} // namespace nlm
