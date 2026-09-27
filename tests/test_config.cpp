// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

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
    nlm::Config config;
    
    // Test with valid values
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    assert(config.validateValue("neuron_count", 1000));
    
    config.set("connection_probability", 0.5, nlm::ConfigSource::Runtime);
    assert(config.validateValue("connection_probability", 0.5));
    
    config.set("log_level", std::string("INFO"), nlm::ConfigSource::Runtime);
    assert(config.validateValue("log_level", std::string("INFO")));
    
    // Test with invalid values
    assert(!config.validateValue("neuron_count", 0)); // Below minimum
    assert(!config.validateValue("neuron_count", 2000000)); // Above maximum
    assert(!config.validateValue("connection_probability", 1.5)); // Above maximum
    assert(!config.validateValue("log_level", std::string("INVALID"))); // Not in allowed values
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigValidationResult() {
    nlm::Config config;
    
    // Set both valid and invalid values
    config.set("neuron_count", 2000, nlm::ConfigSource::Runtime); // Valid
    config.set("connection_probability", 1.5, nlm::ConfigSource::Runtime); // Invalid
    config.set("log_level", std::string("DEBUG"), nlm::ConfigSource::Runtime); // Valid
    
    nlm::ValidationResult result = config.validate();
    assert(!result.valid);
    assert(result.errors.size() > 0);
    assert(result.getErrors().find("connection_probability") != std::string::npos);
    
    // Check that valid values are not in errors
    assert(result.getErrors().find("neuron_count") == std::string::npos);
    assert(result.getErrors().find("log_level") == std::string::npos);
    
    std::cout << "    testConfigValidationResult passed" << std::endl;
}

void testConfigSchema() {
    nlm::Config config;
    
    // Test that schema is properly registered
    auto schema = nlm::Config::getDefaultSchema();
    assert(schema.find("neuron_count") != schema.end());
    assert(schema.find("connection_probability") != schema.end());
    assert(schema.find("log_level") != schema.end());
    
    // Test that all schema entries are valid
    for (const auto& pair : schema) {
        const auto& entry = pair.second;
        assert(config.validateKey(entry.key));
        assert(config.validateValue(entry.key, entry.value));
    }
    
    std::cout << "    testConfigSchema passed" << std::endl;
}

void testConfigLoadFromDefaults() {
    nlm::Config config;
    
    // Load default schema values
    config.loadFromDefaults();
    
    // Check that all default keys are present
    auto schema = nlm::Config::getDefaultSchema();
    for (const auto& pair : schema) {
        assert(config.has(pair.first));
    }
    
    // Check a few specific values
    assert(config.get<int>("neuron_count").has_value());
    assert(config.get<int>("neuron_count").value() == 1000);
    assert(config.get<double>("simulation_timestep").has_value());
    assert(config.get<double>("simulation_timestep").value() == 0.001);
    
    std::cout << "    testConfigLoadFromDefaults passed" << std::endl;
}

void testConfigSaveToFile() {
    nlm::Config config;
    
    // Set some values
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_string", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    
    // Save to file
    assert(config.saveToFile("test_config.cfg"));
    
    // Load it back
    nlm::Config config2;
    assert(config2.loadFromFile("test_config.cfg"));
    
    // Check values
    assert(config2.get<int>("test_int").has_value());
    assert(config2.get<int>("test_int").value() == 42);
    assert(config2.get<std::string>("test_string").has_value());
    assert(config2.get<std::string>("test_string").value() == "hello");
    
    // Clean up
    std::remove("test_config.cfg");
    
    std::cout << "    testConfigSaveToFile passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigValidationResult();
    testConfigSchema();
    testConfigLoadFromDefaults();
    testConfigSaveToFile();
}

} // namespace test_config
