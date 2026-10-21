// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>

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

// New comprehensive tests for configuration system
void testJSONParsing() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::string testFile = "test_config.json";
    {
        std::ofstream file(testFile);
        file << "{\n";
        file << "  \"int_key\": 42,\n";
        file << "  \"double_key\": 3.14,\n";
        file << "  \"string_key\": \"hello world\",\n";
        file << "  \"bool_key\": true,\n";
        file << "  \"int_array\": [1, 2, 3, 4, 5],\n";
        file << "  \"double_array\": [1.5, 2.5, 3.5],\n";
        file << "  \"string_array\": [\"a\", \"b\", \"c\"],\n";
        file << "  \"empty_object\": {},\n";
        file << "  \"invalid_int\": \"not a number\"\n";
        file << "}\n";
    }
    
    assert(config.loadFromFile(testFile));
    
    // Test JSON parsed values
    assert(config.has("int_key"));
    assert(config.get<int>("int_key").value() == 42);
    
    assert(config.has("double_key"));
    assert(fabs(config.get<double>("double_key").value() - 3.14) < 0.001);
    
    assert(config.has("string_key"));
    assert(config.get<std::string>("string_key").value() == "hello world");
    
    assert(config.has("bool_key"));
    assert(config.get<bool>("bool_key").value() == true);
    
    assert(config.has("int_array"));
    auto intArray = config.get<std::vector<int>>("int_array");
    assert(intArray.value().size() == 5);
    assert(intArray.value()[0] == 1 && intArray.value()[4] == 5);
    
    assert(config.has("double_array"));
    auto doubleArray = config.get<std::vector<double>>("double_array");
    assert(doubleArray.value().size() == 3);
    
    assert(config.has("string_array"));
    auto stringArray = config.get<std::vector<std::string>>("string_array");
    assert(stringArray.value().size() == 3);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testJSONParsing passed" << std::endl;
}

void testJSONSaveLoadRoundtrip() {
    nlm::Config config;
    
    // Set various values
    config.set("int_test", 123, nlm::ConfigSource::Runtime);
    config.set("double_test", 45.67, nlm::ConfigSource::Runtime);
    config.set("string_test", "roundtrip test", nlm::ConfigSource::Runtime);
    config.set("bool_test", false, nlm::ConfigSource::Runtime);
    config.set("int_array_test", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    std::string testFile = "roundtrip_test.json";
    assert(config.saveToFile(testFile));
    
    // Load from JSON file
    nlm::Config config2;
    assert(config2.loadFromFile(testFile));
    
    // Verify all values were preserved
    assert(config2.get<int>("int_test").value() == 123);
    assert(fabs(config2.get<double>("double_test").value() - 45.67) < 0.001);
    assert(config2.get<std::string>("string_test").value() == "roundtrip test");
    assert(config2.get<bool>("bool_test").value() == false);
    
    auto loadedArray = config2.get<std::vector<int>>("int_array_test");
    assert(loadedArray.value().size() == 3);
    assert(loadedArray.value()[0] == 1);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testJSONSaveLoadRoundtrip passed" << std::endl;
}

void testLegacyFormatBackwardCompatibility() {
    nlm::Config config;
    
    // Create a legacy key=value file
    std::string legacyFile = "legacy_config.txt";
    {
        std::ofstream file(legacyFile);
        file << "# This is a legacy config file\n";
        file << "legacy_key = 42\n";
        file << "another_key = \"quoted value\"\n";
        file << "bool_legacy = true\n";
        file << "# Comment line\n";
        file << "array_legacy = [1,2,3]\n";
    }
    
    assert(config.loadFromFile(legacyFile));
    
    // Test legacy format parsing
    assert(config.has("legacy_key"));
    assert(config.get<int>("legacy_key").value() == 42);
    
    assert(config.has("another_key"));
    assert(config.get<std::string>("another_key").value() == "quoted value");
    
    assert(config.has("bool_legacy"));
    assert(config.get<bool>("bool_legacy").value() == true);
    
    assert(config.has("array_legacy"));
    assert(config.get<std::string>("array_legacy").value() == "[1,2,3]");
    
    // Clean up
    std::remove(legacyFile.c_str());
    
    std::cout << "    testLegacyFormatBackwardCompatibility passed" << std::endl;
}

void testValidation() {
    nlm::Config config;
    
    // Test valid values
    config.set("valid_int", 42, nlm::ConfigSource::Runtime);
    config.set("valid_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("valid_bool", true, nlm::ConfigSource::Runtime);
    config.set("valid_string", "hello", nlm::ConfigSource::Runtime);
    config.set("valid_int_array", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    
    // Test invalid values (should throw exceptions)
    bool validationPassed = false;
    try {
        config.set("invalid_double", NAN, nlm::ConfigSource::Runtime);
        std::cout << "    ERROR: NAN value should have been rejected!" << std::endl;
        assert(false);
    } catch (const std::invalid_argument&) {
        validationPassed = true;
    }
    
    try {
        config.set("invalid_inf", INFINITY, nlm::ConfigSource::Runtime);
        std::cout << "    ERROR: INFINITY value should have been rejected!" << std::endl;
        assert(false);
    } catch (const std::invalid_argument&) {
        validationPassed = true;
    }
    
    try {
        config.set("invalid_string", "", nlm::ConfigSource::Runtime);
        std::cout << "    ERROR: Empty string should have been rejected!" << std::endl;
        assert(false);
    } catch (const std::invalid_argument&) {
        validationPassed = true;
    }
    
    // Test that valid values still work after validation attempts
    assert(config.get<int>("valid_int").value() == 42);
    assert(config.get<double>("valid_double").value() == 3.14);
    assert(config.get<bool>("valid_bool").value() == true);
    assert(config.get<std::string>("valid_string").value() == "hello");
    
    std::cout << "    testValidation passed" << std::endl;
}

void testTypeSafeGetters() {
    nlm::Config config;
    
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello", nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    config.set("test_int_array", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    config.set("test_double_array", std::vector<double>{1.1, 2.2, 3.3}, nlm::ConfigSource::Runtime);
    config.set("test_string_array", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    
    // Test type-safe getters
    assert(config.getAsInt("test_int") == 42);
    assert(fabs(config.getAsDouble("test_double") - 3.14) < 0.001);
    assert(config.getAsString("test_string") == "hello");
    assert(config.getAsBool("test_bool") == true);
    
    auto intArray = config.getAsIntArray("test_int_array");
    assert(intArray.size() == 3);
    assert(intArray[0] == 1);
    
    auto doubleArray = config.getAsDoubleArray("test_double_array");
    assert(doubleArray.size() == 3);
    assert(fabs(doubleArray[0] - 1.1) < 0.001);
    
    auto stringArray = config.getAsStringArray("test_string_array");
    assert(stringArray.size() == 3);
    assert(stringArray[0] == "a");
    
    std::cout << "    testTypeSafeGetters passed" << std::endl;
}

void testErrorHandling() {
    nlm::Config config;
    
    // Test accessing non-existent keys with type-safe getters
    bool exceptionCaught = false;
    try {
        config.getAsString("non_existent_key");
    } catch (const std::runtime_error& e) {
        exceptionCaught = true;
        assert(std::string(e.what()).find("not found") != std::string::npos);
    }
    assert(exceptionCaught);
    
    // Test accessing non-existent keys with original getters
    assert(!config.get<std::string>("non_existent_key").has_value());
    
    // Test save to non-existent directory (should fail gracefully)
    assert(!config.saveToFile("/non/existent/path/file.txt"));
    
    std::cout << "    testErrorHandling passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    
    // New comprehensive tests
    testJSONParsing();
    testJSONSaveLoadRoundtrip();
    testLegacyFormatBackwardCompatibility();
    testValidation();
    testTypeSafeGetters();
    testErrorHandling();
    
    std::cout << "\nAll configuration tests passed successfully!" << std::endl;
}

} // namespace test_config
