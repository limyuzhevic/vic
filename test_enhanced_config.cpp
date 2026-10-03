// Simple test to verify the enhanced Config functionality
#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "=== Testing Enhanced Config Implementation ===" << std::endl;
    
    // Test 1: Basic functionality
    std::cout << "Test 1: Basic Config operations..." << std::endl;
    nlm::Config config;
    config.set("test_int", 42);
    config.set("test_string", "hello");
    config.set("test_double", 3.14159);
    config.set("test_bool", true);
    
    assert(config.get<int64_t>("test_int") == 42);
    assert(config.get<std::string>("test_string") == "hello");
    assert(config.get<double>("test_double") > 3.141 && config.get<double>("test_double") < 3.142);
    assert(config.get<bool>("test_bool") == true);
    
    std::cout << "  PASSED" << std::endl;
    
    // Test 2: Format detection
    std::cout << "Test 2: Format detection..." << std::endl;
    std::string jsonFormat = config.detectFormat("{\"key\": \"value\"}");
    assert(jsonFormat == "JSON");
    
    std::string yamlFormat = config.detectFormat("key: value");
    assert(yamlFormat == "YAML");
    
    std::string kvFormat = config.detectFormat("key = value");
    assert(kvFormat == "KEY_VALUE");
    
    std::cout << "  PASSED" << std::endl;
    
    // Test 3: File extension detection
    std::cout << "Test 3: File extension detection..." << std::endl;
    std::string jsonExt = config.detectFormatFromExtension("test.json");
    assert(jsonExt == "JSON");
    
    std::string yamlExt = config.detectFormatFromExtension("config.yml");
    assert(yamlExt == "YAML");
    
    std::string cfgExt = config.detectFormatFromExtension("config.cfg");
    assert(cfgExt == "KEY_VALUE");
    
    std::cout << "  PASSED" << std::endl;
    
    // Test 4: Key-value parsing
    std::cout << "Test 4: Key-value parsing..." << std::endl;
    std::string kvContent = "int_val = 123\ndouble_val = 3.14\nstring_val = \"test\"\nbool_val = true";
    
    nlm::Config kvConfig;
    assert(kvConfig.loadFromString(kvContent, "KEY_VALUE"));
    
    assert(kvConfig.get<int64_t>("int_val") == 123);
    assert(std::abs(kvConfig.get<double>("double_val").value() - 3.14) < 0.0001);
    assert(kvConfig.get<std::string>("string_val") == "test");
    assert(kvConfig.get<bool>("bool_val") == true);
    
    std::cout << "  PASSED" << std::endl;
    
    // Test 5: Schema validation
    std::cout << "Test 5: Schema validation..." << std::endl;
    nlm::ConfigSchema schema;
    schema.addField("required_int", "int64", "Required integer", "^-?\d+$", true, "0");
    schema.addField("required_string", "string", "Required string", "^[a-zA-Z]+$", true, "default");
    
    nlm::Config schemaConfig(schema);
    // Add required fields
    schemaConfig.set("required_int", 42);
    schemaConfig.set("required_string", "valid_string");
    
    std::vector<std::string> errors = schemaConfig.validateAgainstSchema();
    assert(errors.empty()); // No validation errors
    
    std::cout << "  PASSED" << std::endl;
    
    // Test 6: Type conversion
    std::cout << "Test 6: Type conversion..." << std::endl;
    std::string intStr = config.toString(config.get<int64_t>("test_int"));
    assert(intStr == "42");
    
    std::string stringStr = config.toString(config.get<std::string>("test_string"));
    assert(stringStr == "\"hello\"");
    
    std::cout << "  PASSED" << std::endl;
    
    // Test 7: Serialization
    std::cout << "Test 7: Serialization..." << std::endl;
    std::string jsonOutput = config.saveToFile("", "JSON");
    assert(!jsonOutput.empty());
    
    std::string yamlOutput = config.saveToFile("", "YAML");
    assert(!yamlOutput.empty());
    
    std::string kvOutput = config.saveToFile("", "KEY_VALUE");
    assert(!kvOutput.empty());
    
    std::cout << "  PASSED" << std::endl;
    
    std::cout << "=== All Tests PASSED ===" << std::endl;
    return 0;
}
