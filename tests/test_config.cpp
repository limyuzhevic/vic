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

void testConfigLoadFromJSONFile() {
    nlm::Config config;
    
    // Create a temporary JSON file
    std::string jsonContent = R"({
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.15,
        "learning_rate": 0.01,
        "enabled": true
    })";
    
    std::string jsonFile = "/tmp/test_config.json";
    std::ofstream file(jsonFile);
    file << jsonContent;
    file.close();
    
    // Load JSON file
    bool loadSuccess = config.loadFromFile(jsonFile);
    assert(loadSuccess);
    
    // Check loaded values
    auto neuronCount = config.get<size_t>("neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 1000);
    
    auto regionCount = config.get<size_t>("region_count");
    assert(regionCount.has_value());
    assert(*regionCount == 2);
    
    auto connectionProb = config.get<float>("connection_probability");
    assert(connectionProb.has_value());
    assert(*connectionProb == 0.15f);
    
    auto learningRate = config.get<float>("learning_rate");
    assert(learningRate.has_value());
    assert(*learningRate == 0.01f);
    
    auto enabled = config.get<bool>("enabled");
    assert(enabled.has_value());
    assert(*enabled == true);
    
    // Cleanup
    std::remove(jsonFile.c_str());
    
    std::cout << "    testConfigLoadFromJSONFile passed" << std::endl;
}

void testConfigLoadFromSimpleFile() {
    nlm::Config config;
    
    // Create a temporary simple config file
    std::string simpleContent = R"(
# Simple config file
neuron_count = 500
region_count = 1
connection_probability = 0.1
learning_rate = 0.005
enabled = false
)";
    
    std::string simpleFile = "/tmp/test_config_simple.txt";
    std::ofstream file(simpleFile);
    file << simpleContent;
    file.close();
    
    // Load simple file
    bool loadSuccess = config.loadFromFile(simpleFile);
    assert(loadSuccess);
    
    // Check loaded values
    auto neuronCount = config.get<size_t>("neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 500);
    
    auto regionCount = config.get<size_t>("region_count");
    assert(regionCount.has_value());
    assert(*regionCount == 1);
    
    auto connectionProb = config.get<float>("connection_probability");
    assert(connectionProb.has_value());
    assert(*connectionProb == 0.1f);
    
    // Cleanup
    std::remove(simpleFile.c_str());
    
    std::cout << "    testConfigLoadFromSimpleFile passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test loading non-existent file
    bool loadSuccess = config.loadFromFile("/tmp/nonexistent_file.json");
    assert(!loadSuccess);
    
    // Test setting invalid string to numeric value
    config.set("test_value", "invalid_number", nlm::ConfigSource::Runtime);
    auto intVal = config.get<int>("test_value");
    assert(!intVal.has_value());  // Should fail to parse
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigLoadFromJSONFile();
    testConfigLoadFromSimpleFile();
    testConfigErrorHandling();
}

} // namespace test_config
