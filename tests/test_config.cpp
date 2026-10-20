// Enhanced Config Tests with JSON/YAML support
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>  // Add this include for ofstream

namespace test_config {

// Helper function to check if file exists
inline bool fileExists(const std::string& filepath) {
    return std::filesystem::exists(filepath);
}

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

void testLegacyFormatSupport() {
    std::cout << "    Testing legacy key=value format support..." << std::endl;
    nlm::Config config;
    
    // Create a legacy format config file
    std::ofstream legacyFile("/tmp/legacy_config.cfg");
    legacyFile << "# Legacy config file\n";
    legacyFile << "legacy_key = legacy_value\n";
    legacyFile << "legacy_int = 123\n";
    legacyFile << "legacy_bool = true\n";
    legacyFile.close();
    
    // Load legacy format
    bool loadResult = config.loadFromFile("/tmp/legacy_config.cfg");
    assert(loadResult);
    
    // Check that legacy values were loaded
    assert(config.has("legacy_key"));
    auto keyVal = config.get<std::string>("legacy_key");
    assert(keyVal.has_value());
    assert(*keyVal == "legacy_value");
    
    assert(config.has("legacy_int"));
    auto intVal = config.get<int>("legacy_int");
    assert(intVal.has_value());
    assert(*intVal == 123);
    
    assert(config.has("legacy_bool"));
    auto boolVal = config.get<bool>("legacy_bool");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Clean up
    std::remove("/tmp/legacy_config.cfg");
    
    std::cout << "    testLegacyFormatSupport passed" << std::endl;
}

void testConfigSaveLoad() {
    std::cout << "    Testing config save/load cycle..." << std::endl;
    nlm::Config config;
    
    // Set some values
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_string", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    // Save to JSON
    bool saveResult = config.saveToFile("/tmp/test_config.json");
    assert(saveResult);
    
    // Clear and reload
    config.clear();
    bool loadResult = config.loadFromFile("/tmp/test_config.json");
    assert(loadResult);
    
    // Verify values
    assert(config.has("test_int"));
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    assert(config.has("test_string"));
    auto stringVal = config.get<std::string>("test_string");
    assert(stringVal.has_value());
    assert(*stringVal == "test");
    
    assert(config.has("test_double"));
    auto doubleVal = config.get<double>("test_double");
    assert(doubleVal.has_value());
    assert(std::abs(*doubleVal - 3.14) < 0.01);
    
    assert(config.has("test_bool"));
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Clean up
    std::remove("/tmp/test_config.json");
    
    std::cout << "    testConfigSaveLoad passed" << std::endl;
}

void testSummary() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "test", nlm::ConfigSource::File);
    
    std::string summary = config.summary();
    assert(summary.find("Configuration (2 entries):") != std::string::npos);
    assert(summary.find("int_value") != std::string::npos);
    assert(summary.find("string_value") != std::string::npos);
    
    std::cout << "    testSummary passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testSummary();
    
    // Test new JSON/YAML functionality (if files exist)
    if (fileExists("/workspace/app/configs/config.json")) {
        testLegacyFormatSupport();
        testConfigSaveLoad();
    }
}

} // namespace test_config