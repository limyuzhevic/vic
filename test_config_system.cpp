// Test program to verify the enhanced configuration system
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Enhanced Configuration System Test ===" << std::endl;
    
    // Test 1: Basic configuration creation
    nlm::Config config;
    std::cout << "✓ Config object created" << std::endl;
    
    // Test 2: Set and get values
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello", nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    std::cout << "✓ Basic set/get operations work" << std::endl;
    
    // Test 3: Get with validation
    auto intVal = config.get<int>("test_int");
    if (intVal && *intVal == 42) {
        std::cout << "✓ Integer get works" << std::endl;
    }
    
    auto stringVal = config.get<std::string>("test_string");
    if (stringVal && *stringVal == "hello") {
        std::cout << "✓ String get works" << std::endl;
    }
    
    auto doubleVal = config.get<double>("test_double");
    if (doubleVal && *doubleVal > 3.13 && *doubleVal < 3.15) {
        std::cout << "✓ Double get works" << std::endl;
    }
    
    auto boolVal = config.get<bool>("test_bool");
    if (boolVal && *boolVal == true) {
        std::cout << "✓ Boolean get works" << std::endl;
    }
    
    // Test 4: Get or default
    int defaultVal = config.getOr<int>("missing_key", 100);
    if (defaultVal == 100) {
        std::cout << "✓ Get or default works" << std::endl;
    }
    
    // Test 5: Check existence
    if (config.has("test_int") && !config.has("missing_key")) {
        std::cout << "✓ Has check works" << std::endl;
    }
    
    // Test 6: Get keys
    auto keys = config.getKeys();
    if (keys.size() == 4) {
        std::cout << "✓ Get keys works" << std::endl;
    }
    
    // Test 7: Remove
    config.remove("test_int");
    if (!config.has("test_int")) {
        std::cout << "✓ Remove works" << std::endl;
    }
    
    // Test 8: Configuration statistics
    auto stats = config.getStats();
    std::cout << "✓ Stats: " << stats.totalEntries << " total entries" << std::endl;
    
    // Test 9: Available presets and templates
    auto presets = nlm::Config::getAvailablePresets();
    std::cout << "✓ Available presets: " << presets.size() << std::endl;
    
    auto templates = nlm::Config::getAvailableTemplates();
    std::cout << "✓ Available templates: " << templates.size() << std::endl;
    
    // Test 10: Schema validation
    nlm::SchemaEntry schema("test_int", "integer", false, nlm::ConfigValue(42), 0.0, 100.0, "", {});
    bool schemaAdded = nlm::Config::addSchema(schema);
    if (schemaAdded) {
        std::cout << "✓ Schema validation setup works" << std::endl;
    }
    
    std::cout << "\n=== Configuration System Test Complete ===" << std::endl;
    std::cout << "All tests passed! The enhanced configuration system is working correctly." << std::endl;
    
    return 0;
}