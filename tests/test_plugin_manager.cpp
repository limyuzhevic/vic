// test_plugin_manager.cpp - Tests for PluginManager
// Tests plugin loading, management, and event handling

#include <iostream>
#include <cassert>
#include <memory>
#include <fstream>
#include "agent/PluginManager.hpp"

namespace test_plugin_manager {

// Example plugin for testing
class ExamplePlugin : public nlm::PluginInterface {
public:
    std::string getName() const override {
        return "ExamplePlugin";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    std::string getDescription() const override {
        return "Example plugin for testing PluginManager functionality";
    }
    
    std::string getAuthor() const override {
        return "NLM Test Suite";
    }
    
    bool initialize() override {
        std::cout << "    [ExamplePlugin] Initializing..." << std::endl;
        initialized_ = true;
        return true;
    }
    
    void shutdown() override {
        std::cout << "    [ExamplePlugin] Shutting down..." << std::endl;
        initialized_ = false;
    }
    
    void onBrainInitialized(std::shared_ptr<nlm::Brain> brain) override {
        std::cout << "    [ExamplePlugin] Brain initialized with " 
                  << brain->getTotalNeuronCount() << " neurons" << std::endl;
    }
    
    void onStepCompleted(nlm::SimulationStep step, nlm::Timestamp time) override {
        if (step % 100 == 0) {
            std::cout << "    [ExamplePlugin] Step " << step << " completed" << std::endl;
        }
    }
};

void testPluginManagerBasic() {
    std::cout << "Testing PluginManager basic functionality..." << std::endl;
    
    nlm::PluginManager pluginManager;
    
    // Test initial state
    auto plugins = pluginManager.listPlugins();
    assert(plugins.empty());  // Should start with no plugins
    
    // Create and load example plugin
    auto plugin = std::make_unique<ExamplePlugin>();
    bool loaded = pluginManager.loadPlugin(std::move(plugin));
    assert(loaded);
    
    // Check plugin list
    plugins = pluginManager.listPlugins();
    assert(plugins.size() == 1);
    assert(plugins[0] == "ExamplePlugin");
    
    // Get plugin instance
    auto retrievedPlugin = pluginManager.getPlugin("ExamplePlugin");
    assert(retrievedPlugin != nullptr);
    assert(retrievedPlugin->getName() == "ExamplePlugin");
    assert(retrievedPlugin->isInitialized());
    
    // Test plugin enabling
    bool disabled = pluginManager.disablePlugin("ExamplePlugin");
    assert(disabled);
    assert(!pluginManager.isPluginEnabled("ExamplePlugin"));
    
    // Re-enable plugin
    bool enabled = pluginManager.enablePlugin("ExamplePlugin");
    assert(enabled);
    assert(pluginManager.isPluginEnabled("ExamplePlugin"));
    
    std::cout << "  ✓ PluginManager basic functionality test passed" << std::endl;
}

void testPluginManagerEventHandling() {
    std::cout << "Testing PluginManager event handling..." << std::endl;
    
    nlm::PluginManager pluginManager;
    
    // Load example plugin
    auto plugin = std::make_unique<ExamplePlugin>();
    pluginManager.loadPlugin(std::move(plugin));
    
    // Create mock brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50);
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Test brain initialized event
    pluginManager.onBrainInitialized(brain);
    
    // Test step completed event
    for (int step = 0; step < 250; step += 50) {
        pluginManager.onStepCompleted(step, step * 0.001);
    }
    
    std::cout << "  ✓ PluginManager event handling test passed" << std::endl;
}

void testPluginManagerConfiguration() {
    std::cout << "Testing PluginManager configuration..." << std::endl;
    
    nlm::PluginManager pluginManager;
    
    // Load example plugin
    auto plugin = std::make_unique<ExamplePlugin>();
    pluginManager.loadPlugin(std::move(plugin));
    
    // Configure plugin
    std::map<std::string, std::string> config;
    config["setting1"] = "value1";
    config["setting2"] = "value2";
    
    pluginManager.configurePlugin("ExamplePlugin", config);
    
    // Get plugin config
    auto retrievedConfig = pluginManager.getPluginConfig("ExamplePlugin");
    assert(retrievedConfig.size() == 2);
    assert(retrievedConfig.at("setting1") == "value1");
    assert(retrievedConfig.at("setting2") == "value2");
    
    std::cout << "  ✓ PluginManager configuration test passed" << std::endl;
}

void testPluginManagerPluginValidation() {
    std::cout << "Testing PluginManager plugin validation..." << std::endl;
    
    nlm::PluginManager pluginManager;
    
    // Test validation of valid plugin
    auto validPlugin = std::make_unique<ExamplePlugin>();
    assert(pluginManager.validatePlugin(std::move(validPlugin)));
    
    std::cout << "  ✓ PluginManager plugin validation test passed" << std::endl;
}

void runAll() {
    std::cout << "=== Running PluginManager Tests ===" << std::endl;
    testPluginManagerBasic();
    testPluginManagerEventHandling();
    testPluginManagerConfiguration();
    testPluginManagerPluginValidation();
    
    std::cout << std::endl;
    std::cout << "=== All PluginManager Tests Passed ===" << std::endl;
}

} // namespace test_plugin_manager