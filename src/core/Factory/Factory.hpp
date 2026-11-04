// Object factory for creating and managing NLM components
// Provides a centralized factory for dependency injection and configuration management

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <stdexcept>

namespace nlm {

// Base factory interface
class Factory {
public:
    virtual ~Factory() = default;
    virtual void registerComponent(const std::string& name, std::function<std::shared_ptr<void>()> creator) = 0;
    virtual std::shared_ptr<void> create(const std::string& name) = 0;
    virtual bool hasComponent(const std::string& name) const = 0;
};

// Default factory implementation
class DefaultFactory : public Factory {
public:
    DefaultFactory() = default;
    
    // Register a component factory
    template<typename T, typename... Args>
    void registerComponent(const std::string& name, Args&&... args) {
        componentCreators_[name] = [args...]() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new T(std::forward<Args>(args)...));
        };
    }
    
    // Register a component factory with function
    void registerComponent(const std::string& name, std::function<std::shared_ptr<void>()> creator) override {
        componentCreators_[name] = std::move(creator);
    }
    
    // Create a component
    std::shared_ptr<void> create(const std::string& name) override {
        auto it = componentCreators_.find(name);
        if (it == componentCreators_.end()) {
            throw std::runtime_error("Component not registered: " + name);
        }
        return it->second();
    }
    
    // Check if component is registered
    bool hasComponent(const std::string& name) const override {
        return componentCreators_.find(name) != componentCreators_.end();
    }
    
    // Get all registered component names
    std::vector<std::string> getRegisteredComponents() const {
        std::vector<std::string> names;
        names.reserve(componentCreators_.size());
        for (const auto& pair : componentCreators_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Clear all registered components
    void clear() {
        componentCreators_.clear();
    }

private:
    std::unordered_map<std::string, std::function<std::shared_ptr<void>()>> componentCreators_;
};

// Component registration macros
#define REGISTER_COMPONENT(Factory, Name, Component) \
    Factory.registerComponent(Name, []() -> std::shared_ptr<void> { \
        return std::shared_ptr<void>(new Component()); \
    })

// Template version for components with constructor arguments
#define REGISTER_COMPONENT_WITH_ARGS(Factory, Name, Component, ...) \
    Factory.registerComponent(Name, [ = ]() -> std::shared_ptr<void> { \
        return std::shared_ptr<void>(new Component(__VA_ARGS__)); \
    })

// Dependency injection container
class DependencyContainer {
public:
    DependencyContainer() : factory_(std::make_unique<DefaultFactory>()) {}
    
    // Set the factory
    void setFactory(std::unique_ptr<Factory> factory) {
        factory_ = std::move(factory);
    }
    
    // Register a component
    template<typename T, typename... Args>
    void registerComponent(const std::string& name, Args&&... args) {
        factory_->registerComponent<T, Args...>(name, std::forward<Args>(args)...);
    }
    
    // Register a component with custom creator
    void registerComponent(const std::string& name, std::function<std::shared_ptr<void>()> creator) {
        factory_->registerComponent(name, std::move(creator));
    }
    
    // Get a component
    template<typename T>
    std::shared_ptr<T> getComponent(const std::string& name) {
        auto component = factory_->create(name);
        return std::static_pointer_cast<T>(component);
    }
    
    // Check if component is registered
    bool hasComponent(const std::string& name) const {
        return factory_->hasComponent(name);
    }
    
    // Get all registered component names
    std::vector<std::string> getRegisteredComponents() const {
        return factory_->getRegisteredComponents();
    }

private:
    std::unique_ptr<Factory> factory_;
};

// Specialized factory for NLM components
class NLMPFactory : public Factory {
public:
    NLMPFactory() = default;
    
    // Register default components
    void registerDefaults() {
        // Core components
        registerComponent("Brain", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new Brain(nullptr));
        });
        
        registerComponent("Neuron", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new Neuron(NeuronId()));
        });
        
        registerComponent("Synapse", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new Synapse(SynapseId(), NeuronId(0), NeuronId(1)));
        });
        
        registerComponent("Logger", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new Logger());
        });
        
        registerComponent("Config", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new Config());
        });
        
        // Plasticity rules
        registerComponent("STDP", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new STDP());
        });
        
        registerComponent("Hebbian", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new Hebbian());
        });
        
        registerComponent("StructuralPlasticity", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new StructuralPlasticity());
        });
        
        // Neuromodulators
        registerComponent("Dopamine", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new Dopamine());
        });
        
        registerComponent("Curiosity", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new Curiosity());
        });
        
        // Memory systems
        registerComponent("WorkingMemory", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new WorkingMemory());
        });
        
        registerComponent("EpisodicMemory", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new EpisodicMemory());
        });
        
        // Development systems
        registerComponent("DevelopmentSystem", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new DevelopmentSystem());
        });
        
        // Prediction systems
        registerComponent("PredictionSystem", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new PredictionSystem());
        });
        
        // Cognition systems
        registerComponent("NeuralPlanner", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new NeuralPlanner());
        });
        
        // Sensory systems
        registerComponent("Vision", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new Vision(nullptr));
        });
        
        registerComponent("Audio", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new Audio(nullptr));
        });
        
        // Motor system
        registerComponent("MotorSystem", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new MotorSystem());
        });
        
        // Environment
        registerComponent("Environment", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new Environment(nullptr));
        });
        
        // Experiments
        registerComponent("Experiment", []() -> std::shared_ptr<void> {
            return std::shared_ptr<void>(new Experiment());
        });
    }
    
    // Register component with custom configuration
    template<typename T>
    void registerComponent(const std::string& name, const Config& config) {
        registerComponent(name, [config]() -> std::shared_ptr<void> {
            // Component-specific configuration would go here
            return std::shared_ptr<void>(new T(config));
        });
    }
    
    // Create component with specific configuration
    template<typename T>
    std::shared_ptr<T> createComponent(const std::string& name, const Config& config) {
        auto component = factory_->create(name);
        return std::static_pointer_cast<T>(component);
    }

private:
    std::unique_ptr<DefaultFactory> factory_;
};

// Global dependency injection container
class NLMContainer {
public:
    static DependencyContainer& getInstance() {
        static DependencyContainer instance;
        return instance;
    }
    
    // Initialize default components
    static void initialize() {
        auto& container = getInstance();
        
        // Create default factory
        auto factory = std::make_unique<NLMPFactory>();
        factory->registerDefaults();
        
        container.setFactory(std::move(factory));
    }
    
    // Reset container
    static void reset() {
        auto& container = getInstance();
        auto factory = std::make_unique<NLMPFactory>();
        factory->registerDefaults();
        container.setFactory(std::move(factory));
    }

private:
    NLMContainer() = default;
};

// Convenience functions for common operations
namespace FactoryUtils {
    
    // Create a brain with default configuration
    std::shared_ptr<Brain> createBrain() {
        auto& container = NLMContainer::getInstance();
        return container.getComponent<Brain>("Brain");
    }
    
    // Create a brain with specific configuration
    std::shared_ptr<Brain> createBrain(const Config& config) {
        auto& container = NLMContainer::getInstance();
        return container.getComponent<Brain>("Brain");
    }
    
    // Get logger
    std::shared_ptr<Logger> getLogger() {
        auto& container = NLMContainer::getInstance();
        return container.getComponent<Logger>("Logger");
    }
    
    // Get config
    std::shared_ptr<Config> getConfig() {
        auto& container = NLMContainer::getInstance();
        return container.getComponent<Config>("Config");
    }
    
    // Get a specific plasticity rule
    template<typename T>
    std::shared_ptr<T> getPlasticityRule() {
        auto& container = NLMContainer::getInstance();
        return container.getComponent<T>("STDP"); // Default to STDP
    }
    
    // Register a custom component
    template<typename T>
    void registerCustomComponent(const std::string& name) {
        auto& container = NLMContainer::getInstance();
        container.registerComponent<T>(name);
    }
}

} // namespace nlm