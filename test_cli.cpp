#!/usr/bin/env g++ -std=c++20 -O2 -I./src

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "src/core/Config/Config.hpp"

using namespace nlm;

int main() {
    std::cout << "=== NLM CLI Test ===" << std::endl;
    
    // Create config instance
    auto config = std::make_shared<Config>();
    
    // Test basic configuration
    config->set("test.int", 42, ConfigSource::Runtime);
    config->set("test.double", 3.14, ConfigSource::Runtime);
    config->set("test.string", "Hello NLM", ConfigSource::Runtime);
    config->set("test.bool", true, ConfigSource::Runtime);
    
    // Test getting values
    std::cout << "test.int = " << config->getOr<int64_t>("test.int", 0) << std::endl;
    std::cout << "test.double = " << config->getOr<double>("test.double", 0.0) << std::endl;
    std::cout << "test.string = " << config->getOr<std::string>("test.string", "") << std::endl;
    std::cout << "test.bool = " << (config->getOr<bool>("test.bool", false) ? "true" : "false") << std::endl;
    
    // Test configuration summary
    std::cout << "\nConfiguration Summary:" << std::endl;
    std::cout << config->summary() << std::endl;
    
    // Test validation
    std::cout << "\nValidation Tests:" << std::endl;
    
    // Test valid configuration
    bool valid = true;
    try {
        config->set("valid.int", 100, ConfigSource::Runtime);
        std::cout << "✓ Setting valid.int = 100 succeeded" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ Setting valid.int = 100 failed: " << e.what() << std::endl;
        valid = false;
    }
    
    // Test invalid configuration (neuron_count > max)
    try {
        config->set("neuron_count", static_cast<int64_t>(2000000), ConfigSource::Runtime);
        std::cout << "✗ Setting neuron_count = 2000000 should have failed" << std::endl;
        valid = false;
    } catch (const std::exception& e) {
        std::cout << "✓ Setting neuron_count = 2000000 correctly failed: " << e.what() << std::endl;
    }
    
    // Test invalid configuration (invalid key characters)
    try {
        config->set("invalid-key!", "value", ConfigSource::Runtime);
        std::cout << "✗ Setting 'invalid-key!' should have failed" << std::endl;
        valid = false;
    } catch (const std::exception& e) {
        std::cout << "✓ Setting 'invalid-key!' correctly failed: " << e.what() << std::endl;
    }
    
    // Test has() and getKeys()
    std::cout << "\nKey Management Tests:" << std::endl;
    std::cout << "Has 'test.int': " << (config->has("test.int") ? "yes" : "no") << std::endl;
    std::cout << "Has 'nonexistent': " << (config->has("nonexistent") ? "yes" : "no") << std::endl;
    
    std::vector<std::string> keys = config->getKeys();
    std::cout << "All keys:" << std::endl;
    for (const auto& key : keys) {
        std::cout << "  - " << key << std::endl;
    }
    
    // Test loadFromFile (create a test config file)
    std::cout << "\nFile I/O Tests:" << std::endl;
    
    // Create a test config file
    std::ofstream testConfig("test_config.cfg");
    testConfig << "# Test configuration file" << std::endl;
    testConfig << "file.int = 123" << std::endl;
    testConfig << "file.double = 2.718" << std::endl;
    testConfig << "file.string = from_file" << std::endl;
    testConfig << "file.bool = true" << std::endl;
    testConfig.close();
    
    if (config->loadFromFile("test_config.cfg")) {
        std::cout << "✓ loadFromFile succeeded" << std::endl;
        std::cout << "file.int from file = " << config->getOr<int64_t>("file.int", 0) << std::endl;
    } else {
        std::cout << "✗ loadFromFile failed: " << config->getLastError() << std::endl;
    }
    
    // Test saveToFile
    if (config->saveToFile("test_output.cfg")) {
        std::cout << "✓ saveToFile succeeded" << std::endl;
        
        // Clean up test files
        std::remove("test_config.cfg");
        std::remove("test_output.cfg");
    } else {
        std::cout << "✗ saveToFile failed" << std::endl;
    }
    
    // Test getLastError
    std::cout << "\nError Handling Tests:" << std::endl;
    config->clear();
    try {
        config->set("invalid_key_with_spaces!", "value", ConfigSource::Runtime);
    } catch (const std::exception& e) {
        std::cout << "✓ Error caught: " << e.what() << std::endl;
    }
    
    std::cout << "\n=== All Tests Completed ===" << std::endl;
    
    return 0;
}
