// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
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

// New comprehensive tests for JSON/YAML parsing and saveToFile improvements
void testSaveToFileFixed() {
    nlm::Config config;
    
    // Set various types of values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", "test_string", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Save to temporary file
    std::string tempFile = "/tmp/test_config_save.cfg";
    bool saveResult = config.saveToFile(tempFile);
    assert(saveResult);
    
    // Read back the file and verify actual values are saved (not placeholders)
    std::ifstream file(tempFile);
    assert(file.is_open());
    
    std::string line;
    bool found_int = false, found_double = false, found_string = false, found_bool = false;
    
    while (std::getline(file, line)) {
        if (line.find("int_value") != std::string::npos) {
            found_int = true;
            // Check that it's not PLACEHOLDER_VALUE
            assert(line.find("PLACEHOLDER_VALUE") == std::string::npos);
            // Check that the value is actually 42
            assert(line.find("= 42") != std::string::npos || line.find("=42") != std::string::npos);
        } else if (line.find("double_value") != std::string::npos) {
            found_double = true;
            assert(line.find("PLACEHOLDER_VALUE") == std::string::npos);
            assert(line.find("= 3.14") != std::string::npos || line.find("=3.14") != std::string::npos);
        } else if (line.find("string_value") != std::string::npos) {
            found_string = true;
            assert(line.find("PLACEHOLDER_VALUE") == std::string::npos);
            assert(line.find("= \"test_string\"") != std::string::npos || line.find("=test_string") != std::string::npos);
        } else if (line.find("bool_value") != std::string::npos) {
            found_bool = true;
            assert(line.find("PLACEHOLDER_VALUE") == std::string::npos);
            assert(line.find("= true") != std::string::npos || line.find("=true") != std::string::npos);
        }
    }
    
    assert(found_int && found_double && found_string && found_bool);
    std::cout << "    testSaveToFileFixed passed" << std::endl;
    
    // Clean up
    std::remove(tempFile.c_str());
}

void testJSONFileLoading() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::string tempFile = "/tmp/test_config_json.json";
    {
        std::ofstream file(tempFile);
        file << R"({
            "int_setting": 123,
            "double_setting": 45.67,
            "string_setting": "json_test",
            "bool_setting": true,
            "array_setting": [1, 2, 3, "four", 5.5]
        })";
    }
    
    // Load the JSON file
    bool loadResult = config.loadFromFile(tempFile);
    assert(loadResult);
    
    // Verify values were loaded correctly
    auto intVal = config.get<int64_t>("int_setting");
    assert(intVal.has_value() && *intVal == 123);
    
    auto doubleVal = config.get<double>("double_setting");
    assert(doubleVal.has_value() && *doubleVal == 45.67);
    
    auto stringVal = config.get<std::string>("string_setting");
    assert(stringVal.has_value() && *stringVal == "json_test");
    
    auto boolVal = config.get<bool>("bool_setting");
    assert(boolVal.has_value() && *boolVal == true);
    
    std::cout << "    testJSONFileLoading passed" << std::endl;
    
    // Clean up
    std::remove(tempFile.c_str());
}

void testKeyValueFileFallback() {
    nlm::Config config;
    
    // Create a temporary key=value file (legacy format)
    std::string tempFile = "/tmp/test_config_legacy.cfg";
    {
        std::ofstream file(tempFile);
        file << "# Test comment\n";
        file << "legacy_int = 999\n";
        file << "legacy_string = \"quoted_value\"\n";
        file << "legacy_bool = false\n";
        file << "# Another comment\n";
        file << "legacy_double = 3.14159\n";
    }
    
    // Load the legacy format file (should fallback to simple parsing)
    bool loadResult = config.loadFromFile(tempFile);
    assert(loadResult);
    
    // Verify values were loaded from legacy format
    auto intVal = config.get<int64_t>("legacy_int");
    assert(intVal.has_value() && *intVal == 999);
    
    auto stringVal = config.get<std::string>("legacy_string");
    assert(stringVal.has_value() && *stringVal == "quoted_value");
    
    auto boolVal = config.get<bool>("legacy_bool");
    assert(boolVal.has_value() && *boolVal == false);
    
    auto doubleVal = config.get<double>("legacy_double");
    assert(doubleVal.has_value() && *doubleVal == 3.14159);
    
    std::cout << "    testKeyValueFileFallback passed" << std::endl;
    
    // Clean up
    std::remove(tempFile.c_str());
}

void testSaveAndLoadRoundtrip() {
    nlm::Config config;
    
    // Set various values with different types
    config.set("string_test", "roundtrip_value", nlm::ConfigSource::Runtime);
    config.set("int_test", 42, nlm::ConfigSource::Runtime);
    config.set("double_test", 3.14159, nlm::ConfigSource::Runtime);
    config.set("bool_test", true, nlm::ConfigSource::Runtime);
    
    // Save to file
    std::string tempFile1 = "/tmp/test_roundtrip_1.cfg";
    bool saveResult = config.saveToFile(tempFile1);
    assert(saveResult);
    
    // Create new config and load from file
    nlm::Config config2;
    bool loadResult = config2.loadFromFile(tempFile1);
    assert(loadResult);
    
    // Verify all values were loaded correctly
    assert(config2.getOr<std::string>("string_test") == "roundtrip_value");
    assert(config2.getOr<int64_t>("int_test") == 42);
    assert(config2.getOr<double>("double_test") > 3.14158 && config2.getOr<double>("double_test") < 3.14160);
    assert(config2.getOr<bool>("bool_test") == true);
    
    std::cout << "    testSaveAndLoadRoundtrip passed" << std::endl;
    
    // Clean up
    std::remove(tempFile1.c_str());
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test loading from non-existent file (should not crash)
    bool loadResult = config.loadFromFile("/tmp/non_existent_file.cfg");
    // This should not crash, but behavior depends on implementation
    
    // Test saving to invalid location (should return false but not crash)
    bool saveResult = config.saveToFile("/invalid/path/that/does/not/exist.cfg");
    assert(!saveResult);
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testTypeConversionRobustness() {
    nlm::Config config;
    
    // Test various type conversions
    config.set("test_int", 123, nlm::ConfigSource::Runtime);
    assert(config.get<int64_t>("test_int").has_value());
    
    config.set("test_double", 45.67, nlm::ConfigSource::Runtime);
    assert(config.get<double>("test_double").has_value());
    
    config.set("test_bool", false, nlm::ConfigSource::Runtime);
    assert(config.get<bool>("test_bool").has_value());
    
    config.set("test_string", "robust_test", nlm::ConfigSource::Runtime);
    assert(config.get<std::string>("test_string").has_value());
    
    std::cout << "    testTypeConversionRobustness passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    
    // New comprehensive tests
    testSaveToFileFixed();
    testJSONFileLoading();
    testKeyValueFileFallback();
    testSaveAndLoadRoundtrip();
    testConfigErrorHandling();
    testTypeConversionRobustness();
    
    std::cout << "=== All Config Tests Passed ===" << std::endl;
}

} // namespace test_config
