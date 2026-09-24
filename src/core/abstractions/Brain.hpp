// Abstract brain class with pluggable components
#pragma once

#include "IComponent.hpp"
#include "DependencyInjector.hpp"
#include "Types.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace nlm {

// Brain component registration
struct BrainComponentInfo {
    std::string name;
    std::string type;
    std::string category;
    size_t priority;  // Lower numbers = higher priority
    std::map<std::string, std::any> configuration;
    std::vector<std::string> dependencies;
    bool required;
    std::vector<std::string> eventsToPublish;
    std::vector<std::string> eventsToSubscribe;

    BrainComponentInfo() : name(""), type(""), category(""), priority(100), required(false) {}
    BrainComponentInfo(std::string n, std::string t) : name(std::move(n)), type(std::move(t)),
        category(""), priority(100), required(false) {}
};

// Brain events for coordination
struct BrainEvent {
    std::string eventType;
    Timestamp timestamp;
    std::map<std::string, std::any> data;
    std::string source;

    BrainEvent() : timestamp(0.0) {}
    BrainEvent(std::string type, std::any data, std::string src = "")
        : eventType(std::move(type)), data(std::move(data)), source(std::move(src)), timestamp(0.0) {}
};

// Brain component configuration
struct BrainComponentConfig {
    std::string name;
    std::string type;
    std::map<std::string, std::any> configuration;
    std::vector<std::string> dependencies;
    bool enabled;

    BrainComponentConfig() : enabled(true) {}
    BrainComponentConfig(std::string n, std::string t) : name(std::move(n)), type(std::move(t)),
        enabled(true) {}
};

// Abstract Brain class with pluggable components
class Brain {
public:
    Brain(std::shared_ptr<Config> config,
        std::shared_ptr<DependencyInjector> injector = nullptr);
    virtual ~Brain() = default;

    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept = default;
    Brain& operator=(Brain&&) noexcept = default;

    // Component management
    virtual bool initialize();
    virtual void update(Timestamp deltaTime);
    virtual void shutdown();

    // Component registration
    virtual void registerComponent(std::shared_ptr<IComponent> component);
    virtual bool registerComponentType(const BrainComponentInfo& info);
    virtual bool unregisterComponent(const std::string& name);

    // Component access
    virtual std::shared_ptr<IComponent> getComponent(const std::string& name) const;
    virtual std::vector<std::string> getComponentNames() const;
    virtual std::vector<std::string> getComponentNamesByCategory(const std::string& category) const;

    // Component lifecycle
    virtual bool activateComponent(const std::string& name);
    virtual bool deactivateComponent(const std::string& name);
    virtual ComponentState getComponentState(const std::string& name) const;

    // Event system
    virtual void publishEvent(const BrainEvent& event);
    virtual void subscribeToEvent(const std::string& eventType,
        std::function<void(const BrainEvent&)> callback);

    // Configuration management
    virtual void configureComponents();
    virtual void updateComponentConfigurations();

    // Brain-specific operations (to be implemented by concrete brain classes)
    virtual void step(SimulationStep currentStep, Timestamp currentTime) = 0;
    virtual void receiveInput(const std::vector<float>& input) = 0;
    virtual std::vector<float> produceOutput() = 0;
    virtual void reset() = 0;

    // Statistics and monitoring
    virtual size_t getComponentCount() const;
    virtual std::map<std::string, std::string> getSystemHealthStatus() const;
    virtual std::map<std::string, float> getSystemMetrics() const;

    // Configuration access
    virtual std::shared_ptr<Config> getConfig() const;
    virtual std::shared_ptr<DependencyInjector> getInjector() const;

protected:
    // Component management internals
    virtual bool initializeComponent(std::shared_ptr<IComponent> component);
    virtual void updateComponent(std::shared_ptr<IComponent> component, Timestamp deltaTime);
    virtual void shutdownComponent(std::shared_ptr<IComponent> component);
    virtual bool resolveComponentDependencies(std::shared_ptr<IComponent> component);

    // Event system internals
    virtual void processEvent(const BrainEvent& event);
    virtual void deliverEventToComponent(std::shared_ptr<IComponent> component,
        const BrainEvent& event);

    // Configuration internals
    virtual void applyComponentConfiguration(std::shared_ptr<IComponent> component);
    virtual std::map<std::string, std::any> getComponentConfigFromBrain(const std::string& name) const;

    // Component state management
    virtual void updateComponentStates(Timestamp deltaTime);
    virtual void cleanupInactiveComponents();

    // Brain state
    std::shared_ptr<Config> config_;
    std::shared_ptr<DependencyInjector> injector_;

    // Component registry
    std::unordered_map<std::string, std::shared_ptr<IComponent>> components_;
    std::unordered_map<std::string, ComponentState> componentStates_;
    std::unordered_map<std::string, BrainComponentInfo> componentInfo_;

    // Event system
    std::unordered_map<std::string, std::vector<std::function<void(const BrainEvent&)>>> eventHandlers_;

    // Configuration
    std::vector<BrainComponentConfig> componentConfigs_;
    bool initialized_;

    // Statistics
    Timestamp lastUpdateTime_;
    size_t componentsInitialized_;
    size_t componentsActive_;
};

// Template helper for creating brain components from factories
class BrainComponentFactory {
public:
    using ComponentFactoryFunc = std::function<std::shared_ptr<IComponent>(DependencyInjector& injector)>;

    template<typename T, typename... Args>
    static void registerFactory(DependencyInjector& injector,
        const std::string& name,
        std::function<std::shared_ptr<T>(Args...)> factory,
        Args&&... args);

    template<typename T>
    static void registerDefaultFactory(DependencyInjector& injector,
        const std::string& name);

    template<typename T>
    static std::shared_ptr<T> createComponent(DependencyInjector& injector,
        const std::string& name);
};

} // namespace nlm

// Include implementations for inline functions
#include "Brain.cpp"