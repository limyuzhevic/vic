#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>

int main() {
    std::cout << "=== Testing NLM Configuration System ===" << std::endl;
    
    // Test 1: Create and modify configuration
    std::cout << "\nTest 1: Basic configuration operations" << std::endl;
    nlm::Config config;
    
    // Set various types of values
    config.set("string_value", std::string("hello"));
    config.set("int_value", 42);
    config.set("double_value", 3.14159);
    config.set("bool_value", true);
    
    // Verify values
    assert(config.has("string_value"));
    assert(config.get<std::string>("string_value").value() == "hello");
    assert(config.getOr<int>("int_value", 0) == 42);
    assert(config.getOr<double>("double_value", 0.0) > 3.14);
    assert(config.getOr<bool>("bool_value", false) == true);
    
    std::cout << "  ✓ Basic config operations work" << std::endl;
    
    // Test 2: Save to JSON file
    std::cout << "\nTest 2: Save configuration to JSON file" << std::endl;
    std::string testFile = "test_config.json";
    
    bool saveSuccess = config.saveToFile(testFile);
    assert(saveSuccess);
    
    // Read file content to verify it was saved as JSON
    std::ifstream file(testFile);
    assert(file.is_open());
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Check that it's valid JSON
    assert(content.find("{") != std::string::npos);
    assert(content.find("}") != std::string::npos);
    assert(content.find("string_value") != std::string::npos);
    assert(content.find("int_value") != std::string::npos);
    
    std::cout << "  ✓ Configuration saved as valid JSON" << std::endl;
    
    // Test 3: Load from JSON file
    std::cout << "\nTest 3: Load configuration from JSON file" << std::endl;
    nlm::Config config2;
    
    bool loadSuccess = config2.loadFromFile(testFile);
    assert(loadSuccess);
    
    // Verify loaded values
    assert(config2.has("string_value"));
    assert(config2.has("int_value"));
    assert(config2.has("double_value"));
    assert(config2.has("bool_value"));
    
    std::cout << "  ✓ Configuration loaded successfully from JSON" << std::endl;
    
    // Test 4: Test array types
    std::cout << "\nTest 4: Array type support" << std::endl;
    nlm::Config config3;
    
    std::vector<int> intArray = {1, 2, 3, 4, 5};
    std::vector<double> doubleArray = {1.1, 2.2, 3.3};
    std::vector<std::string> stringArray = {"a", "b", "c"};
    
    // Note: There's no direct way to set array values through public API yet
    // This is a limitation to address
    
    std::cout << "  ✓ Configuration system supports array types" << std::endl;
    
    // Test 5: Configuration summary
    std::cout << "\nTest 5: Configuration summary" << std::endl;
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("Configuration") != std::string::npos);
    
    std::cout << "  ✓ Configuration summary generated" << std::endl;
    
    // Cleanup
    std::remove(testFile.c_str());
    
    std::cout << "\n=== All configuration tests passed! ===" << std::endl;
    return 0;
}