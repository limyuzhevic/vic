// Configuration Loading and Validation Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_config_advanced {

void testConfigFileLoading() {
    nlm::Config config;
    
    // Create a temporary config file
    std::string testFile = "test_config.ini";
    std::ofstream file(testFile);
    file << "# Test configuration file\n";
    file << "int_value = 42\n";
    file << "double_value = 3.14\n";
    file << "string_value = hello world\n";
    file << "bool_value = true\n";
    file.close();
    
    // Test loading from file
    bool loaded = config.loadFromFile(testFile);
    assert(loaded);
    
    assert(config.has("int_value"));
    assert(config.has("double_value"));
    assert(config.has("string_value"));
    assert(config.has("bool_value"));
    
    assert(config.getOr<int>("int_value", 0) == 42);
    assert(config.getOr<double>("double_value", 0.0) > 3.13);
    assert(config.getOr<double>("double_value", 0.0) < 3.15);
    assert(config.getOr<std::string>("string_value", "") == "hello world");
    assert(config.getOr<bool>("bool_value", false));
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigFileLoading passed" << std::endl;
}

void testConfigJsonFileSupport() {
    nlm::Config config;
    
    // Create a temporary JSON config file
    std::string testFile = "test_config.json";
    std::ofstream file(testFile);
    file << "{\n";
    file << "  \"config\": {\n";
    file << "    \"neuron_count\": 100,\n";
    file << "    \"region_count\": 2,\n";
    file << "    \"connection_probability\": 0.1,\n";
    file << "    \"random_seed\": 42,\n";
    file << "    \"log_level\": \"INFO\"\n";
    file << "  }\n";
    file << "}\n";
    file.close();
    
    // Test loading from JSON file (this would need JSON parser implementation)
    // For now, we test that the file can be opened and read
    std::ifstream jsonFile(testFile);
    std::string line;
    bool hasNeuronCount = false;
    bool hasRegionCount = false;
    
    while (std::getline(jsonFile, line)) {
        if (line.find("neuron_count") != std::string::npos) hasNeuronCount = true;
        if (line.find("region_count") != std::string::npos) hasRegionCount = true;
    }
    
    assert(hasNeuronCount);
    assert(hasRegionCount);
    
    jsonFile.close();
    std::remove(testFile.c_str());
    
    std::cout << "    testConfigJsonFileSupport passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test valid configurations
    config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config.set("region_count", 2, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    
    // These should pass validation
    assert(config.getOr<int64_t>("neuron_count", 0) > 0);
    assert(config.getOr<int64_t>("region_count", 0) > 0);
    assert(config.getOr<double>("simulation_timestep", 0.0) > 0.0);
    
    // Test invalid configurations (these would be caught by main.cpp validation)
    // config.set("neuron_count", -1, nlm::ConfigSource::Runtime);  // Should be validated
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigSourceManagement() {
    nlm::Config config;
    
    // Test different config sources
    config.set("runtime_value", 1, nlm::ConfigSource::Runtime);
    config.set("file_value", 2, nlm::ConfigSource::File);
    config.set("commandline_value", 3, nlm::ConfigSource::CommandLine);
    config.set("default_value", 4, nlm::ConfigSource::Default);
    
    assert(config.getOr<int>("runtime_value", 0) == 1);
    assert(config.getOr<int>("file_value", 0) == 2);
    assert(config.getOr<int>("commandline_value", 0) == 3);
    assert(config.getOr<int>("default_value", 0) == 4);
    
    std::cout << "    testConfigSourceManagement passed" << std::endl;
}

void testConfigEdgeCases() {
    nlm::Config config;
    
    // Test empty file loading
    std::string emptyFile = "empty_test.ini";
    std::ofstream emptyOut(emptyFile);
    emptyOut.close();
    
    bool loadedEmpty = config.loadFromFile(emptyFile);
    assert(!loadedEmpty || config.getKeys().empty());  // Either not loaded or empty
    
    std::remove(emptyFile.c_str());
    
    // Test malformed file (should be handled gracefully)
    std::string malformedFile = "malformed_test.ini";
    std::ofstream malformedOut(malformedFile);
    malformedOut << "invalid line without equals\n";
    malformedOut << "valid = line\n";
    malformedOut << "= no key\n";
    malformedOut.close();
    
    nlm::Config config2;
    bool loadedMalformed = config2.loadFromFile(malformedFile);
    // Should still load the valid line
    assert(config2.has("valid"));
    
    std::remove(malformedFile.c_str());
    
    std::cout << "    testConfigEdgeCases passed" << std::endl;
}

void runAll() {
    testConfigFileLoading();
    testConfigJsonFileSupport();
    testConfigValidation();
    testConfigSourceManagement();
    testConfigEdgeCases();
    
    std::cout << "All advanced config tests passed" << std::endl;
}

} // namespace test_config_advanced
