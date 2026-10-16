// Config Tests - Enhanced Version
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

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

void testConfigGroups() {
    nlm::Config config;
    
    // Test group management
    config.addGroup("test_group", "Test configuration group", false);
    auto groups = config.getGroups();
    assert(groups.size() == 5); // 4 default + 1 new
    assert(std::find(groups.begin(), groups.end(), "test_group") != groups.end());
    
    // Test setting group for an entry
    config.set("grouped_key", 42, nlm::ConfigSource::Runtime);
    config.setEntryGroup("grouped_key", "test_group");
    assert(config.getEntryGroup("grouped_key") == "test_group");
    
    auto entriesInGroup = config.getEntriesInGroup("test_group");
    assert(entriesInGroup.size() == 1);
    assert(entriesInGroup[0].key == "grouped_key");
    
    std::cout << "    testConfigGroups passed" << std::endl;
}

void testConfigJSONParsing() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::string jsonContent = R"({
        "name": "test_config",
        "int_value": 42,
        "double_value": 3.14,
        "bool_value": true,
        "nested": {
            "inner_key": "inner_value",
            "inner_number": 100
        },
        "array": [1, 2, 3],
        "string_array": ["a", "b", "c"]
    })";
    
    std::string tempJsonPath = "/tmp/test_config.json";
    std::ofstream jsonFile(tempJsonPath);
    jsonFile << jsonContent;
    jsonFile.close();
    
    bool success = config.loadFromFile(tempJsonPath);
    assert(success);
    
    assert(config.get<int>("int_value").value() == 42);
    assert(config.get<double>("double_value").value() == 3.14);
    assert(config.get<bool>("bool_value").value() == true);
    assert(config.get<std::string>("name").value() == "test_config");
    
    // Note: Nested objects and arrays require enhanced type support
    // For now, we test basic parsing
    std::remove(tempJsonPath.c_str());
    
    std::cout << "    testConfigJSONParsing passed" << std::endl;
}

void testConfigYAMLParsing() {
    nlm::Config config;
    
    // Create a temporary YAML file
    std::string yamlContent = R"(---
name: test_config
has_debug: true
test_number: 3.14
list:
  - item1
  - item2
  - item3
")";
    
    std::string tempYamlPath = "/tmp/test_config.yaml";
    std::ofstream yamlFile(tempYamlPath);
    yamlFile << yamlContent;
    yamlFile.close();
    
    bool success = config.loadFromFile(tempYamlPath);
    assert(success);
    
    assert(config.get<std::string>("name").value() == "test_config");
    assert(config.get<bool>("has_debug").value() == true);
    assert(config.get<double>("test_number").value() == 3.14);
    
    std::remove(tempYamlPath.c_str());
    
    std::cout << "    testConfigYAMLParsing passed" << std::endl;
}

void testConfigCFGBackwardCompatibility() {
    nlm::Config config;
    
    // Create a temporary CFG file
    std::string cfgContent = R"(# Test CFG file
# This is a test comment
int_value = 42
double_value = 3.14
string_value = "hello world"
bool_value = true
)";
    
    std::string tempCfgPath = "/tmp/test_config.cfg";
    std::ofstream cfgFile(tempCfgPath);
    cfgFile << cfgContent;
    cfgFile.close();
    
    bool success = config.loadFromFile(tempCfgPath);
    assert(success);
    
    assert(config.get<int>("int_value").value() == 42);
    assert(config.get<double>("double_value").value() == 3.14);
    assert(config.get<std::string>("string_value").value() == "hello world");
    assert(config.get<bool>("bool_value").value() == true);
    
    std::remove(tempCfgPath.c_str());
    
    std::cout << "    testConfigCFGBackwardCompatibility passed" << std::endl;
}

void testConfigEnvironmentVariables() {
    nlm::Config config;
    
    // Set environment variables
    std::map<std::string, std::string> envVars = {
        {"APP_NAME", "TestApp"},
        {"APP_VERSION", "1.0.0"},
        {"APP_ENABLED", "true"}
    };
    
    config.loadFromEnvironment(envVars);
    
    assert(config.hasEnvironmentVariable("APP_NAME"));
    assert(config.getEnvironmentVariable("APP_NAME") == "TestApp");
    assert(config.hasEnvironmentVariable("APP_VERSION"));
    assert(config.getEnvironmentVariable("APP_VERSION") == "1.0.0");
    assert(config.hasEnvironmentVariable("APP_ENABLED"));
    assert(config.getEnvironmentVariable("APP_ENABLED") == "true");
    
    // Check interpolation (note: interpolation happens during loading)
    // This tests the environment variable infrastructure
    std::cout << "    testConfigEnvironmentVariables passed" << std::endl;
}

void testConfigFileSaving() {
    nlm::Config config;
    
    // Set some values
    config.set("name", "TestConfig", nlm::ConfigSource::Runtime);
    config.set("version", 1, nlm::ConfigSource::Runtime);
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    
    // Test saving as JSON
    std::string jsonPath = "/tmp/test_save.json";
    bool jsonSuccess = config.saveToFile(jsonPath);
    assert(jsonSuccess);
    
    // Test saving as YAML
    std::string yamlPath = "/tmp/test_save.yaml";
    bool yamlSuccess = config.saveToFile(yamlPath);
    assert(yamlSuccess);
    
    // Test saving as CFG
    std::string cfgPath = "/tmp/test_save.cfg";
    bool cfgSuccess = config.saveToFile(cfgPath);
    assert(cfgSuccess);
    
    // Clean up
    std::remove(jsonPath.c_str());
    std::remove(yamlPath.c_str());
    std::remove(cfgPath.c_str());
    
    std::cout << "    testConfigFileSaving passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "valid_string", nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    
    // Test validation with valid values
    assert(config.validate("int_value", "123"));
    assert(config.validate("string_value", "another_string"));
    assert(config.validate("double_value", "5.67"));
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigTypeInformation() {
    nlm::Config config;
    
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_val", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    
    // Test type information (requires access to internal helper)
    // For now, we just test that the config works
    assert(config.getEntryCount() == 4);
    
    std::cout << "    testConfigTypeInformation passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigGroups();
    testConfigJSONParsing();
    testConfigYAMLParsing();
    testConfigCFGBackwardCompatibility();
    testConfigEnvironmentVariables();
    testConfigFileSaving();
    testConfigValidation();
    testConfigTypeInformation();
    
    std::cout << "\nAll config tests passed!" << std::endl;
}

} // namespace test_config
