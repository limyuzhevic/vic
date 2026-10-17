// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>

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

void testConfigFileIO() {
    nlm::Config config;
    
    // Set some test values
    config.set("int_test", 42, nlm::ConfigSource::Runtime);
    config.set("double_test", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string_test", "Hello World", nlm::ConfigSource::Runtime);
    config.set("bool_test", true, nlm::ConfigSource::Runtime);
    
    // Create a temporary file
    std::string tempFile = "test_config_" + std::to_string(std::time(nullptr)) + ".txt";
    
    // Save to file
    bool saveResult = config.saveToFile(tempFile);
    assert(saveResult);
    
    // Clear config and load from file
    config.clear();
    bool loadResult = config.loadFromFile(tempFile);
    assert(loadResult);
    
    // Verify values were loaded
    assert(config.has("int_test"));
    auto intVal = config.get<int>("int_test");
    assert(intVal.has_value() && *intVal == 42);
    
    assert(config.has("double_test"));
    auto doubleVal = config.get<double>("double_test");
    assert(doubleVal.has_value());
    
    assert(config.has("string_test"));
    auto stringVal = config.get<std::string>("string_test");
    assert(stringVal.has_value() && *stringVal == "Hello World");
    
    assert(config.has("bool_test"));
    auto boolVal = config.get<bool>("bool_test");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testConfigFileIO passed" << std::endl;
}

void testConfigJSONParsing() {
    nlm::Config config;
    
    // Create a JSON file
    std::string tempFile = "test_config_" + std::to_string(std::time(nullptr)) + ".json";
    
    std::ofstream jsonFile(tempFile);
    jsonFile << R"({"json_int": 100, "json_double": 2.718, "json_string": "JSON Test", "json_bool": false})" << std::endl;
    jsonFile.close();
    
    // Load JSON file
    bool loadResult = config.loadFromFile(tempFile);
    assert(loadResult);
    
    // Verify JSON values were loaded
    assert(config.has("json_int"));
    auto intVal = config.get<int>("json_int");
    assert(intVal.has_value() && *intVal == 100);
    
    assert(config.has("json_double"));
    auto doubleVal = config.get<double>("json_double");
    assert(doubleVal.has_value());
    assert(*doubleVal > 2.717 && *doubleVal < 2.719);
    
    assert(config.has("json_string"));
    auto stringVal = config.get<std::string>("json_string");
    assert(stringVal.has_value() && *stringVal == "JSON Test");
    
    assert(config.has("json_bool"));
    auto boolVal = config.get<bool>("json_bool");
    assert(boolVal.has_value() && *boolVal == false);
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testConfigJSONParsing passed" << std::endl;
}

void testConfigCommandLineArgs() {
    nlm::Config config;
    
    // Simulate command line arguments
    const char* args[] = {"test", "--int_arg", "123", "--string_arg", "Hello", "--double_arg", "3.14"};
    int argc = 6;
    
    config.loadFromArgs(argc, const_cast<char**>(args));
    
    // Verify command line arguments were parsed
    assert(config.has("int_arg"));
    auto intVal = config.get<int>("int_arg");
    assert(intVal.has_value() && *intVal == 123);
    
    assert(config.has("string_arg"));
    auto stringVal = config.get<std::string>("string_arg");
    assert(stringVal.has_value() && *stringVal == "Hello");
    
    assert(config.has("double_arg"));
    auto doubleVal = config.get<double>("double_arg");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    std::cout << "    testConfigCommandLineArgs passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("key1", 100, nlm::ConfigSource::File);
    config.set("key2", "test string", nlm::ConfigSource::CommandLine);
    config.set("key3", 3.14, nlm::ConfigSource::Runtime);
    
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    assert(summary.find("key3") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigArrayTypes() {
    nlm::Config config;
    
    std::vector<int> intArray = {1, 2, 3, 4, 5};
    std::vector<double> doubleArray = {1.1, 2.2, 3.3};
    std::vector<std::string> stringArray = {"a", "b", "c"};
    
    config.set("int_array", intArray, nlm::ConfigSource::Runtime);
    config.set("double_array", doubleArray, nlm::ConfigSource::Runtime);
    config.set("string_array", stringArray, nlm::ConfigSource::Runtime);
    
    assert(config.has("int_array"));
    assert(config.has("double_array"));
    assert(config.has("string_array"));
    
    // Note: Getting back array types from Config is complex and would need 
    // additional template support. For now, we just verify they can be stored.
    
    std::cout << "    testConfigArrayTypes passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigFileIO();
    testConfigJSONParsing();
    testConfigCommandLineArgs();
    testConfigSummary();
    testConfigArrayTypes();
}

} // namespace test_config
