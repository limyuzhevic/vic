#include "core/Config/ConfigBuilder.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing ConfigBuilder..." << std::endl;
    
    // Test 1: Basic builder functionality
    std::cout << "Test 1: Basic builder" << std::endl;
    auto config1 = nlm::ConfigBuilder::defaultPhase2()
                     .setNeuronCount(1000)
                     .setRegionCount(2)
                     .build();
    
    assert(config1 != nullptr);
    assert(config1->getOr<int64_t>("neuron_count", 0) == 1000);
    assert(config1->getOr<int64_t>("region_count", 0) == 2);
    std::cout << "  PASSED" << std::endl;
    
    // Test 2: Phase 6 builder
    std::cout << "Test 2: Phase 6 builder" << std::endl;
    auto phase6Config = nlm::ConfigBuilder::defaultPhase6()
                           .setPhase6NeuronCount(2000)
                           .setPhase6MaxSteps(5000)
                           .setPhase6EnableDevelopment(true)
                           .buildPhase6Config();
    
    assert(phase6Config.neuronCount == 2000);
    assert(phase6Config.maxSteps == 5000);
    assert(phase6Config.enableDevelopment == true);
    std::cout << "  PASSED" << std::endl;
    
    // Test 3: Builder chaining
    std::cout << "Test 3: Builder chaining" << std::endl;
    auto config3 = nlm::ConfigBuilder::minimalConfig()
                     .setRandomSeed(12345)
                     .setTimestep(0.01)
                     .setNeuronCount(100)
                     .build();
    
    assert(config3->getOr<int64_t>("random_seed", 0) == 12345);
    assert(config3->getOr<double>("simulation_timestep", 0.0) == 0.01);
    assert(config3->getOr<int64_t>("neuron_count", 0) == 100);
    std::cout << "  PASSED" << std::endl;
    
    // Test 4: Validation
    std::cout << "Test 4: Validation" << std::endl;
    auto config4 = nlm::ConfigBuilder::minimalConfig()
                     .setNeuronCount(0); // Invalid
    
    assert(!config4->validate());
    std::cout << "  PASSED" << std::endl;
    
    std::cout << "All ConfigBuilder tests passed!" << std::endl;
    return 0;
}
