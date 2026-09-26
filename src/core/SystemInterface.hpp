/*
 * NLM System Interface Framework - Abstract Base Classes
 * Provides abstract base classes and interfaces for all neural system components
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <any>
#include <stdexcept>
#include <mutex>

namespace nlm {

// Forward declarations
class ISystem;
class IComponent;
class IDataStore;
class IEventSystem;
class IPluginSystem;
class IServiceLocator;

// Component Interface
class IComponent {
public:
    virtual ~IComponent() = default;
    
    virtual std::string getComponentName() const = 0;
    virtual std::string getComponentVersion() const = 0;
    virtual std::string getComponentId() const = 0;
    
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    virtual bool isInitialized() const { return getComponentState() == ComponentState::Initialized; }
    
    // Component configuration
    virtual void configure(const std::map<std::string, std::any>& config) {}
    virtual std::map<std::string, std::any> getConfig() const { return std::map<std::string, std::any>(); }
    
    // Component metadata
    virtual std::map<std::string, std::any> getMetadata() const { return std::map<std::string, std::any>(); }
    
    // Component lifecycle
    virtual bool start() { return true; }
    virtual bool stop() { return true; }
    
    // Component services
    virtual bool registerService(const std::string& serviceName, std::any service) { return false; }
    virtual std::any getService(const std::string& serviceName) { return std::any(); }
    virtual bool hasService(const std::string& serviceName) const { return false; }
};

// Component State
enum class ComponentState {
    NotInitialized,
    Initializing,
    Initialized,
    Starting,
    Running,
    Stopping,
    Shutdown,
    Error
};

// Abstract Component Base Class
class Component : public IComponent {
public:
    Component(const std::string& name, const std::string& version);
    virtual ~Component() override;
    
    std::string getComponentName() const override { return componentName_; }
    std::string getComponentVersion() const override { return componentVersion_; }
    std::string getComponentId() const override { return componentId_; }
    
    bool initialize() override;
    void shutdown() override;
    
    virtual bool internalInitialize() = 0;
    virtual void internalShutdown() = 0;
    
    ComponentState getComponentState() const { return state_; }
    
    // Configuration
    void configure(const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getConfig() const override;
    
    // Metadata
    std::map<std::string, std::any> getMetadata() const override;
    
    // Lifecycle
    bool start() override;
    bool stop() override;
    
protected:
    std::string componentName_;
    std::string componentVersion_;
    std::string componentId_;
    ComponentState state_;
    std::map<std::string, std::any> config_;
    
    // Component statistics
    mutable std::map<std::string, std::any> statistics_;
    mutable std::mutex statisticsMutex_;
};

// System Interface
class ISystem {
public:
    virtual ~ISystem() = default;
    
    virtual std::string getSystemName() const = 0;
    virtual std::string getSystemVersion() const = 0;
    virtual std::string getSystemId() const = 0;
    
    virtual SystemState getSystemState() const = 0;
    
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    virtual bool isInitialized() const { return getSystemState() == SystemState::Initialized; }
    virtual bool isRunning() const { return getSystemState() == SystemState::Running; }
    
    // System configuration
    virtual void configure(const std::map<std::string, std::any>& config) {}
    virtual std::map<std::string, std::any> getConfig() const { return std::map<std::string, std::any>(); }
    
    // System metadata
    virtual std::map<std::string, std::any> getMetadata() const { return std::map<std::string, std::any>(); }
    
    // System components
    virtual bool registerComponent(std::shared_ptr<IComponent> component) = 0;
    virtual std::shared_ptr<IComponent> getComponent(const std::string& componentName) = 0;
    virtual std::vector<std::shared_ptr<IComponent>> getComponents() const = 0;
    virtual bool hasComponent(const std::string& componentName) const = 0;
    
    // System services
    virtual bool registerService(const std::string& serviceName, std::any service) = 0;
    virtual std::any getService(const std::string& serviceName) = 0;
    virtual bool hasService(const std::string& serviceName) const = 0;
    
    // System lifecycle
    virtual bool start() { return true; }
    virtual bool stop() { return true; }
    
    // System information
    virtual std::string getSystemInfo() const { return "System: " + getSystemName(); }
    virtual std::map<std::string, std::any> getStatistics() const { return std::map<std::string, std::any>(); }
    
    // System validation
    virtual bool validate() const { return true; }
    virtual std::string validateAll() const { return "Validation passed"; }
};

// System State
enum class SystemState {
    NotInitialized,
    Initializing,
    Initialized,
    Running,
    ShuttingDown,
    Shutdown,
    Error
};

// Abstract System Base Class
class System : public ISystem {
public:
    System(const std::string& name, const std::string& version);
    virtual ~System() override;
    
    std::string getSystemName() const override { return systemName_; }
    std::string getSystemVersion() const override { return systemVersion_; }
    std::string getSystemId() const override { return systemId_; }
    
    SystemState getSystemState() const override { return state_; }
    
    bool initialize() override;
    void shutdown() override;
    
    virtual bool internalInitialize() = 0;
    virtual void internalShutdown() = 0;
    
    // Component management
    bool registerComponent(std::shared_ptr<IComponent> component) override;
    std::shared_ptr<IComponent> getComponent(const std::string& componentName) override;
    std::vector<std::shared_ptr<IComponent>> getComponents() const override;
    bool hasComponent(const std::string& componentName) const override;
    
    // Service management
    bool registerService(const std::string& serviceName, std::any service) override;
    std::any getService(const std::string& serviceName) override;
    bool hasService(const std::string& serviceName) const override;
    
    // Lifecycle
    bool start() override;
    bool stop() override;
    
    // Configuration
    void configure(const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getConfig() const override;
    
    // Metadata
    std::map<std::string, std::any> getMetadata() const override;
    
    // Statistics
    std::map<std::string, std::any> getStatistics() const override;
    
    // Validation
    bool validate() const override;
    std::string validateAll() const override;
    
protected:
    std::string systemName_;
    std::string systemVersion_;
    std::string systemId_;
    SystemState state_;
    std::map<std::string, std::any> config_;
    
    std::unordered_map<std::string, std::shared_ptr<IComponent>> components_;
    std::unordered_map<std::string, std::any> services_;
    
    mutable std::mutex componentsMutex_;
    mutable std::mutex servicesMutex_;
    
    // Statistics
    mutable std::map<std::string, std::any> statistics_;
    mutable std::mutex statisticsMutex_;
};

// Data Store Interface
class IDataStore {
public:
    virtual ~IDataStore() = default;
    
    virtual std::string getStoreName() const = 0;
    virtual std::string getStoreVersion() const = 0;
    
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    // Data operations
    virtual bool put(const std::string& key, const std::any& value) = 0;
    virtual std::any get(const std::string& key) const = 0;
    virtual bool remove(const std::string& key) = 0;
    virtual bool exists(const std::string& key) const = 0;
    
    virtual std::vector<std::string> keys() const = 0;
    virtual size_t size() const = 0;
    
    // Transaction support
    virtual bool beginTransaction() = 0;
    virtual bool commitTransaction() = 0;
    virtual bool rollbackTransaction() = 0;
    virtual bool isInTransaction() const = 0;
    
    // Store-specific operations
    virtual bool clear() = 0;
    virtual bool compact() = 0;
    virtual std::string getInfo() const = 0;
};

// Event System Interface
class IEventSystem {
public:
    virtual ~IEventSystem() = default;
    
    virtual std::string getEventSystemName() const = 0;
    virtual std::string getEventSystemVersion() const = 0;
    
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    // Event publishing
    virtual bool publish(const std::string& eventName, const std::any& data) = 0;
    virtual bool publishAsync(const std::string& eventName, const std::any& data) = 0;
    
    // Event subscription
    virtual bool subscribe(const std::string& eventName,
                          std::function<void(const std::any&)> callback) = 0;
    virtual bool unsubscribe(const std::string& eventName,
                            const std::function<void(const std::any&)>& callback) = 0;
    
    // Event filtering
    virtual bool subscribeWithFilter(const std::string& eventName,
                                   std::function<bool(const std::any&)> filter,
                                   std::function<void(const std::any&)> callback) = 0;
    
    // Event history
    virtual std::vector<std::any> getEventHistory(const std::string& eventName, 
                                                size_t maxEvents = 100) const = 0;
    virtual bool clearEventHistory(const std::string& eventName) = 0;
};

// Plugin System Interface
class IPluginSystem {
public:
    virtual ~IPluginSystem() = default;
    
    virtual std::string getPluginSystemName() const = 0;
    virtual std::string getPluginSystemVersion() const = 0;
    
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    // Plugin management
    virtual bool loadPlugin(const std::string& pluginName) = 0;
    virtual bool unloadPlugin(const std::string& pluginName) = 0;
    virtual bool enablePlugin(const std::string& pluginName) = 0;
    virtual bool disablePlugin(const std::string& pluginName) = 0;
    
    virtual std::vector<std::string> getPluginNames() const = 0;
    virtual bool hasPlugin(const std::string& pluginName) const = 0;
    
    // Plugin services
    virtual bool registerPluginService(const std::string& pluginName,
                                      const std::string& serviceName,
                                      std::any service) = 0;
    virtual std::any getPluginService(const std::string& serviceName) = 0;
    virtual bool hasPluginService(const std::string& serviceName) const = 0;
};

// System Registry Interface
class ISystemRegistry {
public:
    virtual ~ISystemRegistry() = default;
    
    // System registration
    virtual bool registerSystem(std::shared_ptr<ISystem> system) = 0;
    virtual bool unregisterSystem(const std::string& systemName) = 0;
    
    // System lookup
    virtual std::shared_ptr<ISystem> getSystem(const std::string& systemName) = 0;
    virtual std::vector<std::shared_ptr<ISystem>> getAllSystems() const = 0;
    
    // System category management
    virtual bool registerSystemCategory(const std::string& category) = 0;
    virtual std::vector<std::string> getSystemCategories() const = 0;
    virtual std::vector<std::shared_ptr<ISystem>> getSystemsByCategory(const std::string& category) const = 0;
};

// Default Implementation
class DefaultComponent : public Component {
public:
    DefaultComponent(const std::string& name, const std::string& version);
    ~DefaultComponent() override;
    
    bool internalInitialize() override;
    void internalShutdown() override;
};

class DefaultSystem : public System {
public:
    DefaultSystem(const std::string& name, const std::string& version);
    ~DefaultSystem() override;
    
    bool internalInitialize() override;
    void internalShutdown() override;
    
    // Additional system-specific functionality
    virtual bool configurePlugins(const std::map<std::string, std::any>& pluginConfig);
    virtual bool configureServices(const std::map<std::string, std::any>& serviceConfig);
    virtual bool configureComponents(const std::map<std::string, std::any>& componentConfig);
};

// System Factory
class SystemFactory {
public:
    template<typename T, typename... Args>
    static std::shared_ptr<T> createSystem(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
    
    template<typename T>
    static std::shared_ptr<T> createSystem(const std::map<std::string, std::any>& config) {
        // For templated systems, configuration would be handled differently
        throw std::runtime_error("Configuration-based system creation not implemented for type " + 
                               std::string(typeid(T).name());
    }
};

// Component Factory
class ComponentFactory {
public:
    template<typename T, typename... Args>
    static std::shared_ptr<T> createComponent(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
    
    template<typename T>
    static std::shared_ptr<T> createComponent(const std::map<std::string, std::any>& config) {
        // For templated components, configuration would be handled differently
        throw std::runtime_error("Configuration-based component creation not implemented for type " + 
                               std::string(typeid(T).name());
    }
};

} // namespace nlm