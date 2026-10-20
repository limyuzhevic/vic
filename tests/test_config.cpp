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
    
    // Test vector types
    config.set("int_list", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    auto intListVal = config.get<std::vector<int>>("int_list");
    assert(intListVal.has_value());
    assert(intListVal.value().size() == 3);
    assert(intListVal.value()[0] == 1);
    
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
    assert(std::find(keys.begin(), keys.end(), "key1") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key2") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key3") != keys.end());
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigFileIO() {
    nlm::Config config;
    
    // Set up test data
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello world", nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    config.set("test_list", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    
    // Save to file
    std::string testFile = "/tmp/test_config.cfg";
    bool saveSuccess = config.saveToFile(testFile);
    assert(saveSuccess);
    
    // Create new config and load from file
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(testFile);
    assert(loadSuccess);
    
    // Verify loaded data
    assert(config2.has("test_int"));
    auto intVal = config2.get<int>("test_int");
    assert(intVal.has_value() && *intVal == 42);
    
    assert(config2.has("test_double"));
    auto doubleVal = config2.get<double>("test_double");
    assert(doubleVal.has_value() && (*doubleVal > 3.13 && *doubleVal < 3.15));
    
    assert(config2.has("test_string"));
    auto stringVal = config2.get<std::string>("test_string");
    assert(stringVal.has_value() && *stringVal == "hello world");
    
    assert(config2.has("test_bool"));
    auto boolVal = config2.get<bool>("test_bool");
    assert(boolVal.has_value() && *boolVal == true);
    
    assert(config2.has("test_list"));
    auto listVal = config2.get<std::vector<int>>("test_list");
    assert(listVal.has_value() && listVal.value().size() == 3);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigFileIO passed" << std::endl;
}

void testConfigCommandLineArgs() {
    nlm::Config config;
    
    // Test command line argument parsing
    int argc = 5;
    char* argv[] = {
        (char*)"test",
        (char*)"--test_int=100",
        (char*)"--test_string=hello",
        (char*)"--test_bool=true",
        nullptr
    };
    
    bool success = config.loadFromArgs(argc, argv);
    assert(success);
    
    // Verify parsed values
    assert(config.has("test_int"));
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value() && *intVal == 100);
    
    assert(config.has("test_string"));
    auto stringVal = config.get<std::string>("test_string");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    assert(config.has("test_bool"));
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value() && *boolVal == true);
    
    std::cout << "    testConfigCommandLineArgs passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Default);
    config.set("key2", "test", nlm::ConfigSource::File);
    config.set("key3", true, nlm::ConfigSource::CommandLine);
    
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    assert(summary.find("key3") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void runAll() {
    std::cout << "=== Running Config Tests ===" << std::endl;
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigFileIO();
    testConfigCommandLineArgs();
    testConfigSummary();
    std::cout << "=== All Config Tests Passed ===" << std::endl;
}

} // namespace test_config
