#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <typeinfo>
#include <typeindex>
#include <functional>
#include <vector>

namespace nlm {

// Forward declarations for all component interfaces
class AbstractBrain;
class AbstractPlasticityRule;
class AbstractNeuromodulator;
class AbstractDevelopmentSystem;
class AbstractMemorySystem;
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;

// Type-safe dependency injection container
class DIContainer {
public:
    // Registration types
    using ComponentFactory = std::function<std::shared_ptr<void>(std::shared_ptr<Config>)>;
    using ConcreteComponentFactory = std::function<std::shared_ptr<void>()>;
    
    // Singleton instance access
    static DIContainer& getInstance() {
        static DIContainer instance;
        return instance;
    }
    
    // Registration methods
    template<typename T>
    void registerType(ComponentFactory factory) {
        std::type_index typeIndex(typeid(T));
        componentFactories_[typeIndex] = factory;
    }
    
    template<typename T>
    void registerSingleton(ComponentFactory factory) {
        std::type_index typeIndex(typeid(T));
        componentFactories_[typeIndex] = factory;
        // Mark as singleton
        singletons_[typeIndex] = factory(nullptr);
    }
    
    template<typename T>
    void registerSingleton() {
        // For singletons that can be default-constructed
        std::type_index typeIndex(typeid(T));
        if (singletons_.find(typeIndex) == singletons_.end()) {
            singletons_[typeIndex] = std::make_shared<T>();
        }
    }
    
    // Registration with configuration
    template<typename T>
    void registerType(const std::string& componentName, ComponentFactory factory) {
        componentFactories_[typeIndex] = factory;
        componentNames_[typeIndex] = componentName;
    }
    
    // Resolution methods
    template<typename T>
    std::shared_ptr<T> resolve() {
        std::type_index typeIndex(typeid(T));
        
        // Check singleton first
        auto it = singletons_.find(typeIndex);
        if (it != singletons_.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        
        // Check factory
        auto factoryIt = componentFactories_.find(typeIndex);
        if (factoryIt != componentFactories_.end()) {
            void* rawPtr = factoryIt->second(nullptr);
            return std::static_pointer_cast<T>(std::shared_ptr<void>(rawPtr, [](void* p) { delete p; }));
        }
        
        // Not registered
        return nullptr;
    }
    
    // Override for direct dependency (config passed to factory)
    template<typename T>
    std::shared_ptr<T> resolve(std::shared_ptr<Config> config) {
        std::type_index typeIndex(typeid(T));
        
        // Check factory
        auto factoryIt = componentFactories_.find(typeIndex);
        if (factoryIt != componentFactories_.end()) {
            void* rawPtr = factoryIt->second(config);
            return std::static_pointer_cast<T>(std::shared_ptr<void>(rawPtr, [](void* p) { delete p; }));
        }
        
        return nullptr;
    }
    
    // Configuration-based component selection
    bool configureFromJSON(const std::string& configFilepath);
    bool configureFromYAML(const std::string& configFilepath);
    
    // Component registration from config
    void registerFromConfig(std::shared_ptr<Config> config);
    
    // Component lifecycle management
    template<typename T>
    void injectInto(T* obj) {
        // Inject dependencies based on constructor signature
        injectDependencies(obj, std::index_sequence_for<typename T::ConstructorArgs...>{});
    }
    
    // Plugin system
    using PluginFactory = std::function<void(DIContainer&)>;
    bool loadPlugin(const std::string& pluginPath);
    void registerPlugin(const std::string& name, PluginFactory factory);
    
    // Component lifecycle
    void startAllComponents();
    void stopAllComponents();
    void updateAllComponents(double dt);
    
    // Debug and inspection
    std::vector<std::string> getRegisteredComponentTypes() const;
    std::string getComponentName(std::type_index typeIndex) const;
    
protected:
    std::unordered_map<std::type_index, ComponentFactory> componentFactories_;
    std::unordered_map<std::type_index, std::shared_ptr<void>> singletons_;
    std::unordered_map<std::type_index, std::string> componentNames_;
    std::unordered_map<std::string, PluginFactory> plugins_;
    
private:
    DIContainer() = default;
    DIContainer(const DIContainer&) = delete;
    DIContainer& operator=(const DIContainer&) = delete;
    
    // Template helper for dependency injection
    template<typename T, std::size_t... I>
    void injectDependencies(T* obj, std::index_sequence<I...>) {
        // Inject dependencies for each constructor parameter
        // This is a simplified version - real implementation would use reflection
        // For now, we'll use explicit injection methods
    }
};

} // namespace nlm
