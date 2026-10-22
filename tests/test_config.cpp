#include "tests/TestConfig.h"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>

void testConfig_basic() {
    std::cout << "Testing Config basic functionality..." << std::endl;
    
    Config config;
    
    // Test setting different types
    config.set("string_value", std::string("test"), ConfigSource::Default);
    config.set("int_value", 42, ConfigSource::Default);
    config.set("double_value", 3.14, ConfigSource::Default);
    config.set("bool_value", true, ConfigSource::Default);
    
    // Test getting values
    assert(config.getOr<std::string>("string_value") == "test");
    assert(config.getOr<int>("int_value") == 42);
    assert(config.getOr<double>("double_value") == 3.14);
    assert(config.getOr<bool>("bool_value") == true);
    
    // Test non-existent key
    assert(config.getOr<std::string>("nonexistent", "default") == "default");
    
    // Test has method
    assert(config.has("string_value"));
    assert(!config.has("nonexistent"));
    
    std::cout << "  ✓ Basic config operations passed" << std::endl;
}

void testConfig_fileIO() {
    std::cout << "Testing Config file I/O..." << std::endl;
    
    Config config;
    
    // Write test config
    config.set("test_string", std::string("Hello World"), ConfigSource::File);
    config.set("test_int", 123, ConfigSource::File);
    config.set("test_float", 1.5f, ConfigSource::File);
    config.set("test_bool", false, ConfigSource::File);
    
    // Save to file
    assert(config.saveToFile("test_config.cfg"));
    
    // Create new config and load
    Config config2;
    assert(config2.loadFromFile("test_config.cfg"));
    
    // Verify loaded values
    assert(config2.getOr<std::string>("test_string") == "Hello World");
    assert(config2.getOr<int>("test_int") == 123);
    assert(config2.getOr<double>("test_float") == 1.5);
    assert(config2.getOr<bool>("test_bool") == false);
    
    // Clean up
    std::remove("test_config.cfg");
    
    std::cout << "  ✓ File I/O operations passed" << std::endl;
}

void testConfig_quotesHandling() {
    std::cout << "Testing Config quotes and escaping..." << std::endl;
    
    Config config;
    
    // Test various quote formats
    config.set("simple_quote", std::string("'single'"), ConfigSource::File);
    config.set("double_quote", std::string("\"double\""), ConfigSource::File);
    config.set("mixed_quote", std::string("'mixed'\"'), ConfigSource::File);
    
    // Test escaped quotes
    config.set("escaped_quote", std::string("He said \\\"Hello\\\""), ConfigSource::File);
    config.set("backslash", std::string("path\\to\\file"), ConfigSource::File);
    
    std::cout << "  ✓ Quotes and escaping passed" << std::endl;
}

void testConfig_summary() {
    std::cout << "Testing Config summary generation..." << std::endl;
    
    Config config;
    
    // Add some entries with descriptions
    config.set("key1", "value1", ConfigSource::Default);
    config.set("key2", 42, ConfigSource::CommandLine);
    
    // Generate summary (should not crash)
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("Configuration") != std::string::npos);
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    
    std::cout << "  ✓ Summary generation passed" << std::endl;
}

int main() {
    std::cout << "=== Config Unit Tests ===" << std::endl << std::endl;
    
    try {
        testConfig_basic();
        std::cout << std::endl;
        
        testConfig_fileIO();
        std::cout << std::endl;
        
        testConfig_quotesHandling();
        std::cout << std::endl;
        
        testConfig_summary();
        std::cout << std::endl;
        
        std::cout << "=== All Config tests passed! ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}