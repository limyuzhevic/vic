// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

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

void testJSONLoading() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::string tempFile = "/tmp/test_config.json";
    std::ofstream file(tempFile);
    file << R"({"test_int": 123, "test_double": 45.67, "test_string": "hello", "test_bool": true})";
    file.close();
    
    assert(config.loadFromFile(tempFile));
    
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value());
    assert(*intVal == 123);
    
    auto doubleVal = config.get<double>("test_double");
    assert(doubleVal.has_value());
    assert(*doubleVal > 45.66 && *doubleVal < 45.68);
    
    auto stringVal = config.get<std::string>("test_string");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testJSONLoading passed" << std::endl;
}

void testYAMLLoading() {
    nlm::Config config;
    
    // Create a temporary YAML file
    std::string tempFile = "/tmp/test_config.yaml";
    std::ofstream file(tempFile);
    file << "test_int: 456\ntest_double: 78.90\ntest_string: \"world\"\ntest_bool: false";
    file.close();
    
    assert(config.loadFromFile(tempFile));
    
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value());
    assert(*intVal == 456);
    
    auto doubleVal = config.get<double>("test_double");
    assert(doubleVal.has_value());
    assert(*doubleVal > 78.89 && *doubleVal < 78.91);
    
    auto stringVal = config.get<std::string>("test_string");
    assert(stringVal.has_value());
    assert(*stringVal == "world");
    
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value());
    assert(*boolVal == false);
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testYAMLLoading passed" << std::endl;
}

void testSchemaLoading() {
    nlm::Config config;
    
    // Load default schema
    config.loadDefaultSchema();
    
    // Test that schema is loaded
    assert(nlm::schema().hasParameter("random_seed"));
    assert(nlm::schema().hasParameter("simulation_timestep"));
    assert(nlm::schema().hasParameter("neuron_count"));
    
    // Test schema parameter types
    auto param = nlm::schema().getParameter("random_seed");
    assert(param.type == nlm::ConfigParameterType::Int);
    assert(param.defaultValue == "42");
    
    param = nlm::schema().getParameter("simulation_timestep");
    assert(param.type == nlm::ConfigParameterType::Double);
    assert(param.defaultValue == "0.001");
    
    param = nlm::schema().getParameter("neuron_count");
    assert(param.type == nlm::ConfigParameterType::Int);
    
    std::cout << "    testSchemaLoading passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testJSONLoading();
    testYAMLLoading();
    testSchemaLoading();
}

} // namespace test_config

} // namespace test_config
