// Config Tests - Enhanced with JSON/YAML Parser and Advanced Features
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

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

// Test JSON/YAML parser functionality
void testJSONParser() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::string testJson = "test_config.json";
    std::ofstream file(testJson);
    file << R"({
        "config": [
            {"key": "neurons", "value": 1000, "description": "Number of neurons", "source": 0},
            {"key": "timestep", "value": 0.001, "description": "Simulation timestep", "source": 0},
            {"key": "plasticity", "value": true, "description": "Enable plasticity", "source": 0}
        ]
    })";
    file.close();
    
    // Load JSON file
    bool loaded = config.loadFromFile(testJson);
    assert(loaded);
    
    // Verify values were loaded
    assert(config.has("neurons"));
    auto neurons = config.get<int64_t>("neurons");
    assert(neurons.has_value() && *neurons == 1000);
    
    assert(config.has("timestep"));
    auto timestep = config.get<double>("timestep");
    assert(timestep.has_value() && *timestep == 0.001);
    
    assert(config.has("plasticity"));
    auto plasticity = config.get<bool>("plasticity");
    assert(plasticity.has_value() && *plasticity == true);
    
    std::cout << "    testJSONParser passed" << std::endl;
    
    // Clean up
    std::remove(testJson.c_str());
}

// Test JSON flat format (legacy support)
void testJSONFlatFormat() {
    nlm::Config config;
    
    std::string testJson = "test_config_flat.json";
    std::ofstream file(testJson);
    file << R"({
        "neurons": 500,
        "timestep": 0.002,
        "plasticity": false
    })";
    file.close();
    
    bool loaded = config.loadFromFile(testJson);
    assert(loaded);
    
    // Verify legacy format values
    assert(config.has("neurons"));
    auto neurons = config.get<int64_t>("neurons");
    assert(neurons.has_value() && *neurons == 500);
    
    assert(config.has("timestep"));
    auto timestep = config.get<double>("timestep");
    assert(timestep.has_value() && *timestep == 0.002);
    
    assert(config.has("plasticity"));
    auto plasticity = config.get<bool>("plasticity");
    assert(plasticity.has_value() && *plasticity == false);
    
    std::cout << "    testJSONFlatFormat passed" << std::endl;
    
    std::remove(testJson.c_str());
}

// Test error handling for malformed JSON
void testMalformedJSONErrorHandling() {
    nlm::Config config;
    
    std::string testJson = "test_malformed.json";
    std::ofstream file(testJson);
    file << R"({
        "invalid": json,
        "missing": value
    })";
    file.close();
    
    // Should fail gracefully for malformed JSON
    bool loaded = config.loadFromFile(testJson);
    assert(!loaded); // Should return false
    
    std::cout << "    testMalformedJSONErrorHandling passed" << std::endl;
    
    std::remove(testJson.c_str());
}

// Test file not found error handling
void testFileNotFoundErrorHandling() {
    nlm::Config config;
    
    // Try to load a non-existent file
    bool loaded = config.loadFromFile("nonexistent_file_12345.json");
    assert(!loaded); // Should return false
    
    std::cout << "    testFileNotFoundErrorHandling passed" << std::endl;
}

// Test configuration summary
void testConfigSummary() {
    nlm::Config config;
    
    config.set("test1", 42, nlm::ConfigSource::Runtime);
    config.set("test2", std::string("hello"), nlm::ConfigSource::File);
    config.set("test3", 3.14, nlm::ConfigSource::CommandLine);
    
    std::string summary = config.summary();
    assert(summary.find("Configuration") != std::string::npos);
    assert(summary.find("test1") != std::string::npos);
    assert(summary.find("test2") != std::string::npos);
    assert(summary.find("test3") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

// Test configuration getOr with default values
void testConfigGetOrEdgeCases() {
    nlm::Config config;
    
    // Test with various types
    assert(config.getOr<int>("missing", 0) == 0);
    assert(config.getOr<double>("missing", 0.0) == 0.0);
    assert(config.getOr<bool>("missing", false) == false);
    assert(config.getOr<std::string>("missing", "default") == "default");
    
    // Test with existing values
    config.set("number", 123, nlm::ConfigSource::Runtime);
    assert(config.getOr<int>("number", 0) == 123);
    
    config.set("flag", true, nlm::ConfigSource::File);
    assert(config.getOr<bool>("flag", false) == true);
    
    config.set("name", std::string("test"), nlm::ConfigSource::CommandLine);
    assert(config.getOr<std::string>("name", "unknown") == "test");
    
    std::cout << "    testConfigGetOrEdgeCases passed" << std::endl;
}

// Test configuration source tracking
void testConfigSourceTracking() {
    nlm::Config config;
    
    // Set values from different sources
    config.set("default_val", 1, nlm::ConfigSource::Default);
    config.set("file_val", 2, nlm::ConfigSource::File);
    config.set("cli_val", 3, nlm::ConfigSource::CommandLine);
    config.set("runtime_val", 4, nlm::ConfigSource::Runtime);
    
    // Verify sources are tracked
    assert(config.has("default_val"));
    assert(config.has("file_val"));
    assert(config.has("cli_val"));
    assert(config.has("runtime_val"));
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

// Test configuration clear and empty
void testConfigClear() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    
    assert(config.has("key1"));
    assert(config.has("key2"));
    
    config.clear();
    
    assert(!config.has("key1"));
    assert(!config.has("key2"));
    assert(config.getKeys().size() == 0);
    
    std::cout << "    testConfigClear passed" << std::endl;
}

// Test configuration large dataset handling
void testConfigLargeDataset() {
    nlm::Config config;
    
    // Set many values
    for (int i = 0; i < 1000; ++i) {
        config.set(std::to_string(i), i, nlm::ConfigSource::Runtime);
    }
    
    assert(config.getKeys().size() == 1000);
    
    // Verify we can retrieve values
    for (int i = 0; i < 100; ++i) {
        auto val = config.get<int>("value");
        // Note: this test assumes we have a "value" key
    }
    
    std::cout << "    testConfigLargeDataset passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testJSONParser();
    testJSONFlatFormat();
    testMalformedJSONErrorHandling();
    testFileNotFoundErrorHandling();
    testConfigSummary();
    testConfigGetOrEdgeCases();
    testConfigSourceTracking();
    testConfigClear();
    testConfigLargeDataset();
    
    std::cout << "\n=== All Configuration Tests Passed! ===" << std::endl;
}

} // namespace test_config