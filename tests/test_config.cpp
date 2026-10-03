// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

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

void testConfigSchema() {
    nlm::Config config;
    
    // Test schema initialization
    config.initializeSchema();
    
    // Test getting schema for existing key
    auto schemaEntry = config.getSchema("random_seed");
    assert(schemaEntry.has_value());
    assert(schemaEntry->key == "random_seed");
    assert(schemaEntry->type == "int64");
    assert(schemaEntry->required == false);
    
    // Test getting schema for non-existing key
    auto missingSchema = config.getSchema("nonexistent_key");
    assert(!missingSchema.has_value());
    
    // Test getting all schema keys
    auto schemaKeys = config.getSchemaKeys();
    assert(!schemaKeys.empty());
    assert(schemaKeys.size() >= 50); // Should have at least 50 config options
    
    // Verify some specific keys exist
    bool hasTimestep = std::find(schemaKeys.begin(), schemaKeys.end(), "simulation_timestep") != schemaKeys.end();
    assert(hasTimestep);
    
    bool hasNeuronCount = std::find(schemaKeys.begin(), schemaKeys.end(), "neuron_count") != schemaKeys.end();
    assert(hasNeuronCount);
    
    bool hasSTDP = std::find(schemaKeys.begin(), schemaKeys.end(), "stdp_ltp_weight") != schemaKeys.end();
    assert(hasSTDP);
    
    std::cout << "    testConfigSchema passed" << std::endl;
}

void testConfigSchemaSummary() {
    nlm::Config config;
    
    // Get schema summary
    std::string summary = config.getSchemaSummary();
    assert(!summary.empty());
    assert(summary.find("Configuration Schema") != std::string::npos);
    assert(summary.find("simulation_timestep") != std::string::npos);
    assert(summary.find("neuron_count") != std::string::npos);
    
    std::cout << "    testConfigSchemaSummary passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test setting valid values
    config.set("simulation_timestep", 0.001);
    config.set("neuron_count", static_cast<int64_t>(100));
    config.set("region_count", static_cast<int64_t>(1));
    config.set("connection_probability", 0.5);
    config.set("stdp_ltp_weight", 0.01);
    config.set("enable_debug_logging", true);
    
    assert(config.has("simulation_timestep"));
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    assert(config.has("connection_probability"));
    assert(config.has("stdp_ltp_weight"));
    assert(config.has("enable_debug_logging"));
    
    // Test getting validated values
    assert(config.getOr<double>("simulation_timestep", 0.0) == 0.001);
    assert(config.getOr<int64_t>("neuron_count", 0) == 100);
    assert(config.getOr<int64_t>("region_count", 0) == 1);
    assert(config.getOr<float>("connection_probability", 0.0f) == 0.5f);
    assert(config.getOr<float>("stdp_ltp_weight", 0.0f) == 0.01f);
    assert(config.getOr<bool>("enable_debug_logging", false) == true);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigTypeSafety() {
    nlm::Config config;
    
    // Test type safety
    config.set("int_value", 42);
    auto stringValue = config.get<std::string>("int_value");
    assert(!stringValue.has_value()); // Should fail type check
    
    config.set("string_value", "hello");
    auto intValue = config.get<int>("string_value");
    assert(!intValue.has_value()); // Should fail type check
    
    config.set("bool_value", true);
    auto doubleValue = config.get<double>("bool_value");
    assert(!doubleValue.has_value()); // Should fail type check
    
    std::cout << "    testConfigTypeSafety passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test invalid key validation
    try {
        config.set("", "value");
        assert(false && "Should have thrown validation error");
    } catch (const nlm::ConfigValidationError& e) {
        std::cout << "    testConfigErrorHandling: caught expected validation error" << std::endl;
    }
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigFileOperations() {
    nlm::Config config;
    
    // Set some values
    config.set("test_int", 42);
    config.set("test_double", 3.14159);
    config.set("test_string", "hello world");
    config.set("test_bool", true);
    config.set("test_int_array", std::vector<int>({1, 2, 3}));
    config.set("test_double_array", std::vector<double>({1.5, 2.5, 3.5}));
    config.set("test_string_array", std::vector<std::string>({"a", "b", "c"}));
    
    // Test saving to file
    bool saveResult = config.saveToFile("/tmp/test_config.json");
    assert(saveResult);
    
    // Test loading back
    nlm::Config config2;
    bool loadResult = config2.loadFromFile("/tmp/test_config.json");
    assert(loadResult);
    
    assert(config2.getOr<int>("test_int", 0) == 42);
    assert(config2.getOr<double>("test_double", 0.0) > 3.14 && config2.getOr<double>("test_double", 0.0) < 3.15);
    assert(config2.getOr<std::string>("test_string", "") == "hello world");
    assert(config2.getOr<bool>("test_bool", false) == true);
    
    std::cout << "    testConfigFileOperations passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSchema();
    testConfigSchemaSummary();
    testConfigValidation();
    testConfigTypeSafety();
    testConfigErrorHandling();
    testConfigFileOperations();
}

} // namespace test_config

