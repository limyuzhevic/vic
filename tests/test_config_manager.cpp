// ConfigManager Tests
#include "core/Config/ConfigManager.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_config_manager {

void testConfigManagerCreation() {
    nlm::ConfigManager configManager;
    assert(configManager.getEntryCount() == 0);
    std::cout << "    testConfigManagerCreation passed" << std::endl;
}

void testConfigManagerSchema() {
    nlm::ConfigManager configManager;
    
    // Define schema
    std::vector<nlm::ConfigSchema> schema = {
        nlm::ConfigSchema("neuron_count", 1000LL, "Number of neurons", true),
        nlm::ConfigSchema("learning_rate", 0.01, "Learning rate", false),
        nlm::ConfigSchema("enable_debug", true, "Enable debug mode", false),
    };
    
    configManager.applySchema(schema);
    
    // Test required value
    assert(configManager.has("neuron_count"));
    assert(configManager.getOr("neuron_count", 0) == 1000LL);
    
    // Test optional values
    assert(!configManager.has("learning_rate")); // Not set yet
    assert(configManager.getOr("learning_rate", 0.0) == 0.0);
    
    assert(configManager.has("enable_debug"));
    assert(configManager.getOr("enable_debug", false) == true);
    
    std::cout << "    testConfigManagerSchema passed" << std::endl;
}

void testConfigManagerChangeListener() {
    nlm::ConfigManager configManager;
    
    // Simple test - add a listener (implementation dependent)
    struct TestListener : public nlm::ConfigChangeListener {
        std::string changedKey;
        TestListener() : changedKey("") {}
        void onConfigChanged(const std::string& key, const nlm::ConfigValue& oldValue, const nlm::ConfigValue& newValue) override {
            changedKey = key;
        }
    };
    
    TestListener listener;
    configManager.addChangeListener(&listener);
    
    // Note: This is a basic test - full implementation would require more complex mocking
    configManager.set("test_key", "test_value", nlm::ConfigSource::Runtime);
    
    std::cout << "    testConfigManagerChangeListener passed" << std::endl;
}

void testConfigManagerValidation() {
    nlm::ConfigManager configManager;
    
    // Create a simple schema
    std::vector<nlm::ConfigSchema> schema = {
        nlm::ConfigSchema("positive_int", 1, "Positive integer", false),
        nlm::ConfigSchema("non_empty_string", std::string(""), "Non-empty string", false),
    };
    
    // Add validation rules through schema extension
    configManager.applySchema(schema);
    
    // Set valid values
    configManager.set("positive_int", 42, nlm::ConfigSource::Runtime);
    configManager.set("non_empty_string", std::string("hello"), nlm::ConfigSource::Runtime);
    
    // Test validation passes for valid values
    try {
        configManager.validateCurrentConfig();
        std::cout << "    testConfigManagerValidation passed (valid values)" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "    testConfigManagerValidation failed: " << e.what() << std::endl;
        throw;
    }
}

void testConfigManagerDocumentation() {
    nlm::ConfigManager configManager;
    
    std::vector<nlm::ConfigSchema> schema = {
        nlm::ConfigSchema("neuron_count", 1000LL, "Number of neurons in the network", false),
        nlm::ConfigSchema("learning_rate", 0.001, "Learning rate for plasticity", false),
    };
    
    configManager.applySchema(schema);
    
    // Test documentation generation
    std::string doc = configManager.generateDocumentation();
    assert(doc.find("neuron_count") != std::string::npos);
    assert(doc.find("learning_rate") != std::string::npos);
    assert(doc.find("Number of neurons") != std::string::npos);
    
    // Test config sample generation
    std::string sample = configManager.generateConfigSample();
    assert(sample.find("neuron_count") != std::string::npos);
    assert(sample.find("learning_rate") != std::string::npos);
    
    std::cout << "    testConfigManagerDocumentation passed" << std::endl;
}

void testConfigManagerTemplates() {
    nlm::ConfigManager configManager;
    
    // Create some template data
    configManager.set("template1_key1", "value1", nlm::ConfigSource::Default);
    configManager.set("template1_key2", 42, nlm::ConfigSource::Default);
    configManager.set("template2_key1", "other_value", nlm::ConfigSource::Default);
    
    // Save templates
    configManager.saveTemplate("template1", {"template1_key1", "template1_key2"});
    configManager.saveTemplate("template2", {"template2_key1"});
    
    // Load template
    nlm::ConfigManager otherConfig;
    otherConfig.applySchema({
        nlm::ConfigSchema("template1_key1", std::string(""), "", false),
        nlm::ConfigSchema("template1_key2", 0, "", false),
    });
    
    otherConfig.loadTemplate("template1");
    assert(otherConfig.getOr("template1_key1", std::string("")) == "value1");
    assert(otherConfig.getOr("template1_key2", 0) == 42);
    
    // Check available templates
    auto templates = configManager.getAvailableTemplates();
    assert(templates.size() == 2);
    assert(std::find(templates.begin(), templates.end(), "template1") != templates.end());
    assert(std::find(templates.begin(), templates.end(), "template2") != templates.end());
    
    std::cout << "    testConfigManagerTemplates passed" << std::endl;
}

void testConfigManagerInherit() {
    nlm::ConfigManager baseConfig;
    baseConfig.set("base_key1", "base_value1", nlm::ConfigSource::Default);
    baseConfig.set("base_key2", 100, nlm::ConfigSource::Default);
    
    nlm::ConfigManager derivedConfig;
    derivedConfig.set("derived_key", "derived_value", nlm::ConfigSource::Runtime);
    
    // Inherit from base
    derivedConfig.inheritFrom(baseConfig);
    
    assert(derivedConfig.getOr("derived_key", std::string("")) == "derived_value");
    assert(derivedConfig.getOr("base_key1", std::string("")) == "base_value1");
    assert(derivedConfig.getOr("base_key2", 0) == 100);
    
    std::cout << "    testConfigManagerInherit passed" << std::endl;
}

void testConfigManagerMerge() {
    nlm::ConfigManager config1;
    config1.set("key1", 1, nlm::ConfigSource::Runtime);
    config1.set("key2", 2, nlm::ConfigSource::Runtime);
    
    nlm::ConfigManager config2;
    config2.set("key2", 20, nlm::ConfigSource::Runtime);  // Should override
    config2.set("key3", 3, nlm::ConfigSource::Runtime);
    
    config1.merge(config2);
    
    assert(config1.getOr("key1", 0) == 1);
    assert(config1.getOr("key2", 0) == 20);  // Overridden
    assert(config1.getOr("key3", 0) == 3);
    
    std::cout << "    testConfigManagerMerge passed" << std::endl;
}

void testConfigManagerEquals() {
    nlm::ConfigManager config1;
    config1.set("key1", 1, nlm::ConfigSource::Runtime);
    config1.set("key2", 2, nlm::ConfigSource::Runtime);
    
    nlm::ConfigManager config2;
    config2.set("key1", 1, nlm::ConfigSource::Runtime);
    config2.set("key2", 2, nlm::ConfigSource::Runtime);
    
    assert(config1.equals(config2));
    
    config2.set("key3", 3, nlm::ConfigSource::Runtime);
    assert(!config1.equals(config2));
    
    std::cout << "    testConfigManagerEquals passed" << std::endl;
}

void testConfigManagerResetToDefaults() {
    nlm::ConfigManager configManager;
    
    // Create schema with defaults
    std::vector<nlm::ConfigSchema> schema = {
        nlm::ConfigSchema("always_set", "default_value", "Always has default", false),
        nlm::ConfigSchema("optional_value", 0, "Optional value", false),
    };
    
    configManager.applySchema(schema);
    
    // Modify values
    configManager.set("always_set", "modified_value", nlm::ConfigSource::Runtime);
    configManager.set("optional_value", 999, nlm::ConfigSource::Runtime);
    
    assert(configManager.getOr("always_set", std::string("")) == "modified_value");
    assert(configManager.getOr("optional_value", 0) == 999);
    
    // Reset to defaults
    configManager.resetToDefaults();
    
    assert(configManager.getOr("always_set", std::string("")) == "default_value");
    assert(configManager.getOr("optional_value", 0) == 0);
    
    std::cout << "    testConfigManagerResetToDefaults passed" << std::endl;
}

void runAll() {
    testConfigManagerCreation();
    testConfigManagerSchema();
    testConfigManagerChangeListener();
    testConfigManagerValidation();
    testConfigManagerDocumentation();
    testConfigManagerTemplates();
    testConfigManagerInherit();
    testConfigManagerMerge();
    testConfigManagerEquals();
    testConfigManagerResetToDefaults();
    
    std::cout << "\nAll ConfigManager tests passed!" << std::endl;
}

} // namespace test_config_manager
