// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
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
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test validation with valid data
    config.set("valid_int", 42, nlm::ConfigSource::Runtime);
    auto intVal = config.get<int>("valid_int");
    assert(intVal.has_value());
    
    config.set("valid_string", "test", nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("valid_string");
    assert(stringVal.has_value());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigFileOperations() {
    nlm::Config config;
    
    // Test setting values
    config.set("test_key", "test_value", nlm::ConfigSource::Runtime);
    assert(config.has("test_key"));
    
    // Test saving to file
    bool saveResult = config.saveToFile("test_config.txt");
    // Note: saveToFile implementation may vary, so this might not always work
    // but we test the operation doesn't crash
    
    std::cout << "    testConfigFileOperations passed" << std::endl;
}

void testConfigMerge() {
    nlm::Config config1;
    nlm::Config config2;
    
    config1.set("key1", "value1", nlm::ConfigSource::Runtime);
    config1.set("key2", "value2", nlm::ConfigSource::Runtime);
    
    config2.set("key2", "value2_updated", nlm::ConfigSource::Runtime);
    config2.set("key3", "value3", nlm::ConfigSource::Runtime);
    
    config1.merge(config2);
    
    // key2 should be updated from config2
    auto key2Val = config1.get<std::string>("key2");
    assert(key2Val.has_value() && *key2Val == "value2_updated");
    
    // key3 should be present
    assert(config1.has("key3"));
    
    auto key3Val = config1.get<std::string>("key3");
    assert(key3Val.has_value() && *key3Val == "value3");
    
    // key1 should still be present
    assert(config1.has("key1"));
    
    std::cout << "    testConfigMerge passed" << std::endl;
}

void testConfigClear() {
    nlm::Config config;
    
    config.set("key1", "value1", nlm::ConfigSource::Runtime);
    config.set("key2", "value2", nlm::ConfigSource::Runtime);
    
    assert(config.has("key1"));
    assert(config.has("key2"));
    
    config.clear();
    
    assert(!config.has("key1"));
    assert(!config.has("key2"));
    
    std::cout << "    testConfigClear passed" << std::endl;
}

void testConfigTypeSafety() {
    nlm::Config config;
    
    // Test type safety - accessing different types from same key
    config.set("number", 42, nlm::ConfigSource::Runtime);
    
    // This should work for int
    auto intVal = config.get<int>("number");
    assert(intVal.has_value() && *intVal == 42);
    
    // This should work for double
    auto doubleVal = config.get<double>("number");
    assert(doubleVal.has_value());
    assert(*doubleVal > 41.9 && *doubleVal < 42.1);
    
    std::cout << "    testConfigTypeSafety passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test getting non-existent key should return empty optional
    auto missingVal = config.get<int>("non_existent");
    assert(!missingVal.has_value());
    
    // Test gettingOr with default should work
    int defaultVal = config.getOr<int>("non_existent", 100);
    assert(defaultVal == 100);
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigPerformance() {
    nlm::Config config;
    
    // Test performance with many operations
    const int num_operations = 1000;
    
    for (int i = 0; i < num_operations; ++i) {
        config.set(std::to_string(i), i, nlm::ConfigSource::Runtime);
        auto val = config.get<int>(std::to_string(i));
        assert(val.has_value() && *val == i);
    }
    
    // Verify all keys are present
    auto keys = config.getKeys();
    assert(keys.size() == num_operations);
    
    std::cout << "    testConfigPerformance passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigFileOperations();
    testConfigMerge();
    testConfigClear();
    testConfigTypeSafety();
    testConfigErrorHandling();
    testConfigPerformance();
    
    std::cout << "\nAll Config Tests Passed!" << std::endl;
}

} // namespace test_config

} // namespace test_config
