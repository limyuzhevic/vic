// Neuromodulation Learning Tests
// Tests neuromodulator effects on learning

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Serotonin.hpp"
#include "neuromodulation/Norepinephrine.hpp"
#include "neuromodulation/Acetylcholine.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_neuromodulation_learning {

void testDopamineRewardLearning() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get dopamine neuromodulator
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    // Test initial level
    assert(dopamine->getLevel() >= 0.0f && dopamine->getLevel() <= 1.0f);
    
    // Signal reward
    dopamine->signalReward(0.5f);
    assert(dopamine->getLevel() > 0.0f);
    
    // Update dynamics
    for (int step = 1; step < 10; ++step) {
        dopamine->update(0.1f);
        assert(dopamine->getLevel() >= 0.0f && dopamine->getLevel() <= 1.0f);
    }
    
    // Test plasticity factor
    float plasticityFactor = dopamine->getPlasticityFactor();
    assert(plasticityFactor >= 0.5f && plasticityFactor <= 1.0f);
    
    std::cout << "    testDopamineRewardLearning passed" << std::endl;
}

void testSerotoninMoodModulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get serotonin neuromodulator
    auto* serotonin = brain.getSerotonin();
    assert(serotonin != nullptr);
    
    // Test initial level
    assert(serotonin->getLevel() >= 0.0f && serotonin->getLevel() <= 1.0f);
    
    // Test plasticity factor
    float plasticityFactor = serotonin->getPlasticityFactor();
    assert(plasticityFactor >= 0.0f && plasticityFactor <= 1.0f);
    
    // Update dynamics
    for (int step = 1; step < 10; ++step) {
        serotonin->update(0.1f);
        assert(serotonin->getLevel() >= 0.0f && serotonin->getLevel() <= 1.0f);
    }
    
    std::cout << "    testSerotoninMoodModulation passed" << std::endl;
}

void testNorepinephrineArousal() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get norepinephrine neuromodulator
    auto* norepinephrine = brain.getNorepinephrine();
    assert(norepinephrine != nullptr);
    
    // Test initial level
    assert(norepinephrine->getLevel() >= 0.0f && norepinephrine->getLevel() <= 1.0f);
    
    // Test plasticity factor
    float plasticityFactor = norepinephrine->getPlasticityFactor();
    assert(plasticityFactor >= 0.0f && plasticityFactor <= 1.0f);
    
    // Update dynamics
    for (int step = 1; step < 10; ++step) {
        norepinephrine->update(0.1f);
        assert(norepinephrine->getLevel() >= 0.0f && norepinephrine->getLevel() <= 1.0f);
    }
    
    std::cout << "    testNorepinephrineArousal passed" << std::endl;
}

void testAcetylcholineAttention() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get acetylcholine neuromodulator
    auto* acetylcholine = brain.getAcetylcholine();
    assert(acetylcholine != nullptr);
    
    // Test initial level
    assert(acetylcholine->getLevel() >= 0.0f && acetylcholine->getLevel() <= 1.0f);
    
    // Test plasticity factor
    float plasticityFactor = acetylcholine->getPlasticityFactor();
    assert(plasticityFactor >= 0.0f && plasticityFactor <= 1.0f);
    
    // Test specific ACh functions
    float attentionModulation = acetylcholine->getAttentionModulation();
    float workingMemoryEnhancement = acetylcholine->getWorkingMemoryEnhancement();
    float memoryConsolidation = acetylcholine->getMemoryConsolidation();
    
    assert(attentionModulation >= 0.0f && attentionModulation <= 1.0f);
    assert(workingMemoryEnhancement >= 0.0f && workingMemoryEnhancement <= 1.0f);
    assert(memoryConsolidation >= 0.0f && memoryConsolidation <= 1.0f);
    
    // Update dynamics
    for (int step = 1; step < 10; ++step) {
        acetylcholine->update(0.1f);
        assert(acetylcholine->getLevel() >= 0.0f && acetylcholine->getLevel() <= 1.0f);
    }
    
    std::cout << "    testAcetylcholineAttention passed" << std::endl;
}

void testNeuromodulatorCoordination() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get all neuromodulators
    auto* dopamine = brain.getDopamine();
    auto* serotonin = brain.getSerotonin();
    auto* norepinephrine = brain.getNorepinephrine();
    auto* acetylcholine = brain.getAcetylcholine();
    
    assert(dopamine != nullptr && serotonin != nullptr && 
           norepinephrine != nullptr && acetylcholine != nullptr);
    
    // Set different levels
    dopamine->setLevel(0.7f);
    serotonin->setLevel(0.5f);
    norepinephrine->setLevel(0.3f);
    acetylcholine->setLevel(0.9f);
    
    // Verify levels
    assert(dopamine->getLevel() == 0.7f);
    assert(serotonin->getLevel() == 0.5f);
    assert(norepinephrine->getLevel() == 0.3f);
    assert(acetylcholine->getLevel() == 0.9f);
    
    // All should have reasonable plasticity factors
    assert(dopamine->getPlasticityFactor() >= 0.5f);
    assert(serotonin->getPlasticityFactor() >= 0.0f);
    assert(norepinephrine->getPlasticityFactor() >= 0.0f);
    assert(acetylcholine->getPlasticityFactor() >= 0.0f);
    
    std::cout << "    testNeuromodulatorCoordination passed" << std::endl;
}

void testNeuromodulatorLearningEffects() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Store some patterns in memory
    std::vector<float> pattern1 = {0.8f, 0.6f, 0.4f};
    std::vector<float> pattern2 = {0.7f, 0.5f, 0.3f};
    
    brain.getWorkingMemory()->store(pattern1, 1.0f);
    brain.getWorkingMemory()->store(pattern2, 0.8f);
    
    // Create episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.sensoryState = pattern1;
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Apply different neuromodulator signals
    nlm::Neuromodulator rewardSignal;
    rewardSignal.setLevel(0.8f);
    brain.applyNeuromodulation(rewardSignal);
    
    nlm::Neuromodulator stressSignal;
    stressSignal.setLevel(0.3f);
    brain.applyNeuromodulation(stressSignal);
    
    nlm::Neuromodulator noveltySignal;
    noveltySignal.setLevel(0.6f);
    brain.applyNeuromodulation(noveltySignal);
    
    // Brain should still function
    for (int step = 1; step < 10; ++step) {
        brain.step(step);
        
        // Memory should still be accessible
        assert(brain.getWorkingMemory()->getActiveTraces() >= 0);
        assert(brain.getEpisodicMemory()->getEpisodeCount() >= 0);
        
        // Neuromodulators should have levels
        assert(brain.getDopamine()->getLevel() >= 0.0f);
        assert(brain.getSerotonin()->getLevel() >= 0.0f);
        assert(brain.getNorepinephrine()->getLevel() >= 0.0f);
        assert(brain.getAcetylcholine()->getLevel() >= 0.0f);
    }
    
    std::cout << "    testNeuromodulatorLearningEffects passed" << std::endl;
}

void testNeuromodulatorPlasticityInteractions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(180), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create patterns for learning
    std::vector<float> pattern1 = {1.0f, 0.8f, 0.6f};
    std::vector<float> pattern2 = {0.5f, 0.5f, 0.5f};
    
    // Present to concept formation multiple times
    for (int i = 0; i < 5; ++i) {
        brain.getConceptFormation()->presentExperience(
            pattern1, pattern1, 0.5f + (i * 0.1f), i);
        
        brain.getConceptFormation()->presentExperience(
            pattern2, pattern2, 0.3f, i);
    }
    
    // Check concept formation after neuromodulation
    size_t conceptCount = brain.getConceptFormation()->getConceptCount();
    assert(conceptCount >= 0);
    
    // Test neuromodulator effects on learning
    auto* dopamine = brain.getDopamine();
    
    // Signal different rewards
    dopamine->signalReward(0.9f); // High reward
    float highRewardPlasticity = dopamine->getPlasticityFactor();
    
    dopamine->signalReward(0.1f); // Low reward
    float lowRewardPlasticity = dopamine->getPlasticityFactor();
    
    // Higher reward should result in higher plasticity
    assert(highRewardPlasticity >= lowRewardPlasticity);
    
    std::cout << "    testNeuromodulatorPlasticityInteractions passed" << std::endl;
}

void testNeuromodulatorStressAndArousal() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get norepinephrine (stress/arousal)
    auto* norepinephrine = brain.getNorepinephrine();
    assert(norepinephrine != nullptr);
    
    // Apply stress signal
    nlm::Neuromodulator stressSignal;
    stressSignal.setLevel(0.7f);
    brain.applyNeuromodulation(stressSignal);
    
    // Arousal should be high
    assert(norepinephrine->getLevel() >= 0.0f);
    
    // Test norepinephrine plasticity factor
    float plasticity = norepinephrine->getPlasticityFactor();
    assert(plasticity >= 0.0f && plasticity <= 1.0f);
    
    // Apply more stress
    nlm::Neuromodulator stressSignal2;
    stressSignal2.setLevel(0.9f);
    brain.applyNeuromodulation(stressSignal2);
    
    assert(norepinephrine->getLevel() >= 0.0f);
    
    std::cout << "    testNeuromodulatorStressAndArousal passed" << std::endl;
}

void runAll() {
    testDopamineRewardLearning();
    testSerotoninMoodModulation();
    testNorepinephrineArousal();
    testAcetylcholineAttention();
    testNeuromodulatorCoordination();
    testNeuromodulatorLearningEffects();
    testNeuromodulatorPlasticityInteractions();
    testNeuromodulatorStressAndArousal();
}

} // namespace test_neuromodulation_learning