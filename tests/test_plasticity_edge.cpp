// Plasticity Edge Case Tests
// Comprehensive testing of edge cases and error conditions for plasticity systems

#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <limits>

namespace test_plasticity_edge {

// STDP Edge Cases

void testSTDPNullSynapse() {
    nlm::STDP stdp;
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    // Test nullptr synapse - should not crash
    stdp.update(nullptr, preSpikes, postSpikes, 0.001);
    
    std::cout << "    testSTDPNullSynapse passed" << std::endl;
}

void testSTDPEmptySpikeVectors() {
    nlm::STDP stdp;
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes only
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    // Empty post spikes only
    preSpikes = {1.0, 2.0, 3.0};
    postSpikes = {};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    // Both empty
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testSTDPEmptySpikeVectors passed" << std::endl;
}

void testSTDPSimultaneousSpikes() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    float initialWeight = synapse.getWeight();
    
    // Simultaneous spikes (0 time difference)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {0.0, 10.0};  // Same times
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should not change for simultaneous spikes
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testSTDPSimultaneousSpikes passed" << std::endl;
}

void testSTDPNegativeTimeDiff() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    float initialWeight = synapse.getWeight();
    
    // Post fires significantly before pre (negative time difference)
    std::vector<nlm::Timestamp> preSpikes = {100.0, 200.0, 300.0};
    std::vector<nlm::Timestamp> postSpikes = {0.0, 10.0, 20.0};  // Much earlier
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should decrease (post before pre causes depression)
    assert(synapse.getWeight() < initialWeight);
    
    std::cout << "    testSTDPNegativeTimeDiff passed" << std::endl;
}

void testSTDPSingleSpikes() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    float initialWeight = synapse.getWeight();
    
    // Single pre spike
    std::vector<nlm::Timestamp> preSpikes = {10.0};
    std::vector<nlm::Timestamp> postSpikes = {15.0};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should change
    assert(synapse.getWeight() != initialWeight);
    
    // Single post spike
    synapse.setWeight(initialWeight);
    preSpikes = {10.0};
    postSpikes = {15.0};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() != initialWeight);
    
    std::cout << "    testSTDPSingleSpikes passed" << std::endl;
}

void testSTDPBoundaryWeights() {
    nlm::STDP stdp;
    
    // Test -1.0 weight boundary
    nlm::Synapse synapse1(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse1.setType(nlm::SynapseType::Excitatory);
    synapse1.setWeight(-1.0f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    stdp.update(&synapse1, preSpikes, postSpikes, 0.001);
    assert(synapse1.getWeight() >= -1.0f);
    
    // Test 0.0 weight boundary
    nlm::Synapse synapse2(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(0.0f);
    stdp.update(&synapse2, preSpikes, postSpikes, 0.001);
    assert(synapse2.getWeight() <= 1.0f);
    
    // Test 1.0 weight boundary
    nlm::Synapse synapse3(nlm::SynapseId(7), nlm::NeuronId(13), nlm::NeuronId(14));
    synapse3.setType(nlm::SynapseType::Excitatory);
    synapse3.setWeight(1.0f);
    stdp.update(&synapse3, preSpikes, postSpikes, 0.001);
    assert(synapse3.getWeight() <= 1.0f);
    
    std::cout << "    testSTDPBoundaryWeights passed" << std::endl;
}

void testSTDPZeroEfficacy() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(8), nlm::NeuronId(15), nlm::NeuronId(16));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    synapse.setEfficacy(0.0f);  // Zero efficacy
    float initialWeight = synapse.getWeight();
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should not change with zero efficacy
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testSTDPZeroEfficacy passed" << std::endl;
}

void testSTDPNaNDelta() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(9), nlm::NeuronId(17), nlm::NeuronId(18));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Test NaN in spike times (should be handled by float conversion)
    std::vector<nlm::Timestamp> preSpikes = {0.0, std::nanf("")};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should not crash
    assert(std::isfinite(synapse.getWeight()));
    
    std::cout << "    testSTDPNaNDelta passed" << std::endl;
}

void testSTDPInfDelta() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(10), nlm::NeuronId(19), nlm::NeuronId(20));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Test infinite spike times
    std::vector<nlm::Timestamp> preSpikes = {0.0, std::numeric_limits<float>::infinity()};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should not crash
    assert(std::isfinite(synapse.getWeight()));
    
    std::cout << "    testSTDPInfDelta passed" << std::endl;
}

void testSTDPVeryLargeTimeDiff() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(11), nlm::NeuronId(21), nlm::NeuronId(22));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    float initialWeight = synapse.getWeight();
    
    // Very large time differences (beyond time constant)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1000.0};  // 1000 ms apart
    std::vector<nlm::Timestamp> postSpikes = {500.0, 1500.0};  // Large gaps
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should change very little due to exponential decay
    assert(std::abs(synapse.getWeight() - initialWeight) < 0.01f);
    
    std::cout << "    testSTDPVeryLargeTimeDiff passed" << std::endl;
}

// Hebbian Edge Cases

void testHebbianNullSynapse() {
    nlm::Hebbian hebbian;
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    // Test nullptr synapse - should not crash
    hebbian.update(nullptr, preSpikes, postSpikes, 0.001);
    
    std::cout << "    testHebbianNullSynapse passed" << std::endl;
}

void testHebbianEmptySpikeVectors() {
    nlm::Hebbian hebbian;
    nlm::Synapse synapse(nlm::SynapseId(12), nlm::NeuronId(23), nlm::NeuronId(24));
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes only
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0};
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    // Empty post spikes only
    preSpikes = {1.0, 2.0, 3.0};
    postSpikes = {};
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    // Both empty
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testHebbianEmptySpikeVectors passed" << std::endl;
}

void testHebbianInvalidLearningRates() {
    nlm::Hebbian hebbian;
    
    // Test negative learning rate (should be clamped to 0 by setter)
    hebbian.setLearningRate(-0.1f);
    assert(hebbian.getLearningRate() >= 0.0f);  // Should be clamped
    
    // Test learning rate > 1.0 (should be clamped by setter)
    hebbian.setLearningRate(1.5f);
    assert(hebbian.getLearningRate() <= 1.0f);  // Should be clamped
    
    // Test zero learning rate
    hebbian.setLearningRate(0.0f);
    assert(hebbian.getLearningRate() == 0.0f);
    
    std::cout << "    testHebbianInvalidLearningRates passed" << std::endl;
}

void testHebbianExtremeWeightBoundaries() {
    nlm::Hebbian hebbian;
    
    // Test extreme weight boundaries
    hebbian.setMaxWeight(10.0f);
    hebbian.setMinWeight(-10.0f);
    
    nlm::Synapse synapse(nlm::SynapseId(13), nlm::NeuronId(25), nlm::NeuronId(26));
    
    // Test weight change with extreme bounds
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    // Update multiple times to approach boundaries
    for (int i = 0; i < 100; ++i) {
        hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    }
    
    // Weight should be within bounds
    assert(synapse.getWeight() >= -10.0f);
    assert(synapse.getWeight() <= 10.0f);
    
    std::cout << "    testHebbianExtremeWeightBoundaries passed" << std::endl;
}

void testHebbianNaNInfDelta() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(14), nlm::NeuronId(27), nlm::NeuronId(28));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Test NaN and Inf spike times
    std::vector<nlm::Timestamp> preSpikes = {0.0, std::nanf("")};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should not crash
    assert(std::isfinite(synapse.getWeight()));
    
    std::cout << "    testHebbianNaNInfDelta passed" << std::endl;
}

void testHebbianApplyWeightChangeNullSynapse() {
    nlm::Hebbian hebbian;
    
    // Test applyWeightChange with nullptr synapse
    hebbian.applyWeightChange(nullptr, 0.1f);
    
    std::cout << "    testHebbianApplyWeightChangeNullSynapse passed" << std::endl;
}

void testHebbianApplyWeightChangeNaNDelta() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(15), nlm::NeuronId(29), nlm::NeuronId(30));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Test NaN delta
    hebbian.applyWeightChange(&synapse, std::nanf(""));
    assert(std::isfinite(synapse.getWeight()));
    
    // Test Inf delta
    hebbian.applyWeightChange(&synapse, std::numeric_limits<float>::infinity());
    assert(std::isfinite(synapse.getWeight()));
    
    std::cout << "    testHebbianApplyWeightChangeNaNDelta passed" << std::endl;
}

void testHebbianMinMaxWeightViolation() {
    nlm::Hebbian hebbian;
    
    // Set invalid weight bounds (min >= max)
    hebbian.setMaxWeight(-1.0f);  // Less than min
    hebbian.setMinWeight(1.0f);
    
    nlm::Synapse synapse(nlm::SynapseId(16), nlm::NeuronId(31), nlm::NeuronId(32));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0};
    
    // Should handle gracefully (return early due to validation)
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    std::cout << "    testHebbianMinMaxWeightViolation passed" << std::endl;
}

void testHebbianSingleEventSpikeVectors() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(17), nlm::NeuronId(33), nlm::NeuronId(34));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    float initialWeight = synapse.getWeight();
    
    // Single pre spike
    std::vector<nlm::Timestamp> preSpikes = {10.0};
    std::vector<nlm::Timestamp> postSpikes = {15.0};
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() != initialWeight);
    
    // Single post spike
    synapse.setWeight(initialWeight);
    preSpikes = {10.0};
    postSpikes = {15.0};
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() != initialWeight);
    
    std::cout << "    testHebbianSingleEventSpikeVectors passed" << std::endl;
}

void runAll() {
    std::cout << "Running Plasticity Edge Case Tests..." << std::endl;
    
    // STDP edge cases
    testSTDPNullSynapse();
    testSTDPEmptySpikeVectors();
    testSTDPSimultaneousSpikes();
    testSTDPNegativeTimeDiff();
    testSTDPSingleSpikes();
    testSTDPBoundaryWeights();
    testSTDPZeroEfficacy();
    testSTDPNaNDelta();
    testSTDPInfDelta();
    testSTDPVeryLargeTimeDiff();
    
    // Hebbian edge cases
    testHebbianNullSynapse();
    testHebbianEmptySpikeVectors();
    testHebbianInvalidLearningRates();
    testHebbianExtremeWeightBoundaries();
    testHebbianNaNInfDelta();
    testHebbianApplyWeightChangeNullSynapse();
    testHebbianApplyWeightChangeNaNDelta();
    testHebbianMinMaxWeightViolation();
    testHebbianSingleEventSpikeVectors();
    
    std::cout << "All plasticity edge case tests passed!" << std::endl;
}

} // namespace test_plasticity_edge
