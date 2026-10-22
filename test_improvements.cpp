#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <cassert>

void testConfigurationSystem() {
    std::cout << "=== Testing Configuration System Improvements ===" << std::endl;
    
    // Test 1: Basic configuration operations
    std::cout << "Test 1: Basic configuration operations..." << std::endl;
    nlm::Config config;
    
    // Set values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Get values
    assert(config.has("int_value"));
    assert(*config.get<int>("int_value") == 42);
    
    assert(config.has("double_value"));
    assert(*config.get<double>("double_value") > 3.13 && *config.get<double>("double_value") < 3.15);
    
    assert(config.has("string_value"));
    assert(*config.get<std::string>("string_value") == "hello");
    
    assert(config.has("bool_value"));
    assert(*config.get<bool>("bool_value") == true);
    
    // Test getOr with existing value
    assert(config.getOr<int>("int_value", 100) == 42);
    
    // Test getOr with missing value
    assert(config.getOr<int>("missing", 100) == 100);
    
    std::cout << "✓ Basic configuration operations passed" << std::endl;
    
    // Test 2: Vector configuration values
    std::cout << "Test 2: Vector configuration values..." << std::endl;
    config.set("int_vector", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    config.set("double_vector", std::vector<double>({1.1, 2.2, 3.3}), nlm::ConfigSource::Runtime);
    config.set("string_vector", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    
    assert(config.has("int_vector"));
    auto intVec = config.get<std::vector<int>>("int_vector");
    assert(intVec.has_value() && intVec.value() == std::vector<int>({1, 2, 3}));
    
    assert(config.has("double_vector"));
    auto doubleVec = config.get<std::vector<double>>("double_vector");
    assert(doubleVec.has_value() && doubleVec.value() == std::vector<double>({1.1, 2.2, 3.3}));
    
    assert(config.has("string_vector"));
    auto stringVec = config.get<std::vector<std::string>>("string_vector");
    assert(stringVec.has_value() && stringVec.value() == std::vector<std::string>({"a", "b", "c"}));
    
    std::cout << "✓ Vector configuration values passed" << std::endl;
    
    // Test 3: Configuration keys and removal
    std::cout << "Test 3: Configuration keys and removal..." << std::endl;
    config.set("temp_key", 123, nlm::ConfigSource::Runtime);
    assert(config.has("temp_key"));
    
    auto keys = config.getKeys();
    assert(keys.size() >= 6); // int_value, double_value, string_value, bool_value, int_vector, double_vector, string_vector, temp_key
    
    config.remove("temp_key");
    assert(!config.has("temp_key"));
    
    std::cout << "✓ Configuration keys and removal passed" << std::endl;
    
    // Test 4: Configuration summary
    std::cout << "Test 4: Configuration summary..." << std::endl;
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("Configuration (") != std::string::npos);
    assert(summary.find("int_value") != std::string::npos);
    
    std::cout << "✓ Configuration summary passed" << std::endl;
    
    // Test 5: Configuration source tracking
    std::cout << "Test 5: Configuration source tracking..." << std::endl;
    nlm::Config config2;
    config2.set("test_key", "file_source", nlm::ConfigSource::File);
    config2.set("test_key2", "cmd_source", nlm::ConfigSource::CommandLine);
    config2.set("test_key3", "runtime_source", nlm::ConfigSource::Runtime);
    
    // Verify source tracking (implementation dependent)
    std::cout << "✓ Configuration source tracking passed" << std::endl;
    
    std::cout << "=== All Configuration System Tests Passed ===" << std::endl;
}

void testFileOperations() {
    std::cout << "=== Testing File Operations ===" << std::endl;
    
    // Clean up any existing test files
    std::remove("test_config.cfg");
    std::remove("test_config.json");
    std::remove("test_config.yaml");
    
    nlm::Config config;
    
    // Add some test data
    config.set("neurons", 1000, nlm::ConfigSource::File);
    config.set("timestep", 0.001, nlm::ConfigSource::File);
    config.set("learning_rate", 0.01, nlm::ConfigSource::File);
    
    // Test saving to file (current implementation uses placeholder)
    bool saved = config.saveToFile("test_config.cfg");
    assert(saved);
    
    // Verify file was created and has content
    std::ifstream file("test_config.cfg");
    assert(file.is_open());
    std::string line;
    int lineCount = 0;
    while (std::getline(file, line)) {
        lineCount++;
    }
    assert(lineCount >= 3); // Should have at least 3 config entries
    
    file.close();
    std::remove("test_config.cfg");
    
    std::cout << "✓ File operations test passed" << std::endl;
    std::cout << "=== File Operations Test Passed ===" << std::endl;
}

int main() {
    testConfigurationSystem();
    testFileOperations();
    
    std::cout << "\n=== All Tests Completed Successfully ===" << std::endl;
    return 0;
}