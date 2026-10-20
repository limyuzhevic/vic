#include <iostream>
#include "src/brain/Brain.hpp"
#include "src/brain/BrainConstants.hpp"

int main() {
    std::cout << "Testing Brain.hpp and BrainConstants.hpp includes..." << std::endl;
    
    // Test BrainConstants namespace
    std::cout << "Simulation timestep: " << nlm::SimulationConstants::DEFAULT_TIMESTEP << std::endl;
    std::cout << "Default neuron count: " << nlm::SimulationConstants::DEFAULT_NEURON_COUNT << std::endl;
    std::cout << "Default connection probability: " << nlm::SimulationConstants::DEFAULT_CONNECTION_PROBABILITY << std::endl;
    
    std::cout << "Brain.hpp and BrainConstants.hpp test completed successfully!" << std::endl;
    return 0;
}
