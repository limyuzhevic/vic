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

void testConfigJSONLoad() {
    nlm::Config config;
    
    // Create a test JSON file
    std::ofstream jsonFile("test.json");
    jsonFile << R"({"json_int": 123, "json_double": 45.67, "json_string": "test", "json_bool": true})";
    jsonFile.close();
    
    assert(config.loadFromJSON("test.json"));
    assert(config.has("json_int"));
    assert(config.get<int>("json_int").value() == 123);
    assert(config.has("json_double"));
    assert(config.get<double>("json_double").value() > 45.66 && config.get<double>("json_double").value() < 45.68);
    assert(config.has("json_string"));
    assert(config.get<std::string>("json_string").value() == "test");
    assert(config.has("json_bool"));
    assert(config.get<bool>("json_bool").value() == true);
    
    // Clean up
    std::remove("test.json");
    
    std::cout << "    testConfigJSONLoad passed" << std::endl;
}

void testConfigYAMLLoad() {
    nlm::Config config;
    
    // Create a test YAML file
    std::ofstream yamlFile("test.yaml");
    yamlFile << "yaml_int: 123\nyaml_double: 45.67\nyaml_string: test\nyaml_bool: true\n";
    yamlFile.close();
    
    assert(config.loadFromYAML("test.yaml"));
    assert(config.has("yaml_int"));
    assert(config.get<int>("yaml_int").value() == 123);
    assert(config.has("yaml_double"));
    assert(config.get<double>("yaml_double").value() > 45.66 && config.get<double>("yaml_double").value() < 45.68);
    assert(config.has("yaml_string"));
    assert(config.get<std::string>("yaml_string").value() == "test");
    assert(config.has("yaml_bool"));
    assert(config.get<bool>("yaml_bool").value() == true);
    
    // Clean up
    std::remove("test.yaml");
    
    std::cout << "    testConfigYAMLLoad passed" << std::endl;
}

void testConfigFileLoad() {
    nlm::Config config;
    
    // Test simple key=value format
    std::ofstream simpleFile("test.cfg");
    simpleFile << "# Test comment\nsimple_key = simple_value\nsimple_int = 42\n";
    simpleFile.close();
    
    assert(config.loadFromFile("test.cfg"));
    assert(config.has("simple_key"));
    assert(config.get<std::string>("simple_key").value() == "simple_value");
    assert(config.has("simple_int"));
    assert(config.get<int>("simple_int").value() == 42);
    
    // Clean up
    std::remove("test.cfg");
    
    // Test JSON format
    std::ofstream jsonFile("test.json");
    jsonFile << R"({"json_key": "json_value", "json_num": 99})";
    jsonFile.close();
    
    assert(config.loadFromFile("test.json"));
    assert(config.has("json_key"));
    assert(config.get<std::string>("json_key").value() == "json_value");
    assert(config.has("json_num"));
    assert(config.get<int>("json_num").value() == 99);
    
    // Clean up
    std::remove("test.json");
    
    // Test YAML format
    std::ofstream yamlFile("test.yaml");
    yamlFile << "yaml_key: yaml_value\nyaml_num: 88\n";
    yamlFile.close();
    
    assert(config.loadFromFile("test.yaml"));
    assert(config.has("yaml_key"));
    assert(config.get<std::string>("yaml_key").value() == "yaml_value");
    assert(config.has("yaml_num"));
    assert(config.get<int>("yaml_num").value() == 88);
    
    // Clean up
    std::remove("test.yaml");
    
    std::cout << "    testConfigFileLoad passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJSONLoad();
    testConfigYAMLLoad();
    testConfigFileLoad();
}

} // namespace test_config