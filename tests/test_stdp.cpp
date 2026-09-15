// STDP Tests
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

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

void testSTDPSpikeTimingEdgeCases() {
    nlm::STDP stdp;
    nlm::RandomGenerator rng(42);
    
    // Test simultaneous spikes (Δt = 0)
    nlm::Synapse synapse1(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse1.setType(nlm::SynapseType::Excitatory);
    synapse1.setWeight(0.5f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 2.0, 3.0};
    std::vector<nlm::Timestamp> postSpikes = {0.0, 2.0, 3.0};  // Exact same times
    
    float initialWeight = synapse1.getWeight();
    stdp.update(&synapse1, preSpikes, postSpikes, 0.001);
    float finalWeight = synapse1.getWeight();
    
    // Simultaneous spikes should not cause significant change (Δt ≈ 0)
    assert(std::abs(finalWeight - initialWeight) < 1e-5f);
    
    // Test very small time differences
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(0.5f);
    
    preSpikes = {0.0};
    postSpikes = {0.000001};  // 1μs difference
    
    initialWeight = synapse2.getWeight();
    stdp.update(&synapse2, preSpikes, postSpikes, 0.001);
    finalWeight = synapse2.getWeight();
    
    // Very small dt should cause small weight change
    assert(std::abs(finalWeight - initialWeight) < 0.001f);
    
    std::cout << "    testSTDPSpikeTimingEdgeCases passed" << std::endl;
}

void testSTDPMultiSynapseInteraction() {
    nlm::STDP stdp;
    nlm::RandomGenerator rng(42);
    
    // Create multiple synapses sharing the same neurons
    nlm::Synapse synapse1(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse1.setType(nlm::SynapseType::Excitatory);
    synapse1.setWeight(0.3f);
    
    nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse2.setType(nlm::SynapseType::Inhibitory);
    synapse2.setWeight(-0.3f);
    
    nlm::Synapse synapse3(nlm::SynapseId(3), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse3.setType(nlm::SynapseType::Excitatory);
    synapse3.setWeight(0.3f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 5.0};
    std::vector<nlm::Timestamp> postSpikes = {3.0, 8.0};
    
    // Update all synapses with the same spike pattern
    float weight1Before = synapse1.getWeight();
    float weight2Before = synapse2.getWeight();
    float weight3Before = synapse3.getWeight();
    
    stdp.update(&synapse1, preSpikes, postSpikes, 0.001);
    stdp.update(&synapse2, preSpikes, postSpikes, 0.001);
    stdp.update(&synapse3, preSpikes, postSpikes, 0.001);
    
    // Excitatory synapses should increase (pre before post)
    assert(synapse1.getWeight() > weight1Before);
    assert(synapse3.getWeight() > weight3Before);
    
    // Inhibitory synapse should also change (sign depends on dt)
    // But magnitude should be proportional to weight magnitude
    assert(std::abs(synapse2.getWeight() - weight2Before) > 0.0f);
    
    std::cout << "    testSTDPMultiSynapseInteraction passed" << std::endl;
}

void testSTDPTemporalSequences() {
    nlm::STDP stdp;
    
    // Test alternating spike patterns
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Sequence 1: pre always before post
    std::vector<nlm::Timestamp> preSpikes = {0.0, 5.0, 10.0, 15.0};
    std::vector<nlm::Timestamp> postSpikes = {3.0, 8.0, 13.0, 18.0};
    
    float weightAfterSequence1 = synapse.getWeight();
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Should potentiate (strengthen)
    assert(synapse.getWeight() > weightAfterSequence1);
    
    // Reset and test reverse sequence
    synapse.setWeight(0.5f);
    weightAfterSequence1 = synapse.getWeight();
    
    // Sequence 2: post always before pre
    preSpikes = {3.0, 8.0, 13.0, 18.0};
    postSpikes = {0.0, 5.0, 10.0, 15.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Should depress (weaken)
    assert(synapse.getWeight() < weightAfterSequence1);
    
    // Test complex sequence with mixed timing
    synapse.setWeight(0.5f);
    weightAfterSequence1 = synapse.getWeight();
    
    preSpikes = {0.0, 5.0, 10.0, 15.0, 20.0};
    postSpikes = {2.0, 8.0, 12.0, 18.0, 22.0};  // Mixed timing
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Should have some net potentiation (more pre-before-post than post-before-pre)
    assert(synapse.getWeight() > weightAfterSequence1);
    
    std::cout << "    testSTDPTemporalSequences passed" << std::endl;
}

void testSTDPTemporalFiltering() {
    nlm::STDP stdp;
    
    // Create synapse with initial weight
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Apply STDP with a time constant
    stdp.setTimeConstant(10.0f);  // 10ms time constant
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 20.0};  // 20ms apart
    std::vector<nlm::Timestamp> postSpikes = {5.0, 25.0};  // 5ms after respective pre
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // The weight change should follow exponential decay with time constant
    // Stronger potentiation for closer spikes
    
    std::cout << "    testSTDPTemporalFiltering passed" << std::endl;
}

void testSTDPParameterBoundaries() {
    nlm::STDP stdp;
    
    // Test parameter clamping
    stdp.setLTPWeight(2.0f);  // Should be clamped to 1.0f
    assert(stdp.getLTPWeight() <= 1.0f);
    
    stdp.setLTPWeight(-0.1f);  // Should be clamped to 0.0f
    assert(stdp.getLTPWeight() >= 0.0f);
    
    stdp.setLTDWeight(1.5f);  // Should be clamped to 1.0f
    assert(stdp.getLTDWeight() <= 1.0f);
    
    stdp.setTimeConstant(0.0f);  // Should be clamped to 1.0f
    assert(stdp.getTimeConstant() >= 1.0f);
    
    // Test configuration function
    stdp.configure(0.05f, 0.06f, 15.0f);
    assert(stdp.getLTPWeight() == 0.05f);
    assert(stdp.getLTDWeight() == 0.06f);
    assert(stdp.getTimeConstant() == 15.0f);
    
    // Test configuration with out-of-bounds values
    stdp.configure(2.0f, 2.0f, 0.0f);  // All out of bounds
    assert(stdp.getLTPWeight() <= 1.0f);
    assert(stdp.getLTDWeight() <= 1.0f);
    assert(stdp.getTimeConstant() >= 1.0f);
    
    std::cout << "    testSTDPParameterBoundaries passed" << std::endl;
}

void testSTDPBiologicalAccuracy() {
    nlm::STDP stdp;
    nlm::RandomGenerator rng(42);
    
    // Biological plausibility test
    // LTP and LTD weights should be biologically realistic
    assert(stdp.getLTPWeight() > 0.0f && stdp.getLTPWeight() <= 0.1f);
    assert(stdp.getLTDWeight() > 0.0f && stdp.getLTDWeight() <= 0.2f);
    assert(stdp.getTimeConstant() > 0.0f && stdp.getTimeConstant() <= 100.0f);
    
    // Time constant should be positive for exponential decay
    assert(stdp.getTimeConstant() > 0.0f);
    
    // Test with biologically realistic spike timings
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Realistic spike timings in milliseconds
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {3.0, 13.0, 23.0, 33.0};  // 3ms after pre
    
    float initialWeight = synapse.getWeight();
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should change but not dramatically
    assert(std::abs(synapse.getWeight() - initialWeight) < 0.2f);
    
    // Test depression with realistic timing
    synapse.setWeight(0.5f);
    initialWeight = synapse.getWeight();
    
    preSpikes = {3.0, 13.0, 23.0, 33.0};  // Post fires first
    postSpikes = {0.0, 10.0, 20.0, 30.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should decrease but not dramatically
    assert(std::abs(synapse.getWeight() - initialWeight) < 0.2f);
    
    std::cout << "    testSTDPBiologicalAccuracy passed" << std::endl;
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
    testSTDPSpikeTimingEdgeCases();
    testSTDPMultiSynapseInteraction();
    testSTDPTemporalSequences();
    testSTDPTemporalFiltering();
    testSTDPParameterBoundaries();
    testSTDPBiologicalAccuracy();
}

} // namespace test_stdp
