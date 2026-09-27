// Types Tests
#include "core/Types/Types.hpp"
#include "core/Exception.hpp"
#include <iostream>

namespace test_types {

void testNeuronId() {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::NeuronId id3(1);
    
    if (!(id1 == id3)) {
        throw nlm::NLMTestException("NeuronId equality test failed: id1 should equal id3");
    }
    if (id1 != id2) {
        throw nlm::NLMTestException("NeuronId inequality test failed: id1 should not equal id2");
    }
    if (!(id1 < id2)) {
        throw nlm::NLMTestException("NeuronId comparison test failed: id1 should be less than id2");
    }
    if (!(id2 > id1)) {
        throw nlm::NLMTestException("NeuronId comparison test failed: id2 should be greater than id1");
    }
    if (id1.index() != 1) {
        throw nlm::NLMTestException("NeuronId index test failed: expected index 1, got " + std::to_string(id1.index()));
    }
    
    std::cout << "    testNeuronId passed" << std::endl;
}

void testSynapseId() {
    nlm::SynapseId id1(100);
    nlm::SynapseId id2(200);
    
    if (id1 == id2) {
        throw nlm::NLMTestException("SynapseId inequality test failed: id1 should not equal id2");
    }
    if (!(id1 < id2)) {
        throw nlm::NLMTestException("SynapseId comparison test failed: id1 should be less than id2");
    }
    if (!(id2 > id1)) {
        throw nlm::NLMTestException("SynapseId comparison test failed: id2 should be greater than id1");
    }
    
    std::cout << "    testSynapseId passed" << std::endl;
}

void testRegionId() {
    nlm::RegionId id1(1);
    nlm::RegionId id2(2);
    
    if (id1 == id2) {
        throw nlm::NLMTestException("RegionId inequality test failed: id1 should not equal id2");
    }
    
    std::cout << "    testRegionId passed" << std::endl;
}

void testPopulationId() {
    nlm::PopulationId id1(1);
    nlm::PopulationId id2(2);
    
    if (id1 == id2) {
        throw nlm::NLMTestException("PopulationId inequality test failed: id1 should not equal id2");
    }
    
    std::cout << "    testPopulationId passed" << std::endl;
}

void testNeuronType() {
    if (nlm::NeuronType::Excitatory == nlm::NeuronType::Inhibitory) {
        throw nlm::NLMTestException("NeuronType enum test failed: Excitatory should not equal Inhibitory");
    }
    if (nlm::NeuronType::Sensory == nlm::NeuronType::Motor) {
        throw nlm::NLMTestException("NeuronType enum test failed: Sensory should not equal Motor");
    }
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testSynapseType() {
    if (nlm::SynapseType::Excitatory == nlm::SynapseType::Inhibitory) {
        throw nlm::NLMTestException("SynapseType enum test failed: Excitatory should not equal Inhibitory");
    }
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testPlasticityFlags() {
    nlm::PlasticityFlags flags;
    if (flags.hebbian) {
        throw nlm::NLMTestException("PlasticityFlags test failed: hebbian should be false initially");
    }
    if (flags.stdp) {
        throw nlm::NLMTestException("PlasticityFlags test failed: stdp should be false initially");
    }
    if (flags.reward_modulated) {
        throw nlm::NLMTestException("PlasticityFlags test failed: reward_modulated should be false initially");
    }
    
    flags.hebbian = true;
    if (!flags.hebbian) {
        throw nlm::NLMTestException("PlasticityFlags test failed: hebbian should be true after set");
    }
    
    std::cout << "    testPlasticityFlags passed" << std::endl;
}

void testSpikeEvent() {
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(1);
    event.timestamp = 0.5;
    event.step = 500;
    
    if (event.source_neuron.index() != 1) {
        throw nlm::NLMTestException("SpikeEvent test failed: source_neuron index should be 1, got " + 
                                   std::to_string(event.source_neuron.index()));
    }
    if (event.timestamp != 0.5) {
        throw nlm::NLMTestException("SpikeEvent test failed: timestamp should be 0.5, got " + 
                                   std::to_string(event.timestamp));
    }
    if (event.step != 500) {
        throw nlm::NLMTestException("SpikeEvent test failed: step should be 500, got " + 
                                   std::to_string(event.step));
    }
    
    std::cout << "    testSpikeEvent passed" << std::endl;
}

void runAll() {
    testNeuronId();
    testSynapseId();
    testRegionId();
    testPopulationId();
    testNeuronType();
    testSynapseType();
    testPlasticityFlags();
    testSpikeEvent();
}

} // namespace test_types
