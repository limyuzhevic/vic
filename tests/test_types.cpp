// Types Tests
// Enhanced with NLM Test Framework
#include "core/Types/Types.hpp"
#include <cassert>
#include <iostream>
#include <string>

// Test macros for NLM Test Framework
#define NLM_TEST_CASE_LEGACY(name, testFunc) \
    void name##TestLegacy() { testFunc; } \
    NLM_TEST_CASE(name, name##TestLegacy)

namespace test_types {

void testNeuronId() {
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::NeuronId id3(1);
    
    NLM_ASSERT_EQ(id1, id3);
    NLM_ASSERT_NE(id1, id2);
    NLM_ASSERT_LT(id1, id2);
    NLM_ASSERT_GT(id2, id1);
    NLM_ASSERT_EQ(id1.index(), 1);
    
    std::cout << "    testNeuronId passed" << std::endl;
}

void testSynapseId() {
    nlm::SynapseId id1(100);
    nlm::SynapseId id2(200);
    
    NLM_ASSERT_NE(id1, id2);
    NLM_ASSERT_LT(id1, id2);
    
    std::cout << "    testSynapseId passed" << std::endl;
}

void testRegionId() {
    nlm::RegionId id1(1);
    nlm::RegionId id2(2);
    
    NLM_ASSERT_NE(id1, id2);
    
    std::cout << "    testRegionId passed" << std::endl;
}

void testPopulationId() {
    nlm::PopulationId id1(1);
    nlm::PopulationId id2(2);
    
    NLM_ASSERT_NE(id1, id2);
    
    std::cout << "    testPopulationId passed" << std::endl;
}

void testNeuronType() {
    NLM_ASSERT_NE(nlm::NeuronType::Excitatory, nlm::NeuronType::Inhibitory);
    NLM_ASSERT_NE(nlm::NeuronType::Sensory, nlm::NeuronType::Motor);
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testSynapseType() {
    NLM_ASSERT_NE(nlm::SynapseType::Excitatory, nlm::SynapseType::Inhibitory);
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testPlasticityFlags() {
    nlm::PlasticityFlags flags;
    NLM_ASSERT_FALSE(flags.hebbian);
    NLM_ASSERT_FALSE(flags.stdp);
    NLM_ASSERT_FALSE(flags.reward_modulated);
    
    flags.hebbian = true;
    NLM_ASSERT_TRUE(flags.hebbian);
    
    std::cout << "    testPlasticityFlags passed" << std::endl;
}

void testSpikeEvent() {
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(1);
    event.timestamp = 0.5;
    event.step = 500;
    
    NLM_ASSERT_EQ(event.source_neuron.index(), 1);
    NLM_ASSERT_EQ(event.timestamp, 0.5);
    NLM_ASSERT_EQ(event.step, 500);
    
    std::cout << "    testSpikeEvent passed" << std::endl;
}

// Modern test with enhanced type features
void testTypesAdvancedFeatures() {
    // Test advanced type features with modern assertions
    nlm::NeuronId id1(10);
    nlm::NeuronId id2(20);
    
    NLM_ASSERT_LT(id1, id2);
    NLM_ASSERT_GT(id2, id1);
    
    nlm::SynapseId sid1(100);
    nlm::SynapseId sid2(200);
    NLM_ASSERT_NE(sid1, sid2);
    NLM_ASSERT_LT(sid1, sid2);
    
    nlm::RegionId rid1(5);
    nlm::RegionId rid2(10);
    NLM_ASSERT_NE(rid1, rid2);
    
    // Test type enumeration
    NLM_ASSERT_NE(nlm::NeuronType::Excitatory, nlm::NeuronType::Inhibitory);
    NLM_ASSERT_NE(nlm::SynapseType::Excitatory, nlm::SynapseType::Inhibitory);
    
    // Test plasticity flags
    nlm::PlasticityFlags flags;
    NLM_ASSERT_FALSE(flags.hebbian);
    NLM_ASSERT_FALSE(flags.stdp);
    NLM_ASSERT_FALSE(flags.reward_modulated);
    
    flags.hebbian = true;
    NLM_ASSERT_TRUE(flags.hebbian);
    
    // Test spike event
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(3);
    event.timestamp = 1.5;
    event.step = 1500;
    
    NLM_ASSERT_EQ(event.source_neuron.index(), 3);
    NLM_ASSERT_EQ(event.timestamp, 1.5);
    NLM_ASSERT_EQ(event.step, 1500);
    
    std::cout << "    testTypesAdvancedFeatures passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced Types tests..." << std::endl;
    
    testNeuronId();
    testSynapseId();
    testRegionId();
    testPopulationId();
    testNeuronType();
    testSynapseType();
    testPlasticityFlags();
    testSpikeEvent();
    testTypesAdvancedFeatures();
}

} // namespace test_types
