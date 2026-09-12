// NLM Test STDP and Hebbian Learning
// Comprehensive tests for Phase 2: Real Neural Computation

#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

namespace test_stdp_hebbian {

void testSTDPLTPotentiation() {
    std::cout << "    Testing STDP LTP (pre-before-post potentiation)..." << std::endl;
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
    assert(finalWeight > initialWeight) << "STDP LTP failed: weight should increase with pre-before-post spikes";
    
    std::cout << "    PASSED (Δ=" << (finalWeight - initialWeight) << ")" << std::endl;
}

void testSTDPLTDepression() {
    std::cout << "    Testing STDP LTD (post-before-pre depression)..." << std::endl;
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
    assert(finalWeight < initialWeight) << "STDP LTD failed: weight should decrease with post-before-pre spikes";
    
    std::cout << "    PASSED (Δ=" << (finalWeight - initialWeight) << ")" << std::endl;
}

void testSTDPWeightBoundsExcitatory() {
    std::cout << "    Testing STDP weight bounds for excitatory synapses..." << std::endl;
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f) << "STDP excitatory weight exceeds max bound";
    assert(synapse.getWeight() >= -1.0f) << "STDP excitatory weight below min bound";
    
    std::cout << "    PASSED (final weight: " << synapse.getWeight() << ")" << std::endl;
}

void testSTDPWeightBoundsInhibitory() {
    std::cout << "    Testing STDP weight bounds for inhibitory synapses..." << std::endl;
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Inhibitory);
    synapse.setWeight(-0.99f);  // Near min (most negative)
    
    // Very strong depression
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 6.0, 7.0, 8.0, 9.0};  // All after pre = depression
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to min
    assert(synapse.getWeight() <= 1.0f) << "STDP inhibitory weight exceeds max bound";
    assert(synapse.getWeight() >= -1.0f) << "STDP inhibitory weight below min bound";
    
    std::cout << "    PASSED (final weight: " << synapse.getWeight() << ")" << std::endl;
}

void testSTDPTimeConstantEffects() {
    std::cout << "    Testing STDP time constant effects..." << std::endl;
    nlm::STDP stdp;
    
    // Test with default time constant (20ms)
    std::vector<nlm::Timestamp> preSpikes = {0.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0};  // 5ms delay
    
    nlm::Synapse synapse1(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse1.setType(nlm::SynapseType::Excitatory);
    synapse1.setWeight(0.5f);
    
    stdp.update(&synapse1, preSpikes, postSpikes, 0.001);
    float weight1 = synapse1.getWeight();
    
    // Change time constant and test
    stdp.setTimeConstant(10.0f);  // Shorter time constant
    nlm::Synapse synapse2(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(0.5f);
    
    stdp.update(&synapse2, preSpikes, postSpikes, 0.001);
    float weight2 = synapse2.getWeight();
    
    // Shorter time constant should produce larger potentiation for same dt
    assert(weight2 > weight1) << "STDP time constant effect failed: shorter tau should produce stronger potentiation";
    
    std::cout << "    PASSED (default tau: " << weight1 << ", short tau: " << weight2 << ")" << std::endl;
}

void testSTDPParameterConfiguration() {
    std::cout << "    Testing STDP parameter configuration..." << std::endl;
    nlm::STDP stdp;
    
    // Configure with custom parameters
    stdp.configure(0.05f, 0.06f, 30.0f);  // High LTP/LTD weights, longer time constant
    
    assert(stdp.getLTPWeight() == 0.05f) << "STDP LTP weight configuration failed";
    assert(stdp.getLTDWeight() == 0.06f) << "STDP LTD weight configuration failed";
    assert(stdp.getTimeConstant() == 30.0f) << "STDP time constant configuration failed";
    
    // Test clamping of LTP weight
    stdp.setLTPWeight(2.0f);  // Should be clamped to 1.0f
    assert(stdp.getLTPWeight() == 1.0f) << "STDP LTP weight clamping failed";
    
    // Test clamping of LTD weight
    stdp.setLTDWeight(-0.5f);  // Should be clamped to 1.0f (abs)
    assert(stdp.getLTDWeight() == 1.0f) << "STDP LTD weight clamping failed";
    
    // Test clamping of time constant
    stdp.setTimeConstant(0.5f);  // Should be clamped to 1.0f
    assert(stdp.getTimeConstant() == 1.0f) << "STDP time constant clamping failed";
    
    std::cout << "    PASSED" << std::endl;
}

void testSTDPEmptySpikeHistories() {
    std::cout << "    Testing STDP with empty spike histories..." << std::endl;
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(7), nlm::NeuronId(13), nlm::NeuronId(14));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight) << "STDP with empty pre spikes should not change weight";
    
    // Empty post spikes
    preSpikes = {1.0, 2.0};
    postSpikes.clear();
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight) << "STDP with empty post spikes should not change weight";
    
    // Both empty
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight) << "STDP with both empty spikes should not change weight";
    
    std::cout << "    PASSED" << std::endl;
}

void testSTDPSimultaneousSpikes() {
    std::cout << "    Testing STDP with simultaneous spikes..." << std::endl;
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(8), nlm::NeuronId(15), nlm::NeuronId(16));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Simultaneous spikes (dt = 0)
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {10.0, 20.0};  // Exact same timestamps
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should not change (simultaneous spikes = no change in STDP)
    assert(synapse.getWeight() == initialWeight) << "STDP with simultaneous spikes should not change weight";
    
    std::cout << "    PASSED" << std::endl;
}

void testHebbianCorrelatedFiringPotentiation() {
    std::cout << "    Testing Hebbian correlated firing potentiation..." << std::endl;
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(9), nlm::NeuronId(17), nlm::NeuronId(18));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Correlated firing in 100ms window
    std::vector<nlm::Timestamp> preSpikes = {0.0, 20.0, 50.0};
    std::vector<nlm::Timestamp> postSpikes = {10.0, 30.0, 60.0};  // All within 100ms of pre
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to correlated firing
    assert(finalWeight > initialWeight) << "Hebbian correlated firing failed: weight should increase";
    
    std::cout << "    PASSED (Δ=" << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianWeightBoundsExcitatory() {
    std::cout << "    Testing Hebbian weight bounds for excitatory synapses..." << std::endl;
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(10), nlm::NeuronId(19), nlm::NeuronId(20));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.95f);  // Near max
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    assert(synapse.getWeight() <= 1.0f) << "Hebbian excitatory weight exceeds max bound";
    assert(synapse.getWeight() >= -1.0f) << "Hebbian excitatory weight below min bound";
    
    std::cout << "    PASSED (final weight: " << synapse.getWeight() << ")" << std::endl;
}

void testHebbianWeightBoundsInhibitory() {
    std::cout << "    Testing Hebbian weight bounds for inhibitory synapses..." << std::endl;
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(11), nlm::NeuronId(21), nlm::NeuronId(22));
    synapse.setType(nlm::SynapseType::Inhibitory);
    synapse.setWeight(-0.95f);  // Near min (most negative)
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to min
    assert(synapse.getWeight() <= 1.0f) << "Hebbian inhibitory weight exceeds max bound";
    assert(synapse.getWeight() >= -1.0f) << "Hebbian inhibitory weight below min bound";
    
    std::cout << "    PASSED (final weight: " << synapse.getWeight() << ")" << std::endl;
}

void testHebbianLearningRateEffects() {
    std::cout << "    Testing Hebbian learning rate effects..." << std::endl;
    nlm::Hebbian hebbian1;
    nlm::Hebbian hebbian2;
    
    // Set different learning rates
    hebbian1.setLearningRate(0.01f);
    hebbian2.setLearningRate(0.05f);  // 5x higher
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 20.0, 40.0, 60.0, 80.0};
    std::vector<nlm::Timestamp> postSpikes = {10.0, 30.0, 50.0, 70.0, 90.0};
    
    nlm::Synapse synapse1(nlm::SynapseId(12), nlm::NeuronId(23), nlm::NeuronId(24));
    synapse1.setType(nlm::SynapseType::Excitatory);
    synapse1.setWeight(0.5f);
    
    nlm::Synapse synapse2(nlm::SynapseId(13), nlm::NeuronId(25), nlm::NeuronId(26));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(0.5f);
    
    hebbian1.update(&synapse1, preSpikes, postSpikes, 0.001);
    hebbian2.update(&synapse2, preSpikes, postSpikes, 0.001);
    
    float delta1 = synapse1.getWeight() - 0.5f;
    float delta2 = synapse2.getWeight() - 0.5f;
    
    // Higher learning rate should produce larger weight change
    assert(delta2 > delta1) << "Hebbian learning rate effect failed: higher rate should produce larger change";
    
    std::cout << "    PASSED (rate 0.01x: " << delta1 << ", rate 0.05x: " << delta2 << ")" << std::endl;
}

void testHebbianCoactivityThresholds() {
    std::cout << "    Testing Hebbian coactivity thresholds..." << std::endl;
    nlm::Hebbian hebbian;
    
    // Test with spikes far apart (>100ms window)
    nlm::Synapse synapse1(nlm::SynapseId(14), nlm::NeuronId(27), nlm::NeuronId(28));
    synapse1.setType(nlm::SynapseType::Excitatory);
    synapse1.setWeight(0.5f);
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 50.0, 100.0};
    std::vector<nlm::Timestamp> postSpikes = {200.0, 250.0, 300.0};  // All >100ms after pre
    
    hebbian.update(&synapse1, preSpikes, postSpikes, 0.001);
    float weight1 = synapse1.getWeight();
    
    // Test with spikes within 100ms window
    nlm::Synapse synapse2(nlm::SynapseId(15), nlm::NeuronId(29), nlm::NeuronId(30));
    synapse2.setType(nlm::SynapseType::Excitatory);
    synapse2.setWeight(0.5f);
    
    preSpikes = {0.0, 50.0, 100.0};
    postSpikes = {10.0, 60.0, 110.0};  // All within 100ms of pre
    
    hebbian.update(&synapse2, preSpikes, postSpikes, 0.001);
    float weight2 = synapse2.getWeight();
    
    // Within-window should produce stronger potentiation
    assert(weight2 > weight1) << "Hebbian coactivity threshold failed: within-window should be stronger";
    
    std::cout << "    PASSED (far apart: " << weight1 << ", within window: " << weight2 << ")" << std::endl;
}

void testHebbianParameterConfiguration() {
    std::cout << "    Testing Hebbian parameter configuration..." << std::endl;
    nlm::Hebbian hebbian;
    
    // Configure with custom parameters
    hebbian.setLearningRate(0.05f);
    hebbian.setMaxWeight(2.0f);
    
    assert(hebbian.getLearningRate() == 0.05f) << "Hebbian learning rate configuration failed";
    assert(hebbian.getMaxWeight() == 2.0f) << "Hebbian max weight configuration failed";
    
    // Test clamping of learning rate
    hebbian.setLearningRate(-0.5f);  // Should be clamped to 0.0f
    assert(hebbian.getLearningRate() == 0.0f) << "Hebbian learning rate clamping failed";
    
    // Test clamping of max weight
    hebbian.setLearningRate(0.1f);  // Reset
    hebbian.setMaxWeight(20.0f);   // Should be clamped to 10.0f
    assert(hebbian.getMaxWeight() == 10.0f) << "Hebbian max weight clamping failed";
    
    std::cout << "    PASSED" << std::endl;
}

void testHebbianEmptySpikeHistories() {
    std::cout << "    Testing Hebbian with empty spike histories..." << std::endl;
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(16), nlm::NeuronId(31), nlm::NeuronId(32));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight) << "Hebbian with empty pre spikes should not change weight";
    
    // Empty post spikes
    preSpikes = {1.0, 2.0};
    postSpikes.clear();
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight) << "Hebbian with empty post spikes should not change weight";
    
    // Both empty
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    assert(synapse.getWeight() == initialWeight) << "Hebbian with both empty spikes should not change weight";
    
    std::cout << "    PASSED" << std::endl;
}

void testHebbianSimultaneousSpikes() {
    std::cout << "    Testing Hebbian with simultaneous spikes..." << std::endl;
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(17), nlm::NeuronId(33), nlm::NeuronId(34));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Simultaneous spikes
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {10.0, 20.0};  // Exact same timestamps
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should change (Hebbian counts simultaneous spikes as coactivity)
    // but it's within the 100ms window, so it should potentiate
    assert(synapse.getWeight() >= initialWeight) << "Hebbian simultaneous spikes should potentiate or stay same";
    
    std::cout << "    PASSED (initial: " << initialWeight << ", final: " << synapse.getWeight() << ")" << std::endl;
}

void testSTDPHebbianInteraction() {
    std::cout << "    Testing STDP + Hebbian interaction..." << std::endl;
    nlm::STDP stdp;
    nlm::Hebbian hebbian;
    
    // Enable both plasticity rules on synapse
    nlm::Synapse synapse(nlm::SynapseId(18), nlm::NeuronId(35), nlm::NeuronId(36));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    synapse.enablePlasticity(true, true, false);  // Enable both STDP and Hebbian
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 20.0, 40.0};
    std::vector<nlm::Timestamp> postSpikes = {10.0, 30.0, 50.0};  // pre-before-post for STDP
    
    float initialWeight = synapse.getWeight();
    
    // Apply both plasticity rules
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Both rules should contribute to weight change
    // STDP should potentiate (pre-before-post), Hebbian should potentiate (correlated)
    assert(finalWeight > initialWeight) << "STDP+Hebbian interaction failed: should potentiate";
    
    std::cout << "    PASSED (initial: " << initialWeight << ", final: " << finalWeight << ")" << std::endl;
}

void testPlasticityRuleCombinations() {
    std::cout << "    Testing different plasticity rules on identical synapses..." << std::endl;
    nlm::STDP stdp;
    nlm::Hebbian hebbian;
    
    // Create three identical synapses
    std::vector<nlm::Synapse*> synapses;
    std::vector<float> initialWeights;
    
    for (int i = 0; i < 3; ++i) {
        nlm::Synapse* syn = new nlm::Synapse(nlm::SynapseId(19 + i), nlm::NeuronId(37 + i), nlm::NeuronId(38 + i));
        syn->setType(nlm::SynapseType::Excitatory);
        syn->setWeight(0.5f);
        syn->enablePlasticity(true, true, false);
        synapses.push_back(syn);
        initialWeights.push_back(0.5f);
    }
    
    std::vector<nlm::Timestamp> preSpikes = {0.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {10.0, 30.0};
    
    // Apply different combinations
    stdp.update(synapses[0], preSpikes, postSpikes, 0.001);  // STDP only
    hebbian.update(synapses[1], preSpikes, postSpikes, 0.001);  // Hebbian only
    stdp.update(synapses[2], preSpikes, postSpikes, 0.001);  // STDP again
    hebbian.update(synapses[2], preSpikes, postSpikes, 0.001);  // Plus Hebbian
    
    // All weights should have changed
    for (size_t i = 0; i < synapses.size(); ++i) {
        assert(synapses[i]->getWeight() != initialWeights[i]) << "Synapse " << i << " weight should have changed";
    }
    
    // Synapse with both rules should have larger change
    assert(synapses[2]->getWeight() > synapses[0]->getWeight() && 
           synapses[2]->getWeight() > synapses[1]->getWeight()) << "Combined plasticity should produce larger change";
    
    // Clean up
    for (auto* syn : synapses) {
        delete syn;
    }
    
    std::cout << "    PASSED" << std::endl;
}

void testRealisticNeuralSimulation() {
    std::cout << "    Testing with realistic neural simulation..." << std::endl;
    nlm::STDP stdp;
    nlm::Hebbian hebbian;
    
    // Create a small neural circuit (10 neurons, 5 synapses)
    std::vector<nlm::Synapse*> synapses;
    
    for (int i = 0; i < 5; ++i) {
        nlm::Synapse* syn = new nlm::Synapse(nlm::SynapseId(i), 
                                            nlm::NeuronId(i), 
                                            nlm::NeuronId(i + 1));
        syn->setType(nlm::SynapseType::Excitatory);
        syn->setWeight(0.1f + (static_cast<float>(rand()) / RAND_MAX) * 0.3f);  // Random initial weight
        syn->enablePlasticity(true, true, false);
        synapses.push_back(syn);
    }
    
    // Simulate realistic spike patterns
    std::vector<nlm::Timestamp> preSpikes = {0.0, 5.0, 10.0, 15.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {3.0, 8.0, 13.0, 18.0, 23.0};  // 3ms delays
    
    // Apply plasticity rules over multiple steps
    for (int step = 0; step < 10; ++step) {
        for (auto* syn : synapses) {
            // Apply both plasticity rules
            stdp.update(syn, preSpikes, postSpikes, 0.001);
            hebbian.update(syn, preSpikes, postSpikes, 0.001);
        }
    }
    
    // Check that all synapses remain within bounds
    for (auto* syn : synapses) {
        assert(syn->getWeight() >= -1.0f) << "Synapse weight below minimum";
        assert(syn->getWeight() <= 1.0f) << "Synapse weight above maximum";
    }
    
    // Clean up
    for (auto* syn : synapses) {
        delete syn;
    }
    
    std::cout << "    PASSED (all 5 synapses stable within bounds)" << std::endl;
}

void runAll() {
    std::cout << "=== NLM STDP and Hebbian Learning Tests ===" << std::endl;
    std::cout << "Testing real neural computation plasticity rules" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    try {
        testSTDPLTPotentiation();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testSTDPLTDepression();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testSTDPWeightBoundsExcitatory();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testSTDPWeightBoundsInhibitory();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testSTDPTimeConstantEffects();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testSTDPParameterConfiguration();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testSTDPEmptySpikeHistories();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testSTDPSimultaneousSpikes();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testHebbianCorrelatedFiringPotentiation();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testHebbianWeightBoundsExcitatory();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testHebbianWeightBoundsInhibitory();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testHebbianLearningRateEffects();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testHebbianCoactivityThresholds();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testHebbianParameterConfiguration();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testHebbianEmptySpikeHistories();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testHebbianSimultaneousSpikes();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testSTDPHebbianInteraction();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPlasticityRuleCombinations();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testRealisticNeuralSimulation();
    } catch (const std::exception& e) {
        std::cout << "    FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All STDP and Hebbian Tests PASSED ===" << std::endl;
        return;
    } else {
        std::cout << "=== Some STDP and Hebbian Tests FAILED ===" << std::endl;
    }
}

} // namespace test_stdp_hebbian

// Test runner for STDP and Hebbian learning
int main() {
    test_stdp_hebbian::runAll();
    return 0;
}