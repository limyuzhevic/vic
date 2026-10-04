#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "Running Config tests..." << std::endl;
    
    // Test 1: Config creation
    {
        nlm::Config config;
        if (!config.has("test")) {
            std::cout << "✓ testConfigCreation passed" << std::endl;
        } else {
            std::cout << "✗ testConfigCreation failed" << std::endl;
            return 1;
        }
    }
    
    // Test 2: Set and get values
    {
        nlm::Config config;
        config.set("int_value", 42, nlm::ConfigSource::Runtime);
        if (!config.has("int_value")) {
            std::cout << "✗ testConfigSetAndGet failed - key not found" << std::endl;
            return 1;
        }
        
        auto intVal = config.get<int>("int_value");
        if (intVal.has_value() && *intVal == 42) {
            std::cout << "✓ testConfigSetAndGet passed" << std::endl;
        } else {
            std::cout << "✗ testConfigSetAndGet failed - value mismatch" << std::endl;
            return 1;
        }
        
        config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
        auto doubleVal = config.get<double>("double_value");
        if (doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15) {
            std::cout << "✓ testConfigSetAndGet double passed" << std::endl;
        } else {
            std::cout << "✗ testConfigSetAndGet double failed" << std::endl;
            return 1;
        }
        
        config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
        auto stringVal = config.get<std::string>("string_value");
        if (stringVal.has_value() && *stringVal == "hello") {
            std::cout << "✓ testConfigSetAndGet string passed" << std::endl;
        } else {
            std::cout << "✗ testConfigSetAndGet string failed" << std::endl;
            return 1;
        }
        
        config.set("bool_value", true, nlm::ConfigSource::Runtime);
        auto boolVal = config.get<bool>("bool_value");
        if (boolVal.has_value() && *boolVal == true) {
            std::cout << "✓ testConfigSetAndGet bool passed" << std::endl;
        } else {
            std::cout << "✗ testConfigSetAndGet bool failed" << std::endl;
            return 1;
        }
    }
    
    // Test 3: getOr
    {
        nlm::Config config;
        int val = config.getOr<int>("missing", 100);
        if (val == 100) {
            std::cout << "✓ testConfigGetOr passed (missing key)" << std::endl;
        } else {
            std::cout << "✗ testConfigGetOr failed (missing key)" << std::endl;
            return 1;
        }
        
        config.set("existing", 50, nlm::ConfigSource::Runtime);
        val = config.getOr<int>("existing", 100);
        if (val == 50) {
            std::cout << "✓ testConfigGetOr passed (existing key)" << std::endl;
        } else {
            std::cout << "✗ testConfigGetOr failed (existing key)" << std::endl;
            return 1;
        }
    }
    
    // Test 4: Remove
    {
        nlm::Config config;
        config.set("test", 123, nlm::ConfigSource::Runtime);
        if (!config.has("test")) {
            std::cout << "✗ testConfigRemove failed - key not found after set" << std::endl;
            return 1;
        }
        
        config.remove("test");
        if (config.has("test")) {
            std::cout << "✗ testConfigRemove failed - key still exists after remove" << std::endl;
            return 1;
        }
        std::cout << "✓ testConfigRemove passed" << std::endl;
    }
    
    // Test 5: Keys
    {
        nlm::Config config;
        config.set("key1", 1, nlm::ConfigSource::Runtime);
        config.set("key2", 2, nlm::ConfigSource::Runtime);
        config.set("key3", 3, nlm::ConfigSource::Runtime);
        
        auto keys = config.getKeys();
        if (keys.size() == 3 && std::find(keys.begin(), keys.end(), "key1") != keys.end() &&
            std::find(keys.begin(), keys.end(), "key2") != keys.end() &&
            std::find(keys.begin(), keys.end(), "key3") != keys.end()) {
            std::cout << "✓ testConfigKeys passed" << std::endl;
        } else {
            std::cout << "✗ testConfigKeys failed" << std::endl;
            return 1;
        }
    }
    
    std::cout << "\n✓ All tests passed!" << std::endl;
    return 0;
}
