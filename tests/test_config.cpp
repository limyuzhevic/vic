// Enhanced Config Tests
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
    
    // Valid configuration
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("region_count", 3, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1f, nlm::ConfigSource::Runtime);
    
    assert(config.validate());
    
    // Invalid configurations
    nlm::Config config2;
    config2.set("neuron_count", 0, nlm::ConfigSource::Runtime); // Invalid: must be positive
    config2.set("simulation_timestep", -0.1, nlm::ConfigSource::Runtime); // Invalid: must be positive
    config2.set("connection_probability", 1.5f, nlm::ConfigSource::Runtime); // Invalid: must be <= 1.0
    
    assert(!config2.validate());
    
    // Check validation errors
    auto errors = config2.getValidationErrors();
    assert(!errors.empty());
    assert(errors.find("neuron_count") != std::string::npos);
    assert(errors.find("simulation_timestep") != std::string::npos);
    assert(errors.find("connection_probability") != std::string::npos);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigFileIO() {
    nlm::Config config;
    
    // Set up test data
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", "test string", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("region_count", 3, nlm::ConfigSource::Runtime);
    
    // Save to file
    std::string testFile = "test_config.txt";
    assert(config.saveToFile(testFile));
    
    // Load from file
    nlm::Config config2;
    assert(config2.loadFromFile(testFile));
    
    // Verify data was loaded correctly
    auto intVal = config2.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config2.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(std::abs(*doubleVal - 3.14) < 0.001);
    
    auto stringVal = config2.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "test string");
    
    auto boolVal = config2.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigFileIO passed" << std::endl;
}

void testConfigCopyAndMerge() {
    nlm::Config config1;
    config1.set("int_value", 42, nlm::ConfigSource::Runtime);
    config1.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config1.set("string_value", "hello", nlm::ConfigSource::Runtime);
    
    // Copy config
    auto config2 = config1.copy();
    
    // Verify copy
    assert(config2->has("int_value"));
    assert(config2->has("double_value"));
    assert(config2->has("string_value"));
    
    auto intVal = config2->get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    // Merge configs
    nlm::Config config3;
    config3.set("double_value", 2.71, nlm::ConfigSource::Runtime);
    config3.set("new_value", "new", nlm::ConfigSource::Runtime);
    
    config2->merge(*config3);
    
    // Verify merge
    assert(config2->has("int_value"));
    assert(config2->has("double_value"));
    assert(config2->has("string_value"));
    assert(config2->has("new_value"));
    
    auto doubleVal = config2->get<double>("double_value");
    assert(doubleVal.has_value());
    assert(std::abs(*doubleVal - 2.71) < 0.001);
    
    auto newVal = config2->get<std::string>("new_value");
    assert(newVal.has_value());
    assert(*newVal == "new");
    
    std::cout << "    testConfigCopyAndMerge passed" << std::endl;
}

void testConfigPresets() {
    // Test development config
    auto devConfig = nlm::Config::createDevelopmentConfig();
    assert(devConfig->has("neuron_count"));
    assert(devConfig->has("region_count"));
    assert(devConfig->has("simulation_timestep"));
    assert(devConfig->has("stdp_ltp_weight"));
    assert(devConfig->has("stdp_ltd_weight"));
    
    auto neuronCount = devConfig->get<int64_t>("neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 1000);
    
    // Test exploration config
    auto explConfig = nlm::Config::createExplorationConfig();
    assert(explConfig->has("curiosity_weight_novelty"));
    assert(explConfig->has("max_exploration_chance"));
    
    // Test efficiency config
    auto effConfig = nlm::Config::createEfficiencyConfig();
    assert(effConfig->has("parallel_processing_enabled"));
    assert(effConfig->has("simd_optimization_enabled"));
    
    // Test social config
    auto socialConfig = nlm::Config::createSocialConfig();
    assert(socialConfig->has("social_tie_strength"));
    assert(socialConfig->has("cooperation_probability"));
    
    // Test adaptive config
    auto adaptiveConfig = nlm::Config::createAdaptiveConfig();
    assert(adaptiveConfig->has("meta_learning_enabled"));
    assert(adaptiveConfig->has("adaptation_threshold"));
    
    std::cout << "    testConfigPresets passed" << std::endl;
}

void testConfigTypeConversion() {
    nlm::Config config;
    
    // Test automatic type conversion from string
    config.set("int_from_string", "123", nlm::ConfigSource::Runtime);
    auto intVal = config.get<int>("int_from_string");
    assert(intVal.has_value());
    assert(*intVal == 123);
    
    config.set("double_from_string", "3.14159", nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_from_string");
    assert(doubleVal.has_value());
    assert(std::abs(*doubleVal - 3.14159) < 0.00001);
    
    config.set("bool_true", "true", nlm::ConfigSource::Runtime);
    auto boolTrue = config.get<bool>("bool_true");
    assert(boolTrue.has_value());
    assert(*boolTrue == true);
    
    config.set("bool_false", "false", nlm::ConfigSource::Runtime);
    auto boolFalse = config.get<bool>("bool_false");
    assert(boolFalse.has_value());
    assert(*boolFalse == false);
    
    std::cout << "    testConfigTypeConversion passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    // Empty config
    std::string emptySummary = config.summary();
    assert(emptySummary.find("Configuration (0 entries)") != std::string::npos);
    
    // Populated config
    config.set("test1", 1, nlm::ConfigSource::Runtime);
    config.set("test2", "hello", nlm::ConfigSource::Runtime);
    
    std::string summary = config.summary();
    assert(summary.find("Configuration (2 entries)") != std::string::npos);
    assert(summary.find("test1") != std::string::npos);
    assert(summary.find("test2") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void runAll() {
    std::cout << "Running Config tests..." << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigFileIO();
    testConfigCopyAndMerge();
    testConfigPresets();
    testConfigTypeConversion();
    testConfigSummary();
    
    std::cout << "All Config tests passed!" << std::endl;
}

} // namespace test_config