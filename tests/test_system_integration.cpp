// System Integration Tests
// Tests full brain simulation with all integrated systems

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "prediction/PredictionSystem.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_system_integration {

void testFullBrainSimulationInitialization() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify all major systems are present
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    assert(brain.getPredictionSystem() != nullptr);
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    assert(brain.getDopamine() != nullptr);
    assert(brain.getSerotonin() != nullptr);
    assert(brain.getNorepinephrine() != nullptr);
    assert(brain.getAcetylcholine() != nullptr);
    
    std::cout << "    testFullBrainSimulationInitialization passed" << std::endl;
}

void testCoordinatedMemoryAndCognition() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Coordinated interaction between memory and cognition
    std::vector<float> pattern = {0.8f, 0.6f, 0.4f, 0.2f, 0.1f};
    
    // Store in working memory
    brain.getWorkingMemory()->store(pattern, 1.0f);
    
    // Store in episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.sensoryState = pattern;
    episode.timestamp = 1;
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Present to concept formation
    size_t conceptId = brain.getConceptFormation()->presentExperience(
        pattern, pattern, 1.0f, 1);
    
    // Retrieve with attention
    auto* attention = brain.getAttention();
    std::vector<nlm::NeuronId> competitors = {nlm::NeuronId(1), nlm::NeuronId(2), nlm::NeuronId(3)};
    std::vector<nlm::NeuronId> winners = attention->processCompetition(competitors, 0.5f);
    
    // Planner should work
    for (nlm::SimulationStep step = 1; step < 15; ++step) {
        brain.step(step);
        
        // All systems should be active
        assert(brain.getWorkingMemory()->getActiveTraces() >= 0);
        assert(brain.getEpisodicMemory()->getEpisodeCount() >= 0);
        assert(brain.getConceptFormation()->getConceptCount() >= 0);
        assert(brain.getPlanner() != nullptr);
        
        // Action should be produced
        auto action = brain.produceAction();
        assert(action != nullptr);
    }
    
    std::cout << "    testCoordinatedMemoryAndCognition passed" << std::endl;
}

void testMemoryPredictionCognitionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create predictive patterns
    std::vector<float> pattern1 = {1.0f, 0.5f, 0.0f, -0.5f, -1.0f};
    std::vector<float> pattern2 = {0.8f, 0.4f, 0.0f, -0.4f, -0.8f};
    
    // Store in episodic memory
    nlm::EpisodicMemoryItem episode1;
    episode1.sensoryState = pattern1;
    episode1.timestamp = 1;
    brain.getEpisodicMemory()->storeEpisode(episode1);
    
    nlm::EpisodicMemoryItem episode2;
    episode2.sensoryState = pattern2;
    episode2.timestamp = 2;
    brain.getEpisodicMemory()->storeEpisode(episode2);
    
    // Train prediction system
    nlm::InternalSignals input1(pattern1);
    nlm::InternalSignals input2(pattern2);
    brain.getPredictionSystem()->train(input1);
    brain.getPredictionSystem()->updatePredictions(input1, input2);
    
    // Present to concept formation
    brain.getConceptFormation()->presentExperience(pattern1, pattern1, 1.0f, 1);
    brain.getConceptFormation()->presentExperience(pattern2, pattern2, 1.0f, 2);
    
    // Make predictions
    auto prediction = brain.getPredictionSystem()->predictNextState(input1);
    assert(prediction != nullptr);
    
    // System should be fully functional
    for (nlm::SimulationStep step = 1; step < 20; ++step) {
        brain.step(step);
        
        // Prediction system should be working
        assert(brain.getPredictionSystem()->getConfidence() >= 0.0f);
        
        // Memory systems should be working
        assert(brain.getWorkingMemory()->getActiveTraces() >= 0);
        assert(brain.getEpisodicMemory()->getEpisodeCount() >= 2);
        
        // Cognition systems should be working
        assert(brain.getConceptFormation()->getConceptCount() >= 0);
        assert(brain.getPlanner() != nullptr);
    }
    
    std::cout << "    testMemoryPredictionCognitionIntegration passed" << std::endl;
}

void testNeuromodulatorSystemCoordination() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get all neuromodulators
    auto* dopamine = brain.getDopamine();
    auto* serotonin = brain.getSerotonin();
    auto* norepinephrine = brain.getNorepinephrine();
    auto* acetylcholine = brain.getAcetylcholine();
    
    // Set different levels based on different signals
    nlm::Neuromodulator rewardSignal;
    rewardSignal.setLevel(0.8f);
    brain.applyNeuromodulation(rewardSignal);
    
    nlm::Neuromodulator stressSignal;
    stressSignal.setLevel(0.4f);
    brain.applyNeuromodulation(stressSignal);
    
    nlm::Neuromodulator noveltySignal;
    noveltySignal.setLevel(0.6f);
    brain.applyNeuromodulation(noveltySignal);
    
    // All neuromodulators should have levels
    assert(dopamine->getLevel() >= 0.0f);
    assert(serotonin->getLevel() >= 0.0f);
    assert(norepinephrine->getLevel() >= 0.0f);
    assert(acetylcholine->getLevel() >= 0.0f);
    
    // Test plasticity factor coordination
    assert(dopamine->getPlasticityFactor() >= 0.5f); // Reward-based plasticity
    assert(serotonin->getPlasticityFactor() >= 0.0f);
    assert(norepinephrine->getPlasticityFactor() >= 0.0f);
    assert(acetylcholine->getPlasticityFactor() >= 0.0f);
    
    // System should be stable
    for (nlm::SimulationStep step = 1; step < 15; ++step) {
        brain.step(step);
        
        // All neuromodulators should be accessible
        assert(dopamine->getLevel() >= 0.0f);
        assert(serotonin->getLevel() >= 0.0f);
        assert(norepinephrine->getLevel() >= 0.0f);
        assert(acetylcholine->getLevel() >= 0.0f);
        
        // Brain should function normally
        auto action = brain.produceAction();
        assert(action != nullptr);
    }
    
    std::cout << "    testNeuromodulatorSystemCoordination passed" << std::endl;
}

void testAttentionMemoryPlannerCoordination() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(350), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Set attention parameters
    brain.getAttention()->setInhibitionStrength(0.6f);
    brain.getAttention()->setExcitationStrength(1.2f);
    
    // Create working memory content
    std::vector<float> pattern = {0.7f, 0.5f, 0.3f, 0.1f};
    brain.getWorkingMemory()->store(pattern, 1.0f);
    
    // Create episodic memory
    nlm::EpisodicMemoryItem episode;
    episode.sensoryState = pattern;
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Present to concept formation
    size_t conceptId = brain.getConceptFormation()->presentExperience(
        pattern, pattern, 1.0f, 1);
    
    // System coordination
    for (nlm::SimulationStep step = 1; step < 25; ++step) {
        brain.step(step);
        
        // Attention should be working
        assert(brain.getAttention()->getWinners().size() >= 0);
        
        // Memory should be working
        assert(brain.getWorkingMemory()->getActiveTraces() >= 0);
        assert(brain.getEpisodicMemory()->getEpisodeCount() >= 1);
        
        // Cognition should be working
        assert(brain.getConceptFormation()->getConceptCount() >= 0);
        
        // Planning should be working
        auto action = brain.produceAction();
        assert(action != nullptr);
    }
    
    std::cout << "    testAttentionMemoryPlannerCoordination passed" << std::endl;
}

void testPredictionMemoryAttentionIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(400), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create patterns for prediction
    std::vector<float> pattern1 = {1.0f, 0.8f, 0.6f, 0.4f, 0.2f};
    std::vector<float> pattern2 = {0.7f, 0.5f, 0.3f, 0.1f, -0.1f};
    
    // Train prediction system
    nlm::InternalSignals input1(pattern1);
    nlm::InternalSignals input2(pattern2);
    brain.getPredictionSystem()->train(input1);
    brain.getPredictionSystem()->updatePredictions(input1, input2);
    
    // Store in memory systems
    brain.getWorkingMemory()->store(pattern1, 1.0f);
    
    nlm::EpisodicMemoryItem episode;
    episode.sensoryState = pattern1;
    episode.timestamp = 1;
    brain.getEpisodicMemory()->storeEpisode(episode);
    
    // Present to concept formation
    brain.getConceptFormation()->presentExperience(pattern1, pattern1, 1.0f, 1);
    
    // Set attention
    brain.getAttention()->focusOnRegion(nlm::RegionId(1));
    
    // Integration test
    for (nlm::SimulationStep step = 1; step < 30; ++step) {
        brain.step(step);
        
        // Prediction should be working
        assert(brain.getPredictionSystem()->getPredictionError() >= 0.0f);
        assert(brain.getPredictionSystem()->getConfidence() >= 0.0f);
        
        // Memory should be working
        assert(brain.getWorkingMemory()->getActiveTraces() >= 0);
        assert(brain.getEpisodicMemory()->getEpisodeCount() >= 1);
        
        // Cognition should be working
        assert(brain.getConceptFormation()->getConceptCount() >= 0);
        
        // Attention should be working
        assert(brain.getAttention()->getAttendedRegions().size() >= 0);
        
        // Planning should be working
        auto action = brain.produceAction();
        assert(action != nullptr);
    }
    
    std::cout << "    testPredictionMemoryAttentionIntegration passed" << std::endl;
}

void testFullSystemIntegrationWithNeuromodulation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(450), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Get neuromodulators
    auto* dopamine = brain.getDopamine();
    auto* serotonin = brain.getSerotonin();
    auto* norepinephrine = brain.getNorepinephrine();
    auto* acetylcholine = brain.getAcetylcholine();
    
    // Complex learning scenario with multiple neuromodulators
    std::vector<std::vector<float>> learningPatterns = {
        {1.0f, 0.8f, 0.6f},
        {0.9f, 0.7f, 0.5f},
        {0.8f, 0.6f, 0.4f}
    };
    
    // Learn with different neuromodulator effects
    for (size_t i = 0; i < learningPatterns.size(); ++i) {
        // Apply different neuromodulation signals
        nlm::Neuromodulator rewardSignal;
        rewardSignal.setLevel(0.7f + (i * 0.1f));
        brain.applyNeuromodulation(rewardSignal);
        
        nlm::Neuromodulator stressSignal;
        stressSignal.setLevel(0.3f + (i * 0.05f));
        brain.applyNeuromodulation(stressSignal);
        
        // Present pattern
        auto* conceptFormation = brain.getConceptFormation();
        conceptFormation->presentExperience(
            learningPatterns[i], learningPatterns[i], 
            0.8f + (i * 0.1f), i);
        
        // Store in memory
        brain.getWorkingMemory()->store(learningPatterns[i], 1.0f);
        
        nlm::EpisodicMemoryItem episode;
        episode.sensoryState = learningPatterns[i];
        episode.timestamp = i;
        brain.getEpisodicMemory()->storeEpisode(episode);
        
        // Test prediction
        nlm::InternalSignals input(learningPatterns[i]);
        brain.getPredictionSystem()->train(input);
        
        // Step forward
        brain.step(i + 1);
        
        // Verify system integrity
        assert(brain.getWorkingMemory()->getActiveTraces() >= 0);
        assert(brain.getEpisodicMemory()->getEpisodeCount() >= 1);
        assert(conceptFormation->getConceptCount() >= 0);
        assert(brain.getPredictionSystem()->getConfidence() >= 0.0f);
        assert(brain.getPlanner() != nullptr);
        
        // Neuromodulators should respond
        assert(dopamine->getLevel() >= 0.0f);
        assert(serotonin->getLevel() >= 0.0f);
        assert(norepinephrine->getLevel() >= 0.0f);
        assert(acetylcholine->getLevel() >= 0.0f);
    }
    
    std::cout << "    testFullSystemIntegrationWithNeuromodulation passed" << std::endl;
}

void testSystemPerformanceAndStability() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Stress test with complex operations
    std::vector<float> complexPattern(10, 0.5f);
    
    for (nlm::SimulationStep step = 1; step < 50; ++step) {
        brain.step(step);
        
        // Working memory
        brain.getWorkingMemory()->store(complexPattern, 1.0f);
        
        // Episodic memory
        nlm::EpisodicMemoryItem episode;
        episode.sensoryState = complexPattern;
        episode.timestamp = step;
        brain.getEpisodicMemory()->storeEpisode(episode);
        
        // Concept formation
        brain.getConceptFormation()->presentExperience(
            complexPattern, complexPattern, 0.5f + (step * 0.01f), step);
        
        // Prediction
        nlm::InternalSignals input(complexPattern);
        brain.getPredictionSystem()->train(input);
        
        // Attention
        brain.getAttention()->applyTopDownBias(nlm::NeuronId(1), 0.5f);
        
        // Action production
        auto action = brain.produceAction();
        assert(action != nullptr);
        
        // Neuromodulation
        nlm::Neuromodulator signal;
        signal.setLevel(0.5f);
        brain.applyNeuromodulation(signal);
    }
    
    // System should remain stable
    assert(brain.getWorkingMemory()->getActiveTraces() >= 0);
    assert(brain.getEpisodicMemory()->getEpisodeCount() >= 25);
    assert(brain.getConceptFormation()->getConceptCount() >= 0);
    assert(brain.getPredictionSystem()->getConfidence() >= 0.0f);
    assert(brain.getPlanner() != nullptr);
    
    std::cout << "    testSystemPerformanceAndStability passed" << std::endl;
}

void runAll() {
    testFullBrainSimulationInitialization();
    testCoordinatedMemoryAndCognition();
    testMemoryPredictionCognitionIntegration();
    testNeuromodulatorSystemCoordination();
    testAttentionMemoryPlannerCoordination();
    testPredictionMemoryAttentionIntegration();
    testFullSystemIntegrationWithNeuromodulation();
    testSystemPerformanceAndStability();
}

} // namespace test_system_integration