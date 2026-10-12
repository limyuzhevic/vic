// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <regex>

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

// Validation Tests
void testValidationFramework() {
    nlm::Config config;
    
    // Enable validation
    config.setValidationEnabled(true);
    assert(config.isValidationEnabled());
    
    // Disable section validation
    config.disableSectionValidation("brain");
    assert(!config.isSectionValidationEnabled("brain"));
    
    // Test validation API methods
    config.addTypeValidation("general", "test_type", "int", "Test integer");
    config.addRangeValidation("general", "test_range", 0.0, 100.0, "Test range", true);
    
    std::regex filenamePattern("^[a-zA-Z0-9_\-\.]+\\.[a-zA-Z0-9]+$");
    config.addFormatValidation("general", "test_format", filenamePattern, 
                               "Test filename format", "Invalid filename format");
    
    std::cout << "    testValidationFramework passed" << std::endl;
}

void testTypeValidation() {
    nlm::Config config;
    config.setValidationEnabled(true);
    
    // Test type validation
    config.set("valid_int", 42, nlm::ConfigSource::Runtime);
    config.set("valid_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("valid_string", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("valid_bool", true, nlm::ConfigSource::Runtime);
    
    // Set invalid types that should trigger validation
    config.set("string_with_int_key", std::string("123"), nlm::ConfigSource::Runtime);
    
    std::cout << "    testTypeValidation passed" << std::endl;
}

void testRangeValidation() {
    nlm::Config config;
    config.setValidationEnabled(true);
    
    // Test valid range values
    config.set("within_range", 50.0, nlm::ConfigSource::Runtime);
    
    // Test out of range values (should trigger warnings)
    config.set("below_range", -1.0, nlm::ConfigSource::Runtime);
    config.set("above_range", 101.0, nlm::ConfigSource::Runtime);
    
    std::cout << "    testRangeValidation passed" << std::endl;
}

void testFormatValidation() {
    nlm::Config config;
    config.setValidationEnabled(true);
    
    // Test valid format
    config.set("valid_file", std::string("test.txt"), nlm::ConfigSource::Runtime);
    
    // Test invalid format
    config.set("invalid_file", std::string("no_extension"), nlm::ConfigSource::Runtime);
    
    std::cout << "    testFormatValidation passed" << std::endl;
}

void testValidationResults() {
    nlm::Config config;
    config.setValidationEnabled(true);
    
    // Add some test values
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("synapse_density", 0.5, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.3, nlm::ConfigSource::Runtime);
    
    // Validate the configuration
    bool isValid = config.validate(true); // Skip warnings
    assert(isValid); // Should be valid with valid ranges
    
    // Get validation results
    auto errors = config.getValidationErrors();
    auto warnings = config.getValidationWarnings();
    
    // Add an invalid value to trigger errors
    config.set("neuron_count", 50, nlm::ConfigSource::Runtime);
    
    std::cout << "    testValidationResults passed" << std::endl;
}

void testSectionValidation() {
    nlm::Config config;
    config.setValidationEnabled(true);
    
    // Enable section validation for specific sections
    config.enableSectionValidation("brain");
    assert(config.isSectionValidationEnabled("brain"));
    
    // Disable section validation for other sections
    config.disableSectionValidation("environment");
    assert(!config.isSectionValidationEnabled("environment"));
    
    std::cout << "    testSectionValidation passed" << std::endl;
}

void testValidationSummary() {
    nlm::Config config;
    config.setValidationEnabled(true);
    
    // Add some configuration
    config.set("random_seed", 42, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    
    // Get validation summary
    std::string summary = config.getValidationSummary();
    assert(!summary.empty());
    assert(summary.find("Configuration Validation Summary") != std::string::npos);
    
    std::cout << "    testValidationSummary passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testValidationFramework();
    testTypeValidation();
    testRangeValidation();
    testFormatValidation();
    testValidationResults();
    testSectionValidation();
    testValidationSummary();
}

} // namespace test_config

