// Basic NLM Example: Simple Brain Simulation

#include "nlm/brain/Brain.hpp"
#include "nlm/config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "=== Basic NLM Brain Example ===" << std::endl;
    
    // Create a simple configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::User);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::User);
    config->set("connection_probability", 0.1, nlm::ConfigSource::User);
    
    // Create the brain
    nlm::Brain brain(config);
    
    // Initialize the brain
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    std::cout << "Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "Regions: " << brain.getRegionCount() << std::endl;
    
    // Run simulation steps
    std::cout << "\nRunning simulation..." << std::endl;
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Print progress every 5 steps
        if (step % 5 == 0) {
            std::cout << "Step " << step << ": "
                     << "Active: " << brain.getActiveNeuronCount()
                     << ", Firing: " << brain.getFiringNeuronCount()
                     << ", Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
        }
    }
    
    std::cout << "\nSimulation completed!" << std::endl;
    std::cout << "Final statistics:" << std::endl;
    std::cout << "  Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "  Active neurons: " << brain.getActiveNeuronCount() << std::endl;
    std::cout << "  Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
    std::cout << "  Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  E/I ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
    
    return 0;
}
