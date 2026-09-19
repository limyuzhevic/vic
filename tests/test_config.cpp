// Config Tests - Enhanced with new features
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_config {

test_config::testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

test_config::testConfigSetAndGet() {
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

test_config::testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

test_config::testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

test_config::testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

test_config::testConfigJSONLoadSave() {
    nlm::Config config;
    
    config.set("network.layers", 3, nlm::ConfigSource::Runtime);
    config.set("network.neuron_count", 100, nlm::ConfigSource::Runtime);
    config.set("network.learning_rate", 0.001, nlm::ConfigSource::Runtime);
    config.set("system.batch_size", 32, nlm::ConfigSource::Runtime);
    config.set("system.enable_stdp", true, nlm::ConfigSource::Runtime);
    
    std::string jsonStr = config.toJSON();
    assert(!jsonStr.empty());
    
    nlm::Config config2;
    assert(config2.fromJSON(jsonStr));
    
    assert(config2.has("network.layers"));
    assert(config2.getOr<int>("network.layers", 0) == 3);
    assert(config2.has("network.neuron_count"));
    assert(config2.getOr<int>("network.neuron_count", 0) == 100);
    assert(config2.has("network.learning_rate"));
    assert(std::abs(config2.getOr<double>("network.learning_rate", 0.0) - 0.001) < 0.000001);
    
    std::cout << "    testConfigJSONLoadSave passed" << std::endl;
}

test_config::testConfigSchemaValidation() {
    nlm::Config config;
    
    // Add validation for positive integers
    config.addValidation("learning_rate", [](const nlm::ConfigValue& value) -> bool {
        return std::holds_alternative<double>(value) && std::get<double>(value) > 0.0;
    });
    
    // Valid value
    config.set("learning_rate", 0.01, nlm::ConfigSource::Runtime);
    assert(config.has("learning_rate"));
    
    // Invalid value (should still set, but validation warns)
    config.set("learning_rate", -0.01, nlm::ConfigSource::Runtime);
    
    std::cout << "    testConfigSchemaValidation passed" << std::endl;
}

test_config::testConfigDefaults() {
    nlm::Config config;
    
    // Check default values are set
    assert(config.has("seed"));
    assert(config.has("max_neurons"));
    assert(config.has("learning_rate"));
    assert(config.has("max_synapses"));
    assert(config.has("enable_stdp"));
    
    // Check default values
    assert(config.getOr<int>("seed", 0) == 42);
    assert(config.getOr<int>("max_neurons", 0) == 1000);
    assert(config.getOr<double>("learning_rate", 0.0) > 0.0);
    assert(config.getOr<int>("max_synapses", 0) == 10000);
    assert(config.getOr<bool>("enable_stdp", false) == true);
    
    std::cout << "    testConfigDefaults passed" << std::endl;
}

test_config::testConfigComparison() {
    nlm::Config config1;
    nlm::Config config2;
    
    config1.set("param1", 10, nlm::ConfigSource::Runtime);
    config1.set("param2", 20.5, nlm::ConfigSource::Runtime);
    
    config2.set("param1", 10, nlm::ConfigSource::Runtime);
    config2.set("param2", 20.5, nlm::ConfigSource::Runtime);
    
    assert(config1.equals(config2));
    
    config2.set("param2", 25.0, nlm::ConfigSource::Runtime);
    assert(!config1.equals(config2));
    
    std::cout << "    testConfigComparison passed" << std::endl;
}

test_config::testConfigSourceTracking() {
    nlm::Config config;
    
    config.set("test_key", 123, nlm::ConfigSource::Runtime);
    assert(config.hasSource("test_key", nlm::ConfigSource::Runtime));
    assert(!config.hasSource("test_key", nlm::ConfigSource::File));
    
    config.set("test_key", 456, nlm::ConfigSource::File);
    assert(config.hasSource("test_key", nlm::ConfigSource::Runtime)); // Runtime source still exists
    assert(config.hasSource("test_key", nlm::ConfigSource::File)); // File source also exists
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

test_config::testConfigDiff() {
    nlm::Config config1;
    nlm::Config config2;
    
    config1.set("param1", 10, nlm::ConfigSource::Runtime);
    config1.set("param2", 20, nlm::ConfigSource::Runtime);
    
    config2.set("param1", 10, nlm::ConfigSource::Runtime);
    config2.set("param2", 30, nlm::ConfigSource::Runtime);
    config2.set("param3", 40, nlm::ConfigSource::Runtime);
    
    std::string diff = config1.getDiff(config2);
    assert(!diff.empty());
    assert(diff.find("Modified key: param2") != std::string::npos);
    assert(diff.find("Added key: param3") != std::string::npos);
    
    std::cout << "    testConfigDiff passed" << std::endl;
}

test_config::testConfigDescription() {
    nlm::Config config;
    
    config.set("learning_rate", 0.01, nlm::ConfigSource::Runtime);
    config.setDescription("learning_rate", "Learning rate for neural network training");
    
    std::string desc = config.getDescription("learning_rate");
    assert(desc == "Learning rate for neural network training");
    
    std::cout << "    testConfigDescription passed" << std::endl;
}

test_config::testConfigLegacyFormat() {
    nlm::Config config;
    
    // Create a legacy config file
    std::ofstream legacyFile("legacy_config.txt");
    legacyFile << "# Legacy configuration file\n";
    legacyFile << "network.layers = 5\n";
    legacyFile << "network.neuron_count = 200\n";
    legacyFile << "network.learning_rate = 0.001\n";
    legacyFile.close();
    
    // Load using legacy format
    assert(config.loadFromFile("legacy_config.txt"));
    
    assert(config.has("network.layers"));
    assert(config.has("network.neuron_count"));
    assert(config.has("network.learning_rate"));
    
    assert(config.getOr<int>("network.layers", 0) == 5);
    assert(config.getOr<int>("network.neuron_count", 0) == 200);
    assert(std::abs(config.getOr<double>("network.learning_rate", 0.0) - 0.001) < 0.000001);
    
    // Clean up
    std::filesystem::remove("legacy_config.txt");
    
    std::cout << "    testConfigLegacyFormat passed" << std::endl;
}

void runAll() {
    testConfig::testConfigCreation();
    testConfig::testConfigSetAndGet();
    testConfig::testConfigGetOr();
    testConfig::testConfigRemove();
    testConfig::testConfigKeys();
    testConfig::testConfigJSONLoadSave();
    testConfig::testConfigSchemaValidation();
    testConfig::testConfigDefaults();
    testConfig::testConfigComparison();
    testConfig::testConfigSourceTracking();
    testConfig::testConfigDiff();
    testConfig::testConfigDescription();
    testConfig::testConfigLegacyFormat();
    
    std::cout << std::endl << "All tests passed!" << std::endl;
}

} // namespace test_config

// main function is in test_main.cpp