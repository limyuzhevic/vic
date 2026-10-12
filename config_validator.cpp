#!/usr/bin/env g++ -std=c++20 -O2 -I. -I/usr/local/cuda/include -lpthread -std=c++20

// Simple configuration validation and diagnostics tool
// Usage:
//   config_validator <config_file> [key=value ...]
//   config_validator --validate-only
//   config_validator --diagnose-config

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <thread>
#include "src/core/Config/Config.hpp"

using namespace nlm;

void printUsage() {
    std::cout << "Configuration Validation and Diagnostics Tool" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  config_validator <config_file> [key=value ...]" << std::endl;
    std::cout << "  config_validator --validate-only" << std::endl;
    std::cout << "  config_validator --diagnose-config" << std::endl;
    std::cout << "  config_validator --help" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  <config_file>     Path to configuration file to validate" << std::endl;
    std::cout << "  key=value          Command-line configuration override" << std::endl;
    std::cout << "  --validate-only    Validate configuration and exit" << std::endl;
    std::cout << "  --diagnose-config  Show configuration diagnostics" << std::endl;
    std::cout << "  --help             Show this help message" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  config_validator configs/default.cfg" << std::endl;
    std::cout << "  config_validator configs/default.cfg --neuron_count=5000" << std::endl;
    std::cout << "  config_validator --validate-only" << std::endl;
    std::cout << "" << std::endl;
}

void validateConfigFile(const std::string& filepath) {
    std::cout << "Validating configuration file: " << filepath << std::endl;
    std::cout << "=============================================" << std::endl;
    
    Config config;
    
    if (!config.loadFromFile(filepath)) {
        std::cout << "ERROR: Failed to load configuration file: " << filepath << std::endl;
        return;
    }
    
    std::cout << "✓ Configuration file loaded successfully" << std::endl;
    
    // Validate schema
    if (config.validateSchema()) {
        std::cout << "✓ Configuration schema validation passed" << std::endl;
    } else {
        std::cout << "✗ Configuration schema validation failed" << std::endl;
        std::cout << "  Missing required keys or invalid values" << std::endl;
    }
    
    // Show configuration summary
    std::cout << "" << std::endl;
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Show diagnostics
    std::cout << "" << std::endl;
    std::cout << "Diagnostics:" << std::endl;
    std::cout << "===========" << std::endl;
    std::cout << config.getDiagnostics() << std::endl;
}

void validateCommandLine(int argc, char** argv) {
    std::cout << "Validating command-line configuration..." << std::endl;
    std::cout << "===========================================" << std::endl;
    
    Config config;
    
    // Parse command line arguments
    if (!config.loadFromArgs(argc, argv)) {
        std::cout << "ERROR: Failed to parse command-line arguments" << std::endl;
        return;
    }
    
    std::cout << "✓ Command-line arguments parsed successfully" << std::endl;
    
    // Validate schema
    if (config.validateSchema()) {
        std::cout << "✓ Command-line configuration schema validation passed" << std::endl;
    } else {
        std::cout << "✗ Command-line configuration schema validation failed" << std::endl;
        std::cout << "  Missing required keys or invalid values" << std::endl;
    }
    
    // Show configuration summary
    std::cout << "" << std::endl;
    std::cout << "Command-Line Configuration Summary:" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << config.summary() << std::endl;
}

void diagnoseConfig() {
    std::cout << "Configuration Diagnostics" << std::endl;
    std::cout << "==========================" << std::endl;
    
    Config config;
    
    // Load default configuration
    std::string defaultConfigPath = "configs/default.cfg";
    if (config.loadFromFile(defaultConfigPath)) {
        std::cout << "✓ Default configuration loaded from: " << defaultConfigPath << std::endl;
    } else {
        std::cout << "✗ Failed to load default configuration" << std::endl;
    }
    
    // Show diagnostics
    std::cout << "" << std::endl;
    std::cout << "System Configuration Diagnostics:" << std::endl;
    std::cout << "===============================" << std::endl;
    std::cout << config.getDiagnostics() << std::endl;
    
    // Performance recommendations
    std::cout << "" << std::endl;
    std::cout << "Performance Recommendations:" << std::endl;
    std::cout << "===========================" << std::endl;
    
    auto neuronCount = config.get<int64_t>("neuron_count");
    auto connectionProb = config.get<double>("connection_probability");
    auto maxSteps = config.get<int64_t>("max_simulation_steps");
    
    if (neuronCount && *neuronCount > 10000) {
        std::cout << "  Large neuron count (" << *neuronCount << ") - consider enabling memory pools" << std::endl;
    }
    
    if (connectionProb && *connectionProb > 0.5) {
        std::cout << "  High connection probability - consider SIMD optimization" << std::endl;
    }
    
    if (maxSteps && *maxSteps > 10000) {
        std::cout << "  Large simulation steps - consider parallel processing" << std::endl;
    }
    
    // Advanced options
    std::cout << "" << std::endl;
    std::cout << "Available Advanced Options:" << std::endl;
    std::cout << "==========================" << std::endl;
    std::cout << "  --enable_simd          Enable SIMD optimization" << std::endl;
    std::cout << "  --max_threads=<n>      Set maximum thread count" << std::endl;
    std::cout << "  --enable_profiling     Enable performance profiling" << std::endl;
    std::cout << "  --enable_debug_mode    Enable debug mode" << std::endl;
    std::cout << "  --enable_structural_plasticity Enable structural plasticity" << std::endl;
    std::cout << "  --enable_prediction_system     Enable prediction system" << std::endl;
    std::cout << "  --enable_working_memory       Enable working memory" << std::endl;
}

int main(int argc, char** argv) {
    std::cout << "NLM Configuration Validation Tool" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "Build date: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "" << std::endl;
    
    if (argc == 1) {
        printUsage();
        return 0;
    }
    
    std::string arg1 = argv[1];
    
    if (arg1 == "--help" || arg1 == "-h") {
        printUsage();
        return 0;
    } else if (arg1 == "--validate-only") {
        validateConfigFile("configs/default.cfg");
        return 0;
    } else if (arg1 == "--diagnose-config") {
        diagnoseConfig();
        return 0;
    } else if (arg1 == "--version") {
        std::cout << "NLM Configuration Tool v1.0.0" << std::endl;
        return 0;
    }
    
    // Check if first argument is a file that exists
    std::ifstream testFile(arg1);
    if (testFile.good()) {
        testFile.close();
        // It's a file, validate config file
        if (argc > 2) {
            // Parse additional key=value pairs
            validateCommandLine(argc, argv);
        } else {
            validateConfigFile(arg1);
        }
    } else {
        // It's not a file, assume it's a key=value pair or command-line config
        validateCommandLine(argc, argv);
    }
    
    return 0;
}