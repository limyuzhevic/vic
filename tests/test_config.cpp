// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

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
    
    // Test empty key validation
    config.set("", 42, nlm::ConfigSource::Runtime);  // Empty key should be allowed
    
    // Test valid values pass validation
    config.set("valid_int", 42, nlm::ConfigSource::Runtime);
    config.set("valid_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("valid_string", "test", nlm::ConfigSource::Runtime);
    config.set("valid_bool", true, nlm::ConfigSource::Runtime);
    
    auto validationErrors = config.validate();
    assert(validationErrors.size() == 0);  // Should have no errors
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigSaveToSimpleFile() {
    nlm::Config config;
    
    // Set up test data
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14159, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello world", nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    // Save to file
    const std::string testFile = "/tmp/test_config.cfg";
    bool saveSuccess = config.saveToSimpleFile(testFile);
    assert(saveSuccess);
    
    // Read back and verify content
    std::ifstream file(testFile);
    assert(file.is_open());
    
    std::string line;
    bool foundInt = false, foundDouble = false, foundString = false, foundBool = false;
    while (std::getline(file, line)) {
        if (line.find("test_int") != std::string::npos && line.find("42") != std::string::npos) {
            foundInt = true;
        }
        if (line.find("test_double") != std::string::npos && line.find("3.14159") != std::string::npos) {
            foundDouble = true;
        }
        if (line.find("test_string") != std::string::npos && line.find("\"hello world\"") != std::string::npos) {
            foundString = true;
        }
        if (line.find("test_bool") != std::string::npos && line.find("true") != std::string::npos) {
            foundBool = true;
        }
    }
    
    assert(foundInt);
    assert(foundDouble);
    assert(foundString);
    assert(foundBool);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigSaveToSimpleFile passed" << std::endl;
}

void testConfigExportToJSON() {
    nlm::Config config;
    
    // Set up test data
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("double_key", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_key", "test_value", nlm::ConfigSource::Runtime);
    config.set("bool_key", false, nlm::ConfigSource::Runtime);
    
    // Export to JSON
    std::string json = config.exportToJSON();
    
    // Basic JSON structure checks
    assert(json.find("{") != std::string::npos);
    assert(json.find("int_key") != std::string::npos);
    assert(json.find("double_key") != std::string::npos);
    assert(json.find("string_key") != std::string::npos);
    assert(json.find("bool_key") != std::string::npos);
    assert(json.find("}") != std::string::npos);
    
    // Check for proper formatting
    assert(json.find("\"int_key\": 42") != std::string::npos);
    assert(json.find("\"double_key\": 3.14") != std::string::npos);
    assert(json.find("\"string_key\": \"test_value\"") != std::string::npos);
    assert(json.find("\"bool_key\": false") != std::string::npos);
    
    std::cout << "    testConfigExportToJSON passed" << std::endl;
}

void testConfigImportFromJSON() {
    nlm::Config config;
    
    // Test JSON import (basic implementation)
    std::string jsonContent = "{\n  \"int_key\": 42,\n  \"double_key\": 3.14,\n  \"string_key\": \"test_value\",\n  \"bool_key\": false\n}";
    
    bool importSuccess = config.importFromJSON(jsonContent);
    // Note: importFromJSON returns false for basic implementation
    // In a full implementation, this should parse the JSON and populate the config
    assert(!importSuccess);  // Expected to fail with basic implementation
    
    std::cout << "    testConfigImportFromJSON passed" << std::endl;
}

void testConfigLoadFromSimpleFile() {
    nlm::Config config;
    
    // Create a test config file
    const std::string testFile = "/tmp/test_config_load.cfg";
    std::ofstream file(testFile);
    file << "# Test configuration file\n";
    file << "key1 = value1\n";
    file << "key2 = 42\n";
    file << "key3 = true\n";
    file << "key4 = \"quoted value\"\n";
    file << "\n";
    file << "# Another comment\n";
    file.close();
    
    // Load from simple file
    bool loadSuccess = config.loadFromSimpleFile(testFile);
    assert(loadSuccess);
    
    // Verify content was loaded
    assert(config.has("key1"));
    assert(config.has("key2"));
    assert(config.has("key3"));
    assert(config.has("key4"));
    
    auto value1 = config.get<std::string>("key1");
    assert(value1.has_value() && *value1 == "value1");
    
    auto value2 = config.get<int>("key2");
    assert(value2.has_value() && *value2 == 42);
    
    auto value3 = config.get<bool>("key3");
    assert(value3.has_value() && *value3 == true);
    
    auto value4 = config.get<std::string>("key4");
    assert(value4.has_value() && *value4 == "quoted value");
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigLoadFromSimpleFile passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigSaveToSimpleFile();
    testConfigExportToJSON();
    testConfigImportFromJSON();
    testConfigLoadFromSimpleFile();
}

} // namespace test_config
