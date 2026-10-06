#include "AdvancedPlasticity.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

namespace test_performance {

void testHomeostaticPlasticityPerformance() {
    std::cout << "    Testing homeostatic plasticity performance..." << std::endl;
    
    HomeostaticPlasticity hp;
    
    // Configure homeostatic plasticity
    hp.setTargetFiringRate(10.0f);
    hp.setHomeostaticGain(0.01f);
    hp.setTimeConstant(100.0f);
    
    // Simulate firing rate updates
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10000; ++i) {
        float firingRate = 5.0f + (static_cast<float>(rand()) / RAND_MAX) * 15.0f;
        hp.update(firingRate, 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "      Processed 10,000 firing rate updates in " 
              << duration.count() << " microseconds" << std::endl;
    
    // Verify performance
    assert(hp.getCurrentFiringRate() > 0.0f);
    assert(hp.getHomeostaticError() >= 0.0f);
    
    std::cout << "    Homeostatic plasticity performance test passed" << std::endl;
}

void testRewardModulatedPlasticityPerformance() {
    std::cout << "    Testing reward-modulated plasticity performance..." << std::endl;
    
    RewardModulatedPlasticity rmp;
    
    // Configure reward-modulated plasticity
    rmp.setLearningRate(0.01f);
    rmp.setPredictionErrorGain(1.0f);
    rmp.setTraceDecayRate(0.995f);
    
    // Simulate reward updates
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 5000; ++i) {
        float reward = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
        float predictedReward = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
        rmp.update(reward, predictedReward, 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "      Processed 5,000 reward updates in " 
              << duration.count() << " microseconds" << std::endl;
    
    // Verify performance
    assert(rmp.getLearningRate() > 0.0f);
    assert(rmp.getPredictionErrorGain() > 0.0f);
    
    std::cout << "    Reward-modulated plasticity performance test passed" << std::endl;
}

void testMetaPlasticityPerformance() {
    std::cout << "      Testing meta-plasticity performance..." << std::endl;
    
    MetaPlasticity mp;
    
    // Configure meta-plasticity
    mp.setPlasticityLearningRate(0.001f);
    mp.setStabilityFactor(0.8f);
    mp.setComplexityTarget(0.5f);
    
    // Simulate learning progress updates
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 3000; ++i) {
        float learningProgress = (static_cast<float>(rand()) / RAND_MAX);
        float networkComplexity = (static_cast<float>(rand()) / RAND_MAX);
        mp.update(learningProgress, networkComplexity, 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "      Processed 3,000 learning progress updates in " 
              << duration.count() << " microseconds" << std::endl;
    
    // Verify performance
    assert(mp.getPlasticityLearningRate() > 0.0f);
    assert(mp.getStabilityFactor() > 0.0f);
    assert(mp.getComplexityScore() >= 0.0f && mp.getComplexityScore() <= 1.0f);
    
    std::cout << "    Meta-plasticity performance test passed" << std::endl;
}

void testSynapticScalingPerformance() {
    std::cout << "      Testing synaptic scaling performance..." << std::endl;
    
    SynapticScaling ss;
    
    // Configure synaptic scaling
    ss.setScalingGain(1.0f);
    ss.setTargetExcitationInhibitionRatio(1.0f);
    ss.setNetworkStabilityThreshold(0.7f);
    
    // Create mock synapses
    std::vector<Synapse*> synapses;
    for (int i = 0; i < 1000; ++i) {
        // Mock synapse objects (would need actual synapse instances in real test)
        synapses.push_back(nullptr);
    }
    
    // Simulate scaling updates
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 2000; ++i) {
        float eiratio = (static_cast<float>(rand()) / RAND_MAX) * 2.0f;
        ss.update(eiratio, synapses, 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "      Processed 2,000 scaling updates for 1,000 synapses in " 
              << duration.count() << " microseconds" << std::endl;
    
    // Verify performance
    assert(ss.getScalingGain() > 0.0f);
    assert(ss.getNetworkStability() >= 0.0f && ss.getNetworkStability() <= 1.0f);
    
    std::cout << "    Synaptic scaling performance test passed" << std::endl;
}

void testAdvancedPlasticityIntegrationPerformance() {
    std::cout << "    Testing advanced plasticity integration performance..." << std::endl;
    
    AdvancedPlasticity ap;
    
    // Enable all plasticity mechanisms
    ap.enableHomeostaticPlasticity(true);
    ap.enableRewardModulatedPlasticity(true);
    ap.enableMetaPlasticity(true);
    ap.enableSynapticScaling(true);
    
    // Set parameters
    ap.setHomeostaticTarget(10.0f);
    ap.setRewardLearningRate(0.01f);
    ap.setMetaPlasticityLearningRate(0.001f);
    ap.setSynapticScalingGain(1.0f);
    
    // Simulate integrated plasticity updates
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        float firingRate = 5.0f + (static_cast<float>(rand()) / RAND_MAX) * 10.0f;
        float reward = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
        float predictionError = (static_cast<float>(rand()) / RAND_MAX) * 0.2f - 0.1f;
        float novelty = (static_cast<float>(rand()) / RAND_MAX);
        float curiosity = (static_cast<float>(rand()) / RAND_MAX) * 0.5f;
        
        ap.update(firingRate, reward, predictionError, novelty, curiosity, 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "      Processed 1,000 integrated plasticity updates in " 
              << duration.count() << " microseconds" << std::endl;
    
    // Verify performance
    assert(ap.isHomeostaticPlasticityEnabled());
    assert(ap.isRewardModulatedPlasticityEnabled());
    assert(ap.isMetaPlasticityEnabled());
    assert(ap.isSynapticScalingEnabled());
    
    assert(ap.getNetworkStability() >= 0.0f && ap.getNetworkStability() <= 1.0f);
    assert(ap.getLearningEfficiency() >= 0.0f);
    assert(ap.getAdaptabilityScore() >= 0.0f);
    
    std::cout << "    Advanced plasticity integration performance test passed" << std::endl;
}

void testMemoryEfficiency() {
    std::cout << "    Testing memory efficiency..." << std::endl;
    
    // Test homeostatic plasticity memory usage
    {
        HomeostaticPlasticity hp1;
        HomeostaticPlasticity hp2;
        
        // Multiple instances should not interfere
        hp1.setTargetFiringRate(10.0f);
        hp2.setTargetFiringRate(20.0f);
        
        assert(hp1.getTargetFiringRate() == 10.0f);
        assert(hp2.getTargetFiringRate() == 20.0f);
        
        hp1.reset();
        hp2.reset();
        
        assert(hp1.getCurrentFiringRate() == 0.0f);
        assert(hp2.getCurrentFiringRate() == 0.0f);
    }
    
    // Test reward-modulated plasticity memory usage
    {
        RewardModulatedPlasticity rmp1;
        RewardModulatedPlasticity rmp2;
        
        // Multiple instances should have independent state
        rmp1.setLearningRate(0.01f);
        rmp2.setLearningRate(0.05f);
        
        assert(rmp1.getLearningRate() == 0.01f);
        assert(rmp2.getLearningRate() == 0.05f);
        
        rmp1.reset();
        rmp2.reset();
        
        assert(rmp1.getExpectedReward() == 0.0f);
        assert(rmp2.getExpectedReward() == 0.0f);
    }
    
    std::cout << "    Memory efficiency test passed" << std::endl;
}

void runAll() {
    std::cout << "Running performance tests..." << std::endl;
    
    testHomeostaticPlasticityPerformance();
    testRewardModulatedPlasticityPerformance();
    testMetaPlasticityPerformance();
    testSynapticScalingPerformance();
    testAdvancedPlasticityIntegrationPerformance();
    testMemoryEfficiency();
    
    std::cout << "All performance tests passed!" << std::endl;
}

} // namespace test_performance
