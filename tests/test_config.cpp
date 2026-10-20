// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <thread>

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
    auto schema = std::make_shared<nlm::ConfigSchema>();
    schema->addRangeConstraint("neuron_count", 0.0, 1000000.0)
          .setDefault("neuron_count", 100, nlm::ConfigSource::Default, "Number of neurons in the network");
    
    schema->addRangeConstraint("learning_rate", 0.0, 1.0)
          .setDefault("learning_rate", 0.01, nlm::ConfigSource::Default, "Learning rate for synaptic plasticity");
    
    schema->addCallbackConstraint("network_name", 
        [](const nlm::ConfigValue& value) {
            std::string name = std::get<std::string>(value);
            return !name.empty() && name.length() >= 3;
        },
        "Network name must be at least 3 characters long");
    
    nlm::Config config(schema);
    
    // Test default values are set
    assert(config.getOr<int>("neuron_count", -1) == 100);
    assert(config.getOr<double>("learning_rate", -1.0) > 0.0 && config.getOr<double>("learning_rate", -1.0) < 1.0);
    
    // Test validation works
    std::string error;
    bool valid = config.validate("neuron_count", error);
    assert(valid);
    
    // Test constraint violations
    config.set("neuron_count", 1500000, nlm::ConfigSource::Runtime);
    valid = config.validate("neuron_count", error);
    assert(!valid);
    assert(error.find("range") != std::string::npos);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigGroups() {
    auto schema = std::make_shared<nlm::ConfigSchema>();
    
    schema->addGroup(nlm::ConfigGroup("Neural Network", "Neural network configuration"))
          .addGroup(nlm::ConfigGroup("Simulation", "Simulation parameters"))
          .addGroup(nlm::ConfigGroup("Learning", "Learning algorithm settings"));
    
    nlm::Config config(schema);
    
    config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config.set("learning_rate", 0.01, nlm::ConfigSource::Runtime);
    config.set("max_steps", 1000, nlm::ConfigSource::Runtime);
    
    // Test group key retrieval
    auto nnKeys = config.getKeysInGroup("Neural Network");
    // Neuron count should be in Neural Network group
    bool neuronInGroup = std::find(nnKeys.begin(), nnKeys.end(), "neuron_count") != nnKeys.end();
    assert(neuronInGroup);
    
    std::cout << "    testConfigGroups passed" << std::endl;
}

void testConfigVersioning() {
    auto schema = std::make_shared<nlm::ConfigSchema>();
    schema->setDefault("version", "1.0.0", nlm::ConfigSource::Default, "Configuration version");
    
    nlm::Config config(schema);
    
    // Test version
    auto version = config.getOr<std::string>("version", "0.0.0");
    assert(version == "1.0.0");
    
    config.setVersion(nlm::ConfigVersion(2, 1, 0));
    assert(config.getVersion().major == 2);
    assert(config.getVersion().minor == 1);
    assert(config.getVersion().patch == 0);
    
    std::cout << "    testConfigVersioning passed" << std::endl;
}

void testConfigThreadSafety() {
    auto schema = std::make_shared<nlm::ConfigSchema>();
    schema->setDefault("thread_test", 0, nlm::ConfigSource::Default, "Thread safety test");
    
    nlm::Config config(schema);
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&config, i]() {
            config.set("thread_test", i, nlm::ConfigSource::Runtime);
            auto val = config.getOr<int>("thread_test", -1);
            assert(val == i);
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "    testConfigThreadSafety passed" << std::endl;
}

void testConfigJSONSerialization() {
    auto schema = std::make_shared<nlm::ConfigSchema>();
    schema->setDefault("test_int", 42, nlm::ConfigSource::Default, "Test integer")
          .setDefault("test_string", "test_value", nlm::ConfigSource::Default, "Test string")
          .setDefault("test_bool", true, nlm::ConfigSource::Default, "Test boolean")
          .setDefault("test_double", 3.14159, nlm::ConfigSource::Default, "Test double");
    
    nlm::Config config(schema);
    config.set("custom_value", 99, nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    bool saved = config.saveToFile("/tmp/test_config.json");
    assert(saved);
    
    // Load back from JSON
    nlm::Config config2;
    bool loaded = config2.loadFromFile("/tmp/test_config.json");
    assert(loaded);
    
    // Verify loaded values
    assert(config2.getOr<int>("test_int", -1) == 42);
    assert(config2.getOr<std::string>("test_string", "") == "test_value");
    assert(config2.getOr<bool>("test_bool", false) == true);
    assert(config2.getOr<double>("test_double", -1.0) > 3.14 && config2.getOr<double>("test_double", -1.0) < 3.15);
    
    // Clean up
    std::remove("/tmp/test_config.json");
    
    std::cout << "    testConfigJSONSerialization passed" << std::endl;
}

void testConfigYAMLSerialization() {
    auto schema = std::make_shared<nlm::ConfigSchema>();
    schema->setDefault("yaml_test", "yaml_value", nlm::ConfigSource::Default, "YAML test");
    
    nlm::Config config(schema);
    
    // Save to YAML file
    bool saved = config.saveToYAMLFile("/tmp/test_config.yaml");
    assert(saved);
    
    // Load back from YAML
    nlm::Config config2;
    bool loaded = config2.loadFromYAMLFile("/tmp/test_config.yaml");
    assert(loaded);
    
    // Verify loaded values
    assert(config2.getOr<std::string>("yaml_test", "") == "yaml_value");
    
    // Clean up
    std::remove("/tmp/test_config.yaml");
    
    std::cout << "    testConfigYAMLSerialization passed" << std::endl;
}

void testConfigResetToDefaults() {
    auto schema = std::make_shared<nlm::ConfigSchema>();
    schema->setDefault("persistent", "original", nlm::ConfigSource::Default, "Persistent value")
          .setDefault("temp", "default_temp", nlm::ConfigSource::Default, "Temporary value");
    
    nlm::Config config(schema);
    
    // Change the persistent value
    config.set("persistent", "modified", nlm::ConfigSource::Runtime);
    assert(config.getOr<std::string>("persistent", "") == "modified");
    
    // Reset to defaults
    config.resetToDefaults();
    
    // Persistent value should be reset
    assert(config.getOr<std::string>("persistent", "") == "original");
    
    std::cout << "    testConfigResetToDefaults passed" << std::endl;
}

void testConfigDocumentation() {
    auto schema = std::make_shared<nlm::ConfigSchema>();
    schema->setDefault("doc_test", 42, nlm::ConfigSource::Default, "Test documentation")
          .addRangeConstraint("doc_test", 0, 100)
          .addCallbackConstraint("doc_test", 
              [](const nlm::ConfigValue& value) { return std::get<int>(value) > 0; });
    
    std::string doc = schema->generateDocumentation();
    
    // Check that documentation contains expected elements
    assert(doc.find("# Configuration Schema") != std::string::npos);
    assert(doc.find("doc_test") != std::string::npos);
    assert(doc.find("Test documentation") != std::string::npos);
    assert(doc.find("range") != std::string::npos);
    
    std::cout << "    testConfigDocumentation passed" << std::endl;
}

void testConfigErrorHandling() {
    auto schema = std::make_shared<nlm::ConfigSchema>();
    schema->addRangeConstraint("error_test", 0.0, 100.0)
          .setDefault("error_test", 50, nlm::ConfigSource::Default, "Error test parameter");
    
    nlm::Config config(schema);
    
    // Test out of range error
    config.set("error_test", 150, nlm::ConfigSource::Runtime);
    
    std::string error;
    bool valid = config.validate("error_test", error);
    assert(!valid);
    assert(!error.empty());
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigGroups();
    testConfigVersioning();
    testConfigThreadSafety();
    testConfigJSONSerialization();
    testConfigYAMLSerialization();
    testConfigResetToDefaults();
    testConfigDocumentation();
    testConfigErrorHandling();
}

} // namespace test_config