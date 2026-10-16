// Config Tests - Enhanced Configuration System
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>

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

void testConfigCategories() {
    nlm::Config config;
    
    // Set values with different categories
    config.set("neuron_count", 100, nlm::ConfigSource::Runtime, nlm::ConfigCategory::Neural);
    config.set("plasticity_rate", 0.5, nlm::ConfigSource::Runtime, nlm::ConfigCategory::Plasticity);
    config.set("log_level", std::string("INFO"), nlm::ConfigSource::Runtime, nlm::ConfigCategory::Logging);
    config.set("custom_value", 3.14, nlm::ConfigSource::Runtime, nlm::ConfigCategory::Custom);
    
    // Test category retrieval
    assert(config.getCategory("neuron_count") == nlm::ConfigCategory::Neural);
    assert(config.getCategory("plasticity_rate") == nlm::ConfigCategory::Plasticity);
    assert(config.getCategory("log_level") == nlm::ConfigCategory::Logging);
    assert(config.getCategory("custom_value") == nlm::ConfigCategory::Custom);
    
    // Test getting keys by category
    auto neuralKeys = config.getKeysByCategory(nlm::ConfigCategory::Neural);
    assert(neuralKeys.size() == 1);
    assert(neuralKeys[0] == "neuron_count");
    
    auto plasticityKeys = config.getKeysByCategory(nlm::ConfigCategory::Plasticity);
    assert(plasticityKeys.size() == 1);
    assert(plasticityKeys[0] == "plasticity_rate");
    
    auto loggingKeys = config.getKeysByCategory(nlm::ConfigCategory::Logging);
    assert(loggingKeys.size() == 1);
    assert(loggingKeys[0] == "log_level");
    
    auto customKeys = config.getKeysByCategory(nlm::ConfigCategory::Custom);
    assert(customKeys.size() == 1);
    assert(customKeys[0] == "custom_value");
    
    // Test all categories
    auto categories = config.getCategories();
    assert(categories.size() == 4);
    
    std::cout << "    testConfigCategories passed" << std::endl;
}

void testConfigTemplates() {
    nlm::Config config;
    
    // Test template names
    auto templateNames = config.getTemplateNames();
    assert(!templateNames.empty());
    assert(std::find(templateNames.begin(), templateNames.end(), "Basic") != templateNames.end());
    assert(std::find(templateNames.begin(), templateNames.end(), "High Performance") != templateNames.end());
    assert(std::find(templateNames.begin(), templateNames.end(), "Plasticity Focus") != templateNames.end());
    
    // Test applying a template
    config.applyTemplate("Basic");
    
    // Check if template values were applied
    assert(config.get<int>("neuron_count").has_value());
    assert(config.get<double>("connection_probability").has_value());
    assert(config.get<int>("random_seed").has_value());
    assert(config.get<double>("simulation_timestep").has_value());
    
    std::cout << "    testConfigTemplates passed" << std::endl;
}

void testConfigPresets() {
    nlm::Config config;
    
    // Create a test preset
    nlm::ConfigPreset testPreset("TestPreset", "Test configuration preset", "test_user");
    testPreset.values = {
        {"neuron_count", 200},
        {"connection_probability", 0.3},
        {"random_seed", 123}
    };
    
    // Add preset
    config.addPreset(testPreset);
    
    // Test preset names
    auto presetNames = config.getPresetNames();
    assert(presetNames.size() == 1);
    assert(presetNames[0] == "TestPreset");
    
    // Test applying preset
    config.applyPreset("TestPreset");
    
    // Check if preset values were applied
    assert(config.get<int>("neuron_count").has_value());
    assert(*config.get<int>("neuron_count") == 200);
    assert(config.get<double>("connection_probability").has_value());
    assert(*config.get<double>("connection_probability") == 0.3);
    assert(config.get<int>("random_seed").has_value());
    assert(*config.get<int>("random_seed") == 123);
    
    std::cout << "    testConfigPresets passed" << std::endl;
}

void testConfigSuggestions() {
    nlm::Config config;
    
    // Test getting suggestions for existing key
    auto suggestions = config.getSuggestions("neuron_count");
    assert(!suggestions.empty());
    
    // Test getting documentation for existing key
    auto documentation = config.getDocumentation("neuron_count");
    assert(!documentation.empty());
    
    // Test getting suggestions for non-existent key
    auto emptySuggestions = config.getSuggestions("nonexistent_key");
    assert(emptySuggestions.empty());
    
    // Test getting documentation for non-existent key
    auto emptyDocumentation = config.getDocumentation("nonexistent_key");
    assert(!emptyDocumentation.empty() && emptyDocumentation.find("nonexistent_key") != std::string::npos);
    
    std::cout << "    testConfigSuggestions passed" << std::endl;
}

void testConfigHelp() {
    nlm::Config config;
    
    // Test getting help
    auto help = config.getHelp();
    assert(!help.empty());
    assert(help.find("Configuration Help") != std::string::npos);
    
    // Test getting statistics
    auto statistics = config.getStatistics();
    assert(!statistics.empty());
    assert(statistics.find("Configuration Statistics") != std::string::npos);
    
    // Test getting usage statistics
    auto usageStatistics = config.getUsageStatistics();
    assert(!usageStatistics.empty());
    assert(usageStatistics.find("Usage Statistics") != std::string::npos);
    
    std::cout << "    testConfigHelp passed" << std::endl;
}

void testConfigExportImport() {
    nlm::Config config1;
    
    // Set some values in config1
    config1.set("key1", 42, nlm::ConfigSource::Runtime);
    config1.set("key2", std::string("hello"), nlm::ConfigSource::Runtime);
    config1.set("key3", 3.14, nlm::ConfigSource::Runtime);
    
    // Export to file
    std::string exportFile = "test_export.cfg";
    bool exportResult = config1.exportToFile(exportFile, false);
    assert(exportResult);
    
    // Create config2 and import from file
    nlm::Config config2;
    bool importResult = config2.importFromFile(exportFile);
    assert(importResult);
    
    // Verify values were imported
    assert(config2.has("key1"));
    assert(config2.has("key2"));
    assert(config2.has("key3"));
    assert(*config2.get<int>("key1") == 42);
    assert(*config2.get<std::string>("key2") == "hello");
    assert(std::abs(*config2.get<double>("key3") - 3.14) < 0.001);
    
    // Clean up
    std::remove(exportFile.c_str());
    
    std::cout << "    testConfigExportImport passed" << std::endl;
}

void testConfigDiffMerge() {
    nlm::Config config1;
    nlm::Config config2;
    
    // Set different values in both configs
    config1.set("key1", 1, nlm::ConfigSource::Runtime);
    config1.set("key2", std::string("hello"), nlm::ConfigSource::Runtime);
    config1.set("key3", 3.14, nlm::ConfigSource::Runtime);
    
    config2.set("key1", 10, nlm::ConfigSource::Runtime);
    config2.set("key2", std::string("world"), nlm::ConfigSource::Runtime);
    config2.set("key4", 99, nlm::ConfigSource::Runtime);
    
    // Test diff
    std::string diff = config1.getDiff(config2);
    assert(!diff.empty());
    assert(diff.find("differs") != std::string::npos || diff.find("only exists") != std::string::npos);
    
    // Test merge
    config1.merge(config2, true);
    
    // After merge, config1 should have all values
    assert(config1.has("key1")); // Should be overwritten
    assert(config1.has("key2")); // Should be overwritten
    assert(config1.has("key3")); // Should remain
    assert(config1.has("key4")); // Should be added
    assert(*config1.get<int>("key1") == 10);
    assert(*config1.get<std::string>("key2") == "world");
    assert(std::abs(*config1.get<double>("key3") - 3.14) < 0.001);
    assert(*config1.get<int>("key4") == 99);
    
    std::cout << "    testConfigDiffMerge passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Set valid values
    config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.5, nlm::ConfigSource::Runtime);
    config.set("log_level", std::string("INFO"), nlm::ConfigSource::Runtime);
    
    // Test validation (should pass for valid values)
    bool validationPassed = config.validateConfiguration();
    assert(validationPassed);
    
    // Test fix configuration
    bool fixApplied = config.fixConfiguration();
    assert(fixApplied); // Should fix if there are issues
    
    // Test recommendations
    auto recommendations = config.getRecommendations();
    assert(!recommendations.empty()); // Should have recommendations
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigEnvironmentVariables() {
    // Set environment variable
    setenv("NLM_TEST_INT", "123", 1);
    setenv("NLM_TEST_DOUBLE", "3.14", 1);
    setenv("NLM_TEST_STRING", "hello", 1);
    setenv("NLM_TEST_BOOL", "true", 1);
    
    nlm::Config config;
    
    // Load from environment variables
    bool envLoaded = config.loadFromEnvironment();
    assert(envLoaded);
    
    // Check that environment variables were loaded
    assert(config.get<int>("test_int").has_value());
    assert(*config.get<int>("test_int") == 123);
    assert(config.get<double>("test_double").has_value());
    assert(std::abs(*config.get<double>("test_double") - 3.14) < 0.001);
    assert(config.get<std::string>("test_string").has_value());
    assert(*config.get<std::string>("test_string") == "hello");
    assert(config.get<bool>("test_bool").has_value());
    assert(*config.get<bool>("test_bool") == true);
    
    // Clean up environment variables
    unsetenv("NLM_TEST_INT");
    unsetenv("NLM_TEST_DOUBLE");
    unsetenv("NLM_TEST_STRING");
    unsetenv("NLM_TEST_BOOL");
    
    std::cout << "    testConfigEnvironmentVariables passed" << std::endl;
}

void testConfigSimpleFile() {
    nlm::Config config;
    
    // Create a simple configuration file
    std::string testFile = "test_simple.cfg";
    std::ofstream file(testFile);
    file << "# Test configuration file\n";
    file << "int_value = 42\n";
    file << "double_value = 3.14\n";
    file << "bool_value = true\n";
    file << "string_value = \"hello\"\n";
    file.close();
    
    // Load from simple file
    bool loaded = config.loadFromSimpleFile(testFile);
    assert(loaded);
    
    // Verify values
    assert(config.get<int>("int_value").has_value());
    assert(*config.get<int>("int_value") == 42);
    assert(config.get<double>("double_value").has_value());
    assert(std::abs(*config.get<double>("double_value") - 3.14) < 0.001);
    assert(config.get<bool>("bool_value").has_value());
    assert(*config.get<bool>("bool_value") == true);
    assert(config.get<std::string>("string_value").has_value());
    assert(*config.get<std::string>("string_value") == "hello");
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigSimpleFile passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    // Add some values
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("key3", 3.14, nlm::ConfigSource::Runtime);
    
    // Get summary
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("Enhanced Configuration Summary") != std::string::npos);
    assert(summary.find("Total entries: 3") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigHotReload() {
    nlm::Config config;
    
    // Enable hot-reload
    config.enableHotReload(true);
    
    // Add a value
    config.set("test_value", 123, nlm::ConfigSource::Runtime);
    
    // Verify hot-reload is enabled
    assert(config.getStatistics().find("Hot-reload: enabled") != std::string::npos);
    
    std::cout << "    testConfigHotReload passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigCategories();
    testConfigTemplates();
    testConfigPresets();
    testConfigSuggestions();
    testConfigHelp();
    testConfigExportImport();
    testConfigDiffMerge();
    testConfigValidation();
    testConfigEnvironmentVariables();
    testConfigSimpleFile();
    testConfigSummary();
    testConfigHotReload();
    
    std::cout << "\nAll configuration tests passed!" << std::endl;
}

} // namespace test_config
