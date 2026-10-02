// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <functional>

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
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigValidationErrors() {
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
    
    std::cout << "    testConfigValidationErrors passed" << std::endl;
}

void testConfigSummary() {
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
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigValidatorHelpers() {
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
    
    std::cout << "    testConfigValidatorHelpers passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigValidationErrors();
    testConfigSummary();
    testConfigValidatorHelpers();
}

} // namespace test_config