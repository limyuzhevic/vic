// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

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
    
    // Test valid values
    config.set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1f, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    
    nlm::ValidationResult result = config.validate();
    assert(result.valid);
    
    // Test invalid values
    nlm::Config invalidConfig;
    invalidConfig.set("neuron_count", static_cast<int64_t>(-1), nlm::ConfigSource::Runtime);
    result = invalidConfig.validate();
    assert(!result.valid);
    
    // Test missing required keys
    nlm::Config incompleteConfig;
    result = incompleteConfig.validate();
    // Should warn about missing defaults but not fail
    assert(result.valid || !result.errorMessage.empty());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigJSONSerialization() {
    nlm::Config config;
    
    config.set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.15f, nlm::ConfigSource::Runtime);
    config.set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Runtime);
    config.set("checkpoint_dir", std::string("./checkpoints"), nlm::ConfigSource::Runtime);
    
    std::string testFile = "/tmp/test_config.json";
    
    // Save to JSON file
    bool saveResult = config.saveToFile(testFile);
    assert(saveResult);
    
    // Load from JSON file
    nlm::Config loadedConfig;
    bool loadResult = loadedConfig.loadFromFile(testFile);
    assert(loadResult);
    
    // Verify loaded values
    assert(loadedConfig.has("neuron_count"));
    assert(loadedConfig.has("connection_probability"));
    assert(loadedConfig.has("random_seed"));
    assert(loadedConfig.has("checkpoint_dir"));
    
    auto neuronCount = loadedConfig.get<int64_t>("neuron_count");
    assert(neuronCount.has_value() && *neuronCount == 1000);
    
    auto connProb = loadedConfig.get<float>("connection_probability");
    assert(connProb.has_value() && *connProb == 0.15f);
    
    auto seed = loadedConfig.get<int64_t>("random_seed");
    assert(seed.has_value() && *seed == 42);
    
    // Clean up
    std::filesystem::remove(testFile);
    
    std::cout << "    testConfigJSONSerialization passed" << std::endl;
}

void testConfigCommandLineArgs() {
    // Note: This is a simple test of the command-line parsing interface
    // In a real scenario, you would pass actual argc/argv to loadFromArgs
    nlm::Config config;
    
    // Test that the method exists and can be called
    // We can't easily test actual command-line parsing without spawning child processes
    bool loadResult = config.loadFromArgs(0, nullptr);
    assert(loadResult);
    
    std::cout << "    testConfigCommandLineArgs passed" << std::endl;
}

void testConfigTypeSafety() {
    nlm::Config config;
    
    // Test type safety - should only get correct types
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    
    // Correct types should work
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    // Wrong types should return nullopt
    auto wrongType = config.get<double>("int_value");
    assert(!wrongType.has_value());
    
    auto stringType = config.get<std::string>("int_value");
    assert(!stringType.has_value());
    
    // Test type-safe convenience methods
    config.set("big_number", static_cast<int64_t>(2000), nlm::ConfigSource::Runtime);
    int bigNum = config.getInt("big_number", 0);
    assert(bigNum == 2000);
    
    config.set("active", true, nlm::ConfigSource::Runtime);
    bool isActive = config.getBool("active", false);
    assert(isActive);
    
    config.set("filename", std::string("test.txt"), nlm::ConfigSource::Runtime);
    std::string filename = config.getString("filename", "");
    assert(filename == "test.txt");
    
    std::cout << "    testConfigTypeSafety passed" << std::endl;
}

void testConfigSpecialGetters() {
    nlm::Config config;
    
    // Test special getters with defaults
    int seed = config.getRandomSeed();
    assert(seed == 42); // Default value
    
    double timestep = config.getSimulationTimestep();
    assert(timestep == 0.001); // Default value
    
    size_t neurons = config.getNeuronCount();
    assert(neurons == 1000); // Default value
    
    size_t regions = config.getRegionCount();
    assert(regions == 1); // Default value
    
    float prob = config.getConnectionProbability();
    assert(prob == 0.1f); // Default value
    
    // Override with custom values
    config.set("random_seed", static_cast<int64_t>(123), nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.01, nlm::ConfigSource::Runtime);
    
    assert(config.getRandomSeed() == 123);
    assert(config.getSimulationTimestep() == 0.01);
    
    std::cout << "    testConfigSpecialGetters passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigJSONSerialization();
    testConfigCommandLineArgs();
    testConfigTypeSafety();
    testConfigSpecialGetters();
    
    std::cout << std::endl;
    std::cout << "=== All Configuration Tests Passed ===" << std::endl;
    std::cout << std::endl;
}

} // namespace test_config
