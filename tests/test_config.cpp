// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>

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

void testConfigSummary() {
    nlm::Config config;
    
    config.set("test_key", 42, nlm::ConfigSource::Runtime);
    config.set("another_key", std::string("test"), nlm::ConfigSource::File);
    
    std::string summary = config.summary();
    assert(summary.find("test_key") != std::string::npos);
    assert(summary.find("another_key") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigClear() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    
    assert(config.has("key1"));
    assert(config.has("key2"));
    
    config.clear();
    
    assert(!config.has("key1"));
    assert(!config.has("key2"));
    
    std::cout << "    testConfigClear passed" << std::endl;
}

void testConfigFileFormatDetection() {
    nlm::Config config;
    
    // Test JSON detection
    assert(config.detectFileFormat("test.json") == nlm::FileFormat::JSON);
    assert(config.detectFileFormat("config.JSON") == nlm::FileFormat::JSON);
    
    // Test YAML detection
    assert(config.detectFileFormat("test.yaml") == nlm::FileFormat::YAML);
    assert(config.detectFileFormat("config.yml") == nlm::FileFormat::YAML);
    
    // Test CFG detection
    assert(config.detectFileFormat("config.cfg") == nlm::FileFormat::CFG);
    assert(config.detectFileFormat("settings.cfg") == nlm::FileFormat::CFG);
    
    // Test unknown format
    assert(config.detectFileFormat("test.unknown") == nlm::FileFormat::CFG); // Falls back to CFG
    
    std::cout << "    testConfigFileFormatDetection passed" << std::endl;
}

void testConfigToLower() {
    nlm::Config config;
    
    assert(config.toLower("TEST") == "test");
    assert(config.toLower("HelloWorld") == "helloworld");
    assert(config.toLower("UPPERCASE") == "uppercase");
    
    std::cout << "    testConfigToLower passed" << std::endl;
}

void testConfigSetFromString() {
    nlm::Config config;
    
    // Test boolean parsing
    assert(config.setFromString("bool_true", "true"));
    assert(config.has("bool_true"));
    auto boolVal = config.get<bool>("bool_true");
    assert(boolVal.has_value() && *boolVal == true);
    
    assert(config.setFromString("bool_false", "false"));
    auto boolVal2 = config.get<bool>("bool_false");
    assert(boolVal2.has_value() && *boolVal2 == false);
    
    // Test integer parsing
    assert(config.setFromString("int_value", "42"));
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    // Test floating point parsing
    assert(config.setFromString("double_value", "3.14"));
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    // Test string parsing
    assert(config.setFromString("str_value", "hello world"));
    auto stringVal = config.get<std::string>("str_value");
    assert(stringVal.has_value() && *stringVal == "hello world");
    
    std::cout << "    testConfigSetFromString passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSummary();
    testConfigClear();
    testConfigFileFormatDetection();
    testConfigToLower();
    testConfigSetFromString();
}

} // namespace test_config

} // namespace test_config
