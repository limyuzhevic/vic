// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <string>

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
    
    // Test with valid parameters
    config.set("random_seed", int64_t(42), nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("neuron_count", int64_t(500), nlm::ConfigSource::Runtime);
    config.set("region_count", int64_t(2), nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1f, nlm::ConfigSource::Runtime);
    
    // Validation should pass with required parameters
    bool valid = config.validate();
    assert(valid);
    
    // Test with invalid parameters
    nlm::Config invalidConfig;
    invalidConfig.set("random_seed", int64_t(-1), nlm::ConfigSource::Runtime);  // Negative seed
    invalidConfig.set("simulation_timestep", 0.0, nlm::ConfigSource::Runtime); // Zero timestep
    
    // Validation should fail with invalid parameters
    valid = invalidConfig.validate();
    assert(!valid);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigSourceTracking() {
    nlm::Config config;
    
    config.set("param1", 1, nlm::ConfigSource::Default);
    config.set("param2", 2, nlm::ConfigSource::File);
    config.set("param3", 3, nlm::ConfigSource::CommandLine);
    
    auto source1 = config.get<nlm::ConfigValue>("param1");
    assert(source1.has_value());
    
    auto source2 = config.get<nlm::ConfigValue>("param2");
    assert(source2.has_value());
    
    auto source3 = config.get<nlm::ConfigValue>("param3");
    assert(source3.has_value());
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

void testConfigHelpText() {
    nlm::Config config;
    
    std::string help = config.getHelpText();
    assert(!help.empty());
    assert(help.find("Configuration Parameters") != std::string::npos);
    assert(help.find("Required parameters") != std::string::npos);
    assert(help.find("Optional parameters") != std::string::npos);
    
    std::cout << "    testConfigHelpText passed" << std::endl;
}

void testConfigEdgeCases() {
    nlm::Config config;
    
    // Test empty key handling (should be ignored)
    config.set("", "value", nlm::ConfigSource::Runtime);
    assert(!config.has(""));
    
    // Test getOr with missing key
    int missingVal = config.getOr<int>("definitely_missing", 999);
    assert(missingVal == 999);
    
    // Test removing non-existent key
    config.remove("non_existent");  // Should not crash
    
    // Test setting same key multiple times
    config.set("test_key", 1, nlm::ConfigSource::Runtime);
    config.set("test_key", 2, nlm::ConfigSource::Runtime);
    
    auto val = config.get<int>("test_key");
    assert(val.has_value());
    assert(*val == 2);  // Should be updated
    
    std::cout << "    testConfigEdgeCases passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigSourceTracking();
    testConfigHelpText();
    testConfigEdgeCases();
}

} // namespace test_config
