// STDP Tests
// Enhanced with NLM Test Framework
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <string>

// Test macros for NLM Test Framework
#define NLM_TEST_CASE_LEGACY(name, testFunc) \
    void name##TestLegacy() { testFunc; } \
    NLM_TEST_CASE(name, name##TestLegacy)

namespace test_stdp {

void testSTDPCreation() {
    nlm::STDP stdp;
    
    NLM_ASSERT_EQ(stdp.getLTPWeight(), 0.01f);
    NLM_ASSERT_EQ(stdp.getLTDWeight(), 0.012f);
    NLM_ASSERT_EQ(stdp.getTimeConstant(), 20.0f);
    
    std::cout << "    testSTDPCreation passed" << std::endl;
}

void testSTDPConfiguration() {
    nlm::STDP stdp;
    
    stdp.setLTPWeight(0.05f);
    stdp.setLTDWeight(0.06f);
    stdp.setTimeConstant(30.0f);
    
    NLM_ASSERT_EQ(stdp.getLTPWeight(), 0.05f);
    NLM_ASSERT_EQ(stdp.getLTDWeight(), 0.06f);
    NLM_ASSERT_EQ(stdp.getTimeConstant(), 30.0f);
    
    std::cout << "    testSTDPConfiguration passed" << std::endl;
}

void testSTDPConfigure() {
    nlm::STDP stdp;
    
    stdp.configure(0.03f, 0.035f, 25.0f);
    
    NLM_ASSERT_EQ(stdp.getLTPWeight(), 0.03f);
    NLM_ASSERT_EQ(stdp.getLTDWeight(), 0.035f);
    NLM_ASSERT_EQ(stdp.getTimeConstant(), 25.0f);
    
    std::cout << "    testSTDPConfigure passed" << std::endl;
}

void testSTDPName() {
    nlm::STDP stdp;
    
    NLM_ASSERT_NOT_NULL(stdp.getName());
    NLM_ASSERT_EQ(std::string(stdp.getName()), "STDP");
    
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
    NLM_ASSERT_GT(finalWeight, initialWeight);
    
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
    NLM_ASSERT_LT(finalWeight, initialWeight);
    
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
    NLM_ASSERT_EQ(synapse.getWeight(), initialWeight);
    
    // Empty post spikes - should not change
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    NLM_ASSERT_EQ(synapse.getWeight(), initialWeight);
    
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
    NLM_ASSERT_LE(synapse.getWeight(), 1.0f);
    
    std::cout << "    testSTDPWeightBounds passed" << std::endl;
}

// Modern test with enhanced STDP features
void testSTDPEnhancedFeatures() {
    nlm::STDP stdp;
    
    // Test enhanced STDP features
    stdp.configure(0.02f, 0.025f, 40.0f);
    
    NLM_ASSERT_EQ(stdp.getLTPWeight(), 0.02f);
    NLM_ASSERT_EQ(stdp.getLTDWeight(), 0.025f);
    NLM_ASSERT_EQ(stdp.getTimeConstant(), 40.0f);
    
    // Test name
    NLM_ASSERT_NOT_NULL(stdp.getName());
    NLM_ASSERT_TRUE(std::string(stdp.getName()).find("STDP") != std::string::npos);
    
    // Test configuration
    stdp.setLTPWeight(0.01f);
    NLM_ASSERT_EQ(stdp.getLTPWeight(), 0.01f);
    
    std::cout << "    testSTDPEnhancedFeatures passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced STDP tests..." << std::endl;
    
    testSTDPCreation();
    testSTDPConfiguration();
    testSTDPConfigure();
    testSTDPName();
    testSTDPPotentiation();
    testSTDPDepression();
    testSTDPEmptySpikes();
    testSTDPWeightBounds();
    testSTDPEnhancedFeatures();
}

} // namespace test_stdp
