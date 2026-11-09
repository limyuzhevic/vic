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

void testConfigKeyValidation() {
    nlm::Config config;
    
    // Valid keys should work
    config.set("valid_key", 1, nlm::ConfigSource::Runtime);
    assert(config.has("valid_key"));
    
    // Invalid keys should be rejected
    bool validKeyTest = false;
    try {
        config.set("", 2, nlm::ConfigSource::Runtime); // Empty key
        validKeyTest = false;
    } catch (...) {
        validKeyTest = true; // Should throw or be rejected
    }
    
    assert(validKeyTest); // Empty key should be rejected
    
    // Keys with invalid characters should be rejected
    validKeyTest = false;
    try {
        config.set("invalid-key", 3, nlm::ConfigSource::Runtime); // Hyphen in key
        validKeyTest = false;
    } catch (...) {
        validKeyTest = true; // Should throw or be rejected
    }
    
    assert(validKeyTest); // Invalid key should be rejected
    
    std::cout << "    testConfigKeyValidation passed" << std::endl;
}

void testConfigSourcePriority() {
    nlm::Config config;
    
    // Set values with different sources
    config.set("test1", "default", nlm::ConfigSource::Default);
    config.set("test2", "file", nlm::ConfigSource::File);
    config.set("test3", "commandline", nlm::ConfigSource::CommandLine);
    config.set("test4", "runtime", nlm::ConfigSource::Runtime);
    
    // Check that sources are preserved
    auto test1 = config.get<std::string>("test1");
    assert(test1.has_value() && test1.value() == "default");
    
    auto test2 = config.get<std::string>("test2");
    assert(test2.has_value() && test2.value() == "file");
    
    auto test3 = config.get<std::string>("test3");
    assert(test3.has_value() && test3.value() == "commandline");
    
    auto test4 = config.get<std::string>("test4");
    assert(test4.has_value() && test4.value() == "runtime");
    
    // Override with higher priority source
    config.set("test1", "commandline_override", nlm::ConfigSource::CommandLine);
    auto test1Override = config.get<std::string>("test1");
    assert(test1Override.has_value() && test1Override.value() == "commandline_override");
    
    std::cout << "    testConfigSourcePriority passed" << std::endl;
}

void testConfigHelpAndVersion() {
    nlm::Config config;
    
    // Test that help and version methods exist and can be called
    // These are public methods added in the enhanced Config class
    try {
        config.printHelp();
        config.printVersion();
        // Note: printHelp and printVersion don't return anything, they just log
        std::cout << "    testConfigHelpAndVersion passed" << std::endl;
    } catch (...) {
        assert(false); // Should not throw
    }
}

void testConfigFileOperations() {
    nlm::Config config;
    
    // Set some test values
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello world", nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14159, nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    // Save to temporary file
    std::string tempFile = "test_config_save.txt";
    bool saveSuccess = config.saveToFile(tempFile);
    assert(saveSuccess);
    
    // Verify file was created
    std::ifstream file(tempFile);
    assert(file.good());
    
    // Load back from file
    bool loadSuccess = config.loadFromFile(tempFile);
    assert(loadSuccess);
    
    // Verify values were loaded correctly
    assert(config.has("test_int"));
    assert(config.get<int>("test_int").has_value() && config.get<int>("test_int").value() == 42);
    
    assert(config.has("test_string"));
    assert(config.get<std::string>("test_string").has_value() && 
           config.get<std::string>("test_string").value() == "hello world");
    
    assert(config.has("test_double"));
    assert(config.get<double>("test_double").has_value() && 
           config.get<double>("test_double").value() > 3.14158 && 
           config.get<double>("test_double").value() < 3.14160);
    
    assert(config.has("test_bool"));
    assert(config.get<bool>("test_bool").has_value() && 
           config.get<bool>("test_bool").value() == true);
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testConfigFileOperations passed" << std::endl;
}

void testConfigCommandLineArguments() {
    nlm::Config config;
    
    // Test command line argument parsing
    const char* argv[] = {
        "test_program",
        "--test_int", "123",
        "--test_string", "command_line_test",
        "--test_double", "1.618",
        "--test_bool", "true"
    };
    int argc = 5;
    
    bool loadSuccess = config.loadFromArgs(argc, const_cast<char**>(argv));
    assert(loadSuccess);
    
    // Verify values were set
    assert(config.has("test_int"));
    assert(config.get<int>("test_int").has_value() && config.get<int>("test_int").value() == 123);
    
    assert(config.has("test_string"));
    assert(config.get<std::string>("test_string").has_value() && 
           config.get<std::string>("test_string").value() == "command_line_test");
    
    assert(config.has("test_double"));
    assert(config.get<double>("test_double").has_value() && 
           config.get<double>("test_double").value() > 1.617 && 
           config.get<double>("test_double").value() < 1.619);
    
    assert(config.has("test_bool"));
    assert(config.get<bool>("test_bool").has_value() && 
           config.get<bool>("test_bool").value() == true);
    
    std::cout << "    testConfigCommandLineArguments passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test error handling for invalid keys
    bool errorHandlingTest = false;
    try {
        // This should either throw or return false
        bool result = config.set("invalid-key!", 1, nlm::ConfigSource::Runtime);
        errorHandlingTest = result; // Should be false or throw
    } catch (...) {
        errorHandlingTest = true; // Exception thrown
    }
    
    assert(errorHandlingTest); // Should handle error gracefully
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigValueTypes() {
    nlm::Config config;
    
    // Test various value types
    config.set("int8", static_cast<int8_t>(42), nlm::ConfigSource::Runtime);
    config.set("uint8", static_cast<uint8_t>(255), nlm::ConfigSource::Runtime);
    config.set("int16", static_cast<int16_t>(1000), nlm::ConfigSource::Runtime);
    config.set("uint16", static_cast<uint16_t>(65535), nlm::ConfigSource::Runtime);
    config.set("int32", static_cast<int32_t>(2000000000), nlm::ConfigSource::Runtime);
    config.set("uint32", static_cast<uint32_t>(4000000000), nlm::ConfigSource::Runtime);
    config.set("int64", static_cast<int64_t>(9000000000000000000), nlm::ConfigSource::Runtime);
    config.set("uint64", static_cast<uint64_t>(18000000000000000000), nlm::ConfigSource::Runtime);
    config.set("float", static_cast<float>(3.14f), nlm::ConfigSource::Runtime);
    config.set("double", static_cast<double>(2.718281828459045), nlm::ConfigSource::Runtime);
    config.set("string", std::string("test_string"), nlm::ConfigSource::Runtime);
    config.set("bool", true, nlm::ConfigSource::Runtime);
    
    // Verify all values can be retrieved
    assert(config.has("int8"));
    assert(config.get<int8_t>("int8").has_value());
    
    assert(config.has("uint8"));
    assert(config.get<uint8_t>("uint8").has_value());
    
    assert(config.has("int16"));
    assert(config.get<int16_t>("int16").has_value());
    
    assert(config.has("uint16"));
    assert(config.get<uint16_t>("uint16").has_value());
    
    assert(config.has("int32"));
    assert(config.get<int32_t>("int32").has_value());
    
    assert(config.has("uint32"));
    assert(config.get<uint32_t>("uint32").has_value());
    
    assert(config.has("int64"));
    assert(config.get<int64_t>("int64").has_value());
    
    assert(config.has("uint64"));
    assert(config.get<uint64_t>("uint64").has_value());
    
    assert(config.has("float"));
    assert(config.get<float>("float").has_value());
    
    assert(config.has("double"));
    assert(config.get<double>("double").has_value());
    
    assert(config.has("string"));
    assert(config.get<std::string>("string").has_value());
    
    assert(config.has("bool"));
    assert(config.get<bool>("bool").has_value());
    
    std::cout << "    testConfigValueTypes passed" << std::endl;
}

void testConfigComplexValues() {
    nlm::Config config;
    
    // Test complex vector values
    std::vector<int> intVector = {1, 2, 3, 4, 5};
    config.set("int_vector", intVector, nlm::ConfigSource::Runtime);
    
    auto retrievedVector = config.get<std::vector<int>>("int_vector");
    assert(retrievedVector.has_value());
    assert(retrievedVector.value().size() == 5);
    assert(retrievedVector.value()[0] == 1);
    assert(retrievedVector.value()[4] == 5);
    
    std::vector<double> doubleVector = {1.1, 2.2, 3.3, 4.4, 5.5};
    config.set("double_vector", doubleVector, nlm::ConfigSource::Runtime);
    
    auto retrievedDoubleVector = config.get<std::vector<double>>("double_vector");
    assert(retrievedDoubleVector.has_value());
    assert(retrievedDoubleVector.value().size() == 5);
    assert(retrievedDoubleVector.value()[0] > 1.09 && retrievedDoubleVector.value()[0] < 1.11);
    
    std::vector<std::string> stringVector = {"hello", "world", "test"};
    config.set("string_vector", stringVector, nlm::ConfigSource::Runtime);
    
    auto retrievedStringVector = config.get<std::vector<std::string>>("string_vector");
    assert(retrievedStringVector.has_value());
    assert(retrievedStringVector.value().size() == 3);
    assert(retrievedStringVector.value()[0] == "hello");
    assert(retrievedStringVector.value()[2] == "test");
    
    std::cout << "    testConfigComplexValues passed" << std::endl;
}

void runAll() {
    std::cout << "Running Config Tests..." << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigKeyValidation();
    testConfigSourcePriority();
    testConfigHelpAndVersion();
    testConfigFileOperations();
    testConfigCommandLineArguments();
    testConfigErrorHandling();
    testConfigValueTypes();
    testConfigComplexValues();
    
    std::cout << "All Config Tests passed!" << std::endl;
}

} // namespace test_config
