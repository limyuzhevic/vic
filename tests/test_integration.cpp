// Config Integration Test with JSON/YAML Support
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <cassert>

void testLegacyFormat() {
    std::cout << "Testing legacy key=value format..." << std::endl;
    
    nlm::Config config;
    
    // Test basic operations
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", std::string("hello"));
    config.set("bool_value", true);
    
    // Test loading from legacy file
    bool loaded = config.loadFromFile("configs/default.cfg");
    assert(loaded);
    
    // Verify some values were loaded
    auto random_seed = config.get<int>("random_seed");
    assert(random_seed.has_value());
    assert(*random_seed == 42);
    
    auto neuron_count = config.get<int>("neuron_count");
    assert(neuron_count.has_value());
    assert(*neuron_count == 1000);
    
    auto environment_name = config.get<std::string>("environment_name");
    assert(environment_name.has_value());
    assert(*environment_name == "GridWorld");
    
    auto visualization_enabled = config.get<bool>("visualization_enabled");
    assert(visualization_enabled.has_value());
    assert(*visualization_enabled == false);
    
    std::cout << "  Legacy format test PASSED" << std::endl;
}

void testJsonFormat() {
    std::cout << "Testing JSON format..." << std::endl;
    
    nlm::Config config;
    
    // Test basic operations
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", std::string("hello"));
    config.set("bool_value", true);
    
    // Test saving to JSON file
    config.saveToFile("test_config.json");
    
    // Verify file was created
    std::ifstream file("test_config.json");
    assert(file.good());
    
    // Load the JSON file back
    nlm::Config config2;
    bool loaded = config2.loadFromFile("test_config.json");
    assert(loaded);
    
    // Verify values
    auto int_val = config2.get<int>("int_value");
    assert(int_val.has_value());
    assert(*int_val == 42);
    
    auto double_val = config2.get<double>("double_value");
    assert(double_val.has_value());
    assert(*double_val > 3.13 && *double_val < 3.15);
    
    auto string_val = config2.get<std::string>("string_value");
    assert(string_val.has_value());
    assert(*string_val == "hello");
    
    auto bool_val = config2.get<bool>("bool_value");
    assert(bool_val.has_value());
    assert(*bool_val == true);
    
    // Cleanup
    std::remove("test_config.json");
    
    std::cout << "  JSON format test PASSED" << std::endl;
}

void testYamlFormat() {
    std::cout << "Testing YAML format..." << std::endl;
    
    nlm::Config config;
    
    // Test basic operations
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", std::string("hello"));
    config.set("bool_value", true);
    
    // Test saving to YAML file
    config.saveToFile("test_config.yaml");
    
    // Verify file was created
    std::ifstream file("test_config.yaml");
    assert(file.good());
    
    // Load the YAML file back
    nlm::Config config2;
    bool loaded = config2.loadFromFile("test_config.yaml");
    assert(loaded);
    
    // Verify values
    auto int_val = config2.get<int>("int_value");
    assert(int_val.has_value());
    assert(*int_val == 42);
    
    auto double_val = config2.get<double>("double_value");
    assert(double_val.has_value());
    assert(*double_val > 3.13 && *double_val < 3.15);
    
    auto string_val = config2.get<std::string>("string_value");
    assert(string_val.has_value());
    assert(*string_val == "hello");
    
    auto bool_val = config2.get<bool>("bool_value");
    assert(bool_val.has_value());
    assert(*bool_val == true);
    
    // Cleanup
    std::remove("test_config.yaml");
    
    std::cout << "  YAML format test PASSED" << std::endl;
}

void testBackwardCompatibility() {
    std::cout << "Testing backward compatibility..." << std::endl;
    
    // Test that existing functionality still works
    nlm::Config config;
    
    // Test basic operations
    config.set("key1", 1);
    config.set("key2", 2.5);
    config.set("key3", std::string("test"));
    config.set("key4", true);
    
    // Test get operations
    auto val1 = config.get<int>("key1");
    assert(val1.has_value());
    assert(*val1 == 1);
    
    auto val2 = config.get<double>("key2");
    assert(val2.has_value());
    assert(*val2 > 2.4 && *val2 < 2.6);
    
    auto val3 = config.get<std::string>("key3");
    assert(val3.has_value());
    assert(*val3 == "test");
    
    auto val4 = config.get<bool>("key4");
    assert(val4.has_value());
    assert(*val4 == true);
    
    // Test getOr operations
    int or_val = config.getOr<int>("nonexistent", 100);
    assert(or_val == 100);
    
    or_val = config.getOr<int>("key1", 100);
    assert(or_val == 1);
    
    // Test has operation
    assert(config.has("key1"));
    assert(!config.has("nonexistent"));
    
    // Test remove operation
    config.remove("key2");
    assert(!config.has("key2"));
    
    // Test getKeys operation
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    // Test clear operation
    config.clear();
    assert(config.getKeys().size() == 0);
    
    std::cout << "  Backward compatibility test PASSED" << std::endl;
}

int main() {
    try {
        testBackwardCompatibility();
        testLegacyFormat();
        testJsonFormat();
        testYamlFormat();
        
        std::cout << std::endl;
        std::cout << "All tests PASSED!" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}