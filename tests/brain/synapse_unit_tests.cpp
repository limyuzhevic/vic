// Unit test for Synapse
#include "brain/Synapse.hpp"
#include <cassert>
#include <iostream>

namespace test_synapse_unit {

void testSynapseCreation() {
    nlm::Synapse synapse;
    
    // Test basic synapse properties
    assert(synapse.getId().has_value());
    assert(synapse.getPreNeuronId().has_value());
    assert(synapse.getPostNeuronId().has_value());
    assert(synapse.getStrength() == 1.0f);
    assert(synapse.getDelay() == 0.001f);
    
    std::cout << "    testSynapseCreation passed" << std::endl;
}

void testSynapsePlasticity() {
    nlm::Synapse synapse;
    
    // Test plasticity
    float initialStrength = synapse.getStrength();
    
    // Apply STDP-like plasticity
    synapse.applyPlasticity(1.0f, 0.5f); // pre_spike, post_spike
    
    float newStrength = synapse.getStrength();
    assert(newStrength != initialStrength);
    
    std::cout << "    testSynapsePlasticity passed" << std::endl;
}

void testSynapseConnection() {
    nlm::Synapse synapse;
    
    // Test connection properties
    neuron_id preNeuron = 1;
    neuron_id postNeuron = 2;
    
    synapse.setPreNeuronId(preNeuron);
    synapse.setPostNeuronId(postNeuron);
    
    assert(synapse.getPreNeuronId() == preNeuron);
    assert(synapse.getPostNeuronId() == postNeuron);
    
    std::cout << "    testSynapseConnection passed" << std::endl;
}

void testSynapseType() {
    nlm::Synapse synapse;
    
    // Test synapse type
    synapse.setType(nlm::SynapseType::Excitatory);
    assert(synapse.getType() == nlm::SynapseType::Excitatory);
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    assert(synapse.getType() == nlm::SynapseType::Inhibitory);
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testSynapseState() {
    nlm::Synapse synapse;
    
    // Test synapse state
    synapse.setActive(true);
    assert(synapse.isActive());
    
    synapse.setActive(false);
    assert(!synapse.isActive());
    
    std::cout << "    testSynapseState passed" << std::endl;
}

void testSynapseDelay() {
    nlm::Synapse synapse;
    
    // Test delay
    float delay = 0.005f;
    synapse.setDelay(delay);
    assert(synapse.getDelay() == delay);
    
    std::cout << "    testSynapseDelay passed" << std::endl;
}

void runAllSynapseUnitTests() {
    testSynapseCreation();
    testSynapsePlasticity();
    testSynapseConnection();
    testSynapseType();
    testSynapseState();
    testSynapseDelay();
}

} // namespace test_synapse_unit