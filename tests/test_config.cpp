// Config Tests
#include "core/Config/Config.hpp"
#include "nlm/test_framework.h"
#include <iostream>

namespace test_config {

TEST_CASE(testConfigCreation) {
    nlm::Config config;
    ASSERT_FALSE(config.has("test"));
}

TEST_CASE(testConfigSetAndGet) {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    ASSERT_TRUE(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    ASSERT_TRUE(intVal.has_value());
    ASSERT_EQ(*intVal, 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    ASSERT_TRUE(doubleVal.has_value());
    ASSERT_TRUE(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    ASSERT_TRUE(stringVal.has_value());
    ASSERT_EQ(*stringVal, "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    ASSERT_TRUE(boolVal.has_value());
    ASSERT_EQ(*boolVal, true);
}

TEST_CASE(testConfigGetOr) {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    ASSERT_EQ(val, 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    ASSERT_EQ(val, 50);
}

TEST_CASE(testConfigRemove) {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    ASSERT_TRUE(config.has("test"));
    
    config.remove("test");
    ASSERT_FALSE(config.has("test"));
}

TEST_CASE(testConfigKeys) {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    ASSERT_EQ(keys.size(), 3);
}

std::vector<TestResult> runAllTests() {
    return test_config::runAllTests();
}

} // namespace test_config

// Simple test runner for backward compatibility
void runAll() {
    auto results = test_config::runAllTests();
    printTestSummary(results);
}
