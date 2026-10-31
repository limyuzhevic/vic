// Config Tests
#include "nlm.h"
#include <cassert>
#include <iostream>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetting() {
    nlm::Config config;
    config.set("test_key", std::string("test_value"));
    
    assert(config.has("test_key"));
    
    std::cout << "    testConfigSetting passed" << std::endl;
}

void testConfigGetting() {
    nlm::Config config;
    config.set("int_key", 42);
    config.set("string_key", std::string("hello"));
    config.set("double_key", 3.14);
    
    auto intVal = config.get<int>("int_key");
    auto stringVal = config.get<std::string>("string_key");
    auto doubleVal = config.get<double>("double_key");
    
    assert(intVal && *intVal == 42);
    assert(stringVal && *stringVal == "hello");
    assert(doubleVal && std::abs(*doubleVal - 3.14) < 0.001);
    
    std::cout << "    testConfigGetting passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    config.set("test", std::string("value"));
    
    std::string summary = config.summary();
    assert(summary.find("test") != std::string::npos);
    assert(summary.find("value") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetting();
    testConfigGetting();
    testConfigSummary();
}

} // namespace test_config
