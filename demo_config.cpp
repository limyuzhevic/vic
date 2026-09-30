// Simple Config demonstration program
// This program demonstrates the improved Config class functionality

#include <iostream>
#include <cassert>
#include <string>
#include <vector>

// Include the Config header
#include "src/core/Config/Config.hpp"

int main() {
    std::cout << "=== Config Class Demonstration ===" << std::endl;
    
    // Test 1: Basic Config creation
    std::cout << "\n1. Testing Config Creation:" << std::endl;
    nlm::Config config;
    std::cout << "   ✓ Config object created successfully" << std::endl;
    
    // Test 2: Setting and getting different data types
    std::cout << "\n2. Testing Set/Get Operations:" << std::endl;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    std::cout << "   ✓ int value: 42" << std::endl;
    
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && *doubleVal > 3.14 && *doubleVal < 3.15);
    std::cout << "   ✓ double value: 3.14159" << std::endl;
    
    config.set("string_value", std::string("Hello Config"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "Hello Config");
    std::cout << "   ✓ string value: 'Hello Config'" << std::endl;
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    std::cout << "   ✓ bool value: true" << std::endl;
    
    // Test 3: Vector types
    std::cout << "\n3. Testing Vector Types:" << std::endl;
    
    config.set("int_vector", std::vector<int>{1, 2, 3, 4, 5}, nlm::ConfigSource::Runtime);
    auto intVector = config.get<std::vector<int>>("int_vector");
    assert(intVector.has_value() && intVector->size() == 5 && (*intVector)[0] == 1);
    std::cout << "   ✓ int vector: [1, 2, 3, 4, 5]" << std::endl;
    
    // Test 4: getOr() method with default values
    std::cout << "\n4. Testing getOr() with Default Values:" << std::endl;
    
    int missingVal = config.getOr<int>("missing_key", 100);
    assert(missingVal == 100);
    std::cout << "   ✓ Missing key returns default: 100" << std::endl;
    
    int existingVal = config.getOr<int>("int_value", 999);
    assert(existingVal == 42);
    std::cout << "   ✓ Existing key returns stored value: 42" << std::endl;
    
    // Test 5: has() and remove() methods
    std::cout << "\n5. Testing has() and remove() Methods:" << std::endl;
    
    assert(config.has("int_value"));
    std::cout << "   ✓ Config has 'int_value' key" << std::endl;
    
    config.remove("int_value");
    assert(!config.has("int_value"));
    std::cout << "   ✓ Config no longer has 'int_value' key after removal" << std::endl;
    
    // Test 6: getKeys() method
    std::cout << "\n6. Testing getKeys() Method:" << std::endl;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2.0, nlm::ConfigSource::Runtime);
    config.set("key3", std::string("test"), nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    std::cout << "   ✓ Config has 3 keys: ";
    for (size_t i = 0; i < keys.size(); ++i) {
        std::cout << keys[i];
        if (i < keys.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    
    // Test 7: Configuration registry
    std::cout << "\n7. Testing Configuration Registry:" << std::endl;
    
    config.registerConfig("neuron_count", "Number of neurons in the brain", nlm::ConfigCategory::Neural, "100");
    config.registerConfig("learning_rate", "STDP learning rate", nlm::ConfigCategory::Plasticity, "0.01");
    
    std::cout << "   ✓ Registered 'neuron_count' in Neural category" << std::endl;
    std::cout << "   ✓ Registered 'learning_rate' in Plasticity category" << std::endl;
    
    // Test 8: Error handling
    std::cout << "\n8. Testing Error Handling:" << std::endl;
    
    std::string errorMsg = config.getLastError();
    assert(errorMsg.empty());  // Should have no errors after successful operations
    std::cout << "   ✓ No errors after successful operations" << std::endl;
    
    // Test 9: Summary generation
    std::cout << "\n9. Testing Summary Generation:" << std::endl;
    
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("Configuration") != std::string::npos);
    std::cout << "   ✓ Summary generated successfully" << std::endl;
    
    std::cout << "\n=== All Config Tests Passed! ===" << std::endl;
    return 0;
}
