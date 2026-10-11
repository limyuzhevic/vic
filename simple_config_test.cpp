// Simple Config test to verify the implementation
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>

int main() {
    std::cout << "=== Simple Config Test ===" << std::endl;
    
    // Test 1: Basic Config creation and value setting
    nlm::Config config;
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_string", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    assert(config.has("test_int"));
    assert(config.has("test_double"));
    assert(config.has("test_string"));
    assert(config.has("test_bool"));
    
    // Verify values
    auto val_int = config.get<int>("test_int");
    assert(val_int.has_value() && *val_int == 42);
    
    auto val_double = config.get<double>("test_double");
    assert(val_double.has_value());
    assert(*val_double > 3.13 && *val_double < 3.15);
    
    auto val_string = config.get<std::string>("test_string");
    assert(val_string.has_value() && *val_string == "hello");
    
    auto val_bool = config.get<bool>("test_bool");
    assert(val_bool.has_value() && *val_bool == true);
    
    // Test 2: File format detection
    assert(config.detectFileFormat("config.json") == "json");
    assert(config.detectFileFormat("test.yaml") == "yaml");
    assert(config.detectFileFormat("data.yml") == "yaml");
    assert(config.detectFileFormat("unknown.txt") == "unknown");
    
    // Test 3: Simple file format loading
    std::string testFile = "test.cfg";
    std::ofstream file(testFile);
    file << "key1 = value1\n";
    file << "key2 = 123\n";
    file << "key3 = true\n";
    file.close();
    
    bool loadResult = config.loadFromFile(testFile);
    assert(loadResult == true);
    assert(config.has("key1"));
    assert(config.has("key2"));
    assert(config.has("key3"));
    
    auto val_key1 = config.get<std::string>("key1");
    assert(val_key1.has_value() && *val_key1 == "value1");
    
    auto val_key2 = config.get<int>("key2");
    assert(val_key2.has_value() && *val_key2 == 123);
    
    auto val_key3 = config.get<bool>("key3");
    assert(val_key3.has_value() && *val_key3 == true);
    
    // Cleanup
    std::filesystem::remove(testFile);
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
