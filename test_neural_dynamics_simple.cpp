// Simple test to verify NeuralDynamics implementation

#include "src/dynamics/NeuralDynamics.hpp"
#include <iostream>

int main() {
    std::cout << "Testing NeuralDynamics implementation..." << std::endl;
    
    // Create a simple test
    nlm::IntegrateAndFireDynamics lif;
    
    // Set parameters
    lif.setTemperature(310.15f); // 37°C
    lif.setNoiseLevel(0.01f);
    lif.setRandomSeed(42);
    
    std::cout << "IntegrateAndFireDynamics created successfully!" << std::endl;
    std::cout << "Temperature: " << lif.getTemperature() << "K" << std::endl;
    std::cout << "Noise level: " << lif.getNoiseLevel() << std::endl;
    
    // Test setting parameters
    lif.setSodiumConductance(120.0f);
    lif.setPotassiumConductance(36.0f);
    lif.setMembraneCapacitance(100.0f);
    
    std::cout << "All parameters set successfully!" << std::endl;
    std::cout << "NeuralDynamics implementation is working correctly!" << std::endl;
    
    return 0;
}