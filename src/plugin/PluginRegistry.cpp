#include "PluginRegistry.hpp"
#include "PluginLoader.hpp"
#include <dlfcn.h>
#include <iostream>

namespace nlm {

std::unique_ptr<PluginBase> PluginRegistry::loadPlugin(const std::string& libraryPath) {
    try {
        auto plugin = PluginLoader::loadPlugin(libraryPath);
        PluginMetadata metadata = plugin->getMetadata();
        
        std::lock_guard<std::mutex> lock(mutex);
        externalPlugins[metadata.name] = plugin->clone();
        metadataRegistry[metadata.name] = metadata;
        
        return std::move(externalPlugins[metadata.name]);
    } catch (const std::exception& e) {
        std::cerr << "Failed to load plugin from " << libraryPath << ": " << e.what() << std::endl;
        return nullptr;
    }
}

std::unique_ptr<PluginBase> PluginRegistry::getPlugin(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = builtInPlugins.find(name);
    if (it != builtInPlugins.end()) {
        return it->second->clone();
    }
    
    it = externalPlugins.find(name);
    if (it != externalPlugins.end()) {
        return it->second->clone();
    }
    
    return nullptr;
}

bool PluginRegistry::hasPlugin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    return builtInPlugins.find(name) != builtInPlugins.end() ||
           externalPlugins.find(name) != externalPlugins.end();
}

std::vector<std::string> PluginRegistry::getPluginNames() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> names;
    names.reserve(builtInPlugins.size() + externalPlugins.size());
    
    for (const auto& pair : builtInPlugins) {
        names.push_back(pair.first);
    }
    
    for (const auto& pair : externalPlugins) {
        names.push_back(pair.first);
    }
    
    return names;
}

PluginMetadata PluginRegistry::getPluginMetadata(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = metadataRegistry.find(name);
    return it != metadataRegistry.end() ? it->second : PluginMetadata{};
}

void PluginRegistry::initializeAll(const std::unordered_map<std::string, std::string>& globalConfig) {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (auto& pair : builtInPlugins) {
        pair.second->initialize(globalConfig);
    }
    
    for (auto& pair : externalPlugins) {
        pair.second->initialize(globalConfig);
    }
}

void PluginRegistry::shutdownAll() {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (auto& pair : builtInPlugins) {
        pair.second->shutdown();
    }
    
    for (auto& pair : externalPlugins) {
        pair.second->shutdown();
    }
}

void PluginRegistry::updateAll(double timestep) {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (auto& pair : builtInPlugins) {
        pair.second->update(timestep);
    }
    
    for (auto& pair : externalPlugins) {
        pair.second->update(timestep);
    }
}

void PluginRegistry::resetAll() {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (auto& pair : builtInPlugins) {
        pair.second->reset();
    }
    
    for (auto& pair : externalPlugins) {
        pair.second->reset();
    }
}

PluginRegistry::RegistryStats PluginRegistry::getStats() const {
    std::lock_guard<std::mutex> lock(mutex);
    RegistryStats stats;
    
    stats.builtInCount = builtInPlugins.size();
    stats.externalCount = externalPlugins.size();
    
    for (const auto& pair : builtInPlugins) {
        stats.totalMemoryUsage += pair.second->getMemoryUsage();
    }
    
    for (const auto& pair : externalPlugins) {
        stats.totalMemoryUsage += pair.second->getMemoryUsage();
    }
    
    return stats;
}

} // namespace nlm