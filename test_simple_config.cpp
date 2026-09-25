#include "src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>

void testBasicConfig() {
    nlm::Config config;
    
    std::cout << "Testing basic config operations..." << std::endl;
    
    // Test setting and getting values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Test getOr
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    // Test remove
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    config.remove("test");
    assert(!config.has("test"));
    
    // Test getKeys
    std::vector<std::string> keys = config.getKeys();
    assert(keys.size() >= 5);
    
    // Test summary
    std::string summary = config.summary();
    assert(summary.find("Configuration") != std::string::npos);
    
    std::cout << "  PASSED" << std::endl;
}

void testBackwardCompatibility() {
    std::cout << "Testing backward compatibility with key=value format..." << std::endl;
    
    // Create a simple config file in old format
    std::ofstream file("/tmp/legacy_config.cfg");
    file << "# Legacy config file\n";
    file << "int_value = 42\n";
    file << "string_value = hello\n";
    file.close();
    
    nlm::Config config;
    bool success = config.loadFromFile("/tmp/legacy_config.cfg");
    assert(success);
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    std::cout << "  PASSED" << std::endl;
}

int main() {
    std::cout << "=== Basic Config System Tests ===" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    try {
        testBasicConfig();
    } catch (const std::exception& e) {
        std::cout << "FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testBackwardCompatibility();
    } catch (const std::exception& e) {
        std::cout << "FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Tests FAILED ===" << std::endl;
        return 1;
    }
}

