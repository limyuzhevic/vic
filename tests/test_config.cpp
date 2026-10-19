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
    
    // Create a JSON config content
    std::string jsonContent = """{
        "simulation_timestep": 0.001,
        "neuron_count": 1000,
        "plasticity_learning_rate": 0.01,
        "visualization_enabled": false
    }""";
    
    nlm::ParseResult result = nlm::Config::parseJSON(jsonContent, config);
    assert(result.success);
    assert(result.format == "json");
    
    // Check that values were parsed correctly
    auto timestep = config.get<double>("simulation_timestep");
    assert(timestep.has_value());
    assert(*timestep == 0.001);
    
    auto neuronCount = config.get<int64_t>("neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 1000);
    
    auto learningRate = config.get<double>("plasticity_learning_rate");
    assert(learningRate.has_value());
    assert(*learningRate == 0.01);
    
    auto vizEnabled = config.get<bool>("visualization_enabled");
    assert(vizEnabled.has_value());
    assert(*vizEnabled == false);
    
    std::cout << "    testJSONParsing passed" << std::endl;
}

void testYAMLParsing() {
    nlm::Config config;
    
    // Create a YAML config content
    std::string yamlContent = """---
dopamine_baseline: 0.1
reward_discount_factor: 0.99
environment_name: GridWorld
visualization_enabled: false
""";
    
    nlm::ParseResult result = nlm::Config::parseYAML(yamlContent, config);
    assert(result.success);
    assert(result.format == "yaml");
    
    // Check that values were parsed correctly
    auto dopamine = config.get<double>("dopamine_baseline");
    assert(dopamine.has_value());
    assert(*dopamine == 0.1);
    
    auto discount = config.get<double>("reward_discount_factor");
    assert(discount.has_value());
    assert(*discount == 0.99);
    
    auto envName = config.get<std::string>("environment_name");
    assert(envName.has_value());
    assert(*envName == "GridWorld");
    
    auto vizEnabled = config.get<bool>("visualization_enabled");
    assert(vizEnabled.has_value());
    assert(*vizEnabled == false);
    
    std::cout << "    testYAMLParsing passed" << std::endl;
}

void testKeyValueParsing() {
    nlm::Config config;
    
    // Create a key=value config content
    std::string kvContent = """random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
visualization_enabled = false
""";
    
    bool result = config.loadFromKeyValue(kvContent);
    assert(result);
    
    // Check that values were parsed correctly
    auto seed = config.get<int64_t>("random_seed");
    assert(seed.has_value());
    assert(*seed == 42);
    
    auto timestep = config.get<double>("simulation_timestep");
    assert(timestep.has_value());
    assert(*timestep == 0.001);
    
    auto neuronCount = config.get<int64_t>("neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 1000);
    
    auto vizEnabled = config.get<bool>("visualization_enabled");
    assert(vizEnabled.has_value());
    assert(*vizEnabled == false);
    
    std::cout << "    testKeyValueParsing passed" << std::endl;
}

void testFileFormatDetection() {
    // Test JSON format detection
    std::string jsonContent = "{\"key\": \"value\"}";
    std::ofstream jsonFile("test_json.json");
    jsonFile << jsonContent;
    jsonFile.close();
    
    std::string format;
    bool detected = nlm::Config::detectFileFormat("test_json.json", format);
    assert(detected);
    assert(format == "json");
    
    // Test key=value format detection
    std::string kvContent = "key = value";
    std::ofstream kvFile("test_kv.cfg");
    kvFile << kvContent;
    kvFile.close();
    
    detected = nlm::Config::detectFileFormat("test_kv.cfg", format);
    assert(detected);
    assert(format == "json" || format == "key=value");
    
    // Clean up test files
    std::filesystem::remove("test_json.json");
    std::filesystem::remove("test_kv.cfg");
    
    std::cout << "    testFileFormatDetection passed" << std::endl;
}

void testSaveToFile() {
    nlm::Config config;
    
    // Set some test values
    config.set("random_seed", 42, nlm::ConfigSource::File);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::File);
    config.set("neuron_count", 1000, nlm::ConfigSource::File);
    config.set("visualization_enabled", false, nlm::ConfigSource::File);
    config.set("dopamine_baseline", 0.1, nlm::ConfigSource::File);
    
    // Save to file
    bool result = config.saveToFile("test_config.cfg");
    assert(result);
    
    // Read back and verify values
    nlm::Config config2;
    bool loaded = config2.loadFromFile("test_config.cfg");
    assert(loaded);
    
    auto seed = config2.get<int64_t>("random_seed");
    assert(seed.has_value());
    assert(*seed == 42);
    
    auto timestep = config2.get<double>("simulation_timestep");
    assert(timestep.has_value());
    assert(*timestep == 0.001);
    
    auto neuronCount = config2.get<int64_t>("neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 1000);
    
    auto vizEnabled = config2.get<bool>("visualization_enabled");
    assert(vizEnabled.has_value());
    assert(*vizEnabled == false);
    
    auto dopamine = config2.get<double>("dopamine_baseline");
    assert(dopamine.has_value());
    assert(*dopamine == 0.1);
    
    // Clean up
    std::filesystem::remove("test_config.cfg");
    
    std::cout << "    testSaveToFile passed" << std::endl;
}

void testValidation() {
    nlm::Config config;
    
    // Add schema for required fields
    nlm::ConfigSchema schema1("random_seed", true, "Random seed for reproducible experiments");
    schema1.addValidator([](const nlm::ConfigValue& value) {
        if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value) > 0;
        }
        return false;
    });
    config.addSchema(schema1);
    
    nlm::ConfigSchema schema2("simulation_timestep", true, "Simulation timestep in seconds");
    schema2.addValidator([](const nlm::ConfigValue& value) {
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) > 0.0 && std::get<double>(value) <= 0.1;
        }
        return false;
    });
    config.addSchema(schema2);
    
    // Test validation with missing required fields
    config.clear();
    bool valid = config.validate();
    assert(!valid);
    
    std::string errors = config.getValidationErrors();
    assert(!errors.empty());
    
    // Set valid values
    config.set("random_seed", 42, nlm::ConfigSource::File);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::File);
    
    valid = config.validate();
    assert(valid);
    
    // Test validation with invalid values
    config.clear();
    config.set("random_seed", 42, nlm::ConfigSource::File);
    config.set("simulation_timestep", 1.0, nlm::ConfigSource::File); // Invalid: too large
    
    valid = config.validate();
    assert(!valid);
    
    std::cout << "    testValidation passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testJSONParsing();
    testYAMLParsing();
    testKeyValueParsing();
    testFileFormatDetection();
    testSaveToFile();
    testValidation();
}

} // namespace test_config
