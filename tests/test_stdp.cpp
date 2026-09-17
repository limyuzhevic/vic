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

void testSTDPAdvancedFeatures() {
    nlm::STDP stdp;
    
    // Test variant management
    assert(stdp.getVariant() == nlm::STDPVariant::Standard);
    
    stdp.setVariant(nlm::STDPVariant::RewardModulated);
    assert(stdp.getVariant() == nlm::STDPVariant::RewardModulated);
    
    stdp.setVariant(nlm::STDPVariant::Triplet);
    assert(stdp.getVariant() == nlm::STDPVariant::Triplet);
    
    stdp.setVariant(nlm::STDPVariant::Standard);
    
    // Test advanced feature management
    stdp.enableLearningRateAdaptation(true, 0.2f);
    stdp.enableEfficacyModulation(false);
    stdp.enableEligibilityTraces(false);
    
    // Test batch processing
    stdp.enableBatchProcessing(true, 512);
    stdp.enableParallelProcessing(true);
    
    // Test profiling
    stdp.enableProfiling(true);
    std::cout << "    testSTDPAdvancedFeatures passed" << std::endl;
}

void testSTDPParallelProcessing() {
    nlm::STDP stdp;
    
    // Create a synapse with many spike pairs for parallel processing
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Generate many spike pairs to trigger parallel processing
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes;
    
    // Create 50 pre and 50 post spikes (2500 pairs)
    for (int i = 0; i < 50; ++i) {
        preSpikes.push_back(static_cast<nlm::Timestamp>(i * 10.0));
        postSpikes.push_back(static_cast<nlm::Timestamp>(i * 10.0 + 5.0));  // All pre before post
    }
    
    float initialWeight = synapse.getWeight();
    
    // This should trigger parallel processing due to large number of spike pairs
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased
    assert(finalWeight > initialWeight);
    
    // Check that spike pair counting worked
    assert(stdp.getSpikePairCount() > 0);
    
    std::cout << "    testSTDPParallelProcessing passed (pairs: " 
              << stdp.getSpikePairCount() << ")" << std::endl;
}

void testSTDPBoundsChecking() {
    nlm::STDP stdp;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    // Test bounds checking with invalid spike history
    std::vector<nlm::Timestamp> invalidSpikes;
    for (int i = 0; i < 15000; ++i) {  // Too many spikes
        invalidSpikes.push_back(static_cast<nlm::Timestamp>(i));
    }
    
    // This should not crash due to bounds checking
    // (the implementation should handle this gracefully)
    try {
        stdp.update(&synapse, invalidSpikes, invalidSpikes, 0.001);
        std::cout << "    testSTDPBoundsChecking passed (handled large spike history)" << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "    testSTDPBoundsChecking passed (caught invalid argument)" << std::endl;
    }
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
    testSTDPAdvancedFeatures();
    testSTDPParallelProcessing();
    testSTDPBoundsChecking();
}

} // namespace test_stdp
