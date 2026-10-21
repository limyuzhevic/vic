// Config Tests
#include "core/Config/Config.hpp"
#include "TestFramework.hpp"
#include <cassert>
#include <iostream>

// Register tests with the test framework
RUN_TEST_CATEGORY(testConfigCreation, "Configuration")
{
    nlm::Config config;
    nlm::test::assert::isFalse(config.has("test"));
}

RUN_TEST_CATEGORY(testConfigSetAndGet, "Configuration")
{
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    nlm::test::assert::isTrue(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    nlm::test::assert::isTrue(intVal.has_value());
    nlm::test::assert::equal(*intVal, 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    nlm::test::assert::isTrue(doubleVal.has_value());
    nlm::test::assert::equal(*doubleVal, 3.14, "double value approximation");
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    nlm::test::assert::isTrue(stringVal.has_value());
    nlm::test::assert::equal(*stringVal, "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    nlm::test::assert::isTrue(boolVal.has_value());
    nlm::test::assert::equal(*boolVal, true);
}

RUN_TEST_CATEGORY(testConfigGetOr, "Configuration")
{
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    nlm::test::assert::equal(val, 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    nlm::test::assert::equal(val, 50);
}

RUN_TEST_CATEGORY(testConfigRemove, "Configuration")
{
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    nlm::test::assert::isTrue(config.has("test"));
    
    config.remove("test");
    nlm::test::assert::isFalse(config.has("test"));
}

RUN_TEST_CATEGORY(testConfigKeys, "Configuration")
{
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    nlm::test::assert::equal(keys.size(), 3);
}

// Legacy test runner for backward compatibility
void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
}
