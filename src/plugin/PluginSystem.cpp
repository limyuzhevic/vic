#include "PluginSystem.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <dlfcn.h>  // For dynamic loading (Linux/Mac)

namespace nlm {

// PluginManager implementation
struct PluginManager::Impl {
    std::shared_ptr<Brain> brain_;
    std::unordered_map<std::string, std::unique_ptr<PluginFactory>> plugins_;
    std::unordered_map<std::string, bool> pluginEnabled_;
    std::vector<std::string> staticPlugins_;
    std::unordered_map<std::string, PluginMetadata> pluginMetadata_;
    std::atomic<size_t> totalPluginsCreated_;
    std::atomic<size_t> totalPluginsDestroyed_;
    
    Impl() : totalPluginsCreated_(0), totalPluginsDestroyed_(0) {}
};

PluginManager::PluginManager() : pImpl(std::make_unique<Impl>()) {}
PluginManager::~PluginManager() = default;

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Extract plugin name from path
    std::string pluginName = pluginPath;
    size_t lastSlash = pluginPath.find_last_of("/");
    if (lastSlash != std::string::npos) {
        pluginName = pluginPath.substr(lastSlash + 1);
    }
    
    // Check if already loaded
    if (plugins_.find(pluginName) != plugins_.end()) {
        std::cout << "Plugin already loaded: " << pluginName << std::endl;
        return true;
    }
    
    // For now, we'll only support static plugins
    // Dynamic loading would require platform-specific code
    std::cout << "Warning: Dynamic plugin loading not yet implemented. Use static registration." << std::endl;
    std::cout << "Plugin loading skipped: " << pluginPath << std::endl;
    
    return false;
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(pluginName);
    if (it == plugins_.end()) {
        std::cout << "Plugin not found: " << pluginName << std::endl;
        return false;
    }
    
    pluginEnabled_.erase(pluginName);
    plugins_.erase(it);
    
    pImpl->totalPluginsDestroyed_++;
    std::cout << "Plugin unloaded: " << pluginName << std::endl;
    
    return true;
}

bool PluginManager::reloadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (unloadPlugin(pluginName)) {
        return loadPlugin("unknown"); // Would need to track original path
    }
    
    return false;
}

std::unique_ptr<BrainComponent> PluginManager::createComponent(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(pluginName);
    if (it == plugins_.end()) {
        std::cout << "Plugin not found: " << pluginName << std::endl;
        return nullptr;
    }
    
    if (!isPluginEnabled(pluginName)) {
        std::cout << "Plugin disabled: " << pluginName << std::endl;
        return nullptr;
    }
    
    auto component = it->second->createInstance();
    if (component) {
        pImpl->totalPluginsCreated_++;
        component->setBrain(pImpl->brain_);
    }
    
    return component;
}

std::vector<std::unique_ptr<BrainComponent>> PluginManager::createComponents(
    const std::vector<std::string>& pluginNames) {
    std::vector<std::unique_ptr<BrainComponent>> components;
    
    for (const auto& name : pluginNames) {
        auto component = createComponent(name);
        if (component) {
            components.push_back(std::move(component));
        }
    }
    
    return components;
}

bool PluginManager::enablePlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(pluginName);
    if (it == plugins_.end()) {
        return false;
    }
    
    pluginEnabled_[pluginName] = true;
    return true;
}

bool PluginManager::disablePlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(pluginName);
    if (it == plugins_.end()) {
        return false;
    }
    
    pluginEnabled_[pluginName] = false;
    return true;
}

bool PluginManager::isPluginEnabled(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pluginEnabled_.find(pluginName);
    if (it == pluginEnabled_.end()) {
        // Default to enabled if not specified
        return true;
    }
    
    return it->second;
}

std::vector<std::string> PluginManager::getAvailablePlugins() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(plugins_.size());
    
    for (const auto& pair : plugins_) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<std::string> PluginManager::getEnabledPlugins() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    
    for (const auto& pair : plugins_) {
        if (isPluginEnabled(pair.first)) {
            names.push_back(pair.first);
        }
    }
    
    return names;
}

PluginMetadata PluginManager::getPluginMetadata(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = pluginMetadata_.find(pluginName);
    if (it == pluginMetadata_.end()) {
        return PluginMetadata();
    }
    
    return it->second;
}

std::vector<PluginMetadata> PluginManager::getPluginMetadata() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PluginMetadata> metadata;
    metadata.reserve(pluginMetadata_.size());
    
    for (const auto& pair : pluginMetadata_) {
        metadata.push_back(pair.second);
    }
    
    return metadata;
}

void PluginManager::setBrain(std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    pImpl->brain_ = brain;
    
    // Pass brain to existing plugins
    for (auto& pair : plugins_) {
        if (pair.second) {
            // This would need a method in PluginFactory to set brain reference
            // For now, we'll handle this when components are created
        }
    }
}

std::shared_ptr<Brain> PluginManager::getBrain() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pImpl->brain_;
}

bool PluginManager::configurePlugin(const std::string& pluginName,
                                   const std::string& key,
                                   const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(pluginName);
    if (it == plugins_.end()) {
        return false;
    }
    
    // This would require the plugin to have configuration methods
    // For now, we'll just log
    std::cout << "Configure not implemented for plugin: " << pluginName 
              << ", key: " << key << ", value: " << value << std::endl;
    
    return true;
}

std::string PluginManager::getManagerStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream ss;
    ss << "Plugin Manager Statistics:" << std::endl;
    ss << "  Total plugins loaded: " << plugins_.size() << std::endl;
    ss << "  Enabled plugins: " << getEnabledPlugins().size() << std::endl;
    ss << "  Components created: " << pImpl->totalPluginsCreated_ << std::endl;
    ss << "  Components destroyed: " << pImpl->totalPluginsDestroyed_ << std::endl;
    
    return ss.str();
}

size_t PluginManager::getPluginCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return plugins_.size();
}

size_t PluginManager::getEnabledPluginCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return getEnabledPlugins().size();
}

bool PluginManager::validateAllPlugins() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& pair : plugins_) {
        if (!pair.second->validate()) {
            std::cout << "Plugin validation failed: " << pair.first << std::endl;
            return false;
        }
    }
    
    return true;
}

bool PluginManager::checkDependencies() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check dependencies between plugins
    for (const auto& pair : plugins_) {
        const std::string& pluginName = pair.first;
        const PluginFactory* factory = pair.second.get();
        
        PluginMetadata metadata = factory->getMetadata();
        
        for (const std::string& dep : metadata.dependencies) {
            if (plugins_.find(dep) == plugins_.end()) {
                std::cout << "Plugin dependency missing: " << pluginName 
                          << " requires " << dep << std::endl;
                return false;
            }
        }
    }
    
    return true;
}

void PluginManager::shutdownAllPlugins() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& pair : plugins_) {
        // This would call shutdown on the factory or plugin instances
        // For now, we'll just log
        std::cout << "Shutting down plugin: " << pair.first << std::endl;
    }
    
    plugins_.clear();
    pluginEnabled_.clear();
}

void PluginManager::clearAllPlugins() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    shutdownAllPlugins();
    staticPlugins_.clear();
    pluginMetadata_.clear();
}

} // namespace nlm
