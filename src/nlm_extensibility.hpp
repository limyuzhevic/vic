"""
NLM Extensibility Framework

This framework provides an extensibility system for NLM that allows users to:
- Create custom agent implementations without modifying core code
- Create custom world implementations without modifying core code
- Plugin system for adding new neural components
- Factory pattern for creating NLM components
- Registry system for tracking custom implementations
"""

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <typeindex>
#include <stdexcept>

namespace nlm {

// Forward declarations for the extensibility framework
class IAgentFactory;
class IWorldFactory;
class IComponentFactory;
class INLMPlugin;

// Plugin base class
class INLMPlugin {
public:
    virtual ~INLMPlugin() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
};

// Agent factory interface
class IAgentFactory {
public:
    virtual ~IAgentFactory() = default;
    virtual std::shared_ptr<void> createAgent(const std::string& name) = 0;
    virtual bool isRegistered(const std::string& name) const = 0;
    virtual std::vector<std::string> getRegisteredAgentTypes() const = 0;
};

// World factory interface
class IWorldFactory {
public:
    virtual ~IWorldFactory() = default;
    virtual std::shared_ptr<void> createWorld(const std::string& name) = 0;
    virtual bool isRegistered(const std::string& name) const = 0;
    virtual std::vector<std::string> getRegisteredWorldTypes() const = 0;
};

// Component factory interface for neural components
class IComponentFactory {
public:
    virtual ~IComponentFactory() = default;
    virtual std::shared_ptr<void> createComponent(const std::string& componentType, 
                                                 const std::string& name) = 0;
    virtual bool isRegistered(const std::string& componentType) const = 0;
    virtual std::vector<std::string> getRegisteredComponentTypes() const = 0;
};

// Registry for managing plugins and factories
class NLMRegistry {
private:
    std::unordered_map<std::string, std::shared_ptr<INLMPlugin>> plugins_;
    std::shared_ptr<IAgentFactory> agentFactory_;
    std::shared_ptr<IWorldFactory> worldFactory_;
    std::shared_ptr<IComponentFactory> componentFactory_;
    
public:
    static NLMRegistry& getInstance() {
        static NLMRegistry instance;
        return instance;
    }
    
    // Plugin management
    void registerPlugin(const std::string& name, std::shared_ptr<INLMPlugin> plugin) {
        plugins_[name] = plugin;
    }
    
    std::shared_ptr<INLMPlugin> getPlugin(const std::string& name) {
        auto it = plugins_.find(name);
        return (it != plugins_.end()) ? it->second : nullptr;
    }
    
    std::vector<std::string> getPluginNames() const {
        std::vector<std::string> names;
        names.reserve(plugins_.size());
        for (const auto& pair : plugins_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Factory management
    void setAgentFactory(std::shared_ptr<IAgentFactory> factory) {
        agentFactory_ = factory;
    }
    
    std::shared_ptr<IAgentFactory> getAgentFactory() const {
        return agentFactory_;
    }
    
    void setWorldFactory(std::shared_ptr<IWorldFactory> factory) {
        worldFactory_ = factory;
    }
    
    std::shared_ptr<IWorldFactory> getWorldFactory() const {
        return worldFactory_;
    }
    
    void setComponentFactory(std::shared_ptr<IComponentFactory> factory) {
        componentFactory_ = factory;
    }
    
    std::shared_ptr<IComponentFactory> getComponentFactory() const {
        return componentFactory_;
    }
    
    // Factory creation helpers
    template<typename T, typename... Args>
    void registerAgentType(const std::string& name) {
        if (agentFactory_) {
            // This would require the factory to implement type registration
            // For now, this is a placeholder for type-safe factory methods
        }
    }
    
    template<typename T, typename... Args>
    void registerWorldType(const std::string& name) {
        if (worldFactory_) {
            // Placeholder for world type registration
        }
    }
};

// Plugin system implementation
class BasicPlugin : public INLMPlugin {
private:
    std::string name_;
    std::string version_;
    bool initialized_ = false;
    
public:
    BasicPlugin(const std::string& name, const std::string& version)
        : name_(name), version_(version) {}
    
    std::string getName() const override { return name_; }
    std::string getVersion() const override { return version_; }
    
    void initialize() override {
        if (!initialized_) {
            // Plugin-specific initialization
            initialized_ = true;
        }
    }
    
    void shutdown() override {
        initialized_ = false;
    }
    
    bool isInitialized() const { return initialized_; }
};

// Default agent factory implementation
class DefaultAgentFactory : public IAgentFactory {
private:
    std::unordered_map<std::string, std::function<std::shared_ptr<void>()>> agentCreators_;
    
public:
    void registerAgent(const std::string& name, std::function<std::shared_ptr<void>()> creator) {
        agentCreators_[name] = creator;
    }
    
    std::shared_ptr<void> createAgent(const std::string& name) override {
        auto it = agentCreators_.find(name);
        if (it != agentCreators_.end()) {
            return it->second();
        }
        return nullptr;
    }
    
    bool isRegistered(const std::string& name) const override {
        return agentCreators_.find(name) != agentCreators_.end();
    }
    
    std::vector<std::string> getRegisteredAgentTypes() const override {
        std::vector<std::string> types;
        types.reserve(agentCreators_.size());
        for (const auto& pair : agentCreators_) {
            types.push_back(pair.first);
        }
        return types;
    }
};

// Default world factory implementation
class DefaultWorldFactory : public IWorldFactory {
private:
    std::unordered_map<std::string, std::function<std::shared_ptr<void>()>> worldCreators_;
    
public:
    void registerWorld(const std::string& name, std::function<std::shared_ptr<void>()> creator) {
        worldCreators_[name] = creator;
    }
    
    std::shared_ptr<void> createWorld(const std::string& name) override {
        auto it = worldCreators_.find(name);
        if (it != worldCreators_.end()) {
            return it->second();
        }
        return nullptr;
    }
    
    bool isRegistered(const std::string& name) const override {
        return worldCreators_.find(name) != worldCreators_.end();
    }
    
    std::vector<std::string> getRegisteredWorldTypes() const override {
        std::vector<std::string> types;
        types.reserve(worldCreators_.size());
        for (const auto& pair : worldCreators_) {
            types.push_back(pair.first);
        }
        return types;
    }
};

// Default component factory implementation
class DefaultComponentFactory : public IComponentFactory {
private:
    std::unordered_map<std::string, std::function<std::shared_ptr<void>(const std::string&, const std::string&)>> componentCreators_;
    
public:
    void registerComponent(const std::string& type, 
                          std::function<std::shared_ptr<void>(const std::string&, const std::string&)> creator) {
        componentCreators_[type] = creator;
    }
    
    std::shared_ptr<void> createComponent(const std::string& componentType, 
                                         const std::string& name) override {
        auto it = componentCreators_.find(componentType);
        if (it != componentCreators_.end()) {
            return it->second(componentType, name);
        }
        return nullptr;
    }
    
    bool isRegistered(const std::string& componentType) const override {
        return componentCreators_.find(componentType) != componentCreators_.end();
    }
    
    std::vector<std::string> getRegisteredComponentTypes() const override {
        std::vector<std::string> types;
        types.reserve(componentCreators_.size());
        for (const auto& pair : componentCreators_) {
            types.push_back(pair.first);
        }
        return types;
    }
};

// Helper functions for easy registration
namespace PluginHelpers {
    // Template function to create typed agent wrappers
    template<typename T>
    std::shared_ptr<T> createAgentTyped(const std::string& name) {
        auto registry = NLMRegistry::getInstance();
        auto factory = registry.getAgentFactory();
        if (!factory) return nullptr;
        
        auto agent = factory->createAgent(name);
        return std::static_pointer_cast<T>(agent);
    }
    
    // Template function to create typed world wrappers
    template<typename T>
    std::shared_ptr<T> createWorldTyped(const std::string& name) {
        auto registry = NLMRegistry::getInstance();
        auto factory = registry.getWorldFactory();
        if (!factory) return nullptr;
        
        auto world = factory->createWorld(name);
        return std::static_pointer_cast<T>(world);
    }
    
    // Template function to create typed component wrappers
    template<typename T>
    std::shared_ptr<T> createComponentTyped(const std::string& componentType, const std::string& name) {
        auto registry = NLMRegistry::getInstance();
        auto factory = registry.getComponentFactory();
        if (!factory) return nullptr;
        
        auto component = factory->createComponent(componentType, name);
        return std::static_pointer_cast<T>(component);
    }
}

// Macro for easy plugin registration
#define NLM_REGISTER_PLUGIN(plugin_name, version) \
    namespace { \
        class plugin_name##_Plugin : public BasicPlugin { \
        public: \
            plugin_name##_Plugin() : BasicPlugin(#plugin_name, version) {} \
            void initialize() override { /* Plugin-specific initialization */ } \
            void shutdown() override { /* Plugin-specific cleanup */ } \
        }; \
        const BasicPlugin& register_plugin_name() { \
            static plugin_name##_Plugin instance; \
            NLMRegistry::getInstance().registerPlugin(#plugin_name, std::make_shared<plugin_name##_Plugin>(instance)); \
            return instance; \
        } \
        const BasicPlugin& plugin_name##_registration = register_plugin_name(); \
    }

// Macro for agent factory registration
#define NLM_REGISTER_AGENT(name, creator_function) \
    do { \
        auto factory = NLMRegistry::getInstance().getAgentFactory(); \
        if (factory) { \
            std::dynamic_pointer_cast<DefaultAgentFactory>(factory)->registerAgent(name, creator_function); \
        } \
    } while(0)

// Macro for world factory registration  
#define NLM_REGISTER_WORLD(name, creator_function) \
    do { \
        auto factory = NLMRegistry::getInstance().getWorldFactory(); \
        if (factory) { \
            std::dynamic_pointer_cast<DefaultWorldFactory>(factory)->registerWorld(name, creator_function); \
        } \
    } while(0)

// Macro for component factory registration
#define NLM_REGISTER_COMPONENT(type, creator_function) \
    do { \
        auto factory = NLMRegistry::getInstance().getComponentFactory(); \
        if (factory) { \
            std::dynamic_pointer_cast<DefaultComponentFactory>(factory)->registerComponent(type, creator_function); \
        } \
    } while(0)

// Factory setup function
inline void initializeExtensibilitySystem() {
    auto registry = NLMRegistry::getInstance();
    
    // Setup default factories
    auto agentFactory = std::make_shared<DefaultAgentFactory>();
    auto worldFactory = std::make_shared<DefaultWorldFactory>();
    auto componentFactory = std::make_shared<DefaultComponentFactory>();
    
    registry.setAgentFactory(agentFactory);
    registry.setWorldFactory(worldFactory);
    registry.setComponentFactory(componentFactory);
}

// Plugin management functions
inline void loadPlugin(const std::string& pluginName) {
    auto registry = NLMRegistry::getInstance();
    auto plugin = registry.getPlugin(pluginName);
    if (plugin) {
        plugin->initialize();
    }
}

inline void unloadPlugin(const std::string& pluginName) {
    auto registry = NLMRegistry::getInstance();
    auto plugin = registry.getPlugin(pluginName);
    if (plugin) {
        plugin->shutdown();
        // Remove from registry
        registry.plugins_.erase(pluginName);
    }
}

inline std::vector<std::string> getAvailablePlugins() {
    auto registry = NLMRegistry::getInstance();
    return registry.getPluginNames();
}

// Get registered types
inline std::vector<std::string> getRegisteredAgentTypes() {
    auto registry = NLMRegistry::getInstance();
    auto factory = registry.getAgentFactory();
    return factory ? factory->getRegisteredAgentTypes() : std::vector<std::string>();
}

inline std::vector<std::string> getRegisteredWorldTypes() {
    auto registry = NLMRegistry::getInstance();
    auto factory = registry.getWorldFactory();
    return factory ? factory->getRegisteredWorldTypes() : std::vector<std::string>();
}

inline std::vector<std::string> getRegisteredComponentTypes() {
    auto registry = NLMRegistry::getInstance();
    auto factory = registry.getComponentFactory();
    return factory ? factory->getRegisteredComponentTypes() : std::vector<std::string>();
}

// Plugin interface for Python bindings
class PythonPlugin : public INLMPlugin {
private:
    std::string name_;
    std::string version_;
    bool initialized_ = false;
    
public:
    PythonPlugin(const std::string& name, const std::string& version)
        : name_(name), version_(version) {}
    
    std::string getName() const override { return name_; }
    std::string getVersion() const override { return version_; }
    
    void initialize() override {
        initialized_ = true;
    }
    
    void shutdown() override {
        initialized_ = false;
    }
};

} // namespace nlm