// NLM Configuration Tests
// Phase 6: FINAL INTEGRATION

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <algorithm>

namespace test_config {
    
    void testBasicConfig() {
        // Test 1: Create basic configuration
        nlm::Config config;
        
        // Set various types of values
        config.set("string_value", std::string("test"));
        config.set("int_value", 42);
        config.set("double_value", 3.14);
        config.set("bool_value", true);
        
        // Get values
        auto strVal = config.get<std::string>("string_value");
        auto intVal = config.get<int>("int_value");
        auto doubleVal = config.get<double>("double_value");
        auto boolVal = config.get<bool>("bool_value");
        
        assert(strVal.has_value() && strVal.value() == "test");
        assert(intVal.has_value() && intVal.value() == 42);
        assert(doubleVal.has_value() && std::abs(doubleVal.value() - 3.14) < 0.001);
        assert(boolVal.has_value() && boolVal.value() == true);
        
        std::cout << "✓ Basic configuration test passed" << std::endl;
    }
    
    void testSaveLoadConfig() {
        // Test 2: Save and load configuration
        nlm::Config config;
        
        config.set("test_string", std::string("Hello World"), nlm::ConfigSource::File);
        config.set("test_int", 123, nlm::ConfigSource::File);
        config.set("test_bool", false, nlm::ConfigSource::File);
        
        // Save to file
        std::string testFile = "test_config.tmp";
        bool saveSuccess = config.saveToFile(testFile);
        assert(saveSuccess);
        
        // Clear and reload
        nlm::Config config2;
        bool loadSuccess = config2.loadFromFile(testFile);
        assert(loadSuccess);
        
        // Verify loaded values
        auto strVal = config2.get<std::string>("test_string");
        auto intVal = config2.get<int>("test_int");
        auto boolVal = config2.get<bool>("test_bool");
        
        assert(strVal.has_value() && strVal.value() == "Hello World");
        assert(intVal.has_value() && intVal.value() == 123);
        assert(boolVal.has_value() && boolVal.value() == false);
        
        // Clean up
        std::remove(testFile.c_str());
        
        std::cout << "✓ Configuration save/load test passed" << std::endl;
    }
    
    void testConfigKeys() {
        // Test 3: Configuration key management
        nlm::Config config;
        
        // Add some keys
        config.set("key1", std::string("value1"));
        config.set("key2", 42);
        config.set("key3", true);
        
        // Check keys exist
        assert(config.has("key1"));
        assert(config.has("key2"));
        assert(config.has("key3"));
        assert(!config.has("nonexistent_key"));
        
        // Get all keys
        auto keys = config.getKeys();
        assert(keys.size() == 3);
        assert(std::find(keys.begin(), keys.end(), "key1") != keys.end());
        assert(std::find(keys.begin(), keys.end(), "key2") != keys.end());
        assert(std::find(keys.begin(), keys.end(), "key3") != keys.end());
        
        // Remove a key
        config.remove("key2");
        assert(!config.has("key2"));
        
        keys = config.getKeys();
        assert(keys.size() == 2);
        
        std::cout << "✓ Configuration keys management test passed" << std::endl;
    }
    
    void testGetOrDefault() {
        // Test 4: getOr with default values
        nlm::Config config;
        
        // Set some values
        config.set("existing_string", std::string("actual_value"));
        config.set("existing_int", 100);
        
        // Test getOr with existing values
        auto val1 = config.getOr<std::string>("existing_string", "default_value");
        auto val2 = config.getOr<int>("existing_int", 999);
        
        assert(val1 == "actual_value");
        assert(val2 == 100);
        
        // Test getOr with non-existing values (should return defaults)
        auto val3 = config.getOr<std::string>("nonexistent", "default_value");
        auto val4 = config.getOr<int>("nonexistent", 999);
        
        assert(val3 == "default_value");
        assert(val4 == 999);
        
        std::cout << "✓ getOr default value test passed" << std::endl;
    }
    
    void testINIFormat() {
        // Test 6: INI format file loading
        nlm::Config config;
        
        // Create a test INI file
        std::string testFile = "test_ini.tmp";
        std::ofstream outFile(testFile);
        outFile << "# Test INI file\n";
        outFile << "string_value = \"hello\"\n";
        outFile << "int_value = 42\n";
        outFile << "double_value = 3.14159\n";
        outFile << "bool_value = true\n";
        outFile.close();
        
        // Load the file
        bool loadSuccess = config.loadFromFile(testFile);
        assert(loadSuccess);
        
        // Verify values
        auto strVal = config.get<std::string>("string_value");
        auto intVal = config.get<int>("int_value");
        auto doubleVal = config.get<double>("double_value");
        auto boolVal = config.get<bool>("bool_value");
        
        assert(strVal.has_value() && strVal.value() == "hello");
        assert(intVal.has_value() && intVal.value() == 42);
        assert(doubleVal.has_value() && std::abs(doubleVal.value() - 3.14159) < 0.0001);
        assert(boolVal.has_value() && boolVal.value() == true);
        
        // Clean up
        std::remove(testFile.c_str());
        
        std::cout << "✓ INI format test passed" << std::endl;
    }
    
    void runAll() {
        std::cout << "=== Configuration Tests ===" << std::endl;
        
        testBasicConfig();
        testSaveLoadConfig();
        testConfigKeys();
        testGetOrDefault();
        testINIFormat();
        
        std::cout << "=== All Configuration Tests Passed ===" << std::endl;
    }
} // namespace test_config
