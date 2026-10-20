//! Component Factory Implementation for NLM Neural Simulation Project
//! 
//! This module implements the ComponentFactory class with its associated
//! methods and provides comprehensive component creation functionality.

#include "ComponentFactory.hpp"

namespace nlm {

class ComponentFactory::Impl {
public:
    // Factory storage
    std::unordered_map<std::string, std::unordered_map<std::string, ComponentFactoryFunc>> componentFactories;
    std::unordered_map<std::string, ComponentTemplate> componentTemplates;
    std::unordered_map<std::string, ComponentMetadata> componentMetadata;
    
    // Statistics
    size_t totalFactories = 0;
    size_t registeredComponents = 0;
    size_t registeredTemplates = 0;
    size_t totalComponentCreations = 0;
    size_t successfulComponentCreations = 0;
    size_t failedComponentCreations = 0;
    double averageCreationTime = 0.0;
    
    // Configuration
    size_t maxConcurrentCreations = 10;
    double componentCreationTimeout = 30.0;
    bool enableComponentCaching = true;
    
    // Integration
    std::shared_ptr<PluginManager> pluginManager;
    std::shared_ptr<PluginRegistry> registry;
    std::shared_ptr<PluginSecurityManager> securityManager;
    std::shared_ptr<PluginUpdateManager> updateManager;
    std::shared_ptr<HookSystem> hookSystem;
    std::shared_ptr<EventSystem> eventSystem;
    
    // Callbacks
    std::vector<std::function<void(const std::string&, const std::string&, bool, const std::string&)>> creationCallbacks;
    
    // Flags and settings
    bool debugLogging = false;
    bool factoryActive = true;
    
    // Constructor
    Impl() : totalFactories(0), registeredComponents(0), registeredTemplates(0),
        totalComponentCreations(0), successfulComponentCreations(0), 
        failedComponentCreations(0), averageCreationTime(0.0), factoryActive(true) {}
};

ComponentFactory::ComponentFactory() : pImpl(std::make_unique<Impl>()) {
    initializeImpl();
}

ComponentFactory::~ComponentFactory() {
    cleanupImpl();
}

void ComponentFactory::initializeImpl() {
    // Initialize factory
}

void ComponentFactory::cleanupImpl() {
    // Cleanup factory
    pImpl->componentFactories.clear();
    pImpl->componentTemplates.clear();
    pImpl->componentMetadata.clear();
    pImpl->creationCallbacks.clear();
}

std::unique_ptr<PluginBase> ComponentFactory::createPlugin(const PluginMetadata& metadata, const PluginConfig& config) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Find factory for plugin type
        auto it = pImpl->componentFactories.find(metadata.pluginType);
        if (it == pImpl->componentFactories.end()) {
            return nullptr; // No factory for this plugin type
        }
        
        // Check if plugin already registered
        auto nameIt = pImpl->componentFactories.find(metadata.name);
        if (nameIt != pImpl->componentFactories.end()) {
            auto& typeMap = nameIt->second;
            auto typeIt = typeMap.find(metadata.pluginType);
            if (typeIt != typeMap.end()) {
                return typeIt->second(config); // Use cached factory
            }
        }
        
        // Find main factory
        auto mainIt = it->second.find("(PluginBase)");
        if (mainIt == it->second.end()) {
            return nullptr; // No suitable factory found
        }
        
        // Create plugin using factory
        std::unique_ptr<PluginBase> plugin = mainIt->second(config);
        if (!plugin) {
            return nullptr;
        }
        
        // Store for caching
        pImpl->componentFactories[metadata.name][metadata.pluginType] = mainIt->second;
        
        // Update statistics
        pImpl->totalComponentCreations++;
        pImpl->successfulComponentCreations++;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        updateAverageCreationTime(duration);
        
        // Call creation callback
        for (const auto& callback : pImpl->creationCallbacks) {
            callback(metadata.name, metadata.pluginType, true, "");
        }
        
        return plugin;
    } catch (const std::exception& e) {
        // Handle error
        pImpl->failedComponentCreations++;
        
        // Call creation callback with error
        for (const auto& callback : pImpl->creationCallbacks) {
            callback(metadata.name, metadata.pluginType, false, e.what());
        }
        
        return nullptr;
    }
}

std::unique_ptr<PluginBase> ComponentFactory::createPlugin(const std::string& pluginName, const PluginConfig& config) {
    // Find plugin metadata
    const PluginMetadata* metadata = nullptr;
    if (pImpl->pluginManager) {
        metadata = pImpl->pluginManager->getPluginMetadata(pluginName);
    } else if (pImpl->registry) {
        metadata = pImpl->registry->findPlugin(pluginName);
    }
    
    if (!metadata) {
        return nullptr;
    }
    
    return createPlugin(*metadata, config);
}

std::unique_ptr<PluginBase> ComponentFactory::createPluginInstance(const PluginMetadata& metadata, 
                                                                 const PluginConfig& config,
                                                                 const std::string& entryPoint) {
    // Create plugin instance from metadata
    return createPlugin(metadata, config);
}

template<typename ComponentType>
std::unique_ptr<ComponentType> ComponentFactory::createComponent(const std::string& componentName,
                                                                     const std::unordered_map<std::string, std::any>& parameters) {
    PluginConfig config;
    config.name = componentName;
    config.parameters = parameters;
    
    std::unique_ptr<PluginBase> baseComponent = createPlugin(componentName, config);
    if (!baseComponent) {
        return nullptr;
    }
    
    // Try to cast to ComponentType
    std::unique_ptr<ComponentType> component = std::unique_ptr<ComponentType>(dynamic_cast<ComponentType*>(baseComponent.release()));
    if (!component) {
        return nullptr;
    }
    
    return component;
}

template<typename ComponentType, typename... Args>
std::unique_ptr<ComponentType> ComponentFactory::createComponent(Args&&... args) {
    // Create component with specific arguments
    return std::make_unique<ComponentType>(std::forward<Args>(args)...);
}

std::unique_ptr<PlasticityRulePlugin> ComponentFactory::createPlasticityRulePlugin(const PluginConfig& config) {
    // Create plasticity rule plugin
    return std::unique_ptr<PlasticityRulePlugin>(nullptr); // Placeholder
}

std::unique_ptr<NeuromodulatorPlugin> ComponentFactory::createNeuromodulatorPlugin(const PluginConfig& config) {
    // Create neuromodulator plugin
    return std::unique_ptr<NeuromodulatorPlugin>(nullptr); // Placeholder
}

std::unique_ptr<DevelopmentPlugin> ComponentFactory::createDevelopmentPlugin(const PluginConfig& config) {
    // Create development plugin
    return std::unique_ptr<DevelopmentPlugin>(nullptr); // Placeholder
}

bool ComponentFactory::registerComponentFactory(const std::string& componentType,
                                               const std::function<std::unique_ptr<PluginBase>(const PluginConfig&)>& factory) {
    return withFactoryLock([&]() {
        pImpl->componentFactories[componentType]["(PluginBase)"] = factory;
        pImpl->totalFactories++;
        return true;
    });
}

bool ComponentFactory::registerComponentFactory(const std::string& componentName,
                                               const std::string& componentType,
                                               const std::function<std::unique_ptr<PluginBase>(const PluginConfig&)>& factory) {
    return withFactoryLock([&]() {
        pImpl->componentFactories[componentType][componentName] = factory;
        pImpl->componentMetadata[componentName] = ComponentMetadata(componentName, componentType);
        pImpl->registeredComponents++;
        return true;
    });
}

bool ComponentFactory::unregisterComponentFactory(const std::string& componentType) {
    return withFactoryLock([&]() {
        auto it = pImpl->componentFactories.find(componentType);
        if (it == pImpl->componentFactories.end()) {
            return false;
        }
        
        pImpl->totalFactories--;
        pImpl->componentFactories.erase(it);
        
        // Remove associated metadata
        for (auto metadataIt = pImpl->componentMetadata.begin(); 
             metadataIt != pImpl->componentMetadata.end(); ) {
            if (metadataIt->second.type == componentType) {
                metadataIt = pImpl->componentMetadata.erase(metadataIt);
                pImpl->registeredComponents--;
            } else {
                ++metadataIt;
            }
        }
        
        return true;
    });
}

bool ComponentFactory::unregisterComponentFactory(const std::string& componentName, const std::string& componentType) {
    return withFactoryLock([&]() {
        auto it = pImpl->componentFactories.find(componentType);
        if (it == pImpl->componentFactories.end()) {
            return false;
        }
        
        auto nameIt = it->second.find(componentName);
        if (nameIt == it->second.end()) {
            return false;
        }
        
        it->second.erase(nameIt);
        
        if (it->second.empty()) {
            pImpl->totalFactories--;
            pImpl->componentFactories.erase(it);
        }
        
        pImpl->componentMetadata.erase(componentName);
        pImpl->registeredComponents--;
        
        return true;
    });
}

const std::function<std::unique_ptr<PluginBase>(const PluginConfig&)>* ComponentFactory::getComponentFactory(const std::string& componentType) const {
    return withFactoryLock([&]() {
        auto it = pImpl->componentFactories.find(componentType);
        if (it == pImpl->componentFactories.end()) {
            return (const std::function<std::unique_ptr<PluginBase>(const PluginConfig&)>*)nullptr;
        }
        
        // Return the first factory
        auto factoryIt = it->second.find("(PluginBase)");
        if (factoryIt == it->second.end()) {
            return (const std::function<std::unique_ptr<PluginBase>(const PluginConfig&)>*)nullptr;
        }
        
        return &factoryIt->second;
    });
}

const std::function<std::unique_ptr<PluginBase>(const PluginConfig&)>* ComponentFactory::getComponentFactory(const std::string& componentName, const std::string& componentType) const {
    return withFactoryLock([&]() {
        auto it = pImpl->componentFactories.find(componentType);
        if (it == pImpl->componentFactories.end()) {
            return (const std::function<std::unique_ptr<PluginBase>(const PluginConfig&)>*)nullptr;
        }
        
        auto nameIt = it->second.find(componentName);
        if (nameIt == it->second.end()) {
            return (const std::function<std::unique_ptr<PluginBase>(const PluginConfig&)>*)nullptr;
        }
        
        return &nameIt->second;
    });
}

std::vector<std::string> ComponentFactory::getRegisteredComponentTypes() const {
    return withFactoryLock([&]() {
        std::vector<std::string> types;
        for (const auto& pair : pImpl->componentFactories) {
            types.push_back(pair.first);
        }
        return types;
    });
}

std::vector<std::string> ComponentFactory::getRegisteredComponentNames(const std::string& componentType) const {
    return withFactoryLock([&]() {
        std::vector<std::string> names;
        auto it = pImpl->componentFactories.find(componentType);
        if (it != pImpl->componentFactories.end()) {
            for (const auto& pair : it->second) {
                names.push_back(pair.first);
            }
        }
        return names;
    });
}

std::vector<std::pair<std::string, std::string>> ComponentFactory::getAllRegisteredComponents() const {
    return withFactoryLock([&]() {
        std::vector<std::pair<std::string, std::string>> result;
        for (const auto& typePair : pImpl->componentFactories) {
            for (const auto& namePair : typePair.second) {
                result.push_back(std::make_pair(namePair.first, typePair.first));
            }
        }
        return result;
    });
}

template<typename ComponentType>
std::unique_ptr<ComponentType> ComponentFactory::cloneComponent(const ComponentType& component, 
                                                            const std::unordered_map<std::string, std::any>& newParameters) {
    // Clone component
    return std::make_unique<ComponentType>(component); // Placeholder
}

template<typename ComponentType>
std::unique_ptr<ComponentType> ComponentFactory::copyComponent(const ComponentType& component,
                                                            const std::unordered_map<std::string, std::any>& newParameters) {
    // Copy component
    return std::make_unique<ComponentType>(component); // Placeholder
}

bool ComponentFactory::configureComponent(std::unique_ptr<PluginBase>& component, const PluginConfig& config) {
    if (!component) {
        return false;
    }
    
    // Configure component
    // This would involve calling component's load method
    return component->load(config);
}

bool ComponentFactory::updateComponentConfiguration(std::unique_ptr<PluginBase>& component, 
                                                    const std::string& key, 
                                                    const std::any& value) {
    if (!component) {
        return false;
    }
    
    // Update component configuration
    // This would involve calling component's configuration update method
    return false; // Placeholder
}

bool ComponentFactory::validateComponentConfiguration(std::unique_ptr<PluginBase>& component) const {
    if (!component) {
        return false;
    }
    
    return component->validateConfiguration();
}

std::string ComponentFactory::getComponentConfigurationError(std::unique_ptr<PluginBase>& component) const {
    if (!component) {
        return "Component is null";
    }
    
    return component->getConfigurationError();
}

bool ComponentFactory::initializeComponent(std::unique_ptr<PluginBase>& component) {
    if (!component) {
        return false;
    }
    
    return component->initialize();
}

void ComponentFactory::startComponent(std::unique_ptr<PluginBase>& component) {
    if (component && component->getState() == PluginState::Initialized) {
        component->start();
    }
}

void ComponentFactory::updateComponent(std::unique_ptr<PluginBase>& component, double deltaTime) {
    if (component && component->getState() == PluginState::Running) {
        component->update(deltaTime);
    }
}

void ComponentFactory::stopComponent(std::unique_ptr<PluginBase>& component) {
    if (component && (component->getState() == PluginState::Initialized || 
                      component->getState() == PluginState::Running)) {
        component->stop();
    }
}

bool ComponentFactory::shutdownComponent(std::unique_ptr<PluginBase>& component) {
    if (!component) {
        return false;
    }
    
    return component->shutdown();
}

bool ComponentFactory::unloadComponent(std::unique_ptr<PluginBase>& component) {
    if (!component) {
        return false;
    }
    
    return component->unload();
}

std::vector<std::string> ComponentFactory::initializeComponents(std::vector<std::unique_ptr<PluginBase>>& components) {
    std::vector<std::string> failedComponents;
    
    for (auto& component : components) {
        if (!initializeComponent(component)) {
            failedComponents.push_back(component->getName());
        }
    }
    
    return failedComponents;
}

void ComponentFactory::startAllComponents(std::vector<std::unique_ptr<PluginBase>>& components) {
    for (auto& component : components) {
        startComponent(component);
    }
}

void ComponentFactory::updateAllComponents(std::vector<std::unique_ptr<PluginBase>>& components, double deltaTime) {
    for (auto& component : components) {
        updateComponent(component, deltaTime);
    }
}

void ComponentFactory::stopAllComponents(std::vector<std::unique_ptr<PluginBase>>& components) {
    for (auto& component : components) {
        stopComponent(component);
    }
}

std::vector<std::string> ComponentFactory::shutdownAllComponents(std::vector<std::unique_ptr<PluginBase>>& components) {
    std::vector<std::string> failedComponents;
    
    for (auto& component : components) {
        if (!shutdownComponent(component)) {
            failedComponents.push_back(component->getName());
        }
    }
    
    return failedComponents;
}

std::vector<std::string> ComponentFactory::unloadAllComponents(std::vector<std::unique_ptr<PluginBase>>& components) {
    std::vector<std::string> failedComponents;
    
    for (auto& component : components) {
        if (!unloadComponent(component)) {
            failedComponents.push_back(component->getName());
        }
    }
    
    return failedComponents;
}

bool ComponentFactory::registerComponentTemplate(const std::string& templateName,
                                                 const std::string& componentType,
                                                 const std::unordered_map<std::string, std::any>& defaultParameters) {
    return withFactoryLock([&]() {
        ComponentTemplate template;
        template.type = componentType;
        template.defaultParameters = defaultParameters;
        template.created = std::chrono::system_clock::now();
        
        pImpl->componentTemplates[templateName] = template;
        pImpl->registeredTemplates++;
        
        return true;
    });
}

bool ComponentFactory::unregisterComponentTemplate(const std::string& templateName) {
    return withFactoryLock([&]() {
        auto it = pImpl->componentTemplates.find(templateName);
        if (it == pImpl->componentTemplates.end()) {
            return false;
        }
        
        pImpl->componentTemplates.erase(it);
        pImpl->registeredTemplates--;
        
        return true;
    });
}

std::unique_ptr<PluginBase> ComponentFactory::createFromTemplate(const std::string& templateName,
                                                                const std::unordered_map<std::string, std::any>& overrideParameters) {
    return withFactoryLock([&]() {
        auto it = pImpl->componentTemplates.find(templateName);
        if (it == pImpl->componentTemplates.end()) {
            return std::unique_ptr<PluginBase>(nullptr);
        }
        
        // Merge with override parameters
        std::unordered_map<std::string, std::any> parameters = it->second.defaultParameters;
        parameters.insert(overrideParameters.begin(), overrideParameters.end());
        
        // Create component
        return createComponent<PluginBase>(
            "template_" + templateName,
            parameters
        );
    });
}

std::vector<std::string> ComponentFactory::getRegisteredTemplates() const {
    return withFactoryLock([&]() {
        std::vector<std::string> names;
        for (const auto& pair : pImpl->componentTemplates) {
            names.push_back(pair.first);
        }
        return names;
    });
}

bool ComponentFactory::validateComponentParameters(const std::string& componentType,
                                                    const std::unordered_map<std::string, std::any>& parameters) const {
    return true; // Placeholder
}

std::vector<std::string> ComponentFactory::getComponentParameterErrors(const std::string& componentType,
                                                                   const std::unordered_map<std::string, std::any>& parameters) const {
    return std::vector<std::string>(); // Placeholder
}

const PluginMetadata* ComponentFactory::getComponentMetadata(const std::string& componentName) const {
    if (pImpl->pluginManager) {
        return pImpl->pluginManager->getPluginMetadata(componentName);
    } else if (pImpl->registry) {
        return pImpl->registry->findPlugin(componentName);
    }
    return nullptr;
}

const PluginConfig* ComponentFactory::getComponentConfig(const std::string& componentName) const {
    if (pImpl->pluginManager) {
        return pImpl->pluginManager->getPluginConfig(componentName);
    }
    return nullptr;
}

PluginState ComponentFactory::getComponentState(const std::string& componentName) const {
    if (pImpl->pluginManager) {
        return pImpl->pluginManager->getPluginState(componentName);
    }
    return PluginState::Unloaded;
}

PluginVersion ComponentFactory::getComponentVersion(const std::string& componentName) const {
    const PluginMetadata* metadata = getComponentMetadata(componentName);
    if (metadata) {
        return metadata->version;
    }
    return PluginVersion();
}

std::string ComponentFactory::getComponentDescription(const std::string& componentName) const {
    const PluginMetadata* metadata = getComponentMetadata(componentName);
    if (metadata) {
        return metadata->description;
    }
    return "";
}

std::vector<std::string> ComponentFactory::getComponentDependencies(const std::string& componentName) const {
    const PluginMetadata* metadata = getComponentMetadata(componentName);
    if (metadata) {
        return metadata->dependencies;
    }
    return std::vector<std::string>();
}

std::vector<std::string> ComponentFactory::getComponentOptionalDependencies(const std::string& componentName) const {
    const PluginMetadata* metadata = getComponentMetadata(componentName);
    if (metadata) {
        return metadata->optionalDependencies;
    }
    return std::vector<std::string>();
}

ComponentFactory::ComponentFactoryStats ComponentFactory::getStatistics() const {
    ComponentFactoryStats stats;
    
    withFactoryLock([&]() {
        stats.totalFactories = pImpl->totalFactories;
        stats.registeredComponents = pImpl->registeredComponents;
        stats.registeredTemplates = pImpl->registeredTemplates;
        stats.totalComponentCreations = pImpl->totalComponentCreations;
        stats.successfulComponentCreations = pImpl->successfulComponentCreations;
        stats.failedComponentCreations = pImpl->failedComponentCreations;
        stats.averageCreationTime = pImpl->averageCreationTime;
    });
    
    return stats;
}

void ComponentFactory::optimizeFactoryCreation() {
    // Optimize factory creation
    // Clear cache and rebuild
    clearComponentFactoryCache();
    rebuildFactoryRegistry();
}

void ComponentFactory::cacheComponentFactories() {
    // Cache component factories for faster access
}

void ComponentFactory::clearComponentFactoryCache() {
    // Clear component factory cache
}

void ComponentFactory::rebuildFactoryRegistry() {
    // Rebuild factory registry
}

void ComponentFactory::setMaxConcurrentCreations(size_t maxCreations) {
    pImpl->maxConcurrentCreations = maxCreations;
}

size_t ComponentFactory::getMaxConcurrentCreations() const {
    return pImpl->maxConcurrentCreations;
}

void ComponentFactory::setComponentCreationTimeout(double timeout) {
    pImpl->componentCreationTimeout = timeout;
}

double ComponentFactory::getComponentCreationTimeout() const {
    return pImpl->componentCreationTimeout;
}

void ComponentFactory::setEnableComponentCaching(bool enable) {
    pImpl->enableComponentCaching = enable;
}

bool ComponentFactory::isComponentCachingEnabled() const {
    return pImpl->enableComponentCaching;
}

void ComponentFactory::pauseAllComponents() {
    // Pause all components
}

void ComponentFactory::resumeAllComponents() {
    // Resume all components
}

void ComponentFactory::shutdownAllComponentFactories() {
    // Shutdown all component factories
}

bool ComponentFactory::isComponentFactoryActive() const {
    return pImpl->factoryActive;
}

void ComponentFactory::setPluginManager(std::shared_ptr<PluginManager> pluginManager) {
    pImpl->pluginManager = pluginManager;
}

void ComponentFactory::setRegistry(std::shared_ptr<PluginRegistry> registry) {
    pImpl->registry = registry;
}

void ComponentFactory::setSecurityManager(std::shared_ptr<PluginSecurityManager> securityManager) {
    pImpl->securityManager = securityManager;
}

void ComponentFactory::setUpdateManager(std::shared_ptr<PluginUpdateManager> updateManager) {
    pImpl->updateManager = updateManager;
}

void ComponentFactory::setHookSystem(std::shared_ptr<HookSystem> hookSystem) {
    pImpl->hookSystem = hookSystem;
}

void ComponentFactory::setEventSystem(std::shared_ptr<EventSystem> eventSystem) {
    pImpl->eventSystem = eventSystem;
}

void ComponentFactory::addLoadCallback(const PluginLoadCallback& callback) {
    pImpl->creationCallbacks.push_back(callback);
}

void ComponentFactory::enableDebugLogging(bool enable) {
    pImpl->debugLogging = enable;
}

bool ComponentFactory::isDebugLoggingEnabled() const {
    return pImpl->debugLogging;
}

void ComponentFactory::logComponentCreation(const std::string& componentType, const std::string& componentName, bool success, const std::string& error) const {
    if (pImpl->debugLogging) {
        // Log component creation
    }
}

void ComponentFactory::logComponentFactoryError(const std::string& error) const {
    if (pImpl->debugLogging) {
        // Log factory error
    }
}

bool ComponentFactory::exportComponentFactories(const std::string& filePath) const {
    // Export component factories
    return false; // Placeholder
}

bool ComponentFactory::importComponentFactories(const std::string& filePath) {
    // Import component factories
    return false; // Placeholder
}

std::string ComponentFactory::exportComponentFactoriesAsJSON() const {
    // Export component factories as JSON
    return ""; // Placeholder
}

bool ComponentFactory::importComponentFactoriesFromJSON(const std::string& jsonData) {
    // Import component factories from JSON
    return false; // Placeholder
}

bool ComponentFactory::testAllComponentFactories() const {
    // Test all component factories
    return false; // Placeholder
}

std::vector<std::string> ComponentFactory::getFactoryTestResults() const {
    // Get factory test results
    return std::vector<std::string>(); // Placeholder
}

bool ComponentFactory::testComponentFactory(const std::string& componentType) const {
    // Test component factory
    return false; // Placeholder
}

void ComponentFactory::updateAverageCreationTime(double creationTime) {
    withFactoryLock([&]() {
        // Update average creation time
        if (pImpl->totalComponentCreations > 0) {
            pImpl->averageCreationTime = 
                ((pImpl->averageCreationTime * (pImpl->totalComponentCreations - 1)) + creationTime) /
                pImpl->totalComponentCreations;
        } else {
            pImpl->averageCreationTime = creationTime;
        }
    });
}

void ComponentFactory::initializeImpl() {
    // Initialize component factory
}

void ComponentFactory::cleanupImpl() {
    // Cleanup component factory
}

// Template implementations
template<typename ComponentType>
std::unique_ptr<ComponentType> ComponentFactory::createComponent(const std::string& componentName,
                                                                 const std::unordered_map<std::string, std::any>& parameters) {
    PluginConfig config;
    config.name = componentName;
    config.parameters = parameters;
    
    std::unique_ptr<PluginBase> baseComponent = createPlugin(componentName, config);
    if (!baseComponent) {
        return nullptr;
    }
    
    return std::unique_ptr<ComponentType>(dynamic_cast<ComponentType*>(baseComponent.release()));
}

// Plugin Configuration Implementation

PluginConfiguration::Impl::Impl() : historyEnabled(false), updatesPaused(false), debugLogging(false) {}

// Component Factory Implementation

} // namespace nlm

// Helper function implementations
namespace PluginUtils {
    std::unique_ptr<PluginBase> createPlugin(const PluginMetadata& metadata, const PluginConfig& config) {
        // Create plugin using factory
        return std::unique_ptr<PluginBase>(nullptr); // Placeholder
    }
}

} // namespace nlm