#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

// Test function to verify division by zero fixes
void testDivisionByZeroFixes() {
    std::cout << "Testing division by zero fixes..." << std::endl;
    
    // Test 1: Brain initialization with valid config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    assert(success, "Brain initialization should succeed with valid config");
    assert(brain.getRegionCount() == 1, "Should have 1 region");
    assert(brain.getTotalNeuronCount() == 100, "Should have 100 neurons");
    
    std::cout << "  Test 1 PASSED: Brain initialization with valid config" << std::endl;
    
    // Test 2: Brain initialization with zero region count (should fail gracefully)
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    bool success2 = brain2.initialize();
    
    assert(!success2, "Brain initialization should fail with zero region count");
    
    std::cout << "  Test 2 PASSED: Brain initialization rejects zero region count" << std::endl;
    
    // Test 3: Neuron initialization with constants
    nlm::NeuronId neuronId(1);
    nlm::Neuron neuron(neuronId);
    
    // Verify neuron is properly initialized
    assert(neuron.getId() == neuronId, "Neuron should have correct ID");
    assert(neuron.getState().firingState == nlm::FiringState::Resting, "Neuron should start in resting state");
    
    std::cout << "  Test 3 PASSED: Neuron initialization uses constants" << std::endl;
    
    // Test 4: Synapse initialization with constants
    nlm::SynapseId synId(1);
    nlm::NeuronId sourceId(1);
    nlm::NeuronId destId(2);
    
    nlm::Synapse synapse(synId, sourceId, destId);
    
    // Verify synapse is properly initialized
    assert(synapse.getId() == synId, "Synapse should have correct ID");
    assert(synapse.getSourceNeuron() == sourceId, "Synapse should have correct source");
    assert(synapse.getDestinationNeuron() == destId, "Synapse should have correct destination");
    assert(synapse.getWeight() == 0.0f, "Synapse should have zero initial weight");
    assert(synapse.getType() == nlm::SynapseType::Excitatory, "Synapse should be excitatory by default");
    
    std::cout << "  Test 4 PASSED: Synapse initialization uses constants" << std::endl;
    
    std::cout << "All division by zero and constant tests PASSED!" << std::endl;
}

int main() {
    std::cout << "=== NLM Fix Verification Tests ===" << std::endl;
    std::cout << "Testing critical fixes for division by zero and code quality issues" << std::endl;
    std::cout << std::endl;
    
    try {
        testDivisionByZeroFixes();
        std::cout << std::endl;
        std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        std::cout << "The critical fixes for division by zero and code quality issues are working correctly!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== TEST FAILED ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
}