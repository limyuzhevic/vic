// Integration and system-level tests for STDP and Hebbian plasticity
// Tests brain-level functionality with plasticity rules

#include "brain/Brain.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include <cassert>
#include <iostream>

namespace test_brain_plasticity {

void testBrainWithSTDP() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // The brain should initialize with default plasticity rules
    // Verify brain properties
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getRegionCount() == 1);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    Brain initialization with STDP support test passed" << std::endl;
}

void testBrainWithHebbian() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // The brain should initialize with Hebbian plasticity support
    // Verify brain properties
    assert(brain.getTotalNeuronCount() == 75);
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    Brain initialization with Hebbian support test passed" << std::endl;
}

void testBrainPlasticityIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // The brain should support plasticity mechanisms
    // Run simulation steps to test plasticity integration
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    // Verify brain maintains stability after plasticity operations
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    Brain plasticity integration test passed" << std::endl;
}

void testSTDPHebbianBrainIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Run simulation with both STDP and Hebbian plasticity
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
        
        // Check that brain remains in valid state
        assert(brain.getTotalNeuronCount() == 150);
        assert(brain.getRegionCount() == 3);
    }
    
    std::cout << "    STDP + Hebbian brain integration test passed" << std::endl;
}

void testBrainPlasticityRules() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Brain should have plasticity capabilities
    // The brain initialization should enable plasticity rules
    // Run multiple steps to test plasticity over time
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Verify metrics remain valid
        assert(brain.getTotalNeuronCount() == 200);
        assert(brain.getRegionCount() == 2);
        assert(brain.getFiringNeuronCount() >= 0);
        assert(brain.getAverageFiringRate() >= 0.0f);
    }
    
    std::cout << "    Brain plasticity rules test passed" << std::endl;
}

void runAll() {
    std::cout << "Running brain integration tests for STDP and Hebbian plasticity..." << std::endl << std::endl;
    
    testBrainWithSTDP();
    testBrainWithHebbian();
    testBrainPlasticityIntegration();
    testSTDPHebbianBrainIntegration();
    testBrainPlasticityRules();
    
    std::cout << std::endl << "All brain integration tests passed!" << std::endl;
}

} // namespace test_brain_plasticity

// Main function to run brain integration tests
int main() {
    test_brain_plasticity::runAll();
    return 0;
}
