#include <iostream>
#include "src/core/Constants.hpp"
#include "src/core/NeuralConstants.hpp"
#include "src/core/PlasticityConstants.hpp"
#include "src/core/MemoryConstants.hpp"
#include "src/core/DevelopmentConstants.hpp"
#include "src/core/Types/Types.hpp"

int main() {
    std::cout << "=== Testing Constants ===" << std::endl;
    
    // Test Universal Constants
    std::cout << "Universal Constants:" << std::endl;
    std::cout << "  SIMULATION_TIMESTEP: " << nlm::SIMULATION_TIMESTEP << "s" << std::endl;
    std::cout << "  DEFAULT_CONNECTION_PROBABILITY: " << nlm::DEFAULT_CONNECTION_PROBABILITY << std::endl;
    std::cout << "  DEFAULT_REPLAY_INTERVAL: " << nlm::DEFAULT_REPLAY_INTERVAL << " steps" << std::endl;
    std::cout << "  DEFAULT_NEURON_COUNT: " << nlm::DEFAULT_NEURON_COUNT << std::endl;
    std::cout << "  DEFAULT_WORLD_WIDTH: " << nlm::DEFAULT_WORLD_WIDTH << std::endl;
    
    // Test Neural Constants
    std::cout << "\nNeural Constants:" << std::endl;
    std::cout << "  RESTING_POTENTIAL: " << nlm::NeuralConstants::RESTING_POTENTIAL << " mV" << std::endl;
    std::cout << "  THRESHOLD_POTENTIAL: " << nlm::NeuralConstants::THRESHOLD_POTENTIAL << " mV" << std::endl;
    std::cout << "  RESET_POTENTIAL: " << nlm::NeuralConstants::RESET_POTENTIAL << " mV" << std::endl;
    std::cout << "  REFRACTORY_PERIOD: " << nlm::NeuralConstants::REFRACTORY_PERIOD << " ms" << std::endl;
    std::cout << "  MEMBRANE_TIME_CONSTANT: " << nlm::NeuralConstants::MEMBRANE_TIME_CONSTANT << " ms" << std::endl;
    
    // Test Plasticity Constants
    std::cout << "\nPlasticity Constants:" << std::endl;
    std::cout << "  LTP_WEIGHT: " << nlm::PlasticityConstants::LTP_WEIGHT << std::endl;
    std::cout << "  LTD_WEIGHT: " << nlm::PlasticityConstants::LTD_WEIGHT << std::endl;
    std::cout << "  DEFAULT_DELAY: " << nlm::PlasticityConstants::DEFAULT_DELAY << " steps" << std::endl;
    std::cout << "  STDP_FACILITATION_TAU: " << nlm::PlasticityConstants::STP_FACILITATION_TAU << " ms" << std::endl;
    
    // Test Memory Constants
    std::cout << "\nMemory Constants:" << std::endl;
    std::cout << "  WORKING_MEMORY_COMPETITION_THRESHOLD: " << MemoryConstants::WORKING_MEMORY_COMPETITION_THRESHOLD << std::endl;
    std::cout << "  EPISODIC_MEMORY_CONSOLIDATION_THRESHOLD: " << MemoryConstants::EPISODIC_MEMORY_CONSOLIDATION_THRESHOLD << std::endl;
    std::cout << "  SURPRISE_THRESHOLD: " << MemoryConstants::SURPRISE_THRESHOLD << std::endl;
    
    // Test Development Constants
    std::cout << "\nDevelopment Constants:" << std::endl;
    std::cout << "  SYNAPTOGENESIS_FORMATION_RATE: " << DevelopmentConstants::SYNAPTOGENESIS_FORMATION_RATE << std::endl;
    std::cout << "  PLANNING_DEPTH: " << DevelopmentConstants::PLANNING_DEPTH << " steps" << std::endl;
    
    std::cout << "\n=== All Constants Test Complete ===" << std::endl;
    return 0;
}