#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    // Create configuration
    nlm::Config config;
    
    // Set some basic values
    config.set("random_seed", 42);
    config.set("neuron_count", 1000);
    config.set("simulation_timestep", 0.001);
    
    // Apply a preset
    config.applyPreset("advanced");
    
    // Override specific values
    config.set("neuron_count", 1500, nlm::ConfigSource::Runtime);
    
    // Validate configuration
    if (!config.isValid()) {
        std::cerr << "Configuration invalid:" << std::endl;
        auto errors = config.getValidationErrors();
        for (const auto& error : errors) {
            std::cerr << "  - " << error << std::endl;
        }
        return 1;
    }
    
    // Display configuration
    std::cout << config.summary() << std::endl;
    
    // Save configuration
    config.saveToFile("final_config.json", nlm::ConfigFileFormat::JSON);
    
    return 0;
}
