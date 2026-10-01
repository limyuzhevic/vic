// Config Tests
#include "core/Config/ConfigManager.hpp"
#include "core/Exceptions/Exceptions.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_config {

void testConfigCreation() {
    nlm::ConfigManager config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::ConfigManager config;
    
    config.setInt("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.setDouble("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.setString("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.setBool("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::ConfigManager config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.setInt("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::ConfigManager config;
    
    config.setInt("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::ConfigManager config;
    
    config.setInt("key1", 1, nlm::ConfigSource::Runtime);
    config.setInt("key2", 2, nlm::ConfigSource::Runtime);
    config.setInt("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigLoadFromKeyValue() {
    nlm::ConfigManager config;
    
    // Create a temporary test config file
    std::string testFile = "/tmp/test_config.cfg";
    std::ofstream file(testFile);
    file << "# Test configuration file\n";
    file << "name = test_value\n";
    file << "number = 42\n";
    file << "enabled = true\n";
    file.close();
    
    // Load the file
    assert(config.loadFromKeyValue(testFile));
    
    // Verify values
    assert(config.has("name"));
    assert(config.get<std::string>("name").has_value());
    assert(*config.get<std::string>("name") == "test_value");
    
    assert(config.has("number"));
    assert(config.get<int>("number").has_value());
    assert(*config.get<int>("number") == 42);
    
    assert(config.has("enabled"));
    assert(config.get<bool>("enabled").has_value());
    assert(*config.get<bool>("enabled") == true);
    
    // Clean up
    std::filesystem::remove(testFile);
    
    std::cout << "    testConfigLoadFromKeyValue passed" << std::endl;
}

void testConfigSchemaValidation() {
    nlm::ConfigManager config;
    
    // Create a schema
    nlm::ConfigSchema schema;
    schema.addField("required_field", nlm::ConfigSource::Default, "Required field", true);
    schema.addField("optional_field", nlm::ConfigSource::Default, "Optional field", false);
    schema.addField("pattern_field", nlm::ConfigSource::Default, "Field with pattern", false, "^[a-zA-Z0-9_]+$");
    
    config.setSchema(schema);
    
    // Set required field (should pass validation)
    config.setString("required_field", "test123", nlm::ConfigSource::Runtime);
    assert(config.validate());
    
    // Set optional field (should pass validation)
    config.setString("optional_field", "optional", nlm::ConfigSource::Runtime);
    assert(config.validate());
    
    // Set pattern field with valid value (should pass)
    config.setString("pattern_field", "valid_123", nlm::ConfigSource::Runtime);
    assert(config.validate());
    
    // Set pattern field with invalid value (should fail)
    config.setString("pattern_field", "invalid-value!", nlm::ConfigSource::Runtime);
    assert(!config.validate());
    auto errors = config.getValidationErrors();
    assert(!errors.empty());
    
    std::cout << "    testConfigSchemaValidation passed" << std::endl;
}

void testConfigSaveToFile() {
    nlm::ConfigManager config;
    
    // Set some test values
    config.setString("name", "test_config");
    config.setInt("version", 1);
    config.setDouble("ratio", 3.14159);
    config.setBool("enabled", true);
    
    // Create a temporary file
    std::string testFile = "/tmp/config_save_test.cfg";
    
    // Save to file
    assert(config.saveToFile(testFile));
    
    // Reload from file
    nlm::ConfigManager config2;
    assert(config2.loadFromKeyValue(testFile));
    
    // Verify values
    assert(config2.get<std::string>("name").has_value());
    assert(*config2.get<std::string>("name") == "test_config");
    
    assert(config2.get<int>("version").has_value());
    assert(*config2.get<int>("version") == 1);
    
    assert(config2.get<double>("ratio").has_value());
    assert(*config2.get<double>("ratio") > 3.14 && *config2.get<double>("ratio") < 3.15);
    
    assert(config2.get<bool>("enabled").has_value());
    assert(*config2.get<bool>("enabled") == true);
    
    // Clean up
    std::filesystem::remove(testFile);
    
    std::cout << "    testConfigSaveToFile passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigLoadFromKeyValue();
    testConfigSchemaValidation();
    testConfigSaveToFile();
}

} // namespace test_config
