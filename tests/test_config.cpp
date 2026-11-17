// Enhanced Config Tests
// Phase 3: Comprehensive configuration testing

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
    
    // Test integer setting and getting
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    // Test double setting and getting
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    // Test string setting and getting
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    // Test bool setting and getting
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    // Test getting non-existent key with default
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    // Test getting existing key
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
    assert(std::find(keys.begin(), keys.end(), "key1") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key2") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key3") != keys.end());
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigFileSaving() {
    nlm::Config config;
    
    // Set some values
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_val", "test", nlm::ConfigSource::Runtime);
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    
    // Save to temporary file
    std::string testFile = "/tmp/test_config.json";
    bool saveSuccess = config.saveToFile(testFile);
    assert(saveSuccess);
    
    // Load back and verify
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(testFile);
    assert(loadSuccess);
    
    // Verify values were preserved
    assert(config2.has("int_val"));
    assert(config2.has("double_val"));
    assert(config2.has("string_val"));
    assert(config2.has("bool_val"));
    
    auto intVal2 = config2.get<int>("int_val");
    assert(intVal2.has_value() && *intVal2 == 42);
    
    std::cout << "    testConfigFileSaving passed" << std::endl;
    
    // Cleanup
    std::remove(testFile.c_str());
}

void testConfigSimpleFileFormat() {
    nlm::Config config;
    
    // Create a simple config file
    std::string testFile = "/tmp/test_simple.cfg";
    std::ofstream file(testFile);
    file << "# Test configuration file\n";
    file << "neuron_count = 1000\n";
    file << "region_count = 2\n";
    file << "connection_probability = 0.1\n";
    file << "stdp_ltp_weight = 0.01\n";
    file << "stdp_ltd_weight = 0.012\n";
    file << "stdp_tau = 20.0\n";
    file.close();
    
    // Load and verify
    bool loadSuccess = config.loadFromFile(testFile);
    assert(loadSuccess);
    
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    assert(config.has("connection_probability"));
    assert(config.has("stdp_ltp_weight"));
    assert(config.has("stdp_ltd_weight"));
    assert(config.has("stdp_tau"));
    
    auto neuronCount = config.getOr<int64_t>("neuron_count", 0);
    assert(neuronCount == 1000);
    
    auto regionCount = config.getOr<int64_t>("region_count", 0);
    assert(regionCount == 2);
    
    auto connectionProb = config.getOr<float>("connection_probability", 0.0f);
    assert(std::abs(connectionProb - 0.1f) < 0.001f);
    
    std::cout << "    testConfigSimpleFileFormat passed" << std::endl;
    
    // Cleanup
    std::remove(testFile.c_str());
}

void testConfigCommandLineParsing() {
    nlm::Config config;
    
    // Simulate command line arguments
    int argc = 5;
    char* argv[5];
    argv[0] = (char*)"test_program";
    argv[1] = (char*)"--neuron_count=2000";
    argv[2] = (char*)"--region_count=4";
    argv[3] = (char*)"--connection_probability=0.2";
    argv[4] = (char*)"--stdp_ltp_weight=0.05";
    
    config.loadFromArgs(argc, argv);
    
    // Verify values were set
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    assert(config.has("connection_probability"));
    assert(config.has("stdp_ltp_weight"));
    
    auto neuronCount = config.getOr<int64_t>("neuron_count", 0);
    assert(neuronCount == 2000);
    
    auto regionCount = config.getOr<int64_t>("region_count", 0);
    assert(regionCount == 4);
    
    auto connectionProb = config.getOr<float>("connection_probability", 0.0f);
    assert(std::abs(connectionProb - 0.2f) < 0.001f);
    
    auto stdpLtp = config.getOr<float>("stdp_ltp_weight", 0.0f);
    assert(std::abs(stdpLtp - 0.05f) < 0.001f);
    
    std::cout << "    testConfigCommandLineParsing passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test setting empty key
    config.set("", 123, nlm::ConfigSource::Runtime);
    // Should not crash and should not set empty key
    assert(!config.has(""));
    
    // Test setting invalid numeric values
    config.set("invalid_num", std::nan(""), nlm::ConfigSource::Runtime);
    // Should handle NaN gracefully
    
    config.set("valid_num", 42.0, nlm::ConfigSource::Runtime);
    assert(config.has("valid_num"));
    
    // Test string validation
    config.set("empty_string", "", nlm::ConfigSource::Runtime);
    // Should log warning for empty string
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("key1", 100, nlm::ConfigSource::Default);
    config.set("key2", "hello", nlm::ConfigSource::File);
    config.set("key3", 3.14, nlm::ConfigSource::CommandLine);
    
    std::string summary = config.summary();
    assert(summary.find("Configuration (3 entries)") != std::string::npos);
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    assert(summary.find("key3") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void runAll() {
    std::cout << "Running Config tests..." << std::endl;
    
    try {
        testConfigCreation();
        testConfigSetAndGet();
        testConfigGetOr();
        testConfigRemove();
        testConfigKeys();
        testConfigFileSaving();
        testConfigSimpleFileFormat();
        testConfigCommandLineParsing();
        testConfigValidation();
        testConfigSummary();
        
        std::cout << "  ALL CONFIG TESTS PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  CONFIG TESTS FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_config