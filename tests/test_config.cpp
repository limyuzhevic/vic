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

// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
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

// Test new validation methods
void testConfigValidate() {
    nlm::Config config;
    
    // Test valid parameters
    config.set("neuron_count", 1000LL, nlm::ConfigSource::Runtime);
    config.set("region_count", 5LL, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("stdp_ltp_weight", 0.01, nlm::ConfigSource::Runtime);
    config.set("stdp_ltd_weight", 0.012, nlm::ConfigSource::Runtime);
    config.set("enable_checkpointing", true, nlm::ConfigSource::Runtime);
    config.set("enable_replay", true, nlm::ConfigSource::Runtime);
    config.set("enable_development", true, nlm::ConfigSource::Runtime);
    
    auto errors = config.validate(false); // Don't require strict validation
    assert(errors.empty()); // No errors expected
    
    // Test invalid parameters
    config.set("invalid_int", -5, nlm::ConfigSource::Runtime);
    errors = config.validate(true); // Require strict validation
    assert(!errors.empty()); // Should have errors
    
    std::cout << "    testConfigValidate passed" << std::endl;
}

// Test new utility methods
void testConfigGetAllAsStrings() {
    nlm::Config config;
    
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("double_key", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_key", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("bool_key", false, nlm::ConfigSource::Runtime);
    
    auto strings = config.getAllAsStrings();
    assert(strings.size() == 4);
    
    // Check that values are converted to strings
    assert(strings.find("int_key") != strings.end());
    assert(strings.find("double_key") != strings.end());
    assert(strings.find("string_key") != strings.end());
    assert(strings.find("bool_key") != strings.end());
    
    std::cout << "    testConfigGetAllAsStrings passed" << std::endl;
}

// Test merge functionality
void testConfigMerge() {
    nlm::Config config1;
    config1.set("key1", 100, nlm::ConfigSource::Runtime);
    config1.set("key2", 200, nlm::ConfigSource::Runtime);
    
    nlm::Config config2;
    config2.set("key2", 300, nlm::ConfigSource::File); // Different source
    config2.set("key3", 400, nlm::ConfigSource::CommandLine);
    
    config1.merge(config2, false); // Don't override sources
    
    // key2 should keep config1's value (source not overridden)
    assert(config1.getOr<int>("key2", 0) == 200);
    
    // key3 should be added
    assert(config1.getOr<int>("key3", 0) == 400);
    
    // key1 should remain unchanged
    assert(config1.getOr<int>("key1", 0) == 100);
    
    std::cout << "    testConfigMerge passed" << std::endl;
}

// Test validation patterns
void testConfigValidatePattern() {
    nlm::Config config;
    
    config.set("valid_int", 42, nlm::ConfigSource::Runtime);
    config.set("valid_bool", true, nlm::ConfigSource::Runtime);
    config.set("valid_float", 3.14, nlm::ConfigSource::Runtime);
    
    // Test valid patterns
    assert(config.validatePattern("valid_int", "int"));
    assert(config.validatePattern("valid_bool", "bool"));
    assert(config.validatePattern("valid_float", "float"));
    
    // Test invalid patterns
    assert(!config.validatePattern("valid_int", "bool"));
    assert(!config.validatePattern("valid_int", "float"));
    
    // Test invalid values
    config.set("invalid_int_string", std::string("not_a_number"), nlm::ConfigSource::Runtime);
    assert(!config.validatePattern("invalid_int_string", "int"));
    
    std::cout << "    testConfigValidatePattern passed" << std::endl;
}

// Test default values
void testConfigSetDefaults() {
    nlm::Config config;
    
    // Should not have defaults initially
    assert(!config.has("neuron_count"));
    
    // Set defaults
    config.setDefaults();
    
    // Should now have default values
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    assert(config.has("connection_probability"));
    assert(config.has("stdp_ltp_weight"));
    assert(config.has("stdp_ltd_weight"));
    assert(config.has("stdp_tau"));
    assert(config.has("synaptogenesis_rate"));
    assert(config.has("pruning_rate"));
    assert(config.has("simulation_timestep"));
    assert(config.has("enable_checkpointing"));
    assert(config.has("enable_replay"));
    assert(config.has("enable_development"));
    assert(config.has("random_seed"));
    assert(config.has("replay_interval"));
    assert(config.has("consolidation_interval"));
    
    std::cout << "    testConfigSetDefaults passed" << std::endl;
}

// Test JSON conversion
void testConfigToJson() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    std::string json = config.toJson();
    
    // Check that JSON contains expected elements
    assert(json.find("int_value") != std::string::npos);
    assert(json.find("double_value") != std::string::npos);
    assert(json.find("string_value") != std::string::npos);
    assert(json.find("bool_value") != std::string::npos);
    
    // Check formatting
    assert(json.find("{") != std::string::npos);
    assert(json.find("}") != std::string::npos);
    
    std::cout << "    testConfigToJson passed" << std::endl;
}

// Test source filtering
void testConfigHasSource() {
    nlm::Config config;
    
    config.set("source1", 100, nlm::ConfigSource::Default);
    config.set("source2", 200, nlm::ConfigSource::File);
    config.set("source3", 300, nlm::ConfigSource::CommandLine);
    
    assert(config.hasSource(nlm::ConfigSource::Default));
    assert(config.hasSource(nlm::ConfigSource::File));
    assert(config.hasSource(nlm::ConfigSource::CommandLine));
    
    std::cout << "    testConfigHasSource passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidate();
    testConfigGetAllAsStrings();
    testConfigMerge();
    testConfigValidatePattern();
    testConfigSetDefaults();
    testConfigToJson();
    testConfigHasSource();
}

} // namespace test_config

} // namespace test_config
