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

void testConfigValidate() {
    nlm::Config config;
    
    // Test valid configuration
    config.set("random_seed", 42, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config.set("log_level", std::string("INFO"), nlm::ConfigSource::Runtime);
    config.set("visualization_enabled", true, nlm::ConfigSource::Runtime);
    
    std::vector<std::string> errors;
    assert(config.validate(errors));
    assert(errors.empty());
    
    // Test invalid configuration (negative timestep)
    nlm::Config config2;
    config2.set("simulation_timestep", -0.001, nlm::ConfigSource::Runtime);
    
    std::vector<std::string> errors2;
    assert(!config2.validate(errors2));
    assert(!errors2.empty());
    
    // Test invalid configuration (zero neuron count)
    nlm::Config config3;
    config3.set("neuron_count", 0, nlm::ConfigSource::Runtime);
    
    std::vector<std::string> errors3;
    assert(!config3.validate(errors3));
    assert(!errors3.empty());
    
    std::cout << "    testConfigValidate passed" << std::endl;
}

void testConfigDuplicateKeys() {
    nlm::Config config;
    
    // Test no duplicates
    assert(!config.hasDuplicateKeys());
    
    // Add some keys
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    assert(!config.hasDuplicateKeys());
    
    // The hasDuplicateKeys method can't actually add duplicates
    // because set() updates existing keys rather than adding new ones
    // So we can't test the duplicate scenario with the current API
    // but we can verify it returns false for a clean config
    
    std::cout << "    testConfigDuplicateKeys passed" << std::endl;
}

void testConfigValueType() {
    nlm::Config config;
    
    // Test different value types
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("double_key", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_key", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    config.set("vector_key", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    
    assert(config.getValueType("int_key") == "int");
    assert(config.getValueType("double_key") == "double");
    assert(config.getValueType("string_key") == "string");
    assert(config.getValueType("bool_key") == "bool");
    assert(config.getValueType("vector_key") == "vector<int>");
    
    assert(config.getValueType("nonexistent") == "NOT_FOUND");
    
    std::cout << "    testConfigValueType passed" << std::endl;
}

void testConfigConvertTo() {
    nlm::Config config;
    
    // Test conversion from int to various types
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    
    // Should be able to convert to int, int64_t, uint64_t, float, double
    assert(config.canConvertTo<int>("int_value"));
    assert(config.canConvertTo<int64_t>("int_value"));
    assert(config.canConvertTo<uint64_t>("int_value"));
    assert(config.canConvertTo<float>("int_value"));
    assert(config.canConvertTo<double>("int_value"));
    assert(!config.canConvertTo<bool>("int_value"));
    assert(!config.canConvertTo<std::string>("int_value"));
    assert(!config.canConvertTo<std::vector<int>>("int_value"));
    
    // Test conversion from double
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    assert(config.canConvertTo<double>("double_value"));
    assert(config.canConvertTo<float>("double_value"));
    assert(config.canConvertTo<int64_t>("double_value"));
    
    // Test conversion from string
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    assert(config.canConvertTo<std::string>("string_value"));
    assert(!config.canConvertTo<int>("string_value"));
    
    // Test conversion from bool
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    assert(config.canConvertTo<bool>("bool_value"));
    assert(!config.canConvertTo<int>("bool_value"));
    
    std::cout << "    testConfigConvertTo passed" << std::endl;
}

void testConfigGetWithValidation() {
    nlm::Config config;
    
    // Test successful conversion with validation
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    
    auto intVal = config.getWithValidation<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config.getWithValidation<double>("int_value");
    assert(doubleVal.has_value());
    assert(*doubleVal == 42.0);
    
    // Test failed conversion (string cannot be converted to int)
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto invalidVal = config.getWithValidation<int>("string_value");
    assert(!invalidVal.has_value());
    
    // Test non-existent key
    auto nonExistent = config.getWithValidation<int>("nonexistent");
    assert(!nonExistent.has_value());
    
    std::cout << "    testConfigGetWithValidation passed" << std::endl;
}

} // namespace test_config
