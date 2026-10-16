// Enhanced Types Tests with Comprehensive Coverage
// Test neural type system, IDs, and classifications

#include "nlm_test_utilities.h"
#include "core/Types/Types.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>

namespace test_types {

void testNeuronIdBasic() {
    nlm::test::TestFramework framework;
    framework.startTest("testNeuronIdBasic", nlm::test::TestCategory::Core);
    
    nlm::NeuronId id1(1);
    nlm::NeuronId id2(2);
    nlm::NeuronId id3(1);
    
    framework.assert(id1 == id3, "Neuron ID 1 should equal Neuron ID 3");
    framework.assert(id1 != id2, "Neuron ID 1 should not equal Neuron ID 2");
    framework.assert(id1 < id2, "Neuron ID 1 should be less than Neuron ID 2");
    framework.assert(id2 > id1, "Neuron ID 2 should be greater than Neuron ID 1");
    framework.assert(id1.index() == 1, "Neuron ID 1 index should be 1");
    
    std::cout << "    testNeuronIdBasic passed" << std::endl;
}

void testNeuronIdEdgeCases() {
    nlm::test::TestFramework framework;
    framework.startTest("testNeuronIdEdgeCases", nlm::test::TestCategory::Core);
    
    // Test with zero ID
    nlm::NeuronId zeroId(0);
    framework.assert(zeroId.index() == 0, "Zero Neuron ID index should be 0");
    
    // Test with maximum value
    const uint64_t maxId = UINT64_MAX;
    nlm::NeuronId maxNeuronId(maxId);
    framework.assert(maxNeuronId.index() == maxId, "Maximum Neuron ID index should equal max value");
    
    // Test equality and inequality chaining
    nlm::NeuronId id1(10);
    nlm::NeuronId id2(20);
    nlm::NeuronId id3(10);
    
    framework.assert((id1 == id3) && (id1 != id2), "Equality chaining test failed");
    framework.assert((id1 < id2) && (id2 > id1) && (id1 != id2), "Inequality chaining test failed");
    
    std::cout << "    testNeuronIdEdgeCases passed" << std::endl;
}

void testSynapseId() {
    nlm::test::TestFramework framework;
    framework.startTest("testSynapseId", nlm::test::TestCategory::Core);
    
    nlm::SynapseId id1(100);
    nlm::SynapseId id2(200);
    nlm::SynapseId id3(100);
    
    framework.assert(id1 == id3, "Synapse ID 100 should equal Synapse ID 100");
    framework.assert(id1 != id2, "Synapse ID 100 should not equal Synapse ID 200");
    framework.assert(id1 < id2, "Synapse ID 100 should be less than Synapse ID 200");
    framework.assert(id2 > id1, "Synapse ID 200 should be greater than Synapse ID 100");
    
    // Test with zero
    nlm::SynapseId zeroId(0);
    framework.assert(zeroId.index() == 0, "Synapse ID 0 index should be 0");
    
    std::cout << "    testSynapseId passed" << std::endl;
}

void testRegionId() {
    nlm::test::TestFramework framework;
    framework.startTest("testRegionId", nlm::test::TestCategory::Core);
    
    nlm::RegionId id1(1);
    nlm::RegionId id2(2);
    nlm::RegionId id3(1);
    
    framework.assert(id1 == id3, "Region ID 1 should equal Region ID 1");
    framework.assert(id1 != id2, "Region ID 1 should not equal Region ID 2");
    framework.assert(id1 < id2, "Region ID 1 should be less than Region ID 2");
    
    std::cout << "    testRegionId passed" << std::endl;
}

void testPopulationId() {
    nlm::test::TestFramework framework;
    framework.startTest("testPopulationId", nlm::test::TestCategory::Core);
    
    nlm::PopulationId id1(1);
    nlm::PopulationId id2(2);
    
    framework.assert(id1 != id2, "Population ID 1 should not equal Population ID 2");
    
    std::cout << "    testPopulationId passed" << std::endl;
}

void testNeuronTypeEnumeration() {
    nlm::test::TestFramework framework;
    framework.startTest("testNeuronTypeEnumeration", nlm::test::TestCategory::Core);
    
    // Test that different neuron types have distinct values
    framework.assert(nlm::NeuronType::Excitatory != nlm::NeuronType::Inhibitory,
                     "Excitatory and Inhibitory should be different");
    framework.assert(nlm::NeuronType::Sensory != nlm::NeuronType::Motor,
                     "Sensory and Motor should be different");
    framework.assert(nlm::NeuronType::Interneuron != nlm::NeuronType::Motor,
                     "Interneuron and Motor should be different");
    
    // Test type casting (if supported)
    std::cout << "    testNeuronTypeEnumeration passed" << std::endl;
}

void testSynapseTypeEnumeration() {
    nlm::test::TestFramework framework;
    framework.startTest("testSynapseTypeEnumeration", nlm::test::TestCategory::Core);
    
    // Test that different synapse types have distinct values
    framework.assert(nlm::SynapseType::Excitatory != nlm::SynapseType::Inhibitory,
                     "Excitatory and Inhibitory should be different");
    
    std::cout << "    testSynapseTypeEnumeration passed" << std::endl;
}

void testPlasticityFlags() {
    nlm::test::TestFramework framework;
    framework.startTest("testPlasticityFlags", nlm::test::TestCategory::Core);
    
    nlm::PlasticityFlags flags;
    
    // Test initial values
    framework.assert(!flags.hebbian, "Hebbian plasticity flag should be false initially");
    framework.assert(!flags.stdp, "STDP plasticity flag should be false initially");
    framework.assert(!flags.reward_modulated, "Reward-modulated plasticity flag should be false initially");
    
    // Test setting individual flags
    flags.hebbian = true;
    framework.assert(flags.hebbian, "Hebbian plasticity flag should be true after setting");
    
    flags.stdp = true;
    framework.assert(flags.stdp, "STDP plasticity flag should be true after setting");
    
    flags.reward_modulated = true;
    framework.assert(flags.reward_modulated, "Reward-modulated plasticity flag should be true after setting");
    
    // Test combined operations
    nlm::PlasticityFlags flags2 = flags;
    framework.assert(flags2.hebbian && flags2.stdp && flags2.reward_modulated,
                     "Copied flags should preserve all true values");
    
    std::cout << "    testPlasticityFlags passed" << std::endl;
}

void testSpikeEvent() {
    nlm::test::TestFramework framework;
    framework.startTest("testSpikeEvent", nlm::test::TestCategory::Core);
    
    nlm::SpikeEvent event;
    
    // Test initial values
    framework.assert(event.source_neuron.index() == 0, "Default source neuron should be 0");
    framework.assert(event.timestamp == 0.0f, "Default timestamp should be 0.0");
    framework.assert(event.step == 0, "Default step should be 0");
    
    // Test setting values
    event.source_neuron = nlm::NeuronId(1);
    event.timestamp = 0.5f;
    event.step = 500;
    
    framework.assert(event.source_neuron.index() == 1, "Source neuron should be 1");
    framework.assert(event.timestamp == 0.5f, "Timestamp should be 0.5");
    framework.assert(event.step == 500, "Step should be 500");
    
    std::cout << "    testSpikeEvent passed" << std::endl;
}

bool runAllTests(nlm::test::TestFramework& framework) {
    std::cout << "Running Types tests..." << std::endl;
    
    try {
        testNeuronIdBasic();
        testNeuronIdEdgeCases();
        testSynapseId();
        testRegionId();
        testPopulationId();
        testNeuronTypeEnumeration();
        testSynapseTypeEnumeration();
        testPlasticityFlags();
        testSpikeEvent();
        
        std::cout << "  PASSED" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        return false;
    }
}

} // namespace test_types
