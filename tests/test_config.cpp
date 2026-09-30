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

void testConfigJSONSerialization() {
    nlm::Config config;
    
    // Set various types of values
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string_val", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    config.set("string_array", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    
    // Save to file
    const std::string filepath = "/tmp/test_config.json";
    bool saveSuccess = config.saveToFile(filepath);
    assert(saveSuccess);
    
    // Load from file (should be JSON)
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(filepath);
    assert(loadSuccess);
    
    // Verify values
    assert(config2.has("int_val"));
    assert(config2.get<int>("int_val") == 42);
    
    assert(config2.has("double_val"));
    auto doubleVal = config2.get<double>("double_val");
    assert(doubleVal.has_value());
    assert(std::abs(*doubleVal - 3.14159) < 0.0001);
    
    assert(config2.has("string_val"));
    assert(config2.get<std::string>("string_val") == "test");
    
    assert(config2.has("bool_val"));
    assert(config2.get<bool>("bool_val") == true);
    
    // Clean up
    std::filesystem::remove(filepath);
    
    std::cout << "    testConfigJSONSerialization passed" << std::endl;
}

void testConfigBackwardCompatibility() {
    nlm::Config config;
    
    // Create a config file in old key=value format
    const std::string filepath = "/tmp/test_config_old.txt";
    std::ofstream file(filepath);
    file << "# Test configuration file\n";
    file << "int_value = 123\n";
    file << "string_value = \"hello\"\n";
    file << "bool_value = true\n";
    file << "# Another comment\n";
    file << "double_value = 3.14\n";
    file.close();
    
    // Load using backward compatible parser
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(filepath);
    assert(loadSuccess);
    
    // Verify old format was parsed correctly
    assert(config2.has("int_value"));
    assert(config2.get<int>("int_value") == 123);
    
    assert(config2.has("string_value"));
    assert(config2.get<std::string>("string_value") == "hello");
    
    assert(config2.has("bool_value"));
    assert(config2.get<bool>("bool_value") == true);
    
    assert(config2.has("double_value"));
    auto doubleVal = config2.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(std::abs(*doubleVal - 3.14) < 0.0001);
    
    // Clean up
    std::filesystem::remove(filepath);
    
    std::cout << "    testConfigBackwardCompatibility passed" << std::endl;
}

void testConfigSchemaValidation() {
    nlm::Config config;
    
    // Add a schema for an integer value (must be between 1 and 100)
    ConfigSchema intSchema("int_config", "Integer value configuration", true);
    intSchema.validator = [](const ConfigValue& val) -> bool {
        int intVal = std::get<int>(val);
        return intVal >= 1 && intVal <= 100;
    };
    
    config.addSchema("int_config", intSchema);
    
    // Set valid value
    config.set("int_config", 50, nlm::ConfigSource::Runtime);
    assert(config.has("int_config"));
    
    // Test invalid value should throw exception
    bool exceptionThrown = false;
    try {
        config.set("int_config", 150, nlm::ConfigSource::Runtime);
    } catch (const nlm::ConfigValidationError& e) {
        exceptionThrown = true;
        assert(std::string(e.what()).find("validation failed") != std::string::npos);
    }
    assert(exceptionThrown);
    
    // Schema should still be present
    assert(config.hasSchema("int_config"));
    
    // Remove schema and set invalid value (should work without schema)
    config.removeSchema("int_config");
    config.set("int_config", 999, nlm::ConfigSource::Runtime);
    assert(config.get<int>("int_config") == 999);
    
    std::cout << "    testConfigSchemaValidation passed" << std::endl;
}

void testConfigMalformedJSON() {
    nlm::Config config;
    
    // Create a malformed JSON file
    const std::string filepath = "/tmp/test_config_malformed.json";
    std::ofstream file(filepath);
    file << "{\n";
    file << "  \"key1\": \"value1\",\n";
    file << "  \"key2\": 42,\n";
    file << "  // This is a comment (invalid in JSON)\n";
    file << "  \"key3\": \"value3\"\n";
    file.close();
    
    // Should fall back to key=value format (which will fail for this file)
    bool loadSuccess = config.loadFromFile(filepath);
    // This should return false since the file doesn't contain valid key=value pairs
    
    // Clean up
    std::filesystem::remove(filepath);
    
    std::cout << "    testConfigMalformedJSON passed" << std::endl;
}

void testConfigComplexArrays() {
    nlm::Config config;
    
    // Set array values
    config.set("int_array", std::vector<int>({1, 2, 3, 4, 5}), nlm::ConfigSource::Runtime);
    config.set("double_array", std::vector<double>({1.5, 2.5, 3.5}), nlm::ConfigSource::Runtime);
    config.set("string_array", std::vector<std::string>({"apple", "banana", "cherry"}), nlm::ConfigSource::Runtime);
    
    // Verify arrays
    auto intArray = config.get<std::vector<int>>("int_array");
    assert(intArray.has_value());
    assert(intArray->size() == 5);
    assert((*intArray)[0] == 1);
    assert((*intArray)[4] == 5);
    
    auto doubleArray = config.get<std::vector<double>>("double_array");
    assert(doubleArray.has_value());
    assert(doubleArray->size() == 3);
    assert(std::abs((*doubleArray)[0] - 1.5) < 0.0001);
    
    auto stringArray = config.get<std::vector<std::string>>("string_array");
    assert(stringArray.has_value());
    assert(stringArray->size() == 3);
    assert((*stringArray)[1] == "banana");
    
    // Save and reload
    const std::string filepath = "/tmp/test_config_arrays.json";
    config.saveToFile(filepath);
    
    nlm::Config config2;
    config2.loadFromFile(filepath);
    
    auto intArray2 = config2.get<std::vector<int>>("int_array");
    assert(intArray2.has_value());
    assert(intArray2->size() == 5);
    
    // Clean up
    std::filesystem::remove(filepath);
    
    std::cout << "    testConfigComplexArrays passed" << std::endl;
}

void testConfigCommandLineArguments() {
    nlm::Config config;
    
    // Simulate command line arguments
    int argc = 5;
    char* argv[] = {
        (char*)"program",
        (char*)"--config_value",
        (char*)"from_command_line",
        (char*)"-debug",
        (char*)"-1",
        NULL
    };
    
    bool loadSuccess = config.loadFromArgs(argc, argv);
    assert(loadSuccess);
    
    // The -debug should have set debug to "1"
    assert(config.has("debug"));
    assert(config.get<std::string>("debug") == "1");
    
    std::cout << "    testConfigCommandLineArguments passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJSONSerialization();
    testConfigBackwardCompatibility();
    testConfigSchemaValidation();
    testConfigMalformedJSON();
    testConfigComplexArrays();
    testConfigCommandLineArguments();
}

} // namespace test_config
