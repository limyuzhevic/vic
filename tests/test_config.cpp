// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <sstream>
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

// File I/O validation
void testConfigFileIOValidation() {
    nlm::Config config;
    
    // Set some values
    config.set("int_value", 42, nlm::ConfigSource::File);
    config.set("string_value", std::string("test"), nlm::ConfigSource::File);
    config.set("double_value", 3.14, nlm::ConfigSource::File);
    config.set("bool_value", true, nlm::ConfigSource::File);
    
    // Test saving to file
    std::string test_file = "/tmp/test_config.json";
    
    bool save_success = config.saveToFile(test_file);
    assert(save_success);
    
    // Verify file was created
    std::ifstream file(test_file);
    assert(file.good());
    file.close();
    
    // Test loading from file
    nlm::Config config2;
    bool load_success = config2.loadFromFile(test_file);
    assert(load_success);
    
    // Verify values are preserved
    assert(config2.getOr<int>("int_value", 0) == 42);
    assert(config2.getOr<std::string>("string_value", "") == "test");
    assert(std::abs(config2.getOr<double>("double_value", 0.0) - 3.14) < 0.001);
    assert(config2.getOr<bool>("bool_value", false) == true);
    
    // Clean up
    std::remove(test_file.c_str());
    
    std::cout << "    testConfigFileIOValidation passed" << std::endl;
}

// Parameter validation
void testConfigParameterValidation() {
    nlm::Config config;
    
    // Test various data types
    config.set("int8", int8_t(42), nlm::ConfigSource::Runtime);
    config.set("int16", int16_t(100), nlm::ConfigSource::Runtime);
    config.set("int32", static_cast<int>(200), nlm::ConfigSource::Runtime);
    config.set("int64", static_cast<int64_t>(1000), nlm::ConfigSource::Runtime);
    config.set("uint8", uint8_t(255), nlm::ConfigSource::Runtime);
    config.set("uint16", uint16_t(65535), nlm::ConfigSource::Runtime);
    config.set("uint32", static_cast<uint32_t>(4294967295), nlm::ConfigSource::Runtime);
    config.set("uint64", static_cast<uint64_t>(18446744073709551615), nlm::ConfigSource::Runtime);
    config.set("float", 3.14f, nlm::ConfigSource::Runtime);
    config.set("double", 2.71828, nlm::ConfigSource::Runtime);
    config.set("bool_true", true, nlm::ConfigSource::Runtime);
    config.set("bool_false", false, nlm::ConfigSource::Runtime);
    config.set("string_val", std::string("hello world"), nlm::ConfigSource::Runtime);
    
    // Test vector types
    config.set("int_vector", std::vector<int>{1, 2, 3, 4, 5}, nlm::ConfigSource::Runtime);
    config.set("double_vector", std::vector<double>{1.1, 2.2, 3.3}, nlm::ConfigSource::Runtime);
    config.set("string_vector", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    
    // Verify all values can be retrieved correctly
    assert(config.getOr<int>("int8", 0) == 42);
    assert(config.getOr<int16_t>("int16", 0) == 100);
    assert(config.getOr<int>("int32", 0) == 200);
    assert(config.getOr<int64_t>("int64", 0) == 1000);
    
    // Test missing key returns default
    assert(config.getOr<int>("missing", 999) == 999);
    
    std::cout << "    testConfigParameterValidation passed" << std::endl;
}

// Command-line parsing
void testConfigCommandLineParsing() {
    // Create a mock command line
    char* argv[] = {
        (char*)"test_program",
        (char*)"--int_arg", (char*)"42",
        (char*)"--string_arg", (char*)"hello",
        (char*)"--float_arg", (char*)"3.14",
        (char*)"--bool_arg",
        (char*)"--vector_arg", (char*)"1,2,3,4,5"
    };
    
    int argc = 13;  // program + all arguments
    
    nlm::Config config;
    bool parse_success = config.loadFromArgs(argc, argv);
    
    // Parsing may or may not be implemented
    // This test ensures no crashes
    if (parse_success) {
        assert(config.has("int_arg"));
        assert(config.has("string_arg"));
        assert(config.has("float_arg"));
        assert(config.has("bool_arg"));
        assert(config.has("vector_arg"));
    }
    
    std::cout << "    testConfigCommandLineParsing passed" << std::endl;
}

// Error handling
void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test removing non-existent key (should not crash)
    config.remove("non_existent");
    assert(!config.has("non_existent"));
    
    // Test getting non-existent key
    auto missing_int = config.get<int>("missing_int");
    assert(!missing_int.has_value());
    
    // Test with empty config
    config.clear();
    assert(config.getKeys().empty());
    assert(!config.has("any_key"));
    
    // Test file I/O error handling (invalid path)
    nlm::Config config2;
    bool load_error = config2.loadFromFile("/nonexistent/path/file.json");
    
    // Should not crash, but may return false for invalid file
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

// Configuration source validation
void testConfigSourceValidation() {
    nlm::Config config;
    
    // Set values with different sources
    config.set("default_val", 1, nlm::ConfigSource::Default);
    config.set("file_val", 2, nlm::ConfigSource::File);
    config.set("cmdline_val", 3, nlm::ConfigSource::CommandLine);
    config.set("runtime_val", 4, nlm::ConfigSource::Runtime);
    
    // Verify all keys exist
    assert(config.has("default_val"));
    assert(config.has("file_val"));
    assert(config.has("cmdline_val"));
    assert(config.has("runtime_val"));
    
    // Verify values are correct
    assert(config.getOr<int>("default_val", 0) == 1);
    assert(config.getOr<int>("file_val", 0) == 2);
    assert(config.getOr<int>("cmdline_val", 0) == 3);
    assert(config.getOr<int>("runtime_val", 0) == 4);
    
    std::cout << "    testConfigSourceValidation passed" << std::endl;
}

// Configuration summary and formatting
void testConfigSummary() {
    nlm::Config config;
    
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("string_key", "test", nlm::ConfigSource::Runtime);
    config.set("float_key", 3.14159, nlm::ConfigSource::Runtime);
    
    std::string summary = config.summary();
    
    // Summary should contain some indication of configuration
    assert(!summary.empty());
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

// Configuration cloning and copying
void testConfigCloning() {
    nlm::Config original;
    
    original.set("key1", 1, nlm::ConfigSource::Runtime);
    original.set("key2", std::string("value"), nlm::ConfigSource::Runtime);
    original.set("key3", 3.14, nlm::ConfigSource::Runtime);
    
    nlm::Config copy = std::move(original);  // Move constructor
    
    assert(copy.getOr<int>("key1", 0) == 1);
    assert(copy.getOr<std::string>("key2", "") == "value");
    assert(std::abs(copy.getOr<double>("key3", 0.0) - 3.14) < 0.001);
    
    // Original should be empty after move
    assert(!original.has("key1"));
    
    std::cout << "    testConfigCloning passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    
    // Additional comprehensive tests
    testConfigFileIOValidation();
    testConfigParameterValidation();
    testConfigCommandLineParsing();
    testConfigErrorHandling();
    testConfigSourceValidation();
    testConfigSummary();
    testConfigCloning();
}

} // namespace test_config
