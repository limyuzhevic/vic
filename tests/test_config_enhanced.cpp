#include "../src/core/Config/Config.hpp"
#include "../src/core/Logger/Logger.hpp"
#include <iostream>
#include <cassert>
#include <fstream>

namespace test_config_enhanced {

void testBasicConfig() {
    std::cout << "Testing Enhanced Config System..." << std::endl;
    
    nlm::Config config;
    
    // Test basic set/get operations
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", "test_string");
    config.set("bool_value", true);
    
    assert(config.get<int64_t>("int_value").value() == 42);
    assert(config.get<double>("double_value").value() == 3.14);
    assert(config.get<std::string>("string_value").value() == "test_string");
    assert(config.get<bool>("bool_value").value() == true);
    
    // Test getOr with defaults
    assert(config.getOr<int64_t>("missing_int", 99) == 99);
    assert(config.getOr<double>("missing_double", 2.71) == 2.71);
    assert(config.getOr<std::string>("missing_string", "default") == "default");
    assert(config.getOr<bool>("missing_bool", false) == false);
    
    std::cout << "✓ Basic Config operations test passed" << std::endl;
}

void testConfigFileIO() {
    std::cout << "Testing Config File I/O..." << std::endl;
    
    nlm::Config config;
    
    // Set some test values
    config.set("neuron_count", 500);
    config.set("connection_probability", 0.15f);
    config.set("enable_development", true);
    config.set("learning_rate", 0.01);
    
    // Save to file
    std::string testFile = "/tmp/test_config.cfg";
    assert(config.saveToFile(testFile));
    
    // Create new config and load from file
    nlm::Config config2;
    assert(config2.loadFromFile(testFile));
    
    // Verify values
    assert(config2.getOr<int64_t>("neuron_count", 0) == 500);
    assert(config2.getOr<double>("connection_probability", 0.0) == 0.15);
    assert(config2.getOr<bool>("enable_development", false) == true);
    assert(config2.getOr<double>("learning_rate", 0.0) == 0.01);
    
    std::cout << "✓ Config File I/O test passed" << std::endl;
}

void testConfigCommandLine() {
    std::cout << "Testing Config Command Line..." << std::endl;
    
    // Test with hardcoded arguments (simulating command line)
    char* testArgs[] = {
        (char*)"program_name",
        (char*)"--neuron_count",
        (char*)"1000",
        (char*)"--connection_probability",
        (char*)"0.2",
        (char*)"--enable_development",
        (char*)"true",
        (char*)"--learning_rate",
        (char*)"0.05",
        nullptr
    };
    
    nlm::Config config;
    assert(config.loadFromArgs(9, testArgs));
    
    // Verify values
    assert(config.getOr<int64_t>("neuron_count", 0) == 1000);
    assert(config.getOr<double>("connection_probability", 0.0) == 0.2);
    assert(config.getOr<bool>("enable_development", false) == true);
    assert(config.getOr<double>("learning_rate", 0.0) == 0.05);
    
    std::cout << "✓ Config Command Line test passed" << std::endl;
}

void testConfigJSONSupport() {
    std::cout << "Testing Config JSON Support..." << std::endl;
    
    // Create JSON config file
    std::string jsonFile = "/tmp/test_config.json";
    std::ofstream jsonOut(jsonFile);
    jsonOut << "{\n";
    jsonOut << "  \"neuron_count\": 750,\n";
    jsonOut << "  \"connection_probability\": 0.25,\n";
    jsonOut << "  \"enable_development\": true,\n";
    jsonOut << "  \"learning_rate\": 0.03,\n";
    jsonOut << "  \"array_values\": [1, 2, 3, 4, 5]\n";
    jsonOut << "}\n";
    jsonOut.close();
    
    // Load JSON config
    nlm::Config config;
    assert(config.loadFromFile(jsonFile));
    
    // Verify JSON values
    assert(config.getOr<int64_t>("neuron_count", 0) == 750);
    assert(config.getOr<double>("connection_probability", 0.0) == 0.25);
    assert(config.getOr<bool>("enable_development", false) == true);
    assert(config.getOr<double>("learning_rate", 0.0) == 0.03);
    
    // Test array value
    auto arrayVal = config.get<std::vector<double>>("array_values");
    assert(arrayVal.has_value());
    assert(arrayVal.value().size() == 5);
    assert(arrayVal.value()[0] == 1.0);
    assert(arrayVal.value()[4] == 5.0);
    
    std::cout << "✓ Config JSON Support test passed" << std::endl;
}

void testConfigSourceTracking() {
    std::cout << "Testing Config Source Tracking..." << std::endl;
    
    nlm::Config config;
    
    // Set values from different sources
    config.set("default_value", 1, nlm::ConfigSource::Default);
    config.set("file_value", 2, nlm::ConfigSource::File);
    config.set("cmdline_value", 3, nlm::ConfigSource::CommandLine);
    config.set("runtime_value", 4, nlm::ConfigSource::Runtime);
    
    // Verify all values are set
    assert(config.getOr<int64_t>("default_value", 0) == 1);
    assert(config.getOr<int64_t>("file_value", 0) == 2);
    assert(config.getOr<int64_t>("cmdline_value", 0) == 3);
    assert(config.getOr<int64_t>("runtime_value", 0) == 4);
    
    std::cout << "✓ Config Source Tracking test passed" << std::endl;
}

void testConfigSummary() {
    std::cout << "Testing Config Summary..." << std::endl;
    
    nlm::Config config;
    
    config.set("setting1", 42);
    config.set("setting2", "hello");
    config.set("setting3", true);
    
    std::string summary = config.summary();
    assert(summary.find("setting1") != std::string::npos);
    assert(summary.find("setting2") != std::string::npos);
    assert(summary.find("setting3") != std::string::npos);
    
    std::cout << "✓ Config Summary test passed" << std::endl;
}

void runAllTests() {
    std::cout << "=== Enhanced Config Tests ===" << std::endl;
    
    try {
        testBasicConfig();
        testConfigFileIO();
        testConfigCommandLine();
        testConfigJSONSupport();
        testConfigSourceTracking();
        testConfigSummary();
        
        std::cout << std::endl;
        std::cout << "=== All Enhanced Config Tests PASSED ===" << std::endl;
        return;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Enhanced Config Tests FAILED ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_config_enhanced

int main() {
    test_config_enhanced::runAllTests();
    return 0;
}