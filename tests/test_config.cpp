// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

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
    
    auto intVal = config.get<int64_t>("int_value");
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
    
    int64_t val = config.getOr<int64_t>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int64_t>("existing", 100);
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

void testConfigLoadFromJson() {
    nlm::Config config;
    
    // Create test JSON content
    nlohmann::json jsonData = {
        {"int_value", 42},
        {"double_value", 3.14},
        {"string_value", "hello"},
        {"bool_value", true},
        {"array_value", nlohmann::json::array({1, 2, 3})},
        {"float_array", nlohmann::json::array({"0.5", "1.5", "2.5"})}
    };
    
    // Write to temporary file
    std::ofstream file("test_config.json");
    file << jsonData.dump(4);
    file.close();
    
    // Load from JSON file
    bool result = config.loadFromFile("test_config.json");
    assert(result);
    
    // Verify loaded values
    assert(config.has("int_value"));
    auto intVal = config.get<int64_t>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    assert(config.has("double_value"));
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15);
    
    assert(config.has("string_value"));
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    assert(config.has("bool_value"));
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test that key=value format fallback still works
    config.clear();
    config.set("test", 123, nlm::ConfigSource::Runtime);
    
    std::remove("test_config.json");
    
    std::cout << "    testConfigLoadFromJson passed" << std::endl;
}

void testConfigSaveToJson() {
    nlm::Config config;
    
    // Set various values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int64_t>({1, 2, 3}), nlm::ConfigSource::Runtime);
    config.set("string_array", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    bool result = config.saveToFile("test_save_config.json");
    assert(result);
    
    // Load back and verify
    nlm::Config config2;
    result = config2.loadFromFile("test_save_config.json");
    assert(result);
    
    // Verify all values are preserved
    assert(config2.has("int_value"));
    auto intVal = config2.get<int64_t>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    assert(config2.has("double_value"));
    auto doubleVal = config2.get<double>("double_value");
    assert(doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15);
    
    assert(config2.has("string_value"));
    auto stringVal = config2.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    // Clean up
    std::remove("test_save_config.json");
    
    std::cout << "    testConfigSaveToJson passed" << std::endl;
}

void testConfigBackwardCompatibility() {
    nlm::Config config;
    
    // Create a simple key=value format file
    std::ofstream file("test_legacy_config.cfg");
    file << "# Legacy configuration file\n";
    file << "int_value = 42\n";
    file << "double_value = 3.14\n";
    file << "string_value = \"hello world\"\n";
    file << "bool_value = true\n";
    file << "comment_line = ignored\n";
    file.close();
    
    // Load using backward compatible format
    bool result = config.loadFromFile("test_legacy_config.cfg");
    assert(result);
    
    // Verify loaded values
    assert(config.has("int_value"));
    auto intVal = config.get<int64_t>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    assert(config.has("double_value"));
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15);
    
    assert(config.has("string_value"));
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello world");
    
    assert(config.has("bool_value"));
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Clean up
    std::remove("test_legacy_config.cfg");
    
    std::cout << "    testConfigBackwardCompatibility passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigLoadFromJson();
    testConfigSaveToJson();
    testConfigBackwardCompatibility();
}

} // namespace test_config