// Plugin example demonstrating the NLM plugin system
// This example shows how to create and use plugins in the NLM brain

#include <iostream>
#include <memory>
#include "plugin/IPluginManager.hpp"

// Simple plugin implementation
class ExamplePlugin : public IPlugin {
public:
    ExamplePlugin() : initialized_(false) {}
    
    std::string getName() const override { return "ExamplePlugin"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Example plugin demonstrating the plugin system"; }
    std::string getType() const override { return "example"; }
    
    bool initialize(void* context, std::shared_ptr<void> config) override {
        std::cout << "[ExamplePlugin] Initializing..." << std::endl;
        initialized_ = true;
        return true;
    }
    
    void update(double timestep) override {
        if (initialized_) {
            static int count = 0;
            if (++count % 50 == 0) {
                std::cout << "[ExamplePlugin] Update: timestep = " << timestep << std::endl;
            }
        }
    }
    
    void shutdown() override {
        std::cout << "[ExamplePlugin] Shutting down..." << std::endl;
        initialized_ = false;
    }
    
private:
    bool initialized_;
};

int main() {
    std::cout << "=== NLM Plugin System Example ===" << std::endl;
    
    // Get plugin manager
    IPluginManager& manager = getPluginManager();
    
    // Register our example plugin
    std::cout << "Registering example plugin..." << std::endl;
    manager.registerPlugin("ExamplePlugin", "1.0.0", 
                          "Example plugin demonstrating the plugin system",
                          []() { return std::make_unique<ExamplePlugin>(); },
                          "example");
    
    // Load the plugin
    std::cout << "Loading plugin..." << std::endl;
    if (manager.loadPlugin("ExamplePlugin", 
                         []() { return std::make_unique<ExamplePlugin>(); })) {
        std::cout << "✓ Plugin loaded successfully" << std::endl;
        
        // Enable the plugin
        std::cout << "Enabling plugin..." << std::endl;
        if (manager.enablePlugin("ExamplePlugin")) {
            std::cout << "✓ Plugin enabled" << std::endl;
            
            // Initialize the plugin
            std::cout << "Initializing plugin..." << std::endl;
            manager.initializePlugins(nullptr, nullptr);
            
            // Run some updates
            std::cout << "Running plugin updates (100 steps)..." << std::endl;
            for (int i = 0; i < 100; ++i) {
                manager.updatePlugins(0.1);
            }
            
            // Shutdown the plugin
            std::cout << "Shutting down plugin..." << std::endl;
            manager.shutdownPlugins();
            
            std::cout << "✓ Plugin shutdown completed" << std::endl;
        } else {
            std::cout << "✗ Failed to enable plugin" << std::endl;
        }
    } else {
        std::cout << "✗ Failed to load plugin" << std::endl;
    }
    
    // Display plugin information
    std::cout << "\n=== Plugin Information ===" << std::endl;
    std::cout << "Loaded plugins:" << std::endl;
    auto pluginNames = manager.getPluginNames();
    for (const auto& name : pluginNames) {
        auto info = manager.getPluginInfo(name);
        std::cout << "  - " << info.name << " (" << info.version << ") - " << info.description << std::endl;
        std::cout << "    Type: " << info.type << ", Enabled: " << (info.enabled ? "Yes" : "No") << std::endl;
    }
    
    std::cout << "\n=== Plugin System Example Complete ===" << std::endl;
    return 0;
}
