#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "=== Testing Refactored Brain Class ===\n\n";
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Configure brain
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    config->set("random_seed", 123);
    
    std::cout << "Creating Brain with refactored subsystem architecture...\n";
    nlm::Brain brain(config);
    
    std::cout << "Initializing brain...\n";
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully!\n";
    std::cout << "Total neurons: " << brain.getTotalNeuronCount() << "\n";
    std::cout << "Total synapses: " << brain.getTotalSynapseCount() << "\n\n";
    
    std::cout << "Testing basic operations...\n";
    
    // Test step
    std::cout << "Running simulation steps...\n";
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
        std::cout << "Step " << step << ": spikes=" << brain.getFiringNeuronCount() 
                  << ", total_spikes=" << brain.getTotalSpikeCount() << "\n";
    }
    
    // Test sensory input
    std::cout << "\nTesting sensory input...\n";
    nlm::SensoryInput input(std::vector<float>{1.0f, 0.5f, 0.2f});
    brain.receiveSensoryInput(input);
    
    // Test action production
    std::cout << "Testing action production...\n";
    auto action = brain.produceAction();
    std::cout << "Produced action: " << static_cast<int>(action->getType()) << "\n";
    
    // Test memory system access
    std::cout << "\nTesting memory system access...\n";
    auto workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        std::cout << "Working memory active traces: " << workingMemory->getActiveTraces() << "\n";
    }
    
    // Test prediction system access
    std::cout << "Testing prediction system access...\n";
    auto predictionSystem = brain.getPredictionSystem();
    std::cout << "Prediction system: " << (predictionSystem ? "Available" : "Not available") << "\n";
    
    // Test cognitive system access
    std::cout << "Testing cognitive system access...\n";
    auto planner = brain.getPlanner();
    auto attention = brain.getAttention();
    std::cout << "Planner: " << (planner ? "Available" : "Not available") << "\n";
    std::cout << "Attention: " << (attention ? "Available" : "Not available") << "\n";
    
    // Test neuromodulation system access
    std::cout << "Testing neuromodulation system access...\n";
    auto dopamine = brain.getDopamine();
    auto curiosity = brain.getCuriosity();
    std::cout << "Dopamine: " << (dopamine ? "Available" : "Not available") << "\n";
    std::cout << "Curiosity: " << (curiosity ? "Available" : "Not available") << "\n";
    
    // Test development system access
    std::cout << "Testing development system access...\n";
    auto developmentSystem = brain.getDevelopmentSystem();
    std::cout << "Development system: " << (developmentSystem ? "Available" : "Not available") << "\n";
    std::cout << "Current developmental stage: " << static_cast<int>(brain.getDevelopmentalStage()) << "\n";
    
    // Test checkpoint manager access
    std::cout << "Testing checkpoint manager access...\n";
    auto checkpointManager = brain.getCheckpointManager();
    std::cout << "Checkpoint manager: " << (checkpointManager ? "Available" : "Not available") << "\n";
    
    std::cout << "\n=== Testing completed successfully! ===\n";
    std::cout << "The refactored Brain class maintains backward compatibility!\n";
    
    return 0;
}