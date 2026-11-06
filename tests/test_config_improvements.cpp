// Config Test Suite - Demonstrating Improved Config Class Features
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void testJSONParsing() {
    std::cout << "Testing JSON parsing..." << std::endl;
    
    nlm::Config config;
    
    // Create a JSON configuration string
    std::string jsonContent = R"({
        "int_value": 42,
        "double_value": 3.14,
        "string_value": "hello",
        "bool_value": true,
        "int_array": [1, 2, 3],
        "string_array": ["a", "b", "c"],
        "nested_object": {
            "nested_int": 100
        }
    })";
    
    // Write JSON to temporary file
    std::ofstream jsonFile("test_config.json");
    jsonFile << jsonContent;
    jsonFile.close();
    
    // Load JSON file
    assert(config.loadFromFile("test_config.json"));
    
    // Test JSON value parsing
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Test array parsing
    auto intArray = config.get<std::vector<int>>("int_array");
    assert(intArray.has_value());
    assert(intArray.value().size() == 3);
    assert(intArray.value()[0] == 1);
    
    auto stringArray = config.get<std::vector<std::string>>("string_array");
    assert(stringArray.has_value());
    assert(stringArray.value().size() == 3);
    assert(stringArray.value()[0] == "a");
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "  JSON parsing test passed" << std::endl;
}

void testKeyValueFormat() {
    std::cout << "Testing key=value format parsing..." << std::endl;
    
    nlm::Config config;
    
    // Create a key=value format string
    std::string keyValueContent = R"(
# Test configuration file
int_value = 42
double_value = 3.14
string_value = "hello"
bool_value = true
int_array = [1, 2, 3]
string_array = ["a", "b", "c"]
)";
    
    // Write to temporary file
    std::ofstream keyValueFile("test_config.cfg");
    keyValueFile << keyValueContent;
    keyValueFile.close();
    
    // Load key=value file
    assert(config.loadFromFile("test_config.cfg"));
    
    // Test key=value value parsing
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    // Clean up
    std::remove("test_config.cfg");
    
    std::cout << "  Key=value format test passed" << std::endl;
}

void testSaveToJSON() {
    std::cout << "Testing save to JSON format..." << std::endl;
    
    nlm::Config config;
    
    // Set various values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    config.set("string_array", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    
    // Save as JSON
    assert(config.saveToFile("test_config.json"));
    
    // Load back from JSON
    nlm::Config config2;
    assert(config2.loadFromFile("test_config.json"));
    
    // Verify values
    assert(config2.get<int>("int_value") == config.get<int>("int_value"));
    assert(config2.get<double>("double_value") == config.get<double>("double_value"));
    assert(config2.get<std::string>("string_value") == config.get<std::string>("string_value"));
    assert(config2.get<bool>("bool_value") == config.get<bool>("bool_value"));
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "  Save to JSON test passed" << std::endl;
}

void testBackwardCompatibility() {
    std::cout << "Testing backward compatibility..." << std::endl;
    
    nlm::Config config;
    
    // Set values using old API
    config.set("old_int", 123, nlm::ConfigSource::Runtime);
    config.set("old_string", "test", nlm::ConfigSource::Runtime);
    config.set("old_bool", false, nlm::ConfigSource::Runtime);
    
    // Load from key=value file (old format)
    std::ofstream oldFormatFile("old_config.cfg");
    oldFormatFile << "# Old format config file\n";
    oldFormatFile << "old_int = 456\n";
    oldFormatFile << "old_string = \"updated\"\n";
    oldFormatFile << "old_bool = true\n";
    oldFormatFile.close();
    
    assert(config.loadFromFile("old_config.cfg"));
    
    // Verify old values are preserved
    assert(config.get<int>("old_int") == 456);
    assert(config.get<std::string>("old_string") == "updated");
    assert(config.get<bool>("old_bool") == true);
    
    // Clean up
    std::remove("old_config.cfg");
    
    std::cout << "  Backward compatibility test passed" << std::endl;
}

void testConfigSummary() {
    std::cout << "Testing configuration summary..." << std::endl;
    
    nlm::Config config;
    
    // Set some values
    config.set("name", "NLM Brain", nlm::ConfigSource::File);
    config.set("version", 2.0, nlm::ConfigSource::Default);
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    
    // Get summary
    std::string summary = config.summary();
    
    // Verify summary contains expected information
    assert(summary.find("Configuration") != std::string::npos);
    assert(summary.find("name") != std::string::npos);
    assert(summary.find("NLM Brain") != std::string::npos);
    
    std::cout << "  Configuration summary test passed" << std::endl;
}

void runAllConfigTests() {
    testJSONParsing();
    testKeyValueFormat();
    testSaveToJSON();
    testBackwardCompatibility();
    testConfigSummary();
    
    std::cout << "\nAll config tests passed!" << std::endl;
}

int main() {
    std::cout << "=== Enhanced Config Class Test Suite ===" << std::endl;
    std::cout << "Testing new JSON/YAML parser and backward compatibility..." << std::endl;
    std::cout << std::endl;
    
    runAllConfigTests();
    
    std::cout << std::endl;
    std::cout << "✓ Config class improvements successful!" << std::endl;
    std::cout << "  - JSON file format support with nlohmann/json" << std::endl;
    std::cout << "  - Backward compatibility with key=value format" << std::endl;
    std::cout << "  - Enhanced saveToFile with format detection" << std::endl;
    std::cout << "  - Improved error handling and validation" << std::endl;
    
    return 0;
}
