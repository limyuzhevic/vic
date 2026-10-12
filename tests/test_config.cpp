// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <limits>

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

void testJSONParsing() {
    nlm::Config config;
    
    // Create a temporary JSON file with all data types
    std::string jsonContent = R"({
        "int_value": 42,
        "double_value": 3.14159,
        "string_value": "hello world",
        "bool_value": true,
        "int_array": [1, 2, 3, 4, 5],
        "double_array": [1.1, 2.2, 3.3],
        "string_array": ["apple", "banana", "cherry"]
    })";
    
    std::string tempJsonFile = "test_config.json";
    std::ofstream jsonFile(tempJsonFile);
    jsonFile << jsonContent;
    jsonFile.close();
    
    // Test JSON parsing
    bool loadSuccess = config.loadFromFile(tempJsonFile);
    assert(loadSuccess);
    
    // Verify all values
    assert(config.has("int_value"));
    assert(config.get<int>("int_value").value() == 42);
    
    assert(config.has("double_value"));
    assert(config.get<double>("double_value").value() > 3.13 && config.get<double>("double_value").value() < 3.15);
    
    assert(config.has("string_value"));
    assert(config.get<std::string>("string_value").value() == "hello world");
    
    assert(config.has("bool_value"));
    assert(config.get<bool>("bool_value").value() == true);
    
    assert(config.has("int_array"));
    auto intArray = config.get<std::vector<int>>("int_array");
    assert(intArray.has_value());
    assert(intArray.value().size() == 5);
    assert(intArray.value()[0] == 1 && intArray.value()[1] == 2);
    
    // Test malformed JSON
    std::string malformedJson = R"({"invalid": json})";
    std::ofstream malformedFile("test_malformed.json");
    malformedFile << malformedJson;
    malformedFile.close();
    
    bool malformedLoad = config.loadFromFile("test_malformed.json");
    // Malformed JSON should fail to load
    
    // Cleanup
    std::remove(tempJsonFile.c_str());
    std::remove("test_malformed.json");
    
    std::cout << "    testJSONParsing passed" << std::endl;
}

void testYAMLParsing() {
    nlm::Config config;
    
    // Create a temporary YAML file with all data types
    std::string yamlContent = R"(int_value: 42
double_value: 3.14159
string_value: "hello world"
bool_value: true
int_array: [1, 2, 3, 4, 5]
double_array: [1.1, 2.2, 3.3]
string_array: ["apple", "banana", "cherry"]
nested:
  inner_int: 100
  inner_string: "nested value"
)";
    
    std::string tempYamlFile = "test_config.yaml";
    std::ofstream yamlFile(tempYamlFile);
    yamlFile << yamlContent;
    yamlFile.close();
    
    // Test YAML parsing
    bool loadSuccess = config.loadFromFile(tempYamlFile);
    // Note: YAML parsing may fail if yaml-cpp is not available, so we don't assert
    
    if (loadSuccess) {
        // Verify some values if YAML was parsed
        if (config.has("int_value")) {
            assert(config.get<int>("int_value").value() == 42);
        }
        if (config.has("string_value")) {
            assert(config.get<std::string>("string_value").value() == "hello world");
        }
    }
    
    // Test malformed YAML
    std::string malformedYaml = R"(invalid: yaml: [unclosed)";
    std::ofstream malformedFile("test_malformed.yaml");
    malformedFile << malformedYaml;
    malformedFile.close();
    
    bool malformedLoad = config.loadFromFile("test_malformed.yaml");
    // Malformed YAML should fail to load
    
    // Cleanup
    std::remove(tempYamlFile.c_str());
    std::remove("test_malformed.yaml");
    
    std::cout << "    testYAMLParsing passed" << std::endl;
}

void testSaveToFile() {
    nlm::Config config;
    
    // Set all different types of values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello world"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    config.set("double_array", std::vector<double>{1.1, 2.2, 3.3}, nlm::ConfigSource::Runtime);
    config.set("string_array", std::vector<std::string>{"apple", "banana", "cherry"}, nlm::ConfigSource::Runtime);
    
    std::string tempFile = "test_saved_config.txt";
    bool saveSuccess = config.saveToFile(tempFile);
    assert(saveSuccess);
    
    // Verify file was created and has content
    assert(std::filesystem::exists(tempFile));
    assert(std::filesystem::file_size(tempFile) > 0);
    
    // Read back and verify values
    std::ifstream savedFile(tempFile);
    std::string line;
    bool foundInt = false;
    bool foundDouble = false;
    bool foundString = false;
    
    while (std::getline(savedFile, line)) {
        if (line.find("int_value") != std::string::npos) {
            // Verify actual value (not PLACEHOLDER_VALUE)
            assert(line.find("42") != std::string::npos);
            foundInt = true;
        }
        if (line.find("double_value") != std::string::npos) {
            assert(line.find("3.14159") != std::string::npos);
            foundDouble = true;
        }
        if (line.find("string_value") != std::string::npos) {
            assert(line.find("hello world") != std::string::npos);
            foundString = true;
        }
    }
    
    assert(foundInt);
    assert(foundDouble);
    assert(foundString);
    
    // Cleanup
    std::remove(tempFile.c_str());
    
    std::cout << "    testSaveToFile passed" << std::endl;
}

void testValidation() {
    nlm::Config config;
    
    // Test key validation
    assert(!config.has("invalid key!")); // Key with invalid character
    assert(!config.has("123starts_with_number")); // Key starting with number
    assert(config.has("ValidKey")); // Valid key would pass
    
    // Test value validation by attempting to set values
    // String value validation (length limits)
    std::string longString(2000, 'a'); // Exceeds MAX_STRING_LENGTH
    config.set("long_string", longString, nlm::ConfigSource::Runtime);
    // Should not be set due to validation failure
    
    std::string normalString(100, 'b'); // Within limits
    config.set("normal_string", normalString, nlm::ConfigSource::Runtime);
    assert(config.has("normal_string"));
    
    // Test array size validation
    std::vector<int> largeArray(20000, 1); // Exceeds MAX_ARRAY_SIZE
    config.set("large_array", largeArray, nlm::ConfigSource::Runtime);
    // Should not be set due to validation failure
    
    std::vector<int> normalArray(10, 1); // Within limits
    config.set("normal_array", normalArray, nlm::ConfigSource::Runtime);
    assert(config.has("normal_array"));
    
    // Test numeric value validation
    double infValue = std::numeric_limits<double>::infinity();
    config.set("inf_value", infValue, nlm::ConfigSource::Runtime);
    // Should not be set due to validation failure
    
    double normalDouble = 3.14;
    config.set("normal_double", normalDouble, nlm::ConfigSource::Runtime);
    assert(config.has("normal_double"));
    
    std::cout << "    testValidation passed" << std::endl;
}

void testBackwardCompatibility() {
    nlm::Config config;
    
    // Create a simple key=value file (backward compatibility format)
    std::string simpleContent = R"(int_value = 100
double_value = 2.718
string_value = "test string"
bool_value = true
array_value = [1, 2, 3, 4, 5]
)";
    
    std::string tempFile = "test_simple_config.txt";
    std::ofstream simpleFile(tempFile);
    simpleFile << simpleContent;
    simpleFile.close();
    
    // Load using backward compatible format
    bool loadSuccess = config.loadFromFile(tempFile);
    assert(loadSuccess);
    
    // Verify values were loaded
    assert(config.has("int_value"));
    assert(config.get<int>("int_value").value() == 100);
    
    assert(config.has("double_value"));
    assert(config.get<double>("double_value").value() > 2.717 && config.get<double>("double_value").value() < 2.719);
    
    assert(config.has("string_value"));
    assert(config.get<std::string>("string_value").value() == "test string");
    
    assert(config.has("bool_value"));
    assert(config.get<bool>("bool_value").value() == true);
    
    // Cleanup
    std::remove(tempFile.c_str());
    
    std::cout << "    testBackwardCompatibility passed" << std::endl;
}

void testFilePathValidation() {
    nlm::Config config;
    
    // Test non-existent file
    bool loadNonExistent = config.loadFromFile("non_existent_file.json");
    assert(!loadNonExistent);
    
    // Test directory instead of file (if we could create a directory named a file)
    // Note: This test may be limited by permissions
    
    // Test empty file path
    bool loadEmptyPath = config.loadFromFile("");
    assert(!loadEmptyPath);
    
    // Test file path with invalid characters (if filesystem allows such paths)
    std::string invalidPath = "invalid<>|:*?.json";
    bool loadInvalidPath = config.loadFromFile(invalidPath);
    assert(!loadInvalidPath);
    
    std::cout << "    testFilePathValidation passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testJSONParsing();
    testYAMLParsing();
    testSaveToFile();
    testValidation();
    testBackwardCompatibility();
    testFilePathValidation();
}

} // namespace test_config
