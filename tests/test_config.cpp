// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
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

// Test new JSON loading functionality
void testConfigLoadFromJson() {
    nlm::Config config;
    
    // Test JSON string loading
    std::string jsonContent = R"({
        "int_val": 42,
        "double_val": 3.14,
        "string_val": "test",
        "bool_val": true,
        "array_val": [1, 2, 3]
    })";
    
    bool result = config.loadFromJsonString(jsonContent);
    assert(result);
    
    assert(config.has("int_val"));
    assert(config.has("double_val"));
    assert(config.has("string_val"));
    assert(config.has("bool_val"));
    assert(config.has("array_val"));
    
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value() && *intVal == 42);
    
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal.has_value() && *stringVal == "test");
    
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal.has_value() && *boolVal == true);
    
    std::cout << "    testConfigLoadFromJson passed" << std::endl;
}

// Test JSON saving functionality
void testConfigSaveToJson() {
    nlm::Config config;
    
    // Set some values
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_val", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    
    // Save to JSON string
    std::string jsonString = config.saveToJsonString();
    assert(!jsonString.empty());
    
    // Verify it contains expected values
    assert(jsonString.find("int_val") != std::string::npos);
    assert(jsonString.find("42") != std::string::npos);
    assert(jsonString.find("string_val") != std::string::npos);
    assert(jsonString.find("test") != std::string::npos);
    assert(jsonString.find("bool_val") != std::string::npos);
    assert(jsonString.find("true") != std::string::npos);
    
    std::cout << "    testConfigSaveToJson passed" << std::endl;
}

// Test validation functionality
void testConfigValidation() {
    nlm::Config config;
    
    // Set up validation rules
    std::vector<nlm::ValidationRule> rules;
    
    // Rule for required int
    rules.push_back({
        "required_int",
        [](const nlm::ConfigValue& val) {
            if (auto v = std::get_if<int>(&val)) return true;
            return false;
        },
        "required_int must be an integer"
    });
    
    // Rule for string length
    rules.push_back({
        "name",
        [](const nlm::ConfigValue& val) {
            if (auto v = std::get_if<std::string>(&val)) return v->length() >= 3;
            return false;
        },
        "name must be at least 3 characters"
    });
    
    // Test validation with missing keys
    bool result = config.validate(rules);
    assert(!result);  // Should fail - missing required keys
    
    // Add values that satisfy validation
    config.set("required_int", 42, nlm::ConfigSource::Runtime);
    config.set("name", std::string("test"), nlm::ConfigSource::Runtime);
    
    // Test validation again
    result = config.validate(rules);
    assert(result);  // Should pass now
    
    // Test validation with invalid value
    config.set("name", std::string("ab"), nlm::ConfigSource::Runtime);  // Too short
    
    result = config.validate(rules);
    assert(!result);  // Should fail now - name too short
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

// Test key=value file loading
void testConfigLoadFromKeyValue() {
    nlm::Config config;
    
    // Create a temporary config file
    std::string tempFile = "temp_config_test.cfg";
    std::ofstream file(tempFile);
    file << "# Test config file\n";
    file << "int_value = 100\n";
    file << "string_value = \"hello\"\n";
    file << "double_value = 2.718\n";
    file << "bool_value = true\n";
    file.close();
    
    // Load the config file
    bool result = config.loadFromFile(tempFile);
    assert(result);
    
    // Verify loaded values
    assert(config.has("int_value"));
    assert(config.has("string_value"));
    assert(config.has("double_value"));
    assert(config.has("bool_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 100);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && *doubleVal > 2.717 && *doubleVal < 2.719);
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testConfigLoadFromKeyValue passed" << std::endl;
}

// Test configuration summary
void testConfigSummary() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", std::string("test"), nlm::ConfigSource::File);
    config.set("key3", 3.14, nlm::ConfigSource::CommandLine);
    
    std::string summary = config.summary();
    assert(summary.find("Configuration (3 entries):") != std::string::npos);
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    assert(summary.find("key3") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigLoadFromJson();
    testConfigSaveToJson();
    testConfigValidation();
    testConfigLoadFromKeyValue();
    testConfigSummary();
}

} // namespace test_config
