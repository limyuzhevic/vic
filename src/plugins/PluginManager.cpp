#include "PluginManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <dlfcn.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

namespace nlm {

struct PluginManager::Impl {
    // Plugin metadata storage
    struct LoadedPlugin {
        std::string name;
        std::string path;
        std::shared_ptr<IPlugin> plugin;
        PluginMetadata metadata;
        bool enabled;
        std::chrono::system_clock::time_point loadTime;
        std::chrono::system_clock::duration totalUpdateTime;
        size_t updateCount;
    };
    
    std::unordered_map<std::string, LoadedPlugin> plugins;
    std::mutex mutex;
    
    Config config;
    Logger* logger;
    
    // Plugin factory functions
    using PluginFactory = std::function<std::shared_ptr<IPlugin>()>;
    std::unordered_map<std::string, PluginFactory> pluginFactories;
    
    // Plugin loading utilities
    bool loadLibraryPlugin(const std::string& pluginPath);
    bool loadCppPlugin(const std::string& sourcePath);
    bool extractPluginInfo(void* handle, PluginMetadata& metadata);
    std::shared_ptr<IPlugin> createPluginFromFactory(const std::string& factoryName);
    
    // Plugin validation
    bool validatePlugin(const PluginMetadata& metadata) const;
    bool checkDependencies(const PluginMetadata& metadata) const;
    
    // Plugin statistics
    void updatePluginStats(const std::string& pluginName, std::chrono::system_clock::duration updateTime);
    std::string generatePluginStats() const;
};

PluginManager::PluginManager() : pImpl(std::make_unique<Impl>()) {
    pImpl->logger = Logger::getInstance();
}

PluginManager::~PluginManager() {
    shutdownAllPlugins();
}

PluginManager::PluginManager(PluginManager&& other) noexcept : pImpl(std::move(other.pImpl)) {}

PluginManager& PluginManager::operator=(PluginManager&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::string pluginName = std::filesystem::path(pluginPath).stem().string();
    
    if (isPluginLoaded(pluginName)) {
        pImpl->logger->warning("Plugin already loaded: " + pluginName);
        return false;
    }
    
    bool success = pImpl->loadLibraryPlugin(pluginPath);
    if (success) {
        pImpl->logger->info("Plugin loaded successfully: " + pluginName);
    } else {
        pImpl->logger->error("Failed to load plugin: " + pluginName);
    }
    
    return success;
}

bool PluginManager::loadPluginFromDirectory(const std::string& directoryPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> pluginFiles = discoverPlugins(directoryPath);
    bool success = true;
    
    for (const auto& pluginFile : pluginFiles) {
        if (!loadPlugin(pluginFile)) {
            success = false;
        }
    }
    
    return success;
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        pImpl->logger->warning("Plugin not found: " + pluginName);
        return false;
    }
    
    it->second.plugin->shutdown();
    pImpl->plugins.erase(it);
    pImpl->logger->info("Plugin unloaded: " + pluginName);
    return true;
}

bool PluginManager::isPluginLoaded(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->plugins.find(pluginName) != pImpl->plugins.end();
}

std::vector<std::string> PluginManager::getLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> pluginNames;
    pluginNames.reserve(pImpl->plugins.size());
    for (const auto& pair : pImpl->plugins) {
        pluginNames.push_back(pair.first);
    }
    return pluginNames;
}

std::vector<std::string> PluginManager::discoverPlugins(const std::string& directory) const {
    std::vector<std::string> plugins;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string extension = entry.path().extension().string();
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                
                if (extension == ".so" || extension == ".dylib" || extension == ".dll") {
                    plugins.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        pImpl->logger->error("Failed to discover plugins: " + std::string(e.what()));
    }
    
    return plugins;
}

PluginMetadata PluginManager::getPluginMetadata(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(pluginName);
    if (it != pImpl->plugins.end()) {
        return it->second.metadata;
    }
    
    return PluginMetadata();
}

std::shared_ptr<INeuronModelPlugin> PluginManager::getNeuronModelPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end() && it->second.enabled) {
        return std::dynamic_pointer_cast<INeuronModelPlugin>(it->second.plugin);
    }
    
    return nullptr;
}

std::shared_ptr<IPlasticityRulePlugin> PluginManager::getPlasticityRulePlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end() && it->second.enabled) {
        return std::dynamic_pointer_cast<IPlasticityRulePlugin>(it->second.plugin);
    }
    
    return nullptr;
}

std::shared_ptr<ISynapticDynamicsPlugin> PluginManager::getSynapticDynamicsPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end() && it->second.enabled) {
        return std::dynamic_pointer_cast<ISynapticDynamicsPlugin>(it->second.plugin);
    }
    
    return nullptr;
}

std::shared_ptr<IPlugin> PluginManager::getPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end() && it->second.enabled) {
        return it->second.plugin;
    }
    
    return nullptr;
}

void PluginManager::setConfig(const Config& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config = config;
}

void PluginManager::update(double timestep) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto startTime = std::chrono::system_clock::now();
    
    for (auto& pair : pImpl->plugins) {
        if (pair.second.enabled) {
            pair.second.plugin->update(timestep);
        }
    }
    
    auto endTime = std::chrono::system_clock::now();
    auto updateTime = endTime - startTime;
    
    for (auto& pair : pImpl->plugins) {
        if (pair.second.enabled) {
            pImpl->updatePluginStats(pair.first, updateTime);
        }
    }
}

void PluginManager::shutdownAllPlugins() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (auto& pair : pImpl->plugins) {
        pair.second.plugin->shutdown();
    }
    pImpl->plugins.clear();
}

void PluginManager::enablePlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end()) {
        it->second.enabled = true;
        pImpl->logger->info("Plugin enabled: " + name);
    }
}

void PluginManager::disablePlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    if (it != pImpl->plugins.end()) {
        it->second.enabled = false;
        pImpl->logger->info("Plugin disabled: " + name);
    }
}

bool PluginManager::isPluginEnabled(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(name);
    return it != pImpl->plugins.end() && it->second.enabled;
}

size_t PluginManager::getLoadedPluginCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->plugins.size();
}

std::string PluginManager::getPluginStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->generatePluginStats();
}

bool PluginManager::Impl::loadLibraryPlugin(const std::string& pluginPath) {
    void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!handle) {
        logger->error("Failed to load library " + pluginPath + ": " + std::string(dlerror()));
        return false;
    }
    
    bool success = extractPluginInfo(handle, metadata);
    if (!success) {
        dlclose(handle);
        return false;
    }
    
    auto factoryIt = pluginFactories.find(metadata.name);
    if (factoryIt != pluginFactories.end()) {
        auto plugin = factoryIt->second();
        if (!plugin) {
            dlclose(handle);
            logger->error("Failed to create plugin instance: " + metadata.name);
            return false;
        }
        
        plugins[metadata.name] = {metadata.name, pluginPath, plugin, metadata, true, std::chrono::system_clock::now(), std::chrono::system_clock::duration(0), 0};
    } else {
        dlclose(handle);
        logger->error("Plugin factory not found for: " + metadata.name);
        return false;
    }
    
    return validatePlugin(metadata) && checkDependencies(metadata);
}

bool PluginManager::Impl::loadCppPlugin(const std::string& sourcePath) {
    // This would require compiling the C++ source at runtime
    // For now, we'll just log that this feature is available
    logger->info("CPP plugin loading from source: " + sourcePath);
    return true;
}

bool PluginManager::Impl::extractPluginInfo(void* handle, PluginMetadata& metadata) {
    // Get the plugin name symbol
    using GetPluginMetadataFunc = PluginMetadata (*)();
    auto getPluginInfo = (GetPluginMetadataFunc)dlsym(handle, "getPluginMetadata");
    
    if (!getPluginInfo) {
        // Try alternative symbol names
        getPluginInfo = (GetPluginMetadataFunc)dlsym(handle, "getPluginInfo");
        if (!getPluginInfo) {
            logger->error("Plugin metadata function not found");
            return false;
        }
    }
    
    metadata = getPluginInfo();
    return true;
}

std::shared_ptr<IPlugin> PluginManager::Impl::createPluginFromFactory(const std::string& factoryName) {
    auto it = pluginFactories.find(factoryName);
    if (it != pluginFactories.end()) {
        return it->second();
    }
    return nullptr;
}

bool PluginManager::Impl::validatePlugin(const PluginMetadata& metadata) const {
    if (metadata.name.empty() || metadata.version.empty()) {
        logger->error("Invalid plugin metadata: missing name or version");
        return false;
    }
    
    // Check for duplicate names
    for (const auto& pair : plugins) {
        if (pair.first == metadata.name) {
            logger->error("Plugin with name already exists: " + metadata.name);
            return false;
        }
    }
    
    return true;
}

bool PluginManager::Impl::checkDependencies(const PluginMetadata& metadata) const {
    for (const auto& dependency : metadata.dependencies) {
        if (plugins.find(dependency) == plugins.end()) {
            logger->error("Plugin dependency not found: " + dependency + " (required by " + metadata.name + ")");
            return false;
        }
    }
    return true;
}

void PluginManager::Impl::updatePluginStats(const std::string& pluginName, std::chrono::system_clock::duration updateTime) {
    auto it = plugins.find(pluginName);
    if (it != plugins.end()) {
        it->second.totalUpdateTime += updateTime;
        it->second.updateCount++;
    }
}

std::string PluginManager::Impl::generatePluginStats() const {
    std::stringstream ss;
    ss << "Plugin Statistics:\n";
    ss << "===================\n";
    
    for (const auto& pair : plugins) {
        const auto& plugin = pair.second;
        double avgUpdateTime = plugin.updateCount > 0 
            ? std::chrono::duration<double>(plugin.totalUpdateTime).count() / plugin.updateCount 
            : 0.0;
        
        ss << "Plugin: " << plugin.name << "\n";
        ss << "  Version: " << plugin.metadata.version << "\n";
        ss << "  Enabled: " << (plugin.enabled ? "yes" : "no") << "\n";
        ss << "  Load time: " << std::chrono::duration<double>(plugin.loadTime.time_since_epoch()).count() << "s ago\n";
        ss << "  Avg update time: " << avgUpdateTime << "s\n";
        ss << "  Update count: " << plugin.updateCount << "\n";
        ss << "\n";
    }
    
    return ss.str();
}

} // namespace nlm
