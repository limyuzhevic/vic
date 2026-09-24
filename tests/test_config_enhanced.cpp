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
    
    // Test array types
    config.set("int_array", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    auto intArrayVal = config.get<std::vector<int>>("int_array");
    assert(intArrayVal.has_value());
    assert(intArrayVal->size() == 3);
    assert((*intArrayVal)[0] == 1);
    
    config.set("double_array", std::vector<double>({1.1, 2.2, 3.3}), nlm::ConfigSource::Runtime);
    auto doubleArrayVal = config.get<std::vector<double>>("double_array");
    assert(doubleArrayVal.has_value());
    assert(doubleArrayVal->size() == 3);
    
    config.set("string_array", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    auto stringArrayVal = config.get<std::vector<std::string>>("string_array");
    assert(stringArrayVal.has_value());
    assert(stringArrayVal->size() == 3);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    // Test with other types
    double doubleVal = config.getOr<double>("missing", 3.14);
    assert(doubleVal > 3.13 && doubleVal < 3.15);
    
    std::string stringVal = config.getOr<std::string>("missing", "default");
    assert(stringVal == "default");
    
    bool boolVal = config.getOr<bool>("missing", false);
    assert(boolVal == false);
    
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
    
    // Check all keys exist
    bool hasKey1 = false, hasKey2 = false, hasKey3 = false;
    for (const auto& key : keys) {
        if (key == "key1") hasKey1 = true;
        if (key == "key2") hasKey2 = true;
        if (key == "key3") hasKey3 = true;
    }
    assert(hasKey1 && hasKey2 && hasKey3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigClear() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    assert(config.getKeys().size() == 3);
    
    config.clear();
    assert(config.getKeys().empty());
    
    std::cout << "    testConfigClear passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "test", nlm::ConfigSource::File);
    config.set("double_value", 3.14, nlm::ConfigSource::CommandLine);
    
    std::string summary = config.summary();
    assert(summary.find("int_value") != std::string::npos);
    assert(summary.find("string_value") != std::string::npos);
    assert(summary.find("double_value") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigFileLoading() {
    // Create a temporary config file
    std::string tempPath = "/tmp/test_config.cfg";
    std::ofstream file(tempPath);
    file << "# Test config file\n";
    file << "key1 = value1\n";
    file << "key2 = 42\n";
    file << "key3 = true\n";
    file << "key4 = 3.14\n";
    file.close();
    
    nlm::Config config;
    bool success = config.loadFromFile(tempPath);
    assert(success);
    
    // Verify values
    auto key1 = config.get<std::string>("key1");
    assert(key1.has_value() && *key1 == "value1");
    
    auto key2 = config.get<int>("key2");
    assert(key2.has_value() && *key2 == 42);
    
    auto key3 = config.get<bool>("key3");
    assert(key3.has_value() && *key3 == true);
    
    auto key4 = config.get<double>("key4");
    assert(key4.has_value());
    assert(*key4 > 3.13 && *key4 < 3.15);
    
    // Test missing file
    nlm::Config config2;
    bool fail = config2.loadFromFile("/tmp/nonexistent.cfg");
    assert(!fail);
    
    // Clean up
    std::remove(tempPath.c_str());
    
    std::cout << "    testConfigFileLoading passed" << std::endl;
}

void testConfigArgsParsing() {
    nlm::Config config;
    
    // Test --key=value format
    const char* args1[] = {"program", "--int_value=100", "--string_value=hello", "--bool_value=true"};
    int argc1 = 4;
    bool success1 = config.loadFromArgs(argc1, const_cast<char**>(args1));
    assert(success1);
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 100);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test -key value format
    nlm::Config config2;
    const char* args2[] = {"program", "-double_value", "3.14159", "-string2", "world"};
    int argc2 = 5;
    bool success2 = config2.loadFromArgs(argc2, const_cast<char**>(args2));
    assert(success2);
    
    auto doubleVal = config2.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.14 && *doubleVal < 3.15);
    
    auto string2Val = config2.get<std::string>("string2");
    assert(string2Val.has_value() && *string2Val == "world");
    
    std::cout << "    testConfigArgsParsing passed" << std::endl;
}

void testConfigMixedSources() {
    nlm::Config config;
    
    // Set from different sources
    config.set("key1", "file_value", nlm::ConfigSource::File);
    config.set("key2", 200, nlm::ConfigSource::Default);
    config.set("key3", "cli_value", nlm::ConfigSource::CommandLine);
    
    // Verify all values are set
    assert(config.has("key1"));
    assert(config.has("key2"));
    assert(config.has("key3"));
    
    // Verify sources
    auto key1 = config.get<std::string>("key1");
    auto key2 = config.get<int>("key2");
    auto key3 = config.get<std::string>("key3");
    
    assert(key1.has_value() && *key1 == "file_value");
    assert(key2.has_value() && *key2 == 200);
    assert(key3.has_value() && *key3 == "cli_value");
    
    std::cout << "    testConfigMixedSources passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigClear();
    testConfigSummary();
    testConfigFileLoading();
    testConfigArgsParsing();
    testConfigMixedSources();
    
    std::cout << std::endl;
    std::cout << "=== All Config Tests PASSED ===" << std::endl;
    std::cout << "Config system now supports:" << std::endl;
    std::cout << "  - Robust file loading with error handling" << std::endl;
    std::cout << "  - Command line argument parsing" << std::endl;
    std::cout << "  - Multiple configuration sources (File, Default, CommandLine)" << std::endl;
    std::cout << "  - Type safety with template getters" << std::endl;
    std::cout << "  - Proper logging and error reporting" << std::endl;
}

} // namespace test_config