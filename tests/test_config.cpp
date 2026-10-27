// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

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

// Test type safety and error handling
void testConfigTypeSafety() {
    nlm::Config config;
    
    // Set different types of values
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_val", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    config.set("vector_int", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    config.set("vector_double", std::vector<double>{1.1, 2.2, 3.3}, nlm::ConfigSource::Runtime);
    config.set("vector_string", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    
    // Test type-safe get methods
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config.get<double>("double_val");
    assert(doubleVal.has_value());
    
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal.has_value());
    assert(*stringVal == "test");
    
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Test type checking with getWithValidation
    auto intValValidated = config.getWithValidation<int>("int_val");
    assert(intValValidated.has_value());
    assert(*intValValidated == 42);
    
    // Test that wrong type returns std::nullopt
    auto wrongTypeVal = config.getWithValidation<int>("double_val");
    assert(!wrongTypeVal.has_value());
    
    std::cout << "    testConfigTypeSafety passed" << std::endl;
}

// Test JSON file loading
void testConfigJSONLoading() {
    nlm::Config config;
    
    // Test loading from a JSON string
    std::stringstream jsonStream;
    jsonStream << "{\n  \"int_val\": 42,\n  \"double_val\": 3.14,\n  \"string_val\": \"hello\",\n  \"bool_val\": true,\n  \"vector_int\": [1, 2, 3]\n}";
    
    // Create a temporary file with JSON content
    std::ofstream tempFile("temp_test_config.json");
    tempFile << jsonStream.str();
    tempFile.close();
    
    // Load from JSON file
    bool loadSuccess = config.parseJSON("temp_test_config.json");
    assert(loadSuccess);
    
    // Verify loaded values
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config.get<double>("double_val");
    assert(doubleVal.has_value());
    
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Verify vector was loaded
    auto vectorVal = config.get<std::vector<int>>("vector_int");
    assert(vectorVal.has_value());
    assert(vectorVal.value().size() == 3);
    assert(vectorVal.value()[0] == 1);
    
    // Clean up
    std::remove("temp_test_config.json");
    
    std::cout << "    testConfigJSONLoading passed" << std::endl;
}

// Test simple format loading (backward compatibility)
void testConfigSimpleFormatLoading() {
    nlm::Config config;
    
    // Create a temporary file with key=value format
    std::ofstream tempFile("temp_test_config.cfg");
    tempFile << "# Test configuration file\n";
    tempFile << "# This is a comment\n";
    tempFile << "int_val = 42\n";
    tempFile << "double_val = 3.14\n";
    tempFile << "string_val = \"hello\"\n";
    tempFile << "bool_val = true\n";
    tempFile << "# Another comment\n";
    tempFile.close();
    
    // Load from simple format file
    bool loadSuccess = config.loadFromSimpleFormat("temp_test_config.cfg");
    assert(loadSuccess);
    
    // Verify loaded values
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config.get<double>("double_val");
    assert(doubleVal.has_value());
    assert(std::abs(*doubleVal - 3.14) < 0.01);
    
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Clean up
    std::remove("temp_test_config.cfg");
    
    std::cout << "    testConfigSimpleFormatLoading passed" << std::endl;
}

// Test error handling
void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test loading from non-existent file
    bool loadSuccess = config.loadFromSimpleFormat("non_existent_file.cfg");
    assert(!loadSuccess);  // Should return false
    
    // Test setting with invalid values (should handle gracefully)
    config.set("test_int", "not_a_number", nlm::ConfigSource::Runtime);
    // The set method should convert string to ConfigValue internally
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

// Test JSON export
void testConfigJSONExport() {
    nlm::Config config;
    
    // Set some values
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_val", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    config.set("vector_int", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    
    // Export to JSON
    bool exportSuccess = config.exportToJSON("temp_test_export.json");
    assert(exportSuccess);
    
    // Verify JSON file was created and has content
    std::ifstream exportFile("temp_test_export.json");
    assert(exportFile.is_open());
    
    std::string line;
    bool foundIntVal = false;
    bool foundDoubleVal = false;
    bool foundStringVal = false;
    
    while (std::getline(exportFile, line)) {
        if (line.find("int_val") != std::string::npos) {
            foundIntVal = true;
        }
        if (line.find("double_val") != std::string::npos) {
            foundDoubleVal = true;
        }
        if (line.find("string_val") != std::string::npos) {
            foundStringVal = true;
        }
    }
    
    assert(foundIntVal);
    assert(foundDoubleVal);
    assert(foundStringVal);
    
    // Clean up
    exportFile.close();
    std::remove("temp_test_export.json");
    
    std::cout << "    testConfigJSONExport passed" << std::endl;
}

// Test configuration validation
void testConfigValidation() {
    nlm::Config config;
    
    // Test has() method
    assert(!config.has("non_existent_key"));
    
    config.set("existing_key", "value", nlm::ConfigSource::Runtime);
    assert(config.has("existing_key"));
    
    // Test getKeys() method
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    // Verify all keys are present
    bool hasKey1 = false, hasKey2 = false, hasKey3 = false;
    for (const auto& key : keys) {
        if (key == "key1") hasKey1 = true;
        if (key == "key2") hasKey2 = true;
        if (key == "key3") hasKey3 = true;
    }
    assert(hasKey1 && hasKey2 && hasKey3);
    
    // Test remove() method
    config.remove("key2");
    assert(!config.has("key2"));
    assert(config.has("key1"));
    assert(config.has("key3"));
    
    // Test clear() method
    config.clear();
    assert(!config.has("key1"));
    assert(!config.has("key2"));
    assert(!config.has("key3"));
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

// Test configuration source tracking
void testConfigSourceTracking() {
    nlm::Config config;
    
    config.set("runtime_key", 1, nlm::ConfigSource::Runtime);
    config.set("file_key", 2, nlm::ConfigSource::File);
    config.set("command_line_key", 3, nlm::ConfigSource::CommandLine);
    config.set("default_key", 4, nlm::ConfigSource::Default);
    
    // All keys should be present
    assert(config.has("runtime_key"));
    assert(config.has("file_key"));
    assert(config.has("command_line_key"));
    assert(config.has("default_key"));
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigTypeSafety();
    testConfigJSONLoading();
    testConfigSimpleFormatLoading();
    testConfigErrorHandling();
    testConfigJSONExport();
    testConfigValidation();
    testConfigSourceTracking();
}

} // namespace test_config
