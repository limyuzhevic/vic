#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "../core/Config/Config.hpp"
#include "Brain.hpp"
#include "commands/AdvancedBrainCommands.hpp"
#include "commands/ExtensionSystem.hpp"
#include "ExtensionManager.hpp"
#include "factory/ComponentFactory.hpp"

namespace nlm {

// Advanced Brain Interface for extended functionality
class AdvancedBrainInterface {
public:
    // Advanced Brain Interface Configuration
    struct Config {
        bool enableAdvancedCommands = true;
        bool enableExtensionSystem = true;
        bool enableComponentFactory = true;
        bool enableEventSystem = true;
        bool enablePluginArchitecture = true;
        bool autoLoadExtensions = true;
        std::string extensionPath = "./extensions";
        bool enableLogging = true;
        bool enableDebugMode = false;
        
        Config() = default;
    };
    
    // Extension listener interface
    class ExtensionListener {
    public:
        virtual ~ExtensionListener() = default;
        
        // Handle extension events
        virtual void onExtensionLoaded(const std::string& extensionName) = 0;
        virtual void onExtensionUnloaded(const std::string& extensionName) = 0;
        virtual void onExtensionError(const std::string& extensionName, const std::string& error) = 0;
        
        // Handle component events
        virtual void onComponentCreated(const std::string& componentName, const std::string& type) = 0;
        virtual void onComponentDestroyed(const std::string& componentName) = 0;
    };
    
    // Get singleton instance
    static AdvancedBrainInterface& getInstance() {
        static AdvancedBrainInterface instance;
        return instance;
    }
    
    // Initialize advanced brain interface
    bool initialize(const Config& config, std::shared_ptr<Config> globalConfig, std::shared_ptr<Brain> brain);
    
    // Shutdown advanced brain interface
    void shutdown();
    
    // Add extension listener
    bool addExtensionListener(std::unique_ptr<ExtensionListener> listener);
    
    // Remove extension listener
    bool removeExtensionListener(const std::string& listenerName);
    
    // Get extension system
    ExtensionSystem* getExtensionSystem() { return extensionSystem_.get(); }
    
    // Get extension manager
    ExtensionManager* getExtensionManager() { return extensionManager_.get(); }
    
    // Get command registry
    AdvancedBrainCommandRegistry* getCommandRegistry() { return commandRegistry_.get(); }
    
    // Get component factory
    template<typename ComponentType>
    ComponentFactory<ComponentType>* getComponentFactory() { return componentFactory_.get(); }
    
    // Execute advanced command
    bool executeCommand(const std::string& name,
                       const std::vector<std::string>& args,
                       const std::unordered_map<std::string, std::any>& parameters = {});
    
    // Create custom component
    template<typename ComponentType>
    std::shared_ptr<ComponentType> createComponent(const std::string& type,
                                                   const std::unordered_map<std::string, std::string>& params = {});
    
    // Register custom component type
    template<typename ComponentType>
    bool registerComponentType(const std::string& name,
                              std::function<std::shared_ptr<ComponentType>(std::shared_ptr<Config>, std::shared_ptr<Brain>)> factory);
    
    // Register extension point handler
    bool registerExtensionPointHandler(const std::string& pointName,
                                       ExtensionPointHandler handler,
                                       const std::string& handlerName = "",
                                       int priority = 0);
    
    // Emit event
    bool emitEvent(const std::string& eventName,
                   const std::any& data = {},
                   const std::string& sourceExtension = "");
    
    // Load extensions from path
    bool loadExtensionsFromPath(const std::string& path);
    
    // Get system status
    std::string getStatus() const;
    
    // Check if advanced brain interface is initialized
    bool isInitialized() const { return isInitialized_; }
    
    // Get initialization error
    std::string getInitializationError() const { return initError_; }
    
private:
    AdvancedBrainInterface() = default;
    AdvancedBrainInterface(const AdvancedBrainInterface&) = delete;
    AdvancedBrainInterface& operator=(const AdvancedBrainInterface&) = delete;
    
    mutable std::mutex mutex_;
    bool isInitialized_ = false;
    std::string initError_;
    
    // Configuration
    Config config_;
    std::shared_ptr<Config> globalConfig_;
    std::shared_ptr<Brain> brain_;
    
    // Components
    std::unique_ptr<ExtensionSystem> extensionSystem_;
    std::unique_ptr<ExtensionManager> extensionManager_;
    std::unique_ptr<AdvancedBrainCommandRegistry> commandRegistry_;
    std::unique_ptr<ComponentFactory<void>> componentFactory_;
    
    // Extension listeners
    std::vector<std::unique_ptr<ExtensionListener>> extensionListeners_;
    
    // Initialization helper methods
    bool initializeExtensionSystem();
    bool initializeExtensionManager();
    bool initializeCommandRegistry();
    bool initializeComponentFactory();
    bool registerAdvancedCommands();
    bool registerExtensionPointHandlers();
    
    // Event handling helper methods
    void notifyExtensionListeners(const std::string& eventType, const std::string& extensionName, const std::string& message = "");
};

// Template method implementations

template<typename ComponentType>
std::shared_ptr<ComponentType> AdvancedBrainInterface::createComponent(const std::string& type,
                                                                       const std::unordered_map<std::string, std::string>& params) {
    if (!componentFactory_) {
        NLM_LOG_ERROR("Component factory not initialized");
        return nullptr;
    }
    
    return ComponentFactory<ComponentType>::create(type, globalConfig_, brain_, params);
}

template<typename ComponentType>
bool AdvancedBrainInterface::registerComponentType(const std::string& name,
                                                 std::function<std::shared_ptr<ComponentType>(std::shared_ptr<Config>, std::shared_ptr<Brain>)> factory) {
    if (!componentFactory_) {
        NLM_LOG_ERROR("Component factory not initialized");
        return false;
    }
    
    return ComponentRegistry<ComponentType>::getInstance().registerComponent(name, factory);
}

} // namespace nlm
