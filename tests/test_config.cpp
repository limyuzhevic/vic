// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

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

void testJSONLoadSave() {
    nlm::Config config;
    
    // Test setting values
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("string_key", "hello", nlm::ConfigSource::Runtime);
    config.set("double_key", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    
    // Test JSON serialization
    std::string jsonStr = config.toJSON();
    
    // Parse JSON string back to config
    nlm::Config config2;
    bool loadSuccess = config2.loadFromJSONString(jsonStr);
    assert(loadSuccess);
    
    // Verify values match
    assert(*config2.get<int>("int_key") == 42);
    assert(*config2.get<std::string>("string_key") == "hello");
    assert(*config2.get<double>("double_key") == 3.14);
    assert(*config2.get<bool>("bool_key") == true);
    
    // Test save to JSON file
    std::string jsonFile = "test_json.json";
    bool saveSuccess = config.toJSONFile(jsonFile);
    assert(saveSuccess);
    
    // Verify file exists and can be loaded
    nlm::Config config3;
    bool loadFileSuccess = config3.loadFromJSONFile(jsonFile);
    assert(loadFileSuccess);
    
    assert(*config3.get<int>("int_key") == 42);
    assert(*config3.get<std::string>("string_key") == "hello");
    
    // Clean up
    std::remove(jsonFile.c_str());
    
    std::cout << "    testJSONLoadSave passed" << std::endl;
}

void testYAMLLoadSave() {
    nlm::Config config;
    
    // Test setting values
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("string_key", "hello", nlm::ConfigSource::Runtime);
    config.set("double_key", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    
    // Test YAML serialization
    std::string yamlStr = config.toYAML();
    
    // Parse YAML string back to config
    nlm::Config config2;
    bool loadSuccess = config2.loadFromYAMLString(yamlStr);
    assert(loadSuccess);
    
    // Verify values match
    assert(*config2.get<int>("int_key") == 42);
    assert(*config2.get<std::string>("string_key") == "hello");
    assert(*config2.get<double>("double_key") == 3.14);
    assert(*config2.get<bool>("bool_key") == true);
    
    // Test save to YAML file
    std::string yamlFile = "test_yaml.yaml";
    bool saveSuccess = config.toYAMLFile(yamlFile);
    assert(saveSuccess);
    
    // Verify file exists and can be loaded
    nlm::Config config3;
    bool loadFileSuccess = config3.loadFromYAMLFile(yamlFile);
    assert(loadFileSuccess);
    
    assert(*config3.get<int>("int_key") == 42);
    assert(*config3.get<std::string>("string_key") == "hello");
    
    // Clean up
    std::remove(yamlFile.c_str());
    
    std::cout << "    testYAMLLoadSave passed" << std::endl;
}

void testFormatDetection() {
    // Test JSON format detection
    nlohmann::json jsonData;
    jsonData["test"] = "value";
    std::string jsonStr = jsonData.dump();
    
    // Test YAML format detection
    YAML::Node yamlData;
    yamlData["test"] = "value";
    std::string yamlStr = YAML::Dump(yamlData);
    
    // Test simple format detection
    std::string simpleData = "key = value\n# comment\nint = 42";
    
    // Test format detection methods
    assert(nlm::Config::isJSON(jsonStr));
    assert(nlm::Config::isYAML(yamlStr));
    assert(nlm::Config::isSimple(simpleData));
    
    // Test auto-format detection
    std::string autoFormat = nlm::Config::detectFormat("test.json");
    
    std::cout << "    testFormatDetection passed" << std::endl;
}

void testJSONSchema() {
    nlm::Config config;
    
    // Add some test data
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("string_key", "hello", nlm::ConfigSource::Runtime);
    config.set("double_key", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    
    // Generate JSON schema
    std::string schema = config.getJSONSchema();
    
    // Parse schema to verify it's valid JSON
    nlohmann::json schemaJson = nlohmann::json::parse(schema);
    
    // Verify schema structure
    assert(schemaJson["type"] == "object");
    assert(schemaJson.contains("properties"));
    assert(schemaJson.contains("required"));
    
    // Verify all properties are present
    assert(schemaJson["properties"].contains("int_key"));
    assert(schemaJson["properties"].contains("string_key"));
    assert(schemaJson["properties"].contains("double_key"));
    assert(schemaJson["properties"].contains("bool_key"));
    assert(schemaJson["properties"].contains("int_array"));
    
    std::cout << "    testJSONSchema passed" << std::endl;
}

void testAutoFormatDetection() {
    // Create a temporary config file with JSON content
    std::string jsonFile = "test_auto.json";
    nlm::Config config;
    config.set("key", "value", nlm::ConfigSource::Runtime);
    
    // Save as JSON
    config.toJSONFile(jsonFile);
    
    // Load using auto-detection
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(jsonFile);
    assert(loadSuccess);
    
    assert(*config2.get<std::string>("key") == "value");
    
    // Create a temporary config file with YAML content
    std::string yamlFile = "test_auto.yaml";
    config.toYAMLFile(yamlFile);
    
    // Load using auto-detection
    nlm::Config config3;
    loadSuccess = config3.loadFromFile(yamlFile);
    assert(loadSuccess);
    
    assert(*config3.get<std::string>("key") == "value");
    
    // Create a temporary config file with simple content
    std::string simpleFile = "test_auto.cfg";
    std::ofstream simpleFileStream(simpleFile);
    simpleFileStream << "# Test config\n";
    simpleFileStream << "key = value\n";
    simpleFileStream << "number = 123\n";
    simpleFileStream.close();
    
    // Load using auto-detection
    nlm::Config config4;
    loadSuccess = config4.loadFromFile(simpleFile);
    assert(loadSuccess);
    
    assert(*config4.get<std::string>("key") == "value");
    assert(*config4.get<int>("number") == 123);
    
    // Clean up
    std::remove(jsonFile.c_str());
    std::remove(yamlFile.c_str());
    std::remove(simpleFile.c_str());
    
    std::cout << "    testAutoFormatDetection passed" << std::endl;
}

void testParserErrors() {
    nlm::Config config;
    
    // Try to load invalid JSON
    bool loadSuccess = config.loadFromJSONString("invalid json string");
    assert(!loadSuccess);
    
    // Verify errors are captured
    auto errors = config.getParserErrors();
    assert(!errors.empty());
    
    // Try to load invalid YAML
    bool loadYAMLSuccess = config.loadFromYAMLString("invalid: yaml: content: [");
    assert(!loadYAMLSuccess);
    
    // Clear errors
    config.clearParserErrors();
    assert(config.getParserErrors().empty());
    
    std::cout << "    testParserErrors passed" << std::endl;
}

void testStaticFactoryMethods() {
    // Test fromJSON static method
    std::string jsonStr = "{\"key\": \"value\", \"num\": 42}";
    auto config = nlm::Config::fromJSON(jsonStr);
    assert(config != nullptr);
    assert(*config->get<std::string>("key") == "value");
    assert(*config->get<int>("num") == 42);
    
    // Test fromYAML static method
    std::string yamlStr = "key: value\nnum: 42";
    auto config2 = nlm::Config::fromYAML(yamlStr);
    assert(config2 != nullptr);
    assert(*config2->get<std::string>("key") == "value");
    assert(*config2->get<int>("num") == 42);
    
    // Test mergeJSON static method
    auto baseConfig = std::make_shared<nlm::Config>();
    baseConfig->set("existing", "value", nlm::ConfigSource::Runtime);
    
    std::string jsonToMerge = "{\"new_key\": \"new_value\", \"existing\": \"updated\""};
    auto mergedConfig = nlm::Config::mergeJSON(baseConfig, jsonToMerge);
    assert(mergedConfig != nullptr);
    assert(*mergedConfig->get<std::string>("existing") == "updated");
    assert(*mergedConfig->get<std::string>("new_key") == "new_value");
    
    std::cout << "    testStaticFactoryMethods passed" << std::endl;
}

void testConfigExport() {
    nlm::Config config;
    
    // Add test data
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("string_key", "hello", nlm::ConfigSource::Runtime);
    config.set("double_key", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    
    // Test exportJSON and exportYAML
    std::string jsonStr = config.exportJSON();
    std::string yamlStr = config.exportYAML();
    
    // Parse exports to verify they are valid
    nlohmann::json jsonData = nlohmann::json::parse(jsonStr);
    YAML::Node yamlData = YAML::Load(yamlStr);
    
    assert(jsonData["int_key"] == 42);
    assert(jsonData["string_key"] == "hello");
    assert(jsonData["double_key"] == 3.14);
    assert(jsonData["bool_key"] == true);
    
    assert(yamlData["int_key"].as<int>() == 42);
    assert(yamlData["string_key"].as<std::string>() == "hello");
    assert(yamlData["double_key"].as<double>() == 3.14);
    assert(yamlData["bool_key"].as<bool>() == true);
    
    std::cout << "    testConfigExport passed" << std::endl;
}

void testConfigEntries() {
    nlm::Config config;
    
    // Add entries with descriptions
    config.set("key1", "value1", nlm::ConfigSource::File, "First key");
    config.set("key2", "value2", nlm::ConfigSource::JSON, "Second key");
    
    // Get all entries
    auto entries = config.getEntries();
    assert(entries.size() == 2);
    
    // Verify entries
    bool foundKey1 = false;
    bool foundKey2 = false;
    
    for (const auto& entry : entries) {
        if (entry.key == "key1") {
            assert(entry.description == "First key");
            assert(entry.source == nlm::ConfigSource::File);
            foundKey1 = true;
        } else if (entry.key == "key2") {
            assert(entry.description == "Second key");
            assert(entry.source == nlm::ConfigSource::JSON);
            foundKey2 = true;
        }
    }
    
    assert(foundKey1 && foundKey2);
    
    std::cout << "    testConfigEntries passed" << std::endl;
}

void testCompareJSON() {
    nlm::Config config1;
    config1.set("key1", "value1", nlm::ConfigSource::Runtime);
    config1.set("key2", 42, nlm::ConfigSource::Runtime);
    
    nlm::Config config2;
    config2.set("key1", "value1", nlm::ConfigSource::Runtime);
    config2.set("key2", 42, nlm::ConfigSource::Runtime);
    
    // Test identical configs
    std::string jsonStr1 = config1.toJSON();
    std::string jsonStr2 = config2.toJSON();
    
    assert(config1.compareJSON(jsonStr2)); // Should be equal
    
    // Modify config2
    config2.set("key3", "new_value", nlm::ConfigSource::Runtime);
    std::string jsonStr3 = config2.toJSON();
    
    assert(!config1.compareJSON(jsonStr3)); // Should not be equal
    
    // Test diffJSON
    std::string diff = config1.diffJSON(jsonStr3);
    assert(!diff.empty());
    assert(diff.find("+ key3: new_value") != std::string::npos ||
           diff.find("- key3: new_value") != std::string::npos);
    
    std::cout << "    testCompareJSON passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testJSONLoadSave();
    testYAMLLoadSave();
    testFormatDetection();
    testJSONSchema();
    testAutoFormatDetection();
    testParserErrors();
    testStaticFactoryMethods();
    testConfigExport();
    testConfigEntries();
    testCompareJSON();
}

} // namespace test_config
