// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
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

void testConfigFileIO() {
    // Create temporary config file
    std::string testFile = "test_config.cfg";
    
    {
        std::ofstream file(testFile);
        file << "# Test Configuration File\n";
        file << "key1 = value1\n";
        file << "key2 = 42\n";
        file << "key3 = true\n";
    }
    
    nlm::Config config;
    bool loaded = config.loadFromFile(testFile);
    assert(loaded);
    
    // Check loaded values
    auto val1 = config.get<std::string>("key1");
    assert(val1.has_value() && *val1 == "value1");
    
    auto val2 = config.get<int>("key2");
    assert(val2.has_value() && *val2 == 42);
    
    auto val3 = config.get<bool>("key3");
    assert(val3.has_value() && *val3 == true);
    
    // Test save functionality
    std::string outputFile = "test_output.cfg";
    bool saved = config.saveToFile(outputFile);
    assert(saved);
    
    // Verify saved content
    nlm::Config config2;
    config2.loadFromFile(outputFile);
    
    auto val1_2 = config2.get<std::string>("key1");
    assert(val1_2.has_value() && *val1_2 == "value1");
    
    // Clean up
    std::remove(testFile.c_str());
    std::remove(outputFile.c_str());
    
    std::cout << "    testConfigFileIO passed" << std::endl;
}

void testConfigSourcePriority() {
    nlm::Config config;
    
    // Set values from different sources
    config.set("key1", 1, nlm::ConfigSource::Default);
    config.set("key2", 2, nlm::ConfigSource::File);
    config.set("key3", 3, nlm::ConfigSource::CommandLine);
    config.set("key4", 4, nlm::ConfigSource::Runtime);
    
    // Check that all keys are present
    assert(config.has("key1"));
    assert(config.has("key2"));
    assert(config.has("key3"));
    assert(config.has("key4"));
    
    // Verify values
    assert(config.getOr<int>("key1", -1) == 1);
    assert(config.getOr<int>("key2", -1) == 2);
    assert(config.getOr<int>("key3", -1) == 3);
    assert(config.getOr<int>("key4", -1) == 4);
    
    // Test summary
    std::string summary = config.summary();
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    assert(summary.find("key3") != std::string::npos);
    assert(summary.find("key4") != std::string::npos);
    
    std::cout << "    testConfigSourcePriority passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test invalid key (empty string)
    config.set("", "invalid", nlm::ConfigSource::Runtime);  // Should log warning but not crash
    
    // Test get from non-existent key with default
    int val = config.getOr<int>("nonexistent", 999);
    assert(val == 999);
    
    // Test type mismatches
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("string_key", "hello", nlm::ConfigSource::Runtime);
    
    // Should return nullopt for wrong type
    auto wrong_type = config.get<double>("string_key");
    assert(!wrong_type.has_value());
    
    // Should return correct type
    auto correct_type = config.get<int>("int_key");
    assert(correct_type.has_value() && *correct_type == 42);
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigClear() {
    nlm::Config config;
    
    // Add some values
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", "value", nlm::ConfigSource::Runtime);
    config.set("key3", 3.14, nlm::ConfigSource::Runtime);
    
    assert(config.has("key1"));
    assert(config.has("key2"));
    assert(config.has("key3"));
    
    // Clear config
    config.clear();
    
    // Verify all keys are gone
    assert(!config.has("key1"));
    assert(!config.has("key2"));
    assert(!config.has("key3"));
    
    std::cout << "    testConfigClear passed" << std::endl;
}

void testConfigCommandLineParsing() {
    nlm::Config config;
    
    // Test command line argument parsing
    // Simulate: program --key1=value1 --key2=42 -key3=true
    char* args[] = {
        (char*)"program",
        (char*)"--key1=value1",
        (char*)"--key2=42",
        (char*)"-key3=true",
        nullptr
    };
    
    int argc = 4;
    bool loaded = config.loadFromArgs(argc, args);
    assert(loaded);
    
    // Verify parsed values
    auto val1 = config.get<std::string>("key1");
    assert(val1.has_value() && *val1 == "value1");
    
    auto val2 = config.get<int>("key2");
    assert(val2.has_value() && *val2 == 42);
    
    auto val3 = config.get<bool>("key3");
    assert(val3.has_value() && *val3 == true);
    
    std::cout << "    testConfigCommandLineParsing passed" << std::endl;
}

void runAll() {
    std::cout << "Running Config tests..." << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigFileIO();
    testConfigSourcePriority();
    testConfigErrorHandling();
    testConfigClear();
    testConfigCommandLineParsing();
    
    std::cout << "=== All Config tests passed ===" << std::endl;
}

} // namespace test_config