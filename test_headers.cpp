#include <iostream>
#include <ostream>

#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/brain/Neuron.hpp"

int main() {
    // Test that the headers can be included without errors
    // This is a minimal test to verify the constants are properly defined
    
    // Test ConfigConstants
    static_assert(nlm::ConfigConstants::DEFAULT_NEURON_COUNT == 1000);
    static_assert(nlm::ConfigConstants::DEFAULT_REGION_COUNT == 1);
    static_assert(nlm::ConfigConstants::DEFAULT_CONNECTION_PROBABILITY == 0.1f);
    static_assert(nlm::ConfigConstants::DEFAULT_SIMULATION_TIMESTEP == 0.001);
    
    // Test BrainConstants
    static_assert(nlm::BrainConstants::DEFAULT_FIRST_REGION_ID.index() == 1);
    static_assert(nlm::BrainConstants::REPLAY_EPISODE_COUNT == 3);
    static_assert(nlm::BrainConstants::DEVELOPMENT_UPDATE_INTERVAL == 1000);
    static_assert(nlm::BrainConstants::MEMORY_CONSOLIDATION_THRESHOLD == 0.3f);
    
    // Test NeuronConstants
    static_assert(nlm::NeuronConstants::DEFAULT_RESTING_POTENTIAL == -70.0f);
    static_assert(nlm::NeuronConstants::DEFAULT_THRESHOLD == -55.0f);
    static_assert(nlm::NeuronConstants::DEFAULT_RESET_POTENTIAL == -70.0f);
    static_assert(nlm::NeuronConstants::MAX_MEMBRANE_POTENTIAL == 50.0f);
    static_assert(nlm::NeuronConstants::MIN_MEMBRANE_POTENTIAL == -100.0f);
    static_assert(nlm::NeuronConstants::DEFAULT_LEAK_CONDUCTANCE == 10.0f);
    static_assert(nlm::NeuronConstants::DEFAULT_MEMBRANE_CAPACITANCE == 1.0f);
    static_assert(nlm::NeuronConstants::DEFAULT_TIME_CONSTANT == 20.0f);
    static_assert(nlm::NeuronConstants::TIME_SCALE_FACTOR == 1000.0f);
    static_assert(nlm::NeuronConstants::DEFAULT_REFACTORY_PERIOD == 5);
    static_assert(nlm::NeuronConstants::MIN_REFACTORY_PERIOD == 2);
    static_assert(nlm::NeuronConstants::MAX_REFACTORY_PERIOD == 10);
    static_assert(nlm::NeuronConstants::ADAPTATION_INCREMENT == 1.0f);
    static_assert(nlm::NeuronConstants::ADAPTATION_DECAY_FACTOR == 0.95f);
    static_assert(nlm::NeuronConstants::LAST_SPIKE_TIME_SENTINEL == -1.0f);
    static_assert(nlm::NeuronConstants::DEFAULT_TIMESTEP == 0.001);
    
    std::cout << "All header files compiled successfully!" << std::endl;
    return 0;
}
