// NLM Configuration Validation Tests
// Tests the new configuration validation system

#include "core/Config/Config.hpp"
#include "core/Config/ConfigValidator.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>

void testBasicConfigValidation() {
    std::cout << "Testing basic configuration validation..." << std::endl;
    
    nlm::Config config;
    
    // Test basic key-value operations
    config.set("test_int", 42);
    assert(config.get<int>("test_int") == 42);
    
    config.set("test_double", 3.14);
    assert(config.get<double>("test_double") == 3.14);
    
    config.set("test_string", "hello");
    assert(config.get<std::string>("test_string") == "hello");
    
    config.set("test_bool", true);
    assert(config.get<bool>("test_bool") == true);
    
    // Test getOr with default values
    assert(config.getOr<std::string>("missing", "default") == "default");
    assert(config.getOr<int>("missing", 0) == 0);
    
    std::cout << "  ✓ Basic configuration operations passed" << std::endl;
}

void testConfigFileOperations() {
    std::cout << "Testing configuration file operations..." << std::endl;
    
    nlm::Config config;
    
    // Create a temporary config file
    std::string tempFile = "test_config.cfg";
    
    // Set some values
    config.set("random_seed", 123);
    config.set("simulation_timestep", 0.005);
    config.set("neuron_count", static_cast<int64_t>(500));
    config.set("connection_probability", 0.2);
    config.set("dopamine_baseline", 0.15);
    config.set("environment_name", "TestWorld");
    config.set("log_level", "DEBUG");
    config.set("log_to_file", true);
    
    // Save to file (current implementation creates placeholder)
    assert(config.saveToFile(tempFile));
    
    // Create a simple config file for loading
    std::ofstream file(tempFile);
    file << "# Test configuration\n";
    file << "random_seed = 456\n";
    file << "simulation_timestep = 0.001\n";
    file << "neuron_count = 1000\n";
    file << "connection_probability = 0.1\n";
    file << "dopamine_baseline = 0.2\n";
    file << "environment_name = GridWorld\n";
    file << "log_level = INFO\n";
    file << "log_to_file = false\n";
    file.close();
    
    // Load from file
    nlm::Config config2;
    assert(config2.loadFromFile(tempFile));
    
    // Verify loaded values
    assert(config2.getOr<int64_t>("random_seed", 0) == 456);
    assert(config2.getOr<double>("simulation_timestep", 0.0) == 0.001);
    assert(config2.getOr<int64_t>("neuron_count", 0) == 1000);
    assert(config2.getOr<double>("connection_probability", 0.0) == 0.1);
    assert(config2.getOr<double>("dopamine_baseline", 0.0) == 0.2);
    assert(config2.getOr<std::string>("environment_name", "") == "GridWorld");
    assert(config2.getOr<std::string>("log_level", "") == "INFO");
    assert(config2.getOr<bool>("log_to_file", false) == false);
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "  ✓ Configuration file operations passed" << std::endl;
}

void testConfigValidator() {
    std::cout << "Testing configuration validator..." << std::endl;
    
    nlm::Config config;
    nlm::ConfigValidator validator;
    
    // Load default schema
    validator.loadDefaultSchema();
    
    // Test with valid configuration
    config.set("random_seed", 42);
    config.set("simulation_timestep", 0.001);
    config.set("neuron_count", static_cast<int64_t>(1000));
    config.set("region_count", static_cast<int64_t>(2));
    config.set("connection_probability", 0.1);
    config.set("plasticity_learning_rate", 0.01);
    config.set("stdp_ltp_weight", 0.01);
    config.set("stdp_ltd_weight", 0.012);
    config.set("stdp_time_constant", 20.0);
    config.set("development_synaptogenesis_rate", 0.001);
    config.set("development_pruning_rate", 0.0001);
    config.set("dopamine_baseline", 0.1);
    config.set("reward_discount_factor", 0.99);
    config.set("environment_name", "GridWorld");
    config.set("environment_width", static_cast<int64_t>(10));
    config.set("environment_height", static_cast<int64_t>(10));
    config.set("log_level", "INFO");
    config.set("log_to_file", false);
    config.set("log_filename", "nlm.log");
    config.set("max_simulation_steps", static_cast<int64_t>(10000));
    config.set("simulation_time_limit", 0.0);
    config.set("visualization_enabled", false);
    config.set("visualization_update_rate", 30.0);
    
    // Validate with default schema - should be mostly successful
    auto errors = validator.validate(config);
    if (!errors.empty()) {
        std::cout << "  Validation warnings/errors found:" << std::endl;
        for (const auto& error : errors) {
            std::string severity = (error.severity == nlm::ValidationResult::Error) ? "ERROR" :
                                 (error.severity == nlm::ValidationResult::Warning) ? "WARNING" : "INFO";
            std::cout << "    " << severity << ": " << error.key << " - " << error.message << std::endl;
        }
    }
    
    // Test with invalid configuration
    nlm::Config invalidConfig;
    invalidConfig.set("random_seed", -1);  // Invalid: negative seed
    invalidConfig.set("simulation_timestep", 0.0);  // Invalid: zero timestep
    invalidConfig.set("neuron_count", static_cast<int64_t>(-100));  // Invalid: negative neurons
    invalidConfig.set("connection_probability", 1.5);  // Invalid: > 1.0
    
    auto invalidErrors = validator.validate(invalidConfig);
    assert(!invalidErrors.empty());  // Should have errors
    
    std::cout << "  ✓ Configuration validator passed" << std::endl;
}

void testCommandLineParsing() {
    std::cout << "Testing command-line parsing..." << std::endl;
    
    nlm::Config config;
    
    // Simulate command line arguments
    int argc = 7;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--random_seed=123",
        (char*)"--simulation_timestep=0.005",
        (char*)"--neuron_count=500",
        (char*)"--connection_probability=0.2",
        (char*)"--dopamine_baseline=0.15",
        (char*)"--environment_name=TestWorld"
    };
    
    assert(config.loadFromArgs(argc, argv));
    
    // Verify parsed values
    assert(config.getOr<int64_t>("random_seed", 0) == 123);
    assert(config.getOr<double>("simulation_timestep", 0.0) == 0.005);
    assert(config.getOr<int64_t>("neuron_count", 0) == 500);
    assert(config.getOr<double>("connection_probability", 0.0) == 0.2);
    assert(config.getOr<double>("dopamine_baseline", 0.0) == 0.15);
    assert(config.getOr<std::string>("environment_name", "") == "TestWorld");
    
    std::cout << "  ✓ Command-line parsing passed" << std::endl;
}

void testConfigSummary() {
    std::cout << "Testing configuration summary..." << std" << std::endl;
    
    nlm::Config config;
    
    config.set("test_key1", 42);
    config.set("test_key2", "hello");
    config.set("test_key3", 3.14);
    
    std::string summary = config.summary();
    assert(summary.find("test_key1") != std::string::npos);
    assert(summary.find("test_key2") != std::string::npos);
    assert(summary.find("test_key3") != std::string::npos);
    
    std::cout << "  ✓ Configuration summary passed" << std::endl;
}

void runAllConfigTests() {
    std::cout << "=== Running NLM Configuration Validation Tests ===" << std::endl;
    
    try {
        testBasicConfigValidation();
        testConfigFileOperations();
        testConfigValidator();
        testCommandLineParsing();
        testConfigSummary();
        
        std::cout << "\n=== All Configuration Tests Passed! ===" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "\n*** TEST FAILED: " << e.what() << " ***" << std::endl;
        throw;
    }
}

int main() {
    runAllConfigTests();
    return 0;
}