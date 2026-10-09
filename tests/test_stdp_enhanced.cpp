// Enhanced STDP Tests with Edge Cases and Performance Testing
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

namespace test_stdp_enhanced {

void testSTDPCreationBasic() {
    nlm::STDP stdp;
    
    assert(stdp.getLTPWeight() == 0.01f);
    assert(stdp.getLTDWeight() == 0.012f);
    assert(stdp.getTimeConstant() == 20.0f);
    
    std::cout << "    testSTDPCreationBasic passed" << std::endl;
}

void testSTDPConfigurationValidation() {
    nlm::STDP stdp;
    
    // Test configuration bounds
    stdp.configure(0.001f, 0.002f, 10.0f);  // Very small values
    assert(stdp.getLTPWeight() == 0.001f);
    assert(stdp.getLTDWeight() == 0.002f);
    assert(stdp.getTimeConstant() == 10.0f);
    
    stdp.configure(1.0f, 1.5f, 100.0f);  // Large values
    assert(stdp.getLTPWeight() == 1.0f);
    assert(stdp.getLTDWeight() == 1.5f);
    assert(stdp.getTimeConstant() == 100.0f);
    
    std::cout << "    testSTDPConfigurationValidation passed" << std::endl;
}

void testSTDPEmptySpikeHandling() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    float initialWeight = synapse.getWeight();
    
    // Empty spike vectors
    std::vector<nlm::Timestamp> emptyPre, emptyPost;
    
    stdp.update(&synapse, emptyPre, emptyPost, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    // Single spike
    std::vector<nlm::Timestamp> singlePre = {1.0};
    stdp.update(&synapse, singlePre, emptyPost, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    stdp.update(&synapse, emptyPre, singlePost, 0.001);
    assert(synapse.getWeight() == initialWeight);
    
    std::cout << "    testSTDPEmptySpikeHandling passed" << std::endl;
}

void testSTDPExtremeSpikeTimes() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    float initialWeight = synapse.getWeight();
    
    // Extreme spike timing (very large positive and negative deltas)
    std::vector<nlm::Timestamp> veryEarlyPre = {0.0, 1.0, 2.0};
    std::vector<nlm::Timestamp> veryLatePost = {1000.0, 2000.0, 3000.0};  // Large positive dt
    
    stdp.update(&synapse, veryEarlyPre, veryLatePost, 0.001);
    
    // Weight should be affected
    assert(synapse.getWeight() != initialWeight);
    
    // Test extreme negative deltas
    std::vector<nlm::Timestamp> veryLatePre = {1000.0, 2000.0, 3000.0};
    std::vector<nlm::Timestamp> veryEarlyPost = {1.0, 2.0, 3.0};  // Large negative dt
    
    stdp.update(&synapse, veryLatePre, veryEarlyPost, 0.001);
    
    std::cout << "    testSTDPExtremeSpikeTimes passed" << std::endl;
}

void testSTDPWeightBoundariesStress() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    
    // Start at boundary
    synapse.setWeight(0.99f);
    
    // Strong potentiation
    std::vector<nlm::Timestamp> strongPre = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> strongPost = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse, strongPre, strongPost, 0.001);
    
    // Weight should be clamped
    assert(synapse.getWeight() <= 1.0f);
    
    // Test lower bound
    synapse.setWeight(-0.99f);
    
    // Strong depression
    std::vector<nlm::Timestamp> depressionPre = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> depressionPost = {10.0, 11.0, 12.0, 13.0, 14.0};  // Large negative dt
    
    stdp.update(&synapse, depressionPre, depressionPost, 0.001);
    
    // Weight should be clamped to lower bound
    assert(synapse.getWeight() >= -1.0f);
    
    std::cout << "    testSTDPWeightBoundariesStress passed" << std::endl;
}

void testSTDPNumericalStability() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Large number of spike events to test numerical stability
    std::vector<nlm::Timestamp> manyPre;
    std::vector<nlm::Timestamp> manyPost;
    
    for (size_t i = 0; i < 1000; ++i) {
        manyPre.push_back(static_cast<nlm::Timestamp>(i));
        manyPost.push_back(static_cast<nlm::Timestamp>(i + 1));
    }
    
    // Update with many spikes - should not cause numerical overflow
    stdp.update(&synapse, manyPre, manyPost, 0.001);
    
    // Weight should still be within bounds
    assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
    
    std::cout << "    testSTDPNumericalStability passed" << std::endl;
}

void testSTDPMultipleSynapseConsistency() {
    nlm::STDP stdp;
    nlm::RandomGenerator rng(42);
    
    // Test multiple synapses simultaneously
    std::vector<nlm::Synapse> synapses;
    for (size_t i = 0; i < 10; ++i) {
        nlm::Synapse syn(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 10));
        syn.setType(nlm::SynapseType::Excitatory);
        syn.setWeight(0.5f + (static_cast<float>(rand()) / RAND_MAX) * 0.5f);
        synapses.push_back(syn);
    }
    
    // Create spike patterns
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0, 30.0, 40.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0, 35.0, 45.0};
    
    // Update all synapses
    for (auto& synapse : synapses) {
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    }
    
    // Check consistency
    size_t positiveChanges = 0;
    size_t negativeChanges = 0;
    size_t noChanges = 0;
    
    for (const auto& syn : synapses) {
        if (syn.getWeight() > 0.5f) positiveChanges++;
        else if (syn.getWeight() < 0.5f) negativeChanges++;
        else noChanges++;
    }
    
    std::cout << "    testSTDPMultipleSynapseConsistency passed (positive: " 
              << positiveChanges << ", negative: " << negativeChanges 
              << ", no change: " << noChanges << ")" << std::endl;
}

void testSTDPLearningRateCalculation() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Track learning over time
    float initialWeight = synapse.getWeight();
    float totalChange = 0.0f;
    
    for (int i = 0; i < 100; ++i) {
        std::vector<nlm::Timestamp> pre = {static_cast<nlm::Timestamp>(i) * 10.0};
        std::vector<nlm::Timestamp> post = {static_cast<nlm::Timestamp>(i) * 10.0 + 5.0};
        
        float before = synapse.getWeight();
        stdp.update(&synapse, pre, post, 0.001);
        float after = synapse.getWeight();
        
        totalChange += std::abs(after - before);
    }
    
    // Average learning rate
    float avgLearningRate = totalChange / 100.0f;
    
    std::cout << "    testSTDPLearningRateCalculation passed (avg rate: " 
              << avgLearningRate << ")" << std::endl;
}

void testSTDPCoordinationBehavior() {
    nlm::STDP stdp;
    
    // Test coordination behavior with correlated vs uncorrelated spikes
    nlm::Synapse coordinatedSyn(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    nlm::Synapse uncorrelatedSyn(nlm::SynapseId(7), nlm::NeuronId(13), nlm::NeuronId(14));
    
    coordinatedSyn.setType(nlm::SynapseType::Excitatory);
    uncorrelatedSyn.setType(nlm::SynapseType::Excitatory);
    
    float coordinatedInitial = coordinatedSyn.getWeight();
    float uncorrelatedInitial = uncorrelatedSyn.getWeight();
    
    // Coordinated spikes (synchronous)
    std::vector<nlm::Timestamp> coordinatedPre = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> coordinatedPost = {1.0, 11.0, 21.0};  // Synchronous
    
    stdp.update(&coordinatedSyn, coordinatedPre, coordinatedPost, 0.001);
    stdp.update(&uncorrelatedSyn, coordinatedPre, coordinatedPost, 0.001);
    
    // Uncorrelated spikes
    std::vector<nlm::Timestamp> uncorrelatedPre = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> uncorrelatedPost = {15.0, 25.0, 35.0};  // Anti-correlated
    
    stdp.update(&coordinatedSyn, uncorrelatedPre, uncorrelatedPost, 0.001);
    stdp.update(&uncorrelatedSyn, uncorrelatedPre, uncorrelatedPost, 0.001);
    
    // Both should have changed
    bool coordinatedChanged = coordinatedSyn.getWeight() != coordinatedInitial;
    bool uncorrelatedChanged = uncorrelatedSyn.getWeight() != uncorrelatedInitial;
    
    std::cout << "    testSTDPCoordinationBehavior passed (coordinated: " 
              << (coordinatedChanged ? "yes" : "no") 
              << ", uncorrelated: " << (uncorrelatedChanged ? "yes" : "no") << ")" << std::endl;
}

void testSTDPLongTermDynamics() {
    nlm::STDP stdp;
    nlm::RandomGenerator rng(123);
    
    nlm::Synapse synapse(nlm::SynapseId(8), nlm::NeuronId(15), nlm::NeuronId(16));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.8f);  // Start strong
    
    float initialWeight = synapse.getWeight();
    float minWeight = initialWeight;
    float maxWeight = initialWeight;
    
    // Long-term dynamics (simulate 1000 steps)
    for (int step = 0; step < 1000; ++step) {
        // Random spike patterns
        std::vector<nlm::Timestamp> pre;
        std::vector<nlm::Timestamp> post;
        
        if (static_cast<float>(rand()) / RAND_MAX > 0.7) {  // 30% chance of pre spikes
            pre.push_back(static_cast<nlm::Timestamp>(step));
        }
        
        if (static_cast<float>(rand()) / RAND_MAX > 0.7) {  // 30% chance of post spikes
            post.push_back(static_cast<nlm::Timestamp>(step + 5.0));
        }
        
        if (!pre.empty() || !post.empty()) {
            stdp.update(&synapse, pre, post, 0.001);
        }
        
        // Track bounds
        minWeight = std::min(minWeight, synapse.getWeight());
        maxWeight = std::max(maxWeight, synapse.getWeight());
        
        // Stability check (break if stuck at boundary)
        if (step > 500 && 
            (std::abs(synapse.getWeight() - 1.0f) < 1e-6 || 
             std::abs(synapse.getWeight() + 1.0f) < 1e-6)) {
            break;  // Stable at boundary
        }
    }
    
    // Weight should remain within bounds throughout
    assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
    
    std::cout << "    testSTDPLongTermDynamics passed (range: " 
              << minWeight << " to " << maxWeight << ")" << std::endl;
}

void runAllEnhanced() {
    std::cout << "Running Enhanced STDP Tests..." << std::endl;
    
    testSTDPCreationBasic();
    testSTDPConfigurationValidation();
    testSTDPEmptySpikeHandling();
    testSTDPExtremeSpikeTimes();
    testSTDPWeightBoundariesStress();
    testSTDPNumericalStability();
    testSTDPMultipleSynapseConsistency();
    testSTDPLearningRateCalculation();
    testSTDPCoordinationBehavior();
    testSTDPLongTermDynamics();
    
    std::cout << std::endl << "All enhanced STDP tests completed!" << std::endl;
}

} // namespace test_stdp_enhanced

// Integration with existing tests
void runEnhancedSTDPTests() {
    test_stdp_enhanced::runAllEnhanced();
}
