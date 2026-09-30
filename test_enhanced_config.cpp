// Simple test program for enhanced Config class
// This demonstrates the new JSON/YAML parsing, error handling, and advanced features

#include "src/core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <map>

void testBasicFunctionality() {
    std::cout << "Testing basic Config functionality..." << std::endl;
    
    nlm::Config config;
    
    // Test setting and getting values
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", "hello");
    config.set("bool_value", true);
    
    assert(config.has("int_value"));
    assert(config.has("double_value"));
    assert(config.has("string_value"));
    assert(config.has("bool_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(std::abs(*doubleVal - 3.14) < 0.001);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Test getOr
    int missing = config.getOr<int>("missing", 100);
    assert(missing == 100);
    
    int present = config.getOr<int>("int_value", 999);
    assert(present == 42);
    
    // Test remove
    config.remove("string_value");
    assert(!config.has("string_value"));
    
    // Test keys
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "✓ Basic functionality tests passed" << std::endl;
}

void testJSONParsing() {
    std::cout << "Testing JSON parsing..." << std::endl;
    
    // Create a temporary JSON file
    std::ofstream jsonFile("test_config.json");
    jsonFile << "{\n"
             << "    \"brain.neuron_count\": 100,\n"
             << "    \"brain.synaptic_weight\": 0.5,\n"
             << "    \"brain.enable_learning\": true,\n"
             << "    \"brain.regions\": [\n"
             << "        1,\n"
             << "        2,\n"
             << "        3\n"
             << "    ],\n"
             << "    \"brain.settings\": {\n"
             << "        \"temperature\": 37.0,\n"
             << "        \"pressure\": 1.0\n"
             << "    }\n"
             << "}";
    jsonFile.close();
    
    // Load and test
    nlm::Config config;
    config.loadFromFile("test_config.json");
    
    assert(config.has("brain.neuron_count"));
    assert(config.has("brain.synaptic_weight"));
    assert(config.has("brain.enable_learning"));
    assert(config.has("brain.regions"));
    assert(config.has("brain.settings"));
    
    auto neuronCount = config.get<int>("brain.neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 100);
    
    auto synapticWeight = config.get<double>("brain.synaptic_weight");
    assert(synapticWeight.has_value() && std::abs(*synapticWeight - 0.5) < 0.001);
    
    auto learningEnabled = config.get<bool>("brain.enable_learning");
    assert(learningEnabled.has_value() && *learningEnabled == true);
    
    // Test array value
    auto regions = config.get<std::vector<int>>("brain.regions");
    assert(regions.has_value() && regions->size() == 3);
    
    // Test object value
    auto settings = config.get<std::map<std::string, double>>("brain.settings");
    assert(settings.has_value() && settings->size() == 2);
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "✓ JSON parsing tests passed" << std::endl;
}

void testYAMLParsing() {
    std::cout << "Testing YAML parsing..." << std::endl;
    
    // Create a temporary YAML file
    std::ofstream yamlFile("test_config.yaml");
    yamlFile << "brain:\n"
             << "  neuron_count: 200\n"
             << "  synaptic_weight: 0.3\n"
             << "  enable_learning: false\n"
             << "  regions:\n"
             << "    - 10\n"
             << "    - 20\n"
             << "  settings:\n"
             << "    temperature: 36.5\n"
             << "    pressure: 0.9\n";
    yamlFile.close();
    
    // Load and test
    nlm::Config config;
    try {
        config.loadFromFile("test_config.yaml");
        
        assert(config.has("brain.neuron_count"));
        assert(config.has("brain.synaptic_weight"));
        assert(config.has("brain.enable_learning"));
        
        auto neuronCount = config.get<int>("brain.neuron_count");
        assert(neuronCount.has_value() && *neuronCount == 200);
        
        auto synapticWeight = config.get<double>("brain.synaptic_weight");
        assert(synapticWeight.has_value() && std::abs(*synapticWeight - 0.3) < 0.001);
        
        auto learningEnabled = config.get<bool>("brain.enable_learning");
        assert(learningEnabled.has_value() && *learningEnabled == false);
        
    } catch (const nlm::ConfigError& e) {
        std::cout << "YAML support not available: " << e.what() << std::endl;
        // This is expected if yaml-cpp is not available
    }
    
    // Clean up
    std::remove("test_config.yaml");
    
    std::cout << "✓ YAML parsing tests passed (or skipped if yaml-cpp not available)" << std::endl;
}

void testJSONSaving() {
    std::cout << "Testing JSON saving..." << std::endl;
    
    nlm::Config config;
    config.set("brain.neuron_count", 150);
    config.set("brain.synaptic_weight", 0.7);
    config.set("brain.enable_learning", true);
    config.set("brain.regions", std::vector<int>({1, 2, 3, 4, 5}));
    
    // Save as JSON
    config.saveToFile("test_config_save.json", "json");
    
    // Load it back
    nlm::Config config2;
    config2.loadFromFile("test_config_save.json");
    
    assert(config2.has("brain.neuron_count"));
    assert(config2.has("brain.synaptic_weight"));
    assert(config2.has("brain.enable_learning"));
    assert(config2.has("brain.regions"));
    
    auto neuronCount = config2.get<int>("brain.neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 150);
    
    // Clean up
    std::remove("test_config_save.json");
    
    std::cout << "✓ JSON saving tests passed" << std::endl;
}

void testSimpleFormat() {
    std::cout << "Testing simple format (backward compatibility)..." << std::endl;
    
    // Create a temporary ini-style file
    std::ofstream simpleFile("test_config.ini");
    simpleFile << "# Test configuration\n"
               << "brain.neuron_count = 300\n"
               << "brain.synaptic_weight = 0.9\n"
               << "brain.enable_learning = true\n"
               << "brain.max_firing_rate = 250\n";
    simpleFile.close();
    
    nlm::Config config;
    config.loadFromFile("test_config.ini");
    
    assert(config.has("brain.neuron_count"));
    assert(config.has("brain.synaptic_weight"));
    assert(config.has("brain.enable_learning"));
    assert(config.has("brain.max_firing_rate"));
    
    auto neuronCount = config.get<int>("brain.neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 300);
    
    auto firingRate = config.get<int>("brain.max_firing_rate");
    assert(firingRate.has_value() && *firingRate == 250);
    
    // Clean up
    std::remove("test_config.ini");
    
    std::cout << "✓ Simple format tests passed" << std::endl;
}

void testTypeConversion() {
    std::cout << "Testing type conversion..." << std::endl;
    
    nlm::Config config;
    
    // Test setWithType
    config.setWithType("int_from_string", "42", "int");
    config.setWithType("double_from_string", "3.14", "double");
    config.setWithType("bool_from_string", "true", "bool");
    
    auto intVal = config.get<int>("int_from_string");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("double_from_string");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14) < 0.001);
    
    auto boolVal = config.get<bool>("bool_from_string");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test automatic type conversion
    config.set("auto_int", "123");
    config.set("auto_double", "45.67");
    config.set("auto_bool", "false");
    
    auto autoInt = config.get<int>("auto_int");
    assert(autoInt.has_value() && *autoInt == 123);
    
    auto autoDouble = config.get<double>("auto_double");
    assert(autoDouble.has_value() && std::abs(*autoDouble - 45.67) < 0.001);
    
    auto autoBool = config.get<bool>("auto_bool");
    assert(autoBool.has_value() && *autoBool == false);
    
    std::cout << "✓ Type conversion tests passed" << std::endl;
}

void testConfigurationComparison() {
    std::cout << "Testing configuration comparison..." << std::endl;
    
    nlm::Config config1;
    config1.set("brain.neuron_count", 100);
    config1.set("brain.synaptic_weight", 0.5);
    config1.set("brain.enable_learning", true);
    
    nlm::Config config2;
    config2.set("brain.neuron_count", 100);
    config2.set("brain.synaptic_weight", 0.5);
    config2.set("brain.enable_learning", true);
    
    nlm::Config config3;
    config3.set("brain.neuron_count", 200);
    config3.set("brain.synaptic_weight", 0.5);
    config3.set("brain.enable_learning", false);
    
    // Identical configs should have no differences
    auto diff1 = nlm::Config::compare(config1, config2);
    assert(diff1.empty());
    
    // Different configs should have differences
    auto diff2 = nlm::Config::compare(config1, config3);
    assert(!diff2.empty());
    assert(diff2.find("brain.neuron_count") != diff2.end());
    assert(diff2.find("brain.enable_learning") != diff2.end());
    
    // Test hasChanged
    assert(!config1.hasChanged(config2)); // Same
    assert(config1.hasChanged(config3)); // Different
    
    std::cout << "✓ Configuration comparison tests passed" << std::endl;
}

void testConfigurationMerging() {
    std::cout << "Testing configuration merging..." << std::endl;
    
    nlm::Config base;
    base.set("brain.neuron_count", 100);
    base.set("brain.synaptic_weight", 0.5);
    base.set("brain.enable_learning", true);
    
    nlm::Config override;
    override.set("brain.neuron_count", 200); // Override
    override.set("brain.enable_development", true); // Add new
    
    auto merged = base.mergeWith(override, true);
    
    assert(merged.has("brain.neuron_count"));
    assert(merged.has("brain.synaptic_weight"));
    assert(merged.has("brain.enable_learning"));
    assert(merged.has("brain.enable_development"));
    
    auto neuronCount = merged.get<int>("brain.neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 200);
    
    auto development = merged.get<bool>("brain.enable_development");
    assert(development.has_value() && *development == true);
    
    std::cout << "✓ Configuration merging tests passed" << std::endl;
}

void testConfigurationClone() {
    std::cout << "Testing configuration cloning..." << std::endl;
    
    nlm::Config original;
    original.set("brain.neuron_count", 300);
    original.set("brain.synaptic_weight", 0.8);
    
    auto clone = original.clone();
    
    assert(clone->has("brain.neuron_count"));
    assert(clone->has("brain.synaptic_weight"));
    
    auto neuronCount = clone->get<int>("brain.neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 300);
    
    // Modify clone, original should be unaffected
    clone->set("brain.neuron_count", 400);
    
    auto originalNeuronCount = original.get<int>("brain.neuron_count");
    assert(originalNeuronCount.has_value() && *originalNeuronCount == 300);
    
    std::cout << "✓ Configuration cloning tests passed" << std::endl;
}

void testConfigurationDictionaryConversion() {
    std::cout << "Testing configuration dictionary conversion..." << std::endl;
    
    nlm::Config config;
    config.set("brain.neuron_count", 500);
    config.set("brain.synaptic_weight", 0.2);
    config.set("brain.enable_learning", true);
    
    auto dict = config.toDictionary();
    
    assert(dict.find("brain.neuron_count") != dict.end());
    assert(dict.find("brain.synaptic_weight") != dict.end());
    assert(dict.find("brain.enable_learning") != dict.end());
    
    assert(dict["brain.neuron_count"] == "500");
    assert(dict["brain.synaptic_weight"] == "0.200000"); // Note: formatting
    assert(dict["brain.enable_learning"] == "true");
    
    std::cout << "✓ Configuration dictionary conversion tests passed" << std::endl;
}

void testErrorHandling() {
    std::cout << "Testing error handling..." << std::endl;
    
    try {
        nlm::Config config;
        config.set("brain.neuron_count", 100);
        
        // Try to get non-existent key
        auto value = config.get<int>("nonexistent");
        assert(!value.has_value());
        
        // Try to get with wrong type
        auto stringVal = config.get<std::string>("brain.neuron_count");
        assert(!stringVal.has_value());
        
        // Test error message formatting
        nlm::ConfigError error("Test error message");
        std::string formatted = config.formatError(error);
        assert(formatted.find("Test error message") != std::string::npos);
        
    } catch (...) {
        // Should not throw
        assert(false);
    }
    
    std::cout << "✓ Error handling tests passed" << std::endl;
}

void testFileFormatDetection() {
    std::cout << "Testing file format detection..." << std::endl;
    
    nlm::Config config;
    
    // Test extension-based detection
    std::string jsonFormat = config.detectFileFormat("test.json");
    assert(jsonFormat == "json");
    
    std::string yamlFormat = config.detectFileFormat("config.yaml");
    assert(yamlFormat == "yaml");
    
    std::string ymlFormat = config.detectFileFormat("config.yml");
    assert(ymlFormat == "yml");
    
    std::string iniFormat = config.detectFileFormat("settings.ini");
    assert(iniFormat == "ini");
    
    std::string defaultFormat = config.detectFileFormat("unknown_file");
    assert(defaultFormat == "ini"); // Default to ini
    
    std::cout << "✓ File format detection tests passed" << std::endl;
}

int main() {
    std::cout << "=== Enhanced Config Class Tests ===" << std::endl;
    
    try {
        testBasicFunctionality();
        testJSONParsing();
        testYAMLParsing();
        testJSONSaving();
        testSimpleFormat();
        testTypeConversion();
        testConfigurationComparison();
        testConfigurationMerging();
        testConfigurationClone();
        testConfigurationDictionaryConversion();
        testErrorHandling();
        testFileFormatDetection();
        
        std::cout << std::endl;
        std::cout << "=== All tests passed! ===" << std::endl;
        std::cout << "The enhanced Config class is working correctly." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception." << std::endl;
        return 1;
    }
    
    return 0;
}
