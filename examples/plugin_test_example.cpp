// Plugin System Test Example
// ================================

// This example demonstrates how to use the NLM plugin system.
// It shows basic plugin discovery, loading, and lifecycle management.

#include "src/plugin/manager/PluginManager.hpp"
#include "src/plugin/core/PluginBase.hpp"
#include <iostream>
#include <memory>

// Example plugin implementation
class ExamplePlugin : public nlm::plugin::ThreadSafePlugin {
public:
    ExamplePlugin() {
        metadata_.name = "Example Plugin";
        metadata_.version = "1.0.0";
        metadata_.description = "Example plugin demonstrating plugin system usage";
        metadata_.author = "NLM Team";
        metadata_.license = "MIT";
        metadata_.category = "example";
    }
    
    const std::string& getName() const override {
        return metadata_.name;
    }
    
    const std::string& getVersion() const override {
        return metadata_.version;
    }
    
    nlm::plugin::PluginStatus getStatus() const override {
        return status_;
    }
    
    bool load(const nlohmann::json& config) override {
        std::cout << "ExamplePlugin::load() called" << std::endl;
        // Load configuration
        config_ = config;
        return true;
    }
    
    bool initialize() override {
        std::cout << "ExamplePlugin::initialize() called" << std::endl;
        // Initialize resources
        return true;
    }
    
    bool run() override {
        std::cout << "ExamplePlugin::run() called" << std::endl;
        // Main plugin functionality
        return true;
    }
    
    bool shutdown() override {
        std::cout << "ExamplePlugin::shutdown() called" << std::endl;
        // Clean up resources
        return true;
    }
    
    bool unload() override {
        std::cout << "ExamplePlugin::unload() called" << std::endl;
        // Clean up all resources
        return true;
    }
    
    void setEnabled(bool enabled) override {
        enabled_ = enabled;
    }
    
    bool isEnabled() const override {
        return enabled_;
    }
    
    nlohmann::json getState() const override {
        return state_;
    }
    
    bool setState(const nlohmann::json& state) override {
        state_ = state;
        return true;
    }
    
    bool hasError() const override {
        return !errors_.empty();
    }
    
    std::string getError() const override {
        if (errors_.empty()) return "";
        return errors_.back();
    }
    
    std::vector<std::string> getErrors() const override {
        return errors_;
    }
    
    nlohmann::json getDefaultConfig() const override {
        return nlohmann::json({
            {"exampleParameter", 1.0},
            {"exampleString", "test"},
            {"exampleBoolean", true}
        });
    }
    
    bool validateConfig(const nlohmann::json& config) override {
        return config.contains("exampleParameter") &&
               config["exampleParameter"].is_number();
    }
    
    void lock() override {
        mutex_.lock();
    }
    
    void unlock() override {
        mutex_.unlock();
    }
    
    bool tryLock() override {
        return mutex_.try_lock();
    }
    
    bool registerPlugin() override {
        std::cout << "ExamplePlugin::registerPlugin() called" << std::endl;
        // Register with plugin manager
        return true;
    }
    
    bool unregisterPlugin() override {
        std::cout << "ExamplePlugin::unregisterPlugin() called" << std::endl;
        // Unregister from plugin manager
        return true;
    }
    
    // Example-specific functionality
    void performExampleOperation() {
        std::cout << "ExamplePlugin performing example operation" << std::endl;
    }
    
private:
    std::mutex mutex_;
    bool enabled_ = true;
    nlohmann::json config_;
    nlohmann::json state_;
    std::vector<std::string> errors_;
};

// Plugin factory function
extern "C" nlm::plugin::Plugin* createPlugin() {
    return new ExamplePlugin();
}

int main() {
    std::cout << "NLM Plugin System Test" << std::endl;
    std::cout << "=======================" << std::endl;
    
    try {
        // Create plugin manager
        nlm::plugin::PluginManagerConfig config;
        config.pluginDirectory = "plugins";
        config.enableAutoLoad = false; // Don't auto-load for this test
        config.enableAutoStart = false;
        
        nlm::plugin::PluginManager manager(config);
        
        // Create a test plugin instance directly
        std::cout << "\n1. Creating plugin instance directly..." << std::endl;
        std::unique_ptr<nlm::plugin::Plugin> plugin = std::make_unique<ExamplePlugin>();
        
        std::cout << "   Plugin name: " << plugin->getName() << std::endl;
        std::cout << "   Plugin version: " << plugin->getVersion() << std::endl;
        
        // Test plugin lifecycle
        std::cout << "\n2. Testing plugin lifecycle..." << std::endl;
        
        // Load plugin
        nlohmann::json testConfig = {
            {"exampleParameter", 2.0},
            {"exampleString", "modified"},
            {"exampleBoolean", false}
        };
        
        if (plugin->load(testConfig)) {
            std::cout << "   Plugin loaded successfully" << std::endl;
        }
        
        // Initialize plugin
        if (plugin->initialize()) {
            std::cout << "   Plugin initialized successfully" << std::endl;
        }
        
        // Run plugin
        if (plugin->run()) {
            std::cout << "   Plugin run successfully" << std::endl;
        }
        
        // Check plugin state
        std::cout << "   Plugin is enabled: " << (plugin->isEnabled() ? "yes" : "no") << std::endl;
        std::cout << "   Plugin has errors: " << (plugin->hasError() ? "yes" : "no") << std::endl;
        
        // Perform example operation
        static_cast<ExamplePlugin*>(plugin.get())->performExampleOperation();
        
        // Shutdown plugin
        if (plugin->shutdown()) {
            std::cout << "   Plugin shut down successfully" << std::endl;
        }
        
        // Unload plugin
        if (plugin->unload()) {
            std::cout << "   Plugin unloaded successfully" << std::endl;
        }
        
        // Test plugin manager with example plugin
        std::cout << "\n3. Testing plugin manager..." << std::endl;
        
        // Create a temporary plugin file for testing
        std::string pluginPath = "test_example_plugin.so";
        
        // Note: In a real scenario, we would compile the example plugin
        // as a shared library and then load it using the plugin manager.
        // For this test, we'll demonstrate the concept.
        
        std::cout << "   Plugin manager created successfully" << std::endl;
        std::cout << "   Plugin manager is running: " << (manager.isRunning() ? "yes" : "no") << std::endl;
        
        // Display plugin manager statistics
        auto stats = manager.getManagerStats();
        std::cout << "   Plugin manager statistics:" << std::endl;
        std::cout << "     Loaded plugins: " << stats["loadedPluginCount"] << std::endl;
        std::cout << "     Total plugins: " << stats["totalPluginCount"] << std::endl;
        std::cout << "     Load progress: " << stats["loadProgress"] * 100 << "%" << std::endl;
        
        // Test thread safety
        std::cout << "\n4. Testing thread safety..." << std::endl;
        
        // Try to lock plugin (thread safety test)
        if (plugin->tryLock()) {
            std::cout << "   Plugin lock acquired successfully" << std::endl;
            plugin->unlock();
        }
        
        std::cout << "\nPlugin system test completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
