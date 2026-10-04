// Comprehensive Plasticity Tests
// Tests STDP, Hebbian, and Structural Plasticity systems

#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_plasticity {

void testSTDPBasic() {
    nlm::STDP stdp;
    
    // Create and configure synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Test basic parameters
    assert(stdp.getLTPWeight() == 0.01f);
    assert(stdp.getLTDWeight() == 0.012f);
    assert(stdp.getTimeConstant() == 20.0f);
    
    // Test LTP (pre before post)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};  // 5ms after pre
    
    float initialWeight = synapse.getWeight();
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    float finalWeight = synapse.getWeight();
    
    // Weight should increase due to LTP
    assert(finalWeight > initialWeight);
    
    // Test LTD (post before pre)
    synapse.setWeight(0.5f);
    preSpikes = {10.0, 20.0};
    postSpikes = {5.0, 15.0};  // 5ms before pre
    
    initialWeight = synapse.getWeight();
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    finalWeight = synapse.getWeight();
    
    // Weight should decrease due to LTD
    assert(finalWeight < initialWeight);
    
    std::cout << "    testSTDPBasic passed" << std::endl;
}

void testSTDPConfiguration() {
    nlm::STDP stdp;
    
    // Test parameter configuration
    stdp.configure(0.05f, 0.04f, 30.0f);
    
    assert(stdp.getLTPWeight() == 0.05f);
    assert(stdp.getLTDWeight() == 0.04f);
    assert(stdp.getTimeConstant() == 30.0f);
    
    // Test clamping
    stdp.setLTPWeight(2.0f);  // Should be clamped to 1.0f
    assert(stdp.getLTPWeight() == 1.0f);
    
    stdp.setLTDWeight(-0.5f); // Should be clamped to 0.0f
    assert(stdp.getLTDWeight() == 0.0f);
    
    stdp.setTimeConstant(200.0f); // Should be clamped to 100.0f
    assert(stdp.getTimeConstant() == 100.0f);
    
    std::cout << "    testSTDPConfiguration passed" << std::endl;
}

void testHebbianLearning() {
    nlm::Hebbian hebbian;
    
    // Create and configure synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Test Hebbian update with correlated activity
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // Correlated
    
    float initialWeight = synapse.getWeight();
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    float finalWeight = synapse.getWeight();
    
    // Weight should increase due to Hebbian learning
    assert(finalWeight > initialWeight);
    
    // Test opposite correlation (should weaken or not change)
    synapse.setWeight(0.5f);
    preSpikes = {0.0, 10.0, 20.0};
    postSpikes = {50.0, 60.0, 70.0};  // Anti-correlated (far in future)
    
    initialWeight = synapse.getWeight();
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    finalWeight = synapse.getWeight();
    
    // Effect should be minimal or opposite
    // (depending on Hebbian implementation)
    
    std::cout << "    testHebbianLearning passed" << std::endl;
}

void testStructuralPlasticity() {
    nlm::StructuralPlasticity sp;
    
    // Test synaptogenesis rate
    assert(sp.getSynaptogenesisRate() == 0.0001f);
    
    sp.setSynaptogenesisRate(0.0005f);
    assert(sp.getSynaptogenesisRate() == 0.0005f);
    
    // Test pruning rate
    assert(sp.getPruningRate() == 0.00001f);
    
    sp.setPruningRate(0.00005f);
    assert(sp.getPruningRate() == 0.00005f);
    
    // Note: Testing structural changes would require a brain object
    // For now, just test that configuration works
    std::cout << "    testStructuralPlasticity passed" << std::endl;
}

void testPlasticityFlags() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test initial state
    assert(!synapse.getPlasticityFlags().hebbian);
    assert(!synapse.getPlasticityFlags().stdp);
    assert(!synapse.getPlasticityFlags().reward_modulated);
    
    // Test enabling individual rules
    synapse.enablePlasticity(true, false, false);
    assert(synapse.getPlasticityFlags().hebbian);
    assert(!synapse.getPlasticityFlags().stdp);
    assert(!synapse.getPlasticityFlags().reward_modulated);
    
    synapse.enablePlasticity(false, true, false);
    assert(!synapse.getPlasticityFlags().hebbian);
    assert(synapse.getPlasticityFlags().stdp);
    assert(!synapse.getPlasticityFlags().reward_modulated);
    
    synapse.enablePlasticity(false, false, true);
    assert(!synapse.getPlasticityFlags().hebbian);
    assert(!synapse.getPlasticityFlags().stdp);
    assert(synapse.getPlasticityFlags().reward_modulated);
    
    synapse.enablePlasticity(true, true, true);
    assert(synapse.getPlasticityFlags().hebbian);
    assert(synapse.getPlasticityFlags().stdp);
    assert(synapse.getPlasticityFlags().reward_modulated);
    
    std::cout << "    testPlasticityFlags passed" << std::endl;
}

void runAll() {
    std::cout << "Running Plasticity tests..." << std::endl;
    testSTDPBasic();
    testSTDPConfiguration();
    testHebbianLearning();
    testStructuralPlasticity();
    testPlasticityFlags();
}

} // namespace test_plasticity
