// Enhanced Config Tests
// Tests for the improved NLM configuration system with JSON/YAML support,
// validation, and error handling.

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

namespace test_config_enhanced {

/**
 * @brief Test basic configuration creation and initialization.
 * 
 * Verifies that a Config object starts in a clean state with no entries.
 */
void testConfigCreation() {
    std::cout << "Running testConfigCreation..." << std::endl;
    nlm::Config config;
    assert(!config.has("test"));
    assert(config.getEntryCount() == 0);
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

/**
 * @brief Test setting and getting configuration values.
 * 
 * Tests the full range of configuration value types (int, double, string, bool)
 * and verifies that values can be correctly retrieved with type conversion.
 */
void testConfigSetAndGet() {
    std::cout << "Running testConfigSetAndGet..." << std::endl;
    nlm::Config config;
    
    // Test integer values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    // Test floating-point values
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    // Test string values
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    // Test boolean values
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Test vector values
    config.set("int_vector", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    auto vectorVal = config.get<std::vector<int>>("int_vector");
    assert(vectorVal.has_value());
    assert(vectorVal->size() == 3);
    assert((*vectorVal)[0] == 1);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

/**
 * @brief Test getOr functionality for default values.
 * 
 * Verifies that getOr() returns default values for missing keys and actual values
 * when keys exist, providing a convenient way to handle optional configuration.
 */
void testConfigGetOr() {
    std::cout << "Running testConfigGetOr..." << std::endl;
    nlm::Config config;
    
    // Test with missing key
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    // Test with existing key
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    // Test with different types
    double doubleVal = config.getOr<double>("missing", 3.14);
    assert(doubleVal == 3.14);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

/**
 * @brief Test configuration key removal functionality.
 * 
 * Verifies that configuration keys can be properly removed and that the has()
 * method correctly reflects the removal.
 */
void testConfigRemove() {
    std::cout << "Running testConfigRemove..." << std::endl;
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    assert(config.getEntryCount() == 1);
    
    config.remove("test");
    assert(!config.has("test"));
    assert(config.getEntryCount() == 0);
    
    // Test removing non-existent key (should not crash)
    config.remove("non_existent");
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

/**
 * @brief Test configuration key enumeration.
 * 
 * Verifies that getKeys() correctly returns all configuration keys and that
 * the order and content are maintained.
 */
void testConfigKeys() {
    std::cout << "Running testConfigKeys..." << std::endl;
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    // Check that all expected keys are present
    assert(std::find(keys.begin(), keys.end(), "key1") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key2") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key3") != keys.end());
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

/**
 * @brief Test configuration validation system.
 * 
 * Tests the validation framework including setting validation rules and
 * checking that invalid configurations are properly detected.
 */
void testConfigValidation() {
    std::cout << "Running testConfigValidation..." << std::endl;
    nlm::Config config;
    
    // Test valid configuration
    config.set("int_value", 10, nlm::ConfigSource::Runtime, "Integer between 1 and 100", "int(1,100)");
    assert(config.validate());
    
    // Test validation error with invalid value
    config.set("invalid_int", -5, nlm::ConfigSource::Runtime, "Integer between 1 and 100", "int(1,100)");
    assert(!config.validate());
    
    auto errors = config.getValidationErrors();
    assert(!errors.empty());
    assert(errors.size() == 1); // Only the invalid_int should cause an error
    
    // Test with valid string
    config.set("name", "test_user", nlm::ConfigSource::Runtime, "Username (3-20 chars)", "string(3,20)");
    assert(config.validate());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

/**
 * @brief Test configuration source tracking and precedence.
 * 
 * Verifies that configuration source information is correctly tracked for each
 * configuration entry, which is important for precedence resolution.
 */
void testConfigSourceTracking() {
    std::cout << "Running testConfigSourceTracking..." << std::endl;
    nlm::Config config;
    
    // Test all source types
    config.set("default_value", 1, nlm::ConfigSource::Default);
    assert(config.getSource("default_value") == nlm::ConfigSource::Default);
    
    config.set("file_value", 2, nlm::ConfigSource::File);
    assert(config.getSource("file_value") == nlm::ConfigSource::File);
    
    config.set("command_line_value", 3, nlm::ConfigSource::CommandLine);
    assert(config.getSource("command_line_value") == nlm::ConfigSource::CommandLine);
    
    config.set("runtime_value", 4, nlm::ConfigSource::Runtime);
    assert(config.getSource("runtime_value") == nlm::ConfigSource::Runtime);
    
    // Verify that runtime values can override file values
    config.set("command_line_value", 99, nlm::ConfigSource::CommandLine);
    assert(config.get<int>("command_line_value").value() == 99);
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

/**
 * @brief Test file format detection.
 * 
 * Tests the automatic detection of configuration file formats based on file
 * extensions and content, supporting JSON, YAML, and INI-style formats.
 */
void testFormatDetection() {
    std::cout << "Running testFormatDetection..." << std::endl;
    
    // Test file extension detection
    assert(nlm::Config::detectFileFormat("config.json") == "json");
    assert(nlm::Config::detectFileFormat("config.yaml") == "yaml");
    assert(nlm::Config::detectFileFormat("config.yml") == "yaml");
    assert(nlm::Config::detectFileFormat("config.ini") == "ini");
    assert(nlm::Config::detectFileFormat("config.cfg") == "ini");
    
    // Test content detection
    std::string jsonContent = "{\"key\": \"value\"}";
    assert(nlm::Config::detectFormatFromContent(jsonContent) == "json");
    
    std::string yamlContent = "key: value";
    assert(nlm::Config::detectFormatFromContent(yamlContent) == "yaml");
    
    std::string iniContent = "key = value";
    assert(nlm::Config::detectFormatFromContent(iniContent) == "ini");
    
    std::cout << "    testFormatDetection passed" << std::endl;
}

/**
 * @brief Test comprehensive configuration operations.
 * 
 * Runs a series of configuration operations to test the complete workflow
 * of the enhanced configuration system.
 */
void testConfigComprehensive() {
    std::cout << "Running testConfigComprehensive..." << std::endl;
    nlm::Config config;
    
    // Add multiple configuration values
    config.set("network.neuron_count", 1000, nlm::ConfigSource::Runtime, 
               "Number of neurons in the network", "int(100,10000)");
    config.set("network.region_count", 5, nlm::ConfigSource::Runtime,
               "Number of neural regions", "int(1,20)");
    config.set("simulation.timestep", 0.001, nlm::ConfigSource::Runtime,
               "Simulation time step in seconds", "double(0.0001,0.1)");
    config.set("experiment.name", "learning_test", nlm::ConfigSource::Runtime,
               "Experiment name", "string(1,100)");
    config.set("experiment.enabled", true, nlm::ConfigSource::Runtime,
               "Whether experiment mode is enabled", "bool");
    
    // Validate configuration
    assert(config.validate());
    
    // Get specific values
    assert(config.getOr<int64_t>("network.neuron_count", 500) == 1000);
    assert(config.getOr<double>("simulation.timestep", 0.01) == 0.001);
    assert(config.getOr<std::string>("experiment.name", "default") == "learning_test");
    assert(config.getOr<bool>("experiment.enabled", false) == true);
    
    // Test source tracking
    assert(config.getSource("network.neuron_count") == nlm::ConfigSource::Runtime);
    
    // Clean up
    config.clear();
    assert(config.getEntryCount() == 0);
    
    std::cout << "    testConfigComprehensive passed" << std::endl;
}

/**
 * @brief Run all configuration tests.
 * 
 * Executes all configuration tests and reports the overall success status.
 * This provides a comprehensive test suite for the enhanced configuration system.
 */
void runAll() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║            Enhanced Configuration System Tests               ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigSourceTracking();
    testFormatDetection();
    testConfigComprehensive();
    
    std::cout << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                  All Config Tests Passed!                   ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

} // namespace test_config_enhanced