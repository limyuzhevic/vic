/*
 * Example 1: Basic Configuration Loading
 * Demonstrates basic configuration loading and validation in NLM framework.
 */

#include <nlm.h>
#include <iostream>
#include <fstream>

int main() {
    try {
        std::cout << "Loading NLM configuration..." << std::endl;
        
        // Create configuration
        nlm::Config config;
        
        // Load from file
        if (!config.loadFromFile("configs/default.cfg")) {
            std::cerr << "Failed to load configuration from configs/default.cfg" << std::endl;
            return 1;
        }
        
        std::cout << "Configuration loaded successfully!" << std::endl;
        
        // Validate configuration
        if (!config.validate()) {
            std::cerr << "Configuration validation failed!" << std::endl;
            std::cout << "Configuration summary:" << std::endl;
            std::cout << config.summary();
            return 1;
        }
        
        std::cout << "Configuration validated successfully!" << std::endl;
        
        // Access configuration values
        std::cout << "\n=== Configuration Values ===" << std::endl;
        
        // Required parameters
        double timestep = config.getDouble("simulation_timestep");
        int neuronCount = config.getInt("neuron_count");
        int regionCount = config.getInt("region_count");
        double connectionProb = config.getDouble("connection_probability");
        
        std::cout << "Simulation timestep: " << timestep << " s" << std::endl;
        std::cout << "Neuron count: " << neuronCount << std::endl;
        std::cout << "Region count: " << regionCount << std::endl;
        std::cout << "Connection probability: " << connectionProb << std::endl;
        
        // Optional parameters with defaults
        int randomSeed = config.getOr("random_seed", 42);
        std::string envName = config.getOr("environment_name", "Unknown");
        bool logToFile = config.getOr("log_to_file", false);
        
        std::cout << "Random seed: " << randomSeed << std::endl;
        std::cout << "Environment name: " << envName << std::endl;
        std::cout << "Log to file: " << (logToFile ? "Yes" : "No") << std::endl;
        
        // Save configuration
        config.saveToFile("output_basic_config.cfg");
        std::cout << "\nConfiguration saved to output_basic_config.cfg" << std::endl;
        
        // Get detailed configuration report
        std::cout << "\n=== Configuration Report ===" << std::endl;
        std::cout << config.getConfigurationReport();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}