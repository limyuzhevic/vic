// JSON/YAML Config Tests
#include "core/Config/Config.hpp"
#include "core/Config/ConfigValidator.hpp"
#include "core/Config/ConfigPresets.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>

namespace test_json_config {

void testJSONLoad() {
    nlm::Config config;
    
    // Test JSON string
    std::string jsonStr = R"({"int_value": 42, "double_value": 3.14, "string_value": "hello", "bool_value": true})";
    assert(config.loadFromJSON(jsonStr));
    
    assert(config.has("int_value"));
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    assert(config.has("double_value"));
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    assert(config.has("string_value"));
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    assert(config.has("bool_value"));
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testJSONLoad passed" << std::endl;
}

void testJSONSave() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Test JSON save to file
    std::string jsonFile = "test_config.json";
    assert(config.saveToJSON(jsonFile));
    
    // Load it back
    nlm::Config config2;
    assert(config2.loadFromJSON(jsonFile));
    
    assert(config2.get<int>("int_value") == config.get<int>("int_value"));
    assert(config2.get<double>("double_value") == config.get<double>("double_value"));
    assert(config2.get<std::string>("string_value") == config.get<std::string>("string_value"));
    assert(config2.get<bool>("bool_value") == config.get<bool>("bool_value"));
    
    // Clean up
    std::remove(jsonFile.c_str());
    
    std::cout << "    testJSONSave passed" << std::endl;
}

void testYAMLLoad() {
    nlm::Config config;
    
    // Create a simple YAML file for testing
    std::string yamlContent = """---
int_value: 42
double_value: 3.14
string_value: hello
bool_value: true
array_int: [1, 2, 3]
""";
    
    std::string yamlFile = "test_config.yaml";
    std::ofstream yamlOut(yamlFile);
    yamlOut << yamlContent;
    yamlOut.close();
    
    assert(config.loadFromYAML(yamlFile));
    
    assert(config.has("int_value"));
    assert(config.has("double_value"));
    assert(config.has("string_value"));
    assert(config.has("bool_value"));
    assert(config.has("array_int"));
    
    // Clean up
    std::remove(yamlFile.c_str());
    
    std::cout << "    testYAMLLoad passed" << std::endl;
}

void testFileFormatDetection() {
    nlm::Config config;
    
    // Test JSON file detection
    std::string jsonFile = "test.json";
    config.set("test_key", "test_value", nlm::ConfigSource::Runtime);
    assert(config.saveToFile(jsonFile));
    
    nlm::Config config2;
    assert(config2.loadFromFile(jsonFile));
    
    std::remove(jsonFile.c_str());
    
    // Test simple file detection
    std::string simpleFile = "test.cfg";
    config.set("test_key", "test_value", nlm::ConfigSource::Runtime);
    assert(config.saveToFile(simpleFile));
    
    nlm::Config config3;
    assert(config3.loadFromFile(simpleFile));
    
    std::remove(simpleFile.c_str());
    
    std::cout << "    testFileFormatDetection passed" << std::endl;
}

void testConfigPresets() {
    // Test built-in presets
    auto defaultPreset = nlm::ConfigPresets::getDefaultConfig();
    assert(defaultPreset != nullptr);
    assert(defaultPreset->has("random_seed"));
    
    auto testPreset = nlm::ConfigPresets::getTestConfig();
    assert(testPreset != nullptr);
    assert(testPreset->has("neuron_count"));
    
    auto highAccuracyPreset = nlm::ConfigPresets::getHighAccuracyConfig();
    assert(highAccuracyPreset != nullptr);
    assert(highAccuracyPreset->has("neuron_count"));
    
    auto lowPowerPreset = nlm::ConfigPresets::getLowPowerConfig();
    assert(lowPowerPreset != nullptr);
    assert(lowPowerPreset->has("neuron_count"));
    
    // Test preset listing
    auto presets = nlm::ConfigPresets::listPresets();
    assert(std::find(presets.begin(), presets.end(), "Default") != presets.end());
    assert(std::find(presets.begin(), presets.end(), "FastTest") != presets.end());
    assert(std::find(presets.begin(), presets.end(), "HighAccuracy") != presets.end());
    assert(std::find(presets.begin(), presets.end(), "LowPower") != presets.end());
    
    std::cout << "    testConfigPresets passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    config.set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    config.set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config.set("log_level", std::string("INFO"), nlm::ConfigSource::Default);
    
    nlm::ConfigValidator validator;
    auto result = validator.validate(config);
    
    assert(result.isValid);
    assert(result.errors.empty());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void runAll() {
    testJSONLoad();
    testJSONSave();
    testYAMLLoad();
    testFileFormatDetection();
    testConfigPresets();
    testConfigValidation();
}

} // namespace test_json_config
