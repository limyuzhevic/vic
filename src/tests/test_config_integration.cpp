// Config Integration Tests
// Tests for configuration integration points including JSON/YAML parsing and saving,
// parameter validation, type conversion utilities, and backward compatibility

#include "src/core/Config/Config.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <cmath>

namespace test_config_integration {

void testJsonParsingAndSaving() {
    std::cout << "    Testing JSON parsing and saving..." << std::endl;
    
    nlm::Config config;
    
    // Set some test values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    // Create temporary JSON file
    const std::string jsonFile = "/tmp/agent_d7ce0cd8-58a1-4ca3-8a7c-3f364158a1a0/test_config.json";
    
    // Save to JSON
    bool saveResult = config.saveToFile(jsonFile);
    assert(saveResult);
    
    // Create new config and load from JSON
    nlm::Config config2;
    bool loadResult = config2.loadFromFile(jsonFile);
    assert(loadResult);
    
    // Verify values match
    assert(config2.get<int>("int_value").value() == 42);
    assert(config2.get<double>("double_value").value() > 3.13 && config2.get<double>("double_value").value() < 3.15);
    assert(config2.get<std::string>("string_value").value() == "hello");
    assert(config2.get<bool>("bool_value").value() == true);
    assert(config2.get<int64_t>("neuron_count").value() == 1000);
    assert(config2.get<double>("connection_probability").value() > 0.09 && config2.get<double>("connection_probability").value() < 0.11);
    
    // Clean up
    std::remove(jsonFile.c_str());
    
    std::cout << "    JSON parsing and saving test passed" << std::endl;
}

void testYamlParsingAndSaving() {
    std::cout << "    Testing YAML parsing and saving..." << std::endl;
    
    nlm::Config config;
    
    // Set some test values
    config.set("int_value", 123, nlm::ConfigSource::Runtime);
    config.set("double_value", 2.718, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("world"), nlm::ConfigSource::Runtime);
    config.set("bool_value", false, nlm::ConfigSource::Runtime);
    
    // Create temporary YAML file
    const std::string yamlFile = "/tmp/agent_d7ce0cd8-58a1-4ca3-8a7c-3f364158a1a0/test_config.yaml";
    
    // Save to YAML
    bool saveResult = config.saveToFile(yamlFile);
    assert(saveResult);
    
    // Create new config and load from YAML
    nlm::Config config2;
    bool loadResult = config2.loadFromFile(yamlFile);
    assert(loadResult);
    
    // Verify values match
    assert(config2.get<int>("int_value").value() == 123);
    assert(config2.get<double>("double_value").value() > 2.717 && config2.get<double>("double_value").value() < 2.719);
    assert(config2.get<std::string>("string_value").value() == "world");
    assert(config2.get<bool>("bool_value").value() == false);
    
    // Clean up
    std::remove(yamlFile.c_str());
    
    std::cout << "    YAML parsing and saving test passed" << std::endl;
}

void testParameterValidation() {
    std::cout << "    Testing parameter validation..." << std::endl;
    
    nlm::Config config;
    
    // Test neuron_count validation (> 0)
    config.set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Runtime);
    std::string error = config.getValidationError("neuron_count");
    assert(!error.empty()); // Should have validation error
    
    config.set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
    error = config.getValidationError("neuron_count");
    assert(error.empty()); // Should be valid
    
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Runtime);
    error = config.getValidationError("neuron_count");
    assert(error.empty()); // Should be valid
    
    // Test connection_probability validation (0-1)
    config.set("connection_probability", -0.1, nlm::ConfigSource::Runtime);
    error = config.getValidationError("connection_probability");
    assert(!error.empty()); // Should have validation error
    
    config.set("connection_probability", 0.5, nlm::ConfigSource::Runtime);
    error = config.getValidationError("connection_probability");
    assert(error.empty()); // Should be valid
    
    config.set("connection_probability", 1.0, nlm::ConfigSource::Runtime);
    error = config.getValidationError("connection_probability");
    assert(error.empty()); // Should be valid
    
    config.set("connection_probability", 1.1, nlm::ConfigSource::Runtime);
    error = config.getValidationError("connection_probability");
    assert(!error.empty()); // Should have validation error
    
    std::cout << "    Parameter validation test passed" << std::endl;
}

void testTypeConversionUtilities() {
    std::cout << "    Testing type conversion utilities..." << std::endl;
    
    nlm::Config config;
    
    // Test int to JSON
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    auto val = config.get<int>("int_val");
    assert(val && *val == 42);
    
    // Test double to JSON
    config.set("double_val", 3.14159, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_val");
    assert(doubleVal && *doubleVal > 3.14 && *doubleVal < 3.15);
    
    // Test bool to JSON
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal && *boolVal == true);
    
    // Test string to JSON
    config.set("string_val", std::string("test"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal && *stringVal == "test");
    
    // Test vector<int> to JSON
    config.set("vector_int", std::vector<int>{1, 2, 3, 4, 5}, nlm::ConfigSource::Runtime);
    auto vectorVal = config.get<std::vector<int>>("vector_int");
    assert(vectorVal && vectorVal->size() == 5);
    assert((*vectorVal)[0] == 1 && (*vectorVal)[4] == 5);
    
    // Test vector<double> to JSON
    config.set("vector_double", std::vector<double>{1.1, 2.2, 3.3}, nlm::ConfigSource::Runtime);
    auto vectorDoubleVal = config.get<std::vector<double>>("vector_double");
    assert(vectorDoubleVal && vectorDoubleVal->size() == 3);
    
    // Test vector<string> to JSON
    config.set("vector_string", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    auto vectorStringVal = config.get<std::vector<std::string>>("vector_string");
    assert(vectorStringVal && vectorStringVal->size() == 3);
    assert((*vectorStringVal)[0] == "a" && (*vectorStringVal)[2] == "c");
    
    std::cout << "    Type conversion utilities test passed" << std::endl;
}

void testBackwardCompatibilityKeyValue() {
    std::cout << "    Testing backward compatibility with simple key=value format..." << std::endl;
    
    nlm::Config config;
    
    // Create temporary simple config file
    const std::string simpleFile = "/tmp/agent_d7ce0cd8-58a1-4ca3-8a7c-3f364158a1a0/test_simple.cfg";
    std::ofstream file(simpleFile);
    file << "# Test configuration file\n";
    file << "int_value = 100\n";
    file << "double_value = 3.14\n";
    file << "bool_value = true\n";
    file << "string_value = \"hello world\"\n";
    file << "vector_value = [10, 20, 30]\n";
    file.close();
    
    // Load from simple key=value format
    bool loadResult = config.loadFromFile(simpleFile);
    assert(loadResult);
    
    // Verify values were parsed correctly
    assert(config.get<int>("int_value").value() == 100);
    assert(config.get<double>("double_value").value() > 3.13 && config.get<double>("double_value").value() < 3.15);
    assert(config.get<bool>("bool_value").value() == true);
    assert(config.get<std::string>("string_value").value() == "hello world");
    
    // Note: vector<int> parsing from simple format may not be supported
    // The current implementation only supports basic types in simple format
    
    std::remove(simpleFile.c_str());
    
    std::cout << "    Backward compatibility test passed" << std::endl;
}

void testConfigSerializationRoundTrip() {
    std::cout << "    Testing serialization round-trip (JSON -> Config -> YAML -> Config)..." << std::endl;
    
    nlm::Config config;
    
    // Set comprehensive test values
    config.set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Runtime);
    config.set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.25, nlm::ConfigSource::Runtime);
    config.set("random_seed", static_cast<int64_t>(123), nlm::ConfigSource::Runtime);
    config.set("stdp_ltp_weight", 0.01, nlm::ConfigSource::Runtime);
    config.set("stdp_ltd_weight", 0.012, nlm::ConfigSource::Runtime);
    config.set("stdp_tau", 20.0, nlm::ConfigSource::Runtime);
    config.set("synaptogenesis_rate", 0.0001, nlm::ConfigSource::Runtime);
    config.set("pruning_rate", 0.00001, nlm::ConfigSource::Runtime);
    config.set("replay_interval", 100, nlm::ConfigSource::Runtime);
    config.set("consolidation_interval", 1000, nlm::ConfigSource::Runtime);
    config.set("description", std::string("Test configuration"), nlm::ConfigSource::Runtime);
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    
    // Create temporary files
    const std::string jsonFile = "/tmp/agent_d7ce0cd8-58a1-4ca3-8a7c-3f364158a1a0/test_roundtrip.json";
    const std::string yamlFile = "/tmp/agent_d7ce0cd8-58a1-4ca3-8a7c-3f364158a1a0/test_roundtrip.yaml";
    
    // Save as JSON
    assert(config.saveToFile(jsonFile));
    
    // Load from JSON
    nlm::Config config2;
    assert(config2.loadFromFile(jsonFile));
    
    // Save as YAML
    assert(config2.saveToFile(yamlFile));
    
    // Load from YAML
    nlm::Config config3;
    assert(config3.loadFromFile(yamlFile));
    
    // Verify all values match
    assert(config3.get<int64_t>("neuron_count").value() == 500);
    assert(config3.get<int64_t>("region_count").value() == 2);
    assert(config3.get<double>("connection_probability").value() > 0.24 && config3.get<double>("connection_probability").value() < 0.26);
    assert(config3.get<int64_t>("random_seed").value() == 123);
    assert(config3.get<double>("stdp_ltp_weight").value() == 0.01);
    assert(config3.get<double>("stdp_ltd_weight").value() == 0.012);
    assert(config3.get<double>("stdp_tau").value() == 20.0);
    assert(config3.get<double>("synaptogenesis_rate").value() == 0.0001);
    assert(config3.get<double>("pruning_rate").value() == 0.00001);
    assert(config3.get<int64_t>("replay_interval").value() == 100);
    assert(config3.get<int64_t>("consolidation_interval").value() == 1000);
    assert(config3.get<std::string>("description").value() == "Test configuration");
    assert(config3.get<bool>("enabled").value() == true);
    
    std::remove(jsonFile.c_str());
    std::remove(yamlFile.c_str());
    
    std::cout << "    Serialization round-trip test passed" << std::endl;
}

void runAll() {
    testJsonParsingAndSaving();
    testYamlParsingAndSaving();
    testParameterValidation();
    testTypeConversionUtilities();
    testBackwardCompatibilityKeyValue();
    testConfigSerializationRoundTrip();
}

} // namespace test_config_integration
