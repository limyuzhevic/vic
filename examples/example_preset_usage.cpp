// NLM Configuration Examples
// Demonstrates the enhanced configuration system with preset profiles and file format support

#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    nlm::Config config;
    
    // List available presets
    auto presets = nlm::Config::getAvailablePresets();
    std::cout << "Available presets: ";
    for (const auto& preset : presets) {
        std::cout << preset << " ";
    }
    std::cout << std::endl;
    
    // Apply and use different presets
    config.applyPreset("basic");
    std::cout << "Basic config neuron_count: " 
              << config.getOr<int>("neuron_count", 0) << std::endl;
    
    config.applyPreset("advanced");
    std::cout << "Advanced config neuron_count: " 
              << config.getOr<int>("neuron_count", 0) << std::endl;
    
    config.applyPreset("research");
    std::cout << "Research config neuron_count: " 
              << config.getOr<int>("neuron_count", 0) << std::endl;
    
    return 0;
}
