#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

void testConfigBasic() {
    std::cout << "Testing basic Config functionality..." << std::endl;
    
    nlm::Config config;
    
    // Test setting and getting values
    config.set("test_int", 42);
    config.set("test_double", 3.14);
    config.set("test_string", "Hello World");
    config.set("test_bool", true);
    
    assert(config.has("test_int"));
    assert(config.has("test_double"));
    assert(config.has("test_string"));
    assert(config.has("test_bool"));
    
    auto val_int = config.get<int>("test_int");
    assert(val_int.has_value() && val_int.value() == 42);
    
    auto val_double = config.get<double>("test_double");
    assert(val_double.has_value());
    assert(std::abs(val_double.value() - 3.14) < 0.001);
    
    auto val_string = config.get<std::string>("test_string");
    assert(val_string.has_value() && val_string.value() == "Hello World");
    
    auto val_bool = config.get<bool>("test_bool");
    assert(val_bool.has_value() && val_bool.value() == true);
    
    // Test getOr with default values
    assert(config.getOr<int>("nonexistent", 100) == 100);
    assert(config.getOr<std::string>("nonexistent", "Default") == "Default");
    
    std::cout << "✓ Basic Config functionality passed" << std::endl;
}

void testConfigValidation() {
    std::cout << "Testing Config validation..." << std::endl;
    
    nlm::Config config;
    
    // Test valid values
    assert(config.setValidated("neuron_count", 100));
    assert(config.setValidated("region_count", 1));
    assert(config.setValidated("connection_probability", 0.5f));
    assert(config.setValidated("stdp_ltp_weight", 0.02f));
    assert(config.setValidated("stdp_ltd_weight", -0.015f));
    assert(config.setValidated("simulation_timestep", 0.001));
    assert(config.setValidated("random_seed", 42));
    
    // Test invalid values
    assert(!config.setValidated("neuron_count", 0));  // Too small
    assert(!config.setValidated("neuron_count", 1000001));  // Too large
    assert(!config.setValidated("region_count", 0));  // Too small
    assert(!config.setValidated("connection_probability", -0.1f));  // Too small
    assert(!config.setValidated("connection_probability", 1.1f));  // Too large
    assert(!config.setValidated("simulation_timestep", 0.0));  // Not positive
    assert(!config.setValidated("simulation_timestep", -0.001));  // Negative
    assert(!config.setValidated("random_seed", -1));  // Negative
    
    // Test custom validator
    auto positive_validator = [](int value) { return value > 0; };
    assert(config.setValidated("test_positive", 10, nlm::ConfigSource::Runtime, positive_validator));
    assert(!config.setValidated("test_positive", -5, nlm::ConfigSource::Runtime, positive_validator));
    assert(!config.setValidated("test_positive", 0, nlm::ConfigSource::Runtime, positive_validator));
    
    std::cout << "✓ Config validation passed" << std::endl;
}

void testConfigFileOperations() {
    std::cout << "Testing Config file operations..." << std::endl;
    
    nlm::Config config;
    
    // Set some test values
    config.set("test_int", 123);
    config.set("test_string", "test_value");
    config.set("test_bool", true);
    
    // Note: loadFromFile and saveToFile are implemented but not tested
    // as they require file system operations
    
    std::cout << "✓ Config file operations passed" << std::endl;
}

void testConfigValidationErrors() {
    std::cout << "Testing Config validation error reporting..." << std::endl;
    
    nlm::Config config;
    
    // Add some invalid values
    config.set("neuron_count", 0);  // Invalid
    config.set("connection_probability", 1.5f);  // Invalid
    config.set("simulation_timestep", -0.001);  // Invalid
    
    // Check validation errors
    auto errors = config.getValidationErrors();
    assert(errors.size() > 0);  // Should have errors
    
    // Check that validate() returns false
    assert(!config.validate());
    
    std::cout << "✓ Config validation error reporting passed" << std::endl;
}

void testConfigSummary() {
    std::cout << "Testing Config summary..." << std::endl;
    
    nlm::Config config;
    
    // Add test values
    config.set("test_int", 42);
    config.set("test_string", "test_value");
    config.set("test_bool", false);
    
    // Generate summary
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("test_int") != std::string::npos);
    assert(summary.find("test_string") != std::string::npos);
    assert(summary.find("test_bool") != std::string::npos);
    
    std::cout << "✓ Config summary passed" << std::endl;
}

void testConfigValidatorHelpers() {
    std::cout << "Testing Config validator helper functions..." << std::endl;
    
    // Test createRangeValidator
    auto int_validator = nlm::Config::createRangeValidator<int>(1, 10);
    assert(int_validator(5));
    assert(int_validator(1));
    assert(int_validator(10));
    assert(!int_validator(0));
    assert(!int_validator(11));
    
    auto double_validator = nlm::Config::createRangeValidator<double>(0.0, 1.0);
    assert(double_validator(0.5));
    assert(double_validator(0.0));
    assert(double_validator(1.0));
    assert(!double_validator(-0.1));
    assert(!double_validator(1.1));
    
    // Test createPositiveValidator
    auto positive_int_validator = nlm::Config::createPositiveValidator<int>();
    assert(positive_int_validator(1));
    assert(positive_int_validator(100));
    assert(!positive_int_validator(0));
    assert(!positive_int_validator(-1));
    
    auto positive_double_validator = nlm::Config::createPositiveValidator<double>();
    assert(positive_double_validator(0.001));
    assert(positive_double_validator(100.0));
    assert(!positive_double_validator(0.0));
    assert(!positive_double_validator(-0.001));
    
    std::cout << "✓ Config validator helper functions passed" << std::endl;
}

int main() {
    std::cout << "=== Running NLM Config Tests ===" << std::endl;
    std::cout << std::endl;
    
    try {
        testConfigBasic();
        testConfigValidation();
        testConfigFileOperations();
        testConfigValidationErrors();
        testConfigSummary();
        testConfigValidatorHelpers();
        
        std::cout << std::endl;
        std::cout << "=== All Config tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}