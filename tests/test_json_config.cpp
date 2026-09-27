// JSON Configuration Tests
// Tests for the new JSON configuration parser in Config.cpp

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_json_config {

void testJsonLoad() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::string testFile = "test_config.json";
    std::ofstream file(testFile);
    file << "{\n";
    file << "  \"int_value\": 42,\n";
    file << "  \"double_value\": 3.14,\n";
    file << "  \"string_value\": \"hello world\",\n";
    file << "  \"bool_value\": true,\n";
    file << "  \"array_int\": [1, 2, 3, 4, 5]\n";
    file << "}\n";
    file.close();
    
    // Load JSON file
    bool result = config.loadFromFile(testFile);
    assert(result == true);
    
    // Check values
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello world");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    auto arrayVal = config.get<std::vector<int>>("array_int");
    assert(arrayVal.has_value());
    assert(arrayVal->size() == 5);
    assert((*arrayVal)[0] == 1);
    assert((*arrayVal)[4] == 5);
    
    std::cout << "    testJsonLoad passed" << std::endl;
    
    // Clean up
    std::remove(testFile.c_str());
}

void testJsonFallback() {
    nlm::Config config;
    
    // Create a simple key=value file (not JSON)
    std::string testFile = "test_config.txt";
    std::ofstream file(testFile);
    file << "# Simple config file\n";
    file << "int_value = 123\n";
    file << "string_value = test\n";
    file << "# Comment line\n";
    file << "double_value = 9.87\n";
    file.close();
    
    // Load as JSON (should fallback to simple format)
    bool result = config.loadFromFile(testFile);
    assert(result == true);
    
    // Check values were loaded
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 123);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "test");
    
    std::cout << "    testJsonFallback passed" << std::endl;
    
    // Clean up
    std::remove(testFile.c_str());
}

void testJsonInvalidFile() {
    nlm::Config config;
    
    // Try to load non-existent file
    bool result = config.loadFromFile("non_existent.json");
    assert(result == false);
    
    std::cout << "    testJsonInvalidFile passed" << std::endl;
}

void runAll() {
    testJsonLoad();
    testJsonFallback();
    testJsonInvalidFile();
}

} // namespace test_json_config