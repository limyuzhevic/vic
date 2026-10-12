#include "PluginSystem.hpp"
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <optional>
#include <string>

namespace nlm {

// Default plugin registry implementation
class DefaultPluginRegistry : public IPluginRegistry {
public:
    DefaultPluginRegistry() {
        // Register built-in components
        registerBuiltInComponents();
    }
    
    bool registerPlugin(const PluginMetadata& metadata, PluginFactoryFunc factory) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (plugins_.find(metadata.name) != plugins_.end()) {
            return false; // Already registered
        }
        
        plugins_[metadata.name] = metadata;
        factories_[metadata.name] = factory;
        
        return true;
    }
    
    bool registerComponent(const ComponentMetadata& metadata, ComponentFactoryFunc factory) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (components_.find(metadata.name) != components_.end()) {
            return false; // Already registered
        }
        
        components_[metadata.name] = metadata;
        componentFactories_[metadata.name] = factory;
        
        return true;
    }
    
    std::optional<PluginMetadata> getPlugin(const std::string& name) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = plugins_.find(name);
        if (it == plugins_.end()) {
            return std::nullopt;
        }
        
        return it->second;
    }
    
    std::optional<ComponentMetadata> getComponent(const std::string& name) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = components_.find(name);
        if (it == components_.end()) {
            return std::nullopt;
        }
        
        return it->second;
    }
    
    bool isPluginRegistered(const std::string& name) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return plugins_.find(name) != plugins_.end();
    }
    
    bool isComponentRegistered(const std::string& name) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return components_.find(name) != components_.end();
    }
    
    std::vector<PluginMetadata> getAllPlugins() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<PluginMetadata> plugins;
        plugins.reserve(plugins_.size());
        
        for (const auto& pair : plugins_) {
            plugins.push_back(pair.second);
        }
        
        return plugins;
    }
    
    std::vector<ComponentMetadata> getAllComponents() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<ComponentMetadata> components;
        components.reserve(components_.size());
        
        for (const auto& pair : components_) {
            components.push_back(pair.second);
        }
        
        return components;
    }
    
    // Create component from type index
    std::optional<std::unique_ptr<IComponent>> createComponent(const std::type_index& type) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (const auto& pair : components_) {
            if (pair.second.type == type) {
                if (componentFactories_.find(pair.first) != componentFactories_.end()) {
                    auto component = componentFactories_.at(pair.first)();
                    return std::make_optional(std::move(component));
                }
            }
        }
        
        return std::nullopt;
    }
    
    // Get component by type
    std::optional<ComponentMetadata> getComponentByType(const std::type_index& type) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (const auto& pair : components_) {
            if (pair.second.type == type) {
                return pair.second;
            }
        }
        
        return std::nullopt;
    }
    
private:
    void registerBuiltInComponents() {
        // Register built-in components for the NLM system
        // This includes all the core components from the existing codebase
        
        // Neuron component
        ComponentMetadata neuronInfo;
        neuronInfo.name = "neuron";
        neuronInfo.category = "brain";
        neuronInfo.type = std::type_index(typeid(class Neuron));
        neuronInfo.isBuiltIn = true;
        
        // Synapse component
        ComponentMetadata synapseInfo;
        synapseInfo.name = "synapse";
        synapseInfo.category = "brain";
        synapseInfo.type = std::type_index(typeid(class Synapse));
        synapseInfo.isBuiltIn = true;
        
        // Brain component
        ComponentMetadata brainInfo;
        brainInfo.name = "brain";
        brainInfo.category = "brain";
        brainInfo.type = std::type_index(typeid(class Brain));
        brainInfo.isBuiltIn = true;
        
        // Register components
        registerComponent(neuronInfo, []() -> std::unique_ptr<IComponent> {
            // Would create actual Neuron instance
            return nullptr;
        });
        
        registerComponent(synapseInfo, []() -> std::unique_ptr<IComponent> {
            // Would create actual Synapse instance
            return nullptr;
        });
        
        registerComponent(brainInfo, []() -> std::unique_ptr<IComponent> {
            // Would create actual Brain instance
            return nullptr;
        });
    }
    
    std::mutex mutex_;
    std::unordered_map<std::string, PluginMetadata> plugins_;
    std::unordered_map<std::string, PluginFactoryFunc> factories_;
    std::unordered_map<std::string, ComponentMetadata> components_;
    std::unordered_map<std::string, ComponentFactoryFunc> componentFactories_;
};

// Global registry instance
IPluginRegistry& getRegistry() {
    static DefaultPluginRegistry registry;
    return registry;
}

// Plugin registration helper functions
void registerBuiltInPlugins() {
    // Register built-in plugins for the NLM system
    // This includes system plugins and utility plugins
    
    PluginMetadata corePlugin;
    corePlugin.name = "nlm_core";
    corePlugin.version = NLM_PLUGIN_SYSTEM_VERSION;
    corePlugin.type = PluginType::SYSTEM;
    corePlugin.description = "Core NLM system components and functionality";
    corePlugin.author = "NLM Team";
    corePlugin.contact = "";
    corePlugin.minSystemVersion = "1.0.0";
    corePlugin.state = PluginState::LOADED;
    corePlugin.isBuiltIn = true;
    
    // Register core plugin
    getRegistry().registerPlugin(corePlugin, []() -> std::unique_ptr<IPlugin> {
        // Would create actual core plugin instance
        return nullptr;
    });
}

void registerBuiltInComponents() {
    // Register built-in components
    // This function is called by DefaultPluginRegistry constructor
}

// Plugin discovery function
bool discoverPlugins(const std::string& directory) {
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                std::string extension = entry.path().extension().string();
                
                // Check if file has plugin extension
                if (extension == ".plugin" || extension == ".dll" || extension == ".so" || extension == ".dylib") {
                    // Try to load plugin metadata
                    PluginMetadata metadata;
                    metadata.name = "discovered_plugin";
                    metadata.version = "1.0.0";
                    metadata.type = PluginType::COMPONENT;
                    metadata.description = "Plugin discovered from " + directory;
                    metadata.author = "Unknown";
                    metadata.minSystemVersion = "1.0.0";
                    metadata.state = PluginState::UNLOADED;
                    metadata.isBuiltIn = false;
                    metadata.filePath = filePath;
                    metadata.className = "PluginClass";
                    
                    // Register plugin
                    getRegistry().registerPlugin(metadata, []() -> std::unique_ptr<IPlugin> {
                        // Would create actual plugin instance
                        return nullptr;
                    });
                }
            }
        }
    } catch (const std::exception&) {
        // Directory doesn't exist or other error
        return false;
    }
    
    return true;
}

} // namespace nlm