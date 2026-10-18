// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

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
    
    // Valid values
    config.setValidated("neuron.threshold", 45.0, nlm::ConfigSource::Runtime);
    assert(config.has("neuron.threshold"));
    
    config.setValidated("synapse.weight", 0.5, nlm::ConfigSource::Runtime);
    assert(config.has("synapse.weight"));
    
    config.setValidated("simulation.timestep", 0.01, nlm::ConfigSource::Runtime);
    assert(config.has("simulation.timestep"));
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigFactoryMethods() {
    // Test default config factory
    auto config1 = nlm::Config::createDefaultConfig();
    assert(config1.has("simulation.timestep"));
    assert(config1.has("neuron.threshold"));
    
    // Test minimal config factory
    auto config2 = nlm::Config::createMinimalConfig();
    assert(config2.has("simulation.timestep"));
    assert(config2.has("neuron.threshold"));
    
    // Test fast simulation config
    auto config3 = nlm::Config::createFastSimulationConfig();
    assert(config3.has("simulation.timestep"));
    assert(config3.getOr<double>("simulation.timestep", 0.0) < 0.02);
    
    // Test accurate simulation config
    auto config4 = nlm::Config::createAccurateSimulationConfig();
    assert(config4.has("simulation.timestep"));
    assert(config4.getOr<double>("simulation.timestep", 0.0) > 0.01);
    
    std::cout << "    testConfigFactoryMethods passed" << std::endl;
}

void testConfigPresets() {
    // Test available presets
    auto presets = nlm::Config::PRESETS;
    assert(!presets.empty());
    assert(presets.find("default") != presets.end());
    assert(presets.find("minimal") != presets.end());
    assert(presets.find("fast") != presets.end());
    assert(presets.find("accurate") != presets.end());
    
    // Test preset contents
    auto defaultConfig = presets.at("default");
    assert(defaultConfig.has("simulation.timestep"));
    assert(defaultConfig.has("neuron.threshold"));
    
    auto fastConfig = presets.at("fast");
    assert(fastConfig.has("simulation.timestep"));
    
    std::cout << "    testConfigPresets passed" << std::endl;
}

void testConfigStatistics() {
    nlm::Config config;
    
    config.set("string_key", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("number_key", 42, nlm::ConfigSource::Runtime);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    config.set("array_key", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    
    auto stats = config.getStats();
    assert(stats.totalEntries == 4);
    assert(stats.stringValues == 1);
    assert(stats.numericValues == 1);
    assert(stats.boolValues == 1);
    assert(stats.arrayValues == 1);
    
    // Test filtered entries
    auto runtimeEntries = config.getFilteredEntries(nlm::ConfigSource::Runtime);
    assert(runtimeEntries.size() == 4);
    
    auto defaultEntries = config.getFilteredEntries(nlm::ConfigSource::Default);
    assert(defaultEntries.empty());
    
    std::cout << "    testConfigStatistics passed" << std::endl;
}

void testConfigMerging() {
    nlm::Config baseConfig;
    baseConfig.set("key1", "value1", nlm::ConfigSource::Default);
    baseConfig.set("key2", "value2", nlm::ConfigSource::File);
    
    nlm::Config overrideConfig;
    overrideConfig.set("key2", "override", nlm::ConfigSource::CommandLine);
    overrideConfig.set("key3", "value3", nlm::ConfigSource::Runtime);
    
    bool merged = baseConfig.mergeConfig(overrideConfig);
    assert(merged);
    
    assert(baseConfig.has("key1"));
    assert(baseConfig.getOr<std::string>("key1", "") == "value1");
    assert(baseConfig.has("key2"));
    // CommandLine should override File
    assert(baseConfig.getOr<std::string>("key2", "") == "override");
    assert(baseConfig.has("key3"));
    assert(baseConfig.getOr<std::string>("key3", "") == "value3");
    
    std::cout << "    testConfigMerging passed" << std::endl;
}

void testConfigFileIO() {
    nlm::Config config;
    config.set("int_value", 123, nlm::ConfigSource::Runtime);
    config.set("string_value", "test", nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Test JSON file save/load
    std::string testFile = "test_config.json";
    bool saveSuccess = config.saveToFile(testFile);
    assert(saveSuccess);
    
    nlm::Config loadedConfig;
    bool loadSuccess = loadedConfig.loadFromFile(testFile);
    assert(loadSuccess);
    
    assert(loadedConfig.has("int_value"));
    assert(loadedConfig.has("string_value"));
    assert(loadedConfig.has("double_value"));
    assert(loadedConfig.has("bool_value"));
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigFileIO passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigFactoryMethods();
    testConfigPresets();
    testConfigStatistics();
    testConfigMerging();
    testConfigFileIO();
}

} // namespace test_config
