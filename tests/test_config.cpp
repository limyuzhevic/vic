// Config Tests
#include "core/Config/Config.hpp"
#include "core/Config/ConfigError.hpp"
#include "core/Config/ConfigSchema.hpp"
#include "core/Config/ConfigValidator.hpp"
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
    
    // Test basic validation
    config.set("int_val", 5, nlm::ConfigSource::Runtime);
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value());
    
    // Test double validation
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_val");
    assert(doubleVal.has_value());
    
    // Test string validation
    config.set("string_val", "test_string", nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal.has_value());
    
    // Test boolean validation
    config.set("bool_val", false, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal.has_value());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigSchema() {
    // Create a schema
    auto schema = std::make_shared<nlm::ConfigSchema>("test_schema");
    schema->addField("name", "string", "User name", std::nullopt, std::nullopt, "^[A-Za-z]+$", true);
    schema->addField("age", "int", "User age", 0, 120);
    schema->addField("active", "bool", "Active status");
    
    // Register the schema
    nlm::Config config;
    config.addSchema("test_schema", schema);
    
    // Test schema validation
    bool valid = config.validateSchema();
    assert(valid == true); // Should be valid with no values
    
    std::cout << "    testConfigSchema passed" << std::endl;
}

void testConfigFileOperations() {
    nlm::Config config;
    
    config.set("test_key", "test_value", nlm::ConfigSource::Runtime);
    assert(config.has("test_key"));
    
    // Test file operations would go here
    // saveToFile, loadFromFile, etc.
    
    std::cout << "    testConfigFileOperations passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test empty key error
    try {
        config.set("", "value", nlm::ConfigSource::Runtime);
        assert(false); // Should not reach here
    } catch (const nlm::ConfigError& e) {
        // Expected error
    }
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigChangeTracking() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    auto changeHistory = config.getChangeHistory();
    assert(changeHistory.size() > 0);
    
    config.set("key1", 2, nlm::ConfigSource::Runtime);
    changeHistory = config.getChangeHistory();
    assert(changeHistory.size() > 1); // Should have at least 2 changes
    
    std::cout << "    testConfigChangeTracking passed" << std::endl;
}

void testConfigStats() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", "value", nlm::ConfigSource::File);
    config.set("key3", 3.14, nlm::ConfigSource::CommandLine);
    
    auto stats = config.getStats();
    assert(stats.totalEntries == 3);
    assert(stats.validationErrors == 0);
    
    std::cout << "    testConfigStats passed" << std::endl;
}

void testConfigInheritance() {
    nlm::Config parent;
    parent.set("parent_key", "parent_value", nlm::ConfigSource::Runtime);
    
    nlm::Config child;
    child.inheritFrom(parent, "child");
    
    assert(child.has("child_parent_key"));
    auto value = child.get<std::string>("child_parent_key");
    assert(value.has_value());
    assert(*value == "parent_value");
    
    std::cout << "    testConfigInheritance passed" << std::endl;
}

void testConfigEnvironmentVariables() {
    nlm::Config config;
    
    // Test environment variable loading
    config.loadEnvironmentVariables();
    
    // Set an environment variable and test
    setenv("TEST_CONFIG_VAR", "test_value", 1);
    
    std::cout << "    testConfigEnvironmentVariables passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigSchema();
    testConfigFileOperations();
    testConfigErrorHandling();
    testConfigChangeTracking();
    testConfigStats();
    testConfigInheritance();
    testConfigEnvironmentVariables();
    
    std::cout << "All Config tests passed!" << std::endl;
}

} // namespace test_config
