// Enhanced Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_enhanced_config {

void testConfigFileParsing() {
    nlm::Config config;
    
    // Test parsing simple key=value files
    std::ofstream file("test_config.txt");
    file << "# Test config file\n";
    file << "int_value = 42\n";
    file << "double_value = 3.14159\n";
    file << "string_value = \"hello world\"\n";
    file << "bool_value = true\n";
    file << "# Comment line\n";
    file << "another_int = -5\n";
    file.close();
    
    bool loaded = config.loadFromFile("test_config.txt");
    assert(loaded);
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.14 && *doubleVal < 3.142);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello world");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    auto negInt = config.get<int>("another_int");
    assert(negInt.has_value());
    assert(*negInt == -5);
    
    // Clean up
    std::remove("test_config.txt");
    
    std::cout << "    testConfigFileParsing passed" << std::endl;
}

void testConfigSaveToFile() {
    nlm::Config config;
    
    // Set up test values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::File);
    config.set("string_value", std::string("test string"), nlm::ConfigSource::CommandLine);
    config.set("bool_value", false, nlm::ConfigSource::Runtime);
    
    // Save to file
    bool saved = config.saveToFile("test_save_config.txt");
    assert(saved);
    
    // Read back and verify
    nlm::Config config2;
    bool loaded = config2.loadFromFile("test_save_config.txt");
    assert(loaded);
    
    // Check that key names are preserved
    assert(config2.has("int_value"));
    assert(config2.has("double_value"));
    assert(config2.has("string_value"));
    assert(config2.has("bool_value"));
    
    // Verify types are preserved
    auto intVal2 = config2.get<int>("int_value");
    assert(intVal2.has_value());
    assert(*intVal2 == 42);
    
    auto doubleVal2 = config2.get<double>("double_value");
    assert(doubleVal2.has_value());
    assert(*doubleVal2 > 3.14 && *doubleVal2 < 3.142);
    
    auto stringVal2 = config2.get<std::string>("string_value");
    assert(stringVal2.has_value());
    assert(*stringVal2 == "test string");
    
    auto boolVal2 = config2.get<bool>("bool_value");
    assert(boolVal2.has_value());
    assert(*boolVal2 == false);
    
    // Clean up
    std::remove("test_save_config.txt");
    
    std::cout << "    testConfigSaveToFile passed" << std::endl;
}

void testConfigLoadFromArgs() {
    nlm::Config config;
    
    // Simulate command line arguments
    int argc = 5;
    char* argv[] = {
        (char*)"program",
        (char*)"--int_value=100",
        (char*)"-double_value=2.718",
        (char*)"-string_value=\"command line\"",
        (char*)"--bool_value=true"
    };
    
    bool loaded = config.loadFromArgs(argc, argv);
    assert(loaded);
    
    // Check that command line arguments were parsed
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 100);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 2.71 && *doubleVal < 2.73);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "command line");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigLoadFromArgs passed" << std::endl;
}

void testConfigValueTypes() {
    nlm::Config config;
    
    // Test various value types
    config.set("int", 42, nlm::ConfigSource::Runtime);
    config.set("int64", int64_t(123456789012345), nlm::ConfigSource::Runtime);
    config.set("double", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string", std::string("test string"), nlm::ConfigSource::Runtime);
    config.set("bool_true", true, nlm::ConfigSource::Runtime);
    config.set("bool_false", false, nlm::ConfigSource::Runtime);
    
    // Test integer retrieval
    auto intVal = config.get<int>("int");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto int64Val = config.get<int64_t>("int64");
    assert(int64Val.has_value());
    assert(*int64Val == 123456789012345);
    
    // Test double retrieval
    auto doubleVal = config.get<double>("double");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.14 && *doubleVal < 3.142);
    
    // Test string retrieval
    auto stringVal = config.get<std::string>("string");
    assert(stringVal.has_value());
    assert(*stringVal == "test string");
    
    // Test boolean retrieval
    auto boolTrueVal = config.get<bool>("bool_true");
    assert(boolTrueVal.has_value());
    assert(*boolTrueVal == true);
    
    auto boolFalseVal = config.get<bool>("bool_false");
    assert(boolFalseVal.has_value());
    assert(*boolFalseVal == false);
    
    std::cout << "    testConfigValueTypes passed" << std::endl;
}

void testConfigEdgeCases() {
    nlm::Config config;
    
    // Test empty config
    assert(!config.has("nonexistent"));
    assert(config.getOr<int>("nonexistent", 999) == 999);
    
    // Test case sensitivity
    config.set("TestKey", 123, nlm::ConfigSource::Runtime);
    assert(config.has("TestKey"));
    assert(!config.has("testkey"));  // Should be case sensitive
    
    // Test overwriting values
    config.set("TestKey", 456, nlm::ConfigSource::Runtime);
    auto value = config.get<int>("TestKey");
    assert(value.has_value());
    assert(*value == 456);  // Should be overwritten
    
    // Test empty string
    config.set("empty_string", std::string(""), nlm::ConfigSource::Runtime);
    auto emptyStr = config.get<std::string>("empty_string");
    assert(emptyStr.has_value());
    assert(emptyStr.value() == "");
    
    // Test negative numbers
    config.set("negative_int", -100, nlm::ConfigSource::Runtime);
    auto negInt = config.get<int>("negative_int");
    assert(negInt.has_value());
    assert(*negInt == -100);
    
    // Test large numbers
    config.set("large_double", 1.23e-10, nlm::ConfigSource::Runtime);
    auto largeDouble = config.get<double>("large_double");
    assert(largeDouble.has_value());
    assert(*largeDouble > 0 && *largeDouble < 1e-9);
    
    std::cout << "    testConfigEdgeCases passed" << std::endl;
}

void testConfigSourceTracking() {
    nlm::Config config;
    
    // Set values from different sources
    config.set("default_val", 100, nlm::ConfigSource::Default);
    config.set("file_val", 200, nlm::ConfigSource::File);
    config.set("command_line_val", 300, nlm::ConfigSource::CommandLine);
    config.set("runtime_val", 400, nlm::ConfigSource::Runtime);
    
    // Verify all values are set
    assert(config.get<int>("default_val").value() == 100);
    assert(config.get<int>("file_val").value() == 200);
    assert(config.get<int>("command_line_val").value() == 300);
    assert(config.get<int>("runtime_val").value() == 400);
    
    // Get all keys and verify count
    auto keys = config.getKeys();
    assert(keys.size() == 4);
    
    // Check that summary works (doesn't crash)
    std::string summary = config.summary();
    assert(!summary.empty());
    
    // Test clear function
    config.clear();
    assert(config.getKeys().size() == 0);
    assert(!config.has("default_val"));
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

void testConfigComparison() {
    nlm::Config config1;
    nlm::Config config2;
    
    // Set same values in both configs
    config1.set("key1", 1, nlm::ConfigSource::Runtime);
    config1.set("key2", "value", nlm::ConfigSource::Runtime);
    
    config2.set("key1", 1, nlm::ConfigSource::Runtime);
    config2.set("key2", "value", nlm::ConfigSource::Runtime);
    
    // They should have same keys
    assert(config1.getKeys().size() == config2.getKeys().size());
    
    // Different configs should be independent
    config2.set("key3", 999, nlm::ConfigSource::Runtime);
    assert(!config1.has("key3"));
    assert(config2.has("key3"));
    
    std::cout << "    testConfigComparison passed" << std::endl;
}

void runAllEnhanced() {
    testConfigFileParsing();
    testConfigSaveToFile();
    testConfigLoadFromArgs();
    testConfigValueTypes();
    testConfigEdgeCases();
    testConfigSourceTracking();
    testConfigComparison();
}

} // namespace test_enhanced_config
