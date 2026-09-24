// Dependency injection container
#pragma once

#include "IComponent.hpp"
#include "Config.hpp"
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <mutex>
#include <algorithm>

namespace nlm {

// Component lifecycle phases
enum class LifecyclePhase {
    Construction,
    Initialization,
    Active,
    Shutdown,
    Destruction
};

// Component configuration entry
struct ComponentConfig {
    std::string name;
    std::string type;
    std::map<std::string, std::any> configuration;
    std::vector<std::string> dependencies;
    bool enabled;
    size_t instances;

    ComponentConfig() : enabled(true), instances(1) {}
    ComponentConfig(std::string n, std::string t) : name(std::move(n)), type(std::move(t)),
        enabled(true), instances(1) {}
};

// Dependency injection container implementation
class DependencyInjector {
public:
    DependencyInjector(std::shared_ptr<Config> config = nullptr);
    ~DependencyInjector() = default;

    // Component registration
    template<typename T, typename... Args>
    void registerComponent(const std::string& name,
        std::function<std::shared_ptr<IComponent>(Args...)> factory,
        Args&&... args);

    void registerComponentType(const std::string& name, const ComponentConfig& config);
    void registerFactory(const std::string& name, ComponentFactory factory);
    void unregisterComponent(const std::string& name);

    // Component lifecycle management
    bool initializeComponents();
    void updateComponents(Timestamp deltaTime);
    void shutdownComponents();

    // Component access
    std::shared_ptr<IComponent> getComponent(const std::string& name) const;
    template<typename T>
    std::shared_ptr<T> getComponent(const std::string& name) const;

    // Component discovery and management
    std::vector<std::string> getComponentNames() const;
    std::vector<std::string> getComponentNamesByCategory(const std::string& category) const;
    ComponentState getComponentState(const std::string& name) const;

    // Configuration management
    void setConfig(std::shared_ptr<Config> config);
    std::shared_ptr<Config> getConfig() const;

    // Event system
    void publishEvent(const ComponentEvent& event);
    void subscribeToEvent(const std::string& eventType,
        std::function<void(const ComponentEvent&)> callback);

    // Service locator pattern
    template<typename T>
    std::shared_ptr<T> locate() const;
    template<typename T>
    bool hasService() const;

    // Component health and monitoring
    bool isComponentHealthy(const std::string& name) const;
    std::map<std::string, std::string> getComponentHealthStatus() const;

    // Configuration-driven instantiation
    bool loadComponentsFromConfig(const std::string& configSection = "components");
    bool saveComponentsToConfig(std::string& configSection = "components") const;

    // Utility methods
    void clear();
    size_t getComponentCount() const;

private:
    // Internal component management
    struct ComponentInstance {
        std::shared_ptr<IComponent> component;
        ComponentState state;
        size_t instanceId;
        Timestamp lastUpdateTime;
        std::vector<std::string> activeDependencies;
        std::chrono::steady_clock::time_point creationTime;
    };

    std::unordered_map<std::string, ComponentFactory> factories_;
    std::unordered_map<std::string, ComponentConfig> configs_;
    std::unordered_map<std::string, std::vector<ComponentInstance>> componentInstances_;

    std::shared_ptr<Config> config_;
    std::unordered_map<std::string, std::vector<std::function<void(const ComponentEvent&)>>> eventSubscriptions_;

    mutable std::mutex mutex_;

    // Internal helper methods
    ComponentInstance createComponentInstance(const ComponentConfig& config);
    bool initializeComponentInstance(ComponentInstance& instance);
    void updateComponentInstance(ComponentInstance& instance, Timestamp deltaTime);
    void shutdownComponentInstance(ComponentInstance& instance);
    std::string generateInstanceName(const std::string& baseName, size_t instanceId) const;
    bool resolveDependencies(ComponentInstance& instance);
};

} // namespace nlm

// Include implementation for inline functions
#include "DependencyInjector.cpp"