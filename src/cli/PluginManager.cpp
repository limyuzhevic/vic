// Plugin Management System Implementation

#include "PluginManager.hpp"
#include "NLMSession.hpp"
#include <dlfcn.h>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <thread>

namespace nlm {
namespace cli {

struct PluginManager::Impl {
    std::unordered_map<std::string, std::shared_ptr<Plugin>> plugins;
    std::unordered_map<std::string, void*> pluginHandles;
    std::unordered_map<std::string, std::string> pluginPaths;
    std::unordered_map<std::string, json> pluginMetadata;
    std::unordered_map<std::string, std::vector<std::string>> pluginDependencies;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> loadTimes;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> unloadTimes;
    bool shuttingDown = false;
    std::mutex mutex;
};

PluginManager::PluginManager(std::shared_ptr<Config> config, std::shared_ptr<Logger> logger)
    : pImpl(new Impl()), config_(config), logger_(logger) {
    if (logger_) {
        NLM_LOG_INFO("PluginManager initialized");
    }
}

PluginManager::~PluginManager() {
    shuttingDownPlugins(this);
    delete pImpl;
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->shuttingDown) return false;
    
    std::string pluginName = std::filesystem::path(pluginPath).stem().string();
    
    if (isPluginLoaded(pluginName)) {
        if (logger_) {
            NLM_LOG_WARNING("Plugin already loaded: " + pluginName);
        }
        return true;
    }
    
    void* handle = dlopen(pluginPath.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!handle) {
        if (logger_) {
            NLM_LOG_ERROR("Failed to load plugin " + pluginName + ": " + std::string(dlerror()));
        }
        return false;
    }
    
    // Create plugin instance
    using CreatePluginFunc = std::shared_ptr<Plugin>(*)();
    auto createPlugin = reinterpret_cast<CreatePluginFunc>(dlsym(handle, "createPlugin"));
    if (!createPlugin) {
        dlclose(handle);
        if (logger_) {
            NLM_LOG_ERROR("Plugin " + pluginName + " does not export createPlugin function");
        }
        return false;
    }
    
    auto plugin = createPlugin();
    if (!plugin) {
        dlclose(handle);
        if (logger_) {
            NLM_LOG_ERROR("Failed to create plugin instance for " + pluginName);
        }
        return false;
    }
    
    // Validate plugin
    if (!validatePluginDependencies(plugin->getName())) {
        dlclose(handle);
        if (logger_) {
            NLM_LOG_ERROR("Plugin dependency validation failed for " + pluginName);
        }
        return false;
    }
    
    // Initialize plugin
    if (!plugin->initialize(shared_from_this(), config_)) {
        dlclose(handle);
        if (logger_) {
            NLM_LOG_ERROR("Plugin initialization failed for " + pluginName);
        }
        return false;
    }
    
    // Store plugin
    pImpl->plugins[pluginName] = plugin;
    pImpl->pluginHandles[pluginName] = handle;
    pImpl->pluginPaths[pluginName] = pluginPath;
    pImpl->loadTimes[pluginName] = std::chrono::system_clock::now();
    
    if (logger_) {
        NLM_LOG_INFO("Plugin loaded successfully: " + pluginName + " v" + plugin->getVersion());
    }
    
    return true;
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!isPluginLoaded(pluginName)) {
        if (logger_) {
            NLM_LOG_WARNING("Plugin not loaded: " + pluginName);
        }
        return false;
    }
    
    auto plugin = pImpl->plugins[pluginName];
    
    // Shutdown plugin
    plugin->shutdown();
    
    // Unload library
    void* handle = pImpl->pluginHandles[pluginName];
    if (dlclose(handle) != 0) {
        if (logger_) {
            NLM_LOG_ERROR("Error closing plugin library for " + pluginName + ": " + std::string(dlerror()));
        }
    }
    
    // Clean up
    pImpl->plugins.erase(pluginName);
    pImpl->pluginHandles.erase(pluginName);
    pImpl->pluginPaths.erase(pluginName);
    pImpl->loadTimes.erase(pluginName);
    
    if (logger_) {
        NLM_LOG_INFO("Plugin unloaded successfully: " + pluginName);
    }
    
    return true;
}

bool PluginManager::reloadPlugin(const std::string& pluginName) {
    if (!isPluginLoaded(pluginName)) {
        if (logger_) {
            NLM_LOG_WARNING("Plugin not loaded: " + pluginName);
        }
        return false;
    }
    
    auto pluginPath = pImpl->pluginPaths[pluginName];
    
    // Unload current
    if (!unloadPlugin(pluginName)) {
        if (logger_) {
            NLM_LOG_ERROR("Failed to unload plugin during reload: " + pluginName);
        }
        return false;
    }
    
    // Reload
    return loadPlugin(pluginPath);
}

bool PluginManager::isPluginLoaded(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->plugins.find(pluginName) != pImpl->plugins.end();
}

const std::vector<std::string>& PluginManager::getLoadedPlugins() const {
    static std::vector<std::string> loadedPlugins;
    loadedPlugins.clear();
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    loadedPlugins.reserve(pImpl->plugins.size());
    for (const auto& pair : pImpl->plugins) {
        loadedPlugins.push_back(pair.first);
    }
    return loadedPlugins;
}

std::shared_ptr<Plugin> PluginManager::getPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->plugins.find(pluginName);
    if (it != pImpl->plugins.end()) {
        return it->second;
    }
    return nullptr;
}

bool PluginManager::executePluginFunction(const std::string& pluginName, 
                                         const std::string& functionName,
                                         const std::vector<std::string>& args) {
    auto plugin = getPlugin(pluginName);
    if (!plugin) {
        if (logger_) {
            NLM_LOG_ERROR("Plugin not found: " + pluginName);
        }
        return false;
    }
    
    return plugin->onPluginMessage(functionName, args);
}

std::string PluginManager::getPluginInfo(const std::string& pluginName) const {
    auto it = pImpl->plugins.find(pluginName);
    if (it == pImpl->plugins.end()) {
        return "Plugin not found: " + pluginName;
    }
    
    auto plugin = it->second;
    json info;
    info["name"] = plugin->getName();
    info["version"] = plugin->getVersion();
    info["author"] = plugin->getAuthor();
    info["description"] = plugin->getDescription();
    info["loaded"] = true;
    
    if (pImpl->loadTimes.find(pluginName) != pImpl->loadTimes.end()) {
        auto loadTime = pImpl->loadTimes.at(pluginName);
        auto loadTimeStr = std::chrono::system_clock::to_time_t(loadTime);
        info["load_time"] = std::string(std::ctime(&loadTimeStr));
    }
    
    std::stringstream ss;
    ss << info.dump(2);
    return ss.str();
}

std::string PluginManager::listAvailablePlugins() const {
    std::string result;
    std::ifstream file(pluginDirectory_ + "/plugin_index.json");
    if (!file.is_open()) {
        return "No plugin index found at: " + pluginDirectory_;
    }
    
    json index;
    file >> index;
    
    result = "Available plugins:\n";
    for (const auto& [name, info] : index.items()) {
        result += "  " + name + ": " + info["description"].get<std::string>() + "\n";
    }
    
    return result;
}

bool PluginManager::validatePluginDependencies(const std::string& pluginName) const {
    // This would check if required plugins are loaded
    // For now, it's a placeholder
    return true;
}

void PluginManager::logStatus() const {
    if (!logger_) return;
    
    NLM_LOG_INFO("=== Plugin Manager Status ===");
    NLM_LOG_INFO("Plugins loaded: " + std::to_string(pImpl->plugins.size()));
    for (const auto& pair : pImpl->plugins) {
        NLM_LOG_INFO("  - " + pair.first);
    }
}

bool PluginManager::initializePlugins(const std::vector<std::string>& pluginNames, 
                                     std::shared_ptr<PluginManager> manager) {
    bool success = true;
    for (const auto& name : pluginNames) {
        if (!manager->loadPlugin("./plugins/" + name + ".so")) {
            if (logger_) {
                NLM_LOG_ERROR("Failed to initialize plugin: " + name);
            }
            success = false;
        }
    }
    return success;
}

bool PluginManager::shutdownPlugins(std::shared_ptr<PluginManager> manager) {
    if (!manager) return true;
    
    auto plugins = manager->getLoadedPlugins();
    bool success = true;
    for (const auto& name : plugins) {
        if (!manager->unloadPlugin(name)) {
            if (logger_) {
                NLM_LOG_ERROR("Error shutting down plugin: " + name);
            }
            success = false;
        }
    }
    return success;
}

} // namespace cli
} // namespace nlm
