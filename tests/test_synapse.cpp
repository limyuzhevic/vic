// Synapse Tests
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>

namespace test_synapse {

void testSynapseCreation() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getId() == nlm::SynapseId(1));
    assert(synapse.getSourceNeuron() == nlm::NeuronId(10));
    assert(synapse.getDestinationNeuron() == nlm::NeuronId(20));
    assert(synapse.getType() == nlm::SynapseType::Excitatory);
    assert(synapse.getWeight() == 0.0f);
    assert(synapse.getDelay() == 1);  // Default delay
    assert(synapse.getEligibilityTrace() == 0.0f);
    assert(synapse.getEfficacy() == 1.0f);
    
    std::cout << "    testSynapseCreation passed" << std::endl;
}

void testSynapseWeight() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getWeight() == 0.0f);
    
    synapse.setWeight(0.5f);
    assert(synapse.getWeight() == 0.5f);
    
    synapse.addToWeight(0.3f);
    assert(synapse.getWeight() == 0.8f);
    
    synapse.addToWeight(-1.0f);  // Try to go below minimum
    assert(synapse.getWeight() == -1.0f);  // Should be clamped to minimum
    
    synapse.addToWeight(5.0f);  // Try to go above maximum
    assert(synapse.getWeight() == 1.0f);  // Should be clamped to maximum
    
    std::cout << "    testSynapseWeight passed" << std::endl;
}

void testSynapseType() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    assert(!synapse.isExcitatory());
    assert(synapse.isInhibitory());
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testSynapseDelay() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getDelay() == 1);  // Default delay
    
    synapse.setDelay(5);
    assert(synapse.getDelay() == 5);
    
    std::cout << "    testSynapseDelay passed" << std::endl;
}

void testSynapseSpikeRecording() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    synapse.recordPreSpike(100.0f);
    assert(synapse.getPreSpikeHistory().size() == 1);
    assert(synapse.getPreSpikeHistory()[0] == 100.0f);
    
    synapse.recordPostSpike(200.0f);
    assert(synapse.getPostSpikeHistory().size() == 1);
    assert(synapse.getPostSpikeHistory()[0] == 200.0f);
    
    // Test spike history limiting
    for (int i = 0; i < 105; ++i) {
        synapse.recordPreSpike(static_cast<float>(i));
    }
    // Should be limited to MAX_SPIKE_HISTORY (100)
    assert(synapse.getPreSpikeHistory().size() == 100);
    
    synapse.clearHistory();
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    std::cout << "    testSynapseSpikeRecording passed" << std::endl;
}

void testSynapsePlasticityFlags() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    // Default plasticity flags
    auto flags = synapse.getPlasticityFlags();
    assert(flags.hebbian == false);
    assert(flags.stdp == false);
    assert(flags.reward_modulated == false);
    
    synapse.enablePlasticity(true, true, true);
    flags = synapse.getPlasticityFlags();
    assert(flags.hebbian == true);
    assert(flags.stdp == true);
    assert(flags.reward_modulated == true);
    
    std::cout << "    testSynapsePlasticityFlags passed" << std::endl;
}

void testSynapseEligibilityTrace() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getEligibilityTrace() == 0.0f);
    
    synapse.setEligibilityTrace(0.5f);
    assert(synapse.getEligibilityTrace() == 0.5f);
    
    synapse.decayEligibilityTrace(0.1f);
    assert(synapse.getEligibilityTrace() > 0.0f);
    
    synapse.setEligibilityTrace(0.0f);
    synapse.decayEligibilityTrace(0.5f);
    assert(synapse.getEligibilityTrace() == 0.0f);  // Should stay at 0
    
    std::cout << "    testSynapseEligibilityTrace passed" << std::endl;
}

void testSynapseEfficacy() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getEfficacy() == 1.0f);
    
    synapse.setEfficacy(1.5f);
    assert(synapse.getEfficacy() == 1.5f);
    
    synapse.setEfficacy(0.0f);
    assert(synapse.getEfficacy() == 0.0f);
    
    synapse.setEfficacy(-1.0f);  // Try below minimum
    assert(synapse.getEfficacy() == 0.0f);  // Should be clamped to minimum
    
    synapse.setEfficacy(10.0f);  // Try above maximum
    assert(synapse.getEfficacy() == 2.0f);  // Should be clamped to maximum
    
    std::cout << "    testSynapseEfficacy passed" << std::endl;
}

void runAll() {
    testSynapseCreation();
    testSynapseWeight();
    testSynapseType();
    testSynapseDelay();
    testSynapseSpikeRecording();
    testSynapsePlasticityFlags();
    testSynapseEligibilityTrace();
    testSynapseEfficacy();
}

} // namespace test_synapse
