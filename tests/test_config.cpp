// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <sstream>

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

void testConfigStringConversion() {
    nlm::Config config;
    
    // Test string to int conversion
    config.set("str_int", std::string("42"), nlm::ConfigSource::Runtime);
    auto intVal = config.get<int>("str_int");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    // Test string to double conversion
    config.set("str_double", std::string("3.14159"), nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("str_double");
    assert(doubleVal.has_value());
    assert(std::abs(*doubleVal - 3.14159) < 0.0001);
    
    // Test string to bool conversion
    config.set("str_true", std::string("true"), nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("str_true");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    config.set("str_false", std::string("false"), nlm::ConfigSource::Runtime);
    boolVal = config.get<bool>("str_false");
    assert(boolVal.has_value());
    assert(*boolVal == false);
    
    // Test invalid string conversion
    config.set("invalid_int", std::string("abc"), nlm::ConfigSource::Runtime);
    auto invalidVal = config.get<int>("invalid_int");
    assert(!invalidVal.has_value());  // Should fail to convert
    
    std::cout << "    testConfigStringConversion passed" << std::endl;
}

void testConfigLoadFromSimpleFormat() {
    nlm::Config config;
    
    // Test saving to file and loading back
    config.set("test_int", 123, nlm::ConfigSource::Runtime);
    config.set("test_string", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("test_double", 45.67, nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    // Save to temporary file
    const std::string tempFile = "/tmp/test_config.cfg";
    bool saveSuccess = config.saveToFile(tempFile);
    assert(saveSuccess);
    
    // Clear and load back
    config.clear();
    bool loadSuccess = config.loadFromSimpleFormat(tempFile);
    assert(loadSuccess);
    
    // Verify loaded values
    assert(config.has("test_int"));
    assert(config.getOr<int>("test_int", 0) == 123);
    
    assert(config.has("test_string"));
    assert(config.getOr<std::string>("test_string", "") == "hello");
    
    assert(config.has("test_double"));
    assert(std::abs(config.getOr<double>("test_double", 0.0) - 45.67) < 0.001);
    
    assert(config.has("test_bool"));
    assert(config.getOr<bool>("test_bool", false) == true);
    
    std::cout << "    testConfigLoadFromSimpleFormat passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Default);
    config.set("key2", std::string("test"), nlm::ConfigSource::File);
    config.set("key3", 3.14, nlm::ConfigSource::CommandLine);
    config.set("key4", false, nlm::ConfigSource::Runtime);
    
    std::string summary = config.summary();
    assert(!summary.empty());
    
    // Check that summary contains our keys
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    assert(summary.find("key3") != std::string::npos);
    assert(summary.find("key4") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigStringConversion();
    testConfigLoadFromSimpleFormat();
    testConfigSummary();
    
    std::cout << "All config tests passed!" << std::endl;
}

} // namespace test_config