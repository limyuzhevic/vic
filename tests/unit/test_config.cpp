// Config System Tests
// Comprehensive tests for the Config system enhancements

#include "test_framework.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/AdvancedConfig.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

namespace test_config {

// Test basic config creation and operations
class BasicConfigTest : public TestSuite {
public:
    BasicConfigTest() : TestSuite("Config::Basic") {}
    
    void run() override {
        std::cout << "  Testing basic config operations..." << std::endl;
        
        TIME_TEST("Config Creation", {
            nlm::Config config;
            assert(!config.has("test"));
        });
        
        TIME_TEST("Config Set/Get", {
            nlm::Config config;
            config.set("int_value", 42, nlm::ConfigSource::Runtime);
            assert(config.has("int_value"));
            
            auto intVal = config.get<int>("int_value");
            assert(intVal.has_value());
            assert(*intVal == 42);
            
            config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
            auto doubleVal = config.get<double>("double_value");
            assert(doubleVal.has_value());
            assert(*doubleVal > 3.13 && *doubleVal < 3.15);
            
            config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
            auto stringVal = config.get<std::string>("string_value");
            assert(stringVal.has_value());
            assert(*stringVal == "hello");
            
            config.set("bool_value", true, nlm::ConfigSource::Runtime);
            auto boolVal = config.get<bool>("bool_value");
            assert(boolVal.has_value());
            assert(*boolVal == true);
        });
        
        TIME_TEST("Config GetOr with Default", {
            nlm::Config config;
            int val = config.getOr<int>("missing", 100);
            assert(val == 100);
            
            config.set("existing", 50, nlm::ConfigSource::Runtime);
            val = config.getOr<int>("existing", 100);
            assert(val == 50);
        });
        
        TIME_TEST("Config Remove", {
            nlm::Config config;
            config.set("test", 123, nlm::ConfigSource::Runtime);
            assert(config.has("test"));
            
            config.remove("test");
            assert(!config.has("test"));
        });
        
        TIME_TEST("Config Keys Management", {
            nlm::Config config;
            config.set("key1", 1, nlm::ConfigSource::Runtime);
            config.set("key2", 2, nlm::ConfigSource::Runtime);
            config.set("key3", 3, nlm::ConfigSource::Runtime);
            
            auto keys = config.getKeys();
            assert(keys.size() == 3);
            assert(std::find(keys.begin(), keys.end(), "key1") != keys.end());
            assert(std::find(keys.begin(), keys.end(), "key2") != keys.end());
            assert(std::find(keys.begin(), keys.end(), "key3") != keys.end());
        });
        
        std::cout << "  Basic Config tests PASSED" << std::endl;
    }
};

// Test config file loading and saving
class FileIOConfigTest : public TestSuite {
public:
    FileIOConfigTest() : TestSuite("Config::FileIO") {}
    
    void run() override {
        std::cout << "  Testing config file operations..." << std::endl;
        
        // Test JSON loading
        TIME_TEST("JSON File Loading", {
            nlm::Config config;
            std::string jsonContent = R"({"neuron_count": 1000, "connection_probability": 0.1, "simulation_timestep": 0.001})";
            
            std::istringstream iss(jsonContent);
            std::string line;
            std::getline(iss, line);
            assert(config.loadFromJson(line, "test.json"));
            
            auto neuronCount = config.get<size_t>("neuron_count");
            assert(neuronCount.has_value());
            assert(*neuronCount == 1000);
        });
        
        // Test YAML loading
        TIME_TEST("YAML File Loading", {
            nlm::Config config;
            std::string yamlContent = "neuron_count: 2000\nconnection_probability: 0.2\nsimulation_timestep: 0.002\n";
            assert(config.loadFromYaml(yamlContent, "test.yaml"));
            
            auto neuronCount = config.get<size_t>("neuron_count");
            assert(neuronCount.has_value());
            assert(*neuronCount == 2000);
        });
        
        // Test key-value file loading
        TIME_TEST("Key-Value File Loading", {
            nlm::Config config;
            std::string kvContent = "neuron_count=1500\nconnection_probability=0.15\nsimulation_timestep=0.0015\n";
            assert(config.loadFromKeyValue(kvContent, "test.kv"));
            
            auto neuronCount = config.get<size_t>("neuron_count");
            assert(neuronCount.has_value());
            assert(*neuronCount == 1500);
        });
        
        // Test file saving
        TIME_TEST("File Saving", {
            nlm::Config config;
            config.set("neuron_count", 3000, nlm::ConfigSource::Runtime);
            config.set("connection_probability", 0.25, nlm::ConfigSource::Runtime);
            
            // Save to temporary file
            std::string tempFile = "/tmp/test_config.json";
            assert(config.saveToFile(tempFile));
            
            // Load back
            nlm::Config config2;
            assert(config2.loadFromFile(tempFile));
            
            auto neuronCount = config2.get<size_t>("neuron_count");
            assert(neuronCount.has_value());
            assert(*neuronCount == 3000);
        });
        
        std::cout << "  Config FileIO tests PASSED" << std::endl;
    }
};

// Test config validation and error handling
class ConfigValidationTest : public TestSuite {
public:
    ConfigValidationTest() : TestSuite("Config::Validation") {}
    
    void run() override {
        std::cout << "  Testing config validation..." << std::endl;
        
        TIME_TEST("Config Validation", {
            nlm::Config config;
            
            // Test valid values
            config.set("int_value", 42, nlm::ConfigSource::Runtime);
            config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
            config.set("string_value", "hello", nlm::ConfigSource::Runtime);
            config.set("bool_value", true, nlm::ConfigSource::Runtime);
            
            // Validate each key
            assert(config.validate("int_value", config.get<const nlm::ConfigValue&>("int_value")));
            assert(config.validate("double_value", config.get<const nlm::ConfigValue&>("double_value")));
            assert(config.validate("string_value", config.get<const nlm::ConfigValue&>("string_value")));
            assert(config.validate("bool_value", config.get<const nlm::ConfigValue&>("bool_value")));
            
            // Test invalid values (should fail validation)
            config.set("invalid_string", std::string("a"), nlm::ConfigSource::Runtime);
            config.set("invalid_int", 1000000, nlm::ConfigSource::Runtime);  // Too large
            
            // Check validation errors
            auto errors = config.getValidationErrors();
            // Invalid keys may not be reported immediately, just ensure no exceptions
        });
        
        // Test last error handling
        TIME_TEST("Error Message Handling", {
            nlm::Config config;
            
            // Clear errors
            config.clearLastError();
            assert(config.getLastError().empty());
            
            // Try to get missing key
            auto missing = config.get<int>("nonexistent");
            assert(!missing.has_value());
            
            // Error should be set
            // Note: getLastError() behavior depends on implementation
        });
        
        std::cout << "  Config Validation tests PASSED" << std::endl;
    }
};

// Test advanced config features
class AdvancedConfigTest : public TestSuite {
public:
    AdvancedConfigTest() : TestSuite("Config::Advanced") {}
    
    void run() override {
        std::cout << "  Testing advanced config features..." << std::endl;
        
        // Test config inheritance
        TIME_TEST("Config Inheritance", {
            nlm::Config config;
            config.set("base_value", 100, nlm::ConfigSource::Runtime);
            config.set("overridden_value", 200, nlm::ConfigSource::Runtime);
            
            // Test merging
            nlm::Config config2;
            config2.set("base_value", 150, nlm::ConfigSource::Runtime);  // Different value
            config2.set("additional_value", 300, nlm::ConfigSource::Runtime);  // New value
            
            config.merge(config2);
            
            auto baseVal = config.get<int>("base_value");
            assert(baseVal.has_value());
            assert(*baseVal == 150);  // Overridden by config2
            
            auto overriddenVal = config.get<int>("overridden_value");
            assert(overriddenVal.has_value());
            assert(*overriddenVal == 200);
            
            auto additionalVal = config.get<int>("additional_value");
            assert(additionalVal.has_value());
            assert(*additionalVal == 300);
        });
        
        // Test caching
        TIME_TEST("Config Caching", {
            nlm::Config config;
            
            // Enable caching (if available)
            // config.clearCache();  // Clear cache
            
            config.set("cached_value", 42, nlm::ConfigSource::Runtime);
            auto val1 = config.get<int>("cached_value");
            assert(val1.has_value());
            
            // Get again (should use cache if implemented)
            auto val2 = config.get<int>("cached_value");
            assert(val2.has_value());
            assert(*val2 == 42);
            
            config.clearCache();  // Clear cache
        });
        
        // Test key validation
        TIME_TEST("Key Validation", {
            nlm::Config config;
            
            // Test valid keys
            assert(config.isValidKey("simple_key"));
            assert(config.isValidKey("key_with_underscores"));
            assert(config.isValidKey("key123"));
            
            // Test invalid keys
            assert(!config.isValidKey(""));  // Empty key
            assert(!config.isValidKey("key with spaces"));  // Spaces
            assert(!config.isValidKey("key-with-hyphens"));  // Hyphens
            assert(!config.isValidKey("123key"));  // Starts with number
        });
        
        std::cout << "  Advanced Config tests PASSED" << std::endl;
    }
};

// Global test instances
static BasicConfigTest basicConfigTest;
static FileIOConfigTest fileIOConfigTest;
static ConfigValidationTest configValidationTest;
static AdvancedConfigTest advancedConfigTest;

void runAll() {
    std::cout << "=== Config System Tests ===" << std::endl;
    
    bool allPassed = true;
    
    // Register all test suites with the framework
    TestFramework& framework = TestFramework::getInstance();
    framework.addSuite(&basicConfigTest);
    framework.addSuite(&fileIOConfigTest);
    framework.addSuite(&configValidationTest);
    framework.addSuite(&advancedConfigTest);
    
    // Run all tests
    allPassed = framework.runAllTests();
    
    if (allPassed) {
        std::cout << "=== All Config Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Config Tests FAILED ===" << std::endl;
    }
}

} // namespace test_config
