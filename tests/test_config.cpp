// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <algorithm>

namespace test_config {

void testConfigBasic() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigBasic passed" << std::endl;
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
    
    // Test vector support (new functionality)
    std::vector<int> intVec{1, 2, 3, 4, 5};
    config.set("int_vector", intVec, nlm::ConfigSource::Runtime);
    auto vecVal = config.get<std::vector<int>>("int_vector");
    assert(vecVal.has_value());
    assert(vecVal.value().size() == 5);
    assert(vecVal.value()[0] == 1 && vecVal.value()[4] == 5);
    
    std::vector<double> doubleVec{1.1, 2.2, 3.3};
    config.set("double_vector", doubleVec, nlm::ConfigSource::Runtime);
    auto doubleVecVal = config.get<std::vector<double>>("double_vector");
    assert(doubleVecVal.has_value());
    assert(doubleVecVal.value().size() == 3);
    
    std::vector<std::string> stringVec{"a", "b", "c"};
    config.set("string_vector", stringVec, nlm::ConfigSource::Runtime);
    auto stringVecVal = config.get<std::vector<std::string>>("string_vector");
    assert(stringVecVal.has_value());
    assert(stringVecVal.value().size() == 3);
    assert(stringVecVal.value()[0] == "a");
    
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

void testConfigFileIO() {
    std::cout << "    testConfigFileIO starting" << std::endl;
    
    nlm::Config config;
    
    // Set values to test file I/O
    config.set("test_int", 123, nlm::ConfigSource::Runtime);
    config.set("test_double", 45.67, nlm::ConfigSource::Runtime);
    config.set("test_bool", false, nlm::ConfigSource::Runtime);
    config.set("test_string", "Test String", nlm::ConfigSource::Runtime);
    config.set("test_vector", std::vector<int>{10, 20, 30}, nlm::ConfigSource::Runtime);
    
    // Save to file
    std::string testFile = "/tmp/nlm_test_config.txt";
    bool saved = config.saveToFile(testFile);
    assert(saved);
    
    // Verify file exists and has content
    std::ifstream file(testFile);
    assert(file.good());
    std::string line;
    assert(std::getline(file, line)); // Header comment
    assert(!line.empty());
    assert(line.find("# test_int") != std::string::npos);
    
    // Load from file
    nlm::Config config2;
    bool loaded = config2.loadFromFile(testFile);
    assert(loaded);
    
    // Verify loaded values
    assert(config2.has("test_int"));
    assert(config2.has("test_double"));
    assert(config2.has("test_bool"));
    assert(config2.has("test_string"));
    assert(config2.has("test_vector"));
    
    // Verify values are correct (not placeholders)
    auto intVal = config2.get<int>("test_int");
    assert(intVal.has_value() && intVal.value() == 123);
    
    auto doubleVal = config2.get<double>("test_double");
    assert(doubleVal.has_value() && doubleVal.value() == 45.67);
    
    auto boolVal = config2.get<bool>("test_bool");
    assert(boolVal.has_value() && boolVal.value() == false);
    
    auto stringVal = config2.get<std::string>("test_string");
    assert(stringVal.has_value() && stringVal.value() == "Test String");
    
    auto vecVal = config2.get<std::vector<int>>("test_vector");
    assert(vecVal.has_value());
    assert(vecVal.value().size() == 3);
    assert(vecVal.value()[0] == 10);
    
    std::cout << "    testConfigFileIO passed" << std::endl;
    
    // Clean up
    std::remove(testFile.c_str());
}

void testConfigMoveSemantics() {
    nlm::Config config1;
    config1.set("test", "value");
    
    nlm::Config config2(std::move(config1));
    assert(config2.has("test"));
    assert(config1.has("test") == false); // Should be moved from
    
    config2.set("test2", "value2");
    
    config1 = std::move(config2);
    assert(config1.has("test2"));
    
    std::cout << "    testConfigMoveSemantics passed" << std::endl;
}

void runAll() {
    testConfigBasic();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigFileIO();
    testConfigMoveSemantics();
}

} // namespace test_config

} // namespace test_config
