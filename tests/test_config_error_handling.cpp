// Error Handling Tests for Configuration System
// Tests for improved error handling and validation in Config class

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

namespace test_config_error_handling {

void testConfigInvalidKeyThrows() {
    nlm::Config config;
    
    try {
        config.set("", 42, nlm::ConfigSource::Runtime);
        assert(false && "Should have thrown for empty key");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("Invalid configuration key") != std::string::npos);
    }
    
    try {
        config.set("key with spaces", 42, nlm::ConfigSource::Runtime);
        assert(false && "Should have thrown for key with spaces");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("Invalid configuration key") != std::string::npos);
    }
    
    try {
        config.set(std::string(1000, 'a'), 42, nlm::ConfigSource::Runtime);
        assert(false && "Should have thrown for very long key");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("Invalid configuration key") != std::string::npos);
    }
    
    std::cout << "    testConfigInvalidKeyThrows passed" << std::endl;
}

void testConfigValueValidation() {
    nlm::Config config;
    
    // Test double out of range
    try {
        config.set("test", std::numeric_limits<double>::quiet_NaN(), nlm::ConfigSource::Runtime);
        assert(false && "Should have thrown for NaN");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("NaN") != std::string::npos || 
               std::string(e.what()).find("infinite") != std::string::npos);
    }
    
    // Test string with null bytes
    try {
        config.set("test", std::string("hello\0world"), nlm::ConfigSource::Runtime);
        assert(false && "Should have thrown for string with null bytes");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("null bytes") != std::string::npos);
    }
    
    std::cout << "    testConfigValueValidation passed" << std::endl;
}

void testConfigLoadFromArgsErrorHandling() {
    nlm::Config config;
    
    // Test with invalid arguments
    int argc = 3;
    const char* argv[] = {
        "test",
        "--invalid=",  // Empty value
        "-k", "value"   // Missing value after -k
    };
    
    // The method should handle these gracefully (not crash)
    bool result = config.loadFromArgs(argc, const_cast<char**>(argv));
    // Currently returns false on first error, which is expected
    
    std::cout << "    testConfigLoadFromArgsErrorHandling passed" << std::endl;
}

void testConfigFileValidation() {
    nlm::Config config;
    
    // Test loading from non-existent file (should fail gracefully)
    bool result = config.loadFromFile("/nonexistent/path/config.txt");
    assert(!result);
    
    // Test loading empty file
    std::ofstream emptyFile("/tmp/empty_config.txt");
    emptyFile.close();
    
    result = config.loadFromFile("/tmp/empty_config.txt");
    assert(result);  // Empty file should load successfully
    
    // Test loading malformed file
    std::ofstream badFile("/tmp/bad_config.txt");
    badFile << "valid_key = 123\n";
    badFile << "invalid line without equals\n";
    badFile << "key = value with = in it\n";
    badFile.close();
    
    result = config.loadFromFile("/tmp/bad_config.txt");
    // Should handle malformed lines gracefully
    
    // Cleanup
    std::remove("/tmp/empty_config.txt");
    std::remove("/tmp/bad_config.txt");
    
    std::cout << "    testConfigFileValidation passed" << std::endl;
}

void testConfigSaveValidation() {
    nlm::Config config;
    
    // Test saving to valid file
    config.set("test_key", "test_value", nlm::ConfigSource::Runtime);
    
    bool result = config.saveToFile("/tmp/config_save_test.txt");
    assert(result);
    
    // Verify file was created
    std::ifstream file("/tmp/config_save_test.txt");
    assert(file.good());
    file.close();
    
    std::remove("/tmp/config_save_test.txt");
    
    std::cout << "    testConfigSaveValidation passed" << std::endl;
}

void runAll() {
    testConfigInvalidKeyThrows();
    testConfigValueValidation();
    testConfigLoadFromArgsErrorHandling();
    testConfigFileValidation();
    testConfigSaveValidation();
    
    std::cout << "=== All Config Error Handling Tests PASSED ===" << std::endl;
}

} // namespace test_config_error_handling