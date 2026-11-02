#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "Testing JSON Config Loading..." << std::endl;
    
    nlm::Config config;
    
    // Test loading JSON file
    if (config.loadFromFile("configs/test.json")) {
        std::cout << "✓ Successfully loaded JSON config" << std::endl;
        
        // Test getting values
        std::cout << "\nTesting value retrieval:" << std::endl;
        
        auto seed = config.getOr<int64_t>("random_seed", 0);
        std::cout << "random_seed: " << seed << std::endl;
        
        auto timestep = config.getOr<double>("simulation_timestep", 0.0);
        std::cout << "simulation_timestep: " << timestep << std::endl;
        
        auto neurons = config.getOr<int64_t>("neuron_count", 0);
        std::cout << "neuron_count: " << neurons << std::endl;
        
        auto regions = config.getOr<int64_t>("region_count", 0);
        std::cout << "region_count: " << regions << std::endl;
        
        auto connProb = config.getOr<float>("connection_probability", 0.0f);
        std::cout << "connection_probability: " << connProb << std::endl;
        
        auto logLevel = config.getOr<std::string>("log_level", "");
        std::cout << "log_level: " << logLevel << std::endl;
        
        auto logToFile = config.getOr<bool>("log_to_file", false);
        std::cout << "log_to_file: " << (logToFile ? "true" : "false") << std::endl;
        
        // Test summary
        std::cout << "\nConfiguration summary:" << std::endl;
        std::cout << config.summary() << std::endl;
        
        // Test saving to file
        if (config.saveToFile("configs/test_output.json")) {
            std::cout << "✓ Successfully saved config to JSON" << std::endl;
        } else {
            std::cout << "✗ Failed to save config" << std::endl;
        }
        
        return 0;
        
    } else {
        std::cout << "✗ Failed to load JSON config" << std::endl;
        return 1;
    }
}