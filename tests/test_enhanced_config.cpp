// Enhanced Configuration Tests
// Tests for the new JSON/YAML support, validation, and backward compatibility
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_enhanced_config {

void testBackwardCompatibility() {
    nlm::Config config;
    
    // Test basic key=value format (backward compatibility)
    config.set("int_value", 42, nlm::ConfigSource::File);
    config.set("string_value", "hello", nlm::ConfigSource::File);
    config.set("double_value", 3.14, nlm::ConfigSource::File);
    config.set("bool_value", true, nlm::ConfigSource::File);
    
    assert(config.has("int_value"));
    assert(config.has("string_value"));
    assert(config.has("double_value"));
    assert(config.has("bool_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testBackwardCompatibility passed" << std::endl;
}

void testAutoTypeDetection() {
    nlm::Config config;
    
    // Test automatic type detection from strings
    config.setAuto("int_from_string", "42", nlm::ConfigSource::File);
    config.setAuto("double_from_string", "3.14", nlm::ConfigSource::File);
    config.setAuto("bool_from_string", "true", nlm::ConfigSource::File);
    config.setAuto("string_from_string", "hello", nlm::ConfigSource::File);
    
    assert(config.has("int_from_string"));
    assert(config.has("double_from_string"));
    assert(config.has("bool_from_string"));
    assert(config.has("string_from_string"));
    
    // Check types
    assert(config.getType("int_from_string") == "int");
    assert(config.getType("double_from_string") == "double");
    assert(config.getType("bool_from_string") == "bool");
    assert(config.getType("string_from_string") == "string");
    
    std::cout << "    testAutoTypeDetection passed" << std::endl;
}

void testJSONSaving() {
    nlm::Config config;
    
    config.set("name", "Test Brain", nlm::ConfigSource::Runtime);
    config.set("neurons", 1000, nlm::ConfigSource::Runtime);
    config.set("learning_rate", 0.01, nlm::ConfigSource::Runtime);
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    
    std::string jsonFile = "test_config.json";
    bool saveSuccess = config.saveToJSON(jsonFile, true);
    assert(saveSuccess);
    
    // Load and verify JSON file
    nlm::Config config2;
    bool loadSuccess = config2.loadFromJSON(jsonFile);
    assert(loadSuccess);
    
    assert(config2.has("name"));
    assert(config2.has("neurons"));
    assert(config2.has("learning_rate"));
    assert(config2.has("enabled"));
    
    std::string name = config2.getString("name");
    assert(name == "Test Brain");
    
    // Clean up
    std::remove(jsonFile.c_str());
    
    std::cout << "    testJSONSaving passed" << std::endl;
}

void testYAMLSaving() {
    nlm::Config config;
    
    config.set("name", "YAML Brain", nlm::ConfigSource::Runtime);
    config.set("version", 1.0, nlm::ConfigSource::Runtime);
    config.set("tags", std::vector<std::string>("tag1,tag2,tag3"), nlm::ConfigSource::Runtime);
    
    std::string yamlFile = "test_config.yaml";
    bool saveSuccess = config.saveToYAML(yamlFile, true);
    assert(saveSuccess);
    
    // Load and verify YAML file
    nlm::Config config2;
    bool loadSuccess = config2.loadFromYAML(yamlFile);
    assert(loadSuccess);
    
    assert(config2.has("name"));
    assert(config2.has("version"));
    assert(config2.has("tags"));
    
    std::string name = config2.getString("name");
    assert(name == "YAML Brain");
    
    // Clean up
    std::remove(yamlFile.c_str());
    
    std::cout << "    testYAMLSaving passed" << std::endl;
}

void testKeyValueSaving() {
    nlm::Config config;
    
    config.addEntry("database", "localhost", nlm::ConfigSource::File, "Database host", "string");
    config.addEntry("port", 5432, nlm::ConfigSource::File, "Database port", "int");
    config.addEntry("ssl_enabled", true, nlm::ConfigSource::File, "SSL enabled", "bool");
    
    std::string cfgFile = "test_config.cfg";
    bool saveSuccess = config.saveToKeyValue(cfgFile);
    assert(saveSuccess);
    
    // Load and verify key=value file
    nlm::Config config2;
    bool loadSuccess = config2.loadFromKeyValue(cfgFile);
    assert(loadSuccess);
    
    assert(config2.has("database"));
    assert(config2.has("port"));
    assert(config2.has("ssl_enabled"));
    
    std::string database = config2.getString("database");
    assert(database == "localhost");
    
    int port = config2.getOr<int>("port", 0);
    assert(port == 5432);
    
    bool sslEnabled = config2.getOr<bool>("ssl_enabled", false);
    assert(sslEnabled == true);
    
    // Clean up
    std::remove(cfgFile.c_str());
    
    std::cout << "    testKeyValueSaving passed" << std::endl;
}

void testFormatDetection() {
    nlm::Config config;
    
    // Test format detection based on file extension
    assert(config.getFileFormat("config.json") == "json");
    assert(config.getFileFormat("settings.yaml") == "yaml");
    assert(config.getFileFormat("settings.yml") == "yaml");
    assert(config.getFileFormat("config.cfg") == "keyvalue");
    assert(config.getFileFormat("config.conf") == "keyvalue");
    assert(config.getFileFormat("config") == "keyvalue"); // default
    
    std::cout << "    testFormatDetection passed" << std::endl;
}

void testGetString() {
    nlm::Config config;
    
    config.set("name", "Test", nlm::ConfigSource::Runtime);
    config.set("count", 42, nlm::ConfigSource::Runtime);
    config.set("ratio", 3.14, nlm::ConfigSource::Runtime);
    config.set("active", true, nlm::ConfigSource::Runtime);
    
    assert(config.getString("name") == "Test");
    assert(config.getString("count") == "42");
    assert(config.getString("ratio") == "3.14");
    assert(config.getString("active") == "true");
    
    std::cout << "    testGetString passed" << std::endl;
}

void testValidation() {
    nlm::Config config;
    
    // Add entries with type information
    config.addEntry("int_value", 42, nlm::ConfigSource::Runtime, "Integer value", "int");
    config.addEntry("string_value", "hello", nlm::ConfigSource::Runtime, "String value", "string");
    config.addEntry("double_value", 3.14, nlm::ConfigSource::Runtime, "Double value", "double");
    config.addEntry("bool_value", true, nlm::ConfigSource::Runtime, "Boolean value", "bool");
    
    // Basic validation (currently passes for all valid types)
    assert(config.validate());
    
    std::cout << "    testValidation passed" << std::endl;
}

void testLoadFromArgs() {
    nlm::Config config;
    
    // Simulate command line arguments
    int argc = 5;
    char* argv[] = {
        (char*)"program",
        (char*)"--name", (char*)"test",
        (char*)"-count", (char*)"42",
        nullptr
    };
    
    bool loadSuccess = config.loadFromArgs(argc, argv);
    assert(loadSuccess);
    
    assert(config.has("name"));
    assert(config.has("count"));
    
    std::string name = config.getString("name");
    assert(name == "test");
    
    int count = config.getOr<int>("count", 0);
    assert(count == 42);
    
    std::cout << "    testLoadFromArgs passed" << std::endl;
}

void runAll() {
    testBackwardCompatibility();
    testAutoTypeDetection();
    testJSONSaving();
    testYAMLSaving();
    testKeyValueSaving();
    testFormatDetection();
    testGetString();
    testValidation();
    testLoadFromArgs();
    
    std::cout << "\nAll enhanced configuration tests passed!" << std::endl;
}

} // namespace test_enhanced_config