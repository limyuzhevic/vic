// Config Tests
#include "core/Config/Config.hpp"
#include "core/test/TestFramework.hpp"
#include <iostream>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    nlm::test::TestAssert::equal(false, config.has("test"), "testConfigCreation");
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    nlm::test::TestAssert::equal(true, config.has("int_value"), "testConfigSetAndGet");
    
    auto intVal = config.get<int>("int_value");
    nlm::test::TestAssert::equal(true, intVal.has_value(), "testConfigSetAndGet");
    nlm::test::TestAssert::equal(42, *intVal, "testConfigSetAndGet");
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    nlm::test::TestAssert::equal(true, doubleVal.has_value(), "testConfigSetAndGet");
    nlm::test::TestAssert::equal(3.14, *doubleVal, "testConfigSetAndGet");
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    nlm::test::TestAssert::equal(true, stringVal.has_value(), "testConfigSetAndGet");
    nlm::test::TestAssert::equal(std::string("hello"), *stringVal, "testConfigSetAndGet");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    nlm::test::TestAssert::equal(true, boolVal.has_value(), "testConfigSetAndGet");
    nlm::test::TestAssert::equal(true, *boolVal, "testConfigSetAndGet");
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    nlm::test::TestAssert::equal(100, val, "testConfigGetOr");
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    nlm::test::TestAssert::equal(50, val, "testConfigGetOr");
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    nlm::test::TestAssert::equal(true, config.has("test"), "testConfigRemove");
    
    config.remove("test");
    nlm::test::TestAssert::equal(false, config.has("test"), "testConfigRemove");
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    nlm::test::TestAssert::equal(3, keys.size(), "testConfigKeys");
    
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
