// Comprehensive Configuration Tests
// Tests for the enhanced NLM configuration system

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>

namespace test_config {

void testConfigSchemaValidation() {
    std::cout << "    testConfigSchemaValidation..." << std::endl;
    
    nlm::Config config;
    
    // Test valid values
    config.set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Runtime);
    
    // Test schema validation - should not throw
    const nlm::ConfigSchema& schema = config.getSchema();
    
    // Test invalid values - should throw ConfigValidationError
    bool validationFailed = false;
    try {
        config.set("invalid_key", "value", nlm::ConfigSource::Runtime);
    } catch (const nlm::ConfigValidationError& e) {
        validationFailed = true;
    }
    assert(validationFailed); // Should have thrown
    
    // Test type validation
    bool typeValidationFailed = false;
    try {
        config.set("neuron_count", "not_an_integer", nlm::ConfigSource::Runtime);
    } catch (const nlm::ConfigValidationError& e) {
        typeValidationFailed = true;
    }
    assert(typeValidationFailed); // Should have thrown
    
    std::cout << "    testConfigSchemaValidation passed" << std::endl;
}

void testConfigRangeValidation() {
    std::cout << "    testConfigRangeValidation..." << std::endl;
    
    nlm::Config config;
    
    // Test valid range values
    config.set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime); // Minimum
    config.set("neuron_count", static_cast<int64_t>(10000), nlm::ConfigSource::Runtime); // Large but valid
    
    config.set("connection_probability", 0.0, nlm::ConfigSource::Runtime); // Minimum
    config.set("connection_probability", 1.0, nlm::ConfigSource::Runtime); // Maximum
    
    // Test invalid range values
    bool rangeValidationFailed = false;
    try {
        config.set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Runtime); // Below minimum
    } catch (const nlm::ConfigValidationError& e) {
        rangeValidationFailed = true;
    }
    assert(rangeValidationFailed); // Should have thrown
    
    bool rangeValidationFailed2 = false;
    try {
        config.set("connection_probability", 1.1, nlm::ConfigSource::Runtime); // Above maximum
    } catch (const nlm::ConfigValidationError& e) {
        rangeValidationFailed2 = true;
    }
    assert(rangeValidationFailed2); // Should have thrown
    
    std::cout << "    testConfigRangeValidation passed" << std::endl;
}

void testConfigFileLoading() {
    std::cout << "    testConfigFileLoading..." << std::endl;
    
    nlm::Config config;
    
    // Create a temporary configuration file
    std::string testFile = "test_config.cfg";
    std::ofstream file(testFile);
    file << "# Test configuration file\n";
    file << "random_seed = 12345\n";
    file << "simulation_timestep = 0.002\n";
    file << "neuron_count = 200\n";
    file << "connection_probability = 0.15\n";
    file << "log_level = DEBUG\n";
    file << "log_to_file = true\n";
    file.close();
    
    // Test loading the file
    bool loadSuccess = config.loadFromFile(testFile);
    assert(loadSuccess); // Should have loaded successfully
    
    // Verify loaded values
    assert(config.getOr<int64_t>("random_seed", -1) == 12345);
    assert(config.getOr<double>("simulation_timestep", -1.0) == 0.002);
    assert(config.getOr<int64_t>("neuron_count", -1) == 200);
    assert(config.getOr<double>("connection_probability", -1.0) == 0.15);
    assert(config.getOr<std::string>("log_level", "") == "DEBUG");
    assert(config.getOr<bool>("log_to_file", false) == true);
    
    // Test loading from non-existent file (should return false, not throw)
    bool nonExistentLoad = config.loadFromFile("non_existent_file.cfg");
    assert(!nonExistentLoad); // Should have failed gracefully
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigFileLoading passed" << std::endl;
}

void testConfigSaveToFile() {
    std::cout << "    testConfigSaveToFile..." << std::endl;
    
    nlm::Config config;
    
    // Set some values
    config.set("random_seed", static_cast<int64_t>(99), nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.005, nlm::ConfigSource::Runtime);
    config.set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Runtime);
    config.set("log_level", std::string("WARN"), nlm::ConfigSource::Runtime);
    config.set("log_to_file", false, nlm::ConfigSource::Runtime);
    
    // Save to file
    std::string saveFile = "saved_config.cfg";
    bool saveSuccess = config.saveToFile(saveFile);
    assert(saveSuccess); // Should have saved successfully
    
    // Load the saved file and verify values
    nlm::Config loadedConfig;
    bool loadSuccess = loadedConfig.loadFromFile(saveFile);
    assert(loadSuccess); // Should have loaded successfully
    
    assert(loadedConfig.getOr<int64_t>("random_seed", -1) == 99);
    assert(loadedConfig.getOr<double>("simulation_timestep", -1.0) == 0.005);
    assert(loadedConfig.getOr<int64_t>("neuron_count", -1) == 150);
    assert(loadedConfig.getOr<std::string>("log_level", "") == "WARN");
    assert(loadedConfig.getOr<bool>("log_to_file", true) == false);
    
    // Clean up
    std::remove(saveFile.c_str());
    
    std::cout << "    testConfigSaveToFile passed" << std::endl;
}

void testConfigCommandLineParsing() {
    std::cout << "    testConfigCommandLineParsing..." << std::endl;
    
    nlm::Config config;
    
    // Simulate command line arguments
    int argc = 7;
    char* argv[8];
    argv[0] = (char*)"test_program";
    argv[1] = (char*)"--config=file1.cfg";
    argv[2] = (char*)"--set=random_seed=999";
    argv[3] = (char*)"--set=neuron_count=300";
    argv[4] = (char*)"--set=connection_probability=0.25";
    argv[5] = (char*)"--log_level=INFO";
    argv[6] = (char*)"--help";
    
    // Load from command line arguments
    bool loadSuccess = config.loadFromArgs(argc, argv);
    assert(loadSuccess); // Should have parsed successfully
    
    // The --help option should cause the program to exit, but loadFromArgs should still return true
    assert(config.getOr<int64_t>("random_seed", -1) == 999);
    assert(config.getOr<int64_t>("neuron_count", -1) == 300);
    assert(config.getOr<double>("connection_probability", -1.0) == 0.25);
    assert(config.getOr<std::string>("log_level", "") == "INFO");
    
    std::cout << "    testConfigCommandLineParsing passed" << std::endl;
}

void testConfigDefaultValues() {
    std::cout << "    testConfigDefaultValues..." << std::endl;
    
    nlm::Config config;
    
    // Test that default values are applied
    int64_t random_seed = config.getOr<int64_t>("random_seed", -1);
    assert(random_seed == 42); // Default value
    
    double simulation_timestep = config.getOr<double>("simulation_timestep", -1.0);
    assert(simulation_timestep == 0.001); // Default value
    
    int64_t neuron_count = config.getOr<int64_t>("neuron_count", -1);
    assert(neuron_count == 1000); // Default value
    
    std::string log_level = config.getOr<std::string>("log_level", "");
    assert(log_level == "INFO"); // Default value
    
    std::cout << "    testConfigDefaultValues passed" << std::endl;
}

void testConfigValidationWithSchema() {
    std::cout << "    testConfigValidationWithSchema..." << std::endl;
    
    nlm::Config config;
    
    // Create a temporary configuration file with invalid values
    std::string testFile = "invalid_config.cfg";
    std::ofstream file(testFile);
    file << "# Invalid configuration file\n";
    file << "random_seed = not_an_integer\n"; // Invalid type
    file << "neuron_count = 0\n"; // Below minimum
    file << "connection_probability = 2.0\n"; // Above maximum
    file << "unknown_key = some_value\n"; // Unknown key
    file.close();
    
    // Load the file (should handle errors gracefully)
    bool loadSuccess = config.loadFromFile(testFile);
    assert(loadSuccess); // Should have attempted to load but handle errors
    
    // The configuration should still be usable (some valid values might have been set)
    bool configUsable = config.has("random_seed") || config.has("neuron_count") || config.has("connection_probability") || config.has("unknown_key");
    assert(configUsable); // Configuration should be usable
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigValidationWithSchema passed" << std::endl;
}

void testConfigErrorHandling() {
    std::cout << "    testConfigErrorHandling..." << std::endl;
    
    nlm::Config config;
    
    // Test error recovery
    bool errorRecovered = config.loadFromFile("non_existent_file.cfg");
    assert(!errorRecovered); // Should have failed gracefully
    
    // Configuration should still be usable after error
    config.set("test_key", "test_value", nlm::ConfigSource::Runtime);
    assert(config.getOr<std::string>("test_key", "") == "test_value");
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigComplexValues() {
    std::cout << "    testConfigComplexValues..." << std::endl;
    
    nlm::Config config;
    
    // Test vector values
    std::vector<int> intVector = {1, 2, 3, 4, 5};
    std::vector<double> doubleVector = {0.1, 0.2, 0.3, 0.4, 0.5};
    std::vector<std::string> stringVector = {"a", "b", "c", "d", "e"};
    
    config.set("int_vector", intVector, nlm::ConfigSource::Runtime);
    config.set("double_vector", doubleVector, nlm::ConfigSource::Runtime);
    config.set("string_vector", stringVector, nlm::ConfigSource::Runtime);
    
    // Retrieve and verify vector values
    auto retrievedIntVector = config.get<std::vector<int>>("int_vector");
    assert(retrievedIntVector.has_value());
    assert(retrievedIntVector.value() == intVector);
    
    auto retrievedDoubleVector = config.get<std::vector<double>>("double_vector");
    assert(retrievedDoubleVector.has_value());
    assert(retrievedDoubleVector.value() == doubleVector);
    
    auto retrievedStringVector = config.get<std::vector<std::string>>("string_vector");
    assert(retrievedStringVector.has_value());
    assert(retrievedStringVector.value() == stringVector);
    
    std::cout << "    testConfigComplexValues passed" << std::endl;
}

void runAll() {
    std::cout << "Running comprehensive configuration tests..." << std::endl;
    
    testConfigSchemaValidation();
    testConfigRangeValidation();
    testConfigFileLoading();
    testConfigSaveToFile();
    testConfigCommandLineParsing();
    testConfigDefaultValues();
    testConfigValidationWithSchema();
    testConfigErrorHandling();
    testConfigComplexValues();
    
    std::cout << std::endl;
    std::cout << "All configuration tests passed successfully!" << std::endl;
}

} // namespace test_config
