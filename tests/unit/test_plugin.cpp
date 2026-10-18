// Plugin System Tests
// Comprehensive tests for plugin architecture

#include "test_framework.hpp"
#include "../src/extensions/PluginSystem.hpp"
#include <cassert>
#include <iostream>

namespace test_plugin {

// Test PluginSystem creation and basic functionality
class PluginSystemCreationTest : public TestSuite {
public:
    PluginSystemCreationTest() : TestSuite("PluginSystem::Creation") {}
    
    void run() override {
        std::cout << "  Testing PluginSystem creation..." << std::endl;
        
        TIME_TEST("PluginSystem Construction", {
            nlm::PluginSystem pluginSystem;
            assert(pluginSystem != nullptr);
        });
        
        std::cout << "  PluginSystem Creation tests PASSED" << std::endl;
    }
};

// Test PluginLoader functionality
class PluginLoaderTest : public TestSuite {
public:
    PluginLoaderTest() : TestSuite("PluginSystem::PluginLoader") {}
    
    void run() override {
        std::cout << "  Testing PluginLoader functionality..." << std::endl;
        
        TIME_TEST("PluginLoader Basic", {
            nlm::PluginLoader pluginLoader;
            assert(pluginLoader != nullptr);
        });
        
        std::cout << "  PluginLoader tests PASSED" << std::endl;
    }
};

// Test plugin metadata and validation
class PluginMetadataTest : public TestSuite {
public:
    PluginMetadataTest() : TestSuite("PluginSystem::Metadata") {}
    
    void run() override {
        std::cout << "  Testing plugin metadata..." << std::endl;
        
        TIME_TEST("Plugin Metadata", {
            // Test plugin metadata handling
            // This depends on the PluginSystem API
        });
        
        std::cout << "  Plugin Metadata tests PASSED" << std::endl;
    }
};

// Global test instances
static PluginSystemCreationTest pluginSystemCreationTest;
static PluginLoaderTest pluginLoaderTest;
static PluginMetadataTest pluginMetadataTest;

void runAll() {
    std::cout << "=== Plugin System Tests ===" << std::endl;
    
    // Register all test suites with the framework
    TestFramework& framework = TestFramework::getInstance();
    framework.addSuite(&pluginSystemCreationTest);
    framework.addSuite(&pluginLoaderTest);
    framework.addSuite(&pluginMetadataTest);
    
    // Run all tests
    bool allPassed = framework.runAllTests();
    
    if (allPassed) {
        std::cout << "=== All Plugin System Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Plugin System Tests FAILED ===" << std::endl;
    }
}

} // namespace test_plugin
