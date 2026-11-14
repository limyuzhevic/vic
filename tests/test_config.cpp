#include "Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    std::cout << "    ✓ testConfigCreation passed" << std::endl;
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
    
    std::cout << "    ✓ testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    ✓ testConfigGetOr passed" << std::endl;
}

void testConfigFileIO() {
    nlm::Config config;
    
    // Set some test values
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14159, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello world", nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    config.set("test_array", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    
    // Save to file
    assert(config.saveToFile("test_config.txt"));
    
    // Load from file
    nlm::Config config2;
    assert(config2.loadFromFile("test_config.txt"));
    
    // Verify values
    assert(config2.has("test_int"));
    auto intVal = config2.get<int>("test_int");
    assert(intVal.has_value() && *intVal == 42);
    
    assert(config2.has("test_double"));
    auto doubleVal = config2.get<double>("test_double");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14159) < 0.0001);
    
    assert(config2.has("test_string"));
    auto stringVal = config2.get<std::string>("test_string");
    assert(stringVal.has_value() && *stringVal == "hello world");
    
    assert(config2.has("test_bool"));
    auto boolVal = config2.get<bool>("test_bool");
    assert(boolVal.has_value() && *boolVal == true);
    
    assert(config2.has("test_array"));
    auto arrayVal = config2.get<std::vector<int>>("test_array");
    assert(arrayVal.has_value() && arrayVal->size() == 3 && (*arrayVal)[0] == 1);
    
    // Clean up
    std::remove("test_config.txt");
    
    std::cout << "    ✓ testConfigFileIO passed" << std::endl;
}

void testConfigJSON() {
    // Create JSON content
    std::string jsonContent = R"({
        "brain.neuron_count": 1000,
        "brain.synapse_density": 0.1,
        "plasticity.stdp.enable": true,
        "neuromod.dopamine.scale": 1.0,
        "world.width": 20,
        "world.height": 20,
        "world.visionWidth": 8,
        "world.visionHeight": 8
    })";
    
    nlm::Config config;
    assert(config.loadFromJSON(jsonContent));
    
    // Verify values
    assert(config.has("brain.neuron_count"));
    auto neuronCount = config.get<int64_t>("brain.neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 1000);
    
    assert(config.has("brain.synapse_density"));
    auto synapseDensity = config.get<double>("brain.synapse_density");
    assert(synapseDensity.has_value() && *synapseDensity == 0.1);
    
    assert(config.has("plasticity.stdp.enable"));
    auto stdpEnable = config.get<bool>("plasticity.stdp.enable");
    assert(stdpEnable.has_value() && *stdpEnable == true);
    
    assert(config.has("neuromod.dopamine.scale"));
    auto dopamineScale = config.get<double>("neuromod.dopamine.scale");
    assert(dopamineScale.has_value() && *dopamineScale == 1.0);
    
    std::cout << "    ✓ testConfigJSON passed" << std::endl;
}

void testConfigMixedIO() {
    nlm::Config config;
    
    // Set values using C++ API
    config.set("mixed_test", 123, nlm::ConfigSource::Runtime);
    config.set("mixed_name", "test", nlm::ConfigSource::Runtime);
    
    // Save to file (should use key=value format for simple values)
    assert(config.saveToFile("mixed_config.txt"));
    
    // Load from file
    nlm::Config config2;
    assert(config2.loadFromFile("mixed_config.txt"));
    
    // Verify values
    assert(config2.has("mixed_test"));
    auto testVal = config2.get<int>("mixed_test");
    assert(testVal.has_value() && *testVal == 123);
    
    assert(config2.has("mixed_name"));
    auto nameVal = config2.get<std::string>("mixed_name");
    assert(nameVal.has_value() && *nameVal == "test");
    
    // Clean up
    std::remove("mixed_config.txt");
    
    std::cout << "    ✓ testConfigMixedIO passed" << std::endl;
}

void testConfigClearAndRemove() {
    nlm::Config config;
    
    // Add some values
    config.set("test1", 1, nlm::ConfigSource::Runtime);
    config.set("test2", 2, nlm::ConfigSource::Runtime);
    config.set("test3", 3, nlm::ConfigSource::Runtime);
    
    assert(config.getKeys().size() >= 3);
    
    // Remove one
    config.remove("test2");
    assert(!config.has("test2"));
    
    // Clear all
    config.clear();
    assert(config.getKeys().empty());
    
    std::cout << "    ✓ testConfigClearAndRemove passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("neuron_count", 500, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    config.set("plasticity", true, nlm::ConfigSource::Runtime);
    
    std::string summary = config.summary();
    assert(!summary.empty());
    
    std::cout << "    ✓ testConfigSummary passed" << std::endl;
}

void runAllTests() {
    std::cout << "Config Tests:" << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigFileIO();
    testConfigJSON();
    testConfigMixedIO();
    testConfigClearAndRemove();
    testConfigSummary();
    
    std::cout << std::endl;
    std::cout << "All config tests passed!" << std::endl;
}

}} // namespace test_config

int main() {
    test_config::runAllTests();
    return 0;
}