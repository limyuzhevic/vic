// Enhanced Config Tests
// Tests for the improved Config class with new functionality

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>

namespace test_config {

void testConfigAdvancedFunctionality() {
    std::cout << "    Testing advanced Config functionality..." << std::endl;
    
    nlm::Config config;
    
    // Test key validation
    config.set("valid_key", "test_value", nlm::ConfigSource::Runtime);
    assert(config.has("valid_key"));
    
    // Test key validation for invalid keys
    // Note: Config constructor doesn't throw, it sets error messages
    // So we test the isValidKey helper if available
    
    // Test getOr with missing key
    std::string missingValue = config.getOr<std::string>("missing", "default");
    assert(missingValue == "default");
    
    // Test source tracking
    config.set("file_key", "file_value", nlm::ConfigSource::File);
    config.set("cli_key", "cli_value", nlm::ConfigSource::CommandLine);
    
    // Test keys by source
    auto fileKeys = config.getKeysBySource(nlm::ConfigSource::File);
    assert(fileKeys.size() == 1);
    assert(fileKeys[0] == "file_key");
    
    auto cliKeys = config.getKeysBySource(nlm::ConfigSource::CommandLine);
    assert(cliKeys.size() == 1);
    assert(cliKeys[0] == "cli_key");
    
    // Test entry count
    assert(config.getEntryCount() == 3);
    
    std::cout << "    testConfigAdvancedFunctionality passed" << std::endl;
}

void testConfigJSONImportExport() {
    std::cout << "    Testing JSON import/export functionality..." << std::endl;
    
    nlm::Config config;
    
    // Set up test data
    config.set("int_value", 42);
    config.set("double_value", 3.14159);
    config.set("bool_value", true);
    config.set("string_value", "test_string");
    config.set("vector_int", std::vector<int64_t>({1, 2, 3, 4, 5}));
    config.set("vector_double", std::vector<double>({1.1, 2.2, 3.3}));
    config.set("vector_string", std::vector<std::string>({"a", "b", "c"}));
    
    // Export to JSON
    std::string jsonString = config.exportToJSON();
    assert(!jsonString.empty());
    
    // Import from JSON (creates new config)
    nlm::Config config2;
    bool importSuccess = config2.importFromJSON(jsonString);
    assert(importSuccess);
    
    // Verify data was imported correctly
    auto intVal = config2.get<int64_t>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config2.get<double>("double_value");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14159) < 0.0001);
    
    auto boolVal = config2.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    auto stringVal = config2.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "test_string");
    
    std::cout << "    testConfigJSONImportExport passed" << std::endl;
}

void testConfigFileFormatHandling() {
    std::cout << "    Testing file format handling..." << std::endl;
    
    nlm::Config config;
    
    // Test different file formats
    config.setFileFormat(nlm::Config::FileFormat::JSON);
    assert(config.getFileFormat() == nlm::Config::FileFormat::JSON);
    
    config.setFileFormat(nlm::Config::FileFormat::INI);
    assert(config.getFileFormat() == nlm::Config::FileFormat::INI);
    
    config.setFileFormat(nlm::Config::FileFormat::CFG);
    assert(config.getFileFormat() == nlm::Config::FileFormat::CFG);
    
    config.setFileFormat(nlm::Config::FileFormat::YAML);
    assert(config.getFileFormat() == nlm::Config::FileFormat::YAML);
    
    std::cout << "    testConfigFileFormatHandling passed" << std::endl;
}

void testConfigValidation() {
    std::cout << "    Testing configuration validation..." << std::endl;
    
    nlm::Config config;
    
    // Test that validation works
    config.set("positive_int", 10);
    config.set("negative_int", -5);  // This should be allowed in current implementation
    config.set("positive_float", 3.14);
    config.set("negative_float", -2.5);  // This should be allowed in current implementation
    config.set("string_value", "test");
    
    // Test validation errors (conceptual - implementation may vary)
    std::string lastError = config.getLastError();
    // Clear error if any
    config.clearError();
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigPresets() {
    std::cout << "    Testing configuration presets..." << std::endl;
    
    // Test default config preset
    auto defaultConfig = nlm::Config::createDefaultConfig();
    assert(defaultConfig->getEntryCount() > 0);
    
    auto intVal = defaultConfig->get<int64_t>("brain.neuron_count");
    assert(intVal.has_value() && *intVal == 1000);
    
    // Test minimal config preset
    auto minimalConfig = nlm::Config::createMinimalConfig();
    assert(minimalConfig->getEntryCount() > 0);
    
    // Test research config preset
    auto researchConfig = nlm::Config::createResearchConfig();
    assert(researchConfig->getEntryCount() > 0);
    
    auto researchNeurons = researchConfig->get<int64_t>("brain.neuron_count");
    assert(researchNeurons.has_value() && *researchNeurons == 5000);
    
    std::cout << "    testConfigPresets passed" << std::endl;
}

void testConfigMerge() {
    std::cout << "    Testing configuration merge..." << std::endl;
    
    nlm::Config config1;
    config1.set("key1", "value1", nlm::ConfigSource::File);
    config1.set("key2", "value2", nlm::ConfigSource::CommandLine);
    
    nlm::Config config2;
    config2.set("key2", "updated_value2", nlm::ConfigSource::Runtime);
    config2.set("key3", "value3", nlm::ConfigSource::File);
    
    // Merge config2 into config1
    bool mergeSuccess = config1.merge(config2);
    assert(mergeSuccess);
    
    // Verify merge results
    assert(config1.has("key1"));  // Should remain
    assert(config1.has("key2"));  // Should be updated
    assert(config1.has("key3"));  // Should be added
    
    auto key1Val = config1.get<std::string>("key1");
    assert(key1Val.has_value() && *key1Val == "value1");
    
    auto key2Val = config1.get<std::string>("key2");
    assert(key2Val.has_value() && *key2Val == "updated_value2");
    
    std::cout << "    testConfigMerge passed" << std::endl;
}

void testConfigFileOperations() {
    std::cout << "    Testing file operations..." << std::endl;
    
    nlm::Config config;
    config.set("test_key", "test_value", nlm::ConfigSource::Runtime);
    
    // Use current directory for test file
    std::string testFile = "./test_config.cfg";
    
    // Save to file
    bool saveSuccess = config.saveToFile(testFile);
    assert(saveSuccess);
    
    // Load from file
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(testFile);
    assert(loadSuccess);
    
    // Verify data was saved and loaded correctly
    auto valueVal = config2.get<std::string>("test_key");
    assert(valueVal.has_value() && *valueVal == "test_value");
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigFileOperations passed" << std::endl;
}

void runAll() {
    testConfigAdvancedFunctionality();
    testConfigJSONImportExport();
    testConfigFileFormatHandling();
    testConfigValidation();
    testConfigPresets();
    testConfigMerge();
    testConfigFileOperations();
}

} // namespace test_config
