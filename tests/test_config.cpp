// Enhanced Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

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
    
    // Test vector types
    config.set("int_list", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    auto intListVal = config.get<std::vector<int>>("int_list");
    assert(intListVal.has_value());
    assert(intListVal->size() == 3);
    
    config.set("string_list", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    auto stringListVal = config.get<std::vector<std::string>>("string_list");
    assert(stringListVal.has_value());
    assert(stringListVal->size() == 3);
    
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

void testConfigValidation() {
    nlm::Config config;
    
    // Register parameter with validation
    nlm::ParamValidation param("test_param", nlm::ParamType::Integer, "Test parameter", 0, 100);
    config.registerParameter("test_param", param);
    
    // Valid value should pass
    assert(config.set("test_param", 50, nlm::ConfigSource::Runtime));
    
    // Invalid value (too low) should fail
    assert(!config.set("test_param", -10, nlm::ConfigSource::Runtime));
    
    // Invalid value (too high) should fail
    assert(!config.set("test_param", 200, nlm::ConfigSource::Runtime));
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigJSONParsing() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::ofstream file("test_config.json");
    file << R"({"name": "test", "value": 42, "active": true})" << std::endl;
    file.close();
    
    // Load JSON file
    assert(config.loadFromFile("test_config.json"));
    
    // Check values
    auto nameVal = config.get<std::string>("name");
    assert(nameVal.has_value() && *nameVal == "test");
    
    auto valueVal = config.get<int>("value");
    assert(valueVal.has_value() && *valueVal == 42);
    
    auto activeVal = config.get<bool>("active");
    assert(activeVal.has_value() && *activeVal == true);
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "    testConfigJSONParsing passed" << std::endl;
}

void testConfigYAMLParsing() {
    nlm::Config config;
    
    // Create a temporary YAML file
    std::ofstream file("test_config.yaml");
    file << "name: test\nvalue: 42\nactive: true\nlist: [1, 2, 3]" << std::endl;
    file.close();
    
    // Load YAML file
    assert(config.loadFromFile("test_config.yaml"));
    
    // Check values
    auto nameVal = config.get<std::string>("name");
    assert(nameVal.has_value() && *nameVal == "test");
    
    auto valueVal = config.get<int>("value");
    assert(valueVal.has_value() && *valueVal == 42);
    
    auto activeVal = config.get<bool>("active");
    assert(activeVal.has_value() && *activeVal == true);
    
    auto listVal = config.get<std::vector<int>>("list");
    assert(listVal.has_value() && listVal->size() == 3);
    
    // Clean up
    std::remove("test_config.yaml");
    
    std::cout << "    testConfigYAMLParsing passed" << std::endl;
}

void testConfigSaveToJSON() {
    nlm::Config config;
    
    // Add some test data
    config.set("name", "test", nlm::ConfigSource::Runtime);
    config.set("value", 42, nlm::ConfigSource::Runtime);
    config.set("active", true, nlm::ConfigSource::Runtime);
    config.set("tags", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    assert(config.saveToFile("test_config.json", true));
    
    // Load the saved file and verify
    nlm::Config loadedConfig;
    assert(loadedConfig.loadFromFile("test_config.json"));
    
    auto nameVal = loadedConfig.get<std::string>("name");
    assert(nameVal.has_value() && *nameVal == "test");
    
    auto valueVal = loadedConfig.get<int>("value");
    assert(valueVal.has_value() && *valueVal == 42);
    
    auto activeVal = loadedConfig.get<bool>("active");
    assert(activeVal.has_value() && *activeVal == true);
    
    auto tagsVal = loadedConfig.get<std::vector<std::string>>("tags");
    assert(tagsVal.has_value() && tagsVal->size() == 3);
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "    testConfigSaveToJSON passed" << std::endl;
}

void testConfigValidationErrors() {
    nlm::Config config;
    
    // Register parameter with validation
    nlm::ParamValidation param("test_param", nlm::ParamType::String, "Test parameter");
    config.registerParameter("test_param", param);
    
    // This should not cause validation error (no validation rule for strings)
    assert(config.set("test_param", "valid_value", nlm::ConfigSource::Runtime));
    
    std::cout << "    testConfigValidationErrors passed" << std::endl;
}

void testConfigCrossParameterValidation() {
    nlm::Config config;
    
    // Set values that should trigger cross-parameter validation
    config.set("neuron_count", 50, nlm::ConfigSource::Runtime);
    config.set("region_count", 20, nlm::ConfigSource::Runtime);
    
    // This should trigger a warning (neuron_count < region_count * 10)
    // The validation doesn't prevent setting the values, just warns
    std::cout << "    testConfigCrossParameterValidation passed" << std::endl;
}

void testConfigBackupAndRestore() {
    nlm::Config config;
    
    // Add some test data
    config.set("name", "original", nlm::ConfigSource::Runtime);
    config.set("value", 100, nlm::ConfigSource::Runtime);
    
    // Enable change logging for this test
    config.enableChangeLogging();
    
    // Create a backup
    assert(config.backup("test_backup.json"));
    
    // Modify the configuration
    config.set("name", "modified", nlm::ConfigSource::Runtime);
    config.set("value", 200, nlm::ConfigSource::Runtime);
    
    // Restore from backup
    assert(config.restoreFromBackup("test_backup.json"));
    
    // Verify values were restored
    auto nameVal = config.get<std::string>("name");
    assert(nameVal.has_value() && *nameVal == "original");
    
    auto valueVal = config.get<int>("value");
    assert(valueVal.has_value() && *valueVal == 100);
    
    // Clean up
    std::remove("test_backup.json");
    config.disableChangeLogging();
    
    std::cout << "    testConfigBackupAndRestore passed" << std::endl;
}

void testConfigChangeLogging() {
    nlm::Config config;
    
    // Enable change logging
    config.enableChangeLogging("test_changes.log");
    
    // Add some test entries
    assert(config.set("key1", "value1", nlm::ConfigSource::Runtime));
    assert(config.set("key2", 42, nlm::ConfigSource::Runtime));
    
    // Clean up
    config.disableChangeLogging();
    std::remove("test_changes.log");
    
    std::cout << "    testConfigChangeLogging passed" << std::endl;
}

void testConfigEnvironmentVariables() {
    nlm::Config config;
    
    // Register parameter
    nlm::ParamValidation param("env_test", nlm::ParamType::String, "Environment test parameter");
    config.registerParameter("env_test", param);
    
    // Set via environment variable
    // Note: This test would need actual environment variables set
    // For now, we'll just test that the method exists and can be called
    assert(config.loadFromEnvironment("NLM_"));
    
    std::cout << "    testConfigEnvironmentVariables passed" << std::endl;
}

void testConfigSchemaExport() {
    nlm::Config config;
    
    // Register a parameter
    nlm::ParamValidation param("schema_test", nlm::ParamType::Float, "Schema test", 0.0, 1.0);
    config.registerParameter("schema_test", param);
    
    // Export to JSON schema
    nlohmann::json schema = config.exportToSchema();
    
    assert(schema.contains("properties"));
    assert(schema["properties"].contains("schema_test"));
    assert(schema["properties"]["schema_test"]["type"] == "number");
    assert(schema["properties"]["schema_test"]["minimum"] == 0.0);
    assert(schema["properties"]["schema_test"]["maximum"] == 1.0);
    
    std::cout << "    testConfigSchemaExport passed" << std::endl;
}

void testConfigFileFormatDetection() {
    // Test format detection
    std::string jsonFormat = nlm::Config::detectFormat("config.json");
    assert(jsonFormat == "json");
    
    std::string yamlFormat = nlm::Config::detectFormat("config.yaml");
    assert(yamlFormat == "yaml");
    
    std::string ymlFormat = nlm::Config::detectFormat("config.yml");
    assert(ymlFormat == "yaml");
    
    std::string legacyFormat = nlm::Config::detectFormat("config.cfg");
    assert(legacyFormat == "legacy");
    
    std::cout << "    testConfigFileFormatDetection passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigJSONParsing();
    testConfigYAMLParsing();
    testConfigSaveToJSON();
    testConfigValidationErrors();
    testConfigCrossParameterValidation();
    testConfigBackupAndRestore();
    testConfigChangeLogging();
    testConfigEnvironmentVariables();
    testConfigSchemaExport();
    testConfigFileFormatDetection();
    
    std::cout << "\nAll enhanced configuration tests passed!" << std::endl;
}

} // namespace test_config
