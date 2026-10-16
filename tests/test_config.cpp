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

void testConfigValidationRange() {
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    // Add range validation
    schema.addRangeRule("temperature", 0.0, 100.0, nlm::ValidationSeverity::Error, "Temperature must be between 0 and 100");
    
    // Test valid values
    config.set("temperature", 50.0, nlm::ConfigSource::Runtime);
    assert(config.validate(schema));
    assert(config.isValid(schema));
    
    // Test invalid values
    config.set("temperature", 150.0, nlm::ConfigSource::Runtime);
    assert(!config.validate(schema));
    assert(!config.isValid(schema));
    
    auto errors = config.getValidationErrors(schema);
    assert(!errors.empty());
    assert(errors[0].severity == nlm::ValidationSeverity::Error);
    
    std::cout << "    testConfigValidationRange passed" << std::endl;
}

void testConfigValidationEnum() {
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    // Add enum validation
    schema.addEnumRule("status", {"active", "inactive", "pending"}, nlm::ValidationSeverity::Error, "Status must be one of active, inactive, or pending");
    
    // Test valid values
    config.set("status", std::string("active"), nlm::ConfigSource::Runtime);
    assert(config.validate(schema));
    
    // Test invalid values
    config.set("status", std::string("unknown"), nlm::ConfigSource::Runtime);
    assert(!config.validate(schema));
    
    auto errors = config.getValidationErrors(schema);
    assert(!errors.empty());
    
    std::cout << "    testConfigValidationEnum passed" << std::endl;
}

void testConfigValidationRequired() {
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    // Add required validation
    schema.addCustomRule("name", [](const nlm::ConfigValue& val, const std::string& key, const nlm::Config& cfg) {
        auto nameVal = cfg.get<std::string>(key);
        return nameVal.has_value() && !nameVal.value().empty();
    }, nlm::ValidationSeverity::Error, "Name is required and must not be empty");
    
    // Test missing field
    assert(!config.validate(schema));
    
    // Test empty field
    config.set("name", std::string(""), nlm::ConfigSource::Runtime);
    assert(!config.validate(schema));
    
    // Test valid field
    config.set("name", std::string("John Doe"), nlm::ConfigSource::Runtime);
    assert(config.validate(schema));
    
    std::cout << "    testConfigValidationRequired passed" << std::endl;
}

void testConfigValidationCache() {
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    schema.addRangeRule("value", 0, 100);
    
    // Set a value and validate
    config.set("value", 50, nlm::ConfigSource::Runtime);
    assert(config.validate(schema));
    
    // Clear validation cache and set a new value
    config.clearValidationCache();
    config.set("value", 150, nlm::ConfigSource::Runtime);
    assert(!config.validate(schema));
    
    std::cout << "    testConfigValidationCache passed" << std::endl;
}

void testConfigValidationModes() {
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    // Add a rule that would be a warning
    schema.addRangeRule("value", 0, 100, nlm::ValidationSeverity::Warning, "Value out of recommended range");
    
    config.set("value", 150, nlm::ConfigSource::Runtime);
    
    // Test strict mode (should be invalid due to error)
    config.setValidationMode(nlm::Config::ValidationMode::Strict);
    assert(!config.validate(schema));
    
    // Test lenient mode (only critical errors)
    config.setValidationMode(nlm::Config::ValidationMode::Lenient);
    assert(config.validate(schema)); // Warning is not critical
    
    // Test warnings mode (only warnings)
    config.setValidationMode(nlm::Config::ValidationMode::Warnings);
    assert(config.validate(schema)); // This should pass since we only check warnings
    
    std::cout << "    testConfigValidationModes passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidationRange();
    testConfigValidationEnum();
    testConfigValidationRequired();
    testConfigValidationCache();
    testConfigValidationModes();
}

} // namespace test_config
