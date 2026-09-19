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

void testConfigCommandLineParsing() {
    nlm::Config config;
    
    // Test --key=value format
    const char* args[] = {"test", "--int_value=42", "--string_value=hello"};
    int argc = 3;
    config.loadFromArgs(argc, const_cast<char**>(args));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    // Test -key value format
    nlm::Config config2;
    const char* args2[] = {"test", "-bool_value", "true", "-double_value", "3.14159"};
    int argc2 = 5;
    config2.loadFromArgs(argc2, const_cast<char**>(args2));
    
    auto boolVal = config2.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    auto doubleVal = config2.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.14158 && *doubleVal < 3.14160);
    
    std::cout << "    testConfigCommandLineParsing passed" << std::endl;
}

void testConfigValueTypes() {
    nlm::Config config;
    
    // Test integer types
    config.set("int8", static_cast<int8_t>(100), nlm::ConfigSource::Runtime);
    config.set("int16", static_cast<int16_t>(1000), nlm::ConfigSource::Runtime);
    config.set("int32", static_cast<int32_t>(10000), nlm::ConfigSource::Runtime);
    config.set("int64", static_cast<int64_t>(100000), nlm::ConfigSource::Runtime);
    
    auto int8Val = config.get<int8_t>("int8");
    assert(int8Val.has_value());
    assert(*int8Val == 100);
    
    auto int16Val = config.get<int16_t>("int16");
    assert(int16Val.has_value());
    assert(*int16Val == 1000);
    
    auto int32Val = config.get<int32_t>("int32");
    assert(int32Val.has_value());
    assert(*int32Val == 10000);
    
    auto int64Val = config.get<int64_t>("int64");
    assert(int64Val.has_value());
    assert(*int64Val == 100000);
    
    // Test string vector
    std::vector<std::string> strVec = {"apple", "banana", "cherry"};
    config.set("str_vec", strVec, nlm::ConfigSource::Runtime);
    auto vecVal = config.get<std::vector<std::string>>("str_vec");
    assert(vecVal.has_value());
    assert(vecVal->size() == 3);
    assert((*vecVal)[0] == "apple");
    
    std::cout << "    testConfigValueTypes passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigCommandLineParsing();
    testConfigValueTypes();
}

} // namespace test_config
