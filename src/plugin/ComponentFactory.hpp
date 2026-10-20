//! Component Factory for NLM Neural Simulation Project
//! 
//! This module implements the ComponentFactory class which provides factory methods
//! for creating plugin instances and plugin components. The ComponentFactory:
//! - Creates plugin instances from metadata and configuration
//! - Instantiates plugin components (neurons, synapses, plasticity rules, etc.)
//! - Manages plugin component lifecycle
//! - Provides component templates and factories
//! - Supports component cloning and copying
//! - Manages component configuration and initialization
//! 
//! The ComponentFactory serves as a centralized factory for all plugin components,
//! enabling consistent creation patterns and lifecycle management across the plugin system.

#pragma once

#include "PluginBase.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <mutex>
#include <any>
#include <typeindex>
#include <unordered_set>
#include <algorithm>
#include <stdexcept>

namespace nlm {

class ComponentFactory {
public:
    ComponentFactory();
    ~ComponentFactory();
    
    // Disable copying
    ComponentFactory(const ComponentFactory&) = delete;
    ComponentFactory& operator=(const ComponentFactory&) = delete;
    
    // Enable moving
    ComponentFactory(ComponentFactory&&) noexcept = default;
    ComponentFactory& operator=(ComponentFactory&&) noexcept = default;
    
    // Plugin instance creation
    std::unique_ptr<PluginBase> createPlugin(const PluginMetadata& metadata, const PluginConfig& config);
    std::unique_ptr<PluginBase> createPlugin(const std::string& pluginName, const PluginConfig& config);
    std::unique_ptr<PluginBase> createPluginInstance(const PluginMetadata& metadata, const PluginConfig& config,
                                                    const std::string& entryPoint = "");
    
    // Plugin component creation
    template<typename ComponentType>
    std::unique_ptr<ComponentType> createComponent(const std::string& componentName,
                                                  const std::unordered_map<std::string, std::any>& parameters = {});
    
    template<typename ComponentType, typename... Args>
    std::unique_ptr<ComponentType> createComponent(Args&&... args);
    
    // Plugin type-specific component creation
    std::unique_ptr<PlasticityRulePlugin> createPlasticityRulePlugin(const PluginConfig& config);
    std::unique_ptr<NeuromodulatorPlugin> createNeuromodulatorPlugin(const PluginConfig& config);
    std::unique_ptr<DevelopmentPlugin> createDevelopmentPlugin(const PluginConfig& config);
    
    // Component factory registration
    bool registerComponentFactory(const std::string& componentType,
                                 const std::function<std::unique_ptr<PluginBase>(const PluginConfig&)>& factory);
    bool registerComponentFactory(const std::string& componentName,
                                 const std::string& componentType,
                                 const std::function<std::unique_ptr<PluginBase>(const PluginConfig&)>& factory);
    bool unregisterComponentFactory(const std::string& componentType);
    bool unregisterComponentFactory(const std::string& componentName, const std::string& componentType);
    
    // Component factory lookup
    const std::function<std::unique_ptr<PluginBase>(const PluginConfig&)>* getComponentFactory(const std::string& componentType) const;
    const std::function<std::unique_ptr<PluginBase>(const PluginConfig&)>* getComponentFactory(const std::string& componentName, const std::string& componentType) const;
    std::vector<std::string> getRegisteredComponentTypes() const;
    std::vector<std::string> getRegisteredComponentNames(const std::string& componentType) const;
    std::vector<std::pair<std::string, std::string>> getAllRegisteredComponents() const;
    
    // Component cloning and copying
    template<typename ComponentType>
    std::unique_ptr<ComponentType> cloneComponent(const ComponentType& component, const std::unordered_map<std::string, std::any>& newParameters = {});
    
    template<typename ComponentType>
    std::unique_ptr<ComponentType> copyComponent(const ComponentType& component, const std::unordered_map<std::string, std::any>& newParameters = {});
    
    // Component configuration
    bool configureComponent(std::unique_ptr<PluginBase>& component, const PluginConfig& config);
    bool updateComponentConfiguration(std::unique_ptr<PluginBase>& component, const std::string& key, const std::any& value);
    bool validateComponentConfiguration(std::unique_ptr<PluginBase>& component) const;
    std::string getComponentConfigurationError(std::unique_ptr<PluginBase>& component) const;
    
    // Component lifecycle management
    bool initializeComponent(std::unique_ptr<PluginBase>& component);
    void startComponent(std::unique_ptr<PluginBase>& component);
    void updateComponent(std::unique_ptr<PluginBase>& component, double deltaTime);
    void stopComponent(std::unique_ptr<PluginBase>& component);
    bool shutdownComponent(std::unique_ptr<PluginBase>& component);
    bool unloadComponent(std::unique_ptr<PluginBase>& component);
    
    // Component lifecycle for multiple components
    std::vector<std::string> initializeComponents(std::vector<std::unique_ptr<PluginBase>>& components);
    void startAllComponents(std::vector<std::unique_ptr<PluginBase>>& components);
    void updateAllComponents(std::vector<std::unique_ptr<PluginBase>>& components, double deltaTime);
    void stopAllComponents(std::vector<std::unique_ptr<PluginBase>>& components);
    std::vector<std::string> shutdownAllComponents(std::vector<std::unique_ptr<PluginBase>>& components);
    std::vector<std::string> unloadAllComponents(std::vector<std::unique_ptr<PluginBase>>& components);
    
    // Component template management
    bool registerComponentTemplate(const std::string& templateName,
                                  const std::string& componentType,
                                  const std::unordered_map<std::string, std::any>& defaultParameters);
    bool unregisterComponentTemplate(const std::string& templateName);
    std::unique_ptr<PluginBase> createFromTemplate(const std::string& templateName,
                                                 const std::unordered_map<std::string, std::any>& overrideParameters = {});
    std::vector<std::string> getRegisteredTemplates() const;
    
    // Component parameter validation
    bool validateComponentParameters(const std::string& componentType,
                                    const std::unordered_map<std::string, std::any>& parameters) const;
    std::vector<std::string> getComponentParameterErrors(const std::string& componentType,
                                                       const std::unordered_map<std::string, std::any>& parameters) const;
    
    // Component information
    const PluginMetadata* getComponentMetadata(const std::string& componentName) const;
    const PluginConfig* getComponentConfig(const std::string& componentName) const;
    PluginState getComponentState(const std::string& componentName) const;
    PluginVersion getComponentVersion(const std::string& componentName) const;
    std::string getComponentDescription(const std::string& componentName) const;
    std::vector<std::string> getComponentDependencies(const std::string& componentName) const;
    std::vector<std::string> getComponentOptionalDependencies(const std::string& componentName) const;
    
    // Component factory statistics
    struct ComponentFactoryStats {
        size_t totalFactories;
        size_t registeredComponents;
        size_t registeredTemplates;
        size_t totalComponentCreations;
        size_t successfulComponentCreations;
        size_t failedComponentCreations;
        double averageCreationTime;
        
        ComponentFactoryStats() : totalFactories(0), registeredComponents(0), registeredTemplates(0),
            totalComponentCreations(0), successfulComponentCreations(0), failedComponentCreations(0),
            averageCreationTime(0.0) {}
    };
    
    ComponentFactoryStats getStatistics() const;
    
    // Component factory optimization
    void optimizeFactoryCreation();
    void cacheComponentFactories();
    void clearComponentFactoryCache();
    void rebuildFactoryRegistry();
    
    // Component factory configuration
    void setMaxConcurrentCreations(size_t maxCreations);
    size_t getMaxConcurrentCreations() const;
    void setComponentCreationTimeout(double timeout);
    double getComponentCreationTimeout() const;
    void setEnableComponentCaching(bool enable);
    bool isComponentCachingEnabled() const;
    
    // Component factory lifecycle
    void pauseAllComponents();
    void resumeAllComponents();
    void shutdownAllComponentFactories();
    bool isComponentFactoryActive() const;
    
    // Component factory integration
    void setPluginManager(std::shared_ptr<PluginManager> pluginManager);
    void setRegistry(std::shared_ptr<PluginRegistry> registry);
    void setSecurityManager(std::shared_ptr<PluginSecurityManager> securityManager);
    void setUpdateManager(std::shared_ptr<PluginUpdateManager> updateManager);
    void setHookSystem(std::shared_ptr<HookSystem> hookSystem);
    void setEventSystem(std::shared_ptr<EventSystem> eventSystem);
    
    // Component factory debugging and monitoring
    void enableDebugLogging(bool enable);
    bool isDebugLoggingEnabled() const;
    void logComponentCreation(const std::string& componentType, const std::string& componentName, bool success, const std::string& error = "") const;
    void logComponentFactoryError(const std::string& error) const;
    
    // Component factory export/import
    bool exportComponentFactories(const std::string& filePath) const;
    bool importComponentFactories(const std::string& filePath);
    std::string exportComponentFactoriesAsJSON() const;
    bool importComponentFactoriesFromJSON(const std::string& jsonData);
    
    // Component factory testing
    bool testAllComponentFactories() const;
    std::vector<std::string> getFactoryTestResults() const;
    bool testComponentFactory(const std::string& componentType) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void initializeImpl();
    void cleanupImpl();
    
    // Thread-safe operations
    template<typename T>
    T withFactoryLock(T func) {
        std::unique_lock<std::mutex> lock(factoryMutex_);
        return func();
    }
    
    // Factory function type definitions
    using ComponentFactoryFunc = std::function<std::unique_ptr<PluginBase>(const PluginConfig&)>;
    using ComponentTemplate = std::unordered_map<std::string, std::any>;
    
    // Component metadata
    struct ComponentMetadata {
        std::string name;
        std::string type;
        PluginMetadata pluginMetadata;
        PluginConfig config;
        std::chrono::system_clock::time_point creationTime;
        size_t creationCount;
        
        ComponentMetadata(const std::string& n, const std::string& t)
            : name(n), type(t), creationTime(std::chrono::system_clock::now()), creationCount(0) {}
    };
    
    // Factory statistics
    struct FactoryStats {
        std::string componentType;
        size_t creationCount;
        size_t successCount;
        size_t failureCount;
        double averageCreationTime;
        std::chrono::system_clock::time_point lastCreationTime;
        
        FactoryStats() : creationCount(0), successCount(0), failureCount(0),
            averageCreationTime(0.0), lastCreationTime(std::chrono::system_clock::now()) {}
    };
}; // namespace nlm