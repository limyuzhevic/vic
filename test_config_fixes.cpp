#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Config System Fixes..." << std::endl;
    
    // Test 1: Config creation
    nlm::Config config;
    assert(!config.has("test"));
    std::cout << "✓ Config creation passed" << std::endl;
    
    // Test 2: Set and get values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    std::cout << "✓ Set and get int passed" << std::endl;
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    std::cout << "✓ Set and get double passed" << std::endl;
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    std::cout << "✓ Set and get string passed" << std::endl;
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    std::cout << "✓ Set and get bool passed" << std::endl;
    
    // Test 3: getOr for missing key
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    std::cout << "✓ getOr for missing key passed" << std::endl;
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    std::cout << "✓ getOr for existing key passed" << std::endl;
    
    // Test 4: Remove key
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    std::cout << "✓ Remove key passed" << std::endl;
    
    // Test 5: Get keys
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    std::cout << "✓ Get keys passed" << std::endl;
    
    // Test 6: Summary
    std::string summary = config.summary();
    assert(!summary.empty());
    std::cout << "✓ Summary generation passed" << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== All Config Tests PASSED ===" << std::endl;
    
    return 0;
}
