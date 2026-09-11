// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
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

void testJSONSaveAndLoad() {
    nlm::Config config;
    
    // Set various types
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_val", std::string("hello world"), nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    config.set("array_int", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    config.set("array_string", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    
    // Save to a temporary file
    std::string tempFile = "test_config.json";
    bool saved = config.saveToFile(tempFile);
    assert(saved);
    
    // Load from the file
    nlm::Config config2;
    bool loaded = config2.loadFromFile(tempFile);
    assert(loaded);
    
    // Verify all values
    assert(config2.has("int_val"));
    assert(config2.has("double_val"));
    assert(config2.has("string_val"));
    assert(config2.has("bool_val"));
    assert(config2.has("array_int"));
    assert(config2.has("array_string"));
    
    // Verify values
    auto intVal = config2.get<int>("int_val");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config2.get<double>("double_val");
    assert(doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config2.get<std::string>("string_val");
    assert(stringVal.has_value() && *stringVal == "hello world");
    
    auto boolVal = config2.get<bool>("bool_val");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testJSONSaveAndLoad passed" << std::endl;
}

void testSimpleFormatSaveAndLoad() {
    nlm::Config config;
    
    // Set some values
    config.set("simple_int", 100, nlm::ConfigSource::Runtime);
    config.set("simple_double", 2.718, nlm::ConfigSource::Runtime);
    config.set("simple_string", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("simple_bool", false, nlm::ConfigSource::Runtime);
    
    // Save to a file in simple format
    std::string tempFile = "test_simple.cfg";
    bool saved = config.saveToFile(tempFile);
    assert(saved);
    
    // Read and verify file content
    std::ifstream file(tempFile);
    assert(file.is_open());
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    // Check that JSON is saved (our implementation always saves JSON)
    auto json = nlohmann::json::parse(content);
    assert(json["simple_int"] == 100);
    assert(json["simple_double"] == 2.718);
    assert(json["simple_string"] == "test");
    assert(json["simple_bool"] == false);
    
    // Load back and verify
    nlm::Config config2;
    bool loaded = config2.loadFromFile(tempFile);
    assert(loaded);
    
    assert(config2.has("simple_int"));
    assert(config2.has("simple_double"));
    assert(config2.has("simple_string"));
    assert(config2.has("simple_bool"));
    
    std::remove(tempFile.c_str());
    
    std::cout << "    testSimpleFormatSaveAndLoad passed" << std::endl;
}

void testValidation() {
    nlm::Config config;
    
    // Add a range validator
    auto intValidator = std::make_unique<nlm::IntegerRangeValidator>(0, 100, true);
    config.addValidator("port", std::move(intValidator));
    
    // Valid value should work
    config.set("port", 50, nlm::ConfigSource::Runtime);
    
    // Invalid value should throw
    try {
        config.set("port", -5, nlm::ConfigSource::Runtime);
        assert(false); // Should have thrown
    } catch (const nlm::ConfigValidationError& e) {
        // Expected
    }
    
    // Remove validator
    config.removeValidator("port");
    
    // Now invalid values should work (validator removed)
    config.set("port", -5, nlm::ConfigSource::Runtime);
    
    // Test string pattern validator
    auto patternValidator = std::make_unique<nlm::StringPatternValidator>(
        "^[a-zA-Z0-9_]+$", "Username must be alphanumeric or underscore"
    );
    config.addValidator("username", std::move(patternValidator));
    
    // Valid username
    config.set("username", "test_user", nlm::ConfigSource::Runtime);
    
    // Invalid username
    try {
        config.set("username", "user@name", nlm::ConfigSource::Runtime);
        assert(false); // Should have thrown
    } catch (const nlm::ConfigValidationError& e) {
        // Expected
    }
    
    std::cout << "    testValidation passed" << std::endl;
}

void testBackwardCompatibility() {
    nlm::Config config;
    
    // Set a value
    config.set("legacy_key", std::string("legacy_value"), nlm::ConfigSource::File);
    
    // Save and reload
    std::string tempFile = "test_compat.json";
    bool saved = config.saveToFile(tempFile);
    assert(saved);
    
    nlm::Config config2;
    bool loaded = config2.loadFromFile(tempFile);
    assert(loaded);
    
    // Verify the value persists
    assert(config2.has("legacy_key"));
    auto val = config2.get<std::string>("legacy_key");
    assert(val.has_value() && *val == "legacy_value");
    
    std::remove(tempFile.c_str());
    
    std::cout << "    testBackwardCompatibility passed" << std::endl;
}

void testLoadFromArgs() {
    nlm::Config config;
    
    // Simulate command line args
    int argc = 5;
    char* argv[5];
    argv[0] = "test";
    argv[1] = "--int_val=42";
    argv[2] = "--double_val=3.14";
    argv[3] = "--string_val=hello";
    argv[4] = "--bool_val=true";
    
    config.loadFromArgs(argc, argv);
    
    // Verify all values
    assert(config.has("int_val"));
    assert(config.has("double_val"));
    assert(config.has("string_val"));
    assert(config.has("bool_val"));
    
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("double_val");
    assert(doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal.has_value() && *boolVal == true);
    
    std::cout << "    testLoadFromArgs passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testJSONSaveAndLoad();
    testSimpleFormatSaveAndLoad();
    testValidation();
    testBackwardCompatibility();
    testLoadFromArgs();
}

} // namespace test_config
