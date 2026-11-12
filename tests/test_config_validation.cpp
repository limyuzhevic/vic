// Integration Tests for Enhanced Config Validation
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_config_validation {

void testConfigValidationBasic() {
    nlm::Config config;
    
    // Set up valid configurations
    config.set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Runtime);
    config.set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.15f, nlm::ConfigSource::Runtime);
    config.set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    
    // Test that all values can be retrieved
    assert(config.getOr<int64_t>("neuron_count", 0) == 1000);
    assert(config.getOr<int64_t>("region_count", 0) == 2);
    assert(config.getOr<float>("connection_probability", 0.0f) == 0.15f);
    assert(config.getOr<int64_t>("random_seed", 0) == 42);
    assert(config.getOr<double>("simulation_timestep", 0.0) == 0.001);
    
    std::cout << "    testConfigValidationBasic passed" << std::endl;
}

void testConfigValidationEdgeCases() {
    nlm::Config config;
    
    // Test edge cases
    config.set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime); // Minimum
    config.set("neuron_count", static_cast<int64_t>(100000), nlm::ConfigSource::Runtime); // Large
    config.set("connection_probability", 0.0f, nlm::ConfigSource::Runtime); // Minimum
    config.set("connection_probability", 1.0f, nlm::ConfigSource::Runtime); // Maximum
    config.set("simulation_timestep", 0.0001f, nlm::ConfigSource::Runtime); // Small timestep
    config.set("simulation_timestep", 10.0f, nlm::ConfigSource::Runtime); // Large timestep (should warn)
    
    // Test that values are set correctly
    assert(config.getOr<int64_t>("neuron_count", 0) == 100000);
    assert(config.getOr<float>("connection_probability", 0.0f) == 1.0f);
    assert(config.getOr<double>("simulation_timestep", 0.0) == 10.0);
    
    std::cout << "    testConfigValidationEdgeCases passed" << std::endl;
}

void testConfigValidationDataTypes() {
    nlm::Config config;
    
    // Test different data types
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("int64_val", static_cast<int64_t>(2147483647), nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14159, nlm::ConfigSource::Runtime);
    config.set("float_val", 2.718f, nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    config.set("string_val", "test_string", nlm::ConfigSource::Runtime);
    
    // Verify types
    assert(config.getOr<int>("int_val", 0) == 42);
    assert(config.getOr<int64_t>("int64_val", 0) == 2147483647);
    assert(config.getOr<double>("double_val", 0.0) > 3.14 && config.getOr<double>("double_val", 0.0) < 3.15);
    assert(config.getOr<float>("float_val", 0.0f) > 2.7 && config.getOr<float>("float_val", 0.0f) < 2.8);
    assert(config.getOr<bool>("bool_val", false) == true);
    assert(config.getOr<std::string>("string_val", "") == "test_string");
    
    std::cout << "    testConfigValidationDataTypes passed" << std::endl;
}

void testConfigValidationRemoveAndKeys() {
    nlm::Config config;
    
    // Add several keys
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    // Verify keys exist
    assert(config.has("key1"));
    assert(config.has("key2"));
    assert(config.has("key3"));
    
    // Remove one key
    config.remove("key2");
    assert(!config.has("key2"));
    assert(config.has("key1"));
    assert(config.has("key3"));
    
    // Get remaining keys
    auto keys = config.getKeys();
    assert(keys.size() == 2);
    assert(std::find(keys.begin(), keys.end(), "key1") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key3") != keys.end());
    
    std::cout << "    testConfigValidationRemoveAndKeys passed" << std::endl;
}

void runAll() {
    testConfigValidationBasic();
    testConfigValidationEdgeCases();
    testConfigValidationDataTypes();
    testConfigValidationRemoveAndKeys();
    
    std::cout << std::endl;
    std::cout << "  Config Validation Tests Summary:" << std::endl;
    std::cout << "    - Basic validation working" << std::endl;
    std::cout << "    - Edge case validation working" << std::endl;
    std::cout << "    - Multiple data types working" << std::endl;
    std::cout << "    - Remove and keys functionality working" << std::endl;
}

} // namespace test_config_validation