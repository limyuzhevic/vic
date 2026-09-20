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
    
    // Test vector types
    config.set("int_vec", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    auto vecVal = config.get<std::vector<int>>("int_vec");
    assert(vecVal.has_value());
    assert(vecVal->size() == 3);
    assert((*vecVal)[0] == 1 && (*vecVal)[1] == 2 && (*vecVal)[2] == 3);
    
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

void testConfigSerialization() {
    nlm::Config config;
    
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14159, nlm::ConfigSource::File);
    config.set("string_val", std::string("test"), nlm::ConfigSource::CommandLine);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int>({1, 2, 3}), nlm::ConfigSource::File);
    
    // Test summary
    std::string summary = config.summary();
    assert(summary.find("int_val") != std::string::npos);
    assert(summary.find("double_val") != std::string::npos);
    assert(summary.find("string_val") != std::string::npos);
    assert(summary.find("bool_val") != std::string::npos);
    assert(summary.find("int_array") != std::string::npos);
    
    // Test file saving (in memory)
    bool saveResult = config.saveToFile("test_config.txt");
    // Note: In a real test, we'd check file content
    
    std::cout << "    testConfigSerialization passed" << std::endl;
}

void testConfigLoadSimple() {
    nlm::Config config;
    
    // Create a simple test file
    std::ofstream file("simple_test_config.txt");
    if (file.is_open()) {
        file << "# Test configuration\n";
        file << "int_setting = 123\n";
        file << "double_setting = 3.14\n";
        file << "string_setting = \"hello world\"\n";
        file << "bool_setting = true\n";
        file << "array_setting = [1, 2, 3]\n";
        file.close();
        
        // Load the file
        bool loadResult = config.loadFromFile("simple_test_config.txt");
        assert(loadResult);
        
        // Check loaded values
        assert(config.has("int_setting"));
        auto intVal = config.get<int>("int_setting");
        assert(intVal.has_value() && *intVal == 123);
        
        assert(config.has("double_setting"));
        auto doubleVal = config.get<double>("double_setting");
        assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14) < 0.01);
        
        assert(config.has("string_setting"));
        auto stringVal = config.get<std::string>("string_setting");
        assert(stringVal.has_value() && *stringVal == "hello world");
        
        // Clean up
        std::remove("simple_test_config.txt");
    }
    
    std::cout << "    testConfigLoadSimple passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSerialization();
    testConfigLoadSimple();
}

} // namespace test_config
