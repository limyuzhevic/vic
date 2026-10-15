#include <iostream>
#include <memory>
#include "plugin/IPluginManager.hpp"
#include "plugin/PluginManager.hpp"

// Simple test plugin implementation
class TestPlugin : public IPlugin {
public:
    TestPlugin() {}
    ~TestPlugin() override {}
    
    std::string getName() const override { return "TestPlugin"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Test plugin for demonstration"; }
    std::string getType() const override { return "test"; }
    
    bool initialize(void* context, std::shared_ptr<void> config) override {
        std::cout << "TestPlugin initialized!" << std::endl;
        return true;
    }
    
    void update(double timestep) override {
        static int count = 0;
        if (++count % 100 == 0) {
            std::cout << "TestPlugin update: timestep = " << timestep << std::endl;
        }
    }
    
    void shutdown() override {
        std::cout << "TestPlugin shutdown!" << std::endl;
    }
};

int main() {
    std::cout << "=== NLM Plugin System Test ===" << std::endl;
    
    // Create plugin manager
    auto& manager = PluginManager::getInstance();
    
    // Load test plugin
    if (manager.loadPlugin("TestPlugin", []() { return std::make_unique<TestPlugin>(); })) {
        std::cout << "✓ Test plugin loaded successfully" << std::endl;
        
        // Enable plugin
        if (manager.enablePlugin("TestPlugin")) {
            std::cout << "✓ Test plugin enabled" << std::endl;
            
            // Initialize plugin (no context or config for this test)
            manager.initializePlugins(nullptr, nullptr);
            
            // Simulate some updates
            std::cout << "Simulating plugin updates..." << std::endl;
            for (int i = 0; i < 250; ++i) {
                manager.updatePlugins(0.001);
            }
            
            // Shutdown plugins
            manager.shutdownPlugins();
            
            std::cout << "✓ Test plugin shutdown completed" << std::endl;
        } else {
            std::cout << "✗ Failed to enable test plugin" << std::endl;
        }
    } else {
        std::cout << "✗ Failed to load test plugin" << std::endl;
    }
    
    std::cout << "\n=== Plugin System Test Complete ===" << std::endl;
    return 0;
}
