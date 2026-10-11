#include "src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "=== Testing Config Class JSON/YAML Support ===" << std::endl;
    
    nlm::Config config;
    
    // Test basic set/get functionality
    config.set("int_value", 42);
    config.set("double_value", 3.14159);
    config.set("string_value", std::string("test"));
    config.set("bool_value", true);
    config.set("vector_value", std::vector<int>({1, 2, 3}));
    
    // Test get functionality
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && *doubleVal > 3.14 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "test");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test getOr functionality
    int missingVal = config.getOr<int>("missing", 100);
    assert(missingVal == 100);
    
    // Test has functionality
    assert(config.has("int_value"));
    assert(!config.has("nonexistent"));
    
    // Test remove functionality
    config.remove("string_value");
    assert(!config.has("string_value"));
    
    // Test keys functionality
    auto keys = config.getKeys();
    assert(keys.size() >= 4); // At least int_value, double_value, bool_value, vector_value
    
    // Test summary functionality
    std::string summary = config.summary();
    assert(!summary.empty());
    
    std::cout << "✓ All Config class tests passed!" << std::endl;
    std::cout << "\n=== Config Summary ===" << std::endl;
    std::cout << summary;
    
    return 0;
}