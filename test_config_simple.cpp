#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

void testConfig() {
    nlm::Config config;
    
    std::cout << "Testing configuration system..." << std::endl;
    
    // Test setting values
    config.set("test_int", 42);
    config.set("test_string", std::string("hello"));
    config.set("test_double", 3.14159);
    config.set("test_bool", true);
    
    // Test existence
    assert(config.has("test_int"));
    assert(config.has("test_string"));
    assert(config.has("test_double"));
    assert(config.has("test_bool"));
    
    // Test getting values
    auto intVal = config.get<int64_t>("test_int");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto strVal = config.get<std::string>("test_string");
    assert(strVal.has_value());
    assert(*strVal == "hello");
    
    auto doubleVal = config.get<double>("test_double");
    assert(doubleVal.has_value());
    assert(std::abs(*doubleVal - 3.14159) < 0.0001);
    
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Test getOr with default
    auto missingVal = config.getOr<int64_t>("missing", 100);
    assert(missingVal == 100);
    
    // Test key listing
    auto keys = config.getKeys();
    assert(keys.size() >= 4);
    
    // Test summary
    std::string summary = config.summary();
    assert(summary.find("Configuration") != std::string::npos);
    assert(summary.find("test_int") != std::string::npos);
    
    std::cout << "✓ All configuration tests passed!" << std::endl;
}

int main() {
    try {
        testConfig();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
