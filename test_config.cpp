// Test program to demonstrate the enhanced configuration system
#include "core/Config/Config.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "=== NLM Configuration System Test ===\n\n";
    
    // Create configuration
    nlm::Config config;
    
    // Load from test configuration file
    std::cout << "Loading configuration from config_test.toml...\n";
    bool loaded = config.loadFromFile("config_test.toml");
    
    if (!loaded) {
        std::cout << "Failed to load configuration!\n";
        return 1;
    }
    
    // Display configuration summary
    std::cout << config.summary() << std::endl;
    
    // Test getting values
    std::cout << "\n=== Testing Configuration Access ===\n";
    
    // Test basic types
    std::cout << "neuron_count = " << config.getOr<int>("neuron_count", 1000) << std::endl;
    std::cout << "simulation_timestep = " << config.getOr<double>("simulation_timestep", 0.001) << std::endl;
    std::cout << "enable_event_driven = " << (config.getOr<bool>("enable_event_driven", false) ? "true" : "false") << std::endl;
    std::cout << "enable_multithreading = " << (config.getOr<bool>("enable_multithreading", false) ? "true" : "false") << std::endl;
    std::cout << "enable_simd = " << (config.getOr<bool>("enable_simd", false) ? "true" : "false") << std::endl;
    
    // Test string values
    std::cout << "checkpoint_dir = " << config.getOr<std::string>("checkpoint_dir", "./") << std::endl;
    
    // Test has method
    std::cout << "\n=== Testing Configuration Existence ===\n";
    std::cout << "Has 'neuron_count': " << (config.has("neuron_count") ? "yes" : "no") << std::endl;
    std::cout << "Has 'nonexistent_key': " << (config.has("nonexistent_key") ? "yes" : "no") << std::endl;
    
    // Test setting values
    std::cout << "\n=== Testing Configuration Setting ===\n";
    config.set("custom_test_int", 42);
    config.set("custom_test_string", "test_value");
    config.set("custom_test_bool", true);
    
    std::cout << "custom_test_int = " << config.getOr<int>("custom_test_int", 0) << std::endl;
    std::cout << "custom_test_string = " << config.getOr<std::string>("custom_test_string", "") << std::endl;
    std::cout << "custom_test_bool = " << (config.getOr<bool>("custom_test_bool", false) ? "true" : "false") << std::endl;
    
    // Test removing values
    std::cout << "\n=== Testing Configuration Removal ===\n";
    config.remove("custom_test_int");
    std::cout << "Has 'custom_test_int' after removal: " << (config.has("custom_test_int") ? "yes" : "no") << std::endl;
    
    // Test command line loading simulation
    std::cout << "\n=== Testing Command Line Loading ===\n";
    nlm::Config config2;
    // Simulate command line arguments
    const char* argv[] = {
        "test_program",
        "--neuron_count=256",
        "--enable_event_driven=true",
        "--checkpoint_dir=./cmd_checkpoints",
        "-simulation_timestep=0.01"
    };
    
    config2.loadFromArgs(5, const_cast<char**>(argv));
    
    std::cout << "neuron_count from command line = " << config2.getOr<int>("neuron_count", 0) << std::endl;
    std::cout << "enable_event_driven from command line = " << (config2.getOr<bool>("enable_event_driven", false) ? "true" : "false") << std::endl;
    std::cout << "checkpoint_dir from command line = " << config2.getOr<std::string>("checkpoint_dir", "./") << std::endl;
    std::cout << "simulation_timestep from command line = " << config2.getOr<double>("simulation_timestep", 0.001) << std::endl;
    
    // Test configuration modification
    std::cout << "\n=== Testing Configuration Modification ===\n";
    config.set("neuron_count", 777, nlm::ConfigSource::Runtime);
    std::cout << "neuron_count after runtime modification = " << config.getOr<int>("neuron_count", 0) << std::endl;
    
    // Test saving configuration
    std::cout << "\n=== Testing Configuration Saving ===\n";
    bool saved = config.saveToFile("config_output.toml");
    if (saved) {
        std::cout << "Configuration saved to config_output.toml\n";
    } else {
        std::cout << "Failed to save configuration!\n";
    }
    
    std::cout << "\n=== Configuration System Test Completed Successfully ===\n";
    return 0;
}
