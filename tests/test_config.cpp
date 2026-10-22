// Config Tests
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

void testConfigSaveAndLoadJSON() {
    nlm::Config config;
    
    // Set various configuration values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Save to temporary file
    std::string tempFile = "./test_config.json";
    bool saveSuccess = config.saveToFile(tempFile);
    assert(saveSuccess);
    
    // Verify file was created
    assert(std::filesystem::exists(tempFile));
    
    // Load from file
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(tempFile);
    assert(loadSuccess);
    
    // Verify loaded values
    assert(config2.has("int_value"));
    assert(config2.has("double_value"));
    assert(config2.has("string_value"));
    assert(config2.has("bool_value"));
    
    auto intVal = config2.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config2.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.14158 && *doubleVal < 3.14160);
    
    auto stringVal = config2.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "test");
    
    auto boolVal = config2.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Clean up
    std::filesystem::remove(tempFile);
    
    std::cout << "    testConfigSaveAndLoadJSON passed" << std::endl;
}

void testConfigSaveAndLoadSimpleFormat() {
    nlm::Config config;
    
    // Set configuration values
    config.set("random_seed", 42, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    
    // Save to temporary file
    std::string tempFile = "./test_config_simple.cfg";
    bool saveSuccess = config.saveToFile(tempFile);
    assert(saveSuccess);
    
    // Load from file using simple format
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(tempFile);
    assert(loadSuccess);
    
    // Verify loaded values
    assert(config2.has("random_seed"));
    assert(config2.has("simulation_timestep"));
    assert(config2.has("neuron_count"));
    
    auto seedVal = config2.get<int64_t>("random_seed");
    assert(seedVal.has_value());
    assert(*seedVal == 42);
    
    // Clean up
    std::filesystem::remove(tempFile);
    
    std::cout << "    testConfigSaveAndLoadSimpleFormat passed" << std::endl;
}

void testConfigLoadInvalidFile() {
    nlm::Config config;
    
    // Try to load non-existent file
    bool loadSuccess = config.loadFromFile("./non_existent_file.json");
    assert(!loadSuccess);
    
    // Try to load invalid JSON file
    std::string tempFile = "./test_invalid.json";
    std::ofstream invalidFile(tempFile);
    invalidFile << "{"; // Invalid JSON
    invalidFile.close();
    
    bool loadSuccess2 = config.loadFromFile(tempFile);
    assert(!loadSuccess2); // Should fail to parse invalid JSON
    
    // Clean up
    std::filesystem::remove(tempFile);
    
    std::cout << "    testConfigLoadInvalidFile passed" << std::endl;
}

void testConfigLoadEmptyFile() {
    nlm::Config config;
    
    // Try to load empty file
    std::string tempFile = "./test_empty.json";
    std::ofstream emptyFile(tempFile);
    emptyFile.close(); // Create empty file
    
    bool loadSuccess = config.loadFromFile(tempFile);
    assert(loadSuccess); // Empty file should be valid
    
    // Verify config is empty
    assert(config.getKeys().empty());
    
    // Clean up
    std::filesystem::remove(tempFile);
    
    std::cout << "    testConfigLoadEmptyFile passed" << std::endl;
}

void testConfigTypeValidation() {
    nlm::Config config;
    
    // Test type mismatches
    config.set("string_val", std::string("123"), nlm::ConfigSource::Runtime);
    
    // Try to get as integer (should fail gracefully)
    auto intVal = config.get<int>("string_val");
    assert(!intVal.has_value()); // String "123" cannot be converted to int automatically
    
    // Test boolean validation
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigTypeValidation passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSaveAndLoadJSON();
    testConfigSaveAndLoadSimpleFormat();
    testConfigLoadInvalidFile();
    testConfigLoadEmptyFile();
    testConfigTypeValidation();
}

} // namespace test_config
