// Enhanced Config Tests
// Now includes comprehensive tests for JSON/YAML support and advanced features
// #include "core/Config/Config.hpp"
// #include <cassert>
// #include <iostream>
// #include <fstream>
// #include <filesystem>

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

void testConfigJSONFormat() {
    nlm::Config config;
    
    // Set various types of values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string_value", "test_string", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    config.set("double_array", std::vector<double>({1.1, 2.2, 3.3}), nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    std::string jsonFile = "test_config.json";
    bool saveSuccess = config.saveToFile(jsonFile);
    assert(saveSuccess);
    
    // Create a new config and load from JSON
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(jsonFile);
    assert(loadSuccess);
    
    // Verify values were loaded correctly
    auto intVal = config2.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config2.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.14 && *doubleVal < 3.142);
    
    auto stringVal = config2.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "test_string");
    
    auto boolVal = config2.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Clean up
    std::remove(jsonFile.c_str());
    
    std::cout << "    testConfigJSONFormat passed" << std::endl;
}

void testConfigBackwardCompatibility() {
    nlm::Config config;
    
    // Create a simple key=value file (backward compatibility)
    std::string cfgFile = "test_config.cfg";
    std::ofstream file(cfgFile);
    file << "# Test config file" << std::endl;
    file << "int_value = 42" << std::endl;
    file << "double_value = 3.14" << std::endl;
    file << "string_value = \"hello world\"" << std::endl;
    file << "bool_value = true" << std::endl;
    file << "# This is a comment" << std::endl;
    file << "" << std::endl;
    file << "another_key = 123" << std::endl;
    file.close();
    
    // Load the config
    bool loadSuccess = config.loadFromFile(cfgFile);
    assert(loadSuccess);
    
    // Verify values were loaded
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config.get<double>"double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello world");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    auto anotherVal = config.get<int>("another_key");
    assert(anotherVal.has_value());
    assert(*anotherVal == 123);
    
    // Clean up
    std::remove(cfgFile.c_str());
    
    std::cout << "    testConfigBackwardCompatibility passed" << std::endl;
}

void testConfigCommandLine() {
    nlm::Config config;
    
    // Test loading from command line arguments
    int argc = 7;
    char* argv[] = {
        (char*)"test",
        (char*)"--int_value",
        (char*)"100",
        (char*)"--string_value",
        (char*)"test",
        (char*)"--bool_value",
        (char*)"false"
    };
    
    bool loadSuccess = config.loadFromArgs(argc, argv);
    assert(loadSuccess);
    
    // Verify values were loaded
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 100);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "test");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == false);
    
    std::cout << "    testConfigCommandLine passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Default);
    config.set("key2", 2.5, nlm::ConfigSource::File);
    config.set("key3", std::string("test"), nlm::ConfigSource::CommandLine);
    config.set("key4", true, nlm::ConfigSource::Runtime);
    
    std::string summary = config.summary();
    
    // Verify summary contains expected information
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    assert(summary.find("key3") != std::string::npos);
    assert(summary.find("key4") != std::string::npos);
    assert(summary.find("Configuration") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJSONFormat();
    testConfigBackwardCompatibility();
    testConfigCommandLine();
    testConfigSummary();
    
    std::cout << std::endl;
    std::cout << "=== Config Tests ===" << std::endl;
    std::cout << "All config tests passed!" << std::endl;
}

} // namespace test_config