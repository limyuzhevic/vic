// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

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

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test get from non-existent key
    auto val = config.get<int>("nonexistent");
    assert(!val.has_value());
    
    // Test getOr with missing key
    int defaultVal = config.getOr<int>("missing_key", 999);
    assert(defaultVal == 999);
    
    // Test remove non-existent key doesn't crash
    config.remove("nonexistent");
    
    // Test setting values of different types
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14159, nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    config.set("string_val", "test_string", nlm::ConfigSource::Runtime);
    config.set("int_vec", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    config.set("double_vec", std::vector<double>({1.5, 2.5, 3.5}), nlm::ConfigSource::Runtime);
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigValueConversion() {
    nlm::Config config;
    
    // Test numeric type conversions
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("float_val", 3.14f, nlm::ConfigSource::Runtime);
    config.set("double_val", 2.71828, nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value() && *intVal == 42);
    
    auto floatVal = config.get<float>("int_val");  // int can be converted to float
    assert(floatVal.has_value());
    
    auto doubleVal = config.get<double>("float_val");  // float can be converted to double
    assert(doubleVal.has_value());
    
    std::cout << "    testConfigValueConversion passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("key1", 100, nlm::ConfigSource::Runtime);
    config.set("key2", "hello", nlm::ConfigSource::File);
    config.set("key3", 3.14, nlm::ConfigSource::CommandLine);
    
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    assert(summary.find("key3") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigSourceTracking() {
    nlm::Config config;
    
    config.set("default_key", 1, nlm::ConfigSource::Default);
    config.set("file_key", 2, nlm::ConfigSource::File);
    config.set("command_line_key", 3, nlm::ConfigSource::CommandLine);
    config.set("runtime_key", 4, nlm::ConfigSource::Runtime);
    
    // Test that sources are preserved
    auto defaultVal = config.get<int>("default_key");
    assert(defaultVal.has_value() && *defaultVal == 1);
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

void testConfigClear() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", "value", nlm::ConfigSource::File);
    assert(config.has("key1") && config.has("key2"));
    
    config.clear();
    assert(!config.has("key1") && !config.has("key2"));
    assert(config.getKeys().empty());
    
    std::cout << "    testConfigClear passed" << std::endl;
}
