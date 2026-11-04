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

void testConfigTypeSafeGetters() {
    nlm::Config config;
    
    // Test int64_t getter
    config.set("int_key", int64_t(100), nlm::ConfigSource::Runtime);
    int64_t intVal = 0;
    assert(config.get<int64_t>("int_key", intVal));
    assert(intVal == 100);
    
    // Test double getter
    config.set("double_key", 3.14, nlm::ConfigSource::Runtime);
    double doubleVal = 0.0;
    assert(config.get<double>("double_key", doubleVal));
    assert(doubleVal > 3.13 && doubleVal < 3.15);
    
    // Test bool getter
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    bool boolVal = false;
    assert(config.get<bool>("bool_key", boolVal));
    assert(boolVal == true);
    
    // Test string getter
    config.set("string_key", std::string("test"), nlm::ConfigSource::Runtime);
    std::string stringVal = "";
    assert(config.get<std::string>("string_key", stringVal));
    assert(stringVal == "test");
    
    std::cout << "    testConfigTypeSafeGetters passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test empty validation
    assert(config.validate());
    std::string errors = config.getValidationErrors();
    assert(errors.find("Configuration is valid") != std::string::npos);
    
    // Set valid values
    config.set("random_seed", int64_t(42), nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("neuron_count", int64_t(100), nlm::ConfigSource::Runtime);
    config.set("region_count", int64_t(1), nlm::ConfigSource::Runtime);
    
    // Test valid configuration
    assert(config.validate());
    errors = config.getValidationErrors();
    assert(errors.find("Configuration is valid") != std::string::npos);
    
    // Test invalid configuration - wrong type for random_seed
    config.set("random_seed", 3.14, nlm::ConfigSource::Runtime); // Should be int64
    assert(!config.validate());
    errors = config.getValidationErrors();
    assert(errors.find("Expected int64 for random_seed") != std::string::npos);
    
    // Reset to valid configuration
    config.set("random_seed", int64_t(42), nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", -0.001, nlm::ConfigSource::Runtime); // Invalid: must be positive
    assert(!config.validate());
    errors = config.getValidationErrors();
    assert(errors.find("simulation_timestep must be positive") != std::string::npos);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigInputValidation() {
    nlm::Config config;
    
    // Test invalid key format
    config.set("invalid key with spaces", 1, nlm::ConfigSource::Runtime);
    assert(!config.validate());
    std::string errors = config.getValidationErrors();
    assert(errors.find("Invalid configuration key format") != std::string::npos);
    
    // Test required key validation
    config.clear();
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("neuron_count", int64_t(100), nlm::ConfigSource::Runtime);
    // Missing required keys: random_seed, region_count
    assert(!config.validate());
    errors = config.getValidationErrors();
    assert(errors.find("Required configuration key is missing") != std::string::npos);
    
    std::cout << "    testConfigInputValidation passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigTypeSafeGetters();
    testConfigValidation();
    testConfigInputValidation();
}

} // namespace test_config
