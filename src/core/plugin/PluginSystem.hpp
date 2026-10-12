// Plugin System Factory and Registry Headers
// Provides factory functions and registry for plugin management

#pragma once

#include "IPlugin.hpp"
#include <memory>
#include <functional>
#include <string>

namespace nlm {

// Plugin factory registration macros
#define REGISTER_PLUGIN(name, version, type, description, author, factory_func) \
    namespace { \
        struct PluginRegistrar { \
            PluginRegistrar() { \
                PluginMetadata metadata; \
                metadata.name = name; \
                metadata.version = version; \
                metadata.type = type; \
                metadata.description = description; \
                metadata.author = author; \
                metadata.minSystemVersion = NLM_PLUGIN_VERSION_MAJOR "." NLM_PLUGIN_VERSION_MINOR ".0"; \
                metadata.isBuiltIn = false; \
                getRegistry().registerBuiltInPlugin(metadata, factory_func); \
            } \
        }; \
        static PluginRegistrar pluginRegistrarInstance; \
    }

#define REGISTER_COMPONENT(name, category, type, default_config) \
    namespace { \
        struct ComponentRegistrar { \
            ComponentRegistrar() { \
                ComponentInfo info; \
                info.name = name; \
                info.category = category; \
                info.type = type; \
                info.defaultConfig = default_config; \
                info.isBuiltIn = true; \
                getRegistry().registerBuiltInComponent(info); \
            } \
        }; \
        static ComponentRegistrar componentRegistrarInstance; \
    }

// Plugin factory function type
using PluginFactoryFunc = std::function<std::unique_ptr<IPlugin>()>;

// Component factory function type
using ComponentFactoryFunc = std::function<std::unique_ptr<IComponent>()>;

// Plugin metadata structure
struct PluginMetadata {
    std::string name;
    std::string version;
    PluginType type;
    std::string description;
    std::string author;
    std::string contact;
    std::vector<std::string> dependencies;
    std::vector<std::string> optionalDependencies;
    std::string minSystemVersion;
    PluginState state;
    std::unordered_map<std::string, std::string> configuration;
    double loadTime;
    std::unordered_map<std::string, double> metrics;
    bool isBuiltIn;
    std::string filePath;
    std::string className;
};

// Component metadata structure
struct ComponentMetadata {
    std::string name;
    std::string category;
    std::type_index type;
    std::unordered_map<std::string, std::string> defaultConfig;
    bool isBuiltIn;
};

// Plugin registry interface (simplified version)
class IPluginRegistry {
public:
    virtual ~IPluginRegistry() = default;
    
    // Register plugin
    virtual bool registerPlugin(const PluginMetadata& metadata, PluginFactoryFunc factory) = 0;
    
    // Register component
    virtual bool registerComponent(const ComponentMetadata& metadata, ComponentFactoryFunc factory) = 0;
    
    // Get plugin by name
    virtual std::optional<PluginMetadata> getPlugin(const std::string& name) const = 0;
    
    // Get component by name
    virtual std::optional<ComponentMetadata> getComponent(const std::string& name) const = 0;
    
    // Check if plugin is registered
    virtual bool isPluginRegistered(const std::string& name) const = 0;
    
    // Check if component is registered
    virtual bool isComponentRegistered(const std::string& name) const = 0;
    
    // Get all plugins
    virtual std::vector<PluginMetadata> getAllPlugins() const = 0;
    
    // Get all components
    virtual std::vector<ComponentMetadata> getAllComponents() const = 0;
};

// Global registry instance
IPluginRegistry& getRegistry();

// Plugin registration helper functions
void registerBuiltInPlugins();
void registerBuiltInComponents();

// Plugin discovery function
bool discoverPlugins(const std::string& directory);

} // namespace nlm

// Plugin main header - includes all plugin system components
#pragma once

#include "IPlugin.hpp"
#include "PluginFactory.hpp"
#include "PluginRegistry.hpp"
#include "PluginLoader.hpp"
#include "PluginManager.hpp"

namespace nlm {

// Convenience functions for plugin system
inline IPluginManager& getPluginManager() {
    return nlm::getPluginManager();
}

inline void initializePluginSystem() {
    nlm::initializePluginSystem();
}

inline bool isPluginSystemInitialized() {
    return nlm::globalPluginManager_ != nullptr;
}

inline void shutdownPluginSystem() {
    if (nlm::globalPluginManager_) {
        nlm::globalPluginManager_->shutdownAll();
        nlm::globalPluginManager_.reset();
    }
}

} // namespace nlm

// Plugin system version
#define NLM_PLUGIN_SYSTEM_VERSION "1.0.0"

// Plugin system features
#define NLM_PLUGIN_SUPPORTS_DYNAMIC_LOADING true
#define NLM_PLUGIN_SUPPORTS_COMPONENT_FACTORIES true
#define NLM_PLUGIN_SUPPORTS_EVENT_SYSTEM true
#define NLM_PLUGIN_SUPPORTS_METRIC_COLLECTION true
#define NLM_PLUGIN_SUPPORTS_HEALTH_CHECKS true