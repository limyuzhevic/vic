#pragma once
#include "../core/Exceptions/NLMErrors.hpp"
#include "../core/Config/ConfigurationManager.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <string>
#include <map>
#include <mutex>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <any>

namespace nlm {

// Forward declarations
class BrainComponent;
class BrainComponentManager;
class Neuron;
class Synapse;
class NeuralPopulation;
class NeuralRegion;
class Config;

/**
 * Exception for factory-related errors
 */
class BrainComponentFactoryException : public NLMException {
public:
    enum class FactoryErrorCode {
        CreationFailed = 1000,
        ComponentNotFound = 1001,
        InvalidConfiguration = 1002,
        ComponentAlreadyRegistered = 1003,
        ComponentDependencyError = 1004,
        SerializationFailed = 1005,
        ThreadSafetyViolation = 1006,
        CloneFailed = 1007
    };

    BrainComponentFactoryException(FactoryErrorCode code, const std::string& message,
                                   const std::string& file = "",
                                   int line = 0,
                                   const std::string& function = "");

    virtual std::string serialize() const override;
    virtual std::unique_ptr<NLMException> clone() const override;

    FactoryErrorCode getFactoryErrorCode() const noexcept {
        return static_cast<FactoryErrorCode>(getErrorCode());
    }
};

/**
 * Abstract brain component base class
 * Provides common interface for all brain components
 */
class BrainComponent {
public:
    BrainComponent() = default;
    virtual ~BrainComponent() = default;

    // Disable copying, enable moving
    BrainComponent(const BrainComponent&) = delete;
    BrainComponent& operator=(const BrainComponent&) = delete;
    BrainComponent(BrainComponent&&) noexcept = default;
    BrainComponent& operator=(BrainComponent&&) noexcept = default;

    // Component identity and type
    virtual std::string getTypeName() const = 0;
    virtual std::string getComponentId() const = 0;
    virtual ComponentType getComponentType() const = 0;

    // Component lifecycle
    virtual bool initialize(const std::shared_ptr<Config>& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;

    // Configuration management
    virtual void updateConfiguration(const std::shared_ptr<Config>& config) = 0;

    // Cloning support
    virtual std::unique_ptr<BrainComponent> clone() const = 0;
    virtual bool canClone() const = 0;

    // Serialization support
    virtual std::string serialize() const = 0;
    virtual bool deserialize(const std::string& data) = 0;
    virtual bool canSerialize() const = 0;

    // Configuration validation
    virtual bool validateConfiguration(const std::shared_ptr<Config>& config) const = 0;

    // Dependency management
    virtual const std::vector<std::string>& getDependencies() const = 0;
    virtual const std::vector<std::string>& getDependents() const = 0;

    // Performance optimization
    virtual bool isOptimized() const = 0;
    virtual void optimize() = 0;

    // Component state
    virtual std::string getState() const = 0;
    virtual void setState(const std::string& state) = 0;

    // Metadata
    virtual const std::map<std::string, std::string>& getMetadata() const = 0;
    virtual void setMetadata(const std::string& key, const std::string& value) = 0;

protected:
    mutable bool initialized_ = false;
    std::map<std::string, std::string> metadata_;
};

/**
 * Abstract factory interface for creating brain components
 */
class BrainComponentFactory {
public:
    virtual ~BrainComponentFactory() = default;

    // Create component from configuration
    virtual std::unique_ptr<BrainComponent> createComponent(
        const std::string& componentType,
        const std::shared_ptr<Config>& config,
        const std::string& componentId = "") = 0;

    // Create component with dependencies
    virtual std::unique_ptr<BrainComponent> createComponent(
        const std::string& componentType,
        const std::shared_ptr<Config>& config,
        const std::map<std::string, std::shared_ptr<BrainComponent>>& dependencies,
        const std::string& componentId = "") = 0;

    // Create component from template
    virtual std::unique_ptr<BrainComponent> createComponentFromTemplate(
        const std::string& templateName,
        const std::map<std::string, std::any>& parameters = {}) = 0;

    // Clone an existing component
    virtual std::unique_ptr<BrainComponent> cloneComponent(
        const BrainComponent& component) = 0;

    // Validate component configuration
    virtual bool validateComponentConfiguration(
        const std::string& componentType,
        const std::shared_ptr<Config>& config) const = 0;

    // Get supported component types
    virtual std::vector<std::string> getSupportedComponentTypes() const = 0;

    // Get component factory info
    virtual std::string getFactoryInfo() const = 0;

    // Factory information
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
};

/**
 * Component template for reusable component definitions
 */
struct ComponentTemplate {
    std::string name;
    std::string description;
    std::string componentType;
    std::shared_ptr<Config> defaultConfig;
    std::map<std::string, std::any> prototypeParameters;
    bool isCloneable = true;
    bool isSerializable = true;
    std::vector<std::string> dependencies;
    std::map<std::string, std::any> metadata;

    ComponentTemplate() = default;
    ComponentTemplate(const std::string& name, const std::string& description)
        : name(name), description(description), isCloneable(true), isSerializable(true) {}
};

/**
 * Component registry for managing component types and factories
 */
class ComponentRegistry {
public:
    static ComponentRegistry& getInstance() {
        static ComponentRegistry instance;
        return instance;
    }

    // Register a component factory
    bool registerFactory(const std::string& factoryName,
                        std::unique_ptr<BrainComponentFactory> factory);

    // Register a component type factory
    bool registerComponentType(const std::string& componentType,
                              std::function<std::unique_ptr<BrainComponent>(const std::shared_ptr<Config>&)> factory,
                              const std::string& factoryName = "");

    // Register a component template
    bool registerTemplate(const ComponentTemplate& templateData);

    // Get factory for component type
    std::unique_ptr<BrainComponentFactory> getFactory(const std::string& componentType) const;

    // Get factory by name
    std::unique_ptr<BrainComponentFactory> getFactoryByName(const std::string& factoryName) const;

    // Check if component type is supported
    bool isComponentTypeSupported(const std::string& componentType) const;

    // Get all registered component types
    std::vector<std::string> getSupportedComponentTypes() const;

    // Get all registered factory names
    std::vector<std::string> getFactoryNames() const;

    // Get template by name
    const ComponentTemplate* getTemplate(const std::string& templateName) const;

    // Remove factory
    bool unregisterFactory(const std::string& factoryName);

    // Remove component type
    bool unregisterComponentType(const std::string& componentType);

    // Remove template
    bool unregisterTemplate(const std::string& templateName);

    // Clear all registrations
    void clear();

    // Get registry statistics
    struct RegistryStats {
        size_t factoryCount = 0;
        size_t componentTypeCount = 0;
        size_t templateCount = 0;
        size_t totalComponentsCreated = 0;
    };

    RegistryStats getStats() const;

    // Thread safety
    void lock() const;
    void unlock() const;

private:
    ComponentRegistry() = default;
    ~ComponentRegistry() = default;

    // Disable copying
    ComponentRegistry(const ComponentRegistry&) = delete;
    ComponentRegistry& operator=(const ComponentRegistry&) = delete;

    // Factory map: factory name -> factory
    std::map<std::string, std::unique_ptr<BrainComponentFactory>> factories_;

    // Component type registry: component type -> factory function
    std::map<std::string, std::function<std::unique_ptr<BrainComponent>(const std::shared_ptr<Config>&)>> componentFactories_;

    // Template registry: template name -> template
    std::map<std::string, ComponentTemplate> templates_;

    mutable std::mutex mutex_;
    RegistryStats stats_;
};

/**
 * Component lifecycle manager
 * Handles component initialization, shutdown, configuration updates, and lifecycle tracking
 */
class BrainComponentManager {
public:
    BrainComponentManager();
    ~BrainComponentManager();

    // Disable copying
    BrainComponentManager(const BrainComponentManager&) = delete;
    BrainComponentManager& operator=(const BrainComponentManager&) = delete;

    // Component creation with dependency resolution
    std::unique_ptr<BrainComponent> createComponent(
        const std::string& componentType,
        const std::shared_ptr<Config>& config,
        const std::string& componentId = "");

    std::unique_ptr<BrainComponent> createComponent(
        const std::string& componentType,
        const std::shared_ptr<Config>& config,
        const std::map<std::string, std::shared_ptr<BrainComponent>>& dependencies,
        const std::string& componentId = "");

    // Component lifecycle management
    bool initializeComponent(BrainComponent& component);
    bool shutdownComponent(BrainComponent& component);
    bool updateComponentConfiguration(BrainComponent& component, const std::shared_ptr<Config>& config);

    // Component cloning
    std::unique_ptr<BrainComponent> cloneComponent(const BrainComponent& component);

    // Component serialization
    std::string serializeComponent(const BrainComponent& component) const;
    std::unique_ptr<BrainComponent> deserializeComponent(const std::string& data);

    // Component lifecycle tracking
    bool registerComponent(std::unique_ptr<BrainComponent> component);
    bool unregisterComponent(const std::string& componentId);
    bool hasComponent(const std::string& componentId) const;
    std::shared_ptr<BrainComponent> getComponent(const std::string& componentId) const;
    std::vector<std::shared_ptr<BrainComponent>> getAllComponents() const;

    // Component validation
    bool validateComponentConfiguration(const BrainComponent& component) const;
    bool validateComponentDependencies(const BrainComponent& component) const;

    // Configuration-driven creation with validation
    std::unique_ptr<BrainComponent> createComponentFromConfig(const std::shared_ptr<Config>& config);

    // Batch operations
    std::vector<std::unique_ptr<BrainComponent>> createComponents(
        const std::vector<std::pair<std::string, std::shared_ptr<Config>>>& componentConfigs);

    // Performance optimization
    void optimizeAllComponents();
    void cleanupInactiveComponents();

    // Statistics and monitoring
    struct ManagerStats {
        size_t activeComponentCount = 0;
        size_t totalComponentsCreated = 0;
        size_t totalComponentsDestroyed = 0;
        size_t totalComponentsCloned = 0;
        size_t totalComponentsSerialized = 0;
        std::chrono::steady_clock::time_point lastCleanupTime;
    };

    ManagerStats getStats() const;

    // Registry access
    ComponentRegistry& getRegistry() { return ComponentRegistry::getInstance(); }

    // Component type resolution
    std::string resolveComponentType(const std::string& componentType) const;

    // Thread safety
    void lock() const;
    void unlock() const;

private:
    // Component factory
    class DefaultBrainComponentFactory : public BrainComponentFactory {
    public:
        std::unique_ptr<BrainComponent> createComponent(
            const std::string& componentType,
            const std::shared_ptr<Config>& config,
            const std::string& componentId = "") override;

        std::unique_ptr<BrainComponent> createComponent(
            const std::string& componentType,
            const std::shared_ptr<Config>& config,
            const std::map<std::string, std::shared_ptr<BrainComponent>>& dependencies,
            const std::string& componentId = "") override;

        std::unique_ptr<BrainComponent> createComponentFromTemplate(
            const std::string& templateName,
            const std::map<std::string, std::any>& parameters = {}) override;

        std::unique_ptr<BrainComponent> cloneComponent(const BrainComponent& component) override;

        bool validateComponentConfiguration(
            const std::string& componentType,
            const std::shared_ptr<Config>& config) const override;

        std::vector<std::string> getSupportedComponentTypes() const override;

        std::string getFactoryInfo() const override;

        std::string getName() const override { return "DefaultBrainComponentFactory"; }
        std::string getVersion() const override { return "1.0.0"; }
        std::string getDescription() const override { return "Default factory for creating brain components"; }
    };

    std::unique_ptr<BrainComponentFactory> defaultFactory_;

    // Component storage
    std::map<std::string, std::shared_ptr<BrainComponent>> components_;
    mutable std::mutex componentsMutex_;

    // Configuration manager integration
    std::shared_ptr<ConfigurationManager> configManager_;

    // Component creation statistics
    mutable ManagerStats stats_;
    std::chrono::steady_clock::time_point lastCleanupTime_;

    // Component type to factory mapping
    std::map<std::string, std::string> componentTypeToFactory_;

    // Dependency resolution helper
    bool resolveDependencies(
        const BrainComponent& component,
        std::map<std::string, std::shared_ptr<BrainComponent>>& dependencies) const;

    // Component validation helper
    bool validateComponent(const BrainComponent& component) const;

    // Performance optimization helper
    void optimizeComponent(BrainComponent& component);

    // Cleanup helper
    void cleanupComponent(std::shared_ptr<BrainComponent>& component);
};

/**
 * Specialized factory for creating complete brain instances
 */
class BrainFactory {
public:
    BrainFactory();
    ~BrainFactory() = default;

    // Disable copying
    BrainFactory(const BrainFactory&) = delete;
    BrainFactory& operator=(const BrainFactory&) = delete;

    // Create brain instance with all components
    std::unique_ptr<Brain> createBrain(const std::shared_ptr<Config>& config);

    // Create brain with pre-existing components
    std::unique_ptr<Brain> createBrain(
        const std::shared_ptr<Config>& config,
        const std::map<std::string, std::shared_ptr<BrainComponent>>& components);

    // Create brain from template
    std::unique_ptr<Brain> createBrainFromTemplate(
        const std::string& templateName,
        const std::map<std::string, std::any>& parameters = {});

    // Add component to brain configuration
    bool addComponentToBrainConfig(std::shared_ptr<Config>& brainConfig,
                                  const std::string& componentType,
                                  const std::map<std::string, std::any>& parameters = {});

    // Get brain component templates
    std::vector<std::string> getBrainComponentTemplates() const;

    // Validate brain configuration
    bool validateBrainConfiguration(const std::shared_ptr<Config>& config) const;

    // Get factory info
    std::string getFactoryInfo() const;

    std::string getName() const { return "BrainFactory"; }
    std::string getVersion() const { return "1.0.0"; }
    std::string getDescription() const { return "Factory for creating complete brain instances with all components"; }

    // Thread safety
    void lock() const;
    void unlock() const;

private:
    // Component manager for brain component creation
    BrainComponentManager componentManager_;

    // Brain component templates
    std::map<std::string, std::shared_ptr<Config>> brainComponentTemplates_;

    // Template creation helper
    bool createBrainComponentTemplates();

    // Component integration helper
    bool integrateComponentsIntoBrain(std::shared_ptr<Brain> brain,
                                     const std::map<std::string, std::shared_ptr<BrainComponent>>& components);
};

} // namespace nlm