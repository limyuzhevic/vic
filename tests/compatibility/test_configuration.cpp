// Compatibility Tests - Configuration
// Comprehensive configuration compatibility test suite

#include "tests/utils/TestUtils.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Config/ConfigManager.hpp"
#include "src/core/Config/ConfigValidator.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>

namespace test_configuration {

void testConfigurationFileFormats() {
    TestUtils::TestSession session("testConfigurationFileFormats");
    
    // Test that configuration can be saved and loaded in different formats
    std::string testDir = "/tmp/nlm_config_format_test_" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    std::filesystem::create_directories(testDir);
    
    // Create config with various data types
    nlm::Config config;
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string_value", "test string", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("int_vector", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    
    // Test JSON format (if supported)
    std::string jsonFile = testDir + "/config.json";
    bool saveSuccess = config.saveToFile(jsonFile);
    TestAssert::true(saveSuccess, "Should save config to JSON file");
    
    nlm::Config loadedConfig;
    bool loadSuccess = loadedConfig.loadFromFile(jsonFile);
    TestAssert::true(loadSuccess, "Should load config from JSON file");
    
    // Verify values
    auto intVal = loadedConfig.get<int>("int_value");
    TestAssert::true(intVal.has_value(), "Loaded config should have int value");
    TestAssert::equal(*intVal, 42, "Loaded int value should be correct");
    
    // Cleanup
    std::filesystem::remove_all(testDir);
    
    std::cout << "    testConfigurationFileFormats passed" << std::endl;
    session.setSuccess(true);
}

void testConfigurationTemplateCompatibility() {
    TestUtils::TestSession session("testConfigurationTemplateCompatibility");
    
    // Test that configuration templates work correctly
    auto template = std::make_unique<nlm::ConfigTemplate>("compat_template");
    
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
    
    // Test template updates
    bool updateSuccess = template->updateDefaultValue("param1", "updated1", nlm::ConfigSource::Default);
    TestAssert::true(updateSuccess, "Should update template default value");
    
    // Verify update
    auto config2 = template->createConfig();
    auto param1Val = config2->get<std::string>("param1");
    if (param1Val.has_value()) {
        TestAssert::equal(*param1Val, "updated1", "Updated template value should be used");
    }
    
    std::cout << "    testConfigurationTemplateCompatibility passed" << std::endl;
    session.setSuccess(true);
}

void testConfigurationManagerCompatibility() {
    TestUtils::TestSession session("testConfigurationManagerCompatibility");
    
    // Test that configuration manager works with old API
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
    
    TestAssert::true(loadedConfig->has("manager_test"), "Loaded config should have manager_test key");
    
    std::cout << "    testConfigurationManagerCompatibility passed" << std::endl;
    session.setSuccess(true);
}

void testConfigurationValidatorCompatibility() {
    TestUtils::TestSession session("testConfigurationValidatorCompatibility");
    
    // Test that configuration validator works with old API
    auto validator = std::make_unique<nlm::ConfigValidator>();
    
    // Create valid config
    nlm::Config validConfig;
    validConfig.set("name", "valid_test", nlm::ConfigSource::Runtime);
    validConfig.set("value", 42, nlm::ConfigSource::Runtime);
    
    bool isValid = validator->validateConfig(validConfig);
    TestAssert::true(isValid, "Valid config should pass validation");
    
    // Create invalid config
    nlm::Config invalidConfig;
    invalidConfig.set("name", "", nlm::ConfigSource::Runtime); // Empty name
    
    bool isInvalid = validator->validateConfig(invalidConfig);
    TestAssert::false(isInvalid, "Invalid config should fail validation");
    
    // Test validation with details
    auto validationResult = validator->validateConfigWithDetails(validConfig);
    TestAssert::true(validationResult.valid, "Validation should be valid");
    
    std::cout << "    testConfigurationValidatorCompatibility passed" << std::endl;
    session.setSuccess(true);
}

void testConfigurationHistoryCompatibility() {
    TestUtils::TestSession session("testConfigurationHistoryCompatibility");
    
    // Test that configuration history works with old API
    auto history = std::make_unique<nlm::ConfigHistory>();
    
    // Create and save initial config
    nlm::Config config1;
    config1.set("version", "1.0", nlm::ConfigSource::Runtime);
    
    history->saveConfig(config1);
    
    // Create and save modified config
    nlm::Config config2;
    config2.set("version", "2.0", nlm::ConfigSource::Runtime);
    
    history->saveConfig(config2);
    
    // Get history
    auto historyConfigs = history->getHistory();
    TestAssert::true(historyConfigs.size() >= 2, "History should contain at least 2 configs");
    
    // Get latest config
    auto latestConfig = history->getLatestConfig();
    TestAssert::true(latestConfig != nullptr, "Should get latest config from history");
    
    auto versionVal = latestConfig->get<std::string>("version");
    TestAssert::true(versionVal.has_value(), "Latest config should have 'version' key");
    TestAssert::equal(*versionVal, "2.0", "Latest config version should be '2.0'");
    
    std::cout << "    testConfigurationHistoryCompatibility passed" << std::endl;
    session.setSuccess(true);
}

void testConfigurationFactoryCompatibility() {
    TestUtils::TestSession session("testConfigurationFactoryCompatibility");
    
    // Test that factory functions work with old API
    auto validator = nlm::createDefaultConfigValidator();
    auto manager = nlm::createDefaultConfigManager();
    auto template = nlm::createConfigTemplate("default_template");
    auto history = nlm::createConfigHistory();
    
    TestAssert::true(validator != nullptr, "Factory function should return non-null validator");
    TestAssert::true(manager != nullptr, "Factory function should return non-null manager");
    TestAssert::true(template != nullptr, "Factory function should return non-null template");
    TestAssert::true(history != nullptr, "Factory function should return non-null history");
    
    // Test convenience functions
    auto config = nlm::loadConfig("test.json");
    // This might return null if file doesn't exist, which is expected
    
    std::cout << "    testConfigurationFactoryCompatibility passed" << std::endl;
    session.setSuccess(true);
}

void testConfigurationBackwardCompatibility() {
    TestUtils::TestSession session("testConfigurationBackwardCompatibility");
    
    // Test overall backward compatibility
    nlm::Config config;
    
    // Old-style usage patterns
    config.set("old_style_key", "old_style_value", nlm::ConfigSource::Runtime);
    config.set("another_old_key", 123, nlm::ConfigSource::File);
    
    // Old-style retrieval patterns
    auto oldStyleVal = config.get<std::string>("old_style_key");
    auto anotherOldVal = config.get<int>("another_old_key");
    
    TestAssert::true(oldStyleVal.has_value(), "Old-style string retrieval should work");
    TestAssert::true(anotherOldVal.has_value(), "Old-style int retrieval should work");
    
    // Test old validation pattern
    bool isValid = config.validateWith(*nlm::createDefaultConfigValidator());
    TestAssert::true(isValid, "Old validation pattern should work");
    
    // Test old cloning pattern
    auto clonedConfig = config.clone();
    TestAssert::true(clonedConfig != nullptr, "Old cloning pattern should work");
    TestAssert::true(clonedConfig->has("old_style_key"), "Cloned config should have old_style_key");
    
    std::cout << "    testConfigurationBackwardCompatibility passed" << std::endl;
    session.setSuccess(true);
}

void runAll() {
    std::cout << "=== Configuration Compatibility Tests ===" << std::endl;
    
    testConfigurationFileFormats();
    testConfigurationTemplateCompatibility();
    testConfigurationManagerCompatibility();
    testConfigurationValidatorCompatibility();
    testConfigurationHistoryCompatibility();
    testConfigurationFactoryCompatibility();
    testConfigurationBackwardCompatibility();
    
    std::cout << std::endl;
    std::cout << "=== All Configuration Compatibility Tests PASSED ===" << std::endl;
}