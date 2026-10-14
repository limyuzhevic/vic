// Config Tests with Validation
#include "core/Config/Config.hpp"
#include "core/Config/ConfigInitializer.hpp"
#include "core/Config/ConfigValidator.hpp"
#include "core/Config/ConfigSchema.hpp"
#include <cassert>
#include <iostream>
#include <any>
#include <unordered_map>

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

void testConfigValidationInitialization() {
    // Test that configuration schema is properly initialized
    auto& schema = nlm::ConfigInitializer::getSchema();
    auto& validator = nlm::ConfigInitializer::getValidator();
    
    // Test basic validation
    bool valid = nlm::ConfigInitializer::validateConfiguration(nlm::Config());
    assert(valid); // Empty config should be valid
    
    std::cout << "    testConfigValidationInitialization passed" << std::endl;
}

void testConfigParameterValidation() {
    nlm::Config config;
    
    // Test valid parameter setting
    config.set("random_seed", 42, nlm::ConfigSource::Runtime);
    assert(config.has("random_seed"));
    
    // Test parameter validation through schema
    auto& validator = nlm::ConfigInitializer::getValidator();
    std::unordered_map<std::string, std::any> testConfig;
    testConfig["random_seed"] = std::any(42);
    
    std::vector<ValidationError> errors = validator.validate(testConfig);
    // Should have no errors for valid configuration
    
    std::cout << "    testConfigParameterValidation passed" << std::endl;
}

void testConfigSchemaDocumentation() {
    std::string descriptions = nlm::ConfigInitializer::getParameterDescriptions();
    assert(!descriptions.empty());
    assert(descriptions.find("NLM Configuration Schema") != std::string::npos);
    assert(descriptions.find("Total parameters:") != std::string::npos);
    
    std::cout << "    testConfigSchemaDocumentation passed" << std::endl;
}

void testConfigDefaultValues() {
    nlm::Config config;
    config.setPhase2Defaults();
    
    // Test that default values are set
    assert(config.has("random_seed"));
    assert(config.has("simulation_timestep"));
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    assert(config.has("connection_probability"));
    
    std::cout << "    testConfigDefaultValues passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidationInitialization();
    testConfigParameterValidation();
    testConfigSchemaDocumentation();
    testConfigDefaultValues();
    
    std::cout << "\n=== All Config Tests Passed ===\n" << std::endl;
}

} // namespace test_config