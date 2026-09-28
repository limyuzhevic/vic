// Config Tests - Comprehensive Test Suite
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

void testConfigSchema() {
    nlm::Config config;
    
    // Create a simple schema
    auto schema = std::make_shared<nlm::ConfigSchema>();
    schema->addTypeValidation("int_value", typeid(int));
    schema->addTypeValidation("string_value", typeid(std::string));
    schema->addTypeValidation("bool_value", typeid(bool));
    
    config.setSchema(schema);
    
    // Set valid values
    config.set("int_value", 42);
    config.set("string_value", "test");
    config.set("bool_value", true);
    
    // Validate should pass
    assert(config.validate());
    assert(config.getValidationErrors().empty());
    
    // Set invalid type (string instead of int)
    try {
        config.set("int_value", std::string("invalid"));
        std::cout << "    ERROR: Should have thrown ValidationError" << std::endl;
        assert(false);
    } catch (const nlm::ValidationError& e) {
        std::cout << "    testConfigSchema validation caught error as expected: " << e.what() << std::endl;
    }
    
    std::cout << "    testConfigSchema passed" << std::endl;
}

void testConfigJSONLoad() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::string jsonPath = "test_config.json";
    std::ofstream file(jsonPath);
    file << "{\"test_int\": 42, \"test_string\": \"hello\", \"test_bool\": true}";
    file.close();
    
    // Load JSON file
    assert(config.loadFromFile(jsonPath));
    
    // Verify loaded values
    assert(config.has("test_int"));
    assert(config.has("test_string"));
    assert(config.has("test_bool"));
    
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto stringVal = config.get<std::string>("test_string");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Clean up
    std::remove(jsonPath.c_str());
    
    std::cout << "    testConfigJSONLoad passed" << std::endl;
}

void testConfigLegacyLoad() {
    nlm::Config config;
    
    // Create a temporary legacy cfg file
    std::string cfgPath = "test_config.cfg";
    std::ofstream file(cfgPath);
    file << "# Test comment\n";
    file << "legacy_int = 123\n";
    file << "legacy_string = \"test\"\n";
    file << "legacy_bool = true\n";
    file.close();
    
    // Load legacy file
    assert(config.loadFromFile(cfgPath));
    
    // Verify loaded values
    assert(config.has("legacy_int"));
    assert(config.has("legacy_string"));
    assert(config.has("legacy_bool"));
    
    auto intVal = config.get<int>("legacy_int");
    assert(intVal.has_value());
    assert(*intVal == 123);
    
    auto stringVal = config.get<std::string>("legacy_string");
    assert(stringVal.has_value());
    assert(*stringVal == "test");
    
    auto boolVal = config.get<bool>("legacy_bool");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Clean up
    std::remove(cfgPath.c_str());
    
    std::cout << "    testConfigLegacyLoad passed" << std::endl;
}

void testConfigSave() {
    nlm::Config config;
    
    // Set various values
    config.set("test_int", 42);
    config.set("test_string", "hello");
    config.set("test_bool", true);
    config.set("test_double", 3.14159);
    config.set("test_vector", std::vector<int>({1, 2, 3}));
    
    // Save to file
    std::string savePath = "test_config_save.json";
    assert(config.saveToFile(savePath));
    
    // Load back and verify
    nlm::Config config2;
    assert(config2.loadFromFile(savePath));
    
    // Verify all values
    assert(config2.has("test_int"));
    assert(config2.has("test_string"));
    assert(config2.has("test_bool"));
    assert(config2.has("test_double"));
    assert(config2.has("test_vector"));
    
    auto intVal = config2.get<int>("test_int");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto stringVal = config2.get<std::string>("test_string");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    auto boolVal = config2.get<bool>("test_bool");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    auto doubleVal = config2.get<double>("test_double");
    assert(doubleVal.has_value());
    assert(std::abs(*doubleVal - 3.14159) < 0.0001);
    
    // Clean up
    std::remove(savePath.c_str());
    
    std::cout << "    testConfigSave passed" << std::endl;
}

void testConfigRangeValidation() {
    nlm::Config config;
    auto schema = std::make_shared<nlm::ConfigSchema>();
    
    schema->addRangeValidation("value", 0.0, 100.0);
    config.setSchema(schema);
    
    // Valid values
    config.set("value", 50.0);
    config.set("value", 0.0);
    config.set("value", 100.0);
    assert(config.validate());
    
    // Invalid values (will throw exception)
    bool caughtException = false;
    try {
        config.set("value", 150.0);
    } catch (const nlm::ValidationError& e) {
        caughtException = true;
    }
    assert(caughtException);
    
    std::cout << "    testConfigRangeValidation passed" << std::endl;
}

void testConfigCommandLineLoad() {
    nlm::Config config;
    
    // Simulate command line arguments
    int argc = 5;
    char* argv[5];
    argv[0] = (char*)"test";
    argv[1] = (char*)"--cli_int=123";
    argv[2] = (char*)"-cli_string";
    argv[3] = (char*)"test_value";
    argv[4] = (char*)"--cli_bool=true";
    
    assert(config.loadFromArgs(argc, argv));
    
    assert(config.has("cli_int"));
    assert(config.has("cli_string"));
    assert(config.has("cli_bool"));
    
    auto intVal = config.get<int>("cli_int");
    assert(intVal.has_value());
    assert(*intVal == 123);
    
    auto stringVal = config.get<std::string>("cli_string");
    assert(stringVal.has_value());
    assert(*stringVal == "test_value");
    
    auto boolVal = config.get<bool>("cli_bool");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigCommandLineLoad passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("test1", 1);
    config.set("test2", "hello");
    config.set("test3", true);
    
    std::string summary = config.summary();
    assert(summary.find("test1 = [1]") != std::string::npos);
    assert(summary.find("test2 = [\"hello\"]") != std::string::npos);
    assert(summary.find("test3 = [true]") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigClear() {
    nlm::Config config;
    
    config.set("test1", 1);
    config.set("test2", 2);
    assert(config.has("test1"));
    assert(config.has("test2"));
    
    config.clear();
    assert(!config.has("test1"));
    assert(!config.has("test2"));
    
    std::cout << "    testConfigClear passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSchema();
    testConfigJSONLoad();
    testConfigLegacyLoad();
    testConfigSave();
    testConfigRangeValidation();
    testConfigCommandLineLoad();
    testConfigSummary();
    testConfigClear();
    
    std::cout << "\nAll config tests passed!" << std::endl;
}

} // namespace test_config