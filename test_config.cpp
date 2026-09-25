#include "Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Config class..." << std::endl;
    
    nlm::Config config;
    
    // Test basic set and get
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    assert(config.has("test_int"));
    
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    // Test double
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("test_double");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    // Test string
    config.set("test_string", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("test_string");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    // Test bool
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Test getOr with default
    int defaultVal = config.getOr<int>("missing", 100);
    assert(defaultVal == 100);
    
    // Test remove
    config.set("temp", 999, nlm::ConfigSource::Runtime);
    assert(config.has("temp"));
    config.remove("temp");
    assert(!config.has("temp"));
    
    // Test getKeys
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
