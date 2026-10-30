#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

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

void testConfigAdvancedTypes() {
    nlm::Config config;
    
    // Test int64
    config.set("int64_value", int64_t(123456789012345), nlm::ConfigSource::Runtime);
    auto int64Val = config.get<int64_t>("int64_value");
    assert(int64Val.has_value());
    assert(*int64Val == 123456789012345);
    
    // Test vector types
    std::vector<int> intVector = {1, 2, 3, 4, 5};
    config.set("int_vector", intVector, nlm::ConfigSource::Runtime);
    auto intVectorVal = config.get<std::vector<int>>("int_vector");
    assert(intVectorVal.has_value());
    assert(intVectorVal->size() == 5);
    assert((*intVectorVal)[0] == 1);
    assert((*intVectorVal)[4] == 5);
    
    std::vector<double> doubleVector = {1.5, 2.5, 3.5};
    config.set("double_vector", doubleVector, nlm::ConfigSource::Runtime);
    auto doubleVectorVal = config.get<std::vector<double>>("double_vector");
    assert(doubleVectorVal.has_value());
    assert(doubleVectorVal->size() == 3);
    
    std::vector<std::string> stringVector = {"hello", "world", "test"};
    config.set("string_vector", stringVector, nlm::ConfigSource::Runtime);
    auto stringVectorVal = config.get<std::vector<std::string>>("string_vector");
    assert(stringVectorVal.has_value());
    assert(stringVectorVal->size() == 3);
    assert((*stringVectorVal)[0] == "hello");
    
    std::cout << "    testConfigAdvancedTypes passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test validation
    assert(config.validate()); // Empty config should be valid
    
    // Add valid entries
    config.set("valid_int", 42, nlm::ConfigSource::Runtime);
    config.set("valid_string", "test", nlm::ConfigSource::Runtime);
    config.set("valid_bool", true, nlm::ConfigSource::Runtime);
    config.set("valid_float", 3.14, nlm::ConfigSource::Runtime);
    
    assert(config.validate()); // Valid config should be valid
    
    // Test validation key method
    assert(config.validateKey("valid_key"));
    assert(config.validateKey("another-key"));
    assert(config.validateKey("key.with.dots"));
    assert(config.validateKey("_underscore"));
    
    assert(!config.validateKey("") ); // Empty key
    assert(!config.validateKey("key with spaces")); // Key with spaces
    assert(!config.validateKey("key@symbol")); // Key with invalid symbol
    
    // Test getValidatedKeys
    auto validatedKeys = config.getValidatedKeys();
    assert(validatedKeys.size() >= 4); // At least 4 valid keys
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigSerialization() {
    nlm::Config config;
    
    // Add test data
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("string_val", "hello world", nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    config.set("float_val", 3.14159, nlm::ConfigSource::Runtime);
    
    std::vector<int> intArray = {1, 2, 3};
    config.set("array_val", intArray, nlm::ConfigSource::Runtime);
    
    // Test JSON serialization
    std::string jsonOutput = config.serialize("json");
    assert(!jsonOutput.empty());
    assert(jsonOutput.find("int_val") != std::string::npos);
    assert(jsonOutput.find("string_val") != std::string::npos);
    assert(jsonOutput.find("bool_val") != std::string::npos);
    assert(jsonOutput.find("float_val") != std::string::npos);
    assert(jsonOutput.find("array_val") != std::string::npos);
    
    // Test YAML serialization
    std::string yamlOutput = config.serialize("yaml");
    assert(!yamlOutput.empty());
    assert(yamlOutput.find("int_val") != std::string::npos);
    assert(yamlOutput.find("string_val") != std::string::npos);
    
    // Test INI serialization
    std::string iniOutput = config.serialize("ini");
    assert(!iniOutput.empty());
    assert(iniOutput.find("int_val") != std::string::npos);
    assert(iniOutput.find("string_val") != std::string::npos);
    
    std::cout << "    testConfigSerialization passed" << std::endl;
}

void testConfigStats() {
    nlm::Config config;
    
    // Add different types of entries
    config.set("int1", 1, nlm::ConfigSource::Runtime);
    config.set("int2", 2, nlm::ConfigSource::Default);
    config.set("string1", "hello", nlm::ConfigSource::CommandLine);
    config.set("bool1", true, nlm::ConfigSource::File);
    
    std::vector<int> intArray = {1, 2, 3};
    config.set("array1", intArray, nlm::ConfigSource::Runtime);
    
    // Test stats
    auto stats = config.getStats();
    assert(stats.totalEntries == 5);
    assert(stats.entriesBySource[static_cast<int>(nlm::ConfigSource::Runtime)] == 2);
    assert(stats.entriesBySource[static_cast<int>(nlm::ConfigSource::Default)] == 1);
    assert(stats.entriesBySource[static_cast<int>(nlm::ConfigSource::CommandLine)] == 1);
    assert(stats.entriesBySource[static_cast<int>(nlm::ConfigSource::File)] == 1);
    
    assert(stats.valuesByType["int"] == 2);
    assert(stats.valuesByType["string"] == 1);
    assert(stats.valuesByType["bool"] == 1);
    assert(stats.valuesByType["int_vector"] == 1);
    
    std::cout << "    testConfigStats passed" << std::endl;
}

void testConfigFileIO() {
    nlm::Config config;
    
    // Create test data
    config.set("name", "Test Brain", nlm::ConfigSource::Runtime);
    config.set("neurons", 1000, nlm::ConfigSource::Runtime);
    config.set("learning_rate", 0.01, nlm::ConfigSource::Runtime);
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    
    std::string testFile = "test_config.json";
    
    // Test JSON save/load
    bool saveResult = config.saveToFile(testFile);
    assert(saveResult);
    
    nlm::Config loadedConfig;
    bool loadResult = loadedConfig.loadFromFile(testFile);
    assert(loadResult);
    
    assert(loadedConfig.get<std::string>("name").has_value());
    assert(*loadedConfig.get<std::string>("name") == "Test Brain");
    
    // Test simple format save/load
    std::string simpleFile = "test_config.txt";
    bool simpleSaveResult = config.saveToFile(simpleFile);
    assert(simpleSaveResult);
    
    nlm::Config loadedSimpleConfig;
    bool simpleLoadResult = loadedSimpleConfig.loadFromFile(simpleFile);
    assert(simpleLoadResult);
    
    assert(loadedSimpleConfig.get<std::string>("name").has_value());
    assert(*loadedSimpleConfig.get<std::string>("name") == "Test Brain");
    
    // Clean up
    std::filesystem::remove(testFile);
    std::filesystem::remove(simpleFile);
    
    std::cout << "    testConfigFileIO passed" << std::endl;
}

void runAll() {
    std::cout << "Running enhanced Config tests..." << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigAdvancedTypes();
    testConfigValidation();
    testConfigSerialization();
    testConfigStats();
    testConfigFileIO();
    
    std::cout << "All enhanced Config tests passed!" << std::endl;
}

} // namespace test_config
