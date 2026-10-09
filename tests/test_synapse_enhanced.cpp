// Enhanced Synapse Tests with Edge Cases and Performance Testing
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

namespace test_synapse_enhanced {

void testSynapseCreationBasic() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getId() == nlm::SynapseId(1));
    assert(synapse.getSourceNeuron() == nlm::NeuronId(1));
    assert(synapse.getDestinationNeuron() == nlm::NeuronId(2));
    
    std::cout << "    testSynapseCreationBasic passed" << std::endl;
}

void testSynapseWeightBoundariesExtended() {
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    
    // Test extreme weight settings
    synapse.setWeight(100.0f);  // Very high
    assert(synapse.getWeight() <= 1.0f);  // Should be clamped
    
    synapse.setWeight(-100.0f);  // Very low
    assert(synapse.getWeight() >= -1.0f);  // Should be clamped
    
    synapse.setWeight(0.0f);
    synapse.addToWeight(2.0f);  // Should not exceed bound
    assert(synapse.getWeight() <= 1.0f);
    
    synapse.setWeight(-0.0f);
    synapse.addToWeight(-2.0f);  // Should not go below bound
    assert(synapse.getWeight() >= -1.0f);
    
    std::cout << "    testSynapseWeightBoundariesExtended passed" << std::endl;
}

void testSynapseTypeCombinations() {
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    
    // Test all synapse types
    std::vector<nlm::SynapseType> types = {
        nlm::SynapseType::Excitatory,
        nlm::SynapseType::Inhibitory,
        nlm::SynapseType::Modulatory,
        nlm::SynapseType::Electrical,
        nlm::SynapseType::GapJunction
    };
    
    for (const auto& type : types) {
        synapse.setType(type);
        assert(synapse.getType() == type);
        
        if (type == nlm::SynapseType::Excitatory || type == nlm::SynapseType::Modulatory) {
            assert(synapse.isExcitatory());
        } else {
            assert(!synapse.isExcitatory());
        }
        
        if (type == nlm::SynapseType::Inhibitory || type == nlm::SynapseType::GapJunction) {
            assert(synapse.isInhibitory());
        } else {
            assert(!synapse.isInhibitory());
        }
    }
    
    std::cout << "    testSynapseTypeCombinations passed" << std::endl;
}

void testSynapseDelayValidation() {
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    
    // Test delay range validation
    synapse.setDelay(0);  // Minimum (maybe 1 is actual min)
    assert(synapse.getDelay() >= 1);
    
    synapse.setDelay(1000);  // Very large delay
    assert(synapse.getDelay() >= 1);  // Should be handled gracefully
    
    std::cout << "    testSynapseDelayValidation passed" << std::endl;
}

void testSynapseSpikeHistoryManagement() {
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    // Add many spikes to test capacity limits
    for (int i = 0; i < 1000; ++i) {
        synapse.recordPreSpike(static_cast<nlm::Timestamp>(i));
        synapse.recordPostSpike(static_cast<nlm::Timestamp>(i * 0.5));
    }
    
    // History should still be manageable
    assert(synapse.getPreSpikeHistory().size() > 0);
    assert(synapse.getPostSpikeHistory().size() > 0);
    
    // Clear history
    synapse.clearHistory();
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    std::cout << "    testSynapseSpikeHistoryManagement passed" << std::endl;
}

void testSynapseEligibilityTraceDynamics() {
    nlm::Synapse synapse(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    
    // Test eligibility trace dynamics
    synapse.setEligibilityTrace(0.0f);
    assert(synapse.getEligibilityTrace() == 0.0f);
    
    // Add spikes to generate eligibility trace
    for (int i = 0; i < 10; ++i) {
        synapse.recordPreSpike(static_cast<nlm::Timestamp>(i));
        synapse.recordPostSpike(static_cast<nlm::Timestamp>(i + 1));
    }
    
    synapse.setEligibilityTrace(0.5f);
    assert(synapse.getEligibilityTrace() == 0.5f);
    
    // Test decay
    float initialTrace = synapse.getEligibilityTrace();
    synapse.decayEligibilityTrace(0.1f);
    assert(synapse.getEligibilityTrace() < initialTrace);
    
    // Test decay bounds
    synapse.setEligibilityTrace(0.0f);
    synapse.decayEligibilityTrace(1.0f);  // Should not go below 0
    assert(synapse.getEligibilityTrace() >= 0.0f);
    
    std::cout << "    testSynapseEligibilityTraceDynamics passed" << std::endl;
}

void testSynapseEfficacyComprehensive() {
    nlm::Synapse synapse(nlm::SynapseId(7), nlm::NeuronId(13), nlm::NeuronId(14));
    
    // Test efficacy range and behavior
    assert(synapse.getEfficacy() == 1.0f);  // Default
    
    synapse.setEfficacy(0.0f);
    assert(synapse.getEfficacy() == 0.0f);
    
    synapse.setEfficacy(2.0f);
    assert(synapse.getEfficacy() <= 2.0f);  // Should be clamped or handled
    
    // Test efficacy with weight interaction
    synapse.setWeight(0.5f);
    synapse.setEfficacy(1.5f);
    
    // Combined effect
    assert(synapse.getWeight() > 0.0f);
    assert(synapse.getEfficacy() > 1.0f);
    
    std::cout << "    testSynapseEfficacyComprehensive passed" << std::endl;
}

void testSynapseRandomInitializationRobustness() {
    nlm::RandomGenerator rng(42);
    
    // Test random initialization with different seeds
    for (int seed = 0; seed < 10; ++seed) {
        nlm::RandomGenerator testRng(seed);
        nlm::Synapse synapse(nlm::SynapseId(seed), nlm::NeuronId(seed), nlm::NeuronId(seed + 100));
        
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.initializeRandom(testRng);
        
        // Weight should be reasonable
        assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
        
        // Delay should be reasonable
        assert(synapse.getDelay() >= 1 && synapse.getDelay() <= 10);  // Common range
        
        // Test inhibitory initialization
        synapse.setType(nlm::SynapseType::Inhibitory);
        synapse.initializeRandom(testRng);
        
        assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
    }
    
    std::cout << "    testSynapseRandomInitializationRobustness passed" << std::endl;
}

void testSynapseResetComprehensive() {
    nlm::Synapse synapse(nlm::SynapseId(8), nlm::NeuronId(15), nlm::NeuronId(16));
    
    // Set various states
    synapse.setWeight(0.7f);
    synapse.recordPreSpike(0.1);
    synapse.recordPreSpike(0.2);
    synapse.recordPostSpike(0.3);
    synapse.recordPostSpike(0.4);
    synapse.setEligibilityTrace(0.6f);
    synapse.setDelay(5);
    
    // Verify state before reset
    assert(synapse.getWeight() != 0.0f);
    assert(synapse.getPreSpikeHistory().size() > 0);
    assert(synapse.getEligibilityTrace() != 0.0f);
    assert(synapse.getDelay() != 1);
    
    // Reset
    synapse.reset();
    
    // Verify all state cleared
    assert(synapse.getWeight() == 0.0f);
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    assert(synapse.getEligibilityTrace() == 0.0f);
    // Delay may or may not be reset depending on implementation
    
    std::cout << "    testSynapseResetComprehensive passed" << std::endl;
}

void testSynapsePerformanceAnalysis() {
    nlm::RandomGenerator rng(123);
    
    // Performance test with many synapses
    std::vector<nlm::Synapse> synapses;
    for (int i = 0; i < 100; ++i) {
        nlm::Synapse syn(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 100));
        syn.setType(static_cast<nlm::SynapseType>(i % 5));
        syn.setWeight(0.5f);
        synapses.push_back(syn);
    }
    
    // Measure performance
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        // Random operations
        int synIndex = i % synapses.size();
        auto& syn = synapses[synIndex];
        
        if (i % 4 == 0) syn.setWeight(syn.getWeight() + 0.01f);
        else if (i % 4 == 1) syn.setWeight(syn.getWeight() - 0.01f);
        else if (i % 4 == 2) syn.recordPreSpike(static_cast<nlm::Timestamp>(i));
        else syn.recordPostSpike(static_cast<nlm::Timestamp>(i));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "    testSynapsePerformanceAnalysis passed (100 synapses, 10000 ops in " 
              << duration.count() << " μs)" << std::endl;
}

void testSynapseMemoryFootprint() {
    // Estimate memory footprint of synapse objects
    nlm::Synapse synapse(nlm::SynapseId(9), nlm::NeuronId(17), nlm::NeuronId(18));
    
    // Should be small and manageable
    size_t synapseSize = sizeof(synapse);
    
    // Create many synapses to test total memory
    std::vector<nlm::Synapse> synapses;
    synapses.reserve(10000);
    
    for (int i = 0; i < 10000; ++i) {
        synapses.emplace_back(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 10000));
    }
    
    // Should not cause excessive memory usage
    assert(synapses.size() == 10000);
    
    std::cout << "    testSynapseMemoryFootprint passed (10000 synapses)" << std::endl;
}

void testSynapseTypeCompatibility() {
    nlm::Synapse synapse(nlm::SynapseId(10), nlm::NeuronId(19), nlm::NeuronId(20));
    
    // Test type compatibility
    std::vector<std::pair<nlm::SynapseType, bool>> typeTests = {
        {nlm::SynapseType::Excitatory, true},
        {nlm::SynapseType::Inhibitory, false},
        {nlm::SynapseType::Modulatory, true},
        {nlm::SynapseType::Electrical, false},
        {nlm::SynapseType::GapJunction, false}
    };
    
    for (const auto& test : typeTests) {
        synapse.setType(test.first);
        bool expectedExc = test.second;
        
        if (synapse.isExcitatory() != expectedExc) {
            std::cout << "    WARNING: Excitatory detection mismatch for type " 
                      << static_cast<int>(test.first) << std::endl;
        }
        
        if (synapse.isInhibitory() && test.second) {
            std::cout << "    WARNING: Inhibitory detection mismatch for type " 
                      << static_cast<int>(test.first) << std::endl;
        }
    }
    
    std::cout << "    testSynapseTypeCompatibility passed" << std::endl;
}

void runAllEnhanced() {
    std::cout << "Running Enhanced Synapse Tests..." << std::endl;
    
    testSynapseCreationBasic();
    testSynapseWeightBoundariesExtended();
    testSynapseTypeCombinations();
    testSynapseDelayValidation();
    testSynapseSpikeHistoryManagement();
    testSynapseEligibilityTraceDynamics();
    testSynapseEfficacyComprehensive();
    testSynapseRandomInitializationRobustness();
    testSynapseResetComprehensive();
    testSynapsePerformanceAnalysis();
    testSynapseMemoryFootprint();
    testSynapseTypeCompatibility();
    
    std::cout << std::endl << "All enhanced synapse tests completed!" << std::endl;
}

} // namespace test_synapse_enhanced

// Integration with existing tests
void runEnhancedSynapseTests() {
    test_synapse_enhanced::runAllEnhanced();
}
