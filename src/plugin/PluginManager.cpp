#include "PluginSystem.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>

namespace nlm {
    class PluginManagerImpl : public PluginManager {
    private:
        std::unordered_map<std::string, std::unique_ptr<Plugin>> activePlugins;
        std::unordered_map<PluginType, std::vector<std::string>> pluginsByType;
        std::shared_ptr<Config> config;
        bool initialized;
        
    public:
        PluginManagerImpl() : initialized(false) {}
        
        bool initialize(const std::unordered_map<std::string, std::string>& configMap) override {
            // Create config object
            config = std::make_shared<Config>();
            for (const auto& pair : configMap) {
                config->set(pair.first, pair.second);
            }
            
            initialized = true;
            NLM_LOG_INFO("Plugin manager initialized with " + std::to_string(configMap.size()) + " configuration entries");
            return true;
        }
        
        bool loadPlugin(const std::string& name) override {
            if (!initialized) {
                NLM_LOG_ERROR("Plugin manager not initialized");
                return false;
            }
            
            if (activePlugins.find(name) != activePlugins.end()) {
                NLM_LOG_INFO("Plugin already loaded: " + name);
                return true;
            }
            
            // Create plugin using registry
            auto plugin = PluginRegistry::createPlugin(name);
            if (!plugin) {
                NLM_LOG_ERROR("Failed to create plugin: " + name);
                return false;
            }
            
            // Get plugin type for categorization
            PluginType pluginType = PluginRegistry::getPluginType(name);
            
            // Initialize plugin with config
            std::unordered_map<std::string, std::string> pluginConfig;
            if (config) {
                // Extract plugin-specific configuration
                std::string prefix = name + ".";
                for (const auto& entry : config->getKeys()) {
                    if (entry.find(prefix) == 0) {
                        std::string key = entry.substr(prefix.length());
                        if (auto value = config->getOr<std::string>(entry, "")) {
                            pluginConfig[key] = *value;
                        }
                    }
                }
            }
            
            if (!plugin->initialize(pluginConfig)) {
                NLM_LOG_ERROR("Failed to initialize plugin: " + name);
                return false;
            }
            
            // Add to active plugins
            activePlugins[name] = std::move(plugin);
            pluginsByType[pluginType].push_back(name);
            
            NLM_LOG_INFO("Plugin loaded successfully: " + name + " (type: " + std::to_string(static_cast<int>(pluginType)) + ")");
            return true;
        }
        
        bool unloadPlugin(const std::string& name) override {
            auto it = activePlugins.find(name);
            if (it == activePlugins.end()) {
                NLM_LOG_INFO("Plugin not loaded: " + name);
                return false;
            }
            
            // Shutdown plugin
            it->second->shutdown();
            
            // Remove from categorization
            PluginType pluginType = PluginRegistry::getPluginType(name);
            auto& typeList = pluginsByType[pluginType];
            typeList.erase(std::remove(typeList.begin(), typeList.end(), name), typeList.end());
            if (typeList.empty()) {
                pluginsByType.erase(pluginType);
            }
            
            // Remove from active plugins
            activePlugins.erase(it);
            
            NLM_LOG_INFO("Plugin unloaded successfully: " + name);
            return true;
        }
        
        void update(double timestep) override {
            if (!initialized) return;
            
            for (auto& pair : activePlugins) {
                try {
                    pair.second->update(timestep);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error updating plugin " + pair.first + ": " + e.what());
                }
            }
        }
        
        void shutdown() override {
            if (!initialized) return;
            
            for (auto& pair : activePlugins) {
                try {
                    pair.second->shutdown();
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error shutting down plugin " + pair.first + ": " + e.what());
                }
            }
            
            activePlugins.clear();
            pluginsByType.clear();
            initialized = false;
            
            NLM_LOG_INFO("Plugin manager shut down");
        }
        
        Plugin* getPlugin(const std::string& name) override {
            auto it = activePlugins.find(name);
            return it != activePlugins.end() ? it->second.get() : nullptr;
        }
        
        std::vector<Plugin*> getAllPlugins() override {
            std::vector<Plugin*> result;
            result.reserve(activePlugins.size());
            for (auto& pair : activePlugins) {
                result.push_back(pair.second.get());
            }
            return result;
        }
        
        std::vector<Plugin*> getPluginsByType(PluginType type) override {
            std::vector<Plugin*> result;
            
            auto it = pluginsByType.find(type);
            if (it == pluginsByType.end()) return result;
            
            for (const std::string& name : it->second) {
                auto pluginIt = activePlugins.find(name);
                if (pluginIt != activePlugins.end()) {
                    result.push_back(pluginIt->second.get());
                }
            }
            
            return result;
        }
        
        bool isPluginLoaded(const std::string& name) override {
            return activePlugins.find(name) != activePlugins.end();
        }
        
        std::vector<std::string> getLoadedPlugins() override {
            std::vector<std::string> result;
            result.reserve(activePlugins.size());
            for (const auto& pair : activePlugins) {
                result.push_back(pair.first);
            }
            return result;
        }
    };
    
    namespace {
        PluginManagerImpl& getPluginManagerInstance() {
            static PluginManagerImpl instance;
            return instance;
        }
    }
    
    bool PluginManager::initialize(const std::unordered_map<std::string, std::string>& config) {
        return getPluginManagerInstance().initialize(config);
    }
    
    bool PluginManager::loadPlugin(const std::string& name) {
        return getPluginManagerInstance().loadPlugin(name);
    }
    
    bool PluginManager::unloadPlugin(const std::string& name) {
        return getPluginManagerInstance().unloadPlugin(name);
    }
    
    void PluginManager::update(double timestep) {
        getPluginManagerInstance().update(timestep);
    }
    
    void PluginManager::shutdown() {
        getPluginManagerInstance().shutdown();
    }
    
    Plugin* PluginManager::getPlugin(const std::string& name) {
        return getPluginManagerInstance().getPlugin(name);
    }
    
    std::vector<Plugin*> PluginManager::getAllPlugins() {
        return getPluginManagerInstance().getAllPlugins();
    }
    
    std::vector<Plugin*> PluginManager::getPluginsByType(PluginType type) {
        return getPluginManagerInstance().getPluginsByType(type);
    }
    
    bool PluginManager::isPluginLoaded(const std::string& name) {
        return getPluginManagerInstance().isPluginLoaded(name);
    }
    
    std::vector<std::string> PluginManager::getLoadedPlugins() {
        return getPluginManagerInstance().getLoadedPlugins();
    }
} // namespace nlm