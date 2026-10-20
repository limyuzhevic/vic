// Test program demonstrating the new JSON/YAML configuration system
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <cassert>

void testJsonSerialization() {
    std::cout << "Testing JSON serialization..." << std::endl;
    
    nlm::Config config;
    
    // Set some values
    config.set("int_value", 42);
    config.set("double_value", 3.14159);
    config.set("string_value", std::string("hello"));
    config.set("bool_value", true);
    config.set("int_array", std::vector<int>({1, 2, 3}));
    config.set("double_array", std::vector<double>({1.5, 2.5, 3.5}));
    config.set("string_array", std::vector<std::string>({"a", "b", "c"}));
    
    // Test nested access
    config.setNested("brain.neuron_count", 1000);
    config.setNested("plasticity.stdp.enable", true);
    
    // Serialize to JSON
    std::string jsonString;
    assert(config.saveToJson(jsonString));
    
    std::cout << "Generated JSON:" << std::endl;
    std::cout << jsonString << std::endl;
    
    // Parse JSON back
    nlm::Config config2;
    assert(config2.loadFromJson(jsonString));
    
    // Verify values
    assert(config2.get<int>("int_value").value() == 42);
    assert(config2.get<double>("double_value").value() > 3.14 && config2.get<double>("double_value").value() < 3.15);
    assert(config2.get<std::string>("string_value").value() == "hello");
    assert(config2.get<bool>("bool_value").value() == true);
    assert(config2.getNested("brain.neuron_count").value() == 1000);
    assert(config2.getNested("plasticity.stdp.enable").value() == true);
    
    std::cout << "JSON serialization test passed!" << std::endl;
}

void testYamlSerialization() {
    std::cout << "Testing YAML serialization..." << std::endl;
    
    nlm::Config config;
    
    // Set some values
    config.set("int_value", 100);
    config.set("name", std::string("test_config"));
    config.set("enabled", false);
    
    // Serialize to YAML
    std::string yamlString;
    assert(config.saveToYaml(yamlString));
    
    std::cout << "Generated YAML:" << std::endl;
    std::cout << yamlString << std::endl;
    
    // Parse YAML back
    nlm::Config config2;
    assert(config2.loadFromYaml(yamlString));
    
    // Verify values
    assert(config2.get<int>("int_value").value() == 100);
    assert(config2.get<std::string>("name").value() == "test_config");
    assert(config2.get<bool>("enabled").value() == false);
    
    std::cout << "YAML serialization test passed!" << std::endl;
}

void testFileFormatDetection() {
    std::cout << "Testing file format detection..." << std::endl;
    
    nlm::Config config;
    
    // Test simple config file (backward compatibility)
    std::ofstream simpleFile("test_simple.cfg");
    simpleFile << "# Simple config file\n";
    simpleFile << "key1 = value1\n";
    simpleFile << "key2 = 42\n";
    simpleFile << "key3 = 3.14\n";
    simpleFile << "key4 = true\n";
    simpleFile.close();
    
    assert(config.loadFromFile("test_simple.cfg"));
    assert(config.has("key1"));
    assert(config.has("key2"));
    assert(config.has("key3"));
    assert(config.has("key4"));
    
    std::cout << "Simple config file test passed!" << std::endl;
    
    // Clean up
    std::remove("test_simple.cfg");
    std::cout << "File format detection test passed!" << std::endl;
}

void testSchemaValidation() {
    std::cout << "Testing schema validation..." << std::endl;
    
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    // Define a simple schema
    nlm::ConfigSchema::ConfigProperty prop1;
    prop1.name = "int_prop";
    prop1.type = "int";
    prop1.defaultValue = nlm::ConfigValue(0);
    prop1.description = "An integer property";
    
    auto validator = [](const std::string& key, const nlm::ConfigValue& value) -> bool {
        if (key == "int_prop") {
            return std::get<int>(value) >= 0 && std::get<int>(value) <= 100;
        }
        return true;
    };
    prop1.validator = validator;
    
    schema.addProperty(prop1);
    config.setSchema(schema);
    
    // Valid value
    config.set("int_prop", 50);
    assert(config.validate());
    
    // Invalid value
    config.set("int_prop", 150);
    assert(!config.validate());
    
    std::cout << "Schema validation test passed!" << std::endl;
}

void testBackwardCompatibility() {
    std::cout << "Testing backward compatibility..." << std::endl;
    
    nlm::Config config;
    
    // Test loading existing simple config file
    assert(config.loadFromFile("/workspace/app/configs/default.cfg"));
    
    // Verify some key values from the config
    assert(config.has("random_seed"));
    assert(config.has("neuron_count"));
    assert(config.has("simulation_timestep"));
    
    std::cout << "Backward compatibility test passed!" << std::endl;
}

int main() {
    std::cout << "=== NLM Configuration System Test ===" << std::endl;
    
    try {
        testJsonSerialization();
        testYamlSerialization();
        testFileFormatDetection();
        testSchemaValidation();
        testBackwardCompatibility();
        
        std::cout << std::endl;
        std::cout << "=== All tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}