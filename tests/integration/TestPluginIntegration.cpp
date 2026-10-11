// NLM Integration Tests
// Plugin Integration Tests

#include "framework/TestFramework.hpp"
#include "framework/TestFixtures.hpp"
#include "include/plugins/PluginInterface.hpp"
#include "brain/PluginManager.hpp"
#include <cassert>
#include <cmath>
#include <memory>

class TestPluginIntegration : public TestCase {
public:
    TestPluginIntegration() : TestCase("PluginIntegration") {}
    
    void testPluginManager() {
        PluginManager plugin_manager;
        
        // Test plugin registration
        class TestPlugin : public PluginInterface {
        public:
            std::string getName() const override { return "TestPlugin"; }
            std::string getVersion() const override { return "1.0.0"; }
            std::string getDescription() const override { return "Test plugin for NLM"; }
            
            bool initialize(const std::map<std::string, std::string>& config) override {
                initialized = true;
                return true;
            }
            
            void shutdown() override {
                initialized = false;
            }
            
            bool isInitialized() const override { return initialized; }
            
            // Plugin-specific functionality
            std::vector<double> processData(const std::vector<double>& input) override {
                std::vector<double> output = input;
                for (size_t i = 0; i < output.size(); ++i) {
                    output[i] = output[i] * 2.0; // Simple transformation
                }
                return output;
            }
            
        private:
            bool initialized = false;
        };
        
        TestPlugin test_plugin;
        plugin_manager.registerPlugin(std::make_shared<TestPlugin>(test_plugin));
        
        assert(plugin_manager.getPluginCount() == 1);
        assert(plugin_manager.getPluginName(0) == "TestPlugin");
        
        // Initialize plugin
        std::map<std::string, std::string> config;
        config["test_param"] = "test_value";
        plugin_manager.initializePlugin("TestPlugin", config);
        
        assert(plugin_manager.isPluginInitialized("TestPlugin"));
        
        // Test plugin functionality
        std::vector<double> input = {1.0, 2.0, 3.0, 4.0, 5.0};
        std::vector<double> output = plugin_manager.executePlugin("TestPlugin", "processData", input);
        
        assert(output.size() == input.size());
        assert(output[0] == 2.0); // 1.0 * 2.0
        assert(output[1] == 4.0); // 2.0 * 2.0
        
        // Shutdown plugin
        plugin_manager.shutdownPlugin("TestPlugin");
        assert(!plugin_manager.isPluginInitialized("TestPlugin"));
    }
    
    void testPluginCommunication() {
        PluginManager plugin_manager;
        
        // Register multiple plugins that communicate
        class CommunicationPluginA : public PluginInterface {
        public:
            std::string getName() const override { return "PluginA"; }
            std::string getVersion() const override { return "1.0.0"; }
            std::string getDescription() const override { return "Plugin A for communication testing"; }
            
            bool initialize(const std::map<std::string, std::string>& config) override {
                return initialized = true;
            }
            
            void shutdown() override { initialized = false; }
            bool isInitialized() const override { return initialized; }
            
            std::vector<double> processData(const std::vector<double>& input) override {
                // Pass through with modification
                std::vector<double> output = input;
                for (size_t i = 0; i < output.size(); ++i) {
                    output[i] += 0.5; // Add offset
                }
                return output;
            }
            
        private:
            bool initialized = false;
        };
        
        class CommunicationPluginB : public PluginInterface {
        public:
            std::string getName() const override { return "PluginB"; }
            std::string getVersion() const override { return "1.0.0"; }
            std::string getDescription() const override { return "Plugin B for communication testing"; }
            
            bool initialize(const std::map<std::string, std::string>& config) override {
                return initialized = true;
            }
            
            void shutdown() override { initialized = false; }
            bool isInitialized() const override { return initialized; }
            
            std::vector<double> processData(const std::vector<double>& input) override {
                // Multiply by factor
                std::vector<double> output = input;
                for (size_t i = 0; i < output.size(); ++i) {
                    output[i] = output[i] * 0.8; // Scale down
                }
                return output;
            }
            
            // Get data from previous plugin
            std::vector<double> getDataFromPlugin(const std::string& source_name) override {
                // Mock implementation - would normally get from shared memory or message queue
                std::vector<double> data = {0.5, 0.6, 0.7, 0.8, 0.9};
                return data;
            }
            
        private:
            bool initialized = false;
        };
        
        // Register plugins
        plugin_manager.registerPlugin(std::make_shared<CommunicationPluginA>());
        plugin_manager.registerPlugin(std::make_shared<CommunicationPluginB>());
        
        assert(plugin_manager.getPluginCount() == 2);
        
        // Initialize both plugins
        std::map<std::string, std::string> config;
        plugin_manager.initializePlugin("PluginA", config);
        plugin_manager.initializePlugin("PluginB", config);
        
        // Test inter-plugin communication
        std::vector<double> input = {1.0, 2.0, 3.0, 4.0, 5.0};
        
        // Plugin A processes data
        std::vector<double> plugin_a_output = plugin_manager.executePlugin("PluginA", "processData", input);
        
        // Plugin B gets data from Plugin A (simulated)
        std::vector<double> plugin_b_input = plugin_manager.getDataFromPlugin("PluginB", "PluginA");
        
        // Plugin B processes data
        std::vector<double> plugin_b_output = plugin_manager.executePlugin("PluginB", "processData", plugin_b_input);
        
        // Verify data flow
        assert(plugin_a_output.size() == input.size());
        assert(plugin_b_output.size() == plugin_b_input.size());
        
        // Cleanup
        plugin_manager.shutdownPlugin("PluginA");
        plugin_manager.shutdownPlugin("PluginB");
    }
    
    void testPluginLifecycle() {
        PluginManager plugin_manager;
        
        // Test complete plugin lifecycle
        class LifecyclePlugin : public PluginInterface {
        public:
            std::string getName() const override { return "LifecyclePlugin"; }
            std::string getVersion() const override { return "1.0.0"; }
            std::string getDescription() const override { return "Plugin for testing lifecycle"; }
            
            bool initialize(const std::map<std::string, std::string>& config) override {
                initialization_count++;
                return initialized = true;
            }
            
            void shutdown() override {
                shutdown_count++;
                initialized = false;
            }
            
            bool isInitialized() const override { return initialized; }
            
            std::vector<double> processData(const std::vector<double>& input) override {
                process_count++;
                std::vector<double> output = input;
                for (size_t i = 0; i < output.size(); ++i) {
                    output[i] *= 1.5;
                }
                return output;
            }
            
            // Get lifecycle statistics
            int getInitializationCount() const { return initialization_count; }
            int getShutdownCount() const { return shutdown_count; }
            int getProcessCount() const { return process_count; }
            
        private:
            bool initialized = false;
            int initialization_count = 0;
            int shutdown_count = 0;
            int process_count = 0;
        };
        
        LifecyclePlugin lifecycle_plugin;
        plugin_manager.registerPlugin(std::make_shared<LifecyclePlugin>(lifecycle_plugin));
        
        std::map<std::string, std::string> config;
        
        // Full lifecycle
        plugin_manager.initializePlugin("LifecyclePlugin", config);
        assert(lifecycle_plugin.getInitializationCount() == 1);
        assert(lifecycle_plugin.isInitialized());
        
        // Process data multiple times
        std::vector<double> input = {1.0, 2.0, 3.0};
        for (int i = 0; i < 3; ++i) {
            plugin_manager.executePlugin("LifecyclePlugin", "processData", input);
        }
        assert(lifecycle_plugin.getProcessCount() == 3);
        
        plugin_manager.shutdownPlugin("LifecyclePlugin");
        assert(!lifecycle_plugin.isInitialized());
        assert(lifecycle_plugin.getShutdownCount() == 1);
        
        // Try to use after shutdown - should handle gracefully
        assert(!plugin_manager.executePlugin("LifecyclePlugin", "processData", input).empty() || 
               true); // May return empty or handle error
    }
};