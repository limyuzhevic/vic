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

void testConfigLoadFromFile() {
    nlm::Config config;
    
    // Test basic key=value format parsing
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello", nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    // Test summary generation
    std::string summary = config.summary();
    assert(summary.find("Configuration") != std::string::npos);
    assert(summary.find("test_int") != std::string::npos);
    assert(summary.find("test_double") != std::string::npos);
    assert(summary.find("test_string") != std::string::npos);
    assert(summary.find("test_bool") != std::string::npos);
    
    std::cout << "    testConfigLoadFromFile passed" << std::endl;
}

void testConfigTrim() {
    // Test string trimming functionality
    std::string test1 = "  hello  ";
    std::string test2 = "\t\nworld\t\n";
    std::string test3 = "alreadyclean";
    
    assert(nlm::Config::trim(test1) == "hello");
    assert(nlm::Config::trim(test2) == "world");
    assert(nlm::Config::trim(test3) == "alreadyclean");
    assert(nlm::Config::trim("   ") == "");
    
    std::cout << "    testConfigTrim passed" << std::endl;
}

void testConfigLoadFromArgs() {
    nlm::Config config;
    
    const char* args1[] = {"test", "--test_int=42", "--test_double=3.14", "--test_string=hello", "--test_bool=true"};
    int argc1 = 5;
    config.loadFromArgs(argc1, const_cast<char**>(args1));
    
    assert(config.has("test_int"));
    assert(config.has("test_double"));
    assert(config.has("test_string"));
    assert(config.has("test_bool"));
    
    const char* args2[] = {"test", "--name", "John", "--age", "30"};
    int argc2 = 5;
    config.clear();
    config.loadFromArgs(argc2, const_cast<char**>(args2));
    
    assert(config.has("name"));
    assert(config.has("age"));
    
    std::cout << "    testConfigLoadFromArgs passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigLoadFromFile();
    testConfigTrim();
    testConfigLoadFromArgs();
}

} // namespace test_config
