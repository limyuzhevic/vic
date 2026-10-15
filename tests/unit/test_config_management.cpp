// Unit Tests for Configuration Management
// Comprehensive test suite for advanced configuration features

#include "tests/utils/TestUtils.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Config/ConfigManager.hpp"
#include "src/core/Config/ConfigTemplate.hpp"
#include "src/core/Config/ConfigHistory.hpp"
#include "src/core/Config/ConfigValidator.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>

namespace test_config_management {

void testConfigCreation() {
    TestUtils::TestSession session("testConfigCreation");
    
    nlm::Config config;
    TestAssert::false(config.has("test"), "Config should not have 'test' key initially");
    
    std::cout << "    testConfigCreation passed" << std::endl;
    session.setSuccess(true);
}

void testConfigSetAndGet() {
    TestUtils::TestSession session("testConfigSetAndGet");
    
    nlm::Config config;
    
    // Test setting and getting various types
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    TestAssert::true(config.has("int_value"), "Config should have 'int_value' after setting");
    
    auto intVal = config.get<int>("int_value");
    TestAssert::true(intVal.has_value(), "int_value should have value");
    TestAssert::equal(*intVal, 42, 0.001, "int_value should equal 42");
    
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    TestAssert::true(doubleVal.has_value(), "double_value should have value");
    TestAssert::equal(*doubleVal, 3.14159, 0.0001, "double_value should equal 3.14159");
    
    config.set("string_value", std::string("hello world"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    TestAssert::true(stringVal.has_value(), "string_value should have value");
    TestAssert::equal(*stringVal, "hello world", "string_value should equal 'hello world'");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    TestAssert::true(boolVal.has_value(), "bool_value should have value");
    TestAssert::true(*boolVal == true, "bool_value should equal true");
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
    session.setSuccess(true);
}

void testConfigGetOr() {
    TestUtils::TestSession session("testConfigGetOr");
    
    nlm::Config config;
    
    // Test getOr with missing key
    int val = config.getOr<int>("missing", 100);
    TestAssert::equal(val, 100, "getOr for missing key should return default");
    
    // Test getOr with existing key
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    TestAssert::equal(val, 50, "getOr for existing key should return actual value");
    
    std::cout << "    testConfigGetOr passed" << std::endl;
    session.setSuccess(true);
}

void testConfigRemove() {
    TestUtils::TestSession session("testConfigRemove");
    
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    TestAssert::true(config.has("test"), "Config should have 'test' after setting");
    
    config.remove("test");
    TestAssert::false(config.has("test"), "Config should not have 'test' after removal");
    
    std::cout << "    testConfigRemove passed" << std::endl;
    session.setSuccess(true);
}

void testConfigKeys() {
    TestUtils::TestSession session("testConfigKeys");
    
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    TestAssert::equal(keys.size(), 3, "Config should have 3 keys");
    
    std::cout << "    testConfigKeys passed" << std::endl;
    session.setSuccess(true);
}

void testConfigClone() {
    TestUtils::TestSession session("testConfigClone");
    
    nlm::Config config;
    config.set("original", "value", nlm::ConfigSource::Runtime);
    config.set("number", 42, nlm::ConfigSource::File);
    
    auto cloned = config.clone();
    TestAssert::true(cloned != nullptr, "Clone should not return nullptr");
    
    TestAssert::true(cloned->has("original"), "Cloned config should have 'original' key");
    TestAssert::true(cloned->has("number"), "Cloned config should have 'number' key");
    
    auto originalVal = config.get<std::string>("original");
    auto clonedVal = cloned->get<std::string>("original");
    TestAssert::true(originalVal.has_value(), "Original should have 'original' value");
    TestAssert::true(clonedVal.has_value(), "Cloned should have 'original' value");
    TestAssert::equal(*originalVal, *clonedVal, "Original and cloned values should be equal");
    
    std::cout << "    testConfigClone passed" << std::endl;
    session.setSuccess(true);
}

void testConfigClear() {
    TestUtils::TestSession session("testConfigClear");
    
    nlm::Config config;
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    
    TestAssert::true(config.has("key1"), "Config should have 'key1' before clear");
    TestAssert::true(config.has("key2"), "Config should have 'key2' before clear");
    
    config.clear();
    
    TestAssert::false(config.has("key1"), "Config should not have 'key1' after clear");
    TestAssert::false(config.has("key2"), "Config should not have 'key2' after clear");
    
    auto keys = config.getKeys();
    TestAssert::equal(keys.size(), 0, "Config should have no keys after clear");
    
    std::cout << "    testConfigClear passed" << std::endl;
    session.setSuccess(true);
}

void testConfigSummary() {
    TestUtils::TestSession session("testConfigSummary");
    
    nlm::Config config;
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("string_key", "test", nlm::ConfigSource::File);
    config.set("bool_key", true, nlm::ConfigSource::CommandLine);
    
    std::string summary = config.summary();
    TestAssert::true(summary.size() > 0, "Summary should not be empty");
    TestAssert::contains(summary, "int_key", "Summary should contain 'int_key'");
    TestAssert::contains(summary, "string_key", "Summary should contain 'string_key'");
    TestAssert::contains(summary, "bool_key", "Summary should contain 'bool_key'");
    
    std::cout << "    testConfigSummary passed" << std::endl;
    session.setSuccess(true);
}

void testConfigFileIO() {
    TestUtils::TestSession session("testConfigFileIO");
    
    std::string testDir = "/tmp/nlm_config_test_" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    std::filesystem::create_directories(testDir);
    
    std::string filepath = testDir + "/test_config.json";
    
    // Create and save config
    nlm::Config config1;
    config1.set("saved_int", 100, nlm::ConfigSource::Runtime);
    config1.set("saved_string", "hello", nlm::ConfigSource::File);
    config1.set("saved_double", 3.14159, nlm::ConfigSource::CommandLine);
    
    bool saveSuccess = config1.saveToFile(filepath);
    TestAssert::true(saveSuccess, "Config should save to file successfully");
    
    // Load config from file
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(filepath);
    TestAssert::true(loadSuccess, "Config should load from file successfully");
    
    // Verify loaded values
    auto intVal = config2.get<int>("saved_int");
    TestAssert::true(intVal.has_value(), "Loaded config should have 'saved_int' value");
    TestAssert::equal(*intVal, 100, "Loaded int should equal 100");
    
    auto stringVal = config2.get<std::string>("saved_string");
    TestAssert::true(stringVal.has_value(), "Loaded config should have 'saved_string' value");
    TestAssert::equal(*stringVal, "hello", "Loaded string should equal 'hello'");
    
    auto doubleVal = config2.get<double>("saved_double");
    TestAssert::true(doubleVal.has_value(), "Loaded config should have 'saved_double' value");
    TestAssert::equal(*doubleVal, 3.14159, 0.0001, "Loaded double should equal 3.14159");
    
    // Cleanup
    std::filesystem::remove_all(testDir);
    
    std::cout << "    testConfigFileIO passed" << std::endl;
    session.setSuccess(true);
}

void testConfigManager() {
    TestUtils::TestSession session("testConfigManager");
    
    auto configManager = std::make_unique<nlm::ConfigManager>();
    
    // Create config
    auto config = std::make_shared<nlm::Config>();
    config->set("manager_test", "value", nlm::ConfigSource::Runtime);
    
    // Save config
    bool saveSuccess = configManager->saveConfig(config);
    TestAssert::true(saveSuccess, "ConfigManager should save config successfully");
    
    // Load config
    auto loadedConfig = configManager->loadConfig();
    TestAssert::true(loadedConfig != nullptr, "ConfigManager should load config successfully");
    TestAssert::true(loadedConfig->has("manager_test"), "Loaded config should have 'manager_test' key");
    
    std::cout << "    testConfigManager passed" << std::endl;
    session.setSuccess(true);
}

void testConfigValidator() {
    TestUtils::TestSession session("testConfigValidator");
    
    auto validator = std::make_unique<nlm::ConfigValidator>();
    
    nlm::Config validConfig;
    validConfig.set("name", "valid", nlm::ConfigSource::Runtime);
    validConfig.set("value", 42, nlm::ConfigSource::Runtime);
    
    bool isValid = validator->validateConfig(validConfig);
    TestAssert::true(isValid, "Valid config should pass validation");
    
    nlm::Config invalidConfig;
    invalidConfig.set("name", "", nlm::ConfigSource::Runtime); // Empty name might be invalid
    
    bool isInvalid = validator->validateConfig(invalidConfig);
    TestAssert::false(isInvalid, "Invalid config should fail validation");
    
    std::cout << "    testConfigValidator passed" << std::endl;
    session.setSuccess(true);
}

void testConfigTemplate() {
    TestUtils::TestSession session("testConfigTemplate");
    
    auto template = std::make_unique<nlm::ConfigTemplate>("test_template");
    
    // Add default values
    template->setDefaultValue("param1", "default1", nlm::ConfigSource::Default);
    template->setDefaultValue("param2", 42, nlm::ConfigSource::Default);
    template->setDefaultValue("param3", 3.14, nlm::ConfigSource::Default);
    
    // Create config from template
    auto config = template->createConfig();
    TestAssert::true(config != nullptr, "Template should create config successfully");
    
    TestAssert::true(config->has("param1"), "Config from template should have 'param1'");
    TestAssert::true(config->has("param2"), "Config from template should have 'param2'");
    TestAssert::true(config->has("param3"), "Config from template should have 'param3'");
    
    std::cout << "    testConfigTemplate passed" << std::endl;
    session.setSuccess(true);
}

void testConfigHistory() {
    TestUtils::TestSession session("testConfigHistory");
    
    auto history = std::make_unique<nlm::ConfigHistory>();
    
    nlm::Config config1;
    config1.set("version", "1.0", nlm::ConfigSource::Runtime);
    
    // Save initial config
    history->saveConfig(config1);
    
    nlm::Config config2;
    config2.set("version", "2.0", nlm::ConfigSource::Runtime);
    
    // Save modified config
    history->saveConfig(config2);
    
    // Get history
    auto historyConfigs = history->getHistory();
    TestAssert::true(historyConfigs.size() >= 2, "History should contain at least 2 configs");
    
    // Check latest version
    auto latestConfig = history->getLatestConfig();
    TestAssert::true(latestConfig != nullptr, "Should get latest config from history");
    
    auto version = latestConfig->get<std::string>("version");
    TestAssert::true(version.has_value(), "Latest config should have 'version' key");
    TestAssert::equal(*version, "2.0", "Latest config version should be '2.0'");
    
    std::cout << "    testConfigHistory passed" << std::endl;
    session.setSuccess(true);
}

void runAll() {
    std::cout << "=== Configuration Management Tests ===" << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigClone();
    testConfigClear();
    testConfigSummary();
    testConfigFileIO();
    testConfigManager();
    testConfigValidator();
    testConfigTemplate();
    testConfigHistory();
    
    std::cout << std::endl;
    std::cout << "=== All Configuration Management Tests PASSED ===" << std::endl;
}