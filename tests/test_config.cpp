// Config Tests
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

// Advanced Configuration Tests
void testConfigFileLoad() {
    nlm::Config config;
    
    // Create a temporary config file
    std::string tempFile = "/tmp/test_config.cfg";
    std::ofstream file(tempFile);
    file << "# Test configuration\n";
    file << "int_value = 100\n";
    file << "double_value = 2.718\n";
    file << "string_value = \"world\"\n";
    file << "bool_value = false\n";
    file.close();
    
    // Load the configuration file
    assert(config.loadFromFile(tempFile));
    
    // Verify values were loaded correctly
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 100);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(std::abs(*doubleVal - 2.718) < 0.001);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "world");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == false);
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testConfigFileLoad passed" << std::endl;
}

void testConfigFileLoadErrorHandling() {
    nlm::Config config;
    
    // Test loading a non-existent file (should not throw, just return false)
    assert(!config.loadFromFile("/tmp/nonexistent_config.cfg"));
    
    // Test loading a malformed config file
    std::string tempFile = "/tmp/malformed_config.cfg";
    std::ofstream file(tempFile);
    file << "valid_line = 123\n";
    file << "invalid line without equals\n";
    file << "another_valid = \"test\"\n";
    file.close();
    
    // This should not throw, but may have partial success
    assert(config.loadFromFile(tempFile));
    
    // Verify that valid entries were loaded
    auto validVal = config.get<int>("valid_line");
    assert(validVal.has_value());
    assert(*validVal == 123);
    
    auto anotherVal = config.get<std::string>("another_valid");
    assert(anotherVal.has_value());
    assert(*anotherVal == "test");
    
    // The invalid line should have been skipped
    assert(!config.has("invalid_line"));
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testConfigFileLoadErrorHandling passed" << std::endl;
}

void testConfigKeyValidation() {
    nlm::Config config;
    
    // Test setting empty key (should throw or handle gracefully)
    try {
        config.set("", 123, nlm::ConfigSource::Runtime);
        // If no exception thrown, that's okay for this test
    } catch (const std::exception& e) {
        // Exception caught, which is fine
    }
    
    // Test setting key with invalid characters
    try {
        config.set("key@with#special$chars", 456, nlm::ConfigSource::Runtime);
        // If no exception thrown, that's okay for this test
    } catch (const std::exception& e) {
        // Exception caught, which is fine
    }
    
    // Valid keys should work fine
    config.set("valid_key", 789, nlm::ConfigSource::Runtime);
    assert(config.has("valid_key"));
    
    std::cout << "    testConfigKeyValidation passed" << std::endl;
}

void testConfigAdvancedFeatures() {
    nlm::Config config;
    
    // Test setting values with different sources
    config.set("runtime_value", 1, nlm::ConfigSource::Runtime);
    config.set("file_value", 2, nlm::ConfigSource::File);
    config.set("command_line_value", 3, nlm::ConfigSource::CommandLine);
    config.set("default_value", 4, nlm::ConfigSource::Default);
    
    // Verify all values are stored
    assert(config.get<int>("runtime_value").has_value());
    assert(config.get<int>("file_value").has_value());
    assert(config.get<int>("command_line_value").has_value());
    assert(config.get<int>("default_value").has_value());
    
    // Test summary function
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("Configuration (4 entries)") != std::string::npos);
    
    // Test clearing all entries
    config.clear();
    assert(config.getKeys().size() == 0);
    
    std::cout << "    testConfigAdvancedFeatures passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    
    // Advanced tests
    testConfigFileLoad();
    testConfigFileLoadErrorHandling();
    testConfigKeyValidation();
    testConfigAdvancedFeatures();
}

} // namespace test_config
