// Configuration System Tests
// Tests the improved JSON/YAML configuration support

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>

namespace test_config {

void testBasicConfig() {
    std::cout << "Testing basic config functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Test setting different types
    config->set("string_key", std::string("test_value"));
    config->set("int_key", 42);
    config->set("double_key", 3.14);
    config->set("bool_key", true);
    
    // Test getting values
    assert(config->get<std::string>("string_key").value() == "test_value");
    assert(config->get<int>("int_key").value() == 42);
    assert(config->get<double>("double_key").value() == 3.14);
    assert(config->get<bool>("bool_key").value() == true);
    
    // Test getOr with default
    assert(config->getOr<std::string>("missing_key", "default") == "default");
    
    std::cout << "  PASSED" << std::endl;
}

void testFileSaveLoad() {
    std::cout << "Testing file save/load functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Set some test values
    config->set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("brain.connection_probability", 0.1f, nlm::ConfigSource::Default);
    config->set("brain.initial_weight_mean", 0.5, nlm::ConfigSource::Default);
    
    // Save to simple file format
    std::string simpleFile = "/tmp/test_config_simple.cfg";
    assert(config->saveToSimpleFile(simpleFile));
    
    // Create new config and load it
    auto config2 = std::make_shared<nlm::Config>();
    assert(config2->loadFromFile(simpleFile));
    
    // Verify values were loaded correctly
    assert(config2->get<int64_t>("brain.neuron_count").value() == 1000);
    assert(config2->get<double>("brain.connection_probability").value() == 0.1);
    assert(config2->get<double>("brain.initial_weight_mean").value() == 0.5);
    
    std::cout << "  PASSED" << std::endl;
}

#ifdef YAML_CPP_AVAILABLE
void testYAMLSaveLoad() {
    std::cout << "Testing YAML save/load functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Set some test values
    config->set("brain.neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("brain.connection_probability", 0.15, nlm::ConfigSource::Default);
    config->set("experiment.name", std::string("test_experiment"), nlm::ConfigSource::Default);
    
    // Save to YAML file
    std::string yamlFile = "/tmp/test_config.yaml";
    assert(config->saveToYAML(yamlFile));
    
    // Create new config and load it
    auto config2 = std::make_shared<nlm::Config>();
    assert(config2->loadFromFile(yamlFile));
    
    // Verify values were loaded correctly
    assert(config2->get<int64_t>("brain.neuron_count").value() == 500);
    assert(config2->get<double>("brain.connection_probability").value() == 0.15);
    assert(config2->get<std::string>("experiment.name").value() == "test_experiment");
    
    std::cout << "  PASSED" << std::endl;
}
#endif

void testJSONSaveLoad() {
    std::cout << "Testing JSON save/load functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Set some test values
    config->set("brain.neuron_count", static_cast<int64_t>(750), nlm::ConfigSource::Default);
    config->set("brain.connection_probability", 0.12, nlm::ConfigSource::Default);
    config->set("model.type", std::string("spiking_neural_network"), nlm::ConfigSource::Default);
    
    // Save to JSON file
    std::string jsonFile = "/tmp/test_config.json";
    assert(config->saveToJSON(jsonFile));
    
    // Create new config and load it
    auto config2 = std::make_shared<nlm::Config>();
    assert(config2->loadFromFile(jsonFile));
    
    // Verify values were loaded correctly
    assert(config2->get<int64_t>("brain.neuron_count").value() == 750);
    assert(config2->get<double>("brain.connection_probability").value() == 0.12);
    assert(config2->get<std::string>("model.type").value() == "spiking_neural_network");
    
    std::cout << "  PASSED" << std::endl;
}

void testConfigSummary() {
    std::cout << "Testing config summary functionality..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    config->set("test.key1", std::string("value1"));
    config->set("test.key2", 42);
    config->set("test.key3", 3.14);
    
    std::string summary = config->summary();
    assert(summary.find("Configuration (3 entries):") != std::string::npos);
    assert(summary.find("test.key1 = \"value1\"") != std::string::npos);
    assert(summary.find("test.key2 = 42") != std::string::npos);
    assert(summary.find("test.key3 = 3.14") != std::string::npos);
    
    std::cout << "  PASSED" << std::endl;
}

void testConfigKeyManagement() {
    std::cout << "Testing config key management..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Add some keys
    config->set("key1", std::string("value1"));
    config->set("key2", 100);
    config->set("key3", 2.5);
    
    // Get all keys
    auto keys = config->getKeys();
    assert(keys.size() == 3);
    assert(std::find(keys.begin(), keys.end(), "key1") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key2") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key3") != keys.end());
    
    // Remove a key
    config->remove("key2");
    assert(config->getKeys().size() == 2);
    assert(std::find(config->getKeys().begin(), config->getKeys().end(), "key2") == config->getKeys().end());
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Configuration System Tests ===" << std::endl;
    
    testBasicConfig();
    testFileSaveLoad();
    testJSONSaveLoad();
    
#ifdef YAML_CPP_AVAILABLE
    testYAMLSaveLoad();
#endif
    
    testConfigSummary();
    testConfigKeyManagement();
    
    std::cout << "=== All Configuration Tests PASSED ===" << std::endl;
}

} // namespace test_config

// Test runner for configuration system
int main() {
    test_config::runAll();
    return 0;
}
