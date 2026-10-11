// NLM Unit Testing Framework
// Configuration Tests

#include "framework/TestFramework.hpp"
#include "brain/ConfigSystem.hpp"
#include "brain/BrainConfig.hpp"
#include <cassert>
#include <string>
#include <map>

class TestConfig : public TestCase {
public:
    TestConfig() : TestCase("Config") {}
    
    void testConfigSystemInitialization() {
        ConfigSystem config;
        
        // Should have default values
        assert(config.getConfigValue("brain.neuron.count", 100) == 100);
        assert(config.getConfigValue("brain.synaptic.connectivity", 0.5) == 0.5);
        assert(config.getConfigValue("brain.learning.rate", 0.01) == 0.01);
    }
    
    void testConfigValidation() {
        ConfigSystem config;
        
        // Test parameter validation
        BrainConfig valid_config;
        valid_config.neuron_count = 1000;
        valid_config.synaptic_connectivity = 0.3;
        valid_config.learning_rate = 0.001;
        
        // Config should be valid
        assert(config.validateConfig(valid_config));
        
        // Invalid config
        BrainConfig invalid_config;
        invalid_config.neuron_count = -100; // Negative should be invalid
        assert(!config.validateConfig(invalid_config));
        
        invalid_config.neuron_count = 100; // Fix
        invalid_config.synaptic_connectivity = 1.5; // > 1 should be invalid
        assert(!config.validateConfig(invalid_config));
    }
    
    void testConfigLoading() {
        ConfigSystem config;
        
        // Test loading from file
        std::string config_file = "nlm_config.json";
        // Create a test config file
        std::ofstream outfile(config_file);
        outfile << "{\"brain.neuron.count\": 500, \"brain.synaptic.connectivity\": 0.4}";
        outfile.close();
        
        bool load_success = config.loadFromFile(config_file);
        assert(load_success);
        
        assert(config.getConfigValue("brain.neuron.count", 0) == 500);
        assert(config.getConfigValue("brain.synaptic.connectivity", 0.0) == 0.4);
        
        // Cleanup
        std::remove(config_file.c_str());
    }
    
    void testConfigSerialization() {
        ConfigSystem config;
        
        // Set up config values
        config.setConfigValue("brain.neuron.count", 1000);
        config.setConfigValue("brain.synaptic.connectivity", 0.3);
        config.setConfigValue("brain.learning.rate", 0.01);
        
        // Serialize to string
        std::string serialized = config.serialize();
        assert(!serialized.empty());
        
        // Deserialize
        ConfigSystem new_config;
        bool deserialize_success = new_config.deserialize(serialized);
        assert(deserialize_success);
        
        assert(new_config.getConfigValue("brain.neuron.count", 0) == 1000);
        assert(new_config.getConfigValue("brain.synaptic.connectivity", 0.0) == 0.3);
        assert(new_config.getConfigValue("brain.learning.rate", 0.0) == 0.01);
    }
    
    void testConfigInheritance() {
        ConfigSystem config;
        
        // Test configuration inheritance
        config.setConfigValue("brain.base.neuron.count", 100);
        config.setConfigValue("brain.override.neuron.count", 200);
        
        // Should inherit base when not overridden
        int base_value = config.getConfigValue("brain.base.neuron.count", 0);
        assert(base_value == 100);
        
        // Should use override when specified
        int override_value = config.getConfigValue("brain.override.neuron.count", 0);
        assert(override_value == 200);
        
        // Should inherit from parent in hierarchy
        config.setConfigValue("brain.parent.setting", "value1");
        config.setConfigValue("brain.child.setting", "value2", true); // Inherited
        
        assert(config.getConfigValue("brain.child.setting", "") == "value2");
    }
};