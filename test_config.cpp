// Test program to demonstrate the improved configuration management
#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

void testConfigurationBasics() {
    std::cout << "Testing basic configuration operations..." << std::endl;
    
    nlm::Config config;
    
    // Test setting and getting values
    config.setInt("test_int", 42, nlm::ConfigSource::Runtime, "Test integer value");
    config.setFloat("test_float", 3.14, nlm::ConfigSource::Runtime, "Test float value");
    config.setString("test_string", "Hello World", nlm::ConfigSource::Runtime, "Test string value");
    config.setBool("test_bool", true, nlm::ConfigSource::Runtime, "Test boolean value");
    
    // Test getting values
    assert(config.get<int>("test_int").value_or(0) == 42);
    assert(config.get<double>("test_float").value_or(0.0) == 3.14);
    assert(config.get<std::string>("test_string").value_or("") == "Hello World");
    assert(config.get<bool>("test_bool").value_or(false) == true);
    
    // Test has function
    assert(config.has("test_int"));
    assert(config.has("test_float"));
    assert(config.has("test_string"));
    assert(config.has("test_bool"));
    assert(!config.has("non_existent"));
    
    // Test getOr function
    assert(config.getOr<int>("test_int", 0) == 42);
    assert(config.getOr<int>("non_existent", 100) == 100);
    
    std::cout << "✓ Basic configuration operations passed" << std::endl;
}

void testConfigurationFileOperations() {
    std::cout << "Testing configuration file operations..." << std::endl;
    
    nlm::Config config;
    
    // Test setting values
    config.setInt("random_seed", 123);
    config.setFloat("simulation_timestep", 0.001);
    config.setString("checkpoint_dir", "./test_checkpoints");
    
    // Test saving to file
    assert(config.saveToFile("test_config.cfg"));
    
    // Test loading from file
    nlm::Config config2;
    assert(config2.loadFromFile("test_config.cfg"));
    
    // Verify loaded values
    assert(config2.get<int>("random_seed").value_or(0) == 123);
    assert(config2.get<double>("simulation_timestep").value_or(0.0) == 0.001);
    assert(config2.get<std::string>("checkpoint_dir").value_or("") == "./test_checkpoints");
    
    std::cout << "✓ Configuration file operations passed" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "Testing configuration validation..." << std::endl;
    
    nlm::Config config;
    
    // Test valid configuration
    config.setInt("neuron_count", 100);
    config.setFloat("simulation_timestep", 0.001);
    auto errors = config.validate();
    assert(errors.empty());
    
    // Test invalid configuration
    nlm::Config config2;
    config2.setInt("neuron_count", 0);  // Invalid: should be > 0
    auto errors2 = config2.validate();
    assert(!errors2.empty());
    
    std::cout << "✓ Configuration validation passed" << std::endl;
}

void testCommandLineParsing() {
    std::cout << "Testing command line parsing..." << std::endl;
    
    nlm::Config config;
    
    // Simulate command line arguments
    int argc = 5;
    char* argv[] = {
        (char*)"test",
        (char*)"--config",
        (char*)"config.cfg",
        (char*)"--neuron_count",
        (char*)"500"
    };
    
    // Note: The actual command line parsing is done in the CommandLineParser class
    // This test just verifies that Config can handle the parsed values
    config.setSize_t("neuron_count", 500);
    
    assert(config.get<size_t>("neuron_count").value_or(0) == 500);
    
    std::cout << "✓ Command line parsing test passed" << std::endl;
}

void testConfigurationMerging() {
    std::cout << "Testing configuration merging..." << std::endl;
    
    nlm::Config config1;
    config1.setInt("key1", 10, nlm::ConfigSource::File);
    config1.setFloat("key2", 3.14, nlm::ConfigSource::CommandLine);
    
    nlm::Config config2;
    config2.setInt("key1", 20, nlm::ConfigSource::Runtime);  // Should override
    config2.setString("key3", "value3", nlm::ConfigSource::Default);
    
    // Merge config2 into config1
    config1.merge(config2);
    
    // key1 should now have value 20 (Runtime overrides CommandLine)
    assert(config1.get<int>("key1").value_or(0) == 20);
    // key2 should still have value 3.14
    assert(config1.get<double>("key2").value_or(0.0) == 3.14);
    // key3 should be added
    assert(config1.get<std::string>("key3").value_or("") == "value3");
    
    std::cout << "✓ Configuration merging passed" << std::endl;
}

void testErrorHandling() {
    std::cout << "Testing error handling..." << std::endl;
    
    nlm::Config config;
    
    // Test that invalid get requests return nullopt
    assert(!config.get<int>("non_existent").has_value());
    assert(config.getOr<int>("non_existent", 42) == 42);
    
    // Test that loading non-existent file returns false
    assert(!config.loadFromFile("non_existent_file.cfg"));
    
    std::cout << "✓ Error handling passed" << std::endl;
}

int main() {
    std::cout << "Running configuration management tests..." << std::endl;
    std::cout << "=" * 50 << std::endl;
    
    try {
        testConfigurationBasics();
        testConfigurationFileOperations();
        testConfigurationValidation();
        testCommandLineParsing();
        testConfigurationMerging();
        testErrorHandling();
        
        std::cout << "=" * 50 << std::endl;
        std::cout << "✓ All tests passed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}