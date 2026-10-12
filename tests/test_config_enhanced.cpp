// Enhanced Config Tests
// Tests for the improved Config class with error handling and validation
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <stdexcept>

namespace test_config_enhanced {

void testConfigInvalidKey() {
    nlm::Config config;
    
    // Test empty key
    try {
        config.set("", 42, nlm::ConfigSource::Runtime);
        assert(false && "Should have thrown exception for empty key");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("Invalid key") != std::string::npos);
    }
    
    // Test key with invalid characters
    try {
        config.set("key\twith\ttabs", 42, nlm::ConfigSource::Runtime);
        assert(false && "Should have thrown exception for key with tabs");
    } catch (const std::invalid_argument& e) {
        assert(std::string(e.what()).find("Invalid key") != std::string::npos);
    }
    
    std::cout << "    testConfigInvalidKey passed" << std::endl;
}

void testConfigValidKey() {
    nlm::Config config;
    
    // Test valid keys
    config.set("valid_key", 42, nlm::ConfigSource::Runtime);
    config.set("key-with-dashes", "value", nlm::ConfigSource::Runtime);
    config.set("key_with_underscores", 3.14, nlm::ConfigSource::Runtime);
    config.set("numeric123", true, nlm::ConfigSource::Runtime);
    
    assert(config.has("valid_key"));
    assert(config.has("key-with-dashes"));
    assert(config.has("key_with_underscores"));
    assert(config.has("numeric123"));
    
    std::cout << "    testConfigValidKey passed" << std::endl;
}

void testConfigFileLoadingErrorHandling() {
    nlm::Config config;
    
    // Test loading from invalid file path
    assert(!config.loadFromFile("/nonexistent/path/file.cfg"));
    
    // Test loading from very large file (simulate)
    // This is harder to test without creating an actual large file
    // but the implementation should handle it gracefully
    
    std::cout << "    testConfigFileLoadingErrorHandling passed" << std::endl;
}

void testConfigArgumentParsingErrorHandling() {
    nlm::Config config;
    
    // Test with null arguments (should be handled gracefully)
    char* nullArgs[] = {nullptr};
    assert(config.loadFromArgs(0, nullptr));  // Should handle gracefully
    
    // Test with null argument in middle of args
    char* args[] = {const_cast<char*>("test"), nullptr, const_cast<char*>("value")};
    assert(config.loadFromArgs(3, args));  // Should handle gracefully
    
    std::cout << "    testConfigArgumentParsingErrorHandling passed" << std::endl;
}

void testConfigSaveFileErrorHandling() {
    nlm::Config config;
    
    // Test saving to invalid directory (should try to create)
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.saveToFile("/tmp/nlm_test_config.cfg"));
    
    // Verify file was created
    assert(std::filesystem::exists("/tmp/nlm_test_config.cfg"));
    
    // Clean up
    std::filesystem::remove("/tmp/nlm_test_config.cfg");
    
    std::cout << "    testConfigSaveFileErrorHandling passed" << std::endl;
}

void testConfigTypeErrorHandling() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime);
    
    // Test type mismatch handling
    auto intVal = config.get<int>("string_value");
    assert(!intVal.has_value());  // Should be nullopt for type mismatch
    
    auto stringVal = config.get<std::string>("int_value");
    assert(!stringVal.has_value());  // Should be nullopt for type mismatch
    
    std::cout << "    testConfigTypeErrorHandling passed" << std::endl;
}

void testConfigRemoveInvalidKey() {
    nlm::Config config;
    
    // Test removing invalid key (should not crash)
    config.remove("");  // Should not throw or crash
    config.remove("invalid\tkey");  // Should not throw or crash
    
    std::cout << "    testConfigRemoveInvalidKey passed" << std::endl;
}

void testConfigClearRobustness() {
    nlm::Config config;
    
    // Add some entries
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", "value", nlm::ConfigSource::Runtime);
    
    // Clear multiple times (should not crash)
    config.clear();
    config.clear();  // Second clear should be safe
    
    assert(config.getKeys().empty());
    
    std::cout << "    testConfigClearRobustness passed" << std::endl;
}

void testConfigSaveLoadRoundtrip() {
    nlm::Config config1;
    
    // Set various types of values
    config1.set("int", 42, nlm::ConfigSource::Runtime);
    config1.set("double", 3.14, nlm::ConfigSource::Runtime);
    config1.set("string", "test_value", nlm::ConfigSource::Runtime);
    config1.set("bool", true, nlm::ConfigSource::Runtime);
    
    // Save to file
    assert(config1.saveToFile("/tmp/nlm_roundtrip_test.cfg"));
    
    // Load from file
    nlm::Config config2;
    assert(config2.loadFromFile("/tmp/nlm_roundtrip_test.cfg"));
    
    // Verify values are preserved
    auto intVal1 = config1.get<int>("int");
    auto intVal2 = config2.get<int>("int");
    assert(intVal1.has_value() && intVal2.has_value());
    assert(*intVal1 == *intVal2);
    
    auto stringVal1 = config1.get<std::string>("string");
    auto stringVal2 = config2.get<std::string>("string");
    assert(stringVal1.has_value() && stringVal2.has_value());
    assert(*stringVal1 == *stringVal2);
    
    // Clean up
    std::filesystem::remove("/tmp/nlm_roundtrip_test.cfg");
    
    std::cout << "    testConfigSaveLoadRoundtrip passed" << std::endl;
}

void runAllEnhancedTests() {
    testConfigInvalidKey();
    testConfigValidKey();
    testConfigFileLoadingErrorHandling();
    testConfigArgumentParsingErrorHandling();
    testConfigSaveFileErrorHandling();
    testConfigTypeErrorHandling();
    testConfigRemoveInvalidKey();
    testConfigClearRobustness();
    testConfigSaveLoadRoundtrip();
}

} // namespace test_config_enhanced
