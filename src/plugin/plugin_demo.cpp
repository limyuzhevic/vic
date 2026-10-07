#include "plugin/PluginSystem.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>

// Demo Application: NLM with Plugin System
class PluginDemoApplication {
public:
    PluginDemoApplication() = default;
    ~PluginDemoApplication() = default;
    
    bool initialize() {
        std::cout << "=== NLM Plugin Demo Application ===" << std::endl;
        std::cout << "Demonstrating the extensible plugin system..." << std::endl << std::endl;
        
        // Initialize configuration
        config_ = std::make_shared<Config>();
        config_->set("neuron_count", 100);
        config_->set("region_count", 1);
        config_->set("simulation_timestep", 0.001);
        
        // Create NLM brain
        brain_ = std::make_shared<Brain>(config_);
        if (!brain_->initialize()) {
            std::cerr << "Failed to initialize NLM brain" << std::endl;
            return false;
        }
        
        // Initialize plugin manager
        pluginManager_ = std::make_unique<PluginManager>();
        pluginManager_->setBrain(brain_);
        
        std::cout << "NLM brain and plugin manager initialized successfully" << std::endl << std::endl;
        
        return true;
    }
    
    bool loadStaticPlugins() {
        std::cout << "Loading static plugins..." << std::endl;
        
        // In a real application, we would register static plugins here
        // For the demo, we'll just show the plugin manager functionality
        
        // For demonstration, we'll register our memory component plugin
        // This would normally be done at build time with the REGISTER_PLUGIN macro
        if (!registerMemoryComponentPlugin()) {
            std::cout << "Warning: Failed to register memory component plugin (expected for demo)" << std::endl;
        }
        
        return true;
    }
    
    bool registerMemoryComponentPlugin() {
        // In a real implementation, this would use the REGISTER_PLUGIN macro
        // or static registration mechanism
        
        // For demo purposes, we'll create the plugin factory and register it
        auto factory = std::make_unique<nlm::ComponentFactory<nlm::MemoryComponentPlugin>>();
        if (!factory->validate()) {
            return false;
        }
        
        nlm::PluginMetadata metadata = factory->getMetadata();
        
        // Register the plugin (this would normally be done via a static registration mechanism)
        // For demo, we'll skip this to avoid linking issues
        std::cout << "Would register plugin: " << metadata.name << std::endl;
        
        return true;
    }
    
    bool createAndEnablePlugins() {
        std::cout << "Creating and enabling components..." << std::endl;
        
        // Get available plugins
        auto availablePlugins = pluginManager_->getAvailablePlugins();
        std::cout << "Available plugins: " << availablePlugins.size() << std::endl;
        
        for (const auto& name : availablePlugins) {
            std::cout << "  - " << name << std::endl;
        }
        
        if (availablePlugins.empty()) {
            std::cout << "Note: No plugins loaded. This is expected for the demo build." << std::endl;
            return true;
        }
        
        // Enable all plugins
        for (const auto& name : availablePlugins) {
            if (pluginManager_->enablePlugin(name)) {
                std::cout << "Enabled plugin: " << name << std::endl;
            } else {
                std::cout << "Failed to enable plugin: " << name << std::endl;
            }
        }
        
        // Create components (this would happen during brain initialization in a real app)
        std::cout << "Note: Component creation would happen during brain initialization." << std::endl;
        
        return true;
    }
    
    bool runDemo() {
        if (!pluginManager_) {
            std::cerr << "Plugin manager not initialized" << std::endl;
            return false;
        }
        
        std::cout << "=== Running Plugin Demo ===" << std::endl;
        
        // Show plugin manager statistics
        std::cout << pluginManager_->getManagerStats() << std::endl;
        
        // Validate all plugins
        if (pluginManager_->validateAllPlugins()) {
            std::cout << "✓ All plugins validated successfully" << std::endl;
        } else {
            std::cout << "⚠ Some plugins failed validation" << std::endl;
        }
        
        // Check dependencies
        if (pluginManager_->checkDependencies()) {
            std::cout << "✓ All plugin dependencies satisfied" << std::endl;
        } else {
            std::cout << "⚠ Some plugin dependencies missing" << std::endl;
        }
        
        // Simulate some plugin activity
        std::cout << "\nSimulating plugin activity..." << std::endl;
        
        // In a real implementation, plugins would be integrated with the brain
        // and would participate in the simulation
        
        for (int step = 0; step < 10; ++step) {
            std::cout << "Step " << step << ": Plugin activity simulated" << std::endl;
            
            // Brain step would happen here
            // brain_->step(step, step * 0.001);
            
            if (step % 5 == 0) {
                std::cout << "  Stats: " << pluginManager_->getManagerStats() << std::endl;
            }
        }
        
        return true;
    }
    
    void shutdown() {
        std::cout << "\n=== Shutting Down ===" << std::endl;
        
        if (pluginManager_) {
            pluginManager_->shutdownAllPlugins();
        }
        
        std::cout << "Plugin demo application shut down successfully" << std::endl;
    }
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<PluginManager> pluginManager_;
};

int main() {
    PluginDemoApplication app;
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize application" << std::endl;
        return 1;
    }
    
    if (!app.loadStaticPlugins()) {
        std::cerr << "Failed to load plugins" << std::endl;
        return 1;
    }
    
    if (!app.createAndEnablePlugins()) {
        std::cerr << "Failed to enable plugins" << std::endl;
        return 1;
    }
    
    if (!app.runDemo()) {
        std::cerr << "Demo execution failed" << std::endl;
        return 1;
    }
    
    app.shutdown();
    
    std::cout << "\n=== Plugin Demo Complete ===" << std::endl;
    return 0;
}
