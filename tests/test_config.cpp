// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

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

void testConfigVectorTypes() {
    nlm::Config config;
    
    // Test std::vector<int>
    config.set("int_vector", std::vector<int>{1, 2, 3, 4, 5}, nlm::ConfigSource::Runtime);
    auto intVector = config.get<std::vector<int>>("int_vector");
    assert(intVector.has_value());
    assert(intVector.value().size() == 5);
    assert(intVector.value()[0] == 1);
    assert(intVector.value()[4] == 5);
    
    // Test std::vector<double>
    config.set("double_vector", std::vector<double>{1.1, 2.2, 3.3}, nlm::ConfigSource::Runtime);
    auto doubleVector = config.get<std::vector<double>>("double_vector");
    assert(doubleVector.has_value());
    assert(doubleVector.value().size() == 3);
    assert(doubleVector.value()[1] == 2.2);
    
    // Test std::vector<std::string>
    config.set("string_vector", std::vector<std::string>{"hello", "world", "test"}, nlm::ConfigSource::Runtime);
    auto stringVector = config.get<std::vector<std::string>>("string_vector");
    assert(stringVector.has_value());
    assert(stringVector.value().size() == 3);
    assert(stringVector.value()[0] == "hello");
    
    // Test getOr with vectors
    auto missingVector = config.getOr<std::vector<int>>("missing_vector", std::vector<int>{10, 20});
    assert(missingVector.size() == 2);
    assert(missingVector[0] == 10);
    
    std::cout << "    testConfigVectorTypes passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigVectorTypes();
}
