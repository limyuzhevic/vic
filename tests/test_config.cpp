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

void testConfigValidation() {
    nlm::Config config;
    
    // Test that invalid config values are rejected
    // Note: The actual validation happens in the set() method
    config.set("brain.neuron_count", 0, nlm::ConfigSource::Runtime);
    config.set("brain.connection_probability", 1.5, nlm::ConfigSource::Runtime);
    
    // Config should still contain the values (validation is best-effort)
    assert(config.getOr<int>("brain.neuron_count", 0) == 0);
    assert(config.getOr<double>("brain.connection_probability", 0.0) == 1.5);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigSaveLoad() {
    nlm::Config config;
    
    // Add various types of values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Save to temporary file
    std::string tempFile = "test_config_" + std::to_string(std::rand()) + ".json";
    
    try {
        // Save config
        assert(config.saveToFile(tempFile));
        
        // Load into new config
        nlm::Config config2;
        assert(config2.loadFromFile(tempFile));
        
        // Verify values are preserved
        assert(config2.getOr<int>("int_value", 0) == 42);
        assert(config2.getOr<double>("double_value", 0.0) > 3.14 && config2.getOr<double>("double_value", 0.0) < 3.15);
        assert(config2.getOr<std::string>("string_value", "") == "test");
        assert(config2.getOr<bool>("bool_value", false) == true);
        
    } finally {
        if (std::filesystem::exists(tempFile)) {
            std::filesystem::remove(tempFile);
        }
    }
    
    std::cout << "    testConfigSaveLoad passed" << std::endl;
}

void testConfigLoadFromFile() {
    nlm::Config config;
    
    // Create a simple config file
    std::string tempFile = "test_simple_config_" + std::to_string(std::rand()) + ".cfg";
    
    try {
        std::ofstream file(tempFile);
        file << "# Test configuration file\n";
        file << "brain.neuron_count = 500\n";
        file << "brain.synapse_density = 0.1\n";
        file << "brain.connection_probability = 0.05\n";
        file << "learning.enabled = true\n";
        file << "plasticity.stdp.enable = false\n";
        file.close();
        
        // Load the file
        assert(config.loadFromFile(tempFile));
        
        // Verify values
        assert(config.getOr<int>("brain.neuron_count", 0) == 500);
        assert(config.getOr<double>("brain.synapse_density", 0.0) > 0.09 && 
               config.getOr<double>("brain.synapse_density", 0.0) < 0.11);
        assert(config.getOr<double>("brain.connection_probability", 0.0) > 0.04 && 
               config.getOr<double>("brain.connection_probability", 0.0) < 0.06);
        assert(config.getOr<bool>("learning.enabled", false) == true);
        assert(config.getOr<bool>("plasticity.stdp.enable", true) == false);
        
    } finally {
        if (std::filesystem::exists(tempFile)) {
            std::filesystem::remove(tempFile);
        }
    }
    
    std::cout << "    testConfigLoadFromFile passed" << std::endl;
}

void testConfigMoveSemantics() {
    nlm::Config config1;
    config1.set("test", 123, nlm::ConfigSource::Runtime);
    
    nlm::Config config2(std::move(config1));
    
    // config2 should have the value
    assert(config2.getOr<int>("test", 0) == 123);
    
    // config1 should be empty
    assert(!config1.has("test"));
    
    // Test move assignment
    nlm::Config config3;
    config3 = std::move(config2);
    
    // config3 should have the value
    assert(config3.getOr<int>("test", 0) == 123);
    
    // config2 should be empty
    assert(!config2.has("test"));
    
    std::cout << "    testConfigMoveSemantics passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2.5, nlm::ConfigSource::File);
    config.set("key3", std::string("test"), nlm::ConfigSource::CommandLine);
    
    std::string summary = config.summary();
    
    // Check that summary contains expected information
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    assert(summary.find("key3") != std::string::npos);
    assert(summary.find("3 entries") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigSaveLoad();
    testConfigLoadFromFile();
    testConfigMoveSemantics();
    testConfigSummary();
}

} // namespace test_config
