// Plugin registry implementation
// 
// Manages plugin lifecycle, registration, and instantiation.

#include "PluginArchitecture.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace nlm {

void PluginRegistry::registerPlugin(const std::string& name,
                                   const std::string& version,
                                   const std::string& description,
                                   PluginFactory factory) {
    if (pluginFactories.find(name) != pluginFactories.end()) {
        PluginUtils::logPluginEvent(name, "Plugin already registered - overwriting");
    }
    
    pluginMetadata[name] = {version, description};
    pluginFactories[name] = factory;
    pluginEnabled[name] = true;
    
    PluginUtils::logPluginEvent(name, "Plugin registered successfully");
}

PluginFactory PluginRegistry::getPlugin(const std::string& name) const {
    auto it = pluginFactories.find(name);
    if (it == pluginFactories.end()) {
        PluginUtils::logPluginEvent(name, "Plugin not found");
        return nullptr;
    }
    return it->second;
}

bool PluginRegistry::isRegistered(const std::string& name) const {
    return pluginFactories.find(name) != pluginFactories.end();
}

std::vector<std::string> PluginRegistry::getRegisteredPluginNames() const {
    std::vector<std::string> names;
    names.reserve(pluginFactories.size());
    for (const auto& pair : pluginFactories) {
        names.push_back(pair.first);
    }
    return names;
}

std::unique_ptr<Plugin> PluginRegistry::createPlugin(const std::string& name) {
    if (!isRegistered(name) || !pluginEnabled[name]) {
        PluginUtils::logPluginEvent(name, "Cannot create plugin - not registered or disabled");
        return nullptr;
    }
    
    PluginFactory factory = getPlugin(name);
    if (!factory) {
        PluginUtils::logPluginEvent(name, "Cannot create plugin - invalid factory");
        return nullptr;
    }
    
    auto plugin = factory();
    if (!plugin) {
        PluginUtils::logPluginEvent(name, "Cannot create plugin - factory returned null");
        return nullptr;
    }
    
    // Initialize plugin
    if (!plugin->initialize()) {
        PluginUtils::logPluginEvent(name, "Plugin initialization failed");
        return nullptr;
    }
    
    // Store instance
    pluginInstances[name] = std::move(plugin);
    
    PluginUtils::logPluginEvent(name, "Plugin created and initialized successfully");
    return std::move(pluginInstances[name]);
}

bool PluginRegistry::loadPluginsFromDirectory(const std::string& directory) {
    namespace fs = std::filesystem;
    
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        PluginUtils::logPluginEvent("system", "Plugin directory does not exist: " + directory);
        return false;
    }
    
    bool success = true;
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            
            std::string pluginExt = getPlatformPluginExtension();
            if (extension == pluginExt) {
                // Try to load plugin
                auto plugin = createDynamicPlugin(entry.path().string());
                if (plugin) {
                    PluginUtils::logPluginEvent(entry.path().filename().string(), 
                                             "Plugin loaded from directory");
                } else {
                    PluginUtils::logPluginEvent(entry.path().filename().string(),
                                             "Failed to load plugin from directory");
                    success = false;
                }
            }
        }
    }
    
    return success;
}

std::vector<PluginRegistry::PluginInfo> PluginRegistry::getPluginInfo() const {
    std::vector<PluginInfo> info;
    info.reserve(pluginMetadata.size());
    
    for (const auto& pair : pluginMetadata) {
        PluginInfo pluginInfo;
        pluginInfo.name = pair.first;
        pluginInfo.version = pair.second.first;
        pluginInfo.description = pair.second.second;
        pluginInfo.enabled = pluginEnabled.count(pair.first) ? pluginEnabled.at(pair.first) : true;
        info.push_back(pluginInfo);
    }
    
    return info;
}

void PluginRegistry::setPluginEnabled(const std::string& name, bool enabled) {
    if (pluginFactories.find(name) != pluginFactories.end()) {
        pluginEnabled[name] = enabled;
        
        if (!enabled && pluginInstances.find(name) != pluginInstances.end()) {
            pluginInstances[name]->cleanup();
            pluginInstances.erase(name);
        }
        
        PluginUtils::logPluginEvent(name, enabled ? "Plugin enabled" : "Plugin disabled");
    }
}

void PluginRegistry::cleanupAllPlugins() {
    for (auto& pair : pluginInstances) {
        if (pair.second) {
            pair.second->cleanup();
        }
    }
    
    pluginInstances.clear();
    PluginUtils::logPluginEvent("system", "All plugins cleaned up");
}

std::unique_ptr<Plugin> PluginRegistry::createDynamicPlugin(const std::string& path) {
    // Dynamic plugin loading implementation (platform-specific)
    // This is a placeholder - actual implementation would use dlopen/dlsym
    // on Unix or LoadLibrary/GetProcAddress on Windows
    
    PluginUtils::logPluginEvent("system", "Dynamic plugin loading not implemented for " + path);
    return nullptr;
}

namespace PluginUtils {

std::string findPluginDirectory() {
    // Try common plugin directories
    const std::vector<std::string> commonDirs = {
        "./plugins",
        "../plugins",
        "/usr/local/lib/nlm/plugins",
        "/opt/nlm/plugins",
        "plugins"
    };
    
    for (const auto& dir : commonDirs) {
        if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
            return dir;
        }
    }
    
    return "./plugins"; // Default
}

std::string getPlatformPluginExtension() {
    #ifdef _WIN32
        return ".dll";
    #elif __APPLE__ || __MACH__
        return ".dylib";
    #else
        return ".so";
    #endif
}

bool validatePluginName(const std::string& name) {
    if (name.empty()) return false;
    // Valid characters: alphanumeric, underscore, hyphen
    return std::all_of(name.begin(), name.end(), 
                      [](char c) { return std::isalnum(c) || c == '_' || c == '-'; });
}

bool validatePluginVersion(const std::string& version) {
    if (version.empty()) return false;
    // Basic version validation - could be enhanced with semantic version parsing
    return true; // For now, accept any non-empty version
}

void logPluginEvent(const std::string& pluginName, const std::string& event) {
    std::cout << "[PluginRegistry] " << pluginName << ": " << event << std::endl;
    // Could also log to a proper logging system
}

} // namespace PluginUtils

} // namespace nlm