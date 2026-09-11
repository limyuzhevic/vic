#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <cassert>

void testJsonParsing() {
    std::cout << "Testing JSON parsing..." << std::endl;
    
    // Create a temporary JSON file
    std::ofstream jsonFile("test.json");
    jsonFile << "{\n";
    jsonFile << "  \"int_value\": 42,\n";
    jsonFile << "  \"double_value\": 3.14,\n";
    jsonFile << "  \"string_value\": \"hello\",\n";
    jsonFile << "  \"bool_value\": true\n";
    jsonFile << "}";
    jsonFile.close();
    
    nlm::Config config;
    assert(config.loadFromFile("test.json"));
    
    // Verify values
    assert(config.get<int>("int_value").has_value());
    assert(*config.get<int>("int_value") == 42);
    
    assert(config.get<double>("double_value").has_value());
    assert(*config.get<double>("double_value") > 3.13 && *config.get<double>("double_value") < 3.15);
    
    assert(config.get<std::string>("string_value").has_value());
    assert(*config.get<std::string>("string_value") == "hello");
    
    assert(config.get<bool>("bool_value").has_value());
    assert(*config.get<bool>("bool_value") == true);
    
    std::cout << "  JSON parsing test passed" << std::endl;
    
    // Clean up
    std::remove("test.json");
}

void testYamlParsing() {
    std::cout << "Testing YAML parsing..." << std::endl;
    
    // Create a temporary YAML file
    std::ofstream yamlFile("test.yaml");
    yamlFile << "int_value: 100\n";
    yamlFile << "double_value: 2.718\n";
    yamlFile << "string_value: \"world\"\n";
    yamlFile << "bool_value: false\n";
    yamlFile << "array_value: [1, 2, 3]\n";
    yamlFile.close();
    
    nlm::Config config;
    assert(config.loadFromFile("test.yaml"));
    
    // Verify values
    assert(config.get<int64_t>("int_value").has_value());
    assert(*config.get<int64_t>("int_value") == 100);
    
    assert(config.get<double>("double_value").has_value());
    assert(*config.get<double>("double_value") > 2.717 && *config.get<double>("double_value") < 2.719);
    
    assert(config.get<std::string>("string_value").has_value());
    assert(*config.get<std::string>("string_value") == "world");
    
    assert(config.get<bool>("bool_value").has_value());
    assert(*config.get<bool>("bool_value") == false);
    
    std::cout << "  YAML parsing test passed" << std::endl;
    
    // Clean up
    std::remove("test.yaml");
}

void testKeyValueParsing() {
    std::cout << "Testing key=value parsing (backward compatibility)..." << std::endl;
    
    // Create a temporary .cfg file
    std::ofstream cfgFile("test.cfg");
    cfgFile << "# Test configuration file\n";
    cfgFile << "int_value = 999\n";
    cfgFile << "double_value = 1.414\n";
    cfgFile << "string_value = \"test\"\n";
    cfgFile << "bool_value = true\n";
    cfgFile.close();
    
    nlm::Config config;
    assert(config.loadFromFile("test.cfg"));
    
    // Verify values
    assert(config.get<int64_t>("int_value").has_value());
    assert(*config.get<int64_t>("int_value") == 999);
    
    assert(config.get<double>("double_value").has_value());
    assert(*config.get<double>("double_value") > 1.413 && *config.get<double>("double_value") < 1.415);
    
    assert(config.get<std::string>("string_value").has_value());
    assert(*config.get<std::string>("string_value") == "test");
    
    assert(config.get<bool>("bool_value").has_value());
    assert(*config.get<bool>("bool_value") == true);
    
    std::cout << "  key=value parsing test passed" << std::endl;
    
    // Clean up
    std::remove("test.cfg");
}

void testFileTypeDetection() {
    std::cout << "Testing file type detection..." << std::endl;
    
    nlm::Config config;
    
    // Test .json extension
    std::ofstream jsonFile("test_json.json");
    jsonFile << "{\"key\": \"value\"}";
    jsonFile.close();
    
    assert(config.loadFromFile("test_json.json"));
    assert(config.has("key"));
    assert(*config.get<std::string>("key") == "value");
    
    std::remove("test_json.json");
    
    // Test .yaml extension
    std::ofstream yamlFile("test_yaml.yaml");
    yamlFile << "key: value";
    yamlFile.close();
    
    assert(config.loadFromFile("test_yaml.yaml"));
    assert(config.has("key"));
    assert(*config.get<std::string>("key") == "value");
    
    std::remove("test_yaml.yaml");
    
    // Test .yml extension
    std::ofstream ymlFile("test_yml.yml");
    ymlFile << "key: value2";
    ymlFile.close();
    
    assert(config.loadFromFile("test_yml.yml"));
    assert(config.has("key"));
    assert(*config.get<std::string>("key") == "value2");
    
    std::remove("test_yml.yml");
    
    // Test .cfg extension (backward compatibility)
    std::ofstream cfgFile("test_cfg.cfg");
    cfgFile << "key = value3";
    cfgFile.close();
    
    assert(config.loadFromFile("test_cfg.cfg"));
    assert(config.has("key"));
    assert(*config.get<std::string>("key") == "value3");
    
    std::remove("test_cfg.cfg");
    
    std::cout << "  File type detection test passed" << std::endl;
}

int main() {
    std::cout << "=== Config File Parser Tests ===" << std::endl << std::endl;
    
    testFileTypeDetection();
    testJsonParsing();
    testYamlParsing();
    testKeyValueParsing();
    
    std::cout << std::endl << "=== All Tests Passed! ===" << std::endl;
    
    return 0;
}
