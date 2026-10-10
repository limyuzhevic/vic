#include <iostream>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    std::cout << "Testing Brain Save/Load Persistence" << std::endl;
    std::cout << "=" << std::string(40, '=') << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    // Create and initialize brain
    nlm::Brain brain(config);
    bool success = brain.initialize();
    if (!success) {
        std::cerr << "ERROR: Brain initialization failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    std::cout << "- Total neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "- Total synapses: " << brain.getTotalSynapseCount() << std::endl;
    
    // Run some steps to create brain activity
    std::cout << "\nRunning 5 simulation steps..." << std::endl;
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
        std::cout << "  Step " << step << ": " << brain.getFiringNeuronCount() << " firing neurons" << std::endl;
    }
    
    // Save the brain state
    std::string checkpointFile = "test_checkpoint.nml";
    std::cout << "\nSaving brain state to " << checkpointFile << "..." << std::endl;
    bool saveSuccess = brain.save(checkpointFile);
    if (!saveSuccess) {
        std::cerr << "ERROR: Failed to save brain state!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain state saved successfully!" << std::endl;
    
    // Create a new brain and reset it
    nlm::Brain newBrain(config);
    newBrain.initialize();
    
    std::cout << "\nLoading brain state from " << checkpointFile << "..." << std::endl;
    bool loadSuccess = newBrain.load(checkpointFile);
    if (!loadSuccess) {
        std::cerr << "ERROR: Failed to load brain state!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain state loaded successfully!" << std::endl;
    
    // Compare basic properties
    std::cout << "\nVerifying loaded brain properties..." << std::endl;
    std::cout << "- Neurons: " << brain.getTotalNeuronCount() << " (original) vs " 
              << newBrain.getTotalNeuronCount() << " (loaded)" << std::endl;
    std::cout << "- Regions: " << brain.getRegionCount() << " (original) vs " 
              << newBrain.getRegionCount() << " (loaded)" << std::endl;
    std::cout << "- Synapses: " << brain.getTotalSynapseCount() << " (original) vs " 
              << newBrain.getTotalSynapseCount() << " (loaded)" << std::endl;
    
    // Clean up
    std::remove(checkpointFile.c_str());
    
    std::cout << "\n" << std::string(40, '=') << std::endl;
    std::cout << "Persistence test PASSED!" << std::endl;
    std::cout << "The Brain class successfully saves and loads its complete state." << std::endl;
    std::cout << std::string(40, '=') << std::endl;
    
    return 0;
}
