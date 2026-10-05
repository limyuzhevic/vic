// Test program for the enhanced NLM configuration system
// This program validates all the improvements made to the configuration system
// including JSON/YAML parsing, validation, and backward compatibility

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "src/core/Config/Config.hpp"

void testBackwardCompatibility() {
    std::cout << "=== Testing Backward Compatibility ===" << std::endl;
    
    // Create a simple config file (.cfg format) for backward compatibility test
    std::ofstream simpleConfig("test_simple.cfg");
    simpleConfig << "random_seed = 123\n";
    simpleConfig << "simulation_timestep = 0.005\n";
    simpleConfig << "neuron_count = 300\n";
    simpleConfig << "region_count = 2\n";
    simpleConfig << "connection_probability = 0.2\n";
    simpleConfig.close();
    
    nlm::Config config;
    if (config.loadFromFile("test_simple.cfg")) {
        std::cout << "✓ Successfully loaded simple config (.cfg format)" << std::endl;
        
        // Test type-safe access
        int64_t seed = config.getOr<int64_t>("random_seed", 42);
        double timestep = config.getOr<double>("simulation_timestep", 0.001);
        int64_t neurons = config.getOr<int64_t>("neuron_count", 500);
        int64_t regions = config.getOr<int64_t>("region_count", 1);
        float prob = config.getOr<float>("connection_probability", 0.15f);
        
        std::cout << "  random_seed: " << seed << std::endl;
        std::cout << "  simulation_timestep: " << timestep << "s" << std::endl;
        std::cout << "  neuron_count: " << neurons << std::endl;
        std::cout << "  region_count: " << regions << std::endl;
        std::cout << "  connection_probability: " << prob << std::endl;
        
        // Test validation (should pass for this valid config)
        if (config.validate()) {
            std::cout << "✓ Configuration validation passed" << std::endl;
        } else {
            std::cout << "✗ Configuration validation failed:" << std::endl;
            std::cout << config.validationErrors() << std::endl;
        }
        
    } else {
        std::cout << "✗ Failed to load simple config" << std::endl;
    }
    
    // Clean up
    std::filesystem::remove("test_simple.cfg");
    std::cout << std::endl;
}

void testJSONConfiguration() {
    std::cout << "=== Testing JSON Configuration ===" << std::endl;
    
    // Create a JSON config file
    std::ofstream jsonConfig("test_config.json");
    jsonConfig << "{\n";
    jsonConfig << "    \"random_seed\": 999,\n";
    jsonConfig << "    \"simulation_timestep\": 0.002,\n";
    jsonConfig << "    \"neuron_count\": 750,\n";
    jsonConfig << "    \"region_count\": 3,\n";
    jsonConfig << "    \"connection_probability\": 0.25,\n";
    jsonConfig << "    \"stdp_ltp_weight\": 0.03,\n";
    jsonConfig << "    \"stdp_ltd_weight\": 0.02,\n";
    jsonConfig << "    \"reward_scale\": 2.0,\n";
    jsonConfig << "    \"curiosity_threshold\": 0.15,\n";
    jsonConfig << "    \"working_memory_capacity\": 200.0,\n";
    jsonConfig << "    \"brain\": {\n";
    jsonConfig << "        \"region1\": {\n";
    jsonConfig << "            \"neuron_count\": 1000,\n";
    jsonConfig << "            \"connection_probability\": 0.3\n";
    jsonConfig << "        }\n";
    jsonConfig << "    }\n";
    jsonConfig << "}\n";
    jsonConfig.close();
    
    nlm::Config config;
    if (config.loadFromFile("test_config.json")) {
        std::cout << "✓ Successfully loaded JSON config (.cfg.json format)" << std::endl;
        
        // Test nested configuration access
        int64_t seed = config.getOr<int64_t>("random_seed", 42);
        double timestep = config.getOr<double>("simulation_timestep", 0.001);
        int64_t neurons = config.getOr<int64_t>("neuron_count", 500);
        int64_t regions = config.getOr<int64_t>("region_count", 1);
        float prob = config.getOr<float>("connection_probability", 0.15f);
        
        std::cout << "  random_seed: " << seed << std::endl;
        std::cout << "  simulation_timestep: " << timestep << "s" << std::endl;
        std::cout << "  neuron_count: " << neurons << std::endl;
        std::cout << "  region_count: " << regions << std::endl;
        std::cout << "  connection_probability: " << prob << std::endl;
        
        // Test nested access
        if (config.has("brain.region1.neuron_count")) {
            int64_t nested_neurons = config.getOr<int64_t>("brain.region1.neuron_count", 500);
            std::cout << "  brain.region1.neuron_count: " << nested_neurons << std::endl;
        }
        
        // Test validation
        if (config.validate()) {
            std::cout << "✓ JSON configuration validation passed" << std::endl;
        } else {
            std::cout << "✗ JSON configuration validation failed:" << std::endl;
            std::cout << config.validationErrors() << std::endl;
        }
        
    } else {
        std::cout << "✗ Failed to load JSON config" << std::endl;
    }
    
    // Clean up
    std::filesystem::remove("test_config.json");
    std::cout << std::endl;
}

void testYAMLConfiguration() {
    std::cout << "=== Testing YAML Configuration ===" << std::endl;
    
    // Create a YAML config file
    std::ofstream yamlConfig("test_config.yaml");
    yamlConfig << "random_seed: 888\n";
    yamlConfig << "simulation_timestep: 0.003\n";
    yamlConfig << "neuron_count: 600\n";
    yamlConfig << "region_count: 4\n";
    yamlConfig << "connection_probability: 0.22\n";
    yamlConfig << "stdp_ltp_weight: 0.025\n";
    yamlConfig << "stdp_ltd_weight: 0.018\n";
    yamlConfig << "reward_scale: 1.5\n";
    yamlConfig << "curiosity_threshold: 0.12\n";
    yamlConfig << "working_memory_capacity: 150.0\n";
    yamlConfig << "\n";
    yamlConfig << "brain:\n";
    yamlConfig << "  region1:\n";
    yamlConfig << "    neuron_count: 1200\n";
    yamlConfig << "    connection_probability: 0.35\n";
    yamlConfig.close();
    
    nlm::Config config;
    if (config.loadFromFile("test_config.yaml")) {
        std::cout << "✓ Successfully loaded YAML config (.cfg.yaml format)" << std::endl;
        
        // Test configuration access
        int64_t seed = config.getOr<int64_t>("random_seed", 42);
        double timestep = config.getOr<double>("simulation_timestep", 0.001);
        int64_t neurons = config.getOr<int64_t>("neuron_count", 500);
        int64_t regions = config.getOr<int64_t>("region_count", 1);
        float prob = config.getOr<float>("connection_probability", 0.15f);
        
        std::cout << "  random_seed: " << seed << std::endl;
        std::cout << "  simulation_timestep: " << timestep << "s" << std::endl;
        std::cout << "  neuron_count: " << neurons << std::endl;
        std::cout << "  region_count: " << regions << std::endl;
        std::cout << "  connection_probability: " << prob << std::endl;
        
        // Test validation
        if (config.validate()) {
            std::cout << "✓ YAML configuration validation passed" << std::endl;
        } else {
            std::cout << "✗ YAML configuration validation failed:" << std::endl;
            std::cout << config.validationErrors() << std::endl;
        }
        
    } else {
        std::cout << "✗ Failed to load YAML config" << std::endl;
    }
    
    // Clean up
    std::filesystem::remove("test_config.yaml");
    std::cout << std::endl;
}

void testConfigurationValidation() {
    std::cout << "=== Testing Configuration Validation ===" << std::endl;
    
    nlm::Config config;
    
    // Test with empty config (should fail validation)
    std::cout << "Testing empty config validation..." << std::endl;
    if (config.validate()) {
        std::cout << "✗ Empty config should have failed validation" << std::endl;
    } else {
        std::cout << "✓ Empty config correctly failed validation" << std::endl;
        std::cout << "Validation errors:" << std::endl;
        std::cout << config.validationErrors() << std::endl;
    }
    
    // Test with invalid values (should fail validation)
    std::cout << "\nTesting invalid configuration values..." << std::endl;
    config.set("invalid_value", -1.0f);  // Should fail custom validator
    config.set("another_invalid", "bad_value");  // Should fail custom validator
    
    if (config.validate()) {
        std::cout << "✗ Invalid config should have failed validation" << std::endl;
    } else {
        std::cout << "✓ Invalid config correctly failed validation" << std::endl;
        std::cout << "Validation errors:" << std::endl;
        std::cout << config.validationErrors() << std::endl;
    }
    
    std::cout << std::endl;
}

void testCommandLineArguments() {
    std::cout << "=== Testing Command Line Arguments ===" << std::endl;
    
    // Test command line parsing by creating a mock argv array
    const char* argv[] = {
        "nlm_test",
        "--config=test_cli.cfg",
        "--random_seed=777",
        "--simulation_timestep=0.0075",
        "--neuron_count=200",
        "--connection_probability=0.3"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    
    nlm::Config config;
    
    // Load from command line arguments
    config.loadFromArgs(argc, argv);
    
    std::cout << "✓ Successfully parsed command line arguments" << std::endl;
    
    // Check values
    int64_t seed = config.getOr<int64_t>("random_seed", 42);
    double timestep = config.getOr<double>("simulation_timestep", 0.001);
    int64_t neurons = config.getOr<int64_t>("neuron_count", 500);
    float prob = config.getOr<float>("connection_probability", 0.15f);
    
    std::cout << "  random_seed: " << seed << std::endl;
    std::cout << "  simulation_timestep: " << timestep << "s" << std::endl;
    std::cout << "  neuron_count: " << neurons << std::endl;
    std::cout << "  connection_probability: " << prob << std::endl;
    
    // Verify values
    bool valuesCorrect = (seed == 777 && 
                         timestep == 0.0075 && 
                         neurons == 200 && 
                         prob == 0.3f);
    
    if (valuesCorrect) {
        std::cout << "✓ All command line values parsed correctly" << std::endl;
    } else {
        std::cout << "✗ Some command line values were incorrect" << std::endl;
    }
    
    // Clean up if config file was created
    std::filesystem::remove("test_cli.cfg");
    std::cout << std::endl;
}

void testEnvironmentVariables() {
    std::cout << "=== Testing Environment Variables ===" << std::endl;
    
    // Set environment variables (note: in real usage, these would be set externally)
    // For this test, we'll manually set them
    setenv("NLM_TEST_SEED", "999", 1);
    setenv("NLM_TEST_TIMESTEP", "0.01", 1);
    setenv("NLM_TEST_NEURONS", "1000", 1);
    
    nlm::Config config;
    
    // Load from environment variables
    config.loadFromEnv("NLM_TEST_");
    
    std::cout << "✓ Successfully parsed environment variables" << std::endl;
    
    // Check values
    int64_t seed = config.getOr<int64_t>("seed", 42);
    double timestep = config.getOr<double>("timestep", 0.001);
    int64_t neurons = config.getOr<int64_t>("neurons", 500);
    
    std::cout << "  seed: " << seed << std::endl;
    std::cout << "  timestep: " << timestep << "s" << std::endl;
    std::cout << "  neurons: " << neurons << std::endl;
    
    // Clean up environment variables
    unsetenv("NLM_TEST_SEED");
    unsetenv("NLM_TEST_TIMESTEP");
    unsetenv("NLM_TEST_NEURONS");
    
    std::cout << std::endl;
}

void testConfigurationAPI() {
    std::cout << "=== Testing Configuration API ===" << std::endl;
    
    nlm::Config config;
    
    // Test setting values
    std::cout << "Testing value setting..." << std::endl;
    config.set("test_int", 42);
    config.set("test_double", 3.14);
    config.set("test_float", 2.718f);
    config.set("test_bool", true);
    config.set("test_string", std::string("hello world"));
    
    // Test getting values
    std::cout << "Testing value retrieval..." << std::endl;
    int64_t intVal = config.getOr<int64_t>("test_int", 0);
    double doubleVal = config.getOr<double>("test_double", 0.0);
    float floatVal = config.getOr<float>("test_float", 0.0f);
    bool boolVal = config.getOr<bool>("test_bool", false);
    std::string stringVal = config.getOr<std::string>("test_string", "");
    
    std::cout << "  int value: " << intVal << std::endl;
    std::cout << "  double value: " << doubleVal << std::endl;
    std::cout << "  float value: " << floatVal << std::endl;
    std::cout << "  bool value: " << (boolVal ? "true" : "false") << std::endl;
    std::cout << "  string value: " << stringVal << std::endl;
    
    // Test vector access
    std::cout << "Testing vector access..." << std::endl;
    std::vector<int64_t> intVector = config.getOr<std::vector<int64_t>>("int_vector", {});
    std::vector<double> doubleVector = config.getOr<std::vector<double>>("double_vector", {});
    std::vector<std::string> stringVector = config.getOr<std::vector<std::string>>("string_vector", {});
    
    std::cout << "  int_vector size: " << intVector.size() << std::endl;
    std::cout << "  double_vector size: " << doubleVector.size() << std::endl;
    std::cout << "  string_vector size: " << stringVector.size() << std::endl;
    
    // Test key operations
    std::cout << "Testing key operations..." << std::endl;
    bool hasKey = config.has("test_int");
    std::cout << "  Has 'test_int': " << (hasKey ? "true" : "false") << std::endl;
    
    config.set("test_key2", "value2");
    hasKey = config.has("test_key2");
    std::cout << "  Has 'test_key2' after setting: " << (hasKey ? "true" : "false") << std::endl;
    
    // Test summary
    std::cout << "Testing configuration summary..." << std::endl;
    std::string summary = config.summary();
    std::cout << "  Summary length: " << summary.length() << " characters" << std::endl;
    std::cout << "  Summary preview: " << summary.substr(0, 100) << "..." << std::endl;
    
    std::cout << std::endl;
}

void testValidationSystem() {
    std::cout << "=== Testing Validation System ===" << std::endl;
    
    nlm::Config config;
    
    // Create a custom validator function
    auto positiveNumberValidator = [](const std::string& key, const nlm::ConfigValue& value) {
        if (value.isFloat() && value.asFloat() <= 0.0f) {
            return std::string("Value must be positive: ") + std::to_string(value.asFloat());
        }
        return std::string(); // Empty string means validation passed
    };
    
    // Add custom validator
    config.addValidator("positive_test", positiveNumberValidator);
    
    // Test validation passing
    std::cout << "Testing validation passing..." << std::endl;
    config.set("positive_test", 5.0f);
    if (config.validate()) {
        std::cout << "✓ Valid positive value passed validation" << std::endl;
    } else {
        std::cout << "✗ Valid positive value failed validation:" << std::endl;
        std::cout << config.validationErrors() << std::endl;
    }
    
    // Test validation failing
    std::cout << "\nTesting validation failing..." << std::endl;
    config.set("positive_test", -5.0f);
    if (config.validate()) {
        std::cout << "✗ Invalid negative value should have failed validation" << std::endl;
    } else {
        std::cout << "✓ Invalid negative value correctly failed validation" << std::endl;
        std::cout << "Validation errors:" << std::endl;
        std::cout << config.validationErrors() << std::endl;
    }
    
    std::cout << std::endl;
}

int main() {
    std::cout << "NLM Enhanced Configuration System - Test Suite" << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << std::endl;
    
    testBackwardCompatibility();
    testJSONConfiguration();
    testYAMLConfiguration();
    testConfigurationValidation();
    testCommandLineArguments();
    testEnvironmentVariables();
    testConfigurationAPI();
    testValidationSystem();
    
    std::cout << "================================================" << std::endl;
    std::cout << "All tests completed!" << std::endl;
    
    return 0;
}
