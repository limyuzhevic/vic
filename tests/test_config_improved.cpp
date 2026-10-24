// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

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

void testConfigSummary() {
    nlm::Config config;
    config.set("test1", 42, nlm::ConfigSource::Runtime);
    config.set("test2", "hello", nlm::ConfigSource::File);
    
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("test1") != std::string::npos);
    assert(summary.find("test2") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigTypeConversion() {
    nlm::Config config;
    
    // Test various type conversions
    config.set("int64", static_cast<int64_t>(1234567890123));
    auto val = config.get<int64_t>("int64");
    assert(val.has_value());
    assert(*val == 1234567890123LL);
    
    config.set("float_val", 3.14159f);
    val = config.get<double>("float_val");  // Can read as double
    assert(val.has_value());
    assert(*val > 3.14 && *val < 3.15);
    
    std::cout << "    testConfigTypeConversion passed" << std::endl;
}

void testConfigJSONRoundtrip() {
    nlm::Config config;
    
    // Set various types of values
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string_val", "test_string", nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int64_t>({1, 2, 3, 4, 5}), nlm::ConfigSource::Runtime);
    config.set("double_array", std::vector<double>({1.1, 2.2, 3.3}), nlm::ConfigSource::Runtime);
    
    // Save to temporary JSON file
    std::string jsonFile = "/tmp/test_config.json";
    bool saveSuccess = config.saveToFile(jsonFile);
    assert(saveSuccess);
    
    // Create new config and load from JSON
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(jsonFile);
    assert(loadSuccess);
    
    // Verify values were preserved
    auto intVal = config2.get<int>("int_val");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config2.get<double>("double_val");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.14 && *doubleVal < 3.15);
    
    auto stringVal = config2.get<std::string>("string_val");
    assert(stringVal.has_value() && *stringVal == "test_string");
    
    auto boolVal = config2.get<bool>("bool_val");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Clean up
    std::remove(jsonFile.c_str());
    
    std::cout << "    testConfigJSONRoundtrip passed" << std::endl;
}

void testConfigKeyValueRoundtrip() {
    nlm::Config config;
    
    // Set values
    config.set("test_int", 123, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello", nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_bool", false, nlm::ConfigSource::Runtime);
    
    // Save to temporary key=value file
    std::string kvFile = "/tmp/test_config.kv";
    bool saveSuccess = config.saveToFile(kvFile);
    assert(saveSuccess);
    
    // Create new config and load from key=value
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(kvFile);
    assert(loadSuccess);
    
    // Verify values were preserved
    auto intVal = config2.get<int>("test_int");
    assert(intVal.has_value() && *intVal == 123);
    
    auto stringVal = config2.get<std::string>("test_string");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto doubleVal = config2.get<double>("test_double");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto boolVal = config2.get<bool>("test_bool");
    assert(boolVal.has_value() && *boolVal == false);
    
    // Clean up
    std::remove(kvFile.c_str());
    
    std::cout << "    testConfigKeyValueRoundtrip passed" << std::endl;
}

void testConfigFileNotFound() {
    nlm::Config config;
    
    // Try to load a non-existent file
    bool loadSuccess = config.loadFromFile("/tmp/nonexistent_file_xyz123.json");
    assert(!loadSuccess);  // Should fail gracefully
    
    std::cout << "    testConfigFileNotFound passed" << std::endl;
}

void testConfigEmpty() {
    nlm::Config config;
    
    // Should have no keys initially
    assert(config.getKeys().empty());
    assert(config.summary().find("Configuration (0 entries)") != std::string::npos);
    
    std::cout << "    testConfigEmpty passed" << std::endl;
}

void testConfigMoveSemantics() {
    nlm::Config config1;
    config1.set("key1", "value1", nlm::ConfigSource::Runtime);
    
    nlm::Config config2(std::move(config1));
    auto val = config2.get<std::string>("key1");
    assert(val.has_value() && *val == "value1");
    
    // config1 should be in valid but unspecified state after move
    assert(!config1.has("key1"));
    
    nlm::Config config3;
    config3 = std::move(config2);
    auto val2 = config3.get<std::string>("key1");
    assert(val2.has_value() && *val2 == "value1");
    
    std::cout << "    testConfigMoveSemantics passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSummary();
    testConfigTypeConversion();
    testConfigJSONRoundtrip();
    testConfigKeyValueRoundtrip();
    testConfigFileNotFound();
    testConfigEmpty();
    testConfigMoveSemantics();
    
    std::cout << "\n=== All Config tests passed! ===" << std::endl;
}

} // namespace test_config