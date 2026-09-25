// NLM Plugin System Implementation
// Phase 2: Real Neural Computation - Plugin Implementation
//
// This file provides the implementation of the plugin system,
// including plugin loading, component factories, and plugin management.

#include "extensions/PluginSystem.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/MotorSystem.hpp"
#include "environment/Environment.hpp"

#include <fstream>
#include <sstream>
#include <json/json.hpp>

using json = nlohmann::json;

namespace nlm {

// JSON serialization/deserialization helper functions
namespace {
    std::any jsonToAny(const json& j) {
        if (j.is_null()) {
            return std::any();
        } else if (j.is_boolean()) {
            return j.get<bool>();
        } else if (j.is_number_integer()) {
            return j.get<int64_t>();
        } else if (j.is_number_unsigned()) {
            return j.get<uint64_t>();
        } else if (j.is_number()) {
            return j.get<double>();
        } else if (j.is_string()) {
            return j.get<std::string>();
        } else if (j.is_array()) {
            return j.get<std::vector<std::any>>();
        } else if (j.is_object()) {
            return j.get<std::map<std::string, std::any>>();
        }
        return std::any();
    }
    
    json anyToJson(const std::any& value) {
        if (value.type() == typeid(void)) {
            return nullptr;
        } else if (value.type() == typeid(bool)) {
            return std::any_cast<bool>(value);
        } else if (value.type() == typeid(int64_t)) {
            return std::any_cast<int64_t>(value);
        } else if (value.type() == typeid(uint64_t)) {
            return std::any_cast<uint64_t>(value);
        } else if (value.type() == typeid<double)) {
            return std::any_cast<double>(value);
        } else if (value.type() == typeid(std::string)) {
            return std::any_cast<std::string>(value);
        } else if (value.type() == typeid(std::vector<std::any>)) {
            return std::any_cast<std::vector<std::any>>(value);
        } else if (value.type() == typeid(std::map<std::string, std::any>)) {
            return std::any_cast<std::map<std::string, std::any>>(value);
        }
        return nullptr;
    }
}

// FileSystemPluginDiscovery implementation
std::vector<std::string> FileSystemPluginDiscovery::discoverPlugins(const std::string& directory) {
    std::vector<std::string> plugins;
    
    if (!std::filesystem::exists(directory)) {
        return plugins;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file() && isPluginFile(entry.path().string())) {
            plugins.push_back(entry.path().string());
        }
    }
    
    return plugins;
}

PluginMetadata FileSystemPluginDiscovery::loadMetadata(const std::string& pluginPath) {
    PluginMetadata metadata;
    metadata.filePath = pluginPath;
    
    // Extract filename
    std::filesystem::path path(pluginPath);
    metadata.name = path.stem().string();
    
    // Try to load metadata file
    std::string metadataPath = path.parent_path().string() + "/" + metadata.name + ".plugin.json";
    if (std::filesystem::exists(metadataPath)) {
        try {
            std::ifstream file(metadataPath);
            json j = json::parse(file);
            
            metadata.name = j.value("name", metadata.name);
            metadata.version = j.value("version", "1.0.0");
            metadata.description = j.value("description", "");
            metadata.author = j.value("author", "Unknown");
            metadata.license = j.value("license", "MIT");
            metadata.homepage = j.value("homepage", "");
            metadata.category = j.value("category", "general");
            
            if (j.contains("tags")) {
                metadata.tags = j["tags"].get<std::vector<std::string>>();
            }
            
            if (j.contains("dependencies")) {
                metadata.dependencies = j["dependencies"].get<std::vector<std::string>>();
            }
            
            if (j.contains("configuration")) {
                metadata.configuration = j["configuration"].get<std::map<std::string, std::string>>();
            }
            
            metadata.enabled = j.value("enabled", true);
            metadata.autoLoad = j.value("auto_load", false);
            
        } catch (const std::exception& e) {
            // Fallback to default metadata
            metadata.description = "Plugin loaded from " + metadata.name;
            metadata.version = "1.0.0";
            metadata.author = "Unknown";
        }
    } else {
        // Create default metadata
        metadata.description = "Dynamic plugin";
        metadata.version = "1.0.0";
        metadata.author = "NLM System";
    }
    
    metadata.lastModified = std::chrono::system_clock::now();
    
    return metadata;
}

bool FileSystemPluginDiscovery::isPluginFile(const std::string& filePath) {
    std::filesystem::path path(filePath);
    std::string extension = path.extension().string();
    
    // Check for common plugin extensions
    std::vector<std::string> pluginExtensions = {".so", ".dll", ".dylib", ".plugin"};
    return std::find(pluginExtensions.begin(), pluginExtensions.end(), extension) != pluginExtensions.end();
}

// DynamicPluginLoader implementation
std::shared_ptr<Plugin> DynamicPluginLoader::loadPlugin(const std::string& pluginPath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if already loaded
    std::string pluginName = getPluginNameFromPath(pluginPath);
    if (isPluginLoaded(pluginName)) {
        return nullptr;
    }
    
    try {
        // Load metadata
        PluginMetadata metadata = discovery_->loadMetadata(pluginPath);
        
        // Create plugin instance
        auto plugin = createPluginInstance(pluginPath, metadata);
        
        // Store plugin
        plugins_[pluginName] = plugin;
        
        // Initialize plugin
        plugin->initialize();
        
        // Call load hook
        plugin->onLoad();
        
        return plugin;
        
    } catch (const std::exception& e) {
        // Log error
        return nullptr;
    }
}

bool DynamicPluginLoader::unloadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugins_.find(pluginName);
    if (it == plugins_.end()) {
        return false;
    }
    
    auto plugin = it->second;
    
    // Call unload hook
    plugin->onUnload();
    
    // Shutdown plugin
    plugin->shutdown();
    
    // Remove plugin
    plugins_.erase(it);
    
    return true;
}

std::shared_ptr<Plugin> DynamicPluginLoader::getPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = plugins_.find(pluginName);
    return (it != plugins_.end()) ? it->second : nullptr;
}

// PluginManager implementation
void PluginManager::scanPlugins(const std::string& directory) {
    std::vector<std::string> pluginFiles = discovery_->discoverPlugins(directory);
    
    for (const auto& pluginPath : pluginFiles) {
        PluginMetadata metadata = discovery_->loadMetadata(pluginPath);
        
        // Check if plugin should be auto-loaded
        if (metadata.autoLoad) {
            loadPlugin(pluginPath);
        }
    }
}

std::shared_ptr<Plugin> PluginManager::loadPlugin(const std::string& pluginPath) {
    auto plugin = loader_->loadPlugin(pluginPath);
    if (plugin) {
        // Store in registry
        pluginRegistry_[plugin->getMetadata().name] = plugin;
        
        // Configure plugin if configuration exists
        std::string pluginName = plugin->getMetadata().name;
        auto it = pluginConfigurations_.find(pluginName);
        if (it != pluginConfigurations_.end()) {
            plugin->setConfiguration(it->second);
        }
    }
    
    return plugin;
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    // Remove from registry
    auto it = pluginRegistry_.find(pluginName);
    if (it == pluginRegistry_.end()) {
        return false;
    }
    
    // Unload from loader
    bool unloaded = loader_->unloadPlugin(pluginName);
    if (unloaded) {
        pluginRegistry_.erase(it);
    }
    
    // Remove configuration
    pluginConfigurations_.erase(pluginName);
    
    return unloaded;
}

void PluginManager::configurePlugin(const std::string& pluginName, const std::map<std::string, std::any>& config) {
    pluginConfigurations_[pluginName] = config;
    
    // Apply configuration to loaded plugin
    auto plugin = getPlugin(pluginName);
    if (plugin) {
        plugin->setConfiguration(config);
    }
}

void PluginManager::updatePluginConfiguration(const std::string& pluginName, const std::map<std::string, std::any>& newConfig) {
    auto it = pluginConfigurations_.find(pluginName);
    if (it != pluginConfigurations_.end()) {
        it->second.merge(newConfig);
        
        // Notify plugin
        auto plugin = getPlugin(pluginName);
        if (plugin) {
            plugin->onConfigurationUpdate(newConfig);
        }
    }
}

PluginMetadata PluginManager::getPluginMetadata(const std::string& pluginName) const {
    auto plugin = loader_->getPlugin(pluginName);
    return plugin ? plugin->getMetadata() : PluginMetadata{};
}

void PluginManager::refreshPlugins(const std::string& directory) {
    // Unload plugins that no longer exist
    std::vector<std::string> currentPlugins;
    for (const auto& pluginPath : discovery_->discoverPlugins(directory)) {
        currentPlugins.push_back(getPluginNameFromPath(pluginPath));
    }
    
    std::vector<std::string> loadedPlugins = getLoadedPlugins();
    
    for (const auto& pluginName : loadedPlugins) {
        if (std::find(currentPlugins.begin(), currentPlugins.end(), pluginName) == currentPlugins.end()) {
            unloadPlugin(pluginName);
        }
    }
    
    // Load new plugins
    for (const auto& pluginPath : discovery_->discoverPlugins(directory)) {
        if (!isPluginLoaded(getPluginNameFromPath(pluginPath))) {
            loadPlugin(pluginPath);
        }
    }
}

} // namespace nlm
