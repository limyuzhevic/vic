#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "Testing NLM Brain Modular Architecture" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Add basic configuration
    config->set<size_t>("neuron_count", 100);
    config->set<size_t>("region_count", 2);
    config->set<size_t>("connection_probability", 0.1f);
    config->set<size_t>("simulation_timestep", 0.001);
    config->set<size_t>("random_seed", 12345);
    
    // Create brain
    nlm::Brain brain(config);
    
    // Initialize brain
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    
    // Check component architecture
    if (brain.getNeuralComputation() == nullptr) {
        std::cerr << "Neural computation component not available" << std::endl;
        return 1;
    }
    
    if (brain.getMemoryIntegration() == nullptr) {
        std::cerr << "Memory integration component not available" << std::endl;
        return 1;
    }
    
    if (brain.getPredictionCoordinator() == nullptr) {
        std::cerr << "Prediction coordinator component not available" << std::endl;
        return 1;
    }
    
    if (brain.getCognitiveOrchestrator() == nullptr) {
        std::cerr << "Cognitive orchestrator component not available" << std::endl;
        return 1;
    }
    
    if (brain.getNeuromodulationController() == nullptr) {
        std::cerr << "Neuromodulation controller component not available" << std::endl;
        return 1;
    }
    
    if (brain.getPlasticityManager() == nullptr) {
        std::cerr << "Plasticity manager component not available" << std::endl;
        return 1;
    }
    
    if (brain.getDevelopmentManager() == nullptr) {
        std::cerr << "Development manager component not available" << std::endl;
        return 1;
    }
    
    if (brain.getSensoryMotorInterface() == nullptr) {
        std::cerr << "Sensory-motor interface component not available" << std::endl;
        return 1;
    }
    
    std::cout << "All components are available" << std::endl;
    
    // Run a few simulation steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    std::cout << "Simulation steps completed successfully" << std::endl;
    
    // Log brain status
    brain.logStatus();
    
    std::cout << "Modular architecture test completed successfully" << std::endl;
    return 0;
}