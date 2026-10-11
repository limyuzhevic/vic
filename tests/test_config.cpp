// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

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

void testLegacyConfigFile() {
    std::string testFile = "test_legacy.cfg";
    
    // Create a test legacy config file
    std::ofstream outFile(testFile);
    outFile << "# Test legacy config file\n";
    outFile << "int_value = 100\n";
    outFile << "double_value = 3.14159\n";
    outFile << "string_value = test_string\n";
    outFile << "bool_value = true\n";
    outFile.close();
    
    nlm::Config config;
    assert(config.loadFromFile(testFile));
    
    assert(config.has("int_value"));
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 100);
    
    assert(config.has("double_value"));
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.14158 && *doubleVal < 3.14160);
    
    assert(config.has("string_value"));
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "test_string");
    
    assert(config.has("bool_value"));
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Test save
    std::string savedFile = "test_legacy_saved.cfg";
    assert(config.saveToFile(savedFile));
    
    // Load the saved file and verify
    nlm::Config config2;
    assert(config2.loadFromFile(savedFile));
    assert(config2.has("int_value"));
    assert(*config2.get<int>("int_value") == 100);
    
    // Clean up
    std::remove(testFile.c_str());
    std::remove(savedFile.c_str());
    
    std::cout << "    testLegacyConfigFile passed" << std::endl;
}

void testJsonConfigFile() {
    std::string testFile = "test_config.json";
    
    // Create a test JSON config file
    std::ofstream outFile(testFile);
    outFile << "{\n";
    outFile << "  \"int_value\": 100,\n";
    outFile << "  \"double_value\": 3.14159,\n";
    outFile << "  \"string_value\": \"test_string\",\n";
    outFile << "  \"bool_value\": true,\n";
    outFile << "  \"array_value\": [1, 2, 3],\n";
    outFile << "  \"float_array\": [1.5, 2.5, 3.5]\n";
    outFile << "}\n";
    outFile.close();
    
    nlm::Config config;
    assert(config.loadFromFile(testFile));
    
    assert(config.has("int_value"));
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 100);
    
    assert(config.has("double_value"));
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.14158 && *doubleVal < 3.14160);
    
    assert(config.has("string_value"));
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "test_string");
    
    assert(config.has("bool_value"));
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Test array values
    auto arrayVal = config.get<std::vector<int>>("array_value");
    assert(arrayVal.has_value());
    assert(arrayVal->size() == 3);
    assert((*arrayVal)[0] == 1);
    assert((*arrayVal)[1] == 2);
    assert((*arrayVal)[2] == 3);
    
    auto floatArrayVal = config.get<std::vector<double>>("float_array");
    assert(floatArrayVal.has_value());
    assert(floatArrayVal->size() == 3);
    assert((*floatArrayVal)[0] > 1.49 && (*floatArrayVal)[0] < 1.51);
    
    // Test save JSON
    std::string savedFile = "test_config_saved.json";
    assert(config.saveToFile(savedFile));
    
    // Load the saved file and verify
    nlm::Config config2;
    assert(config2.loadFromFile(savedFile));
    assert(config2.has("int_value"));
    assert(*config2.get<int>("int_value") == 100);
    
    // Clean up
    std::remove(testFile.c_str());
    std::remove(savedFile.c_str());
    
    std::cout << "    testJsonConfigFile passed" << std::endl;
}

void testYamlConfigFile() {
    std::string testFile = "test_config.yaml";
    
    // Create a test YAML config file
    std::ofstream outFile(testFile);
    outFile << "int_value: 200\n";
    outFile << "double_value: 2.71828\n";
    outFile << "string_value: yaml_test\n";
    outFile << "bool_value: false\n";
    outFile << "array_value: [4, 5, 6]\n";
    outFile << "float_array: [4.1, 5.2, 6.3]\n";
    outFile.close();
    
    nlm::Config config;
    assert(config.loadFromFile(testFile));
    
    assert(config.has("int_value"));
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 200);
    
    assert(config.has("double_value"));
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 2.71827 && *doubleVal < 2.71829);
    
    assert(config.has("string_value"));
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "yaml_test");
    
    assert(config.has("bool_value"));
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == false);
    
    // Test array values
    auto arrayVal = config.get<std::vector<int>>("array_value");
    assert(arrayVal.has_value());
    assert(arrayVal->size() == 3);
    assert((*arrayVal)[0] == 4);
    assert((*arrayVal)[1] == 5);
    assert((*arrayVal)[2] == 6);
    
    auto floatArrayVal = config.get<std::vector<double>>("float_array");
    assert(floatArrayVal.has_value());
    assert(floatArrayVal->size() == 3);
    assert((*floatArrayVal)[0] > 4.09 && (*floatArrayVal)[0] < 4.11);
    
    // Test save YAML
    std::string savedFile = "test_config_saved.yaml";
    assert(config.saveToFile(savedFile));
    
    // Load the saved file and verify
    nlm::Config config2;
    assert(config2.loadFromFile(savedFile));
    assert(config2.has("int_value"));
    assert(*config2.get<int>("int_value") == 200);
    
    // Clean up
    std::remove(testFile.c_str());
    std::remove(savedFile.c_str());
    
    std::cout << "    testYamlConfigFile passed" << std::endl;
}

void testValidation() {
    nlm::Config config;
    
    // Set up validation constraints
    nlm::ValidationConstraint rangeConstraint;
    rangeConstraint.type = nlm::ValidationType::Range;
    rangeConstraint.minValue = 0.0;
    rangeConstraint.maxValue = 100.0;
    rangeConstraint.errorMessage = "Value must be between 0 and 100";
    
    nlm::ValidationConstraint enumConstraint;
    enumConstraint.type = nlm::ValidationType::Enum;
    enumConstraint.enumValues = {"red", "green", "blue"};
    enumConstraint.errorMessage = "Value must be red, green, or blue";
    
    nlm::ValidationConstraint patternConstraint;
    patternConstraint.type = nlm::ValidationType::Pattern;
    patternConstraint.patternRegex = "^[a-zA-Z0-9_]+$";
    patternConstraint.errorMessage = "Value must contain only letters, numbers, and underscores";
    
    // Add validations
    config.addValidation("temperature", rangeConstraint);
    config.addValidation("color", enumConstraint);
    config.addValidation("username", patternConstraint);
    
    // Test range validation (valid)
    config.set("temperature", 50, nlm::ConfigSource::Runtime);
    assert(config.validate("temperature"));
    
    // Test range validation (invalid)
    config.set("temperature", 150, nlm::ConfigSource::Runtime);
    assert(!config.validate("temperature"));
    
    // Test enum validation (valid)
    config.set("color", std::string("red"), nlm::ConfigSource::Runtime);
    assert(config.validate("color"));
    
    // Test enum validation (invalid)
    config.set("color", std::string("yellow"), nlm::ConfigSource::Runtime);
    assert(!config.validate("color"));
    
    // Test pattern validation (valid)
    config.set("username", std::string("user123"), nlm::ConfigSource::Runtime);
    assert(config.validate("username"));
    
    // Test pattern validation (invalid)
    config.set("username", std::string("user@123"), nlm::ConfigSource::Runtime);
    assert(!config.validate("username"));
    
    std::cout << "    testValidation passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::File);
    config.set("string_value", std::string("test"), nlm::ConfigSource::CommandLine);
    config.set("bool_value", true, nlm::ConfigSource::Default);
    
    std::string summary = config.summary();
    
    assert(summary.find("Configuration (4 entries):") != std::string::npos);
    assert(summary.find("int_value = [42]") != std::string::npos);
    assert(summary.find("double_value = [3.14]") != std::string::npos);
    assert(summary.find("string_value = [\"test\"]") != std::string::npos);
    assert(summary.find("bool_value = [true]") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testLegacyConfigFile();
    testJsonConfigFile();
    testYamlConfigFile();
    testValidation();
    testConfigSummary();
}

} // namespace test_config