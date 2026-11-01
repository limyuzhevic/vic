#!/usr/bin/env g++ -std=c++20
//============================================================================
// Demo: Improved Configuration System for NLM
//============================================================================

#include "src/core/Config/Config.hpp"
#include <iostream>
#include <fstream>

void demo_config_validation() {
    std::cout << "=== NLM Configuration System Demo ===" << std::endl;
    std::cout << std::endl;
    
    // Create a configuration
    nlm::Config config;
    
    // Set some configuration values
    config.set("neuron_count", 1000LL, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1f, nlm::ConfigSource::Runtime);
    config.set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Runtime);
    config.set("stdp_ltd_weight", 0.015f, nlm::ConfigSource::Runtime);
    
    // Show configuration summary
    std::cout << "Current Configuration:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Demonstrate validation
    std::cout << "=== Configuration Validation ===" << std::endl;
    auto errors = config.validate();
    if (errors.empty()) {
        std::cout << "✓ Configuration is valid" << std::endl;
    } else {
        std::cout << "✗ Configuration errors found:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  - " << error.key << ": " << error.message << std::endl;
        }
    }
    
    // Demonstrate auto-fix capability
    std::cout << "\n=== Auto-Fix Demonstration ===" << std::endl;
    nlm::Config config2;
    config2.set("neuron_count", -1000LL, nlm::ConfigSource::File);  // Invalid value
    config2.set("simulation_timestep", 0.0, nlm::ConfigSource::File);  // Invalid value
    
    std::cout << "Config before fix:" << std::endl;
    std::cout << config2.summary() << std::endl;
    
    bool wasFixed = config2.validateAndFix();
    std::cout << "Auto-fix " << (wasFixed ? "applied" : "not needed") << std::endl;
    
    std::cout << "Config after fix:" << std::endl;
    std::cout << config2.summary() << std::endl;
}

void demo_config_file_io() {
    std::cout << "\n=== Configuration File I/O Demo ===" << std::endl;
    
    // Create a config file
    std::ofstream configFile("demo_config.cfg");
    if (configFile.is_open()) {
        configFile << "# Demo NLM Configuration" << std::endl;
        configFile << "neuron_count = 500" << std::endl;
        configFile << "simulation_timestep = 0.001" << std::endl;
        configFile << "connection_probability = 0.15" << std::endl;
        configFile << "stdp_ltp_weight = 0.02" << std::endl;
        configFile << "stdp_ltd_weight = 0.015" << std::endl;
        configFile.close();
        
        std::cout << "Created demo_config.cfg" << std::endl;
    }
    
    // Load the config file
    nlm::Config config;
    bool loaded = config.loadFromFile("demo_config.cfg");
    
    if (loaded) {
        std::cout << "Loaded configuration:" << std::endl;
        std::cout << config.summary() << std::endl;
        
        // Validate loaded config
        auto errors = config.validate();
        if (errors.empty()) {
            std::cout << "✓ Loaded configuration is valid" << std::endl;
        } else {
            std::cout << "✗ Loaded configuration has errors:" << std::endl;
            for (const auto& error : errors) {
                std::cout << "  - " << error.key << ": " << error.message << std::endl;
            }
        }
    } else {
        std::cout << "Failed to load configuration file" << std::endl;
    }
}

void demo_cli_like_usage() {
    std::cout << "\n=== CLI-like Usage Demo ===" << std::endl;
    
    nlm::Config config;
    
    // Simulate command line arguments
    const char* argv[] = {"demo", "--neuron_count=2000", "--simulation_timestep=0.005", 
                         "--connection_probability=0.2", "-stdp_ltp_weight=0.03"};
    int argc = 5;
    
    config.loadFromArgs(argc, const_cast<char**>(argv));
    
    std::cout << "Configuration from CLI-like arguments:" << std::endl;
    std::cout << config.summary() << std::endl;
}

int main() {
    try {
        demo_config_validation();
        demo_config_file_io();
        demo_cli_like_usage();
        
        std::cout << "\n=== Demo Complete ===" << std::endl;
        std::cout << "The improved configuration system provides:" << std::endl;
        std::cout << "  ✓ Better validation with descriptive error messages" << std::endl;
        std::cout << "  ✓ Auto-fix capability for common configuration errors" << std::endl;
        std::cout << "  ✓ Enhanced file I/O with JSON support (planned)" << std::endl;
        std::cout << "  ✓ Better CLI argument parsing and help system" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}