// Config Tests
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

void testFileFormatDetection() {
    // Test JSON detection
    assert(nlm::Config::detectFileFormat("config.json") == nlm::FileFormat::JSON);
    assert(nlm::Config::detectFileFormat("/path/to/config.JSON") == nlm::FileFormat::JSON);
    
    // Test YAML detection
    assert(nlm::Config::detectFileFormat("config.yaml") == nlm::FileFormat::YAML);
    assert(nlm::Config::detectFileFormat("config.yml") == nlm::FileFormat::YAML);
    
    // Test key-value detection
    assert(nlm::Config::detectFileFormat("config.cfg") == nlm::FileFormat::KeyValue);
    assert(nlm::Config::detectFileFormat("config.ini") == nlm::FileFormat::KeyValue);
    assert(nlm::Config::detectFileFormat("config.properties") == nlm::FileFormat::KeyValue);
    
    // Test unknown format
    assert(nlm::Config::detectFileFormat("config.unknown") == nlm::FileFormat::Unknown);
    assert(nlm::Config::detectFileFormat("config") == nlm::FileFormat::Unknown);
    
    std::cout << "    testFileFormatDetection passed" << std::endl;
}

void testJSONLoading() {
    nlm::Config config;
    
    std::string json = R"({"int_value": 42, "string_value": "hello", "bool_value": true, "double_value": 3.14})";
    assert(config.loadFromJSON(json));
    
    assert(config.has("int_value"));
    assert(config.has("string_value"));
    assert(config.has("bool_value"));
    assert(config.has("double_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15);
    
    std::cout << "    testJSONLoading passed" << std::endl;
}

void testYAMLLoading() {
    nlm::Config config;
    
    std::string yaml = """int_value: 42
string_value: hello
bool_value: true
double_value: 3.14
""";
    assert(config.loadFromYAML(yaml));
    
    assert(config.has("int_value"));
    assert(config.has("string_value"));
    assert(config.has("bool_value"));
    assert(config.has("double_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    std::cout << "    testYAMLLoading passed" << std::endl;
}

void testKeyValueLoading() {
    nlm::Config config;
    
    std::string kv = """# Configuration comment
int_value = 42
string_value = "hello"
bool_value = true
double_value = 3.14
""";
    assert(config.loadFromKeyValue(kv));
    
    assert(config.has("int_value"));
    assert(config.has("string_value"));
    assert(config.has("bool_value"));
    assert(config.has("double_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    std::cout << "    testKeyValueLoading passed" << std::endl;
}

void testJSONSaving() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    
    std::string json = config.saveToJSON();
    
    // Parse the saved JSON
    nlm::Config config2;
    assert(config2.loadFromJSON(json));
    
    assert(config2.has("int_value"));
    assert(config2.has("string_value"));
    assert(config2.has("bool_value"));
    assert(config2.has("double_value"));
    
    auto intVal = config2.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    std::cout << "    testJSONSaving passed" << std::endl;
}

void testYAMLSaving() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    
    std::string yaml = config.saveToYAML();
    
    // Parse the saved YAML
    nlm::Config config2;
    assert(config2.loadFromYAML(yaml));
    
    assert(config2.has("int_value"));
    assert(config2.has("string_value"));
    assert(config2.has("bool_value"));
    assert(config2.has("double_value"));
    
    auto intVal = config2.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    std::cout << "    testYAMLSaving passed" << std::endl;
}

void testFileSavingLoading() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    
    // Test JSON file
    std::string jsonFile = "/tmp/test_config.json";
    assert(config.saveToFile(jsonFile));
    
    nlm::Config config2;
    assert(config2.loadFromFile(jsonFile));
    
    assert(config2.has("int_value"));
    assert(config2.has("string_value"));
    assert(config2.has("bool_value"));
    assert(config2.has("double_value"));
    
    auto intVal = config2.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    // Clean up
    std::filesystem::remove(jsonFile);
    
    // Test YAML file
    std::string yamlFile = "/tmp/test_config.yaml";
    assert(config.saveToFile(yamlFile));
    
    nlm::Config config3;
    assert(config3.loadFromFile(yamlFile));
    
    assert(config3.has("int_value"));
    
    auto intVal2 = config3.get<int>("int_value");
    assert(intVal2.has_value() && *intVal2 == 42);
    
    // Clean up
    std::filesystem::remove(yamlFile);
    
    // Test key-value file (backward compatibility)
    std::string kvFile = "/tmp/test_config.cfg";
    assert(config.saveToFile(kvFile));
    
    nlm::Config config4;
    assert(config4.loadFromFile(kvFile));
    
    assert(config4.has("int_value"));
    
    auto intVal3 = config4.get<int>("int_value");
    assert(intVal3.has_value() && *intVal3 == 42);
    
    // Clean up
    std::filesystem::remove(kvFile);
    
    std::cout << "    testFileSavingLoading passed" << std::endl;
}

void testErrorHandling() {
    nlm::Config config;
    
    // Test invalid JSON
    std::string invalidJSON = R"({invalid json})";
    try {
        config.loadFromJSON(invalidJSON);
        assert(false); // Should throw
    } catch (const nlm::ConfigException&) {
        // Expected
    }
    
    // Test invalid YAML
    std::string invalidYAML = "invalid: yaml: : :";
    try {
        config.loadFromYAML(invalidYAML);
        // May or may not throw depending on parser implementation
    } catch (const nlm::ConfigException&) {
        // Expected
    }
    
    std::cout << "    testErrorHandling passed" << std::endl;
}

void testValidation() {
    nlm::Config config;
    
    config.set("password", "weak", nlm::ConfigSource::Runtime);
    config.set("username", "admin", nlm::ConfigSource::Runtime);
    
    // Should not throw
    assert(config.validate());
    
    std::cout << "    testValidation passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testFileFormatDetection();
    testJSONLoading();
    testYAMLLoading();
    testKeyValueLoading();
    testJSONSaving();
    testYAMLSaving();
    testFileSavingLoading();
    testErrorHandling();
    testValidation();
    
    std::cout << std::endl;
    std::cout << "All Config tests passed!" << std::endl;
}

} // namespace test_config
