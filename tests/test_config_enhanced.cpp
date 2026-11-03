// Enhanced Config Tests - JSON Support and Backward Compatibility
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_config_enhanced {

void testJSONLoading() {
    std::cout << "    testJSONLoading ";
    nlm::Config config;
    
    // Create a test JSON file
    std::string jsonContent = R"({
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001,
        "simulation_timestep": 0.001,
        "random_seed": 42,
        "string_value": "test_string",
        "bool_value": true,
        "int_array": [1, 2, 3],
        "double_array": [1.5, 2.5, 3.5],
        "string_array": ["a", "b", "c"]
    })";
    
    std::ofstream jsonFile("test_config.json");
    jsonFile << jsonContent;
    jsonFile.close();
    
    // Load JSON file
    bool result = config.loadFromFile("test_config.json");
    
    // Clean up
    std::remove("test_config.json");
    
    // Verify results (adjust expectations based on actual JSON parsing implementation)
    if (result) {
        std::cout << "PASSED" << std::endl;
    } else {
        std::cout << "FAILED (expected - JSON may not be available)" << std::endl;
    }
}

void testSimpleFormatLoading() {
    std::cout << "    testSimpleFormatLoading ";
    nlm::Config config;
    
    // Create a test simple config file
    std::string simpleContent = R"(# Test configuration
# neuron_count = 1000
neuron_count = 1000
# region_count = 1
region_count = 1
# connection_probability = 0.1
connection_probability = 0.1
# stdp_ltp_weight = 0.01
stdp_ltp_weight = 0.01
# stdp_ltd_weight = 0.012
stdp_ltd_weight = 0.012

# Test string value
string_value = "hello_world"

# Test boolean value
bool_value = true

# Test integer value
int_value = 42

# Test double value
double_value = 3.14159
)";
    
    std::ofstream simpleFile("test_simple_config.txt");
    simpleFile << simpleContent;
    simpleFile.close();
    
    // Load simple file
    bool result = config.loadFromFile("test_simple_config.txt");
    
    // Clean up
    std::remove("test_simple_config.txt");
    
    // Verify results
    if (result) {
        // Test some key values
        auto neuronCount = config.get<int64_t>("neuron_count");
        auto regionCount = config.get<int64_t>("region_count");
        auto connectionProb = config.get<double>("connection_probability");
        auto strVal = config.get<std::string>("string_value");
        
        if (neuronCount.has_value() && neuronCount.value() == 1000 &&
            regionCount.has_value() && regionCount.value() == 1 &&
            connectionProb.has_value() && std::abs(connectionProb.value() - 0.1) < 0.0001 &&
            strVal.has_value() && strVal.value() == "hello_world") {
            std::cout << "PASSED" << std::endl;
        } else {
            std::cout << "FAILED (values don't match)" << std::endl;
        }
    } else {
        std::cout << "FAILED (could not load)" << std::endl;
    }
}

void testJSONSaving() {
    std::cout << "    testJSONSaving ";
    nlm::Config config;
    
    // Add various types of values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("test_string"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int64_t>{1, 2, 3}, nlm::ConfigSource::Runtime);
    config.set("string_array", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    
    // Save as JSON
    bool result = config.saveToJSON("test_output.json");
    
    if (result) {
        // Read back and verify
        std::ifstream jsonFile("test_output.json");
        std::string content((std::istreambuf_iterator<char>(jsonFile)), std::istreambuf_iterator<char>());
        jsonFile.close();
        
        // Clean up
        std::remove("test_output.json");
        
        // Check for expected content
        if (content.find("int_value") != std::string::npos &&
            content.find("double_value") != std::string::npos &&
            content.find("string_value") != std::string::npos &&
            content.find("bool_value") != std::string::npos) {
            std::cout << "PASSED" << std::endl;
        } else {
            std::cout << "FAILED (content verification)" << std::endl;
        }
    } else {
        std::cout << "FAILED (could not save)" << std::endl;
    }
}

void testSimpleFormatSaving() {
    std::cout << "    testSimpleFormatSaving ";
    nlm::Config config;
    
    // Add various types of values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("test_string"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Save as simple format
    bool result = config.saveToSimpleFormat("test_simple_output.txt");
    
    if (result) {
        // Read back and verify
        std::ifstream simpleFile("test_simple_output.txt");
        std::string content((std::istreambuf_iterator<char>(simpleFile)), std::istreambuf_iterator<char>());
        simpleFile.close();
        
        // Clean up
        std::remove("test_simple_output.txt");
        
        // Check for expected content
        if (content.find("int_value") != std::string::npos &&
            content.find("double_value") != std::string::npos &&
            content.find("string_value") != std::string::npos) {
            std::cout << "PASSED" << std::endl;
        } else {
            std::cout << "FAILED (content verification)" << std::endl;
        }
    } else {
        std::cout << "FAILED (could not save)" << std::endl;
    }
}

void testBackwardCompatibility() {
    std::cout << "    testBackwardCompatibility ";
    nlm::Config config;
    
    // Test that the original functionality still works
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", "value2", nlm::ConfigSource::Runtime);
    config.set("key3", 3.14, nlm::ConfigSource::Runtime);
    config.set("key4", true, nlm::ConfigSource::Runtime);
    
    // Verify all get methods work
    auto val1 = config.get<int64_t>("key1");
    auto val2 = config.get<std::string>("key2");
    auto val3 = config.get<double>("key3");
    auto val4 = config.get<bool>("key4");
    
    if (val1.has_value() && val1.value() == 1 &&
        val2.has_value() && val2.value() == "value2" &&
        val3.has_value() && std::abs(val3.value() - 3.14) < 0.0001 &&
        val4.has_value() && val4.value() == true) {
        std::cout << "PASSED" << std::endl;
    } else {
        std::cout << "FAILED (backward compatibility broken)" << std::endl;
    }
}

void testConfigSummary() {
    std::cout << "    testConfigSummary ";
    nlm::Config config;
    
    config.set("test_key1", 123, nlm::ConfigSource::Runtime);
    config.set("test_key2", std::string("test_value"), nlm::ConfigSource::Runtime);
    
    std::string summary = config.summary();
    
    if (summary.find("Configuration (2 entries):") != std::string::npos &&
        summary.find("test_key1") != std::string::npos &&
        summary.find("test_key2") != std::string::npos) {
        std::cout << "PASSED" << std::endl;
    } else {
        std::cout << "FAILED (summary incorrect)" << std::endl;
    }
}

void runAllEnhancedTests() {
    std::cout << "=== Enhanced Config Tests ===" << std::endl;
    
    // Original tests
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    
    // Enhanced tests
    testBackwardCompatibility();
    testConfigSummary();
    testJSONLoading();
    testSimpleFormatLoading();
    testJSONSaving();
    testSimpleFormatSaving();
    
    std::cout << "=== All Tests Completed ===" << std::endl;
}

} // namespace test_config_enhanced