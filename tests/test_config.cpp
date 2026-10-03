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
    
    // Test valid values
    config.set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Runtime);
    assert(config.isValid("random_seed"));
    
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    assert(config.isValid("simulation_timestep"));
    
    config.set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Runtime);
    assert(config.isValid("neuron_count"));
    
    config.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    assert(config.isValid("connection_probability"));
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigValidationErrors() {
    nlm::Config config;
    
    // Set invalid values
    config.set("random_seed", static_cast<int64_t>(-1), nlm::ConfigSource::Runtime);
    assert(!config.isValid("random_seed"));
    
    config.set("simulation_timestep", 2.0, nlm::ConfigSource::Runtime);
    assert(!config.isValid("simulation_timestep"));
    
    config.set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Runtime);
    assert(!config.isValid("neuron_count"));
    
    std::cout << "    testConfigValidationErrors passed" << std::endl;
}

void testConfigSchemaRegistration() {
    nlm::Config config;
    
    // Config should have schema registered
    config.registerKey("custom_key", "Custom configuration key", false,
                     std::make_optional<nlm::ConfigValue>("default_value"),
                     [](const nlm::ConfigValue& v) {
                         if (std::holds_alternative<std::string>(v)) {
                             return nlm::ValidationResult::success();
                         }
                         return nlm::ValidationResult::error("Must be a string");
                     });
    
    // Set valid value
    config.set("custom_key", std::string("test_value"), nlm::ConfigSource::Runtime);
    assert(config.isValid("custom_key"));
    
    // Try to set invalid value
    config.set("custom_key", 123, nlm::ConfigSource::Runtime);
    assert(!config.isValid("custom_key"));
    
    std::cout << "    testConfigSchemaRegistration passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", "value", nlm::ConfigSource::CommandLine);
    
    std::string summary = config.summary();
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigValidationErrorsList() {
    nlm::Config config;
    
    // Create invalid config
    config.set("random_seed", static_cast<int64_t>(-1), nlm::ConfigSource::Runtime);
    
    auto errors = config.getValidationErrors();
    assert(!errors.empty());
    assert(errors[0].find("random_seed") != std::string::npos);
    
    std::cout << "    testConfigValidationErrorsList passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigValidationErrors();
    testConfigSchemaRegistration();
    testConfigSummary();
    testConfigValidationErrorsList();
}

} // namespace test_config
