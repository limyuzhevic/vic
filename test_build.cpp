#include <iostream>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    std::cout << "=== NLM Build Verification ===" << std::endl;
    std::cout << "Testing Phase 6 integrated brain..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    
    // Create brain
    nlm::Brain brain(config);
    
    if (!brain.initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    std::cout << "Neuron count: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Synapse count: " << brain.getTotalSynapseCount() << std::endl;
    
    // Test basic functionality
    for (int i = 0; i < 10; ++i) {
        brain.step(i);
    }
    
    std::cout << "Brain step completed!" << std::endl;
    std::cout << "Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    
    return 0;
}
