// Dependency Injection Framework for NLM
// Provides dependency injection for better testability and maintainability

#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <typeinfo>
#include <functional>
#include <vector>

namespace nlm {

/**
 * Base interface for all injectable components
 */
class IInjectable {
public:
    virtual ~IInjectable() = default;
    virtual std::string getName() const = 0;
    virtual bool initialize() = 0;
    virtual bool shutdown() = 0;
};

/**
 * Dependency injection container
 */
class DependencyContainer {
private:
    std::unordered_map<std::string, std::unique_ptr<IInjectable>> components;
    std::unordered_map<std::string, std::function<std::unique_ptr<IInjectable>()>> factories;
    bool isInitialized = false;
    
public:
    DependencyContainer() = default;
    ~DependencyContainer() = default;
    
    // Register a component by type name
    template<typename T, typename... Args>
    void registerType(const std::string& name = "") {
        std::string componentName = name.empty() ? typeid(T).name() : name;
        factories[componentName] = [this]() -> std::unique_ptr<IInjectable> {
            return std::make_unique<T>();
        };
    }
    
    // Register with factory method
    template<typename T>
    void registerFactory(const std::string& name, 
                        std::function<std::unique_ptr<T>()> factory) {
        std::string componentName = name.empty() ? typeid(T).name() : name;
        factories[componentName] = [factory]() -> std::unique_ptr<IInjectable> {
            return factory();
        };
    }
    
    // Register with shared_ptr
    template<typename T>
    void registerShared(const std::string& name, std::shared_ptr<T> instance) {
        std::string componentName = name.empty() ? typeid(T).name() : name;
        components[componentName] = std::unique_ptr<IInjectable>(instance.release());
    }
    
    // Resolve component by name
    template<typename T>
    std::shared_ptr<T> resolve(const std::string& name = "") {
        std::string componentName = name.empty() ? typeid(T).name() : name;
        
        // Check if already instantiated
        auto it = components.find(componentName);
        if (it != components.end()) {
            return std::dynamic_pointer_cast<T>(std::shared_ptr<IInjectable>(it->second.get(), 
                [](IInjectable*) {}));
        }
        
        // Check if factory exists
        auto factoryIt = factories.find(componentName);
        if (factoryIt != factories.end()) {
            std::unique_ptr<IInjectable> instance = factoryIt->second();
            std::shared_ptr<T> result = std::dynamic_pointer_cast<T>(std::move(instance));
            if (result) {
                components[componentName] = std::unique_ptr<IInjectable>(result.get(), 
                    [](IInjectable*) {});
            }
            return result;
        }
        
        return nullptr;
    }
    
    // Resolve with default if not found
    template<typename T>
    std::shared_ptr<T> resolveSafe(const std::string& name = "", 
                                  std::function<T*()> defaultFactory = nullptr) {
        std::shared_ptr<T> result = resolve<T>(name);
        if (!result && defaultFactory) {
            result = std::shared_ptr<T>(defaultFactory());
            registerShared(name, result);
        }
        return result;
    }
    
    // Initialize all registered components
    void initializeAll() {
        if (isInitialized) return;
        
        for (auto& [name, instance] : components) {
            if (instance) {
                instance->initialize();
            }
        }
        
        isInitialized = true;
    }
    
    // Shutdown all components
    void shutdownAll() {
        if (!isInitialized) return;
        
        for (auto& [name, instance] : components) {
            if (instance) {
                instance->shutdown();
            }
        }
        
        components.clear();
        factories.clear();
        isInitialized = false;
    }
    
    // Get container instance (singleton pattern)
    static DependencyContainer& getInstance() {
        static DependencyContainer instance;
        return instance;
    }
};

/**
 * Configuration component for dependency injection
 */
class ConfigComponent : public IInjectable {
private:
    std::shared_ptr<Config> configInstance;
    bool initialized = false;
    
public:
    ConfigComponent() : configInstance(std::make_shared<Config>()) {}
    ConfigComponent(std::shared_ptr<Config> cfg) : configInstance(cfg) {}
    
    ~ConfigComponent() override = default;
    
    std::string getName() const override {
        return "Config";
    }
    
    bool initialize() override {
        if (initialized) return true;
        
        // Initialize config from defaults
        configInstance->set("neuron_count", 1000);
        configInstance->set("region_count", 1);
        configInstance->set("connection_probability", 0.1f);
        configInstance->set("random_seed", 42);
        configInstance->set("simulation_timestep", 0.001f);
        configInstance->set("stdp_ltp_weight", 0.01f);
        configInstance->set("stdp_ltd_weight", 0.012f);
        
        initialized = true;
        return true;
    }
    
    bool shutdown() override {
        initialized = false;
        return true;
    }
    
    std::shared_ptr<Config> getConfig() {
        return configInstance;
    }
    
    void setConfig(std::shared_ptr<Config> cfg) {
        configInstance = cfg;
    }
};

/**
 * Logger component for dependency injection
 */
class LoggerComponent : public IInjectable {
private:
    std::shared_ptr<Logger> loggerInstance;
    bool initialized = false;
    
public:
    LoggerComponent() : loggerInstance(std::make_shared<Logger>()) {}
    LoggerComponent(std::shared_ptr<Logger> logger) : loggerInstance(logger) {}
    
    ~LoggerComponent() override = default;
    
    std::string getName() const override {
        return "Logger";
    }
    
    bool initialize() override {
        if (initialized) return true;
        
        loggerInstance->setLevel(LogLevel::Info);
        initialized = true;
        return true;
    }
    
    bool shutdown() override {
        initialized = false;
        return true;
    }
    
    std::shared_ptr<Logger> getLogger() {
        return loggerInstance;
    }
};

/**
 * Brain component for dependency injection
 */
class BrainComponent : public IInjectable {
private:
    std::shared_ptr<Brain> brainInstance;
    std::shared_ptr<Config> config;
    bool initialized = false;
    
public:
    BrainComponent() = default;
    BrainComponent(std::shared_ptr<Config> cfg) : config(cfg) {}
    
    ~BrainComponent() override = default;
    
    std::string getName() const override {
        return "Brain";
    }
    
    bool initialize() override {
        if (!config) {
            // Try to get config from container
            auto& container = DependencyContainer::getInstance();
            config = container.resolveSafe<ConfigComponent>("Config");
        }
        
        if (!config) {
            return false;
        }
        
        brainInstance = std::make_shared<Brain>(config);
        brainInstance->initialize();
        
        initialized = true;
        return true;
    }
    
    bool shutdown() override {
        if (brainInstance) {
            brainInstance.reset();
        }
        initialized = false;
        return true;
    }
    
    std::shared_ptr<Brain> getBrain() {
        return brainInstance;
    }
    
    void setBrain(std::shared_ptr<Brain> brain) {
        brainInstance = brain;
    }
};

/**
 * World component for dependency injection
 */
class WorldComponent : public IInjectable {
private:
    std::shared_ptr<SimpleWorld> worldInstance;
    bool initialized = false;
    
public:
    WorldComponent() : worldInstance(std::make_shared<SimpleWorld>()) {}
    WorldComponent(std::shared_ptr<SimpleWorld> world) : worldInstance(world) {}
    
    ~WorldComponent() override = default;
    
    std::string getName() const override {
        return "World";
    }
    
    bool initialize() override {
        if (initialized) return true;
        
        // Initialize world with default settings
        worldInstance->configure(20.0f, 20.0f, 16, 16);
        
        initialized = true;
        return true;
    }
    
    bool shutdown() override {
        if (worldInstance) {
            worldInstance.reset();
        }
        initialized = false;
        return true;
    }
    
    std::shared_ptr<SimpleWorld> getWorld() {
        return worldInstance;
    }
    
    void setWorld(std::shared_ptr<SimpleWorld> world) {
        worldInstance = world;
    }
};

/**
 * AgentBrain component for dependency injection
 */
class AgentBrainComponent : public IInjectable {
private:
    std::shared_ptr<AgentBrain> agentBrainInstance;
    bool initialized = false;
    
public:
    AgentBrainComponent() : agentBrainInstance(std::make_shared<AgentBrain>()) {}
    AgentBrainComponent(std::shared_ptr<AgentBrain> agentBrain) : agentBrainInstance(agentBrain) {}
    
    ~AgentBrainComponent() override = default;
    
    std::string getName() const override {
        return "AgentBrain";
    }
    
    bool initialize() override {
        if (initialized) return true;
        
        // AgentBrain needs a brain and world, which should be configured by container
        auto& container = DependencyContainer::getInstance();
        
        auto brain = container.resolveSafe<BrainComponent>("Brain");
        auto world = container.resolveSafe<WorldComponent>("World");
        
        if (!brain || !world) {
            return false;
        }
        
        agentBrainInstance = std::make_shared<AgentBrain>(brain);
        agentBrainInstance->initialize(*world);
        
        initialized = true;
        return true;
    }
    
    bool shutdown() override {
        if (agentBrainInstance) {
            agentBrainInstance.reset();
        }
        initialized = false;
        return true;
    }
    
    std::shared_ptr<AgentBrain> getAgentBrain() {
        return agentBrainInstance;
    }
    
    void setAgentBrain(std::shared_ptr<AgentBrain> agentBrain) {
        agentBrainInstance = agentBrain;
    }
};

/**
 * Configuration service for dependency injection
 */
class ConfigurationService {
private:
    std::shared_ptr<Config> config;
    
public:
    ConfigurationService(std::shared_ptr<Config> cfg) : config(cfg) {}
    
    virtual ~ConfigurationService() = default;
    
    // Apply configuration to components
    virtual void applyToBrain(std::shared_ptr<Brain> brain) {
        if (brain) {
            brain->configureFromConfig(config);
        }
    }
    
    // Get configuration
    std::shared_ptr<Config> getConfig() const {
        return config;
    }
    
    // Set configuration
    void setConfig(std::shared_ptr<Config> cfg) {
        config = cfg;
    }
};

/**
 * Factory for creating configured components
 */
class ComponentFactory {
public:
    // Create a fully configured dependency container
    static DependencyContainer createDefaultContainer() {
        DependencyContainer container;
        
        // Register components with default factories
        container.registerFactory<ConfigComponent>("Config", []() {
            return std::make_unique<ConfigComponent>();
        });
        
        container.registerFactory<LoggerComponent>("Logger", []() {
            return std::make_unique<LoggerComponent>();
        });
        
        container.registerFactory<WorldComponent>("World", []() {
            return std::make_unique<WorldComponent>();
        });
        
        // Initialize container
        container.initializeAll();
        
        return container;
    }
    
    // Create container with custom configuration
    static DependencyContainer createContainerWithConfig(std::shared_ptr<Config> config) {
        DependencyContainer container;
        
        container.registerFactory<ConfigComponent>("Config", [config]() {
            return std::make_unique<ConfigComponent>(config);
        });
        
        container.registerFactory<LoggerComponent>("Logger", []() {
            return std::make_unique<LoggerComponent>();
        });
        
        container.registerFactory<WorldComponent>("World", []() {
            return std::make_unique<WorldComponent>();
        });
        
        // Initialize container
        container.initializeAll();
        
        return container;
    }
};

/**
 * Smart pointer wrapper for dependency injection
 */
template<typename T>
class Injected {
private:
    std::shared_ptr<T> ptr;
    std::string componentName;
    DependencyContainer& container;
    
public:
    Injected(DependencyContainer& cont, const std::string& name = "") 
        : container(cont), componentName(name) {
        std::string resolvedName = name.empty() ? typeid(T).name() : name;
        ptr = container.resolve<T>(resolvedName);
    }
    
    Injected(DependencyContainer& cont, std::shared_ptr<T> instance) 
        : container(cont), ptr(instance) {
        std::string resolvedName = componentName.empty() ? typeid(T).name() : componentName;
    }
    
    ~Injected() = default;
    
    // Dereference operators
    T* operator->() {
        return ptr.get();
    }
    
    T& operator*() {
        return *ptr;
    }
    
    // Const dereference operators
    const T* operator->() const {
        return ptr.get();
    }
    
    const T& operator*() const {
        return *ptr;
    }
    
    // Smart pointer operations
    T* get() {
        return ptr.get();n    }
    
    std::shared_ptr<T> getShared() {
        return ptr;
    }
    
    bool hasInstance() const {
        return ptr != nullptr;
    }
    
    // Assignment operators
    Injected& operator=(std::shared_ptr<T> other) {
        ptr = other;
        return *this;
    }
    
    // Check if instance exists
    bool isNull() const {
        return ptr == nullptr;
    }
    
    // Reset instance
    void reset() {
        ptr.reset();
    }
    
    // Set component name
    void setComponentName(const std::string& name) {
        componentName = name;
    }
};

/**
 * Service locator for easy access to components
 */
class ServiceLocator {
private:
    static DependencyContainer* container;
    
public:
    static void setLocator(DependencyContainer& cont) {
        container = &cont;
    }
    
    static DependencyContainer& getLocator() {
        if (!container) {
            throw std::runtime_error("ServiceLocator not initialized. Call setLocator() first.");
        }
        return *container;
    }
    
    static void reset() {
        container = nullptr;
    }
};

/**
 * Component registration macros for easy setup
 */
#define REGISTER_COMPONENT(ComponentType, name) \
    container.registerFactory<ComponentType>(name, []() { return std::make_unique<ComponentType>(); })

#define REGISTER_SHARED_COMPONENT(ComponentType, instance) \
    container.registerShared<ComponentType>(#instance, instance)

/**
 * Template function for easy component injection
 */
template<typename T>
Injected<T> inject(DependencyContainer& container, const std::string& name = "") {
    return Injected<T>(container, name);
}

/**
 * Global dependency injection utilities
 */
namespace DI {
    // Get global container instance
    inline DependencyContainer& getContainer() {
        return DependencyContainer::getInstance();
    }
    
    // Initialize global container
    inline void initializeGlobalContainer() {
        DependencyContainer::getInstance().initializeAll();
    }
    
    // Shutdown global container
    inline void shutdownGlobalContainer() {
        DependencyContainer::getInstance().shutdownAll();
    }
    
    // Reset global container
    inline void resetGlobalContainer() {
        DependencyContainer::getInstance().shutdownAll();
        DependencyContainer::getInstance().~DependencyContainer();
        new (&DependencyContainer::getInstance()) DependencyContainer();
    }
}

} // namespace nlm
