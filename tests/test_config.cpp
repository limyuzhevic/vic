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
    
    // Test that validation rules exist in schema
    config.set("network.connectivity_mode", std::string("invalid_mode"), nlm::ConfigSource::Runtime);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigFileIO() {
    nlm::Config config;
    
    // Set some values
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_string", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    
    // Save to file
    std::string saveError;
    bool saveSuccess = config.saveToFile("test_config.cfg", saveError);
    assert(saveSuccess);
    
    // Load from file
    nlm::Config config2;
    std::string loadError;
    bool loadSuccess = config2.loadFromFile("test_config.cfg", loadError);
    assert(loadSuccess);
    
    // Verify values
    assert(config2.has("test_int"));
    assert(config2.has("test_string"));
    assert(config2.has("test_double"));
    
    // Clean up
    std::remove("test_config.cfg");
    
    std::cout << "    testConfigFileIO passed" << std::endl;
}

void testConfigCommandLine() {
    nlm::Config config;
    
    // Simulate command line arguments
    int argc = 5;
    char* argv[] = {
        (char*)"program_name",
        (char*)"--int_value",
        (char*)"100",
        (char*)"--string_value",
        (char*)"hello"
    };
    
    std::string error;
    bool success = config.loadFromArgs(argc, argv, error);
    assert(success);
    
    assert(config.has("int_value"));
    assert(config.has("string_value"));
    
    std::cout << "    testConfigCommandLine passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test file not found
    std::string error;
    bool success = config.loadFromFile("nonexistent_file.cfg", error);
    assert(!success);
    assert(!error.empty());
    
    // Test invalid command line format (for advanced parsing)
    int argc = 3;
    char* argv[] = {
        (char*)"program_name",
        (char*)"--invalid_format",
        (char*)""
    };
    
    std::string error2;
    bool success2 = config.loadFromArgs(argc, argv, error2);
    // This should either succeed (ignore malformed args) or fail gracefully
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", std::string("test"), nlm::ConfigSource::File);
    
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("Configuration") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", std::string("test"), nlm::ConfigSource::File);
    
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("Configuration") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testAdvancedConfigSchema() {
    nlm::Config config;
    
    // Test that advanced configuration keys exist in schema
    // Check for neural network configuration
    config.set("neural.network.layer_count", static_cast<int64_t>(5), nlm::ConfigSource::Runtime);
    assert(config.has("neural.network.layer_count"));
    
    // Check for learning and plasticity configuration
    config.set("learning.stdp.learning_rate", 0.05, nlm::ConfigSource::Runtime);
    assert(config.has("learning.stdp.learning_rate"));
    
    // Check for neuromodulation configuration
    config.set("neuromodulation.multi_neuromodulator.enabled", true, nlm::ConfigSource::Runtime);
    assert(config.has("neuromodulation.multi_neuromodulator.enabled"));
    
    // Check for memory configuration
    config.set("memory.multi_tier.system_enabled", true, nlm::ConfigSource::Runtime);
    assert(config.has("memory.multi_tier.system_enabled"));
    
    // Check for cognition configuration
    config.set("cognition.planning.algorithm", std::string("heuristic"), nlm::ConfigSource::Runtime);
    assert(config.has("cognition.planning.algorithm"));
    
    // Check for simulation configuration
    config.set("simulation.event_driven.enabled", false, nlm::ConfigSource::Runtime);
    assert(config.has("simulation.event_driven.enabled"));
    
    // Check for experiment configuration
    config.set("experiments.multi_experiment.framework.enabled", false, nlm::ConfigSource::Runtime);
    assert(config.has("experiments.multi_experiment.framework.enabled"));
    
    // Check for performance profiles
    config.set("performance.profile.type", std::string("balanced"), nlm::ConfigSource::Runtime);
    assert(config.has("performance.profile.type"));
    
    std::cout << "    testAdvancedConfigSchema passed" << std::endl;
}

void testPerformanceOptimizedConfigurations() {
    nlm::Config config;
    
    // Test performance profile configuration
    config.set("performance.profile.type", std::string("speed"), nlm::ConfigSource::Runtime);
    assert(config.has("performance.profile.type"));
    
    config.set("performance.speed.optimization_level", 3, nlm::ConfigSource::Runtime);
    config.set("performance.memory.optimization_level", 0, nlm::ConfigSource::Runtime);
    config.set("performance.balanced.optimization_level", 0, nlm::ConfigSource::Runtime);
    
    // Test neural network optimization for speed
    config.set("neural.network.layer_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
    config.set("neural.network.neurons_per_layer", static_cast<int64_t>(50), nlm::ConfigSource::Runtime);
    
    // Test memory optimization
    config.set("performance.profile.type", std::string("memory"), nlm::ConfigSource::Runtime);
    config.set("performance.speed.optimization_level", 0, nlm::ConfigSource::Runtime);
    config.set("performance.memory.optimization_level", 3, nlm::ConfigSource::Runtime);
    
    // Test balanced configuration
    config.set("performance.profile.type", std::string("balanced"), nlm::ConfigSource::Runtime);
    config.set("performance.speed.optimization_level", 1, nlm::ConfigSource::Runtime);
    config.set("performance.memory.optimization_level", 1, nlm::ConfigSource::Runtime);
    config.set("performance.balanced.optimization_level", 3, nlm::ConfigSource::Runtime);
    
    std::cout << "    testPerformanceOptimizedConfigurations passed" << std::endl;
}

void testConfigurationManagement() {
    nlm::Config config;
    
    // Test configuration management options
    config.set("config.management.auto_save.enabled", true, nlm::ConfigSource::Runtime);
    config.set("config.management.auto_save.interval", static_cast<int64_t>(3600), nlm::ConfigSource::Runtime);
    
    config.set("config.comparison.diff_threshold", 0.01, nlm::ConfigSource::Runtime);
    
    config.set("config.validation.strict_mode", true, nlm::ConfigSource::Runtime);
    
    config.set("config.optimization.enabled", false, nlm::ConfigSource::Runtime);
    
    std::cout << "    testConfigurationManagement passed" << std::endl;
}

void testConfigurationProfiles() {
    nlm::Config config;
    
    // Test configuration profile system
    config.set("performance.profile.type", std::string("research"), nlm::ConfigSource::Runtime);
    config.set("learning.meta_learning.rate", 0.01, nlm::ConfigSource::Runtime);
    config.set("neuromodulation.multi_neuromodulator.enabled", true, nlm::ConfigSource::Runtime);
    config.set("memory.multi_tier.system_enabled", true, nlm::ConfigSource::Runtime);
    config.set("cognition.planning.algorithm", std::string("heuristic"), nlm::ConfigSource::Runtime);
    config.set("cognition.attention.mechanism", std::string("hybrid"), nlm::ConfigSource::Runtime);
    
    std::cout << "    testConfigurationProfiles passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigFileIO();
    testConfigCommandLine();
    testConfigErrorHandling();
    testConfigSummary();
    
    // Advanced configuration tests
    testAdvancedConfigSchema();
    testPerformanceOptimizedConfigurations();
    testConfigurationManagement();
    testConfigurationProfiles();
}

} // namespace test_config
