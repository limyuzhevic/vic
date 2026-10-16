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

void testJsonParsing() {
    nlm::Config config;
    
    // Test simple JSON parsing
    std::string jsonString = R"({
        "int_value": 42,
        "double_value": 3.14,
        "string_value": "hello",
        "bool_value": true,
        "int_array": [1, 2, 3],
        "double_array": [1.5, 2.5, 3.5],
        "string_array": ["a", "b", "c"]
    })";
    
    assert(config.loadFromJsonString(jsonString));
    
    // Test all value types
    assert(config.get<int>("int_value").has_value());
    assert(config.get<int>("int_value").value() == 42);
    
    assert(config.get<double>("double_value").has_value());
    assert(config.get<double>("double_value").value() > 3.13 && config.get<double>("double_value").value() < 3.15);
    
    assert(config.get<std::string>("string_value").has_value());
    assert(config.get<std::string>("string_value").value() == "hello");
    
    assert(config.get<bool>("bool_value").has_value());
    assert(config.get<bool>("bool_value").value() == true);
    
    std::cout << "    testJsonParsing passed" << std::endl;
}

void testYamlParsing() {
    nlm::Config config;
    
    // Test YAML parsing (for now, test JSON since yaml-cpp is optional)
    std::string yamlString = R"({
        "int_value": 42,
        "double_value": 3.14,
        "string_value": "hello"
    })";
    
    // This will work with our current implementation
    assert(config.loadFromYamlString(yamlString));
    
    assert(config.get<int>("int_value").has_value());
    assert(config.get<int>("int_value").value() == 42);
    
    assert(config.get<double>("double_value").has_value());
    
    assert(config.get<std::string>("string_value").has_value());
    assert(config.get<std::string>("string_value").value() == "hello");
    
    std::cout << "    testYamlParsing passed" << std::endl;
}

void testFileFormatDetection() {
    // Test file format detection
    assert(nlm::Config::getFileFormat("config.json") == "json");
    assert(nlm::Config::getFileFormat("config.JSON") == "json"); // Case insensitive
    assert(nlm::Config::getFileFormat("config.yaml") == "yaml");
    assert(nlm::Config::getFileFormat("config.yml") == "yml");
    assert(nlm::Config::getFileFormat("config.cfg") == "cfg");
    assert(nlm::Config::getFileFormat("config") == ""); // No extension
    
    std::cout << "    testFileFormatDetection passed" << std::endl;
}

void testJsonSerialization() {
    nlm::Config config;
    
    // Set various values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    config.set("double_array", std::vector<double>{1.5, 2.5}, nlm::ConfigSource::Runtime);
    config.set("string_array", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    
    // Test JSON serialization
    std::string jsonString = config.saveToJsonString();
    
    // Parse the JSON string and verify values
    nlm::Config config2;
    assert(config2.loadFromJsonString(jsonString));
    
    assert(config2.get<int>("int_value").value() == 42);
    assert(config2.get<double>("double_value").value() > 3.13);
    assert(config2.get<std::string>("string_value").value() == "hello");
    assert(config2.get<bool>("bool_value").value() == true);
    
    std::cout << "    testJsonSerialization passed" << std::endl;
}

void testFileSaveLoadJson() {
    // Test saving to JSON file and loading from it
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime);
    
    std::string testFile = "/tmp/test_config.json";
    
    assert(config.saveToFile(testFile));
    
    nlm::Config config2;
    assert(config2.loadFromFile(testFile));
    
    assert(config2.get<int>("int_value").value() == 42);
    assert(config2.get<double>("double_value").value() > 3.13);
    assert(config2.get<std::string>("string_value").value() == "hello");
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testFileSaveLoadJson passed" << std::endl;
}

void testFileSaveLoadCfg() {
    // Test saving to .cfg file and loading from it (backward compatibility)
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime);
    
    std::string testFile = "/tmp/test_config.cfg";
    
    assert(config.saveToFile(testFile));
    
    nlm::Config config2;
    assert(config2.loadFromFile(testFile));
    
    assert(config2.get<int>("int_value").value() == 42);
    assert(config2.get<double>("double_value").value() > 3.13);
    assert(config2.get<std::string>("string_value").value() == "hello");
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testFileSaveLoadCfg passed" << std::endl;
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

void runAll() {
    testConfigCreation();
    testJsonParsing();
    testYamlParsing();
    testFileFormatDetection();
    testJsonSerialization();
    testFileSaveLoadJson();
    testFileSaveLoadCfg();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
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

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
}

} // namespace test_config
