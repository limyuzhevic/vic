// Config Tests - Enhanced with error handling and file operations
#include "core/Config/Config.hpp"
#include "core/Exception.hpp"
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

void testConfigSaveLoad() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "test_string", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    
    // Create a temporary file for testing
    std::string testFile = "test_config_tmp.txt";
    
    try {
        // Save config
        bool saveSuccess = config.saveToFile(testFile);
        assert(saveSuccess);
        
        // Create new config and load
        nlm::Config config2;
        bool loadSuccess = config2.loadFromFile(testFile);
        assert(loadSuccess);
        
        // Verify values were loaded
        assert(config2.has("int_value"));
        assert(config2.has("string_value"));
        assert(config2.has("bool_value"));
        assert(config2.has("double_value"));
        
        auto intVal = config2.get<int>("int_value");
        assert(intVal.has_value());
        assert(*intVal == 42);
        
        auto stringVal = config2.get<std::string>("string_value");
        assert(stringVal.has_value());
        assert(*stringVal == "test_string");
        
        auto boolVal = config2.get<bool>("bool_value");
        assert(boolVal.has_value());
        assert(*boolVal == true);
        
        auto doubleVal = config2.get<double>("double_value");
        assert(doubleVal.has_value());
        assert(*doubleVal > 3.14158 && *doubleVal < 3.14160);
        
        std::cout << "    testConfigSaveLoad passed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "    testConfigSaveLoad FAILED: " << e.what() << std::endl;
        throw;
    }
    
    // Clean up
    std::remove(testFile.c_str());
}

void testConfigExceptionHandling() {
    nlm::Config config;
    
    // Test that missing keys return nullopt
    auto missing = config.get<int>("nonexistent");
    assert(!missing.has_value());
    
    // Test that getOr returns default for missing keys
    int defaultVal = config.getOr<int>("missing", 999);
    assert(defaultVal == 999);
    
    std::cout << "    testConfigExceptionHandling passed" << std::endl;
}

void testConfigSourceTracking() {
    nlm::Config config;
    
    config.set("default_key", 1, nlm::ConfigSource::Default);
    config.set("file_key", 2, nlm::ConfigSource::File);
    config.set("command_line_key", 3, nlm::ConfigSource::CommandLine);
    config.set("runtime_key", 4, nlm::ConfigSource::Runtime);
    
    // Test that sources are tracked correctly
    auto summary = config.summary();
    assert(summary.find("default_key") != std::string::npos);
    assert(summary.find("file_key") != std::string::npos);
    assert(summary.find("command_line_key") != std::string::npos);
    assert(summary.find("runtime_key") != std::string::npos);
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSaveLoad();
    testConfigExceptionHandling();
    testConfigSourceTracking();
}

} // namespace test_config
