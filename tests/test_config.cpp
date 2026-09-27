// Config Tests
#include "core/Config/Config.hpp"
#include "core/Exception.hpp"
#include <iostream>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    if (config.has("test")) {
        throw nlm::NLMTestException("Config creation test failed: config should not have 'test' key initially");
    }
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    if (!config.has("int_value")) {
        throw nlm::NLMTestException("Config setAndGet test failed: should have 'int_value' key");
    }
    
    auto intVal = config.get<int>("int_value");
    if (!intVal.has_value()) {
        throw nlm::NLMTestException("Config setAndGet test failed: int_value should have value");
    }
    if (*intVal != 42) {
        throw nlm::NLMTestException("Config setAndGet test failed: int_value should be 42, got " + 
                                   std::to_string(*intVal));
    }
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    if (!doubleVal.has_value()) {
        throw nlm::NLMTestException("Config setAndGet test failed: double_value should have value");
    }
    if (*doubleVal <= 3.13 || *doubleVal >= 3.15) {
        throw nlm::NLMTestException("Config setAndGet test failed: double_value should be in [3.13, 3.15), got " + 
                                   std::to_string(*doubleVal));
    }
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    if (!stringVal.has_value()) {
        throw nlm::NLMTestException("Config setAndGet test failed: string_value should have value");
    }
    if (*stringVal != "hello") {
        throw nlm::NLMTestException("Config setAndGet test failed: string_value should be 'hello', got '" + 
                                   *stringVal + "'");
    }
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    if (!boolVal.has_value()) {
        throw nlm::NLMTestException("Config setAndGet test failed: bool_value should have value");
    }
    if (*boolVal != true) {
        throw nlm::NLMTestException("Config setAndGet test failed: bool_value should be true, got " + 
                                   std::to_string(*boolVal));
    }
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    if (val != 100) {
        throw nlm::NLMTestException("Config getOr test failed: missing key should return default 100, got " + 
                                   std::to_string(val));
    }
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    if (val != 50) {
        throw nlm::NLMTestException("Config getOr test failed: existing key should return its value 50, got " + 
                                   std::to_string(val));
    }
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    if (!config.has("test")) {
        throw nlm::NLMTestException("Config remove test failed: should have 'test' key after set");
    }
    
    config.remove("test");
    if (config.has("test")) {
        throw nlm::NLMTestException("Config remove test failed: should not have 'test' key after remove");
    }
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    if (keys.size() != 3) {
        throw nlm::NLMTestException("Config keys test failed: should have 3 keys, got " + 
                                   std::to_string(keys.size()));
    }
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
}

} // namespace test_config
