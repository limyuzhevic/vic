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

void testConfigSaveToFile() {
    nlm::Config config;
    
    config.set("brain.neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("brain.synapse_density", 0.1f, nlm::ConfigSource::Runtime);
    config.set("plasticity.stdp.enable", true, nlm::ConfigSource::Runtime);
    config.set("development.critical_period_start", 10.0, nlm::ConfigSource::Runtime);
    config.set("development.critical_period_end", 50.0, nlm::ConfigSource::Runtime);
    
    // Save to file
    assert(config.saveToFile("test_config.json"));
    
    // Load from file
    nlm::Config config2;
    assert(config2.loadFromFile("test_config.json"));
    
    // Verify values
    assert(config2.get<int>("brain.neuron_count").value_or(0) == 1000);
    assert(config2.get<double>("brain.synapse_density").value_or(0.0) == 0.1);
    assert(config2.get<bool>("plasticity.stdp.enable").value_or(false) == true);
    assert(config2.get<double>("development.critical_period_start").value_or(0.0) == 10.0);
    assert(config2.get<double>("development.critical_period_end").value_or(0.0) == 50.0);
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "    testConfigSaveToFile passed" << std::endl;
}

void testConfigJSONParsing() {
    nlm::Config config;
    
    // Test JSON parsing with nested keys
    config.set("brain.neuron_count", 2000, nlm::ConfigSource::Runtime);
    config.set("brain.synapse_density", 0.15, nlm::ConfigSource::Runtime);
    config.set("plasticity.stdp.enable", true, nlm::ConfigSource::Runtime);
    config.set("plasticity.stdp.learning_rate", 0.01, nlm::ConfigSource::Runtime);
    config.set("plasticity.hebbian.enable", true, nlm::ConfigSource::Runtime);
    config.set("neuromod.dopamine.scale", 2.0, nlm::ConfigSource::Runtime);
    config.set("development.critical_period_start", 20.0, nlm::ConfigSource::Runtime);
    config.set("development.critical_period_end", 100.0, nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    assert(config.saveToFile("advanced_config.json"));
    
    // Load from JSON file
    nlm::Config config2;
    assert(config2.loadFromFile("advanced_config.json"));
    
    // Verify nested structure
    assert(config2.get<int>("brain.neuron_count").value_or(0) == 2000);
    assert(config2.get<double>("brain.synapse_density").value_or(0.0) == 0.15);
    assert(config2.get<bool>("plasticity.stdp.enable").value_or(false) == true);
    assert(config2.get<double>("plasticity.stdp.learning_rate").value_or(0.0) == 0.01);
    assert(config2.get<bool>("plasticity.hebbian.enable").value_or(false) == true);
    assert(config2.get<double>("neuromod.dopamine.scale").value_or(0.0) == 2.0);
    assert(config2.get<double>("development.critical_period_start").value_or(0.0) == 20.0);
    assert(config2.get<double>("development.critical_period_end").value_or(0.0) == 100.0);
    
    // Clean up
    std::remove("advanced_config.json");
    
    std::cout << "    testConfigJSONParsing passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSaveToFile();
    testConfigJSONParsing();
}

} // namespace test_config
