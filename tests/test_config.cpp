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

void testValidationClasses() {
    nlm::ConfigValidator validator;
    
    // Test RangeValidator
    auto rangeValidator = std::make_unique<nlm::RangeValidator>(0.0, 100.0);
    validator.addValidator(std::move(rangeValidator));
    
    // Test EnumValidator  
    std::set<std::string> allowed = {"active", "inactive", "pending"};
    auto enumValidator = std::make_unique<nlm::EnumValidator>(allowed);
    validator.addValidator(std::move(enumValidator));
    
    // Test FormatValidator
    auto formatValidator = std::make_unique<nlm::FormatValidator>("ip_address");
    validator.addValidator(std::move(formatValidator));
    
    // Register parameter types
    validator.registerParameter("int_param", nlm::ConfigValueType::Integer);
    validator.registerParameter("string_param", nlm::ConfigValueType::String);
    validator.registerParameter("ip_param", nlm::ConfigValueType::String);
    
    // Test with defaults
    std::map<std::string, std::any> configMap;
    configMap["int_param"] = 42;
    configMap["string_param"] = std::string("test");
    configMap["ip_param"] = std::string("192.168.1.1");
    
    auto fillResult = validator.fillDefaults(configMap);
    assert(fillResult.isValid);
    
    std::cout << "    testValidationClasses passed" << std::endl;
}

void testJsonParsing() {
    nlm::Config config;
    
    // Test valid JSON-like config loading (key=value format)
    assert(config.loadFromFile("/nonexistent/file.txt") == false);
    
    // Test setting values with different sources
    config.set("key1", "value1", nlm::ConfigSource::Default);
    config.set("key2", 42, nlm::ConfigSource::File);
    config.set("key3", true, nlm::ConfigSource::CommandLine);
    config.set("key4", 3.14, nlm::ConfigSource::Runtime);
    
    // Verify values are accessible
    assert(config.get<std::string>("key1").value() == "value1");
    assert(config.get<int>("key2").value() == 42);
    assert(config.get<bool>("key3").value() == true);
    assert(config.get<double>("key4").value() == 3.14);
    
    // Verify sources
    auto keys = config.getKeys();
    assert(keys.size() == 4);
    
    std::cout << "    testJsonParsing passed" << std::endl;
}

void testSchemaValidation() {
    nlm::ConfigValidator validator;
    
    // Setup schema
    validator.registerParameter("age", nlm::ConfigValueType::Integer);
    validator.registerParameter("status", nlm::ConfigValueType::String);
    
    // Add validators
    auto rangeValidator = std::make_unique<nlm::RangeValidator>(0.0, 150.0);
    validator.addValidator(std::move(rangeValidator));
    
    std::set<std::string> allowedStatus = {"active", "inactive", "pending"};
    auto enumValidator = std::make_unique<nlm::EnumValidator>(allowedStatus);
    validator.addValidator(std::move(enumValidator));
    
    // Test valid values
    assert(validator.validate("age", 25).isValid);
    assert(validator.validate("status", std::string("active")).isValid);
    
    // Test invalid values
    auto invalidAgeResult = validator.validate("age", -5);
    assert(!invalidAgeResult.isValid);
    assert(!invalidAgeResult.errors.empty());
    
    auto invalidStatusResult = validator.validate("status", std::string("invalid"));
    assert(!invalidStatusResult.isValid);
    assert(!invalidStatusResult.errors.empty());
    
    std::cout << "    testSchemaValidation passed" << std::endl;
}

void testConfigValueTypes() {
    nlm::Config config;
    
    // Test different value types
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    config.set("string_val", "hello", nlm::ConfigSource::Runtime);
    
    // Test retrieval
    assert(config.get<int>("int_val").value() == 42);
    assert(config.get<double>("double_val").value() == 3.14);
    assert(config.get<bool>("bool_val").value() == true);
    assert(config.get<std::string>("string_val").value() == "hello");
    
    // Test type conversion
    auto intFromDouble = config.getOr<int>("double_val", 0);
    assert(intFromDouble == 3);
    
    std::cout << "    testConfigValueTypes passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testValidationClasses();
    testJsonParsing();
    testSchemaValidation();
    testConfigValueTypes();
}

} // namespace test_config
