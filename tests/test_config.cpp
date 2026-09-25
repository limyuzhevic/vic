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

void testConfigJsonLoad() {
    nlm::Config config;
    
    // Create a test JSON file
    std::string testFile = "test_config.json";
    {
        std::ofstream file(testFile);
        file << "{\n";
        file << "    \"int_value\": 42,\n";
        file << "    \"double_value\": 3.14,\n";
        file << "    \"bool_value\": true,\n";
        file << "    \"string_value\": \"hello world\",\n";
        file << "    \"int_array\": [1, 2, 3, 4, 5],\n";
        file << "    \"double_array\": [1.1, 2.2, 3.3],\n";
        file << "    \"string_array\": [\"a\", \"b\", \"c\"]\n";
        file << "}\n";
    }
    
    // Load from JSON file
    bool loadSuccess = config.loadFromFile(testFile);
    assert(loadSuccess);
    
    // Verify loaded values
    assert(config.has("int_value"));
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    assert(config.has("double_value"));
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && (*doubleVal > 3.13 && *doubleVal < 3.15));
    
    assert(config.has("bool_value"));
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    assert(config.has("string_value"));
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello world");
    
    assert(config.has("int_array"));
    auto intArrayVal = config.get<std::vector<int>>("int_array");
    assert(intArrayVal.has_value() && intArrayVal->size() == 5);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigJsonLoad passed" << std::endl;
}

void testConfigJsonSave() {
    nlm::Config config;
    
    // Set various values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello world"), nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    std::string testFile = "test_config_save.json";
    bool saveSuccess = config.saveToFile(testFile);
    assert(saveSuccess);
    
    // Clear config and reload from file
    config.clear();
    bool loadSuccess = config.loadFromFile(testFile);
    assert(loadSuccess);
    
    // Verify loaded values match original
    assert(config.has("int_value"));
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    assert(config.has("double_value"));
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && (*doubleVal > 3.13 && *doubleVal < 3.15));
    
    assert(config.has("bool_value"));
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    assert(config.has("string_value"));
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello world");
    
    assert(config.has("int_array"));
    auto intArrayVal = config.get<std::vector<int>>("int_array");
    assert(intArrayVal.has_value() && intArrayVal->size() == 3);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigJsonSave passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJsonLoad();
    testConfigJsonSave();
}

} // namespace test_config
