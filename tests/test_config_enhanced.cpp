// Enhanced Config Tests with JSON support and error handling
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
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
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigJSONParsing() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::string jsonContent = R"({"neuron_count": 1000, "connection_probability": 0.05, "stdp_enabled": true})";
    std::string tempFile = "/tmp/test_config.json";
    
    std::ofstream file(tempFile);
    if (!file.is_open()) {
        std::cout << "    testConfigJSONParsing skipped (cannot create temp file)" << std::endl;
        return;
    }
    
    file << jsonContent;
    file.close();
    
    // Load JSON file
    bool loaded = config.loadFromFile(tempFile);
    assert(loaded);
    
    // Check that values were loaded
    assert(config.has("neuron_count"));
    assert(config.has("connection_probability"));
    assert(config.has("stdp_enabled"));
    
    auto neuronCount = config.get<int64_t>("neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 1000);
    
    auto connectionProb = config.get<double>("connection_probability");
    assert(connectionProb.has_value() && *connectionProb == 0.05);
    
    auto stdpEnabled = config.get<bool>("stdp_enabled");
    assert(stdpEnabled.has_value() && *stdpEnabled == true);
    
    std::cout << "    testConfigJSONParsing passed" << std::endl;
    
    // Clean up
    std::remove(tempFile.c_str());
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test empty key
    try {
        config.set("", 123, nlm::ConfigSource::Runtime);
        assert(false); // Should have thrown
    } catch (const std::exception& e) {
        // Expected
    }
    
    // Test invalid file path (should not throw when loading)
    bool loaded = config.loadFromFile("/nonexistent/path/file.json");
    assert(!loaded);
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigSaveAndLoad() {
    nlm::Config config;
    
    // Set some test values
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello", nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    // Save to file
    std::string saveFile = "/tmp/test_save.cfg";
    bool saved = config.saveToFile(saveFile);
    assert(saved);
    
    // Clear config
    config.clear();
    assert(config.getKeys().size() == 0);
    
    // Load from file
    bool loaded = config.loadFromFile(saveFile);
    assert(loaded);
    
    // Check that values were loaded
    assert(config.has("test_int"));
    assert(config.has("test_string"));
    assert(config.has("test_double"));
    assert(config.has("test_bool"));
    
    std::cout << "    testConfigSaveAndLoad passed" << std::endl;
    
    // Clean up
    std::remove(saveFile.c_str());
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test invalid types in JSON
    std::string jsonContent = R"({"invalid": "not_a_number"})";
    std::string tempFile = "/tmp/test_validation.json";
    
    std::ofstream file(tempFile);
    if (file.is_open()) {
        file << jsonContent;
        file.close();
        
        // Load JSON file
        config.loadFromFile(tempFile);
        
        // Check that parsing didn't crash
        assert(config.has("invalid"));
        
        std::cout << "    testConfigValidation passed" << std::endl;
        
        // Clean up
        std::remove(tempFile.c_str());
    } else {
        std::cout << "    testConfigValidation skipped (cannot create temp file)" << std::endl;
    }
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJSONParsing();
    testConfigErrorHandling();
    testConfigSaveAndLoad();
    testConfigValidation();
    
    std::cout << "All config tests passed!" << std::endl;
}

} // namespace test_config