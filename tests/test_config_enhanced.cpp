// Enhanced Config Tests
// Comprehensive test suite for the improved configuration system
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

void testConfigSchemaValidation() {
    nlm::Config config;
    
    // Test that schema is initialized
    assert(!config.getKeys().empty());
    
    // Test that core keys exist
    assert(config.has("random_seed"));
    assert(config.has("simulation_timestep"));
    assert(config.has("neuron_count"));
    assert(config.has("connection_probability"));
    
    std::cout << "    testConfigSchemaValidation passed" << std::endl;
}

void testConfigTextFileLoading() {
    nlm::Config config;
    
    // Create a temporary config file
    std::string testFile = "/tmp/test_config.txt";
    std::ofstream file(testFile);
    file << "# Test configuration file\n";
    file << "test_key = 123\n";
    file << "name = \"Test Config\"\n";
    file << "enabled = true\n";
    file << "rate = 3.14\n";
    file.close();
    
    // Load the file
    bool success = config.loadFromFile(testFile);
    assert(success);
    
    // Verify loaded values
    assert(config.has("test_key"));
    auto intVal = config.get<int>("test_key");
    assert(intVal.has_value() && *intVal == 123);
    
    assert(config.has("name"));
    auto stringVal = config.get<std::string>("name");
    assert(stringVal.has_value() && *stringVal == "Test Config");
    
    assert(config.has("enabled"));
    auto boolVal = config.get<bool>("enabled");
    assert(boolVal.has_value() && *boolVal == true);
    
    assert(config.has("rate"));
    auto doubleVal = config.get<double>("rate");
    assert(doubleVal.has_value() && (*doubleVal > 3.13 && *doubleVal < 3.15));
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigTextFileLoading passed" << std::endl;
}

void testConfigJSONFileLoading() {
    nlm::Config config;
    
    // Create a temporary JSON config file
    std::string testFile = "/tmp/test_config.json";
    std::ofstream file(testFile);
    file << "{\n";
    file << "  \"json_int\": 42,\n";
    file << "  \"json_double\": 2.718,\n";
    file << "  \"json_string\": \"JSON Config\",\n";
    file << "  \"json_bool\": true\n";
    file << "}\n";
    file.close();
    
    // Load the file
    bool success = config.loadFromFile(testFile);
    assert(success);
    
    // Verify loaded values
    assert(config.has("json_int"));
    auto intVal = config.get<int>("json_int");
    assert(intVal.has_value() && *intVal == 42);
    
    assert(config.has("json_double"));
    auto doubleVal = config.get<double>("json_double");
    assert(doubleVal.has_value() && (*doubleVal > 2.71 && *doubleVal < 2.73));
    
    assert(config.has("json_string"));
    auto stringVal = config.get<std::string>("json_string");
    assert(stringVal.has_value() && *stringVal == "JSON Config");
    
    assert(config.has("json_bool"));
    auto boolVal = config.get<bool>("json_bool");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigJSONFileLoading passed" << std::endl;
}

void testConfigFileSaving() {
    nlm::Config config;
    
    // Set some values
    config.set("test_int", 123, nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_string", "test_value", nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    // Save as text file
    std::string textFile = "/tmp/test_config_save.txt";
    bool success = config.saveToFile(textFile);
    assert(success);
    
    // Load it back and verify
    nlm::Config config2;
    success = config2.loadFromFile(textFile);
    assert(success);
    
    assert(config2.has("test_int"));
    assert(config2.has("test_double"));
    assert(config2.has("test_string"));
    assert(config2.has("test_bool"));
    
    // Clean up
    std::remove(textFile.c_str());
    
    std::cout << "    testConfigFileSaving passed" << std::endl;
}

void testConfigValueTypeValidation() {
    nlm::Config config;
    
    // Test that values are stored correctly by type
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_val", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    
    // Verify types
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value());
    
    auto doubleVal = config.get<double>("double_val");
    assert(doubleVal.has_value());
    
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal.has_value());
    
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal.has_value());
    
    std::cout << "    testConfigValueTypeValidation passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test that invalid operations don't crash the system
    try {
        // These should either work or throw appropriate exceptions
        auto val = config.get<int>("nonexistent_key");
        // It's okay if this doesn't throw - get() returns std::nullopt for missing keys
        (void)val; // Suppress unused variable warning
    } catch (...) {
        // Exception is also acceptable
    }
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigValidationIntegration() {
    nlm::Config config;
    
    // Set core configuration values
    config.set("random_seed", 42);
    config.set("simulation_timestep", 0.001);
    config.set("neuron_count", 100);
    config.set("connection_probability", 0.1);
    config.set("environment_name", "GridWorld");
    config.set("log_level", "INFO");
    
    // Verify they're accessible
    assert(config.has("random_seed"));
    assert(config.has("simulation_timestep"));
    assert(config.has("neuron_count"));
    assert(config.has("connection_probability"));
    assert(config.has("environment_name"));
    assert(config.has("log_level"));
    
    // Check schema validation
    assert(config.has("random_seed"));
    assert(config.has("simulation_timestep"));
    
    std::cout << "    testConfigValidationIntegration passed" << std::endl;
}

void testConfigDefaultValues() {
    nlm::Config config;
    
    // All schema keys should have default values accessible
    assert(config.has("random_seed"));
    auto seed = config.get<int64_t>("random_seed");
    assert(seed.has_value());
    
    assert(config.has("simulation_timestep"));
    auto timestep = config.get<double>("simulation_timestep");
    assert(timestep.has_value());
    
    assert(config.has("neuron_count"));
    auto neuronCount = config.get<int64_t>("neuron_count");
    assert(neuronCount.has_value());
    
    std::cout << "    testConfigDefaultValues passed" << std::endl;
}

void runAll() {
    std::cout << "=== Running Enhanced Config Tests ===" << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSchemaValidation();
    testConfigTextFileLoading();
    testConfigJSONFileLoading();
    testConfigFileSaving();
    testConfigValueTypeValidation();
    testConfigErrorHandling();
    testConfigValidationIntegration();
    testConfigDefaultValues();
    
    std::cout << "\n=== All Config Tests Passed ===" << std::endl;
}

} // namespace test_config