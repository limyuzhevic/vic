// STDP Tests
// Phase 2: Real Neural Computation Tests - Spike-Timing-Dependent Plasticity

#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace test_stdp {

void testSTDPCreation() {
    nlm::STDP stdp;
    
    assert(stdp.getLTPWeight() == 0.01f);
    assert(stdp.getLTDWeight() == 0.012f);
    assert(stdp.getTimeConstant() == 20.0f);
    
    std::cout << "    testSTDPCreation passed" << std::endl;
}

void testSTDPConfiguration() {
    nlm::STDP stdp;
    
    stdp.setLTPWeight(0.05f);
    stdp.setLTDWeight(0.06f);
    stdp.setTimeConstant(30.0f);
    
    assert(stdp.getLTPWeight() == 0.05f);
    assert(stdp.getLTDWeight() == 0.06f);
    assert(stdp.getTimeConstant() == 30.0f);
    
    std::cout << "    testSTDPConfiguration passed" << std::endl;
}

void testSTDPConfigure() {
    nlm::STDP stdp;
    
    stdp.configure(0.03f, 0.035f, 25.0f);
    
    assert(stdp.getLTPWeight() == 0.03f);
    assert(stdp.getLTDWeight() == 0.035f);
    assert(stdp.getTimeConstant() == 25.0f);
    
    std::cout << "    testSTDPConfigure passed" << std::endl;
}

void testSTDPName() {
    nlm::STDP stdp;
    
    assert(stdp.getName() != nullptr);
    assert(std::string(stdp.getName()) == "STDP");
    
    std::cout << "    testSTDPName passed" << std::endl;
}

void testSTDPPotentiation() {
    nlm::STDP stdp;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post (positive dt) - should potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // ms (10ms after pre)
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to pre-before-post
    assert(finalWeight > initialWeight);
    
    std::cout << "    testSTDPPotentiation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testSTDPDepression() {
    nlm::STDP stdp;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Post fires before pre (negative dt) - should depress
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // 5ms before pre
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased due to post-before-pre
    assert(finalWeight < initialWeight);
    
    std::cout << "    testSTDPDepression passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testSTDPEmptySpikes() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes - should not change
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    // Empty post spikes - should not change
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testSTDPEmptySpikes passed" << std::endl;
}

void testSTDPWeightBounds() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f);
    
    std::cout << "    testSTDPWeightBounds passed" << std::endl;
}

void testSTDPTimeConstant() {
    nlm::STDP stdp;
    
    // Test different time constants
    stdp.setTimeConstant(10.0f);
    assert(stdp.getTimeConstant() == 10.0f);
    
    stdp.setTimeConstant(50.0f);
    assert(stdp.getTimeConstant() == 50.0f);
    
    // Test bounds
    stdp.setTimeConstant(0.5f);  // Below minimum
    assert(stdp.getTimeConstant() == 1.0f);  // Should be clamped to min
    
    stdp.setTimeConstant(1000.0f);  // Above maximum
    assert(stdp.getTimeConstant() == 100.0f);  // Should be clamped to max
    
    std::cout << "    testSTDPTimeConstant passed" << std::endl;
}

void testSTDPParameterBounds() {
    nlm::STDP stdp;
    
    // Test LTP weight bounds
    stdp.setLTPWeight(-1.0f);  // Below minimum
    assert(stdp.getLTPWeight() == 0.0f);  // Should be clamped to min
    
    stdp.setLTPWeight(5.0f);   // Above maximum
    assert(stdp.getLTPWeight() == 1.0f);  // Should be clamped to max
    
    // Test LTD weight bounds
    stdp.setLTDWeight(-1.0f);
    assert(stdp.getLTDWeight() == 0.0f);
    
    stdp.setLTDWeight(5.0f);
    assert(stdp.getLTDWeight() == 1.0f);
    
    std::cout << "    testSTDPParameterBounds passed" << std::endl;
}

void testSTDPSpikeTimingVariations() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Test very small dt (should have significant effect)
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {0.5};  // 0.5ms after pre
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    float weightAfterSmallDt = synapse.getWeight();
    
    // Reset
    synapse.setWeight(0.5f);
    
    // Test large dt (should have minimal effect)
    preSpikes = {0.0};
    postSpikes = {100.0};  // 100ms after pre
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    float weightAfterLargeDt = synapse.getWeight();
    
    // Small dt should produce larger potentiation than large dt
    assert(weightAfterSmallDt > weightAfterLargeDt);
    
    std::cout << "    testSTDPSpikeTimingVariations passed" << std::endl;
}

void testSTDPConnectionType() {
    nlm::STDP stdp;
    
    // Test that STDP works differently for different synapse types
    // Excitatory synapse
    nlm::Synapse excitatorySynapse(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    excitatorySynapse.setType(nlm::SynapseType::Excitatory);
    excitatorySynapse.setWeight(0.5f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0};
    
    stdp.update(&excitatorySynapse, preSpikes, postSpikes, 0.001);
    float excitatoryWeightAfter = excitatorySynapse.getWeight();
    
    // Inhibitory synapse
    nlm::Synapse inhibitorySynapse(nlm::SynapseId(7), nlm::NeuronId(13), nlm::NeuronId(14));
    inhibitorySynapse.setType(nlm::SynapseType::Inhibitory);
    inhibitorySynapse.setWeight(0.5f);
    
    stdp.update(&inhibitorySynapse, preSpikes, postSpikes, 0.001);
    float inhibitoryWeightAfter = inhibitorySynapse.getWeight();
    
    // Both should change, but may do so differently based on type
    assert(inhibitoryWeightAfter != 0.5f);  // Should change due to STDP
    
    std::cout << "    testSTDPConnectionType passed" << std::endl;
}

void testSTDPLearningRate() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(8), nlm::NeuronId(15), nlm::NeuronId(16));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Test with high learning rate
    stdp.setLTPWeight(0.1f);   // Higher potentiation
    stdp.setLTDWeight(0.12f);  // Higher depression
    
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    float weightAfterHighRate = synapse.getWeight();
    
    // Reset
    synapse.setWeight(0.5f);
    
    // Test with low learning rate
    stdp.setLTPWeight(0.001f);   // Lower potentiation
    stdp.setLTDWeight(0.0012f);  // Lower depression
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    float weightAfterLowRate = synapse.getWeight();
    
    // High learning rate should produce larger changes
    assert(std::abs(weightAfterHighRate - initialWeight) > 
           std::abs(weightAfterLowRate - initialWeight));
    
    std::cout << "    testSTDPLearningRate passed" << std::endl;
}

void testSTDPIntegration() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(9), nlm::NeuronId(17), nlm::NeuronId(18));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.3f);
    synapse.enablePlasticity(true, false, false);  // Only STDP enabled
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 5.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {2.0, 7.0, 12.0};
    
    // Record weight before
    float weightBefore = synapse.getWeight();
    
    // Update with STDP enabled
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float weightAfter = synapse.getWeight();
    assert(weightAfter != weightBefore);  // Should have changed
    
    // Now disable STDP
    synapse.enablePlasticity(false, false, false);
    synapse.setWeight(0.3f);  // Reset
    
    weightBefore = synapse.getWeight();
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    weightAfter = synapse.getWeight();
    
    assert(weightAfter == weightBefore);  // Should not change when STDP disabled
    
    std::cout << "    testSTDPIntegration passed" << std::endl;
}

void runAll() {
    std::cout << "Running STDP tests..." << std::endl;
    testSTDPCreation();
    testSTDPConfiguration();
    testSTDPConfigure();
    testSTDPName();
    testSTDPPotentiation();
    testSTDPDepression();
    testSTDPEmptySpikes();
    testSTDPWeightBounds();
    testSTDPTimeConstant();
    testSTDPParameterBounds();
    testSTDPSpikeTimingVariations();
    testSTDPConnectionType();
    testSTDPLearningRate();
    testSTDPIntegration();
}

} // namespace test_stdp
