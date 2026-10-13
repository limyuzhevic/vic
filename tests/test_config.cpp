// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cstring> // for strcmp

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

void testJsonLoading() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::string jsonContent = R"({"string": "value", "number": 42, "float": 3.14, "boolean": true, "int64": 9223372036854775807, "array_int": [1, 2, 3], "array_double": [1.5, 2.5, 3.5], "array_string": ["a", "b", "c"]})";
    
    // Write to a temporary file
    std::string tempFile = "/tmp/test_config.json";
    std::ofstream file(tempFile);
    file << jsonContent;
    file.close();
    
    // Load JSON
    bool result = config.loadFromFile(tempFile);
    assert(result);
    
    // Verify loaded values
    assert(config.has("string"));
    auto stringVal = config.get<std::string>("string");
    assert(stringVal.has_value() && *stringVal == "value");
    
    assert(config.has("number"));
    auto intVal = config.get<int>("number");
    assert(intVal.has_value() && *intVal == 42);
    
    assert(config.has("float"));
    auto doubleVal = config.get<double>("float");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    assert(config.has("boolean"));
    auto boolVal = config.get<bool>("boolean");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test vectors
    assert(config.has("array_int"));
    auto arrayVal = config.get<std::vector<int>>("array_int");
    assert(arrayVal.has_value());
    assert(arrayVal.value().size() == 3);
    
    std::cout << "    testJsonLoading passed" << std::endl;
    
    // Clean up
    std::remove(tempFile.c_str());
}

void testYamlLoading() {
    nlm::Config config;
    
    // Create a temporary YAML file
    std::string yamlContent = R"("string": value
"number": 42
"float": 3.14
"boolean": true
"int64": 9223372036854775807
"array_int": [1, 2, 3]
"array_double": [1.5, 2.5, 3.5]
"array_string": ["a", "b", "c"])";
    
    // Write to a temporary file
    std::string tempFile = "/tmp/test_config.yaml";
    std::ofstream file(tempFile);
    file << yamlContent;
    file.close();
    
    // Load YAML
    bool result = config.loadFromFile(tempFile);
    assert(result);
    
    // Verify loaded values
    assert(config.has("string"));
    auto stringVal = config.get<std::string>("string");
    assert(stringVal.has_value() && *stringVal == "value");
    
    assert(config.has("number"));
    auto intVal = config.get<int>("number");
    assert(intVal.has_value() && *intVal == 42);
    
    assert(config.has("float"));
    auto doubleVal = config.get<double>("float");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    assert(config.has("boolean"));
    auto boolVal = config.get<bool>("boolean");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test vectors
    assert(config.has("array_int"));
    auto arrayVal = config.get<std::vector<int>>("array_int");
    assert(arrayVal.has_value());
    assert(arrayVal.value().size() == 3);
    
    std::cout << "    testYamlLoading passed" << std::endl;
    
    // Clean up
    std::remove(tempFile.c_str());
}

void testNestedObjects() {
    nlm::Config config;
    
    // Create a temporary JSON file with nested objects
    std::string jsonContent = R"({"section": {"key": "value", "number": 42}, "subsection": {"nested": {"deep": "value"}}})";
    
    // Write to a temporary file
    std::string tempFile = "/tmp/test_nested.json";
    std::ofstream file(tempFile);
    file << jsonContent;
    file.close();
    
    // Load JSON
    bool result = config.loadFromFile(tempFile);
    assert(result);
    
    // Verify flattened keys
    assert(config.has("section.key"));
    auto keyVal = config.get<std::string>("section.key");
    assert(keyVal.has_value() && *keyVal == "value");
    
    assert(config.has("section.number"));
    auto numberVal = config.get<int>("section.number");
    assert(numberVal.has_value() && *numberVal == 42);
    
    assert(config.has("subsection.nested.deep"));
    auto deepVal = config.get<std::string>("subsection.nested.deep");
    assert(deepVal.has_value() && *deepVal == "value");
    
    std::cout << "    testNestedObjects passed" << std::endl;
    
    // Clean up
    std::remove(tempFile.c_str());
}

void testJsonSaving() {
    nlm::Config config;
    
    // Set various values
    config.set("string_key", "test_value", nlm::ConfigSource::Runtime);
    config.set("int_key", 123, nlm::ConfigSource::Runtime);
    config.set("double_key", 45.67, nlm::ConfigSource::Runtime);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    config.set("array_key", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    std::string tempFile = "/tmp/test_save.json";
    bool result = config.saveToFile(tempFile);
    assert(result);
    
    // Reload from saved file
    nlm::Config config2;
    result = config2.loadFromFile(tempFile);
    assert(result);
    
    // Verify values match
    assert(config2.has("string_key"));
    auto stringVal = config2.get<std::string>("string_key");
    assert(stringVal.has_value() && *stringVal == "test_value");
    
    assert(config2.has("int_key"));
    auto intVal = config2.get<int>("int_key");
    assert(intVal.has_value() && *intVal == 123);
    
    assert(config2.has("double_key"));
    auto doubleVal = config2.get<double>("double_key");
    assert(doubleVal.has_value());
    assert(*doubleVal > 45.66 && *doubleVal < 45.68);
    
    assert(config2.has("bool_key"));
    auto boolVal = config2.get<bool>("bool_key");
    assert(boolVal.has_value() && *boolVal == true);
    
    assert(config2.has("array_key"));
    auto arrayVal = config2.get<std::vector<int>>("array_key");
    assert(arrayVal.has_value());
    assert(arrayVal.value().size() == 3);
    
    std::cout << "    testJsonSaving passed" << std::endl;
    
    // Clean up
    std::remove(tempFile.c_str());
}

void testYamlSaving() {
    nlm::Config config;
    
    // Set various values
    config.set("string_key", "test_value", nlm::ConfigSource::Runtime);
    config.set("int_key", 123, nlm::ConfigSource::Runtime);
    config.set("double_key", 45.67, nlm::ConfigSource::Runtime);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    config.set("array_key", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    
    // Save to YAML file
    std::string tempFile = "/tmp/test_save.yaml";
    bool result = config.saveToFile(tempFile);
    assert(result);
    
    // Reload from saved file
    nlm::Config config2;
    result = config2.loadFromFile(tempFile);
    assert(result);
    
    // Verify values match
    assert(config2.has("string_key"));
    auto stringVal = config2.get<std::string>("string_key");
    assert(stringVal.has_value() && *stringVal == "test_value");
    
    assert(config2.has("int_key"));
    auto intVal = config2.get<int>("int_key");
    assert(intVal.has_value() && *intVal == 123);
    
    assert(config2.has("double_key"));
    auto doubleVal = config2.get<double>("double_key");
    assert(doubleVal.has_value());
    assert(*doubleVal > 45.66 && *doubleVal < 45.68);
    
    assert(config2.has("bool_key"));
    auto boolVal = config2.get<bool>("bool_key");
    assert(boolVal.has_value() && *boolVal == true);
    
    assert(config2.has("array_key"));
    auto arrayVal = config2.get<std::vector<int>>("array_key");
    assert(arrayVal.has_value());
    assert(arrayVal.value().size() == 3);
    
    std::cout << "    testYamlSaving passed" << std::endl;
    
    // Clean up
    std::remove(tempFile.c_str());
}

void testPlainTextBackwardCompatibility() {
    nlm::Config config;
    
    // Create a temporary plain text file
    std::string tempFile = "/tmp/test_plain.txt";
    std::ofstream file(tempFile);
    file << "key1 = value1\n";
    file << "key2 = 123\n";
    file << "key3 = 3.14\n";
    file << "key4 = true\n";
    file.close();
    
    // Load plain text file
    bool result = config.loadFromFile(tempFile);
    assert(result);
    
    // Verify values
    assert(config.has("key1"));
    auto stringVal = config.get<std::string>("key1");
    assert(stringVal.has_value() && *stringVal == "value1");
    
    assert(config.has("key2"));
    auto intVal = config.get<int>("key2");
    assert(intVal.has_value() && *intVal == 123);
    
    assert(config.has("key3"));
    auto doubleVal = config.get<double>("key3");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    assert(config.has("key4"));
    auto boolVal = config.get<bool>("key4");
    assert(boolVal.has_value() && *boolVal == true);
    
    std::cout << "    testPlainTextBackwardCompatibility passed" << std::endl;
    
    // Clean up
    std::remove(tempFile.c_str());
}

void testFileFormatAutoDetection() {
    nlm::Config config;
    
    // Test .json extension
    std::string jsonContent = R"({"key": "value"})";
    std::string tempJson = "/tmp/test_auto.json";
    std::ofstream jsonFile(tempJson);
    jsonFile << jsonContent;
    jsonFile.close();
    
    bool result = config.loadFromFile(tempJson);
    assert(result);
    assert(config.has("key"));
    std::remove(tempJson.c_str());
    
    // Test .yaml extension
    std::string yamlContent = R"(key: value)";
    std::string tempYaml = "/tmp/test_auto.yaml";
    std::ofstream yamlFile(tempYaml);
    yamlFile << yamlContent;
    yamlFile.close();
    
    result = config.loadFromFile(tempYaml);
    assert(result);
    assert(config.has("key"));
    std::remove(tempYaml.c_str());
    
    // Test .yml extension
    std::string tempYml = "/tmp/test_auto.yml";
    std::ofstream ymlFile(tempYml);
    ymlFile << yamlContent;
    ymlFile.close();
    
    result = config.loadFromFile(tempYml);
    assert(result);
    assert(config.has("key"));
    std::remove(tempYml.c_str());
    
    // Test no extension (plain text)
    std::string tempPlain = "/tmp/test_auto";
    std::ofstream plainFile(tempPlain);
    plainFile << "key = value";
    plainFile.close();
    
    result = config.loadFromFile(tempPlain);
    assert(result);
    assert(config.has("key"));
    std::remove(tempPlain.c_str());
    
    std::cout << "    testFileFormatAutoDetection passed" << std::endl;
}

void testMalformedFileHandling() {
    nlm::Config config;
    
    // Test malformed JSON
    std::string tempJson = "/tmp/test_bad.json";
    std::ofstream jsonFile(tempJson);
    jsonFile << R"({"key": "value""); // Missing closing brace
    jsonFile.close();
    
    bool result = config.loadFromFile(tempJson);
    assert(!result); // Should fail to load
    std::remove(tempJson.c_str());
    
    // Test malformed YAML
    std::string tempYaml = "/tmp/test_bad.yaml";
    std::ofstream yamlFile(tempYaml);
    yamlFile << "key: value:"; // Invalid YAML
    yamlFile.close();
    
    result = config.loadFromFile(tempYaml);
    assert(!result); // Should fail to load
    std::remove(tempYaml.c_str());
    
    std::cout << "    testMalformedFileHandling passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testJsonLoading();
    testYamlLoading();
    testNestedObjects();
    testJsonSaving();
    testYamlSaving();
    testPlainTextBackwardCompatibility();
    testFileFormatAutoDetection();
    testMalformedFileHandling();
}

} // namespace test_config
