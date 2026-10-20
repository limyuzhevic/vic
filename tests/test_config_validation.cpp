// Configuration Validation Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_config_validation {

void testValidationBasic() {
    nlm::Config config;
    
    // Test valid values
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Runtime);
    config.set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.15f, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("log_level", std::string("INFO"), nlm::ConfigSource::Runtime);
    
    std::cout << "    testValidationBasic passed" << std::endl;
}

void testValidationInvalidValues() {
    nlm::Config config;
    
    // Test invalid values that should trigger warnings
    config.set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Runtime);  // Invalid: 0
    config.set("neuron_count", static_cast<int64_t>(100000), nlm::ConfigSource::Runtime);  // Invalid: too high
    config.set("region_count", static_cast<int64_t>(1000), nlm::ConfigSource::Runtime);  // Invalid: too high
    config.set("connection_probability", 1.5f, nlm::ConfigSource::Runtime);  // Invalid: > 1.0
    config.set("connection_probability", -0.5f, nlm::ConfigSource::Runtime);  // Invalid: < 0.0
    config.set("simulation_timestep", 0.0, nlm::ConfigSource::Runtime);  // Invalid: 0.0
    config.set("simulation_timestep", 10.0, nlm::ConfigSource::Runtime);  // Invalid: > 1.0
    config.set("log_level", std::string("INVALID"), nlm::ConfigSource::Runtime);  // Invalid: not in list
    
    std::cout << "    testValidationInvalidValues passed" << std::endl;
}

void testValidationConfigurationFile() {
    nlm::Config config;
    
    // Test loading from config file with validation
    // Create a temporary config file for testing
    const std::string testConfigFile = "test_config.cfg";
    
    // Write test config content
    std::ofstream testFile(testConfigFile);
    testFile << "# Test config file\n";
    testFile << "neuron_count = 500\n";
    testFile << "region_count = 2\n";
    testFile << "connection_probability = 0.15\n";
    testFile << "simulation_timestep = 0.001\n";
    testFile << "log_level = INFO\n";
    testFile.close();
    
    // Load the config file
    bool success = config.loadFromFile(testConfigFile);
    assert(success);
    
    // Verify values were loaded correctly
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    assert(config.has("connection_probability"));
    assert(config.has("simulation_timestep"));
    assert(config.has("log_level"));
    
    // Clean up
    std::remove(testConfigFile.c_str());
    
    std::cout << "    testValidationConfigurationFile passed" << std::endl;
}

void testValidationCommandLineArgs() {
    nlm::Config config;
    
    // Test parsing command line arguments
    int argc = 5;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--neuron_count=200",
        (char*)"--region_count=3",
        (char*)"--connection_probability=0.2",
        (char*)"--log_level=INFO"
    };
    
    bool success = config.loadFromArgs(argc, argv);
    assert(success);
    
    // Verify values were loaded correctly
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    assert(config.has("connection_probability"));
    assert(config.has("log_level"));
    
    std::cout << "    testValidationCommandLineArgs passed" << std::endl;
}

void testValidationErrorHandling() {
    nlm::Config config;
    
    // Test error handling for malformed config file
    const std::string malformedConfigFile = "malformed_config.cfg";
    
    // Write malformed config content
    std::ofstream testFile(malformedConfigFile);
    testFile << "# Valid line\n";
    testFile << "invalid_line_without_equals\n";
    testFile << "key=value\n";
    testFile << "=value_only\n";
    testFile << "key=\n";
    testFile.close();
    
    // Load the malformed config file (should handle errors gracefully)
    bool success = config.loadFromFile(malformedConfigFile);
    // It should still succeed because valid lines should be parsed
    assert(success);
    
    // Verify that valid lines were still loaded
    assert(config.has("key"));
    
    // Clean up
    std::remove(malformedConfigFile.c_str());
    
    std::cout << "    testValidationErrorHandling passed" << std::endl;
}

void testValidationSaveToFile() {
    nlm::Config config;
    
    // Set up configuration
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Runtime);
    config.set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.15f, nlm::ConfigSource::Runtime);
    
    // Save to file
    const std::string saveFile = "test_save.cfg";
    bool success = config.saveToFile(saveFile);
    assert(success);
    
    // Verify file was created and has content
    std::ifstream savedFile(saveFile);
    assert(savedFile.is_open());
    
    std::string line;
    int lineCount = 0;
    while (std::getline(savedFile, line)) {
        lineCount++;
        // Should have comments and key=value pairs
        if (line.find("neuron_count") != std::string::npos) {
            assert(line.find("=") != std::string::npos);
        }
    }
    
    assert(lineCount > 0);  // Should have saved entries
    
    // Clean up
    std::remove(saveFile.c_str());
    
    std::cout << "    testValidationSaveToFile passed" << std::endl;
}

void runAll() {
    testValidationBasic();
    testValidationInvalidValues();
    testValidationConfigurationFile();
    testValidationCommandLineArgs();
    testValidationErrorHandling();
    testValidationSaveToFile();
}

} // namespace test_config_validation
