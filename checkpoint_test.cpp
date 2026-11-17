#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "=== NLM Checkpoint Test ===" << std::endl;
    std::cout << "Brain initialized successfully" << std::endl;
    
    // Test checkpoint manager access
    auto* checkpointManager = brain->getCheckpointManager();
    if (!checkpointManager) {
        std::cerr << "Checkpoint manager not available" << std::endl;
        return 1;
    }
    
    std::cout << "Checkpoint manager is available: " << (checkpointManager ? "YES" : "NO") << std::endl;
    
    // List checkpoints (should be empty initially)
    auto checkpoints = brain->listCheckpoints();
    std::cout << "Initial checkpoints: " << checkpoints.size() << std::endl;
    
    // Create a simple checkpoint provider function
    nlm::NeuronCheckpointData neuronData;
    nlm::SynapseCheckpointData synapseData;
    
    bool neuronProviderSuccess = false;
    bool synapseProviderSuccess = false;
    
    // Set up checkpoint providers
    brain->setCheckpointDataProviders(
        [&neuronData, &neuronProviderSuccess](nlm::NeuronCheckpointData& data) {
            data = neuronData;
            neuronProviderSuccess = true;
            return true;
        },
        [&synapseData, &synapseProviderSuccess](nlm::SynapseCheckpointData& data) {
            data = synapseData;
            synapseProviderSuccess = true;
            return true;
        }
    );
    
    std::cout << "Checkpoint providers set: " << (checkpointManager ? "YES" : "NO") << std::endl;
    
    // Test immediate checkpoint save (should succeed with empty providers)
    bool saveSuccess = brain->saveCheckpoint("test_checkpoint");
    std::cout << "Immediate checkpoint save: " << (saveSuccess ? "SUCCESS" : "FAILED") << std::endl;
    
    // Test checkpoint listing
    checkpoints = brain->listCheckpoints();
    std::cout << "Checkpoints after save: " << checkpoints.size() << std::endl;
    
    std::cout << "=== Checkpoint Test Complete ===" << std::endl;
    return 0;
}
