// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

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
    
    // Test integer vector
    config.set("int_vector", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    auto intVectorOpt = config.get<std::vector<int>>("int_vector");
    assert(intVectorOpt.has_value());
    assert(intVectorOpt.value().size() == 3);
    assert(intVectorOpt.value()[0] == 1);
    assert(intVectorOpt.value()[1] == 2);
    assert(intVectorOpt.value()[2] == 3);
    
    // Test double vector
    config.set("double_vector", std::vector<double>({1.5, 2.7, 3.9}), nlm::ConfigSource::Runtime);
    auto doubleVectorOpt = config.get<std::vector<double>>("double_vector");
    assert(doubleVectorOpt.has_value());
    assert(doubleVectorOpt.value().size() == 3);
    assert(std::abs(doubleVectorOpt.value()[0] - 1.5f) < 0.01f);
    
    // Test string vector
    config.set("string_vector", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    auto stringVectorOpt = config.get<std::vector<std::string>>("string_vector");
    assert(stringVectorOpt.has_value());
    assert(stringVectorOpt.value().size() == 3);
    assert(stringVectorOpt.value()[0] == "a");
    
    std::cout << "    testConfigVectorTypes passed" << std::endl;
}

void testConfigClearAndKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", "value", nlm::ConfigSource::Runtime);
    config.set("key3", 3.14, nlm::ConfigSource::Runtime);
    
    assert(config.has("key1"));
    assert(config.has("key2"));
    assert(config.has("key3"));
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    assert(std::find(keys.begin(), keys.end(), "key1") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key2") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key3") != keys.end());
    
    config.clear();
    assert(config.getKeys().empty());
    assert(!config.has("key1"));
    assert(!config.has("key2"));
    assert(!config.has("key3"));
    
    std::cout << "    testConfigClearAndKeys passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("test_int", 42, nlm::ConfigSource::Default);
    config.set("test_string", "hello", nlm::ConfigSource::File);
    config.set("test_double", 3.14, nlm::ConfigSource::CommandLine);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("test_int") != std::string::npos);
    assert(summary.find("test_string") != std::string::npos);
    assert(summary.find("test_double") != std::string::npos);
    assert(summary.find("test_bool") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigVectorTypes();
    testConfigClearAndKeys();
    testConfigSummary();
}

} // namespace test_config
