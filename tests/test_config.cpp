// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>

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

void testConfigValidation() {
    std::cout << "    testConfigValidation starting..." << std::endl;
    
    // Clear any existing configuration
    nlm::ConfigManager::getInstance().clear();
    
    // Initialize default config
    nlm::ConfigManager::getInstance().initializeDefaultConfig();
    
    nlm::Config config;
    
    // Test validation with missing required fields
    std::vector<std::string> missing = config.validateAgainstSchema();
    assert(!missing.empty()); // Should have missing required keys
    
    // Test with valid configuration
    config.set("random_seed", 42, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("region_count", 2, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.15, nlm::ConfigSource::Runtime);
    config.set("environment_name", "GridWorld", nlm::ConfigSource::Runtime);
    config.set("log_level", "INFO", nlm::ConfigSource::Runtime);
    config.set("log_to_file", false, nlm::ConfigSource::Runtime);
    
    assert(config.isValid());
    
    // Test with invalid values
    config.set("random_seed", -1, nlm::ConfigSource::Runtime); // Should be positive
    assert(!config.isValid());
    
    // Test getValidationErrors
    std::string errors = config.getValidationErrors();
    assert(errors.find("validation errors") != std::string::npos);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigLoadFromFile() {
    std::cout << "    testConfigLoadFromFile starting..." << std::endl;
    
    std::string testFile = "/tmp/test_config_validation.cfg";
    std::ofstream file(testFile);
    file << "# Test configuration" << std::endl;
    file << "random_seed = 123" << std::endl;
    file << "simulation_timestep = 0.002" << std::endl;
    file << "neuron_count = 2000" << std::endl;
    file << "connection_probability = 0.2" << std::endl;
    file << "environment_name = TestWorld" << std::endl;
    file << "log_level = DEBUG" << std::endl;
    file.close();
    
    nlm::Config config;
    assert(config.loadFromFile(testFile));
    
    assert(config.getOr<int64_t>("random_seed", 0) == 123);
    assert(config.getOr<double>("simulation_timestep", 0.0) == 0.002);
    assert(config.getOr<int64_t>("neuron_count", 0) == 2000);
    assert(config.getOr<double>("connection_probability", 0.0) == 0.2);
    assert(config.getOr<std::string>("environment_name", "") == "TestWorld");
    assert(config.getOr<std::string>("log_level", "") == "DEBUG");
    
    // Test with non-existent file
    assert(!config.loadFromFile("/tmp/nonexistent.cfg"));
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigLoadFromFile passed" << std::endl;
}

void testConfigErrorHandling() {
    std::cout << "    testConfigErrorHandling starting..." << std::endl;
    
    nlm::Config config;
    
    // Test exception types
    try {
        throw nlm::ConfigValidationException("Test validation error");
    } catch (const nlm::ConfigValidationException& e) {
        assert(std::string(e.what()) == "Test validation error");
    }
    
    try {
        throw nlm::ConfigParseException("Test parse error");
    } catch (const nlm::ConfigParseException& e) {
        assert(std::string(e.what()) == "Test parse error");
    }
    
    try {
        throw nlm::ConfigException("Test base error");
    } catch (const nlm::ConfigException& e) {
        assert(std::string(e.what()) == "Test base error");
    }
    
    // Test invalid type access
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.get<std::string>("int_value").has_value() == false); // Wrong type
    assert(config.getOr<std::string>("int_value", "default") == "default"); // Should use default
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigLoadFromFile();
    testConfigErrorHandling();
}

} // namespace test_config
