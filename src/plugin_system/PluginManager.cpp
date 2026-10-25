// Plugin manager implementation
#include "Plugin.hpp"
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// Default plugin manager implementation
class DefaultPluginManager : public IPluginManager {
private:
    std::map<std::string, std::shared_ptr<IPlugin>> plugins_;
    std::map<std::string, PluginMetadata> pluginMetadata_;
    std::map<std::string, std::filesystem::path> pluginPaths_;
    std::map<std::string, std::vector<std::string>> pluginDependencies_;
    
    std::mutex mutex_;
    std::shared_mutex readWriteMutex_;
    
    IPluginManager* globalInstance_;
    
    // Event system
    std::unordered_map<PluginEventType, std::vector<std::function<bool(const PluginEventData&)>>> eventListeners_;
    
    // Configuration
    std::string globalConfiguration_;
    bool hotReloadingEnabled_ = false;
    std::chrono::steady_clock::time_point lastUpdateCheck_;
    
    // Statistics
    struct PluginStats {
        size_t loadCount = 0;
        size_t unloadCount = 0;
        size_t errorCount = 0;
        std::chrono::steady_clock::time_point firstLoadTime;
        std::chrono::steady_clock::time_point lastLoadTime;
        std::chrono::nanoseconds totalLoadTime = std::chrono::nanoseconds(0);
    };
    
    std::map<std::string, PluginStats> stats_;
    
    // Plugin repositories
    std::map<std::string, std::filesystem::path> repositories_;
    
    // Security
    std::map<std::string, std::vector<std::string>> securityPolicies_;
    
    // Lifecycle callbacks
    std::function<void(const std::string&)> loadCallback_;
    std::function<void(const std::string&)> unloadCallback_;
    std::function<void(const std::string&, const std::string&)> errorCallback_;
    std::function<void(const std::string&, PluginState, PluginState)> statusChangeCallback_;
    
public:
    DefaultPluginManager() : globalInstance_(this) {
        lastUpdateCheck_ = std::chrono::steady_clock::now();
    }
    
    ~DefaultPluginManager() {
        shutdownAllPlugins();
    }
    
    // Plugin discovery
    std::vector<PluginMetadata> discoverPlugins(const std::filesystem::path& directory) override {
        std::vector<PluginMetadata> discoveredPlugins;
        
        if (!std::filesystem::exists(directory)) {
            return discoveredPlugins;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string extension = entry.path().extension().string();
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                
                if (extension == ".dll" || extension == ".so" || extension == ".dylib" || extension == ".plugin") {
                    try {
                        PluginMetadata metadata = loadPluginMetadata(entry.path());
                        discoveredPlugins.push_back(metadata);
                    } catch (const std::exception& e) {
                        if (errorCallback_) {
                            errorCallback_("Plugin discovery failed", std::string(e.what()));
                        }
                    }
                }
            }
        }
        
        return discoveredPlugins;
    }
    
    bool loadPlugin(const std::string& pluginName) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        if (plugins_.find(pluginName) != plugins_.end()) {
            return true; // Already loaded
        }
        
        // Find plugin in available plugins
        auto metadataIt = pluginMetadata_.find(pluginName);
        if (metadataIt == pluginMetadata_.end()) {
            return false;
        }
        
        const PluginMetadata& metadata = metadataIt->second;
        auto pathIt = pluginPaths_.find(pluginName);
        if (pathIt == pluginPaths_.end()) {
            return false;
        }
        
        return loadPluginFromFile(pathIt->second);
    }
    
    bool unloadPlugin(const std::string& pluginName) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = plugins_.find(pluginName);
        if (it == plugins_.end()) {
            return false;
        }
        
        auto metadataIt = pluginMetadata_.find(pluginName);
        if (metadataIt == pluginMetadata_.end()) {
            return false;
        }
        
        PluginMetadata& metadata = metadataIt->second;
        
        try {
            // Deactivate plugin first
            if (it->second->getState() == PluginState::Active || 
                it->second->getState() == PluginState::Running) {
                it->second->deactivate();
            }
            
            // Shutdown plugin
            it->second->shutdown();
            
            // Remove from maps
            plugins_.erase(it);
            pluginMetadata_.erase(metadataIt);
            pluginPaths_.erase(pluginName);
            
            // Update statistics
            stats_[pluginName].unloadCount++;
            
            if (unloadCallback_) {
                unloadCallback_(pluginName);
            }
            
            emitPluginEvent(PluginEventImpl::createPluginEvent(
                PluginEventType::PluginUnload, "system",
                {{ "plugin_name", pluginName }})
            );
            
            return true;
            
        } catch (const std::exception& e) {
            if (errorCallback_) {
                errorCallback_(pluginName, std::string(e.what()));
            }
            return false;
        }
    }
    
    bool loadPluginFromFile(const std::filesystem::path& filepath) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        // Check if plugin already loaded
        std::string pluginName = extractPluginName(filepath);
        if (plugins_.find(pluginName) != plugins_.end()) {
            return true;
        }
        
        try {
            // Load plugin metadata
            PluginMetadata metadata = loadPluginMetadata(filepath);
            
            // Create plugin instance
            std::shared_ptr<IPlugin> plugin = createPluginInstance(filepath);
            if (!plugin) {
                return false;
            }
            
            // Set plugin manager reference
            plugin->setPluginManager(this);
            
            // Load the plugin
            if (!plugin->load()) {
                errorCallback_(pluginName, plugin->getError());
                return false;
            }
            
            // Store plugin
            plugins_[pluginName] = plugin;
            pluginMetadata_[pluginName] = metadata;
            pluginPaths_[pluginName] = filepath;
            
            // Initialize statistics
            stats_[pluginName].loadCount++;
            stats_[pluginName].firstLoadTime = std::chrono::steady_clock::now();
            stats_[pluginName].lastLoadTime = stats_[pluginName].firstLoadTime;
            
            if (loadCallback_) {
                loadCallback_(pluginName);
            }
            
            emitPluginEvent(PluginEventImpl::createPluginEvent(
                PluginEventType::PluginLoad, "system",
                {{ "plugin_name", pluginName }})
            );
            
            return true;
            
        } catch (const std::exception& e) {
            if (errorCallback_) {
                errorCallback_("Plugin load failed", std::string(e.what()));
            }
            return false;
        }
    }
    
    // Plugin lifecycle management
    bool initializePlugin(const std::string& pluginName) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = plugins_.find(pluginName);
        if (it == plugins_.end()) {
            return false;
        }
        
        if (!it->second->initialize()) {
            errorCallback_(pluginName, it->second->getError());
            return false;
        }
        
        emitPluginEvent(PluginEventImpl::createPluginEvent(
            PluginEventType::PluginInitialize, "system",
            {{ "plugin_name", pluginName }})
        );
        
        return true;
    }
    
    bool configurePlugin(const std::string& pluginName, const std::string& configData) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = plugins_.find(pluginName);
        if (it == plugins_.end()) {
            return false;
        }
        
        if (!it->second->configure(configData)) {
            errorCallback_(pluginName, it->second->getError());
            return false;
        }
        
        emitPluginEvent(PluginEventImpl::createPluginEvent(
            PluginEventType::PluginConfigure, "system",
            {{ "plugin_name", pluginName, "config_data", configData }})
        );
        
        return true;
    }
    
    bool activatePlugin(const std::string& pluginName) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = plugins_.find(pluginName);
        if (it == plugins_.end()) {
            return false;
        }
        
        if (!it->second->activate()) {
            errorCallback_(pluginName, it->second->getError());
            return false;
        }
        
        emitPluginEvent(PluginEventImpl::createPluginEvent(
            PluginEventType::PluginActivate, "system",
            {{ "plugin_name", pluginName }})
        );
        
        return true;
    }
    
    bool deactivatePlugin(const std::string& pluginName) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = plugins_.find(pluginName);
        if (it == plugins_.end()) {
            return false;
        }
        
        if (!it->second->deactivate()) {
            errorCallback_(pluginName, it->second->getError());
            return false;
        }
        
        return true;
    }
    
    bool shutdownPlugin(const std::string& pluginName) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = plugins_.find(pluginName);
        if (it == plugins_.end()) {
            return false;
        }
        
        if (!it->second->shutdown()) {
            errorCallback_(pluginName, it->second->getError());
            return false;
        }
        
        emitPluginEvent(PluginEventImpl::createPluginEvent(
            PluginEventType::PluginUnload, "system",
            {{ "plugin_name", pluginName }})
        );
        
        plugins_.erase(it);
        pluginMetadata_.erase(pluginName);
        pluginPaths_.erase(pluginName);
        stats_.erase(pluginName);
        
        if (unloadCallback_) {
            unloadCallback_(pluginName);
        }
        
        return true;
    }
    
    // Plugin state management
    PluginState getPluginState(const std::string& pluginName) const override {
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = plugins_.find(pluginName);
        if (it == plugins_.end()) {
            return PluginState::Unloaded;
        }
        
        return it->second->getState();
    }
    
    std::vector<std::string> getLoadedPlugins() const override {
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        
        std::vector<std::string> loadedPlugins;
        for (const auto& pair : plugins_) {
            loadedPlugins.push_back(pair.first);
        }
        return loadedPlugins;
    }
    
    bool isPluginLoaded(const std::string& pluginName) const override {
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        return plugins_.find(pluginName) != plugins_.end();
    }
    
    bool isPluginActive(const std::string& pluginName) const override {
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = plugins_.find(pluginName);
        if (it == plugins_.end()) {
            return false;
        }
        
        PluginState state = it->second->getState();
        return state == PluginState::Active || state == PluginState::Running;
    }
    
    // Plugin information
    const PluginMetadata& getPluginMetadata(const std::string& pluginName) const override {
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = pluginMetadata_.find(pluginName);
        if (it == pluginMetadata_.end()) {
            throw std::runtime_error("Plugin not found: " + pluginName);
        }
        
        return it->second;
    }
    
    std::vector<std::string> getPluginDependencies(const std::string& pluginName) const override {
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = pluginDependencies_.find(pluginName);
        if (it == pluginDependencies_.end()) {
            return {};
        }
        
        return it->second;
    }
    
    bool areDependenciesSatisfied(const std::string& pluginName) const override {
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        
        auto depsIt = pluginDependencies_.find(pluginName);
        if (depsIt == pluginDependencies_.end()) {
            return true;
        }
        
        for (const auto& dep : depsIt->second) {
            if (!isPluginLoaded(dep) || !isPluginActive(dep)) {
                return false;
            }
        }
        
        return true;
    }
    
    // Event system
    bool addEventListener(PluginEventType eventType,
                         std::function<bool(const PluginEventData&)> callback) override {
        std::lock_guard<std::mutex> lock(mutex_);
        eventListeners_[eventType].push_back(callback);
        return true;
    }
    
    bool removeEventListener(PluginEventType eventType,
                            std::function<bool(const PluginEventData&)> callback) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = eventListeners_.find(eventType);
        if (it == eventListeners_.end()) {
            return false;
        }
        
        it->second.erase(
            std::remove(it->second.begin(), it->second.end(), callback),
            it->second.end()
        );
        
        return true;
    }
    
    void emitEvent(const PluginEventData& event) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = eventListeners_.find(event.type);
        if (it != eventListeners_.end()) {
            for (auto& callback : it->second) {
                try {
                    callback(event);
                } catch (const std::exception& e) {
                    if (errorCallback_) {
                        errorCallback_("Event handler error", std::string(e.what()));
                    }
                }
            }
        }
    }
    
    void emitPluginEvent(std::shared_ptr<PluginEvent> event) {
        if (!event) return;
        
        PluginEventData data;
        data.type = event->getType();
        data.source = event->getSource();
        data.timestamp = event->getTimestamp();
        data.event = event;
        
        emitEvent(data);
    }
    
    // Configuration management
    bool setGlobalConfiguration(const std::string& configData) override {
        globalConfiguration_ = configData;
        
        // Configure all loaded plugins
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        for (auto& pair : plugins_) {
            pair.second->configure(configData);
        }
        
        return true;
    }
    
    std::string getGlobalConfiguration() const override {
        return globalConfiguration_;
    }
    
    bool validateGlobalConfiguration(const std::string& configData) const override {
        // Basic validation - could be enhanced
        return !configData.empty();
    }
    
    // Performance monitoring
    double getSystemCPUUsage() const override {
        double totalUsage = 0.0;
        size_t count = 0;
        
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        for (const auto& pair : plugins_) {
            totalUsage += pair.second->getCPUUsage();
            count++;
        }
        
        return count > 0 ? totalUsage / count : 0.0;
    }
    
    size_t getSystemMemoryUsage() const override {
        size_t totalUsage = 0;
        
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        for (const auto& pair : plugins_) {
            totalUsage += pair.second->getMemoryUsage();
        }
        
        return totalUsage;
    }
    
    size_t getActivePluginCount() const override {
        std::shared_lock<std::shared_mutex> lock(readWriteMutex_);
        size_t count = 0;
        for (const auto& pair : plugins_) {
            if (isPluginActive(pair.first)) {
                count++;
            }
        }
        return count;
    }
    
    std::chrono::nanoseconds getSystemUptime() const override {
        return std::chrono::steady_clock::now() - lastUpdateCheck_;
    }
    
    // Plugin management
    bool disablePlugin(const std::string& pluginName) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = plugins_.find(pluginName);
        if (it == plugins_.end()) {
            return false;
        }
        
        it->second->deactivate();
        return true;
    }
    
    bool enablePlugin(const std::string& pluginName) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        auto it = plugins_.find(pluginName);
        if (it == plugins_.end()) {
            return false;
        }
        
        if (!areDependenciesSatisfied(pluginName)) {
            return false;
        }
        
        it->second->activate();
        return true;
    }
    
    bool restartPlugin(const std::string& pluginName) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        if (!unloadPlugin(pluginName)) {
            return false;
        }
        
        // Note: This assumes the plugin is still in the path
        // In a real implementation, you might need to track this
        return true;
    }
    
    bool updatePlugin(const std::string& pluginName, const std::filesystem::path& newFilepath) override {
        std::lock_guard<std::shared_mutex> lock(readWriteMutex_);
        
        if (!unloadPlugin(pluginName)) {
            return false;
        }
        
        return loadPluginFromFile(newFilepath);
    }
    
    // Batch operations
    bool loadPlugins(const std::vector<std::string>& pluginNames) override {
        bool allSuccess = true;
        for (const auto& name : pluginNames) {
            if (!loadPlugin(name)) {
                allSuccess = false;
            }
        }
        return allSuccess;
    }
    
    bool unloadPlugins(const std::vector<std::string>& pluginNames) override {
        bool allSuccess = true;
        for (const auto& name : pluginNames) {
            if (!unloadPlugin(name)) {
                allSuccess = false;
            }
        }
        return allSuccess;
    }
    
    bool activatePlugins(const std::vector<std::string>& pluginNames) override {
        bool allSuccess = true;
        for (const auto& name : pluginNames) {
            if (!activatePlugin(name)) {
                allSuccess = false;
            }
        }
        return allSuccess;
    }
    
    bool deactivatePlugins(const std::vector<std::string>& pluginNames) override {
        bool allSuccess = true;
        for (const auto& name : pluginNames) {
            if (!deactivatePlugin(name)) {
                allSuccess = false;
            }
        }
        return allSuccess;
    }
    
    // Plugin repository and marketplace
    bool addPluginRepository(const std::string& name, const std::filesystem::path& path) override {
        std::lock_guard<std::mutex> lock(mutex_);
        repositories_[name] = path;
        return true;
    }
    
    bool removePluginRepository(const std::string& name) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = repositories_.find(name);
        if (it == repositories_.end()) {
            return false;
        }
        repositories_.erase(it);
        return true;
    }
    
    std::vector<PluginMetadata> searchPlugins(const std::string& query) override {
        std::vector<PluginMetadata> results;
        
        for (const auto& repo : repositories_) {
            std::vector<PluginMetadata> repoResults = discoverPlugins(repo.second);
            
            for (const auto& metadata : repoResults) {
                if (metadata.name.find(query) != std::string::npos ||
                    metadata.description.find(query) != std::string::npos) {
                    results.push_back(metadata);
                }
            }
        }
        
        return results;
    }
    
    bool installPlugin(const std::string& pluginName) override {
        // Implementation depends on repository system
        return false;
    }
    
    bool uninstallPlugin(const std::string& pluginName) override {
        return unloadPlugin(pluginName);
    }
    
    // Security and sandboxing
    bool setPluginSecurityPolicy(const std::string& pluginName, const std::vector<std::string>& allowedActions) override {
        std::lock_guard<std::mutex> lock(mutex_);
        securityPolicies_[pluginName] = allowedActions;
        
        auto pluginIt = plugins_.find(pluginName);
        if (pluginIt != plugins_.end()) {
            // In a real implementation, you would update the plugin's security policy
        }
        
        return true;
    }
    
    std::vector<std::string> getPluginSecurityPolicy(const std::string& pluginName) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = securityPolicies_.find(pluginName);
        if (it == securityPolicies_.end()) {
            return {};
        }
        
        return it->second;
    }
    
    bool isPluginSandboxed(const std::string& pluginName) const override {
        auto it = pluginMetadata_.find(pluginName);
        if (it == pluginMetadata_.end()) {
            return false;
        }
        
        return it->second.sandboxed;
    }
    
    // Hot reloading
    bool enableHotReloading(bool enabled) override {
        hotReloadingEnabled_ = enabled;
        return true;
    }
    
    bool isHotReloadingEnabled() const override {
        return hotReloadingEnabled_;
    }
    
    bool checkForPluginUpdates() override {
        // Check for plugin updates from repositories
        // This would involve comparing checksums or versions
        return true;
    }
    
    bool reloadPlugin(const std::string& pluginName) override {
        if (!hotReloadingEnabled_) {
            return false;
        }
        
        // Unload and reload plugin
        return updatePlugin(pluginName, pluginPaths_[pluginName]);
    }
    
    // Diagnostics and debugging
    std::string getSystemStatus() const override {
        std::ostringstream oss;
        oss << "Plugin System Status:" << std::endl;
        oss << "  Loaded plugins: " << plugins_.size() << std::endl;
        oss << "  Active plugins: " << getActivePluginCount() << std::endl;
        oss << "  System CPU usage: " << getSystemCPUUsage() << "%" << std::endl;
        oss << "  System memory usage: " << getSystemMemoryUsage() << " bytes" << std::endl;
        oss << "  Uptime: " << std::chrono::duration_cast<std::chrono::seconds>(getSystemUptime()).count() << " seconds" << std::endl;
        
        return oss.str();
    }
    
    std::string getPluginStatus(const std::string& pluginName) const override {
        std::ostringstream oss;
        
        auto it = plugins_.find(pluginName);
        if (it == plugins_.end()) {
            return "Plugin not loaded";
        }
        
        oss << "Plugin: " << pluginName << std::endl;
        oss << "  State: " << pluginStateToString(it->second->getState()) << std::endl;
        oss << "  Version: " << it->second->getMetadata().version << std::endl;
        oss << "  CPU Usage: " << it->second->getCPUUsage() << "%" << std::endl;
        oss << "  Memory Usage: " << it->second->getMemoryUsage() << " bytes" << std::endl;
        oss << "  Dependencies: ";
        for (size_t i = 0; i < it->second->getDependencies().size(); ++i) {
            if (i > 0) oss << ", ";
            oss << it->second->getDependencies()[i];
        }
        oss << std::endl;
        oss << "  Error: " << (it->second->hasError() ? it->second->getError() : "None") << std::endl;
        
        return oss.str();
    }
    
    std::vector<std::string> getPluginErrors(const std::string& pluginName) const override {
        auto it = plugins_.find(pluginName);
        if (it == plugins_.end()) {
            return {};
        }
        
        std::vector<std::string> errors;
        if (it->second->hasError()) {
            errors.push_back(it->second->getError());
        }
        return errors;
    }
    
    void clearPluginErrors(const std::string& pluginName) override {
        auto it = plugins_.find(pluginName);
        if (it != plugins_.end()) {
            it->second->clearError();
        }
    }
    
    // Statistics
    size_t getPluginLoadCount(const std::string& pluginName) const override {
        auto it = stats_.find(pluginName);
        if (it == stats_.end()) {
            return 0;
        }
        return it->second.loadCount;
    }
    
    size_t getPluginUnloadCount(const std::string& pluginName) const override {
        auto it = stats_.find(pluginName);
        if (it == stats_.end()) {
            return 0;
        }
        return it->second.unloadCount;
    }
    
    double getPluginSuccessRate(const std::string& pluginName) const override {
        auto it = stats_.find(pluginName);
        if (it == stats_.end()) {
            return 0.0;
        }
        
        size_t totalOperations = it->second.loadCount + it->second.unloadCount;
        if (totalOperations == 0) {
            return 0.0;
        }
        
        return 100.0 * (it->second.loadCount) / totalOperations;
    }
    
    std::chrono::nanoseconds getAveragePluginLoadTime(const std::string& pluginName) const override {
        auto it = stats_.find(pluginName);
        if (it == stats_.end()) {
            return std::chrono::nanoseconds(0);
        }
        
        return it->second.totalLoadTime / it->second.loadCount;
    }
    
    // Lifecycle callbacks
    void setLoadCallback(std::function<void(const std::string&)> callback) override {
        loadCallback_ = callback;
    }
    
    void setUnloadCallback(std::function<void(const std::string&)> callback) override {
        unloadCallback_ = callback;
    }
    
    void setErrorCallback(std::function<void(const std::string&, const std::string&)> callback) override {
        errorCallback_ = callback;
    }
    
    void setStatusChangeCallback(std::function<void(const std::string&, PluginState, PluginState)> callback) override {
        statusChangeCallback_ = callback;
    }
    
    // Helper methods
    PluginMetadata loadPluginMetadata(const std::filesystem::path& filepath) {
        PluginMetadata metadata;
        
        // This would involve loading metadata from the plugin file
        // For now, we'll create a basic metadata
        metadata.filePath = filepath;
        metadata.directory = filepath.parent_path();
        metadata.lastModified = std::filesystem::last_write_time(filepath);
        
        // Try to extract metadata from file (in real implementation)
        // This is a placeholder
        
        return metadata;
    }
    
    std::shared_ptr<IPlugin> createPluginInstance(const std::filesystem::path& filepath) {
        // This would involve loading the plugin from the file
        // For now, we'll return a null pointer
        // In a real implementation, you would use dynamic loading
        
        // Placeholder implementation
        return nullptr;
    }
    
    std::string extractPluginName(const std::filesystem::path& filepath) {
        return filepath.filename().string();
    }
    
    void shutdownAllPlugins() {
        std::vector<std::string> pluginNames = getLoadedPlugins();
        for (const auto& name : pluginNames) {
            unloadPlugin(name);
        }
    }
    
    IPluginManager* getGlobalInstance() const {
        return globalInstance_;
    }
};

// Global plugin manager instance
IPluginManager& getGlobalPluginManager() {
    static DefaultPluginManager manager;
    return manager;
}

} // namespace nlm
