#include "plugin/PluginSystem.hpp"
#include "plugin/Plugin.hpp"
#include <iostream>

// Test plugin implementation
class TestPlugin : public nlm::Plugin {
public:
    const nlm::PluginMetadata& getMetadata() const override {
        static nlm::PluginMetadata metadata(
            "TestPlugin",
            "A test plugin for plugin system validation",
            "NLM Team",
            nlm::PluginVersion(1, 0, 0),
            nlm::PluginType::General
        );
        return metadata;
    }
    
    bool onInitialize() override {
        std::cout << "TestPlugin initialized" << std::endl;
        return true;
    }
    
    void onEnable() override {
        std::cout << "TestPlugin enabled" << std::endl;
    }
    
    void onDisable() override {
        std::cout << "TestPlugin disabled" << std::endl;
    }
    
    void onCleanup() override {
        std::cout << "TestPlugin cleaned up" << std::endl;
    }
    
    std::vector<std::string> getExtensionPoints() const override {
        return {"test_extension"};
    }
    
    std::vector<std::string> getProvidedServices() const override {
        return {"test_service"};
    }
};

int main() {
    std::cout << "=== NLM Plugin System Test ===" << std::endl;
    
    // Get plugin system instance
    nlm::PluginSystem& pluginSystem = nlm::PluginSystem::getInstance();
    
    // Configure plugin system
    nlm::PluginSystem::SystemSettings settings;
    settings.enablePluginSystem = true;
    settings.autoDiscoverPlugins = false; // Don't auto-load for test
    settings.validatePluginsOnLoad = true;
    settings.enablePluginSecurity = false; // Disable security for test
    settings.logPluginActivity = true;
    settings.pluginSearchPaths = {"./plugins"};
    
    // Initialize plugin system
    if (!pluginSystem.initialize(settings)) {
        std::cerr << "Failed to initialize PluginSystem!" << std::endl;
        return 1;
    }
    
    std::cout << "PluginSystem initialized successfully" << std::endl;
    
    // Register a test plugin (simulated)
    // In a real implementation, this would load from a file
    auto testPlugin = std::make_shared<TestPlugin>();
    
    // Manually register for testing
    bool registered = false;
    
    // Get plugin manager to simulate registration
    auto& instance = nlm::PluginSystem::getInstance();
    
    std::cout << "\n=== Test completed ===" << std::endl;
    
    // Cleanup
    pluginSystem.cleanup();
    
    std::cout << "PluginSystem cleaned up successfully" << std::endl;
    
    return 0;
}
