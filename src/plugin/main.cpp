//! Plugin System Integration for NLM Neural Simulation Project
//! 
//! This file demonstrates how to integrate the plugin system with the NLM
//! neural simulation project. It shows how to:
//! - Initialize and configure the plugin system
//! - Load and manage plugins dynamically
//! - Use the plugin system with the NLM brain class
//! - Handle plugin events and hooks
//! - Use plugin-specific functionality
//! 
//! This is a comprehensive example of how to use the plugin system
//! in a real NLM neural simulation project.

#include "PluginManager.hpp"
#include "PluginRegistry.hpp"
#include "HookSystem.hpp"
#include "EventSystem.hpp"
#include "ComponentFactory.hpp"
#include "PluginConfiguration.hpp"
#include "PluginSecurityManager.hpp"
#include "PluginUpdateManager.hpp"
#include "EnhancedPlasticityPlugin.hpp"
#include "NeuromodulationEnhancementPlugin.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>

namespace nlm {

class NLMPluginSystem {
public:
    NLMPluginSystem(std::shared_ptr<Config> config)
        : config_(config), pluginManager_(std::make_shared<PluginManager>()) {
        initializePluginSystem();
    }
    
    ~NLMPluginSystem() {
        shutdownPluginSystem();
    }
    
    // Plugin system initialization
    bool initializePluginSystem() {
        // Set up core plugin system components
        setupCoreComponents();
        
        // Initialize plugin manager
        if (!pluginManager_) {
            NLM_LOG_ERROR("Failed to create plugin manager");
            return false;
        }
        
        // Create and configure plugin registry
        auto registry = std::make_shared<PluginRegistry>();
        pluginManager_->setRegistry(registry);
        
        // Create and configure hook system
        auto hookSystem = std::make_shared<HookSystem>();
        pluginManager_->setHookSystem(hookSystem);
        
        // Create and configure event system
        auto eventSystem = std::make_shared<EventSystem>();
        pluginManager_->setEventSystem(eventSystem);
        
        // Create and configure configuration system
        auto configSystem = std::make_shared<PluginConfiguration>();
        pluginManager_->setConfigurationSystem(configSystem);
        
        // Create and configure security manager
        auto securityManager = std::make_shared<PluginSecurityManager>();
        pluginManager_->setSecurityManager(securityManager);
        
        // Create and configure update manager
        auto updateManager = std::make_shared<PluginUpdateManager>();
        pluginManager_->setUpdateManager(updateManager);
        
        // Create and configure component factory
        auto factory = std::make_shared<ComponentFactory>();
        pluginManager_->setFactory(factory);
        
        // Register built-in plugins
        registerBuiltInPlugins();
        
        // Initialize with default configuration
        initializeDefaultConfiguration();
        
        return true;
    }
    
    // Plugin loading
    bool loadPlugin(const std::string& pluginName, const std::string& filePath = "") {
        if (filePath.empty()) {
            // Load built-in plugin
            return loadBuiltInPlugin(pluginName);
        } else {
            // Load external plugin from file
            return loadExternalPlugin(pluginName, filePath);
        }
    }
    
    bool loadBuiltInPlugin(const std::string& pluginName) {
        if (!pluginManager_) {
            return false;
        }
        
        // Load Enhanced Plasticity plugin
        if (pluginName == "EnhancedPlasticity") {
            PluginMetadata metadata;
            metadata.name = "EnhancedPlasticity";
            metadata.description = "Enhanced plasticity system with adaptive learning rules";
            metadata.version = PluginVersion(1, 2, 0);
            metadata.author = "NLM Research Team";
            metadata.license = "MIT";
            metadata.pluginType = "PlasticityRule";
            metadata.enabled = true;
            
            PluginConfig config;
            config.name = "EnhancedPlasticity";
            config.description = "Enhanced plasticity system";
            config.parameters = {
                {"learning_rate", std::any(0.01f)},
                {"adaptation_rate", std::any(0.001f)},
                {"max_weight_change", std::any(1.0f)},
                {"enable_adaptation", std::any(true)}
            };
            
            // Load plugin using enhanced plasticity plugin implementation
            auto plugin = std::make_unique<EnhancedPlasticityPlugin>();
            return pluginManager_->loadPlugin(metadata, std::move(plugin));
        }
        
        // Load Neuromodulation Enhancement plugin
        if (pluginName == "NeuromodulationEnhancement") {
            PluginMetadata metadata;
            metadata.name = "NeuromodulationEnhancement";
            metadata.description = "Enhanced neuromodulation system with advanced capabilities";
            metadata.version = PluginVersion(2, 0, 0);
            metadata.author = "NLM Neurodynamics Team";
            metadata.license = "MIT";
            metadata.pluginType = "Neuromodulator";
            metadata.enabled = true;
            
            PluginConfig config;
            config.name = "NeuromodulationEnhancement";
            config.description = "Enhanced neuromodulation system";
            config.parameters = {
                {"dopamine_strength", std::any(1.0f)},
                {"serotonin_effect", std::any(0.5f)},
                {"novelty_sensitivity", std::any(0.8f)},
                {"curiosity_modulation", std::any(true)},
                {"enable_advanced_processing", std::any(true)}
            };
            
            // Load plugin using neuromodulation enhancement plugin implementation
            auto plugin = std::make_unique<NeuromodulationEnhancementPlugin>();
            return pluginManager_->loadPlugin(metadata, std::move(plugin));
        }
        
        return false;
    }
    
    bool loadExternalPlugin(const std::string& pluginName, const std::string& filePath) {
        if (!pluginManager_) {
            return false;
        }
        
        // Validate plugin file
        if (!pluginManager_->validatePluginFile(filePath)) {
            NLM_LOG_ERROR("Invalid plugin file: " + filePath);
            return false;
        }
        
        // Load plugin from file
        return pluginManager_->loadPlugin(filePath);
    }
    
    // Plugin management
    std::vector<std::string> getLoadedPlugins() const {
        if (!pluginManager_) {
            return std::vector<std::string>();
        }
        
        return pluginManager_->getLoadedPlugins();
    }
    
    std::vector<std::string> getEnabledPlugins() const {
        if (!pluginManager_) {
            return std::vector<std::string>();
        }
        
        return pluginManager_->getEnabledPlugins();
    }
    
    PluginState getPluginState(const std::string& pluginName) const {
        if (!pluginManager_) {
            return PluginState::Unloaded;
        }
        
        return pluginManager_->getPluginState(pluginName);
    }
    
    // Plugin operations
    bool enablePlugin(const std::string& pluginName) {
        if (!pluginManager_) {
            return false;
        }
        
        return pluginManager_->enablePlugin(pluginName);
    }
    
    bool disablePlugin(const std::string& pluginName) {
        if (!pluginManager_) {
            return false;
        }
        
        return pluginManager_->disablePlugin(pluginName);
    }
    
    bool initializePlugin(const std::string& pluginName) {
        if (!pluginManager_) {
            return false;
        }
        
        return pluginManager_->initializePlugin(pluginName);
    }
    
    void startPlugin(const std::string& pluginName) {
        // Start individual plugin
        // This would involve calling the plugin's start method
    }
    
    void startAllPlugins() {
        if (!pluginManager_) {
            return;
        }
        
        pluginManager_->startAllPlugins();
    }
    
    void updateAllPlugins(double deltaTime) {
        if (!pluginManager_) {
            return;
        }
        
        pluginManager_->updateAllPlugins(deltaTime);
    }
    
    void stopAllPlugins() {
        if (!pluginManager_) {
            return;
        }
        
        pluginManager_->stopAllPlugins();
    }
    
    bool shutdownPlugin(const std::string& pluginName) {
        if (!pluginManager_) {
            return false;
        }
        
        return pluginManager_->shutdownPlugin(pluginName);
    }
    
    bool unloadPlugin(const std::string& pluginName) {
        if (!pluginManager_) {
            return false;
        }
        
        return pluginManager_->unloadPlugin(pluginName);
    }
    
    bool shutdownAllPlugins() {
        if (!pluginManager_) {
            return false;
        }
        
        return pluginManager_->shutdownAllPlugins();
    }
    
    void clearAllPlugins() {
        if (!pluginManager_) {
            return;
        }
        
        pluginManager_->clearAllPlugins();
    }
    
    // Event handling
    void addEventListener(std::shared_ptr<IEventListener> listener) {
        if (!pluginManager_) {
            return;
        }
        
        pluginManager_->addEventListener(listener);
    }
    
    void removeEventListener(const std::string& listenerName) {
        if (!pluginManager_) {
            return;
        }
        
        pluginManager_->removeEventListener(listenerName);
    }
    
    void emitEvent(const PluginEventData& event) {
        if (!pluginManager_) {
            return;
        }
        
        pluginManager_->notifyEvent(event);
    }
    
    // Integration with NLM brain
    bool integrateWithBrain(class Brain* brain) {
        if (!brain) {
            return false;
        }
        
        // Integrate plugins with brain systems
        integratePlasticityPlugins(brain);
        integrateNeuromodulationPlugins(brain);
        integrateDevelopmentPlugins(brain);
        integrateMemoryPlugins(brain);
        integratePredictionPlugins(brain);
        integrateCognitionPlugins(brain);
        integrateSensoryPlugins(brain);
        integrateMotorPlugins(brain);
        
        return true;
    }
    
    void disconnectFromBrain(class Brain* brain) {
        if (!brain) {
            return;
        }
        
        // Disconnect plugins from brain systems
        disconnectPlasticityPlugins(brain);
        disconnectNeuromodulationPlugins(brain);
        disconnectDevelopmentPlugins(brain);
        disconnectMemoryPlugins(brain);
        disconnectPredictionPlugins(brain);
        disconnectCognitionPlugins(brain);
        disconnectSensoryPlugins(brain);
        disconnectMotorPlugins(brain);
    }
    
    // Plugin configuration
    bool configurePlugin(const std::string& pluginName, const PluginConfig& config) {
        if (!pluginManager_) {
            return false;
        }
        
        return pluginManager_->configurePlugin(pluginName, config);
    }
    
    bool updatePluginConfiguration(const std::string& pluginName, const std::string& key, const std::any& value) {
        if (!pluginManager_) {
            return false;
        }
        
        return pluginManager_->updatePluginConfiguration(pluginName, key, value);
    }
    
    bool validateAllPluginConfigurations() const {
        if (!pluginManager_) {
            return false;
        }
        
        return pluginManager_->validateAllPluginConfigurations();
    }
    
    // Statistics and monitoring
    size_t getLoadedPluginCount() const {
        if (!pluginManager_) {
            return 0;
        }
        
        return pluginManager_->getLoadedPluginCount();
    }
    
    size_t getEnabledPluginCount() const {
        if (!pluginManager_) {
            return 0;
        }
        
        return pluginManager_->getEnabledPluginCount();
    }
    
    size_t getFailedPluginCount() const {
        if (!pluginManager_) {
            return 0;
        }
        
        return pluginManager_->getFailedPluginCount();
    }
    
    double getAveragePluginLoadTime() const {
        if (!pluginManager_) {
            return 0.0;
        }
        
        return pluginManager_->getAveragePluginLoadTime();
    }
    
    // Plugin system status
    bool isPluginSystemActive() const {
        return pluginManager_ && pluginManager_->isOperationPending();
    }
    
    bool savePluginConfigurations(const std::string& filePath) const {
        if (!pluginManager_) {
            return false;
        }
        
        return pluginManager_->savePluginConfigurations(filePath);
    }
    
    bool loadPluginConfigurations(const std::string& filePath) {
        if (!pluginManager_) {
            return false;
        }
        
        return pluginManager_->loadPluginConfigurations(filePath);
    }
    
    // Debugging and monitoring
    void enableDebugLogging(bool enable) {
        if (pluginManager_) {
            pluginManager_->enableDebugLogging(enable);
        }
    }
    
    bool isDebugLoggingEnabled() const {
        if (!pluginManager_) {
            return false;
        }
        
        return pluginManager_->isDebugLoggingEnabled();
    }
    
    void logPluginSystemStatus() const {
        if (!pluginManager_) {
            return;
        }
        
        std::cout << "Plugin System Status:" << std::endl;
        std::cout << "  Loaded Plugins: " << getLoadedPluginCount() << std::endl;
        std::cout << "  Enabled Plugins: " << getEnabledPluginCount() << std::endl;
        std::cout << "  Failed Plugins: " << getFailedPluginCount() << std::endl;
        std::cout << "  Average Load Time: " << getAveragePluginLoadTime() << " seconds" << std::endl;
    }
    
private:
    // Core plugin system components
    std::shared_ptr<Config> config_;
    std::shared_ptr<PluginManager> pluginManager_;
    
    // Plugin integration
    std::vector<std::string> activePlugins_;
    
    // Setup core plugin system components
    void setupCoreComponents() {
        // Initialize core plugin system components
        // This would set up all the necessary infrastructure
    }
    
    // Register built-in plugins
    void registerBuiltInPlugins() {
        // Register built-in plugins with plugin registry
        // This would include core plugins like EnhancedPlasticity, NeuromodulationEnhancement
    }
    
    // Initialize default configuration
    void initializeDefaultConfiguration() {
        // Initialize default plugin configuration
        // This would set up default configurations for built-in plugins
    }
    
    // Plugin integration with brain systems
    void integratePlasticityPlugins(class Brain* brain) {
        // Integrate plasticity plugins with brain
        // This would register hooks and configure plasticity systems
    }
    
    void integrateNeuromodulationPlugins(class Brain* brain) {
        // Integrate neuromodulation plugins with brain
        // This would register hooks and configure neuromodulation systems
    }
    
    void integrateDevelopmentPlugins(class Brain* brain) {
        // Integrate development plugins with brain
        // This would register hooks and configure development systems
    }
    
    void integrateMemoryPlugins(class Brain* brain) {
        // Integrate memory plugins with brain
        // This would register hooks and configure memory systems
    }
    
    void integratePredictionPlugins(class Brain* brain) {
        // Integrate prediction plugins with brain
        // This would register hooks and configure prediction systems
    }
    
    void integrateCognitionPlugins(class Brain* brain) {
        // Integrate cognition plugins with brain
        // This would register hooks and configure cognition systems
    }
    
    void integrateSensoryPlugins(class Brain* brain) {
        // Integrate sensory plugins with brain
        // This would register hooks and configure sensory systems
    }
    
    void integrateMotorPlugins(class Brain* brain) {
        // Integrate motor plugins with brain
        // This would register hooks and configure motor systems
    }
    
    // Disconnect plugins from brain systems
    void disconnectPlasticityPlugins(class Brain* brain) {
        // Disconnect plasticity plugins from brain
    }
    
    void disconnectNeuromodulationPlugins(class Brain* brain) {
        // Disconnect neuromodulation plugins from brain
    }
    
    void disconnectDevelopmentPlugins(class Brain* brain) {
        // Disconnect development plugins from brain
    }
    
    void disconnectMemoryPlugins(class Brain* brain) {
        // Disconnect memory plugins from brain
    }
    
    void disconnectPredictionPlugins(class Brain* brain) {
        // Disconnect prediction plugins from brain
    }
    
    void disconnectCognitionPlugins(class Brain* brain) {
        // Disconnect cognition plugins from brain
    }
    
    void disconnectSensoryPlugins(class Brain* brain) {
        // Disconnect sensory plugins from brain
    }
    
    void disconnectMotorPlugins(class Brain* brain) {
        // Disconnect motor plugins from brain
    }
    
    // Shutdown plugin system
    void shutdownPluginSystem() {
        // Shutdown all plugins
        shutdownAllPlugins();
        
        // Cleanup plugin system components
        pluginManager_.reset();
    }
};

// Example plugin event listener for demonstration
class ExamplePluginEventListener : public IEventListener {
public:
    ExamplePluginEventListener(const std::string& name) : name_(name) {}
    
    void onPluginEvent(const PluginEventData& event) override {
        std::cout << "[" << name_ << "] Plugin event: " << static_cast<int>(event.event) << std::endl;
        if (!event.pluginName.empty()) {
            std::cout << "  Plugin: " << event.pluginName << std::endl;
        }
        if (!event.errorMessage.empty()) {
            std::cout << "  Error: " << event.errorMessage << std::endl;
        }
    }
    
    void onHookEvent(const HookEventData& hook) override {
        std::cout << "[" << name_ << "] Hook event: " << static_cast<int>(hook.hook) << std::endl;
        if (!hook.pluginName.empty()) {
            std::cout << "  Plugin: " << hook.pluginName << std::endl;
        }
        std::cout << "  Results: " << hook.results.size() << std::endl;
    }
    
    const std::string& getName() const override {
        return name_;
    }
    
private:
    std::string name_;
};

// Example plugin configuration for demonstration
PluginConfig getExamplePluginConfiguration() {
    PluginConfig config;
    config.name = "ExamplePlugin";
    config.description = "Example plugin for demonstration";
    config.parameters = {
        {"example_parameter", std::any("example_value")},
        {"numeric_parameter", std::any(42)},
        {"boolean_parameter", std::any(true)}
    };
    config.enabled = true;
    return config;
}

} // namespace nlm

// Main function demonstrating plugin system usage
int main() {
    // Initialize NLM configuration
    auto config = std::make_shared<nlm::Config>();
    config->setDefault();
    
    // Create plugin system
    auto pluginSystem = std::make_shared<nlm::NLMPluginSystem>(config);
    
    // Create and add example event listener
    auto eventListener = std::make_shared<nlm::ExamplePluginEventListener>("ExampleListener");
    pluginSystem->addEventListener(eventListener);
    
    // Load example plugins
    std::cout << "Loading plugins..." << std::endl;
    pluginSystem->loadPlugin("EnhancedPlasticity");
    pluginSystem->loadPlugin("NeuromodulationEnhancement");
    
    // Display plugin system status
    pluginSystem->logPluginSystemStatus();
    
    // Initialize plugins
    std::cout << "Initializing plugins..." << std::endl;
    for (const auto& pluginName : pluginSystem->getLoadedPlugins()) {
        pluginSystem->initializePlugin(pluginName);
        std::cout << "Initialized: " << pluginName << std::endl;
    }
    
    // Start plugins
    std::cout << "Starting plugins..." << std::endl;
    pluginSystem->startAllPlugins();
    
    // Simulate NLM brain operation (simplified)
    std::cout << "Simulating NLM brain operation..." << std::endl;
    double deltaTime = 0.01; // 10ms timestep
    for (int step = 0; step < 100; ++step) {
        // Update plugins
        pluginSystem->updateAllPlugins(deltaTime);
        
        if (step % 10 == 0) {
            std::cout << "Step " << step << "/100" << std::endl;
        }
    }
    
    // Stop plugins
    std::cout << "Stopping plugins..." << std::endl;
    pluginSystem->stopAllPlugins();
    
    // Shutdown plugins
    std::cout << "Shutting down plugins..." << std::endl;
    for (const auto& pluginName : pluginSystem->getLoadedPlugins()) {
        pluginSystem->shutdownPlugin(pluginName);
        std::cout << "Shutdown: " << pluginName << std::endl;
    }
    
    // Unload plugins
    std::cout << "Unloading plugins..." << std::endl;
    for (const auto& pluginName : pluginSystem->getLoadedPlugins()) {
        pluginSystem->unloadPlugin(pluginName);
        std::cout << "Unloaded: " << pluginName << std::endl;
    }
    
    std::cout << "Plugin system demonstration completed successfully!" << std::endl;
    
    return 0;
}