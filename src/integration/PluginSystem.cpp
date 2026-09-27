#include "PluginSystem.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <unordered_set>

namespace nlm {
namespace integration {

// PluginManager implementation
bool PluginManager::loadPlugin(const std::string& plugin_path) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (isPluginLoaded(plugin_name)) {
        return false; // Plugin already loaded
    }
    
    // In a real implementation, this would load the plugin dynamically
    // For now, we'll create a simple placeholder
    std::string plugin_name = std::filesystem::path(plugin_path).stem().string();
    
    // Create plugin info
    PluginInfo info;
    info.name = plugin_name;
    info.version = "1.0.0";
    info.author = "NLM Core";
    info.description = "Loaded plugin: " + plugin_name;
    info.plugin_path = plugin_path;
    info.interface_version = "1.0";
    
    plugin_infos_[plugin_name] = info;
    
    // Create placeholder plugin interface
    class PlaceholderPlugin : public PluginInterface {
    private:
        PluginInfo info_;
    public:
        PlaceholderPlugin(const PluginInfo& info) : info_(info) {}
        bool initialize(const std::map<std::string, std::string>& config) override { return true; }
        bool shutdown() override { return true; }
        bool pause() override { return true; }
        bool resume() override { return true; }
        const PluginInfo& getInfo() const override { return info_; }
        std::string getInterfaceVersion() const override { return info_.interface_version; }
        std::string getName() const override { return info_.name; }
        std::vector<std::string> getCapabilities() const override { return {}; }
        bool handleEvent(const std::string& event_type, const std::map<std::string, std::string>& data) override { return true; }
        bool subscribeToEvent(const std::string& event_type) override { return true; }
        bool unsubscribeFromEvent(const std::string& event_type) override { return true; }
        bool allocateResources(size_t resources) override { return true; }
        bool releaseResources(size_t resources) override { return true; }
        size_t getAvailableResources() const override { return 1000; }
    };
    
    loaded_plugins_[plugin_name] = std::make_unique<PlaceholderPlugin>(info);
    
    return true;
}

bool PluginManager::unloadPlugin(const std::string& plugin_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = loaded_plugins_.find(plugin_name);
    if (it == loaded_plugins_.end()) {
        return false; // Plugin not found
    }
    
    loaded_plugins_.erase(it);
    plugin_infos_.erase(plugin_name);
    event_subscriptions_.erase(plugin_name);
    
    return true;
}

bool PluginManager::reloadPlugin(const std::string& plugin_name) {
    if (!isPluginLoaded(plugin_name)) {
        return false;
    }
    
    // Get plugin path
    auto it = plugin_infos_.find(plugin_name);
    if (it == plugin_infos_.end()) {
        return false;
    }
    
    // Unload and reload
    unloadPlugin(plugin_name);
    return loadPlugin(it->second.plugin_path);
}

std::vector<PluginInfo> PluginManager::discoverPlugins(const std::string& directory) {
    std::vector<PluginInfo> plugins;
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            
            // Check for common plugin extensions
            if (extension == ".so" || extension == ".dll" || extension == ".dylib" ||
                extension == ".plugin" || extension == ".nlmplugin") {
                
                PluginInfo info;
                info.name = entry.path().stem().string();
                info.plugin_path = entry.path().string();
                
                // Try to load basic info (in real implementation)
                plugins.push_back(info);
            }
        }
    }
    
    return plugins;
}

PluginInfo PluginManager::getPluginInfo(const std::string& plugin_name) const {
    auto it = plugin_infos_.find(plugin_name);
    if (it != plugin_infos_.end()) {
        return it->second;
    }
    
    PluginInfo empty_info;
    empty_info.name = "Not found";
    return empty_info;
}

bool PluginManager::initializeAllPlugins() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& [name, plugin] : loaded_plugins_) {
        if (!checkPluginDependencies(name)) {
            continue; // Skip plugins with unmet dependencies
        }
        
        if (!plugin->initialize({})) {
            std::cerr << "Failed to initialize plugin: " << name << std::endl;
            return false;
        }
    }
    
    plugins_initialized_ = true;
    return true;
}

bool PluginManager::shutdownAllPlugins() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& [name, plugin] : loaded_plugins_) {
        if (!plugin->shutdown()) {
            std::cerr << "Failed to shutdown plugin: " << name << std::endl;
            return false;
        }
    }
    
    plugins_initialized_ = false;
    return true;
}

bool PluginManager::pauseAllPlugins() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& [name, plugin] : loaded_plugins_) {
        if (!plugin->pause()) {
            std::cerr << "Failed to pause plugin: " << name << std::endl;
            return false;
        }
    }
    
    return true;
}

bool PluginManager::resumeAllPlugins() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& [name, plugin] : loaded_plugins_) {
        if (!plugin->resume()) {
            std::cerr << "Failed to resume plugin: " << name << std::endl;
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> PluginManager::getLoadedPluginNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> names;
    for (const auto& [name, plugin] : loaded_plugins_) {
        names.push_back(name);
    }
    
    return names;
}

bool PluginManager::isPluginLoaded(const std::string& plugin_name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return loaded_plugins_.count(plugin_name) > 0;
}

bool PluginManager::isPluginInitialized(const std::string& plugin_name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = loaded_plugins_.find(plugin_name);
    if (it != loaded_plugins_.end()) {
        return it->second->getInfo().name != "";
    }
    
    return false;
}

bool PluginManager::publishEvent(const std::string& event_type, 
                                const std::map<std::string, std::string>& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> plugins_to_notify;
    
    // Find plugins subscribed to this event
    for (const auto& [plugin_name, subscriptions] : event_subscriptions_) {
        if (std::find(subscriptions.begin(), subscriptions.end(), event_type) != subscriptions.end()) {
            plugins_to_notify.push_back(plugin_name);
        }
    }
    
    // Notify all subscribed plugins
    for (const auto& plugin_name : plugins_to_notify) {
        auto it = loaded_plugins_.find(plugin_name);
        if (it != loaded_plugins_.end()) {
            it->second->handleEvent(event_type, data);
        }
    }
    
    return true;
}

bool PluginManager::subscribeToEvent(const std::string& plugin_name, const std::string& event_type) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!isPluginLoaded(plugin_name)) {
        return false;
    }
    
    event_subscriptions_[plugin_name].push_back(event_type);
    return true;
}

bool PluginManager::unsubscribeFromEvent(const std::string& plugin_name, const std::string& event_type) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = event_subscriptions_.find(plugin_name);
    if (it == event_subscriptions_.end()) {
        return false;
    }
    
    auto& subscriptions = it->second;
    subscriptions.erase(std::remove(subscriptions.begin(), subscriptions.end(), event_type),
                       subscriptions.end());
    
    return true;
}

bool PluginManager::allocateResources(const std::string& plugin_name, size_t resources) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = loaded_plugins_.find(plugin_name);
    if (it == loaded_plugins_.end()) {
        return false;
    }
    
    return it->second->allocateResources(resources);
}

bool PluginManager::releaseResources(const std::string& plugin_name, size_t resources) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = loaded_plugins_.find(plugin_name);
    if (it == loaded_plugins_.end()) {
        return false;
    }
    
    return it->second->releaseResources(resources);
}

std::map<std::string, size_t> PluginManager::getResourceUsage() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::map<std::string, size_t> usage;
    for (const auto& [name, plugin] : loaded_plugins_) {
        usage[name] = plugin->getAvailableResources();
    }
    
    return usage;
}

bool PluginManager::configurePlugin(const std::string& plugin_name, const std::map<std::string, std::string>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugin_infos_.find(plugin_name);
    if (it == plugin_infos_.end()) {
        return false;
    }
    
    it->second.configuration = config;
    return true;
}

std::map<std::string, std::string> PluginManager::getPluginConfiguration(const std::string& plugin_name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugin_infos_.find(plugin_name);
    if (it != plugin_infos_.end()) {
        return it->second.configuration;
    }
    
    return std::map<std::string, std::string>();
}

bool PluginManager::addPluginDependency(const std::string& plugin_name, const std::string& dependency) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!isPluginLoaded(plugin_name)) {
        return false;
    }
    
    plugin_dependencies_[plugin_name].push_back(dependency);
    return true;
}

std::vector<std::string> PluginManager::getPluginDependencies(const std::string& plugin_name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugin_dependencies_.find(plugin_name);
    if (it != plugin_dependencies_.end()) {
        return it->second;
    }
    
    return std::vector<std::string>();
}

bool PluginManager::checkPluginDependencies(const std::string& plugin_name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = plugin_dependencies_.find(plugin_name);
    if (it == plugin_dependencies_.end()) {
        return true; // No dependencies
    }
    
    for (const auto& dependency : it->second) {
        if (!isPluginLoaded(dependency)) {
            return false; // Dependency not loaded
        }
    }
    
    return true;
}

bool PluginManager::savePluginState(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    try {
        // Save loaded plugins
        size_t plugin_count = loaded_plugins_.size();
        file.write(reinterpret_cast<const char*>(&plugin_count), sizeof(size_t));
        
        for (const auto& [name, plugin] : loaded_plugins_) {
            size_t name_size = name.size();
            file.write(reinterpret_cast<const char*>(&name_size), sizeof(size_t));
            file.write(name.c_str(), name_size);
            
            // In real implementation, would serialize plugin state
            bool dummy_initialized = plugin->getInfo().name != "";
            file.write(reinterpret_cast<const char*>(&dummy_initialized), sizeof(bool));
        }
        
        // Save plugin infos
        size_t info_count = plugin_infos_.size();
        file.write(reinterpret_cast<const char*>(&info_count), sizeof(size_t));
        
        for (const auto& [name, info] : plugin_infos_) {
            size_t name_size = name.size();
            file.write(reinterpret_cast<const char*>(&name_size), sizeof(size_t));
            file.write(name.c_str(), name_size);
            
            // Serialize plugin info
            size_t desc_size = info.description.size();
            file.write(reinterpret_cast<const char*>(&desc_size), sizeof(size_t));
            file.write(info.description.c_str(), desc_size);
            
            size_t path_size = info.plugin_path.size();
            file.write(reinterpret_cast<const char*>(&path_size), sizeof(size_t));
            file.write(info.plugin_path.c_str(), path_size);
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving plugin state: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

bool PluginManager::loadPluginState(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        return false;
    }

    try {
        // Clear current state
        loaded_plugins_.clear();
        plugin_infos_.clear();
        event_subscriptions_.clear();
        
        // Load loaded plugins
        size_t plugin_count;
        file.read(reinterpret_cast<char*>(&plugin_count), sizeof(size_t));
        
        for (size_t i = 0; i < plugin_count; ++i) {
            size_t name_size;
            file.read(reinterpret_cast<char*>(&name_size), sizeof(size_t));
            std::string name(name_size, '\0');
            file.read(&name[0], name_size);
            
            bool initialized;
            file.read(reinterpret_cast<char*>(&initialized), sizeof(bool));
            
            // In real implementation, would deserialize plugin
            PluginInfo info;
            info.name = name;
            plugin_infos_[name] = info;
        }
        
        // Load plugin infos
        size_t info_count;
        file.read(reinterpret_cast<char*>(&info_count), sizeof(size_t));
        
        for (size_t i = 0; i < info_count; ++i) {
            size_t name_size;
            file.read(reinterpret_cast<char*>(&name_size), sizeof(size_t));
            std::string name(name_size, '\0');
            file.read(&name[0], name_size);
            
            if (!plugin_infos_.count(name)) {
                continue; // Skip if info doesn't exist
            }
            
            PluginInfo& info = plugin_infos_[name];
            
            // Deserialize description
            size_t desc_size;
            file.read(reinterpret_cast<char*>(&desc_size), sizeof(size_t));
            std::string description(desc_size, '\0');
            file.read(&description[0], desc_size);
            info.description = description;
            
            // Deserialize path
            size_t path_size;
            file.read(reinterpret_cast<char*>(&path_size), sizeof(size_t));
            std::string path(path_size, '\0');
            file.read(&path[0], path_size);
            info.plugin_path = path;
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading plugin state: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

bool PluginManager::validatePluginConfiguration(const std::string& plugin_name) const {
    // Simple validation - in real implementation would check specific configuration requirements
    auto it = plugin_infos_.find(plugin_name);
    if (it == plugin_infos_.end()) {
        return false;
    }
    
    return true;
}

std::vector<std::string> PluginManager::getValidationErrors() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> errors;
    
    // Check for plugins with unmet dependencies
    for (const auto& [name, dependencies] : plugin_dependencies_) {
        if (!checkPluginDependencies(name)) {
            errors.push_back("Plugin " + name + " has unmet dependencies");
        }
    }
    
    return errors;
}

} // namespace integration
} // namespace nlm