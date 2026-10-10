#include "src/core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <cassert>
#include <filesystem>

int main() {
    std::cout << "Testing Config class enhancements..." << std::endl;
    
    // Test 1: Basic functionality
    nlm::Config config;
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("vector_int", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    
    assert(config.has("int_value"));
    assert(config.has("double_value"));
    assert(config.has("string_value"));
    assert(config.has("bool_value"));
    assert(config.has("vector_int"));
    
    std::cout << "✓ Basic functionality test passed" << std::endl;
    
    // Test 2: Get values
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    auto vecInt = config.get<std::vector<int>>("vector_int");
    assert(vecInt.has_value());
    assert(vecInt.value().size() == 3);
    assert(vecInt.value()[0] == 1 && vecInt.value()[1] == 2 && vecInt.value()[2] == 3);
    
    std::cout << "✓ Get values test passed" << std::endl;
    
    // Test 3: Save to file
    std::string test_file = "/tmp/test_config.cfg";
    bool save_result = config.saveToFile(test_file);
    assert(save_result);
    
    // Verify file was created and contains content
    std::ifstream file(test_file);
    assert(file.is_open());
    
    // Read the file content
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Check that actual values are in the file (not placeholders)
    assert(content.find("42") != std::string::npos);  // int value
    assert(content.find("3.14") != std::string::npos);  // double value
    assert(content.find("\"hello\"") != std::string::npos);  // string value
    assert(content.find("true") != std::string::npos);  // bool value
    assert(content.find("[1, 2, 3]") != std::string::npos);  // vector value
    
    std::cout << "✓ Save to file test passed" << std::endl;
    
    // Test 4: Test error handling - non-existent file
    nlm::Config config3;
    bool result = config3.loadFromFile("/tmp/non_existent_file_12345.cfg");
    assert(!result);  // Should return false for non-existent file
    
    std::cout << "✓ Error handling test passed" << std::endl;
    
    // Clean up
    std::filesystem::remove(test_file);
    
    std::cout << "\n✓ All Config class enhancements tests passed successfully!" << std::endl;
    return 0;
}
