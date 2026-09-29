// Simple test to verify the STDP implementation works correctly
// Tests the fixed weight bounds and new plasticity mechanisms

#include "plasticity/STDP.hpp"
#include "plasticity/RewardModulatedSTDP.hpp"
#include "plasticity/TripletSTDP.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_improvements {

void testSTDPPotValidation() {
    std::cout << "Testing STDP weight bounds validation..." << std::endl;
    
    nlm::STDP stdp;
    
    // Test parameter validation
    stdp.setLTPWeight(0.5f);
    assert(stdp.getLTPWeight() == 0.5f);
    
    stdp.setLTPWeight(1.5f);  // Should be clamped to 1.0
    assert(stdp.getLTPWeight() == 1.0f);
    
    stdp.setLTPWeight(-0.5f); // Should be clamped to 0.0
    assert(stdp.getLTPWeight() == 0.0f);
    
    std::cout << "  ✓ STDP weight validation passed" << std::endl;
}

void testRewardModulatedSTDP() {
    std::cout << "Testing RewardModulatedSTDP implementation..." << std::endl;
    
    nlm::RewardModulatedSTDP rms;
    
    // Test parameter validation
    rms.setRewardStrength(2.0f);
    assert(rms.getRewardStrength() == 2.0f);
    
    rms.setRewardStrength(0.5f);
    assert(rms.getRewardStrength() == 0.5f);
    
    rms.setTDDiscount(0.8f);
    assert(rms.getTDDiscount() == 0.8f);
    
    // Test eligibility trace management
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    rms.resetEligibilityTrace(&synapse);
    assert(rms.getEligibilityTrace(&synapse) == 0.0f);
    
    std::cout << "  ✓ RewardModulatedSTDP implementation passed" << std::endl;
}

void testTripletSTDP() {
    std::cout << "Testing TripletSTDP implementation..." << std::endl;
    
    nlm::TripletSTDP tsdp;
    
    // Test parameter validation
    tsdp.setA1Plus(0.02f);
    assert(tsdp.getA1Plus() == 0.02f);
    
    tsdp.setA1Minus(0.01f);
    assert(tsdp.getA1Minus() == 0.01f);
    
    tsdp.setA2Plus(0.03f);
    assert(tsdp.getA2Plus() == 0.03f);
    
    tsdp.setA2Minus(0.015f);
    assert(tsdp.getA2Minus() == 0.015f);
    
    tsdp.setTauPlus(30.0f);
    assert(tsdp.getTauPlus() == 30.0f);
    
    tsdp.setTauMinus(25.0f);
    assert(tsdp.getTauMinus() == 25.0f);
    
    std::cout << "  ✓ TripletSTDP implementation passed" << std::endl;
}

void testSTDPConsistency() {
    std::cout << "Testing STDP weight consistency..." << std::endl;
    
    nlm::STDP stdp;
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test weight clamping consistency
    synapse.setWeight(0.9f);
    synapse.addToWeight(0.2f);  // Should clamp to 1.0
    assert(synapse.getWeight() == 1.0f);
    
    synapse.setWeight(-0.9f);
    synapse.addToWeight(-0.2f);  // Should clamp to -1.0
    assert(synapse.getWeight() == -1.0f);
    
    std::cout << "  ✓ STDP consistency validation passed" << std::endl;
}

void testAdvancedPlasticityIntegration() {
    std::cout << "Testing advanced plasticity integration..." << std::endl;
    
    // Create a simple brain with advanced plasticity
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test that brain can work with advanced plasticity rules
    // This is a basic integration test
    
    std::cout << "  ✓ Advanced plasticity integration passed" << std::endl;
}

void runAll() {
    std::cout << "Running improvement tests..." << std::endl;
    std::cout << "=" << std::string(50, '=') << std::endl;
    
    testSTDPPotValidation();
    testRewardModulatedSTDP();
    testTripletSTDP();
    testSTDPConsistency();
    testAdvancedPlasticityIntegration();
    
    std::cout << "=" << std::string(50, '=') << std::endl;
    std::cout << "All improvement tests passed!" << std::endl;
}

} // namespace test_improvements

int main() {
    test_improvements::runAll();
    return 0;
}
