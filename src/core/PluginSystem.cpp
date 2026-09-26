#include "PluginSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace nlm {

struct PluginRegistry::Impl {
    // Plugin management
    std::unordered_map<std::string, std::shared_ptr<IPlugin>> plugins;
    std::unordered_map<std::string, std::function<std::shared_ptr<IPlugin>()>> factories;
    std::unordered_map<std::string, PluginMetadata> pluginMetadata;
    std::unordered_map<std::string, PluginStatus> pluginStatus;
    std::unordered_map<std::string, std::vector<std::shared_ptr<IExtension>>> extensions;
    std::unordered_map<std::string, std::any> services;
    
    // Thread safety
    mutable std::mutex mutex;
    
    // Statistics
    size_t loadCount = 0;
    size_t unloadCount = 0;
    size_t errorCount = 0;
};

PluginRegistry::PluginRegistry() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("PluginRegistry initialized");
}

PluginRegistry::~PluginRegistry() {
    NLM_LOG_INFO("PluginRegistry shutdown");
}

bool PluginRegistry::loadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_ERROR("Plugin not found: " + pluginName);
        return false;
    }
    
    auto plugin = it->second;
    if (!plugin) {
        NLM_LOG_ERROR("Plugin is null: " + pluginName);
        return false;
    }
    
    if (pImpl->pluginStatus[pluginName] == PluginStatus::Ready) {
        NLM_LOG_INFO("Plugin already loaded: " + pluginName);
        return true;
    }
    
    NLM_LOG_INFO("Loading plugin: " + pluginName);
    
    try {
        if (!plugin->load()) {
            NLM_LOG_ERROR("Failed to load plugin: " + pluginName);
            pImpl->pluginStatus[pluginName] = PluginStatus::Error;
            pImpl->errorCount++;
            return false;
        }
        
        if (!plugin->initialize()) {
            NLM_LOG_ERROR("Failed to initialize plugin: " + pluginName);
            plugin->unload();
            pImpl->pluginStatus[pluginName] = PluginStatus::Error;
            pImpl->errorCount++;
            return false;
        }
        
        pImpl->pluginStatus[pluginName] = PluginStatus::Ready;
        pImpl->loadCount++;
        
        NLM_LOG_INFO("Plugin loaded successfully: " + pluginName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading plugin " + pluginName + ": " + std::string(e.what()));
        pImpl->pluginStatus[pluginName] = PluginStatus::Error;
        pImpl->errorCount++;
        return false;
    }
}

bool PluginRegistry::unloadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_ERROR("Plugin not found: " + pluginName);
        return false;
    }
    
    auto plugin = it->second;
    if (!plugin) {
        NLM_LOG_ERROR("Plugin is null: " + pluginName);
        return false;
    }
    
    NLM_LOG_INFO("Unloading plugin: " + pluginName);
    
    try {
        plugin->shutdown();
        if (!plugin->unload()) {
            NLM_LOG_WARNING("Plugin unload returned false: " + pluginName);
        }
        
        // Clean up extensions provided by this plugin
        for (auto& extIt : pImpl->extensions) {
            auto& extensions = extIt.second;
            extensions.erase(std::remove_if(extensions.begin(), extensions.end(),
                [&pluginName](const std::shared_ptr<IExtension>& ext) {
                    return ext->getExtensionPoint().find(pluginName + "::") == 0;
                }),
                extensions.end());
        }
        
        // Clean up services provided by this plugin
        pImpl->services.erase(pluginName + "::services");
        
        pImpl->pluginStatus[pluginName] = PluginStatus::Unloaded;
        pImpl->unloadCount++;
        
        NLM_LOG_INFO("Plugin unloaded successfully: " + pluginName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception unloading plugin " + pluginName + ": " + std::string(e.what()));
        return false;
    }
}

bool PluginRegistry::enablePlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_ERROR("Plugin not found: " + pluginName);
        return false;
    }
    
    pImpl->pluginMetadata[pluginName].enabled = true;
    
    // If plugin is already loaded, we need to re-initialize it
    if (pImpl->pluginStatus[pluginName] == PluginStatus::Ready) {
        unloadPlugin(pluginName);
        loadPlugin(pluginName);
    }
    
    NLM_LOG_INFO("Plugin enabled: " + pluginName);
    return true;
}

bool PluginRegistry::disablePlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_ERROR("Plugin not found: " + pluginName);
        return false;
    }
    
    // If plugin is loaded, unload it first
    if (pImpl->pluginStatus[pluginName] == PluginStatus::Ready) {
        unloadPlugin(pluginName);
    }
    
    pImpl->pluginMetadata[pluginName].enabled = false;
    
    NLM_LOG_INFO("Plugin disabled: " + pluginName);
    return true;
}

bool PluginRegistry::registerPlugin(std::shared_ptr<IPlugin> plugin) {
    if (!plugin) {
        NLM_LOG_ERROR("Cannot register null plugin");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    const auto& metadata = plugin->getMetadata();
    
    // Check if plugin with same name already exists
    if (pImpl->plugins.find(metadata.name) != pImpl->plugins.end()) {
        NLM_LOG_ERROR("Plugin already registered: " + metadata.name);
        return false;
    }
    
    pImpl->plugins[metadata.name] = plugin;
    pImpl->pluginMetadata[metadata.name] = metadata;
    pImpl->pluginStatus[metadata.name] = PluginStatus::Unknown;
    
    NLM_LOG_INFO("Plugin registered: " + metadata.name + " (v" + metadata.version + ")");
    return true;
}

bool PluginRegistry::unregisterPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        NLM_LOG_ERROR("Plugin not found: " + pluginName);
        return false;
    }
    
    // If plugin is loaded, unload it first
    if (pImpl->pluginStatus[pluginName] == PluginStatus::Ready) {
        unloadPlugin(pluginName);
    }
    
    pImpl->plugins.erase(it);
    pImpl->pluginMetadata.erase(pluginName);
    pImpl->pluginStatus.erase(pluginName);
    
    // Clean up extensions provided by this plugin
    for (auto& extIt : pImpl->extensions) {
        auto& extensions = extIt.second;
        extensions.erase(std::remove_if(extensions.begin(), extensions.end(),
            [&pluginName](const std::shared_ptr<IExtension>& ext) {
                return ext->getExtensionPoint().find(pluginName + "::") == 0;
            }),
            extensions.end());
    }
    
    NLM_LOG_INFO("Plugin unregistered: " + pluginName);
    return true;
}

std::vector<std::string> PluginRegistry::getPluginNames() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    names.reserve(pImpl->plugins.size());
    for (const auto& pair : pImpl->plugins) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> PluginRegistry::getPluginNames(const std::string& category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    for (const auto& pair : pImpl->pluginMetadata) {
        if (pair.second.category == category) {
            names.push_back(pair.first);
        }
    }
    return names;
}

PluginMetadata PluginRegistry::getPluginMetadata(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->pluginMetadata.find(pluginName);
    if (it == pImpl->pluginMetadata.end()) {
        NLM_LOG_ERROR("Plugin metadata not found: " + pluginName);
        return PluginMetadata();
    }
    
    return it->second;
}

PluginStatus PluginRegistry::getPluginStatus(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->pluginStatus.find(pluginName);
    if (it == pImpl->pluginStatus.end()) {
        return PluginStatus::Unknown;
    }
    
    return it->second;
}

bool PluginRegistry::registerExtension(const std::string& extensionPointName, std::shared_ptr<IExtension> extension) {
    if (!extension) {
        NLM_LOG_ERROR("Cannot register null extension");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->extensions[extensionPointName].push_back(extension);
    NLM_LOG_INFO("Extension registered for point: " + extensionPointName);
    return true;
}

std::vector<std::shared_ptr<IExtension>> PluginRegistry::getExtensions(const std::string& extensionPointName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->extensions.find(extensionPointName);
    if (it == pImpl->extensions.end()) {
        return {};
    }
    
    return it->second;
}

std::vector<std::string> PluginRegistry::getExtensionPoints() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> points;
    points.reserve(pImpl->extensions.size());
    for (const auto& pair : pImpl->extensions) {
        points.push_back(pair.first);
    }
    return points;
}

bool PluginRegistry::registerService(const std::string& serviceName, std::any service) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->services[serviceName] = service;
    NLM_LOG_INFO("Service registered: " + serviceName);
    return true;
}

std::any PluginRegistry::getService(const std::string& serviceName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->services.find(serviceName);
    if (it == pImpl->services.end()) {
        NLM_LOG_ERROR("Service not found: " + serviceName);
        return std::any();
    }
    
    return it->second;
}

bool PluginRegistry::hasService(const std::string& serviceName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->services.find(serviceName) != pImpl->services.end();
}

bool PluginRegistry::registerFactory(const std::string& pluginName, std::function<std::shared_ptr<IPlugin>()> factory) {
    if (!factory) {
        NLM_LOG_ERROR("Cannot register null factory for plugin: " + pluginName);
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->factories[pluginName] = factory;
    NLM_LOG_INFO("Factory registered for plugin: " + pluginName);
    return true;
}

std::shared_ptr<IPlugin> PluginRegistry::createPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->factories.find(pluginName);
    if (it == pImpl->factories.end()) {
        NLM_LOG_ERROR("Factory not found for plugin: " + pluginName);
        return nullptr;
    }
    
    try {
        auto plugin = it->second();
        if (!plugin) {
            NLM_LOG_ERROR("Factory returned null for plugin: " + pluginName);
            return nullptr;
        }
        
        // Register the plugin
        if (!registerPlugin(plugin)) {
            NLM_LOG_ERROR("Failed to register created plugin: " + pluginName);
            return nullptr;
        }
        
        return plugin;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception creating plugin " + pluginName + ": " + std::string(e.what()));
        return nullptr;
    }
}

PluginRegistry& PluginRegistry::getInstance() {
    static PluginRegistry instance;
    return instance;
}

void PluginRegistry::reset() {
    static PluginRegistry instance;
    // Clear all plugins
    for (const auto& pair : instance.pImpl->plugins) {
        instance.unregisterPlugin(pair.first);
    }
    instance.pImpl->plugins.clear();
    instance.pImpl->pluginMetadata.clear();
    instance.pImpl->pluginStatus.clear();
    instance.pImpl->extensions.clear();
    instance.pImpl->services.clear();
    instance.pImpl->factories.clear();
    instance.pImpl->loadCount = 0;
    instance.pImpl->unloadCount = 0;
    instance.pImpl->errorCount = 0;
}

struct PluginSystem::Impl {
    std::unique_ptr<IPluginRegistry> registry;
    std::string configFile;
    bool initialized = false;
    std::mutex mutex;
};

PluginSystem::PluginSystem() : registry_(PluginRegistry::getInstance()) {
    pImpl = std::make_unique<Impl>();
    pImpl->registry = &registry_;
    NLM_LOG_INFO("PluginSystem created");
}

PluginSystem::~PluginSystem() {
    shutdown();
    NLM_LOG_INFO("PluginSystem destroyed");
}

bool PluginSystem::initialize() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->initialized) {
        NLM_LOG_INFO("PluginSystem already initialized");
        return true;
    }
    
    NLM_LOG_INFO("Initializing PluginSystem");
    
    try {
        // Initialize core plugins here
        // For now, just set initialized flag
        pImpl->initialized = true;
        
        NLM_LOG_INFO("PluginSystem initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception initializing PluginSystem: " + std::string(e.what()));
        return false;
    }
}

void PluginSystem::shutdown() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->initialized) {
        return;
    }
    
    NLM_LOG_INFO("Shutting down PluginSystem");
    
    try {
        // Unload all plugins
        auto pluginNames = registry_.getPluginNames();
        for (const auto& name : pluginNames) {
            registry_.unloadPlugin(name);
        }
        
        pImpl->initialized = false;
        NLM_LOG_INFO("PluginSystem shutdown complete");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception shutting down PluginSystem: " + std::string(e.what()));
    }
}

bool PluginSystem::loadPluginsFromConfig(const std::string& configFile) {
    pImpl->configFile = configFile;
    
    // TODO: Implement actual plugin loading from config file
    // For now, just log a placeholder
    NLM_LOG_INFO("Loading plugins from config file: " + configFile);
    return true;
}

bool PluginSystem::savePluginConfig(const std::string& configFile) const {
    // TODO: Implement actual plugin config saving
    NLM_LOG_INFO("Saving plugin config to file: " + configFile);
    return true;
}

std::string PluginSystem::getSystemStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream ss;
    ss << "PluginSystem Status:" << std::endl;
    ss << "  Initialized: " << (pImpl->initialized ? "Yes" : "No") << std::endl;
    ss << "  Registered Plugins: " << registry_.getPluginNames().size() << std::endl;
    ss << "  Extension Points: " << registry_.getExtensionPoints().size() << std::endl;
    
    auto pluginNames = registry_.getPluginNames();
    for (const auto& name : pluginNames) {
        auto status = registry_.getPluginStatus(name);
        ss << "  " << name << ": " << static_cast<int>(status) << std::endl;
    }
    
    return ss.str();
}

PluginSystem& PluginSystem::getInstance() {
    static PluginSystem instance;
    return instance;
}

void PluginSystem::reset() {
    static PluginSystem instance;
    instance.shutdown();
}

} // namespace nlm