// NLM Configuration Examples
// Demonstrates template inheritance capabilities

#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    // Create a base template
    nlm::ConfigTemplate baseTemplate;
    baseTemplate.name = "base_template";
    baseTemplate.inheritFromDefault = true;
    baseTemplate.baseEntries = {
        {"random_seed", 42, nlm::ConfigSource::Default, "Random seed"},
        {"simulation_timestep", 0.001, nlm::ConfigSource::Default, "Simulation timestep"}
    };
    
    // Create a derived configuration
    nlm::Config derivedConfig;
    derivedConfig.loadFromTemplate(baseTemplate);
    
    // Add specialized entries
    derivedConfig.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    derivedConfig.set("region_count", 4, nlm::ConfigSource::Runtime);
    
    // Validate and display
    if (derivedConfig.isValid()) {
        std::cout << "Derived configuration is valid:" << std::endl;
        std::cout << derivedConfig.summary() << std::endl;
    }
    
    return 0;
}
