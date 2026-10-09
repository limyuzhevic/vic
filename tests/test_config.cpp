// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

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

void testKeyValueFormat() {
    std::string testFile = "test_key_value.cfg";
    
    nlm::Config config;
    
    // Set some values
    config.set("name", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("value", 42, nlm::ConfigSource::Runtime);
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    config.set("tags", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    
    // Save to file
    bool saveResult = config.saveToFile(testFile);
    assert(saveResult);
    
    // Create new config and load from file
    nlm::Config config2;
    bool loadResult = config2.loadFromFile(testFile);
    assert(loadResult);
    
    // Verify values
    auto nameVal = config2.get<std::string>("name");
    assert(nameVal.has_value() && *nameVal == "test");
    
    auto valueVal = config2.get<int>("value");
    assert(valueVal.has_value() && *valueVal == 42);
    
    auto enabledVal = config2.get<bool>("enabled");
    assert(enabledVal.has_value() && *enabledVal == true);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testKeyValueFormat passed" << std::endl;
}

void testJSONFormat() {
    std::string testFile = "test_json.json";
    
    nlm::Config config;
    
    // Set some values
    config.set("name", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("value", 3.14, nlm::ConfigSource::Runtime);
    config.set("active", false, nlm::ConfigSource::Runtime);
    config.set("ids", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    
    // Save as JSON
    bool saveResult = config.saveToFile(testFile);
    assert(saveResult);
    
    // Create new config and load from JSON
    nlm::Config config2;
    bool loadResult = config2.loadFromFile(testFile);
    assert(loadResult);
    
    // Verify values
    auto nameVal = config2.get<std::string>("name");
    assert(nameVal.has_value() && *nameVal == "test");
    
    auto valueVal = config2.get<double>("value");
    assert(valueVal.has_value() && *valueVal > 3.13 && *valueVal < 3.15);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testJSONFormat passed" << std::endl;
}

void testSchemaValidation() {
    nlm::ConfigSchema schema;
    
    // Add schema elements with validation rules
    auto& nameElement = schema.addElement(nlm::ConfigSchemaElement(
        "name", nlm::ConfigSchemaElement::Type::STRING, "User name"
    ));
    nameElement.required = true;
    
    auto& ageElement = schema.addElement(nlm::ConfigSchemaElement(
        "age", nlm::ConfigSchemaElement::Type::INTEGER, "User age"
    ));
    ageElement.required = true;
    
    // Test with valid data
    nlm::Config config1;
    config1.set("name", std::string("Alice"), nlm::ConfigSource::Runtime);
    config1.set("age", 25, nlm::ConfigSource::Runtime);
    
    std::unordered_map<std::string, std::any> validData = {
        {"name", std::any("Alice")},
        {"age", std::any(25)}
    };
    
    assert(schema.validate(validData));
    
    // Test with invalid data (missing field)
    std::unordered_map<std::string, std::any> invalidData = {
        {"name", std::any("Bob")}
    };
    
    assert(!schema.validate(invalidData));
    
    // Test with invalid data (wrong type)
    std::unordered_map<std::string, std::any> invalidTypeData = {
        {"name", std::any(123)},
        {"age", std::any("twenty")}
    };
    
    assert(!schema.validate(invalidTypeData));
    
    std::cout << "    testSchemaValidation passed" << std::endl;
}

void testFileFormatDetection() {
    assert(nlm::detectFileFormat("config.json") == nlm::ConfigFileFormat::JSON);
    assert(nlm::detectFileFormat("config.yaml") == nlm::ConfigFileFormat::YAML);
    assert(nlm::detectFileFormat("config.yml") == nlm::ConfigFileFormat::YAML);
    assert(nlm::detectFileFormat("config.cfg") == nlm::ConfigFileFormat::KEY_VALUE);
    assert(nlm::detectFileFormat("config.ini") == nlm::ConfigFileFormat::KEY_VALUE);
    assert(nlm::detectFileFormat("config.txt") == nlm::ConfigFileFormat::KEY_VALUE);
    assert(nlm::detectFileFormat("config") == nlm::ConfigFileFormat::KEY_VALUE);
    assert(nlm::detectFileFormat("unknown.xyz") == nlm::ConfigFileFormat::UNKNOWN);
    
    std::cout << "    testFileFormatDetection passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testKeyValueFormat();
    testJSONFormat();
    testSchemaValidation();
    testFileFormatDetection();
}

} // namespace test_config
