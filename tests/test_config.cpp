// Comprehensive Configuration Testing
// Enhanced with JSON/YAML and advanced features testing

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

void testConfigFileOperations() {
    nlm::Config config;
    
    // Set some test values
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello world", nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    config.set("test_vector", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    
    // Save to file
    const std::string testFile = "test_config.json";
    assert(config.saveToFile(testFile));
    
    // Clear and reload
    config.clear();
    assert(config.loadFromFile(testFile));
    
    // Verify values were loaded correctly
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("test_double");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14) < 0.01);
    
    auto stringVal = config.get<std::string>("test_string");
    assert(stringVal.has_value() && *stringVal == "hello world");
    
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value() && *boolVal == true);
    
    auto vectorVal = config.get<std::vector<int>>("test_vector");
    assert(vectorVal.has_value());
    assert(vectorVal->size() == 3);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigFileOperations passed" << std::endl;
}

void testConfigJSONFormat() {
    nlm::Config config;
    
    // Set test values
    config.set("name", "test_brain", nlm::ConfigSource::Runtime);
    config.set("neurons", 1000, nlm::ConfigSource::Runtime);
    config.set("synapses", 5000, nlm::ConfigSource::Runtime);
    config.set("learning_rate", 0.01, nlm::ConfigSource::Runtime);
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    
    // Export to JSON
    std::string jsonStr = config.exportToJSON();
    
    // Parse JSON (simplified - would use nlohmann/json in production)
    // For this test, we just verify it contains expected content
    assert(jsonStr.find("name") != std::string::npos);
    assert(jsonStr.find("neurons") != std::string::npos);
    assert(jsonStr.find("1000") != std::string::npos);
    assert(jsonStr.find("enabled") != std::string::npos);
    assert(jsonStr.find("true") != std::string::npos);
    
    std::cout << "    testConfigJSONFormat passed" << std::endl;
}

void testConfigYAMLFormat() {
    nlm::Config config;
    
    // Set test values
    config.set("environment", "grid_world", nlm::ConfigSource::Runtime);
    config.set("width", 10, nlm::ConfigSource::Runtime);
    config.set("height", 10, nlm::ConfigSource::Runtime);
    config.set("obstacles", 5, nlm::ConfigSource::Runtime);
    
    // Export to YAML
    std::string yamlStr = config.exportToYAML();
    
    // Verify YAML content
    assert(yamlStr.find("environment") != std::string::npos);
    assert(yamlStr.find("grid_world") != std::string::npos);
    assert(yamlStr.find("width") != std::string::npos);
    assert(yamlStr.find("10") != std::string::npos);
    
    std::cout << "    testConfigYAMLFormat passed" << std::endl;
}

void testConfigMerge() {
    nlm::Config config1;
    config1.set("key1", 1, nlm::ConfigSource::Runtime);
    config1.set("key2", "value1", nlm::ConfigSource::Runtime);
    
    nlm::Config config2;
    config2.set("key2", "value2", nlm::ConfigSource::Runtime); // Should override
    config2.set("key3", 3.14, nlm::ConfigSource::Runtime);
    
    config1.merge(config2);
    
    assert(config1.get<int>("key1").has_value() && *config1.get<int>("key1") == 1);
    assert(config1.get<std::string>("key2").has_value() && *config1.get<std::string>("key2") == "value2");
    assert(config1.get<double>("key3").has_value() && std::abs(*config1.get<double>("key3") - 3.14) < 0.01);
    
    std::cout << "    testConfigMerge passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Valid configuration
    config.set("random_seed", 42, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    
    assert(config.validate()); // Should pass validation
    
    // Invalid configuration - negative neuron count
    nlm::Config invalidConfig;
    invalidConfig.set("random_seed", 42, nlm::ConfigSource::Runtime);
    invalidConfig.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    invalidConfig.set("neuron_count", -100, nlm::ConfigSource::Runtime);
    
    assert(!invalidConfig.validate()); // Should fail validation
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigStatistics() {
    nlm::Config config;
    
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::File);
    config.set("string_val", "test", nlm::ConfigSource::CommandLine);
    config.set("bool_val", true, nlm::ConfigSource::Default);
    config.set("vector_val", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    
    std::string stats = config.statistics();
    assert(stats.find("Total entries: 5") != std::string::npos);
    assert(stats.find("Integers: 1") != std::string::npos);
    assert(stats.find("Floats: 1") != std::string::npos);
    assert(stats.find("Strings: 1") != std::string::npos);
    assert(stats.find("Booleans: 1") != std::string::npos);
    assert(stats.find("Arrays/Vectors: 1") != std::string::npos);
    assert(stats.find("File: 1") != std::string::npos);
    assert(stats.find("Command Line: 1") != std::string::npos);
    assert(stats.find("Default: 1") != std::string::npos);
    assert(stats.find("Runtime: 2") != std::string::npos);
    
    std::cout << "    testConfigStatistics passed" << std::endl;
}

void testConfigKeyValueFormat() {
    nlm::Config config;
    
    // Create a test key=value file
    const std::string testFile = "test_config.keyvalue";
    std::ofstream file(testFile);
    file << "# Test configuration\n";
    file << "test_key1 = value1\n";
    file << "test_key2 = 42\n";
    file << "test_key3 = 3.14\n";
    file << "test_key4 = true\n";
    file << "test_key5 = \"quoted value\"\n";
    file.close();
    
    // Load the key=value file
    assert(config.loadFromFile(testFile));
    
    // Verify values
    assert(config.get<std::string>("test_key1").has_value() && 
           *config.get<std::string>("test_key1") == "value1");
    
    assert(config.get<int>("test_key2").has_value() && 
           *config.get<int>("test_key2") == 42);
    
    assert(config.get<double>("test_key3").has_value() && 
           std::abs(*config.get<double>("test_key3") - 3.14) < 0.01);
    
    assert(config.get<bool>("test_key4").has_value() && 
           *config.get<bool>("test_key4") == true);
    
    assert(config.get<std::string>("test_key5").has_value() && 
           *config.get<std::string>("test_key5") == "quoted value");
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigKeyValueFormat passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigFileOperations();
    testConfigJSONFormat();
    testConfigYAMLFormat();
    testConfigMerge();
    testConfigValidation();
    testConfigStatistics();
    testConfigKeyValueFormat();
    
    std::cout << "\n=== All Config Tests Passed ===" << std::endl;
    std::cout << "Configuration parser enhancements working correctly!" << std::endl;
}

} // namespace test_config
