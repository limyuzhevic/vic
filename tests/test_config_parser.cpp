// ConfigParser Tests
// Tests for the refactored Config class with JSON/YAML support

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_config_parser {

void testConfigCreation() {
    std::cout << "  Testing Config creation..." << std::endl;
    
    nlm::Config config;
    
    // Test initial state
    assert(config.getKeys().empty());
    assert(!config.has("test_key"));
    
    std::cout << "    ✓ Config creation test passed" << std::endl;
}

void testConfigSetAndGet() {
    std::cout << "  Testing Config set and get operations..." << std::endl;
    
    nlm::Config config;
    
    // Test setting different types
    config.set("string_key", std::string("test_value"));
    config.set("int_key", 42);
    config.set("double_key", 3.14);
    config.set("bool_key", true);
    
    // Test getting values
    assert(config.get<std::string>("string_key").value() == "test_value");
    assert(config.get<int64_t>("int_key").value() == 42);
    assert(config.get<double>("double_key").value() == 3.14);
    assert(config.get<bool>("bool_key").value() == true);
    
    // Test getOr with default
    assert(config.getOr<std::string>("missing_key", "default_value") == "default_value");
    assert(config.getOr<int64_t>("missing_int", 99) == 99);
    
    std::cout << "    ✓ Config set and get test passed" << std::endl;
}

void testConfigFileFormatDetection() {
    std::cout << "  Testing file format detection..." << std::endl;
    
    nlm::Config config;
    
    // Test that loadFromFile handles different extensions
    // We'll test this with a temporary test file
    std::string testDir = "test_configs";
    if (!std::filesystem::exists(testDir)) {
        std::filesystem::create_directory(testDir);
    }
    
    // Create a simple test config file
    std::string simpleConfigPath = testDir + "/test_simple.cfg";
    std::ofstream simpleConfig(simpleConfigPath);
    simpleConfig << "# Test configuration\n";
    simpleConfig << "test_int = 123\n";
    simpleConfig << "test_string = \"hello world\"\n";
    simpleConfig << "test_bool = true\n";
    simpleConfig.close();
    
    // Test loading simple config
    bool loaded = config.loadFromFile(simpleConfigPath);
    assert(loaded);
    assert(config.getOr<int64_t>("test_int", 0) == 123);
    assert(config.getOr<std::string>("test_string", "") == "hello world");
    assert(config.getOr<bool>("test_bool", false) == true);
    
    // Clean up
    std::filesystem::remove(simpleConfigPath);
    
    std::cout << "    ✓ File format detection test passed" << std::endl;
}

void testConfigFileSave() {
    std::cout << "  Testing Config file save functionality..." << std::endl;
    
    nlm::Config config;
    
    // Add some test data
    config.set("neuron_count", 1000ll);
    config.set("region_count", 1ll);
    config.set("connection_probability", 0.15f);
    config.set("random_seed", 42ll);
    
    // Test saving to simple file
    std::string testDir = "test_configs";
    if (!std::filesystem::exists(testDir)) {
        std::filesystem::create_directory(testDir);
    }
    
    std::string simpleConfigPath = testDir + "/test_save.cfg";
    bool saved = config.saveToFile(simpleConfigPath);
    assert(saved);
    
    // Try to load it back
    nlm::Config config2;
    bool loaded = config2.loadFromFile(simpleConfigPath);
    assert(loaded);
    
    assert(config2.getOr<int64_t>("neuron_count", 0) == 1000);
    assert(config2.getOr<int64_t>("region_count", 0) == 1);
    assert(config2.getOr<float>("connection_probability", 0.0f) == 0.15f);
    
    // Clean up
    std::filesystem::remove(simpleConfigPath);
    
    std::cout << "    ✓ Config file save test passed" << std::endl;
}

void testConfigSummary() {
    std::cout << "  Testing Config summary functionality..." << std::endl;
    
    nlm::Config config;
    
    // Add some test data
    config.set("test_key1", "value1", nlm::ConfigSource::Runtime);
    config.set("test_key2", 123, nlm::ConfigSource::File);
    config.set("test_key3", 3.14, nlm::ConfigSource::CommandLine);
    
    // Get summary
    std::string summary = config.summary();
    
    // Check that summary contains our keys
    assert(summary.find("test_key1") != std::string::npos);
    assert(summary.find("test_key2") != std::string::npos);
    assert(summary.find("test_key3") != std::string::npos);
    
    std::cout << "    ✓ Config summary test passed" << std::endl;
}

void testConfigRemove() {
    std::cout << "  Testing Config remove functionality..." << std::endl;
    
    nlm::Config config;
    
    // Add test data
    config.set("temp_key1", "temp_value1");
    config.set("temp_key2", 456);
    
    assert(config.has("temp_key1"));
    assert(config.has("temp_key2"));
    
    // Remove one key
    config.remove("temp_key1");
    assert(!config.has("temp_key1"));
    assert(config.has("temp_key2"));
    
    std::cout << "    ✓ Config remove test passed" << std::endl;
}

void testConfigClear() {
    std::cout << "  Testing Config clear functionality..." << std::endl;
    
    nlm::Config config;
    
    // Add test data
    config.set("key1", "value1");
    config.set("key2", 789);
    
    assert(config.getKeys().size() == 2);
    
    // Clear all
    config.clear();
    assert(config.getKeys().empty());
    
    std::cout << "    ✓ Config clear test passed" << std::endl;
}

void runAll() {
    std::cout << "=== ConfigParser Tests ===" << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigFileFormatDetection();
    testConfigFileSave();
    testConfigSummary();
    testConfigRemove();
    testConfigClear();
    
    std::cout << "=== All ConfigParser Tests PASSED ===" << std::endl;
}

} // namespace test_config_parser