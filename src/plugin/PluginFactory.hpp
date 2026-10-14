#pragma once

#include "Plugin.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <iostream>

namespace nlm {

// Plugin factory function signature
using PluginCreateFunc = std::function<std::unique_ptr<IPlugin>()>;
using PluginDestroyFunc = std::function<void(IPlugin*)>;

// Plugin entry point for dynamic loading
struct PluginEntry {
    std::string name;
    std::string version;
    std::string description;
    std::vector<std::string> dependencies;
    int priority;
    std::string category;
    PluginCreateFunc create;
    PluginDestroyFunc destroy;
    bool(*validate)();
};

// Plugin factory for creating and managing plugins
class PluginFactory {
public:
    // Type alias for plugin factory function
    using PluginFactoryFunc = std::function<std::unique_ptr<IPlugin>()>;
    
    // Plugin registry entry
    struct PluginRegistryEntry {
        std::string name;
        PluginFactoryFunc factory;
        PluginMetadata metadata;
        int priority;
        std::vector<std::string> dependencies;
        bool enabled;
    };
    
    // Get singleton instance
    static PluginFactory& getInstance() {
        static PluginFactory instance;
        return instance;
    }
    
    // Register a plugin factory
    bool registerPlugin(const PluginRegistryEntry& entry) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Check if plugin already registered
        if (plugins_.find(entry.name) != plugins_.end()) {
            std::cerr << "PluginFactory: Plugin " << entry.name << " already registered!" << std::endl;
            return false;
        }
        
        plugins_[entry.name] = entry;
        std::cout << "PluginFactory: Registered plugin " << entry.name << " (priority: " << entry.priority << ")" << std::endl;
        return true;
    }
    
    // Unregister a plugin
    bool unregisterPlugin(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = plugins_.find(name);
        if (it == plugins_.end()) {
            std::cerr << "PluginFactory: Plugin " << name << " not found!" << std::endl;
            return false;
        }
        
        plugins_.erase(it);
        std::cout << "PluginFactory: Unregistered plugin " << name << std::endl;
        return true;
    }
    
    // Create a plugin instance
    std::unique_ptr<IPlugin> createPlugin(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = plugins_.find(name);
        if (it == plugins_.end()) {
            std::cerr << "PluginFactory: Plugin " << name << " not found!" << std::endl;
            return nullptr;
        }
        
        if (!it->second.enabled) {
            std::cerr << "PluginFactory: Plugin " << name << " is disabled!" << std::endl;
            return nullptr;
        }
        
        try {
            auto plugin = it->second.factory();
            std::cout << "PluginFactory: Created plugin " << name << std::endl;
            return plugin;
        } catch (const std::exception& e) {
            std::cerr << "PluginFactory: Error creating plugin " << name << ": " << e.what() << std::endl;
            return nullptr;
        }
    }
    
    // Create a plugin instance with configuration
    std::unique_ptr<IPlugin> createPlugin(const std::string& name, 
                                          const std::unordered_map<std::string, std::string>& config) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = plugins_.find(name);
        if (it == plugins_.end()) {
            std::cerr << "PluginFactory: Plugin " << name << " not found!" << std::endl;
            return nullptr;
        }
        
        if (!it->second.enabled) {
            std::cerr << "PluginFactory: Plugin " << name << " is disabled!" << std::endl;
            return nullptr;
        }
        
        try {
            auto plugin = it->second.factory();
            if (plugin && dynamic_cast<IConfigurable*>(plugin.get())) {
                auto* configurable = dynamic_cast<IConfigurable*>(plugin.get());
                if (!configurable->configure(config)) {
                    std::cerr << "PluginFactory: Failed to configure plugin " << name << std::endl;
                    return nullptr;
                }
            }
            std::cout << "PluginFactory: Created and configured plugin " << name << std::endl;
            return plugin;
        } catch (const std::exception& e) {
            std::cerr << "PluginFactory: Error creating plugin " << name << ": " << e.what() << std::endl;
            return nullptr;
        }
    }
    
    // Check if plugin is registered
    bool isPluginRegistered(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return plugins_.find(name) != plugins_.end();
    }
    
    // Get all registered plugin names
    std::vector<std::string> getRegisteredPluginNames() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> names;
        names.reserve(plugins_.size());
        for (const auto& pair : plugins_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get plugin metadata
    PluginMetadata getPluginMetadata(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = plugins_.find(name);
        if (it == plugins_.end()) {
            return PluginMetadata{};
        }
        return it->second.metadata;
    }
    
    // Enable/disable a plugin
    bool setPluginEnabled(const std::string& name, bool enabled) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = plugins_.find(name);
        if (it == plugins_.end()) {
            std::cerr << "PluginFactory: Plugin " << name << " not found!" << std::endl;
            return false;
        }
        
        it->second.enabled = enabled;
        std::cout << "PluginFactory: " << (enabled ? "Enabled" : "Disabled") 
                  << " plugin " << name << std::endl;
        return true;
    }
    
    // Get all enabled plugins
    std::vector<std::string> getEnabledPlugins() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> enabledNames;
        for (const auto& pair : plugins_) {
            if (pair.second.enabled) {
                enabledNames.push_back(pair.first);
            }
        }
        return enabledNames;
    }
    
    // Create all enabled plugins
    std::vector<std::unique_ptr<IPlugin>> createAllEnabledPlugins(
        const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& pluginConfigs) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::unique_ptr<IPlugin>> plugins;
        
        for (const auto& pair : plugins_) {
            if (!pair.second.enabled) continue;
            
            const auto& name = pair.first;
            const auto& configIt = pluginConfigs.find(name);
            std::unordered_map<std::string, std::string> config;
            if (configIt != pluginConfigs.end()) {
                config = configIt->second;
            }
            
            auto plugin = createPlugin(name, config);
            if (plugin) {
                plugins.push_back(std::move(plugin));
            }
        }
        
        return plugins;
    }
    
    // Sort plugins by priority and dependencies
    std::vector<std::string> getPluginsInLoadingOrder() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Create list of plugins with their priorities
        struct PriorityPlugin {
            std::string name;
            int priority;
            const PluginRegistryEntry* entry;
        };
        
        std::vector<PriorityPlugin> priorityList;
        priorityList.reserve(plugins_.size());
        
        for (const auto& pair : plugins_) {
            if (!pair.second.enabled) continue;
            priorityList.push_back({pair.first, pair.second.priority, &pair.second});
        }
        
        // Sort by priority (higher priority first)
        std::sort(priorityList.begin(), priorityList.end(),
                  [](const PriorityPlugin& a, const PriorityPlugin& b) {
                      return a.priority > b.priority;
                  });
        
        // For now, return in priority order
        // In a full implementation, this would also respect dependencies
        std::vector<std::string> orderedNames;
        orderedNames.reserve(priorityList.size());
        for (const auto& pp : priorityList) {
            orderedNames.push_back(pp.name);
        }
        
        return orderedNames;
    }
    
    // Clear all plugins
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        plugins_.clear();
        std::cout << "PluginFactory: Cleared all plugins" << std::endl;
    }
    
private:
    PluginFactory() = default;
    PluginFactory(const PluginFactory&) = delete;
    PluginFactory& operator=(const PluginFactory&) = delete;
    
    // Plugin registry
    std::unordered_map<std::string, PluginRegistryEntry> plugins_;
    mutable std::mutex mutex_;
};

// Helper function to register a plugin
template<typename TPlugin>
bool registerPlugin(const std::string& name, 
                   const std::string& version = "1.0.0",
                   const std::string& description = "",
                   const std::string& author = "",
                   const std::vector<std::string>& dependencies = std::vector<std::string>(),
                   int priority = 0,
                   const std::string& category = "Generic",
                   bool enabled = true) {
    PluginFactory::PluginRegistryEntry entry;
    entry.name = name;
    entry.metadata.name = name;
    entry.metadata.version = version;
    entry.metadata.description = description;
    entry.metadata.author = author;
    entry.metadata.dependencies = dependencies;
    entry.metadata.priority = priority;
    entry.metadata.category = category;
    entry.metadata.enabled = enabled;
    
    // Create factory function
    entry.factory = []() -> std::unique_ptr<IPlugin> {
        return std::make_unique<TPlugin>();
    };
    
    entry.priority = priority;
    entry.dependencies = dependencies;
    entry.enabled = enabled;
    
    return PluginFactory::getInstance().registerPlugin(entry);
}

// Plugin registration macro
#define REGISTER_PLUGIN(PluginClass, Name, Version, Description, Author, Category, Priority) \
    namespace { \
        bool _register_##Name##_plugin = registerPlugin<PluginClass>( \
            Name, Version, Description, Author, std::vector<std::string>(), Priority, Category, true \
        ); \
    }

} // namespace nlm