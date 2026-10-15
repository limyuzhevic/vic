#include "PluginManager.hpp"
#include <algorithm>
#include <stdexcept>
#include <filesystem>
#include <iostream>

namespace nlm {

PluginManager& PluginManager::getInstance() {
    static PluginManager instance;
    return instance;
}

PluginManager::PluginManager() {
    discoverBuiltInPlugins();
}

void PluginManager::registerPlugin(std::unique_ptr<TestPlugin> plugin) {
    if (!plugin) {
        return;
    }
    
    std::string name = plugin->getName();
    
    // Check if plugin already exists
    if (hasPlugin(name)) {
        std::cerr << "Warning: Plugin " << name << " already registered, replacing" << std::endl;
        removePlugin(name);
    }
    
    loadPlugin(name, std::move(plugin));
}

bool PluginManager::registerDynamicPlugin(const std::string& libraryPath) {
    try {
        auto plugin = std::make_unique<DynamicPlugin>(libraryPath);
        registerPlugin(std::move(plugin));
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to register dynamic plugin " << libraryPath << ": " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::string> PluginManager::getAvailablePlugins() const {
    std::vector<std::string> names;
    
    for (const auto& pair : pluginMap_) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<std::string> PluginManager::getEnabledPlugins() const {
    std::vector<std::string> enabledNames;
    
    for (const auto& pair : pluginMap_) {
        if (pluginEnabled_.find(pair.first) != pluginEnabled_.end() && pluginEnabled_.at(pair.first)) {
            enabledNames.push_back(pair.first);
        }
    }
    
    return enabledNames;
}

bool PluginManager::isPluginEnabled(const std::string& pluginName) const {
    auto it = pluginEnabled_.find(pluginName);
    return it != pluginEnabled_.end() && it->second;
}

void PluginManager::enablePlugin(const std::string& pluginName) {
    auto it = pluginMap_.find(pluginName);
    if (it != pluginMap_.end()) {
        pluginEnabled_[pluginName] = true;
    }
}

void PluginManager::disablePlugin(const std::string& pluginName) {
    pluginEnabled_[pluginName] = false;
}

void PluginManager::enableAllPlugins() {
    for (const auto& pair : pluginMap_) {
        pluginEnabled_[pair.first] = true;
    }
}

void PluginManager::disableAllPlugins() {
    for (auto& pair : pluginEnabled_) {
        pair.second = false;
    }
}

void PluginManager::reloadPlugin(const std::string& pluginName) {
    auto it = pluginMap_.find(pluginName);
    if (it != pluginMap_.end()) {
        std::unique_ptr<TestPlugin> plugin = std::move(it->second);
        
        // Remove the plugin
        pluginMap_.erase(it);
        pluginEnabled_.erase(pluginName);
        
        // Re-register
        registerPlugin(std::move(plugin));
    }
}

void PluginManager::shutdownAllPlugins() {
    for (auto& pair : pluginMap_) {
        pair.second->shutdown();
    }
    
    plugins_.clear();
    pluginMap_.clear();
}

std::vector<std::unique_ptr<TestBase>> PluginManager::loadTests(const std::vector<std::string>& categories) {
    std::vector<std::unique_ptr<TestBase>> allTests;
    
    for (const auto& plugin : plugins_) {
        if (!isPluginEnabled(plugin->getName())) {
            continue;
        }
        
        // Check if plugin supports the requested categories
        bool pluginSupportsCategory = categories.empty();
        if (!pluginSupportsCategory && !categories.empty()) {
            std::vector<std::string> supportedCategories = plugin->getSupportedCategories();
            for (const auto& category : categories) {
                if (std::find(supportedCategories.begin(), supportedCategories.end(), category) != supportedCategories.end()) {
                    pluginSupportsCategory = true;
                    break;
                }
            }
        }
        
        if (pluginSupportsCategory) {
            std::vector<std::unique_ptr<TestBase>> pluginTests = plugin->createTests();
            allTests.insert(allTests.end(), std::make_move_iterator(pluginTests.begin()), 
                          std::make_move_iterator(pluginTests.end()));
        }
    }
    
    return allTests;
}

std::vector<std::unique_ptr<TestBase>> PluginManager::loadTestsByPhase(const std::string& phase) {
    std::vector<std::unique_ptr<TestBase>> allTests;
    
    for (const auto& plugin : plugins_) {
        if (!isPluginEnabled(plugin->getName())) {
            continue;
        }
        
        std::vector<std::string> supportedPhases = plugin->getSupportedPhases();
        if (std::find(supportedPhases.begin(), supportedPhases.end(), phase) != supportedPhases.end()) {
            std::vector<std::unique_ptr<TestBase>> pluginTests = plugin->createTests();
            allTests.insert(allTests.end(), std::make_move_iterator(pluginTests.begin()), 
                          std::make_move_iterator(pluginTests.end()));
        }
    }
    
    return allTests;
}

std::vector<std::unique_ptr<TestBase>> PluginManager::loadTestsByType(const std::string& testType) {
    std::vector<std::unique_ptr<TestBase>> allTests;
    
    for (const auto& plugin : plugins_) {
        if (!isPluginEnabled(plugin->getName())) {
            continue;
        }
        
        if (plugin->canHandle(testType)) {
            std::vector<std::unique_ptr<TestBase>> pluginTests = plugin->createTests();
            allTests.insert(allTests.end(), std::make_move_iterator(pluginTests.begin()), 
                          std::make_move_iterator(pluginTests.end()));
        }
    }
    
    return allTests;
}

void PluginManager::configurePlugin(const std::string& pluginName, const std::map<std::string, std::string>& options) {
    auto it = pluginMap_.find(pluginName);
    if (it != pluginMap_.end()) {
        it->second->configure(options);
    }
}

std::map<std::string, std::string> PluginManager::getPluginConfig(const std::string& pluginName) const {
    std::map<std::string, std::string> config;
    
    auto it = pluginMap_.find(pluginName);
    if (it != pluginMap_.end()) {
        // In a real implementation, plugins would have a method to get their config
        // For now, return empty config
        config["status"] = "configured";
    }
    
    return config;
}

TestPlugin* PluginManager::getPlugin(const std::string& pluginName) {
    auto it = pluginMap_.find(pluginName);
    return it != pluginMap_.end() ? it->second.get() : nullptr;
}

std::vector<TestPlugin*> PluginManager::getAllPlugins() const {
    std::vector<TestPlugin*> pluginPointers;
    
    for (const auto& plugin : plugins_) {
        pluginPointers.push_back(plugin.get());
    }
    
    return pluginPointers;
}

size_t PluginManager::getPluginCount() const {
    return plugins_.size();
}

bool PluginManager::hasPlugin(const std::string& pluginName) const {
    return pluginMap_.find(pluginName) != pluginMap_.end();
}

std::string PluginManager::getPluginPath(const std::string& pluginName) const {
    auto it = pluginPaths_.find(pluginName);
    return it != pluginPaths_.end() ? it->second : "";
}

void PluginManager::discoverBuiltInPlugins() {
    // In a real implementation, this would discover built-in plugins
    // For now, we'll just add a placeholder
    std::cout << "Plugin Manager: Discovering built-in plugins..." << std::endl;
    std::cout << "Plugin Manager: No built-in plugins found (use --help for available options)" << std::endl;
}

void PluginManager::scanPluginDirectories() {
    // Scan for plugins in standard directories
    std::vector<std::string> pluginDirectories = {
        "./plugins",
        "../plugins",
        "/usr/local/lib/nlm/plugins",
        "/usr/lib/nlm/plugins"
    };
    
    for (const auto& directory : pluginDirectories) {
        if (std::filesystem::exists(directory)) {
            std::cout << "Plugin Manager: Scanning " << directory << std::endl;
            // In a real implementation, scan for plugin libraries
        }
    }
}

void PluginManager::loadPlugin(const std::string& name, std::unique_ptr<TestPlugin> plugin) {
    if (!plugin) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Store plugin
    plugins_.push_back(std::move(plugin));
    TestPlugin* pluginPtr = plugins_.back().get();
    pluginMap_[name] = std::move(plugins_.back());
    
    // Set default enabled state
    pluginEnabled_[name] = true;
    
    // Initialize plugin
    try {
        pluginPtr->initialize(Config()); // Empty config for now
    } catch (const std::exception& e) {
        std::cerr << "Warning: Plugin " << name << " failed to initialize: " << e.what() << std::endl;
        // Remove plugin if initialization fails
        pluginMap_.erase(name);
        plugins_.pop_back();
        return;
    }
}

void PluginManager::removePlugin(const std::string& name) {
    auto it = pluginMap_.find(name);
    if (it != pluginMap_.end()) {
        // Shutdown plugin
        it->second->shutdown();
        
        // Remove from collections
        pluginMap_.erase(it);
        pluginEnabled_.erase(name);
        pluginPaths_.erase(name);
        
        // Also remove from plugins_ vector
        plugins_.erase(std::remove_if(plugins_.begin(), plugins_.end(),
            [&name](const std::unique_ptr<TestPlugin>& p) { return p->getName() == name; }),
            plugins_.end());
    }
}

} // namespace nlm
