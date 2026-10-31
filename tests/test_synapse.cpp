// Synapse Tests
#include "nlm.h"
#include <cassert>
#include <iostream>

namespace test_synapse {

void testSynapseCreation() {
    nlm::SynapseId synId(1);
    nlm::NeuronId srcId(1);
    nlm::NeuronId dstId(2);
    
    nlm::Synapse synapse(synId, srcId, dstId);
    
    assert(synapse.getId() == synId);
    assert(synapse.getSourceNeuron() == srcId);
    assert(synapse.getDestinationNeuron() == dstId);
    
    std::cout << "    testSynapseCreation passed" << std::endl;
}

void testSynapseWeight() {
    nlm::SynapseId synId(1);
    nlm::NeuronId srcId(1);
    nlm::NeuronId dstId(2);
    
    nlm::Synapse synapse(synId, srcId, dstId);
    
    assert(synapse.getWeight() == 0.0f);
    
    synapse.setWeight(0.5f);
    assert(synapse.getWeight() == 0.5f);
    
    synapse.addToWeight(0.2f);
    assert(std::abs(synapse.getWeight() - 0.7f) < 0.001f);
    
    std::cout << "    testSynapseWeight passed" << std::endl;
}

void testSynapseType() {
    nlm::SynapseId synId(1);
    nlm::NeuronId srcId(1);
    nlm::NeuronId dstId(2);
    
    nlm::Synapse synapse(synId, srcId, dstId);
    
    assert(synapse.getType() == nlm::SynapseType::Excitatory);
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    assert(synapse.getType() == nlm::SynapseType::Inhibitory);
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testSynapseExcitatoryInhibitory() {
    nlm::SynapseId synId(1);
    nlm::NeuronId srcId(1);
    nlm::NeuronId dstId(2);
    
    nlm::Synapse synapse(synId, srcId, dstId);
    
    assert(synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    assert(!synapse.isExcitatory());
    assert(synapse.isInhibitory());
    
    std::cout << "    testSynapseExcitatoryInhibitory passed" << std::endl;
}

void testSynapseDelay() {
    nlm::SynapseId synId(1);
    nlm::NeuronId srcId(1);
    nlm::NeuronId dstId(2);
    
    nlm::Synapse synapse(synId, srcId, dstId);
    
    assert(synapse.getDelay() == 1);  // Default delay
    
    synapse.setDelay(5);
    assert(synapse.getDelay() == 5);
    
    std::cout << "    testSynapseDelay passed" << std::endl;
}

void testSynapsePlasticity() {
    nlm::SynapseId synId(1);
    nlm::NeuronId srcId(1);
    nlm::NeuronId dstId(2);
    
    nlm::Synapse synapse(synId, srcId, dstId);
    
    synapse.enablePlasticity(true, true, false);  // Enable Hebbian and STDP
    
    // Check that plasticity flags are set
    const auto& flags = synapse.getPlasticityFlags();
    assert(flags.hebbian);
    assert(flags.stdp);
    assert(!flags.reward_modulated);
    
    std::cout << "    testSynapsePlasticity passed" << std::endl;
}

void testSynapseEligibilityTrace() {
    nlm::SynapseId synId(1);
    nlm::NeuronId srcId(1);
    nlm::NeuronId dstId(2);
    
    nlm::Synapse synapse(synId, srcId, dstId);
    
    assert(synapse.getEligibilityTrace() == 0.0f);
    
    synapse.setEligibilityTrace(0.5f);
    assert(std::abs(synapse.getEligibilityTrace() - 0.5f) < 0.001f);
    
    synapse.decayEligibilityTrace(0.1f);
    assert(std::abs(synapse.getEligibilityTrace() - 0.45f) < 0.001f);
    
    std::cout << "    testSynapseEligibilityTrace passed" << std::endl;
}

void testSynapseEfficacy() {
    nlm::SynapseId synId(1);
    nlm::NeuronId srcId(1);
    nlm::NeuronId dstId(2);
    
    nlm::Synapse synapse(synId, srcId, dstId);
    
    assert(synapse.getEfficacy() == 1.0f);
    
    synapse.setEfficacy(1.5f);
    assert(std::abs(synapse.getEfficacy() - 1.5f) < 0.001f);
    
    // Efficacy should be clamped to max 2.0
    synapse.setEfficacy(3.0f);
    assert(std::abs(synapse.getEfficacy() - 2.0f) < 0.001f);
    
    std::cout << "    testSynapseEfficacy passed" << std::endl;
}

void runAll() {
    testSynapseCreation();
    testSynapseWeight();
    testSynapseType();
    testSynapseExcitatoryInhibitory();
    testSynapseDelay();
    testSynapsePlasticity();
    testSynapseEligibilityTrace();
    testSynapseEfficacy();
}

} // namespace test_synapse
