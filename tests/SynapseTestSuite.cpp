// Synapse Test Suite Implementation
// Implementation of the SynapseTestSuite tests

#include "SynapseTestSuite.hpp"
#include "core/Random/Random.hpp"
#include <numeric>
#include <algorithm>

namespace test_synapse {

void SynapseTestSuite::runAllTests() {
    TestRunner::getInstance().addSuite(std::make_unique<SynapseTestSuite>());
}

std::string SynapseTestSuite::getName() const {
    return "Synapse Tests";
}

void SynapseTestSuite::registerTests() {
    // Basic functionality tests
    addTest(std::bind(&SynapseTestSuite::testSynapseCreation, this), "Synapse Creation");
    addTest(std::bind(&SynapseTestSuite::testSynapseWeight, this), "Synapse Weight");
    addTest(std::bind(&SynapseTestSuite::testSynapseType, this), "Synapse Type");
    addTest(std::bind(&SynapseTestSuite::testSynapseDelay, this), "Synapse Delay");
    
    // Advanced functionality tests
    addTest(std::bind(&SynapseTestSuite::testSynapseWeightBounds, this), "Synapse Weight Bounds");
    addTest(std::bind(&SynapseTestSuite::testSynapseSpikeHistory, this), "Synapse Spike History");
    addTest(std::bind(&SynapseTestSuite::testSynapseEligibilityTrace, this), "Synapse Eligibility Trace");
    addTest(std::bind(&SynapseTestSuite::testSynapseEfficacy, this), "Synapse Efficacy");
    addTest(std::bind(&SynapseTestSuite::testSynapseReset, this), "Synapse Reset");
    addTest(std::bind(&SynapseTestSuite::testSynapsePlasticityFlags, this), "Synapse Plasticity Flags");
    
    // Edge case and stress tests
    addTest(std::bind(&SynapseTestSuite::testSynapseRandomInitialization, this), "Synapse Random Initialization");
    addTest(std::bind(&SynapseTestSuite::testSynapseLargeWeight, this), "Synapse Large Weight");
    addTest(std::bind(&SynapseTestSuite::testSynapseNegativeWeight, this), "Synapse Negative Weight");
    addTest(std::bind(&SynapseTestSuite::testSynapseLargeDelay, this), "Synapse Large Delay");
    addTest(std::bind(&SynapseTestSuite::testSynapseMemoryManagement, this), "Synapse Memory Management");
}

bool SynapseTestSuite::testSynapseCreation() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    ASSERT_TRUE(synapse.getId() == nlm::SynapseId(1), "Synapse ID should match");
    ASSERT_TRUE(synapse.getSourceNeuron() == nlm::NeuronId(1), "Source neuron should match");
    ASSERT_TRUE(synapse.getDestinationNeuron() == nlm::NeuronId(2), "Destination neuron should match");
    
    return true;
}

bool SynapseTestSuite::testSynapseWeight() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    ASSERT_TRUE(synapse.getWeight() == 0.0f, "Default weight should be 0.0");
    
    synapse.setWeight(0.5f);
    ASSERT_TRUE(synapse.getWeight() == 0.5f, "Weight should be 0.5");
    
    synapse.addToWeight(0.2f);
    ASSERT_TRUE(synapse.getWeight() == 0.7f, "Weight should be 0.7 after addition");
    
    synapse.addToWeight(-0.3f);
    ASSERT_TRUE(synapse.getWeight() == 0.4f, "Weight should be 0.4 after subtraction");
    
    return true;
}

bool SynapseTestSuite::testSynapseWeightBounds() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setWeight(10.0f);
    ASSERT_TRUE(synapse.getWeight() <= 1.0f, "Weight should be clamped to max 1.0");
    
    synapse.setWeight(-10.0f);
    ASSERT_TRUE(synapse.getWeight() >= -1.0f, "Weight should be clamped to min -1.0");
    
    return true;
}

bool SynapseTestSuite::testSynapseType() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    ASSERT_TRUE(synapse.getType() == nlm::SynapseType::Excitatory, "Type should be Excitatory");
    ASSERT_TRUE(synapse.isExcitatory(), "Should be excitatory");
    ASSERT_TRUE(!synapse.isInhibitory(), "Should not be inhibitory");
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    ASSERT_TRUE(synapse.getType() == nlm::SynapseType::Inhibitory, "Type should be Inhibitory");
    ASSERT_TRUE(synapse.isInhibitory(), "Should be inhibitory");
    ASSERT_TRUE(!synapse.isExcitatory(), "Should not be excitatory");
    
    return true;
}

bool SynapseTestSuite::testSynapseDelay() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    ASSERT_TRUE(synapse.getDelay() == 1, "Default delay should be 1");
    
    synapse.setDelay(5);
    ASSERT_TRUE(synapse.getDelay() == 5, "Delay should be 5");
    
    return true;
}

bool SynapseTestSuite::testSynapseSpikeHistory() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    ASSERT_TRUE(synapse.getPreSpikeHistory().empty(), "Pre-spike history should be empty");
    ASSERT_TRUE(synapse.getPostSpikeHistory().empty(), "Post-spike history should be empty");
    
    synapse.recordPreSpike(0.1);
    synapse.recordPreSpike(0.2);
    synapse.recordPostSpike(0.15);
    
    ASSERT_TRUE(synapse.getPreSpikeHistory().size() == 2, "Should have 2 pre-spikes");
    ASSERT_TRUE(synapse.getPostSpikeHistory().size() == 1, "Should have 1 post-spike");
    
    synapse.clearHistory();
    ASSERT_TRUE(synapse.getPreSpikeHistory().empty(), "Pre-spike history should be empty after clear");
    ASSERT_TRUE(synapse.getPostSpikeHistory().empty(), "Post-spike history should be empty after clear");
    
    return true;
}

bool SynapseTestSuite::testSynapseEligibilityTrace() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    ASSERT_TRUE(synapse.getEligibilityTrace() == 0.0f, "Default eligibility trace should be 0.0");
    
    synapse.setEligibilityTrace(0.5f);
    ASSERT_TRUE(synapse.getEligibilityTrace() == 0.5f, "Eligibility trace should be 0.5");
    
    synapse.decayEligibilityTrace(0.1f);
    ASSERT_TRUE(synapse.getEligibilityTrace() < 0.5f, "Eligibility trace should decay");
    
    return true;
}

bool SynapseTestSuite::testSynapseEfficacy() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    ASSERT_TRUE(synapse.getEfficacy() == 1.0f, "Default efficacy should be 1.0");
    
    synapse.setEfficacy(0.5f);
    ASSERT_TRUE(synapse.getEfficacy() == 0.5f, "Efficacy should be 0.5");
    
    synapse.setEfficacy(5.0f);
    ASSERT_TRUE(synapse.getEfficacy() <= 2.0f, "Efficacy should be clamped to max 2.0");
    
    return true;
}

bool SynapseTestSuite::testSynapseReset() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setWeight(0.5f);
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.3f);
    
    synapse.reset();
    
    ASSERT_TRUE(synapse.getWeight() == 0.0f, "Weight should be reset to 0.0");
    ASSERT_TRUE(synapse.getPreSpikeHistory().empty(), "Pre-spike history should be empty");
    ASSERT_TRUE(synapse.getPostSpikeHistory().empty(), "Post-spike history should be empty");
    ASSERT_TRUE(synapse.getEligibilityTrace() == 0.0f, "Eligibility trace should be 0.0");
    
    return true;
}

bool SynapseTestSuite::testSynapsePlasticityFlags() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    ASSERT_TRUE(!synapse.getPlasticityFlags().hebbian, "Hebbian plasticity should be disabled by default");
    ASSERT_TRUE(!synapse.getPlasticityFlags().stdp, "STDP plasticity should be disabled by default");
    
    synapse.enablePlasticity(true, true, false);
    
    ASSERT_TRUE(synapse.getPlasticityFlags().hebbian, "Hebbian plasticity should be enabled");
    ASSERT_TRUE(synapse.getPlasticityFlags().stdp, "STDP plasticity should be enabled");
    ASSERT_TRUE(!synapse.getPlasticityFlags().reward_modulated, "Reward-modulated plasticity should be disabled");
    
    return true;
}

bool SynapseTestSuite::testSynapseRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.initializeRandom(rng);
    
    ASSERT_TRUE(synapse.getWeight() > 0.0f, "Excitatory synapse weight should be positive");
    
    ASSERT_TRUE(synapse.getDelay() >= 1 && synapse.getDelay() <= 5, "Delay should be in valid range");
    
    return true;
}

bool SynapseTestSuite::testSynapseLargeWeight() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setWeight(100.0f);
    ASSERT_TRUE(synapse.getWeight() <= 1.0f, "Large weight should be clamped to max 1.0");
    
    return true;
}

bool SynapseTestSuite::testSynapseNegativeWeight() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setWeight(-100.0f);
    ASSERT_TRUE(synapse.getWeight() >= -1.0f, "Negative weight should be clamped to min -1.0");
    
    return true;
}

bool SynapseTestSuite::testSynapseLargeDelay() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setDelay(1000);
    ASSERT_TRUE(synapse.getDelay() > 0, "Delay should be positive");
    
    return true;
}

bool SynapseTestSuite::testSynapseMemoryManagement() {
    {
        nlm::Synapse synapse1(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
        
        ASSERT_TRUE(synapse1.getId() != synapse2.getId(), "Synapse IDs should be different");
    }
    
    return true;
}

} // namespace test_synapse