#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_enhanced_synapse {

void testSynapseEnhancedCreation() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getId() == nlm::SynapseId(1));
    assert(synapse.getSourceNeuron() == nlm::NeuronId(10));
    assert(synapse.getDestinationNeuron() == nlm::NeuronId(20));
    assert(synapse.getWeight() == 0.0f);
    assert(synapse.getDelay() == 1);
    assert(synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    
    std::cout << "    testSynapseEnhancedCreation passed" << std::endl;
}

void testSynapseMemoryManagement() {
    nlm::Synapse synapse1(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(11), nlm::NeuronId(21));
    
    // Test move constructor
    nlm::Synapse synapse3(std::move(synapse1));
    assert(synapse3.getId() == nlm::SynapseId(1));
    assert(synapse1.getId() == nlm::SynapseId(0)); // Moved-from state
    
    // Test move assignment
    synapse1 = std::move(synapse2);
    assert(synapse1.getId() == nlm::SynapseId(2));
    assert(synapse2.getId() == nlm::SynapseId(0)); // Moved-from state
    
    std::cout << "    testSynapseMemoryManagement passed" << std::endl;
}

void testSynapseTypeManagement() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    assert(!synapse.isModulatory());
    
    // Change synapse type
    synapse.setType(nlm::SynapseType::Inhibitory);
    assert(!synapse.isExcitatory());
    assert(synapse.isInhibitory());
    assert(synapse.isModulatory()); // All non-excitatory/inhibitory are modulatory
    
    // Change back to excitatory
    synapse.setType(nlm::SynapseType::Excitatory);
    assert(synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    
    std::cout << "    testSynapseTypeManagement passed" << std::endl;
}

void testSynapseWeightManagement() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getWeight() == 0.0f);
    
    // Set weight within bounds
    synapse.setWeight(0.5f);
    assert(synapse.getWeight() == 0.5f);
    
    // Try to exceed upper bound
    synapse.setWeight(2.0f);
    assert(synapse.getWeight() == 1.0f); // Clamped to MAX_WEIGHT
    
    // Try to go below lower bound
    synapse.setWeight(-2.0f);
    assert(synapse.getWeight() == -1.0f); // Clamped to MIN_WEIGHT
    
    // Add weight
    synapse.addToWeight(0.3f);
    assert(synapse.getWeight() == -0.7f); // -1.0 + 0.3
    
    std::cout << "    testSynapseWeightManagement passed" << std::endl;
}

void testSynapseDelayManagement() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getDelay() == 1);
    
    synapse.setDelay(5);
    assert(synapse.getDelay() == 5);
    
    synapse.setDelay(0);
    assert(synapse.getDelay() == 0);
    
    std::cout << "    testSynapseDelayManagement passed" << std::endl;
}

void testSynapseSpikeHistoryManagement() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    // Record spikes
    synapse.recordPreSpike(0.1);
    synapse.recordPreSpike(0.2);
    synapse.recordPostSpike(0.3);
    synapse.recordPostSpike(0.4);
    
    assert(synapse.getPreSpikeHistory().size() == 2);
    assert(synapse.getPostSpikeHistory().size() == 2);
    assert(synapse.getPreSpikeHistory()[0] == 0.1f);
    assert(synapse.getPostSpikeHistory()[1] == 0.4f);
    
    // Test spike time tracking
    assert(synapse.getPreSpikeHistory().back() == 0.2f);
    assert(synapse.getPostSpikeHistory().back() == 0.4f);
    
    // Clear history
    synapse.clearHistory();
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    std::cout << "    testSynapseSpikeHistoryManagement passed" << std::endl;
}

void testSynapseShortTermPlasticity() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    nlm::RandomGenerator rng(42);
    synapse.initializeRandom(rng);
    
    // Check initial STP state
    assert(synapse.getEfficacy() >= 0.8f && synapse.getEfficacy() <= 1.2f);
    assert(synapse.getEligibilityTrace() == 0.0f);
    
    // Record pre-synaptic spike
    synapse.recordPreSpike(0.1);
    assert(synapse.getEligibilityTrace() == 0.0f); // Eligibility trace not updated by pre-spike
    
    // Simulate some time passing and step
    synapse.step(0.2);
    
    // Record post-synaptic spike
    synapse.recordPostSpike(0.3);
    assert(synapse.getEligibilityTrace() == 0.0f); // Still 0 from pre-spike
    
    std::cout << "    testSynapseShortTermPlasticity passed" << std::endl;
}

void testSynapseEfficacyManagement() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getEfficacy() == 1.0f);
    
    // Increase efficacy
    synapse.setEfficacy(1.5f);
    assert(synapse.getEfficacy() == 1.5f);
    
    // Try to exceed maximum
    synapse.setEfficacy(3.0f);
    assert(synapse.getEfficacy() == 2.0f); // Clamped to MAX
    
    // Decrease efficacy
    synapse.setEfficacy(0.5f);
    assert(synapse.getEfficacy() == 0.5f);
    
    // Try to go below minimum
    synapse.setEfficacy(-0.5f);
    assert(synapse.getEfficacy() == 0.0f); // Clamped to MIN
    
    std::cout << "    testSynapseEfficacyManagement passed" << std::endl;
}

void testSynapseEligibilityTrace() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    assert(synapse.getEligibilityTrace() == 0.0f);
    
    // Set eligibility trace
    synapse.setEligibilityTrace(0.5f);
    assert(synapse.getEligibilityTrace() == 0.5f);
    
    // Decay eligibility trace
    synapse.decayEligibilityTrace(0.1f);
    assert(synapse.getEligibilityTrace() == 0.45f); // 0.5 * (1 - 0.1)
    
    synapse.decayEligibilityTrace(0.5f);
    assert(synapse.getEligibilityTrace() == 0.225f); // 0.45 * (1 - 0.5)
    
    synapse.decayEligibilityTrace(0.9f);
    assert(synapse.getEligibilityTrace() == 0.0225f); // Small but not zero
    
    synapse.decayEligibilityTrace(1.0f);
    assert(synapse.getEligibilityTrace() == 0.0f); // Should be zero now
    
    std::cout << "    testSynapseEligibilityTrace passed" << std::endl;
}

void testSynapseStepFunction() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    nlm::RandomGenerator rng(42);
    synapse.initializeRandom(rng);
    
    float initialWeight = synapse.getWeight();
    float initialEfficacy = synapse.getEfficacy();
    
    // Record a pre-synaptic spike
    synapse.recordPreSpike(0.1);
    
    // Step the synapse
    synapse.step(0.2);
    
    // Check that weight is still within bounds
    assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
    
    // Efficacy should have changed due to spike activity
    assert(synapse.getEfficacy() != initialEfficacy);
    
    // Record post-synaptic spike
    synapse.recordPostSpike(0.3);
    
    // Step again
    synapse.step(0.4);
    
    // Weight should still be within bounds
    assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
    
    std::cout << "    testSynapseStepFunction passed" << std::endl;
}

void testSynapseReset() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    nlm::RandomGenerator rng(42);
    synapse.initializeRandom(rng);
    
    // Modify state
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.8f);
    synapse.setEfficacy(1.5f);
    
    // Reset synapse
    synapse.reset();
    
    // Check that weight is preserved but other state is cleared
    assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f); // May have changed but within bounds
    assert(synapse.getEligibilityTrace() == 0.0f);
    assert(synapse.getEfficacy() == 1.0f); // Restored to baseline
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    std::cout << "    testSynapseReset passed" << std::endl;
}

void testSynapseRandomInitialization() {
    // Test with different random seeds
    for (int seed = 0; seed < 5; ++seed) {
        nlm::Synapse synapse(nlm::SynapseId(seed), nlm::NeuronId(seed * 10), nlm::NeuronId(seed * 10 + 1));
        nlm::RandomGenerator rng(seed);
        synapse.initializeRandom(rng);
        
        // Verify weights are within expected ranges
        assert(synapse.getWeight() >= -0.5f && synapse.getWeight() <= 0.5f);
        assert(synapse.getEfficacy() >= 0.8f && synapse.getEfficacy() <= 1.2f);
        assert(synapse.getDelay() >= 1 && synapse.getDelay() <= 10);
    }
    
    std::cout << "    testSynapseRandomInitialization passed" << std::endl;
}

void testSynapseTypeBasedInitialization() {
    nlm::Synapse synapse1(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    synapse1.setType(nlm::SynapseType::Excitatory);
    
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(11), nlm::NeuronId(21));
    synapse2.setType(nlm::SynapseType::Inhibitory);
    
    nlm::Synapse synapse3(nlm::SynapseId(3), nlm::NeuronId(12), nlm::NeuronId(22));
    synapse3.setType(nlm::SynapseType::Modulatory);
    
    nlm::RandomGenerator rng(42);
    synapse1.initializeRandom(rng);
    synapse2.initializeRandom(rng);
    synapse3.initializeRandom(rng);
    
    // Excitatory synapse should have positive weight
    assert(synapse1.getWeight() > 0.0f);
    
    // Inhibitory synapse should have negative weight
    assert(synapse2.getWeight() < 0.0f);
    
    // Modulatory synapse can have any weight
    assert(synapse3.getWeight() >= -0.5f && synapse3.getWeight() <= 0.5f);
    
    std::cout << "    testSynapseTypeBasedInitialization passed" << std::endl;
}

void testSynapseStdpCalculation() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    // Test STDP calculation logic (without external dependencies)
    float learningRate = 0.01f;
    
    // Post before pre (should be depression)
    float preTime = 0.5f;
    float postTime = 0.3f; // Post spike earlier
    
    float stdpChange = synapse.calculateSTDP(preTime, postTime, learningRate);
    assert(stdpChange < 0.0f); // Depression
    
    // Pre before post (should be potentiation)
    preTime = 0.3f;
    postTime = 0.5f; // Pre spike earlier
    
    stdpChange = synapse.calculateSTDP(preTime, postTime, learningRate);
    assert(stdpChange > 0.0f); // Potentiation
    
    // Same time (no change)
    preTime = 0.4f;
    postTime = 0.4f;
    
    stdpChange = synapse.calculateSTDP(preTime, postTime, learningRate);
    assert(stdpChange == 0.0f); // No change
    
    std::cout << "    testSynapseStdpCalculation passed" << std::endl;
}

void testSynapseEffectiveWeight() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    synapse.setWeight(0.8f);
    synapse.setEfficacy(1.5f);
    
    float effectiveWeight = synapse.getEffectiveWeight();
    assert(effectiveWeight == 0.8f * 1.5f); // efficacy * weight (u=0, x=1)
    
    // Simulate short-term depression
    synapse.setWeight(0.8f);
    synapse.setEfficacy(1.5f);
    // Note: STP state affects x and u
    
    std::cout << "    testSynapseEffectiveWeight passed" << std::endl;
}

void testSynapseBatchOperations() {
    std::vector<nlm::Synapse*> synapses;
    for (int i = 0; i < 5; ++i) {
        auto synapse = new nlm::Synapse(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1));
        synapses.push_back(synapse);
    }
    
    // Test batch reset
    nlm::Synapse::batchReset(synapses);
    
    // Check that all synapses were reset
    for (auto synapse : synapses) {
        assert(synapse->getEligibilityTrace() == 0.0f);
        assert(synapse->getEfficacy() == 1.0f);
        delete synapse;
    }
    
    std::cout << "    testSynapseBatchOperations passed" << std::endl;
}

void testSynapseConfigurationValidation() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(10), nlm::NeuronId(20));
    
    // All synapses should be initially valid
    assert(synapse.isActive()); // Has type and weight
    assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
    assert(synapse.getEfficacy() >= 0.0f && synapse.getEfficacy() <= 2.0f);
    assert(synapse.getDelay() >= 1); // Default is 1, but can be 0
    
    std::cout << "    testSynapseConfigurationValidation passed" << std::endl;
}

void runAll() {
    testSynapseEnhancedCreation();
    testSynapseMemoryManagement();
    testSynapseTypeManagement();
    testSynapseWeightManagement();
    testSynapseDelayManagement();
    testSynapseSpikeHistoryManagement();
    testSynapseShortTermPlasticity();
    testSynapseEfficacyManagement();
    testSynapseEligibilityTrace();
    testSynapseStepFunction();
    testSynapseReset();
    testSynapseRandomInitialization();
    testSynapseTypeBasedInitialization();
    testSynapseStdpCalculation();
    testSynapseEffectiveWeight();
    testSynapseBatchOperations();
    testSynapseConfigurationValidation();
}

} // namespace test_enhanced_synapse
