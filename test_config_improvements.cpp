// Test improved configuration system
#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <filesystem>

void testConfigImprovements() {
    std::cout << "Testing improved configuration system..." << std::endl;
    
    // Test 1: Basic config creation
    nlm::Config config;
    assert(!config.has("test"));
    
    // Test 2: Type-based configuration
    config.set("integer", 42);
    config.set("double", 3.14159);
    config.set("boolean", true);
    config.set("string", "hello world");
    
    assert(config.get<int>("integer").value() == 42);
    assert(config.get<double>("double").value() > 3.14 && config.get<double>("double").value() < 3.142);
    assert(config.get<bool>("boolean").value() == true);
    assert(config.get<std::string>("string").value() == "hello world");
    
    // Test 3: getOr method
    assert(config.getOr<int>("missing", 100) == 100);
    assert(config.getOr<int>("integer", 999) == 42);
    
    // Test 4: Test save/load functionality
    std::string testFile = "/tmp/test_config.cfg";
    bool saveResult = config.saveToFile(testFile);
    assert(saveResult);
    
    // Test 5: Create new config and load from file
    nlm::Config config2;
    assert(config2.loadFromFile(testFile));
    
    // Test 6: Verify loaded values
    assert(config2.has("integer"));
    assert(config2.has("double"));
    assert(config2.has("boolean"));
    assert(config2.has("string"));
    
    // Test 7: Test with JSON extension
    std::string jsonFile = "/tmp/test_config.json";
    assert(config.saveToFile(jsonFile));
    
    nlm::Config config3;
    assert(config3.loadFromFile(jsonFile));
    
    // Test 8: Test string parsing
    nlm::Config config4;
    config4.set("value", "123", nlm::ConfigSource::Runtime);
    assert(config4.get<int>("value").value() == 123);
    
    config4.set("value", "3.14", nlm::ConfigSource::Runtime);
    assert(std::abs(config4.get<double>("value").value() - 3.14) < 0.001);
    
    config4.set("value", "true", nlm::ConfigSource::Runtime);
    assert(config4.get<bool>("value").value() == true);
    
    config4.set("value", "false", nlm::ConfigSource::Runtime);
    assert(config4.get<bool>("value").value() == false);
    
    std::cout << "✓ All configuration improvement tests passed!" << std::endl;
    
    // Cleanup
    std::error_code ec;
    std::filesystem::remove(testFile, ec);
    std::filesystem::remove(jsonFile, ec);
}

int main() {
    testConfigImprovements();
    return 0;
}
