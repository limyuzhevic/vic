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

void testConfigValidation() {
    nlm::Config config;
    
    // Test valid configuration
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.5, nlm::ConfigSource::Runtime);
    config.set("log_level", std::string("info"), nlm::ConfigSource::Runtime);
    config.set("log_to_file", true, nlm::ConfigSource::Runtime);
    
    assert(config.validateConfiguration());
    assert(config.getLastError().empty());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigKeyValidation() {
    nlm::Config config;
    
    // Test empty key
    config.set("", "value", nlm::ConfigSource::Runtime);
    assert(!config.validateKey(""));
    assert(!config.getLastError().empty());
    
    // Test too long key
    std::string longKey = "very_long_key_name_that_exceeds_maximum_length_limit_for_testing";
    assert(!config.validateKey(longKey));
    assert(!config.getLastError().empty());
    
    // Test key with invalid characters
    assert(!config.validateKey("invalid@key"));
    assert(!config.getLastError().empty());
    
    // Test valid key
    assert(config.validateKey("valid_key"));
    assert(config.getLastError().empty());
    
    std::cout << "    testConfigKeyValidation passed" << std::endl;
}

void testConfigValueValidation() {
    nlm::Config config;
    
    // Test neuron_count validation
    config.set("neuron_count", -10, nlm::ConfigSource::Runtime);
    assert(!config.validateConfiguration());
    assert(!config.getLastError().empty());
    
    config.remove("neuron_count");
    config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
    assert(config.validateConfiguration());
    
    // Test connection_probability validation
    config.set("connection_probability", 1.5, nlm::ConfigSource::Runtime);
    assert(!config.validateConfiguration());
    assert(!config.getLastError().empty());
    
    config.remove("connection_probability");
    config.set("connection_probability", 0.3, nlm::ConfigSource::Runtime);
    assert(config.validateConfiguration());
    
    // Test log_level validation
    config.set("log_level", "invalid_level", nlm::ConfigSource::Runtime);
    assert(!config.validateConfiguration());
    assert(!config.getLastError().empty());
    
    config.remove("log_level");
    config.set("log_level", "debug", nlm::ConfigSource::Runtime);
    assert(config.validateConfiguration());
    
    std::cout << "    testConfigValueValidation passed" << std::endl;
}

void testAdvancedConfigOptions() {
    nlm::Config config;
    
    // Test advanced boolean options
    config.set("advanced.enable_structural_plasticity", true, nlm::ConfigSource::Runtime);
    config.set("advanced.enable_reward_modulation", true, nlm::ConfigSource::Runtime);
    config.set("advanced.enable_development", true, nlm::ConfigSource::Runtime);
    
    assert(config.validateConfiguration());
    
    // Test optimization level
    config.set("advanced.optimization_level", 2, nlm::ConfigSource::Runtime);
    assert(config.validateConfiguration());
    
    // Test parallel processing
    config.set("advanced.parallel_processing", true, nlm::ConfigSource::Runtime);
    assert(config.validateConfiguration());
    
    // Test invalid advanced option
    config.set("advanced.invalid_option", 123, nlm::ConfigSource::Runtime);
    assert(!config.validateConfiguration());
    assert(!config.getLastError().empty());
    
    std::cout << "    testAdvancedConfigOptions passed" << std::endl;
}

void testConfigHelpAndVersion() {
    nlm::Config config;
    
    // Test that help and version methods exist and can be called
    // (These methods don't return error codes, so we just call them)
    config.printHelp();
    config.printVersion();
    
    } // namespace test_config

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigKeyValidation();
    testConfigValueValidation();
    testAdvancedConfigOptions();
    testConfigHelpAndVersion();
}
