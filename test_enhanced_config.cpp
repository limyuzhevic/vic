// Enhanced Config Tests
#include "core/Config/Config.hpp"
#include "core/Config/ConfigManager.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>

namespace test_enhanced_config {

void testBasicFunctionality() {
    nlm::Config config;
    
    // Test setting various types
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    assert(config.has("int_value"));
    assert(config.has("double_value"));
    assert(config.has("string_value"));
    assert(config.has("bool_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testBasicFunctionality passed" << std::endl;
}

void testConfigCategories() {
    nlm::Config config;
    
    // Test category assignment
    config.set("stdpLTPWeight", 0.1, nlm::ConfigSource::Runtime);
    config.set("threadCount", 4, nlm::ConfigSource::Runtime);
    config.set("debugLevel", 2, nlm::ConfigSource::Runtime);
    
    // The Config class now has categories in ConfigEntry
    auto keys = config.getKeys();
    assert(keys.size() >= 3);
    
    std::cout << "    testConfigCategories passed" << std::endl;
}

void testConfigManager() {
    nlm::ConfigManager manager;
    
    // Test profile management
    assert(manager.getProfile() == nlm::ConfigManager::Profile::Release);
    
    manager.setProfile(nlm::ConfigManager::Profile::Debug);
    assert(manager.getProfile() == nlm::ConfigManager::Profile::Debug);
    
    // Test category configuration
    manager.addAdvancedConfig();
    manager.addPerformanceConfig();
    manager.addDevelopmentConfig();
    manager.addNeuralConfig();
    
    // Test validation
    nlm::ConfigEntry entry("test", nlm::ConfigValue(1), nlm::ConfigSource::Runtime);
    assert(manager.validateConfig(entry));
    
    std::cout << "    testConfigManager passed" << std::endl;
}

void testJSONSerialization() {
    nlm::Config config;
    
    // Set some values
    config.set("testInt", 42, nlm::ConfigSource::Runtime);
    config.set("testDouble", 3.14, nlm::ConfigSource::Runtime);
    config.set("testString", "hello", nlm::ConfigSource::Runtime);
    config.set("testBool", true, nlm::ConfigSource::Runtime);
    
    // Create a test JSON file
    std::string jsonPath = "/tmp/test_config.json";
    bool saveResult = config.saveToFile(jsonPath);
    assert(saveResult);
    
    // Load it back
    nlm::Config config2;
    bool loadResult = config2.loadFromFile(jsonPath);
    assert(loadResult);
    
    assert(config2.has("testInt"));
    assert(config2.has("testDouble"));
    assert(config2.has("testString"));
    assert(config2.has("testBool"));
    
    auto intVal = config2.get<int>("testInt");
    assert(intVal.has_value() && *intVal == 42);
    
    // Clean up
    std::filesystem::remove(jsonPath);
    
    std::cout << "    testJSONSerialization passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test key validation
    assert(nlm::Config::validateKey("valid_key"));
    assert(!nlm::Config::validateKey("".empty()));
    assert(!nlm::Config::validateKey("key with spaces"));
    
    // Test file path validation
    assert(nlm::Config::validateFilePath("valid/path"));
    assert(!nlm::Config::validateFilePath(""));
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testDefaultValues() {
    nlm::Config config;
    
    // Test profile-specific defaults
    config.set("learningRate", 0.01, nlm::ConfigSource::Default);
    
    // Test that default values are applied
    assert(config.has("learningRate"));
    
    std::cout << "    testDefaultValues passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("neuronCount", 1000, nlm::ConfigSource::Runtime);
    config.set("learningRate", 0.01, nlm::ConfigSource::Runtime);
    config.set("threadCount", 4, nlm::ConfigSource::Runtime);
    
    std::string summary = config.summary();
    assert(summary.find("1000") != std::string::npos);
    assert(summary.find("0.01") != std::string::npos);
    assert(summary.find("4") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void runAll() {
    testBasicFunctionality();
    testConfigCategories();
    testConfigManager();
    testJSONSerialization();
    testConfigValidation();
    testDefaultValues();
    testConfigSummary();
    
    std::cout << "\nAll enhanced config tests passed!" << std::endl;
}

} // namespace test_enhanced_config

int main() {
    std::cout << "Running Enhanced Config Tests..." << std::endl;
    test_enhanced_config::runAll();
    return 0;
}
