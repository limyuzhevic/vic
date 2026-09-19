// Configuration Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    
    // Verify empty config
    assert(config.getKeys().empty());
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetGet() {
    nlm::Config config;
    
    // Test setting various types
    config.set("int_val", 42);
    config.set("double_val", 3.14);
    config.set("bool_val", true);
    config.set("string_val", "hello");
    
    // Test getting values
    assert(config.get<int>("int_val") == 42);
    assert(config.get<double>("double_val") == 3.14);
    assert(config.get<bool>("bool_val") == true);
    assert(config.get<std::string>("string_val") == "hello");
    
    // Test getOr with default
    assert(config.getOr<int>("int_val", 0) == 42);
    assert(config.getOr<int>("missing", 99) == 99);
    
    std::cout << "    testConfigSetGet passed" << std::endl;
}

void testConfigVectorTypes() {
    nlm::Config config;
    
    // Test vector types
    std::vector<int> intVec = {1, 2, 3};
    std::vector<double> doubleVec = {1.5, 2.5, 3.5};
    std::vector<std::string> stringVec = {"a", "b", "c"};
    std::vector<bool> boolVec = {true, false, true};
    
    config.set("int_vec", intVec);
    config.set("double_vec", doubleVec);
    config.set("string_vec", stringVec);
    config.set("bool_vec", boolVec);
    
    // Test getting vectors
    auto retrievedIntVec = config.get<std::vector<int>>("int_vec");
    assert(retrievedIntVec && retrievedIntVec.value() == intVec);
    
    auto retrievedDoubleVec = config.get<std::vector<double>>("double_vec");
    assert(retrievedDoubleVec && retrievedDoubleVec.value() == doubleVec);
    
    auto retrievedStringVec = config.get<std::vector<std::string>>("string_vec");
    assert(retrievedStringVec && retrievedStringVec.value() == stringVec);
    
    auto retrievedBoolVec = config.get<std::vector<bool>>("bool_vec");
    assert(retrievedBoolVec && retrievedBoolVec.value() == boolVec);
    
    std::cout << "    testConfigVectorTypes passed" << std::endl;
}

void testConfigSaveLoad() {
    nlm::Config config;
    
    // Set various values
    config.set("int_val", 42);
    config.set("double_val", 3.14);
    config.set("bool_val", true);
    config.set("string_val", "test");
    
    std::vector<int> intVec = {1, 2, 3};
    config.set("int_vec", intVec);
    
    // Test save to file
    const std::string filepath = "/tmp/test_config.txt";
    bool saved = config.saveToFile(filepath);
    assert(saved);
    
    // Clear and reload
    config.clear();
    bool loaded = config.loadFromFile(filepath);
    assert(loaded);
    
    // Verify values were loaded
    assert(config.get<int>("int_val") == 42);
    assert(config.get<double>("double_val") == 3.14);
    assert(config.get<bool>("bool_val") == true);
    assert(config.get<std::string>("string_val") == "test");
    
    auto loadedIntVec = config.get<std::vector<int>>("int_vec");
    assert(loadedIntVec && loadedIntVec.value() == intVec);
    
    std::cout << "    testConfigSaveLoad passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test basic validation
    config.set("valid_key", "value");
    assert(config.validate()); // Should pass with basic validation
    
    // Test custom validation (empty key fails)
    config.set("", "empty_key");
    assert(!config.validate());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("key1", "value1");
    config.set("key2", 42);
    config.set("key3", 3.14);
    
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    assert(summary.find("key3") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetGet();
    testConfigVectorTypes();
    testConfigSaveLoad();
    testConfigValidation();
    testConfigSummary();
}

} // namespace test_config
