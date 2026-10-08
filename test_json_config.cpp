#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>

int main() {
    std::cout << "Testing Config JSON loading..." << std::endl;
    
    nlm::Config config;
    
    // Test loading from JSON file
    std::cout << "Loading from test_config.json..." << std::endl;
    bool success = config.loadFromFile("test_config.json");
    
    if (success) {
        std::cout << "✓ Successfully loaded JSON file!" << std::endl;
        
        std::cout << "\nChecking some values..." << std::endl;
        
        // Check various values
        auto neuronCount = config.getOr<size_t>("neuron_count", 0);
        std::cout << "  neuron_count: " << neuronCount << " (expected: 1000)" << std::endl;
        
        auto regionCount = config.getOr<size_t>("region_count", 0);
        std::cout << "  region_count: " << regionCount << " (expected: 2)" << std::endl;
        
        auto connectionProb = config.getOr<double>("connection_probability", -1.0);
        std::cout << "  connection_probability: " << connectionProb << " (expected: 0.1)" << std::endl;
        
        auto rewardEnabled = config.getOr<bool>("reward_modulation_enabled", false);
        std::cout << "  reward_modulation_enabled: " << rewardEnabled << " (expected: true)" << std::endl;
        
        auto complexParams = config.get<std::string>("complex_parameters");
        if (complexParams.has_value()) {
            std::cout << "  complex_parameters found: " << complexParams.value().substr(0, 50) << "..." << std::endl;
        }
        
        std::cout << "\nConfiguration summary:" << std::endl;
        std::cout << config.summary() << std::endl;
        
        // Test getting a vector from JSON
        auto learningRates = config.get<std::vector<double>>("complex_parameters.learning_rates");
        if (learningRates.has_value()) {
            std::cout << "✓ Successfully loaded vector from JSON: learning_rates = [";
            for (size_t i = 0; i < learningRates.value().size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << learningRates.value()[i];
            }
            std::cout << "]" << std::endl;
        }
        
        std::cout << "\n✓ All JSON parsing tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "✗ Failed to load JSON file!" << std::endl;
        std::cout << "This might be due to missing nlohmann/json dependency." << std::endl;
        std::cout << "Make sure nlohmann/json is available in your system." << std::endl;
        return 1;
    }
}
